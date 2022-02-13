#pragma once

// This header is not part of the public interface

#include "rs-graphics-2d/image.hpp"
#include "rs-graphics-core/colour.hpp"
#include <cstring>
#include <limits>
#include <memory>
#include <type_traits>

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

    private:

        Point shape_;
        std::shared_ptr<T[]> ptr_;

    };

        using ByteMask = ImageMask<unsigned char>;
        using HdrMask = ImageMask<float>;

        template <typename T>
        ImageMask<T>::ImageMask(Point shape) noexcept:
        shape_(shape),
        ptr_(new T[area()]) {
            std::memset(ptr_.get(), 0, area() * sizeof(T));
        }

}
