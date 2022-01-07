#pragma once

#include "rs-graphics-2d/image.hpp"
#include "rs-graphics-core/colour.hpp"
#include "rs-graphics-core/geometry.hpp"
#include "rs-graphics-core/maths.hpp"
#include "rs-graphics-core/vector.hpp"
#include "rs-format/string.hpp"
#include "rs-io/path.hpp"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace RS::Graphics::Plane {

    namespace FontStyle {

        constexpr int regular   = 0;
        constexpr int bold      = 1;
        constexpr int italic    = 2;
        constexpr int fallback  = 4;

    }

    class Font {

    public:

        Font() = default;
        explicit Font(const IO::Path& file, int index = 0);
        virtual ~Font() noexcept {}

        explicit operator bool() const noexcept { return bool(font_); }

        std::string family() const;
        std::string subfamily() const;
        std::string name() const;
        bool has_glyph(char32_t c) const noexcept;
        bool has_glyphs(char32_t first, char32_t last) const noexcept;
        template <typename Range> bool has_glyphs(const Range& range) const;

        static std::vector<Font> load(const IO::Path& file);

    protected:

        struct font_impl;

        std::shared_ptr<std::string> content_;
        std::shared_ptr<font_impl> font_;

    private:

        bool has_glyph_unchecked(char32_t c) const noexcept;

    };

        template <typename Range>
        bool Font::has_glyphs(const Range& range) const {
            if (! font_)
                return false;
            for (auto c: range)
                if (! has_glyph_unchecked(c))
                    return false;
            return true;
        }

    class ScaledFont:
    public Font {

    public:

        ScaledFont() = default;
        ScaledFont(const Font& font, int scale) noexcept: ScaledFont(font, {scale, scale}) {}
        ScaledFont(const Font& font, Point scale) noexcept;

        Point scale() const noexcept;
        int ascent() const noexcept;
        int descent() const noexcept;
        int line_gap() const noexcept;
        int line_offset() const noexcept { return ascent() - descent() + line_gap(); }
        template <typename C, int F> void render(Image<C, F>& image, Point& offset, const std::string& text,
            int line_shift = 0, C text_colour = C::black(), C background = C::clear()) const;
        template <typename C, int F> void render_to(Image<C, F>& image, Point ref_point, const std::string& text,
            int line_shift = 0, C text_colour = C::black()) const;
        Core::Box_i2 text_box(const std::string& text, int line_shift = 0) const;
        size_t text_fit(const std::string& text, size_t max_pixels) const;
        size_t text_wrap(const std::string& text_in, std::string& text_out, size_t max_pixels) const;

    private:

        static constexpr float byte_scale = 1.0f / 255.0f;

        class bitmap_ref;
        struct scaled_impl;

        std::shared_ptr<scaled_impl> scaled_;

        bitmap_ref render_glyph_bitmap(char32_t c, Point& offset) const;
        bitmap_ref render_text_bitmap(const std::u32string& utext, int line_shift, Point& offset) const;
        int scale_x(int x) const noexcept;
        int scale_y(int y) const noexcept;
        Core::Box_i2 scale_box(Core::Box_i2 box) const noexcept;

    };

        class ScaledFont::bitmap_ref {
        public:
            bitmap_ref() = default;
            explicit bitmap_ref(Point shape, unsigned char* ptr = nullptr) noexcept;
            unsigned char* begin() noexcept { return ptr_.get(); }
            const unsigned char* begin() const noexcept { return ptr_.get(); }
            unsigned char* end() noexcept { return ptr_.get() + area(); }
            const unsigned char* end() const noexcept { return ptr_.get() + area(); }
            unsigned char& operator[](Point pos) noexcept { return ptr_.get()[size_t(shape_.x()) * size_t(pos.y()) + size_t(pos.x())]; }
            const unsigned char& operator[](Point pos) const noexcept { return ptr_.get()[size_t(shape_.x()) * size_t(pos.y()) + size_t(pos.x())]; }
            size_t area() const noexcept { return size_t(shape_.x()) * size_t(shape_.y()); }
            bool empty() const noexcept { return ! ptr_ || shape_.x() <= 0 || shape_.y() <= 0; }
            Point shape() const noexcept { return shape_; }
        private:
            std::shared_ptr<unsigned char> ptr_;
            Point shape_;
        };

        // TODO - check for premultiplied alpha

        template <typename C, int F>
        void ScaledFont::render(Image<C, F>& image, Point& offset, const std::string& text, int line_shift, C text_colour, C background) const {

            static_assert(C::colour_space::is_linear);
            static_assert(C::has_alpha);

            offset = Point::null();
            if (! font_)
                throw std::invalid_argument("No font");
            if (text.empty())
                return;

            auto utext = Format::decode_string(text);
            auto bitmap = render_text_bitmap(utext, line_shift, offset);
            if (bitmap.empty())
                return;

            image.reset(bitmap.shape(), background);
            auto bptr = bitmap.begin();

            for (auto& pixel: image) {
                text_colour.alpha() = byte_scale * float(*bptr++);
                pixel = alpha_blend(text_colour, background);
            }

        }

        template <typename C, int F>
        void ScaledFont::render_to(Image<C, F>& image, Point ref_point, const std::string& text, int line_shift, C text_colour) const {

            static_assert(C::colour_space::is_linear);
            static_assert(C::has_alpha);

            if (! font_)
                throw std::invalid_argument("No font");
            if (text.empty())
                return;

            auto utext = Format::decode_string(text);
            Point offset;
            auto bitmap = render_text_bitmap(utext, line_shift, offset);
            if (bitmap.empty())
                return;

            // i_ prefix = image coordinates, b_ prefix = bitmap coordinates
            // ic_, bc_ prefix = clipped to image bounds

            Point i_base = ref_point + offset;       // Top left of bitmap
            Point i_apex = i_base + bitmap.shape();  // Bottom right of bitmap
            Point ic_base = maxv(i_base, Point::null());
            Point ic_apex = minv(i_apex, image.shape());

            if (ic_base.x() >= ic_apex.x() || ic_base.y() >= ic_apex.y())
                return;

            Point bc_base = ic_base - i_base;
            int width = ic_apex.x() - ic_base.x();

            for (int iy = ic_base.y(), by = bc_base.y(); iy < ic_apex.y(); ++iy, ++by) {
                auto i_iter = image.locate({ic_base.x(), iy});
                auto b_iter = &bitmap[{bc_base.x(), by}];
                for (int x = 0; x < width; ++x, ++i_iter, ++b_iter) {
                    text_colour.alpha() = byte_scale * float(*b_iter);
                    *i_iter = alpha_blend(text_colour, *i_iter);
                }
            }

        }

    class FontMap {

    public:

        void clear() noexcept { table_.clear(); }
        bool contains(const std::string& family) const noexcept { return table_.count(family) != 0; }
        bool contains(const std::string& family, const std::string& subfamily) const noexcept;
        bool empty() const noexcept { return table_.empty(); }
        std::vector<std::string> families() const;
        std::vector<std::string> subfamilies(const std::string& family) const;
        Font find(const std::vector<std::string>& families, int style = FontStyle::regular) const;
        Font load(const std::string& family, const std::string& subfamily) const;
        void search(const IO::Path& dir, int flags = 0);
        void search_system();
        size_t size() const noexcept { return table_.size(); }

    private:

        struct mapped_type {
            IO::Path file;
            int index;
        };

        using inner_table = std::map<std::string, mapped_type, Format::AsciiIcaseLess>;
        using outer_table = std::map<std::string, inner_table, Format::AsciiIcaseLess>;

        outer_table table_;

    };

}
