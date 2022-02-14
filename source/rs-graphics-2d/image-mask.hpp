#pragma once

// This header is not part of the public interface

#include "rs-graphics-2d/image.hpp"
#include "rs-graphics-core/colour.hpp"
#include <algorithm>
#include <cstring>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace RS::Graphics::Plane::Detail {

    template <typename T>
    class ImageMask {

    public:

        static_assert(std::is_arithmetic_v<T>);

        using value_type = T;

        static constexpr T scale = std::is_floating_point_v<T> ? 1 : std::numeric_limits<T>::max();

        ImageMask() = default;
        explicit ImageMask(Point shape) noexcept;
        template <typename Del> explicit ImageMask(Point shape, T* ptr, Del del) noexcept: shape_(shape), ptr_(ptr, del) {}

        T& operator[](Point pos) noexcept { return ptr_.get()[size_t(shape_.x()) * size_t(pos.y()) + size_t(pos.x())]; }
        const T& operator[](Point pos) const noexcept { return ptr_.get()[size_t(shape_.x()) * size_t(pos.y()) + size_t(pos.x())]; }

        T* begin() noexcept { return ptr_.get(); }
        const T* begin() const noexcept { return ptr_.get(); }
        T* end() noexcept { return ptr_.get() + area(); }
        const T* end() const noexcept { return ptr_.get() + area(); }
        size_t area() const noexcept { return size_t(shape_.x()) * size_t(shape_.y()); }
        bool empty() const noexcept { return ! ptr_ || shape_.x() <= 0 || shape_.y() <= 0; }
        Point shape() const noexcept { return shape_; }

        template <typename C, int F> void make_image(Image<C, F>& image, C foreground, C background) const;
        template <typename C, int F> void onto_image(Image<C, F>& image, Point offset, C colour) const;

    private:

        Point shape_;
        std::shared_ptr<T[]> ptr_;

        static constexpr T multiply_alpha(T a, T b) noexcept {
            if constexpr (std::is_floating_point_v<T>) {
                return a * b;
            } else if constexpr (sizeof(T) <= 4) {
                using U = std::conditional_t<(sizeof(T) == 1), uint16_t,
                    std::conditional_t<(sizeof(T) == 2), uint32_t, uint64_t>>;
                U c = (U(a) * U(b) + U(scale) / 2) / U(scale);
                return T(c);
            } else {
                double c = double(a) * double(b) / double(scale);
                return T(c + 0.5);
            }
        }

    };

        using ByteMask = ImageMask<unsigned char>;
        using HdrMask = ImageMask<float>;

        template <typename T>
        ImageMask<T>::ImageMask(Point shape) noexcept:
        shape_(shape),
        ptr_(new T[area()]) {
            std::memset(ptr_.get(), 0, area() * sizeof(T));
        }

        template <typename T>
        template <typename C, int F>
        void ImageMask<T>::make_image(Image<C, F>& image, C foreground, C background) const {

            // TODO - allow colours without alpha channel

            static_assert(C::is_linear);

            using namespace Core;

            using output_image = Image<C, F>;
            using working_colour = Colour<T>;

            working_colour fg, bg, wc;
            convert_colour(foreground, fg);
            convert_colour(background, bg);

            output_image result(shape());
            auto out = result.begin();

            for (auto& im: *this) {
                wc = fg;
                wc.alpha() = multiply_alpha(fg.alpha(), im);
                wc = alpha_blend(wc, bg);
                convert_colour(wc, *out);
                if (output_image::is_premultiplied)
                    out->multiply_alpha();
                ++out;
            }

            image = std::move(result);

        }

        template <typename T>
        template <typename C, int F>
        void ImageMask<T>::onto_image(Image<C, F>& image, Point offset, C colour) const {

            // TODO - allow colours without alpha channel

            static_assert(C::is_linear);

            using namespace Core;

            using output_image = Image<C, F>;
            using working_colour = Colour<T>;

            int mask_x1 = std::max(0, - offset.x());
            int mask_y1 = std::max(0, - offset.y());
            int mask_x2 = std::min(shape().x(), image.width() - offset.x());
            int mask_y2 = std::min(shape().y(), image.height() - offset.y());

            if (mask_x1 >= mask_x2 || mask_y1 >= mask_y2)
                return;

            int image_x1 = mask_x1 + offset.x();
            int image_y1 = mask_y1 + offset.y();

            Point p; // point on mask
            Point q; // point on image

            working_colour fg, bg, wc;
            convert_colour(colour, fg);

            for (p.y() = mask_y1, q.y() = image_y1; p.y() < mask_y2; ++p.y(), ++q.y()) {
                for (p.x() = mask_x1, q.x() = image_x1; p.x() < mask_x2; ++p.x(), ++q.x()) {
                    wc = fg;
                    wc.alpha() = multiply_alpha(fg.alpha(), (*this)[p]);
                    convert_colour(image[q], bg);
                    wc = alpha_blend(wc, bg);
                    convert_colour(wc, image[q]);
                    if (output_image::is_premultiplied)
                        image[q].multiply_alpha();
                }
            }

        }

}
