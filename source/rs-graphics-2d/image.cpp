#include "rs-graphics-2d/image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_FAILURE_USERMSG
#define STBI_WINDOWS_UTF8
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define STBIW_WINDOWS_UTF8

#if defined(__aarch64__) || defined(_M_ARM64)
    #define STBI_NEON
#endif

#ifdef _MSC_VER
    #pragma warning(push, 1)
#else
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #pragma GCC diagnostic ignored "-Wsign-compare"
    #pragma GCC diagnostic ignored "-Wunused-function"
    #ifndef __clang__
        #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    #endif
#endif

#include "rs-graphics-2d/stb/stb_image.h"
#include "rs-graphics-2d/stb/stb_image_write.h"

#ifdef _MSC_VER
    #pragma warning(pop)
#else
    #pragma GCC diagnostic pop
#endif

using namespace RS::Format;
using namespace RS::Format::Literals;

namespace RS::Graphics::Plane {

    std::string ImageIoError::make_message(const IO::Path& file, const std::string& details, bool stbi) {
        std::string message = "Image I/O error";
        if (! file.empty())
            message += ": " + file.name();
        if (! details.empty())
            message += ": " + details;
        if (stbi) {
            auto reason = stbi_failure_reason();
            if (reason != nullptr && *reason != 0) {
                message += ": ";
                message += reason;
            }
        }
        return message;
    }

    std::string ImageInfo::str() const {
        if (! *this)
            return "<null>";
        static const auto fmt = "Image:{0}x{1},{2}ch@{3}bit"_fmt;
        auto s = fmt(shape.x(), shape.y(), channels, bits_per_channel);
        if (has_alpha)
            s += ",alpha";
        if (is_hdr)
            s += ",hdr";
        return s;
    }

    ImageInfo query_image(const IO::Path& file) noexcept {
        ImageInfo info;
        auto name = file.name();
        if (! stbi_info(name.data(), &info.shape.x(), &info.shape.y(), &info.channels))
            return info;
        if (stbi_is_hdr(name.data())) {
            info.bits_per_channel = 32;
            info.is_hdr = true;
        } else {
            info.bits_per_channel = stbi_is_16_bit(name.data()) ? 16 : 8;
            info.is_hdr = false;
        }
        info.has_alpha = (info.channels & 1) == 0;
        return info;
    }

    namespace Detail {

        template <typename Channel, typename Function, typename Source>
        StbiPtr<Channel> load_image(Function* stb_function, Source* src, size_t len, Point& shape) {
            auto src_ptr = static_cast<Source*>(src);
            int channels_in_file = 0;
            void* image_ptr;
            if constexpr (std::is_same_v<Source, const stbi_uc>)
                image_ptr = stb_function(src_ptr, int(len), &shape.x(), &shape.y(), &channels_in_file, 4);
            else
                image_ptr = stb_function(src_ptr, &shape.x(), &shape.y(), &channels_in_file, 4);
            if (image_ptr == nullptr) {
                if constexpr (std::is_same_v<std::decay_t<Source>, char>)
                    throw ImageIoError(src_ptr, {}, true);
                else
                    throw ImageIoError({}, {}, true);
            }
            return StbiPtr<Channel>(static_cast<Channel*>(image_ptr));
        }

        StbiPtr<uint8_t> load_image_8(const IO::Path& file, Point& shape) {
            auto name = file.name();
            return load_image<uint8_t>(&stbi_load, name.data(), 0, shape);
        }

        StbiPtr<uint16_t> load_image_16(const IO::Path& file, Point& shape) {
            auto name = file.name();
            return load_image<uint16_t>(&stbi_load_16, name.data(), 0, shape);
        }

        StbiPtr<float> load_image_hdr(const IO::Path& file, Point& shape) {
            auto name = file.name();
            return load_image<float>(&stbi_loadf, name.data(), 0, shape);
        }

        void save_image_8(const Image<Core::Rgba8>& image, const IO::Path& file, const std::string& format, int quality) {
            quality = std::clamp(quality, 1, 100);
            auto name = file.name();
            int rc = 0;
            if (format == ".bmp")
                rc = stbi_write_bmp(name.data(), image.width(), image.height(), 4, image.data());
            else if (format == ".jpg" || format == ".jpeg")
                rc = stbi_write_jpg(name.data(), image.width(), image.height(), 4, image.data(), quality);
            else if (format == ".png")
                rc = stbi_write_png(name.data(), image.width(), image.height(), 4, image.data(), 0);
            else if (format == ".tga")
                rc = stbi_write_tga(name.data(), image.width(), image.height(), 4, image.data());
            else
                throw ImageIoError(file, "Unknown file format", false);
            if (rc == 0)
                throw ImageIoError(file, {}, false);
        }

        void save_image_hdr(const Image<Core::Rgbaf>& image, const IO::Path& file) {
            auto name = file.name();
            int rc = stbi_write_hdr(name.data(), image.width(), image.height(), 4, image.data());
            if (rc == 0)
                throw ImageIoError(file, {}, false);
        }

    }

}
