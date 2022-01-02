#include "rs-graphics-2d/image.hpp"
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

namespace RS::Graphics::Plane {

    std::string ImageIoError::get_message(const std::string& file) {
        // TODO
        (void)file;
        return {};
    }

    // TODO

}
