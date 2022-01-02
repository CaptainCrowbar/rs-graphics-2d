#pragma once

#include "rs-graphics-core/colour.hpp"
#include "rs-graphics-core/colour-space.hpp"
#include "rs-graphics-core/vector.hpp"
#include "rs-format/enum.hpp"
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>

namespace RS::Graphics::Plane {

    RS_DEFINE_ENUM_CLASS(ImageLayout, int, 0,
        top_down,
        bottom_up
    )

    template <typename Colour, ImageLayout Layout = ImageLayout::top_down>
    class Image;

    template <typename T, typename CS, Core::ColourLayout CL, ImageLayout Layout>
    class Image<Core::Colour<T, CS, CL>, Layout> {

    private:

        template <typename CI, typename CC>
        class basic_iterator {

        public:

            using difference_type = int64_t;
            using iterator_category = std::forward_iterator_tag;
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
            bool operator==(const basic_iterator& i) const noexcept { return index_ == i.index_; }
            bool operator!=(const basic_iterator& i) const noexcept { return ! (*this == i); }

            basic_iterator& move(int axis, int distance = 1) noexcept {
                // Axis is 0 for x, 1 for y; you can also use 'x' and 'y'
                int64_t d = distance;
                if ((axis & 1) == 1)
                    d *= image_->width();
                index_ += d;
                return *this;
            }

            Core::Vector<int, 2> pos() const noexcept {
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
        using point_type = Core::Vector<int, 2>;
        using iterator = basic_iterator<Image, colour_type>;
        using const_iterator = basic_iterator<const Image, const colour_type>;

        static constexpr int channels = colour_type::channels;
        static constexpr Core::ColourLayout colour_layout = CL;
        static constexpr bool has_alpha = colour_type::has_alpha;
        static constexpr ImageLayout image_layout = Layout;
        static constexpr bool is_hdr = colour_type::is_hdr;

        Image() noexcept: pixels_(), shape_(0, 0) {}
        explicit Image(point_type shape) { reset(shape); }
        Image(point_type shape, colour_type c) { reset(shape, c); }
        Image(int w, int h) { reset(w, h); }
        Image(int w, int h, colour_type c) { reset(w, h, c); }

        colour_type& operator[](point_type p) noexcept { return (*this)(p.x(), p.y()); }
        const colour_type& operator[](point_type p) const noexcept { return (*this)(p.x(), p.y()); }
        colour_type& operator()(int x, int y) noexcept { return *locate(x, y); }
        const colour_type& operator()(int x, int y) const noexcept { return *locate(x, y); }

        iterator begin() noexcept { return iterator(*this, 0); }
        const_iterator begin() const noexcept { return const_iterator(*this, 0); }
        iterator end() noexcept { return iterator(*this, int64_t(size())); }
        const_iterator end() const noexcept { return const_iterator(*this, int64_t(size())); }
        T* data() noexcept { return reinterpret_cast<T*>(pixels_.data()); }
        const T* data() const noexcept { return reinterpret_cast<const T*>(pixels_.data()); }

        iterator bottom_left() noexcept { return locate(0, Layout == ImageLayout::top_down ? height() - 1 : 0); }
        const_iterator bottom_left() const noexcept { return locate(0, Layout == ImageLayout::top_down ? height() - 1 : 0); }
        iterator bottom_right() noexcept { return locate(width() - 1, Layout == ImageLayout::top_down ? height() - 1 : 0); }
        const_iterator bottom_right() const noexcept { return locate(width() - 1, Layout == ImageLayout::top_down ? height() - 1 : 0); }
        iterator top_left() noexcept { return locate(0, Layout == ImageLayout::top_down ? 0 : height() - 1); }
        const_iterator top_left() const noexcept { return locate(0, Layout == ImageLayout::top_down ? 0 : height() - 1); }
        iterator top_right() noexcept { return locate(width() - 1, Layout == ImageLayout::top_down ? 0 : height() - 1); }
        const_iterator top_right() const noexcept { return locate(width() - 1, Layout == ImageLayout::top_down ? 0 : height() - 1); }

        void clear() noexcept { pixels_.reset(); shape_ = {}; }
        void fill(colour_type c) noexcept { std::fill(pixels_.begin(), pixels_.end(), c); }

        iterator locate(point_type p) noexcept { return locate(p.x(), p.y()); }
        const_iterator locate(point_type p) const noexcept { return locate(p.x(), p.y()); }
        iterator locate(int x, int y) noexcept { return iterator(*this, make_index(x, y)); }
        const_iterator locate(int x, int y) const noexcept { return const_iterator(*this, make_index(x, y)); }

        void reset(point_type shape) { reset(shape.x(), shape.y()); }
        void reset(point_type shape, colour_type c) { reset(shape.x(), shape.y(), c); }
        void reset(int w, int h);
        void reset(int w, int h, colour_type c);

        point_type shape() const noexcept { return shape_; }
        bool empty() const noexcept { return pixels_.empty(); }
        int width() const noexcept { return shape_.x(); }
        int height() const noexcept { return shape_.y(); }
        size_t size() const noexcept { return size_t(width()) * size_t(height()); }

        void swap(Image& img) noexcept { pixels_.swap(img.pixels_); std::swap(shape_, img.shape_); }
        friend void swap(Image& a, Image& b) noexcept { a.swap(b); }

        friend bool operator==(const Image& a, const Image& b) noexcept { return a.shape_ == b.shape_ && a.pixels_ == b.pixels_; }
        friend bool operator!=(const Image& a, const Image& b) noexcept { return ! (a == b); }

    private:

        std::vector<colour_type> pixels_;
        point_type shape_;

        int64_t make_index(int x, int y) const noexcept { return int64_t(width()) * y + x; }

    };

        template <typename T, typename CS, Core::ColourLayout CL, ImageLayout Layout>
        void Image<Core::Colour<T, CS, CL>, Layout>::reset(int w, int h) {
            size_t n = size_t(w) * size_t(h);
            pixels_.resize(n);
            shape_ = {w, h};
        }

        template <typename T, typename CS, Core::ColourLayout CL, ImageLayout Layout>
        void Image<Core::Colour<T, CS, CL>, Layout>::reset(int w, int h, colour_type c) {
            size_t n = size_t(w) * size_t(h);
            pixels_.clear();
            pixels_.resize(n, c);
            shape_ = {w, h};
        }

    using Image8 = Image<Core::Rgba8>;
    using Image16 = Image<Core::Rgba16>;
    using HdrImage = Image<Core::Rgbaf>;
    using sImage8 = Image<Core::sRgba8>;
    using sImage16 = Image<Core::sRgba16>;
    using sHdrImage = Image<Core::sRgbaf>;

}
