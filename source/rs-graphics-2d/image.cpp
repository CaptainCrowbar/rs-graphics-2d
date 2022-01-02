#include "rs-graphics-2d/image.hpp"
#include "rs-format/format.hpp"

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

    std::string ImageIoError::make_message(const std::string& filename, const std::string& details, bool stbi) {
        std::string message = "Image I/O error";
        if (! filename.empty())
            message += ": " + filename;
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

    ImageInfo query_image(const std::string& filename) {
        ImageInfo info;
        if (! stbi_info(filename.data(), &info.shape.x(), &info.shape.y(), &info.channels))
            return info;
        if (stbi_is_hdr(filename.data())) {
            info.bits_per_channel = 32;
            info.is_hdr = true;
        } else {
            info.bits_per_channel = stbi_is_16_bit(filename.data()) ? 16 : 8;
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
                if constexpr (std::is_same_v<Source, const char>)
                    throw ImageIoError(src_ptr, {}, true);
                else
                    throw ImageIoError({}, {}, true);
            }
            return StbiPtr<Channel>(static_cast<Channel*>(image_ptr));
        }

        StbiPtr<uint8_t> load_image_8(const std::string& filename, Point& shape) {
            return load_image<uint8_t>(&stbi_load, filename.data(), 0, shape);
        }

        StbiPtr<uint16_t> load_image_16(const std::string& filename, Point& shape) {
            return load_image<uint16_t>(&stbi_load_16, filename.data(), 0, shape);
        }

        StbiPtr<float> load_image_hdr(const std::string& filename, Point& shape) {
            return load_image<float>(&stbi_loadf, filename.data(), 0, shape);
        }

        std::string get_file_format(const std::string& filename) {
            size_t slash;
            #ifdef _WIN32
                slash = filename.find_last_of("/\\");
            #else
                slash = filename.find_last_of('/');
            #endif
            if (slash == npos)
                slash = 0;
            else
                ++slash;
            size_t dot = filename.find_last_of('.');
            if (dot <= slash || dot == npos)
                throw ImageIoError(filename, "Unknown file format", false);
            auto format = ascii_lowercase(filename.substr(dot + 1));
            if (format != "bmp" && format != "hdr" && format != "jpg" && format != "jpeg" && format != "png" && format != "tga")
                throw ImageIoError(filename, "Unknown file format", false);
            return format;
        }

        void save_image_8(const Image<Core::Rgba8>& image, const std::string& filename, const std::string& format, int quality) {
            quality = std::clamp(quality, 1, 100);
            int rc = 0;
            if (format == "bmp")
                rc = stbi_write_bmp(filename.data(), image.width(), image.height(), 4, image.data());
            else if (format == "jpg" || format == "jpeg")
                rc = stbi_write_jpg(filename.data(), image.width(), image.height(), 4, image.data(), quality);
            else if (format == "png")
                rc = stbi_write_png(filename.data(), image.width(), image.height(), 4, image.data(), 0);
            else if (format == "tga")
                rc = stbi_write_tga(filename.data(), image.width(), image.height(), 4, image.data());
            else
                throw ImageIoError(filename, "Unknown file format", false);
            if (rc == 0)
                throw ImageIoError(filename, {}, false);
        }

        void save_image_hdr(const Image<Core::Rgbaf>& image, const std::string& filename) {
            int rc = stbi_write_hdr(filename.data(), image.width(), image.height(), 4, image.data());
            if (rc == 0)
                throw ImageIoError(filename, {}, false);
        }

    }

}
