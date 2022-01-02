#include "rs-graphics-2d/image.hpp"
#include "rs-format/format.hpp"
#include <cstdio>

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

    // Exceptions

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

    // Image information

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

    // Image input

    // TODO

    // Basic usage (see HDR discussion below for HDR usage):
    //    int x,y,n;
    //    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
    //    // ... process data if not NULL ...
    //    // ... x = width, y = height, n = # 8-bit components per pixel ...
    //    // ... replace '0' with '1'..'4' to force that many components per pixel
    //    // ... but 'n' will always be the number that it would have been if you said 0
    //    stbi_image_free(data)

    //    float *data = stbi_loadf(filename, &x, &y, &n, 0);

    // If image loading fails for any reason, the return value will be NULL,
    // and *x, *y, *channels_in_file will be unchanged. The function
    // stbi_failure_reason() can be queried for an extremely brief, end-user
    // unfriendly explanation of why the load failed. Define STBI_NO_FAILURE_STRINGS
    // to avoid compiling these strings at all, and STBI_FAILURE_USERMSG to get slightly
    // more user-friendly ones.

    // STBIDEF int stbi_convert_wchar_to_utf8(char *buffer, size_t bufferlen, const wchar_t* input);

    // STBIDEF stbi_uc *stbi_load_from_memory (stbi_uc const *buffer, int len , int *x, int *y, int *channels_in_file, int desired_channels);
    // STBIDEF stbi_uc *stbi_load (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    // STBIDEF stbi_uc *stbi_load_from_file (FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);

    // STBIDEF stbi_us *stbi_load_16_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
    // STBIDEF stbi_us *stbi_load_16 (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    // STBIDEF stbi_us *stbi_load_from_file_16(FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);

    // STBIDEF float *stbi_loadf_from_memory (stbi_uc const *buffer, int len, int *x, int *y, int *channels_in_file, int desired_channels);
    // STBIDEF float *stbi_loadf (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    // STBIDEF float *stbi_loadf_from_file (FILE *f, int *x, int *y, int *channels_in_file, int desired_channels);

}
