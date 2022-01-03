#pragma once

// Temporary code, this will be moved to a separate library

#include "rs-format/string.hpp"
#include <cstdio>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Graphics::Plane {

    constexpr size_t npos = std::string::npos;

    namespace Detail {

        constexpr bool windows_target =
            #ifdef _WIN32
                true;
            #else
                false;
            #endif

        constexpr bool big_endian_target =
            #if defined(_WIN32) || __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                false;
            #else
                true;
            #endif

        template <size_t N>
        constexpr void swap_ends(void* ptr) noexcept {
            if constexpr (N >= 2) {
                auto bptr = static_cast<uint8_t*>(ptr);
                uint8_t b = bptr[0];
                bptr[0] = bptr[N - 1];
                bptr[N - 1] = b;
                swap_ends<N - 2>(bptr + 1);
            }
        }

        template <typename T, bool BE>
        T read_endian(FILE* fptr) noexcept {
            static_assert(std::is_integral_v<T>);
            T t = 0;
            auto rc = std::fread(&t, sizeof(T), 1, fptr);
            (void)rc;
            if constexpr (BE != big_endian_target)
                swap_ends<sizeof(T)>(&t);
            return t;
        }

    }

    enum ListDir: int {
        recursive  = 1,
        symlinks   = 2,
    };

    class Cstdio {
    public:
        Cstdio(const std::string& filename, const std::string& mode) noexcept;
        ~Cstdio() noexcept;
        Cstdio(const Cstdio&) = delete;
        Cstdio(Cstdio&&) = delete;
        Cstdio& operator=(const Cstdio&) = delete;
        Cstdio& operator=(Cstdio&&) = delete;
        operator FILE*() const noexcept { return fptr_; }
    private:
        FILE* fptr_ = nullptr;
    };

    bool file_exists(const std::string& filename) noexcept;
    bool file_is_directory(const std::string& filename) noexcept;
    bool file_is_symlink(const std::string& filename) noexcept;
    std::vector<std::string> list_directory(const std::string& dir, int flags = 0);
    bool load_file(const std::string& filename, std::string& content, size_t maxlen = npos);
    bool load_file(FILE* fptr, std::string& content, size_t maxlen = npos);
    template <typename T> T read_be(FILE* fptr) noexcept { return Detail::read_endian<T, true>(fptr); }
    template <typename T> T read_le(FILE* fptr) noexcept { return Detail::read_endian<T, false>(fptr); }

    template <typename T1, typename T2>
    std::string merge_paths(T1&& a, T2&& b) {
        static constexpr char slash = Detail::windows_target ? '\\' : '/';
        static const auto is_slash = [] (char c) { return c == '/' || (Detail::windows_target && c == '\\'); };
        static const auto is_absolute = [] (const std::string& path) {
            return is_slash(path[0]) || (Detail::windows_target && path.size() >= 2 && Format::ascii_isalpha(path[0]) && path[1] == ':');
        };
        std::string left(std::forward<T1>(a)), right(std::forward<T2>(b));
        if (right.empty())
            return left;
        else if (left.empty() || is_absolute(right))
            return right;
        if (! is_slash(left.back()))
            left += slash;
        return left + right;
    }

    template <typename T1, typename T2, typename T3, typename... Args>
    std::string merge_paths(T1&& a, T2&& b, T3&& c, Args&&... d) {
        auto ab = merge_paths(std::forward<T1>(a), std::forward<T2>(b));
        return merge_paths(ab, std::forward<T3>(c), std::forward<Args>(d)...);
    }

}
