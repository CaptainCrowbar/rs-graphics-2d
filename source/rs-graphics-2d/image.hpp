#pragma once

#include "rs-graphics-core/colour.hpp"
#include "rs-graphics-core/colour-space.hpp"
#include "rs-graphics-core/vector.hpp"
#include "rs-format/format.hpp"
#include "rs-format/string.hpp"
#include "rs-io/path.hpp"
#include "rs-tl/binary.hpp"
#include "rs-tl/enum.hpp"
#include "rs-tl/types.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Graphics::Plane {

    using Point = Core::Int2;

    enum class ImageFlags: int {
        none           = 0,
        bottom_up      = 1,
        premultiplied  = 2,
    };

    RS_DEFINE_BITMASK_OPERATORS(ImageFlags)

    enum class ImageResize: int {
        none    = 0,
        unlock  = 1,
        wrap    = 2,
    };

    RS_DEFINE_BITMASK_OPERATORS(ImageResize)

    class ImageIoError:
    public std::runtime_error {
    public:
        ImageIoError(const IO::Path& file, const std::string& details, bool stbi):
            std::runtime_error(make_message(file, details, stbi)), file_(file) {}
        IO::Path file() const noexcept { return file_; }
    private:
        IO::Path file_;
        static std::string make_message(const IO::Path& file, const std::string& details, bool stbi);
    };

    struct ImageInfo {
        Point shape = Point::null();
        int channels = 0;
        int bits_per_channel = 0;
        bool has_alpha = false;
        bool is_hdr = false;
        explicit operator bool() const noexcept { return shape != Point::null(); }
        std::string str() const;
    };

    ImageInfo query_image(const IO::Path& file) noexcept;
    inline std::ostream& operator<<(std::ostream& out, const ImageInfo& info) { return out << info.str(); }

    template <typename Colour, ImageFlags Flags = ImageFlags::none>
    class Image;

    namespace Detail {

        struct StbiFree {
            void operator()(void* ptr) const noexcept { std::free(ptr); }
        };

        template <typename T>
        using StbiPtr = std::unique_ptr<T, StbiFree>;

        StbiPtr<uint8_t> load_image_8(const IO::Path& file, Point& shape);
        StbiPtr<uint16_t> load_image_16(const IO::Path& file, Point& shape);
        StbiPtr<float> load_image_hdr(const IO::Path& file, Point& shape);
        void save_image_8(const Image<Core::Rgba8>& image, const IO::Path& file, const std::string& format, int quality);
        void save_image_hdr(const Image<Core::Rgbaf>& image, const IO::Path& file);
        void resize_image_8(const uint8_t* in, Point ishape, uint8_t* out, Point oshape, int num_channels, int alpha_channel,
            int stb_flags, int stb_edge, int stb_filter, int stb_space);
        void resize_image_16(const uint16_t* in, Point ishape, uint16_t* out, Point oshape, int num_channels, int alpha_channel,
            int stb_flags, int stb_edge, int stb_filter, int stb_space);
        void resize_image_hdr(const float* in, Point ishape, float* out, Point oshape, int num_channels, int alpha_channel,
            int stb_flags, int stb_edge, int stb_filter, int stb_space);

    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    class Image<Core::Colour<T, CS, CL>, Flags> {

    private:

        template <typename CI, typename CC>
        class basic_iterator {

        public:

            using difference_type = int64_t;
            using iterator_category = std::bidirectional_iterator_tag;
            using pointer = CC*;
            using reference = CC&;
            using value_type = T;

            basic_iterator() = default;
            basic_iterator(const basic_iterator<std::remove_const_t<CI>, std::remove_const_t<CC>>& i):
                image_(i.image_), index_(i.index_) {}

            CC& operator*() const noexcept { return image_->pixels_[index_]; }
            CC* operator->() const noexcept { return &**this; }
            basic_iterator& operator++() noexcept { ++index_; return *this; }
            basic_iterator operator++(int) noexcept { auto i = *this; ++*this; return i; }
            basic_iterator& operator--() noexcept { --index_; return *this; }
            basic_iterator operator--(int) noexcept { auto i = *this; --*this; return i; }
            bool operator==(const basic_iterator& i) const noexcept { return index_ == i.index_; }
            bool operator!=(const basic_iterator& i) const noexcept { return ! (*this == i); }

            basic_iterator& move(int axis, int distance = 1) noexcept {
                int64_t d = distance;
                if ((axis & 1) == 1)
                    d *= image_->width();
                index_ += d;
                return *this;
            }

            Point pos() const noexcept {
                int x = int(index_ % image_->width());
                int y = int(index_ / image_->width());
                return {x, y};
            }

        private:

            friend class Image;

            CI* image_;
            CC* data_;
            int64_t index_;

            basic_iterator(CI& image, int64_t index) noexcept:
                image_(&image), index_(index) {}

        };


    public:

        using channel_type = T;
        using colour_space = CS;
        using colour_type = Core::Colour<T, CS, CL>;
        using iterator = basic_iterator<Image, colour_type>;
        using const_iterator = basic_iterator<const Image, const colour_type>;

        static constexpr int channels = colour_type::channels;
        static constexpr Core::ColourLayout colour_layout = CL;
        static constexpr bool has_alpha = colour_type::has_alpha;
        static constexpr bool is_bottom_up = !! (Flags & ImageFlags::bottom_up);
        static constexpr bool is_top_down = ! is_bottom_up;
        static constexpr bool is_hdr = colour_type::is_hdr;
        static constexpr bool is_linear = colour_type::is_linear;
        static constexpr bool is_premultiplied = !! (Flags & ImageFlags::premultiplied);

        static_assert(colour_type::can_premultiply || ! is_premultiplied);

        Image() noexcept: pixels_(), shape_(0, 0) {}
        explicit Image(Point shape) { reset(shape); }
        Image(Point shape, colour_type c) { reset(shape, c); }
        Image(int w, int h) { reset(w, h); }
        Image(int w, int h, colour_type c) { reset(w, h, c); }

        colour_type& operator[](Point p) noexcept { return (*this)(p.x(), p.y()); }
        const colour_type& operator[](Point p) const noexcept { return (*this)(p.x(), p.y()); }
        colour_type& operator()(int x, int y) noexcept { return *locate(x, y); }
        const colour_type& operator()(int x, int y) const noexcept { return *locate(x, y); }

        iterator begin() noexcept { return iterator(*this, 0); }
        const_iterator begin() const noexcept { return const_iterator(*this, 0); }
        iterator end() noexcept { return iterator(*this, int64_t(size())); }
        const_iterator end() const noexcept { return const_iterator(*this, int64_t(size())); }
        T* data() noexcept { return reinterpret_cast<T*>(pixels_.data()); }
        const T* data() const noexcept { return reinterpret_cast<const T*>(pixels_.data()); }

        iterator bottom_left() noexcept { return locate(0, is_top_down ? height() - 1 : 0); }
        const_iterator bottom_left() const noexcept { return locate(0, is_top_down ? height() - 1 : 0); }
        iterator bottom_right() noexcept { return locate(width() - 1, is_top_down ? height() - 1 : 0); }
        const_iterator bottom_right() const noexcept { return locate(width() - 1, is_top_down ? height() - 1 : 0); }
        iterator top_left() noexcept { return locate(0, is_top_down ? 0 : height() - 1); }
        const_iterator top_left() const noexcept { return locate(0, is_top_down ? 0 : height() - 1); }
        iterator top_right() noexcept { return locate(width() - 1, is_top_down ? 0 : height() - 1); }
        const_iterator top_right() const noexcept { return locate(width() - 1, is_top_down ? 0 : height() - 1); }

        void clear() noexcept { pixels_.clear(); shape_ = {}; }
        void fill(colour_type c) noexcept { std::fill(pixels_.begin(), pixels_.end(), c); }

        void load(const IO::Path& file);
        void save(const IO::Path& file, int quality = 90) const;

        iterator locate(Point p) noexcept { return locate(p.x(), p.y()); }
        const_iterator locate(Point p) const noexcept { return locate(p.x(), p.y()); }
        iterator locate(int x, int y) noexcept { return iterator(*this, make_index(x, y)); }
        const_iterator locate(int x, int y) const noexcept { return const_iterator(*this, make_index(x, y)); }

        template <typename U = T>
        Image<colour_type, Flags | ImageFlags::premultiplied>
        multiply_alpha(std::enable_if<TL::SfinaeTrue<U, colour_type::can_premultiply
                && ! is_premultiplied>::value>* = nullptr) const {
            Image<colour_type, Flags | ImageFlags::premultiplied> result(shape());
            auto out = result.begin();
            for (auto& pixel: *this)
                *out++ = pixel.multiply_alpha();
            return result;
        }

        template <typename U = T>
        Image<colour_type, Flags & ~ ImageFlags::premultiplied>
        unmultiply_alpha(std::enable_if<TL::SfinaeTrue<U, colour_type::can_premultiply
                && is_premultiplied>::value>* = nullptr) const {
            Image<colour_type, Flags & ~ ImageFlags::premultiplied> result(shape());
            auto out = result.begin();
            for (auto& pixel: *this)
                *out++ = pixel.unmultiply_alpha();
            return result;
        }

        void reset(Point shape) { reset(shape.x(), shape.y()); }
        void reset(Point shape, colour_type c) { reset(shape.x(), shape.y(), c); }

        void reset(int w, int h) {
            Point p(w, h);
            size_t n = check_size(p);
            pixels_.resize(n);
            shape_ = p;
        }

        void reset(int w, int h, colour_type c) {
            Point p(w, h);
            size_t n = check_size(p);
            pixels_.clear();
            pixels_.resize(n, c);
            shape_ = p;
        }

        void resize(Point new_shape, ImageResize rflags = ImageResize::none);
        void resize(double scale, ImageResize rflags = ImageResize::none);
        Image resized(Point new_shape, ImageResize rflags = ImageResize::none) const;
        Image resized(double scale, ImageResize rflags = ImageResize::none) const;
        Point shape() const noexcept { return shape_; }
        bool empty() const noexcept { return pixels_.empty(); }
        int width() const noexcept { return shape_.x(); }
        int height() const noexcept { return shape_.y(); }
        size_t size() const noexcept { return size_t(width()) * size_t(height()); }
        size_t bytes() const noexcept { return size() * sizeof(colour_type); }

        void swap(Image& img) noexcept { pixels_.swap(img.pixels_); std::swap(shape_, img.shape_); }
        friend void swap(Image& a, Image& b) noexcept { a.swap(b); }

        friend bool operator==(const Image& a, const Image& b) noexcept { return a.shape_ == b.shape_ && a.pixels_ == b.pixels_; }
        friend bool operator!=(const Image& a, const Image& b) noexcept { return ! (a == b); }

    private:

        std::vector<colour_type> pixels_;
        Point shape_;

        int64_t make_index(int x, int y) const noexcept { return int64_t(width()) * y + x; }

        static size_t check_size(Point p) {
            if (p.x() < 0 || p.y() < 0 || (p.x() == 0) != (p.y() == 0))
                throw std::invalid_argument(Format::format("Invalid image dimensions: {0}", p));
            return size_t(p.x()) * size_t(p.y());
        }

    };

    using Image8 = Image<Core::Rgba8>;
    using Image16 = Image<Core::Rgba16>;
    using HdrImage = Image<Core::Rgbaf>;
    using sImage8 = Image<Core::sRgba8>;
    using sImage16 = Image<Core::sRgba16>;
    using sHdrImage = Image<Core::sRgbaf>;
    using PmaImage8 = Image<Core::Rgba8, ImageFlags::premultiplied>;
    using PmaImage16 = Image<Core::Rgba16, ImageFlags::premultiplied>;
    using PmaHdrImage = Image<Core::Rgbaf, ImageFlags::premultiplied>;

    template <typename C1, ImageFlags F1, typename C2, ImageFlags F2>
    void convert_image(const Image<C1, F1>& in, Image<C2, F2>& out) {

        using Img1 = Image<C1, F1>;
        using Img2 = Image<C2, F2>;

        if constexpr (std::is_same_v<Img1, Img2>) {

            out = in;

        } else if constexpr (Img1::is_premultiplied) {

            auto linear_in = in.unmultiply_alpha();
            convert_image(linear_in, out);

        } else if constexpr (Img2::is_premultiplied) {

            Image<C2, F2 & ~ ImageFlags::premultiplied> linear_out;
            convert_image(in, linear_out);
            out = linear_out.multiply_alpha();

        } else {

            Img2 result(in.shape());
            int dy, y1, y2;
            if (Img1::is_top_down == Img2::is_top_down) {
                dy = 1;
                y1 = 0;
                y2 = in.height();
            } else {
                dy = -1;
                y1 = in.height() - 1;
                y2 = -1;
            }

            for (int y = y1; y != y2; y += dy) {
                auto i = in.locate(0, y);
                auto j = result.locate(0, y);
                for (int x = 0; x < in.width(); ++x, ++i, ++j)
                    convert_colour(*i, *j);
            }

            out = std::move(result);

        }

    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    void Image<Core::Colour<T, CS, CL>, Flags>::load(const IO::Path& file) {
        Point shape;
        if constexpr (std::is_same_v<channel_type, uint8_t>) {
            auto image_ptr = Detail::load_image_8(file, shape);
            Image<Core::Rgba8> image(shape);
            std::memcpy(image.data(), image_ptr.get(), image.bytes());
            convert_image(image, *this);
        } else if constexpr (std::is_same_v<channel_type, uint16_t>) {
            auto image_ptr = Detail::load_image_16(file, shape);
            Image<Core::Rgba16> image(shape);
            std::memcpy(image.data(), image_ptr.get(), image.bytes());
            convert_image(image, *this);
        } else {
            auto image_ptr = Detail::load_image_hdr(file, shape);
            Image<Core::Rgbaf> image(shape);
            std::memcpy(image.data(), image_ptr.get(), image.bytes());
            convert_image(image, *this);
        }
    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    void Image<Core::Colour<T, CS, CL>, Flags>::save(const IO::Path& file, int quality) const {
        auto format = Format::ascii_lowercase(file.split_leaf().second);
        if (format == ".hdr" || format == ".rgbe") {
            Image<Core::Rgbaf> image;
            convert_image(*this, image);
            Detail::save_image_hdr(image, file);
        } else {
            Image<Core::Rgba8> image;
            convert_image(*this, image);
            Detail::save_image_8(image, file, format, quality);
        }
    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    void Image<Core::Colour<T, CS, CL>, Flags>::resize(Point new_shape, ImageResize rflags) {
        auto img = resized(new_shape, rflags);
        *this = std::move(img);
    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    void Image<Core::Colour<T, CS, CL>, Flags>::resize(double scale, ImageResize rflags) {
        auto img = resized(scale, rflags);
        *this = std::move(img);
    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    Image<Core::Colour<T, CS, CL>, Flags>
    Image<Core::Colour<T, CS, CL>, Flags>::resized(Point new_shape, ImageResize rflags) const {

        static constexpr int stbir_flag_alpha_premultiplied  = 1;
        static constexpr int stbir_edge_clamp                = 1;
        static constexpr int stbir_edge_wrap                 = 3;
        static constexpr int stbir_filter_default            = 0;
        static constexpr int stbir_colorspace_linear         = 0;
        static constexpr int stbir_colorspace_srgb           = 1;

        using working_channel = std::conditional_t<std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t>, T, float>;
        using working_space = std::conditional_t<std::is_same_v<CS, Core::sRGB>, Core::sRGB, Core::LinearRGB>;
        using working_colour = Core::Colour<working_channel, working_space, CL>;
        using working_image = Image<working_colour, Flags>;

        bool use_unlock = !! (rflags & ImageResize::unlock);
        bool use_wrap = !! (rflags & ImageResize::wrap);

        auto fail = [new_shape] {
            throw std::invalid_argument(Format::format("Invalid image dimensions: {0}", new_shape));
        };

        if (new_shape.x() < 0 || new_shape.y() < 0)
            fail();
        if (new_shape.x() == 0 && new_shape.y() == 0)
            fail();
        if ((new_shape.x() == 0 || new_shape.y() == 0) && use_unlock)
            fail();

        Point actual_shape = new_shape;

        if (! use_unlock) {

            Core::Double2 rshape(new_shape.x(), new_shape.y());
            double ratio = double(width()) / double(height());

            if (new_shape.x() == 0) {
                rshape.x() = rshape.y() * ratio;
            } else if (new_shape.y() == 0) {
                rshape.y() = rshape.x() / ratio;
            } else {
                rshape.x() = std::min(rshape.x(), rshape.y() * ratio);
                rshape.y() = std::min(rshape.y(), rshape.x() / ratio);
            }

            actual_shape.x() = int(std::lround(rshape.x()));
            actual_shape.y() = int(std::lround(rshape.y()));

        }

        int stb_flags = is_premultiplied ? stbir_flag_alpha_premultiplied : 0;
        int stb_edge = use_wrap ? stbir_edge_wrap : stbir_edge_clamp;
        int stb_filter = stbir_filter_default;
        int stb_space = std::is_same_v<CS, Core::sRGB> ? stbir_colorspace_srgb : stbir_colorspace_linear;

        working_image working_input;
        convert_image(*this, working_input);
        working_image working_output(actual_shape);

        if constexpr (std::is_same_v<channel_type, uint8_t>)
            Detail::resize_image_8(working_input.data(), shape_, working_output.data(), actual_shape,
                working_colour::channels, working_colour::alpha_index, stb_flags, stb_edge, stb_filter, stb_space);
        else if constexpr (std::is_same_v<channel_type, uint16_t>)
            Detail::resize_image_16(working_input.data(), shape_, working_output.data(), actual_shape,
                working_colour::channels, working_colour::alpha_index, stb_flags, stb_edge, stb_filter, stb_space);
        else
            Detail::resize_image_hdr(working_input.data(), shape_, working_output.data(), actual_shape,
                working_colour::channels, working_colour::alpha_index, stb_flags, stb_edge, stb_filter, stb_space);

        Image result;
        convert_image(working_output, result);

        return result;

    }

    template <typename T, typename CS, Core::ColourLayout CL, ImageFlags Flags>
    Image<Core::Colour<T, CS, CL>, Flags>
    Image<Core::Colour<T, CS, CL>, Flags>::resized(double scale, ImageResize rflags) const {
        if (scale <= 0)
            throw std::invalid_argument(Format::format("Invalid image scale factor: {0}", scale));
        int w = int(std::lround(scale * width()));
        int h = int(std::lround(scale * height()));
        return resized(Point{w, h}, rflags | ImageResize::unlock);
    }

}
