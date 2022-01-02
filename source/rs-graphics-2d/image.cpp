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

using namespace RS::Format::Literals;

namespace RS::Graphics::Plane {

    std::string ImageIoError::get_message(const std::string& file) {
        std::string msg = "Image I/O error";
        if (! file.empty())
            msg += ": " + file;
        auto reason = stbi_failure_reason();
        if (reason != nullptr && *reason != 0) {
            msg += ": ";
            msg += reason;
        }
        return msg;
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
                    throw ImageIoError(src_ptr);
                else
                    throw ImageIoError();
            }
            return StbiPtr<Channel>(static_cast<Channel*>(image_ptr));
        }

        StbiPtr<uint8_t> load_image_8_from_file(const std::string& filename, Point& shape) {
            return load_image<uint8_t>(&stbi_load, filename.data(), 0, shape);
        }

        StbiPtr<uint16_t> load_image_16_from_file(const std::string& filename, Point& shape) {
            return load_image<uint16_t>(&stbi_load_16, filename.data(), 0, shape);
        }

        StbiPtr<float> load_image_hdr_from_file(const std::string& filename, Point& shape) {
            return load_image<float>(&stbi_loadf, filename.data(), 0, shape);
        }

        StbiPtr<uint8_t> load_image_8_from_cstdio(FILE* file, Point& shape) {
            return load_image<uint8_t>(&stbi_load_from_file, file, 0, shape);
        }

        StbiPtr<uint16_t> load_image_16_from_cstdio(FILE* file, Point& shape) {
            return load_image<uint16_t>(&stbi_load_from_file_16, file, 0, shape);
        }

        StbiPtr<float> load_image_hdr_from_cstdio(FILE* file, Point& shape) {
            return load_image<float>(&stbi_loadf_from_file, file, 0, shape);
        }

        StbiPtr<uint8_t> load_image_8_from_memory(const void* ptr, size_t len, Point& shape) {
            return load_image<uint8_t>(&stbi_load_from_memory, static_cast<const stbi_uc*>(ptr), len, shape);
        }

        StbiPtr<uint16_t> load_image_16_from_memory(const void* ptr, size_t len, Point& shape) {
            return load_image<uint16_t>(&stbi_load_16_from_memory, static_cast<const stbi_uc*>(ptr), len, shape);
        }

        StbiPtr<float> load_image_hdr_from_memory(const void* ptr, size_t len, Point& shape) {
            return load_image<float>(&stbi_loadf_from_memory, static_cast<const stbi_uc*>(ptr), len, shape);
        }

        // Image output

        // bool save_quantised_image(const std::string& filename, const std::string& format,
        //     const uint8_t* ptr, Point shape, int channels, int quality);
        // bool save_hdr_image(const std::string& filename, const float* ptr, Point shape, int channels);

    }

}
