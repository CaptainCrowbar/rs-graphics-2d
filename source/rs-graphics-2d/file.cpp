#include "rs-graphics-2d/file.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

#ifdef _XOPEN_SOURCE
    #include <dirent.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

namespace RS::Graphics::Plane {

    Cstdio::Cstdio(const std::string& filename, const std::string& mode) noexcept:
    fptr_(std::fopen(filename.data(), mode.data())) {}

    Cstdio::~Cstdio() noexcept {
        if (fptr_)
            std::fclose(fptr_);
    }

    bool file_exists(const std::string& filename) noexcept {
        #ifdef _WIN32
            std::u16string ufile;
            encode_utf16(filename, ufile);
            auto wfile = reinterpret_cast<const wchar_t*>(ufile.data());
            DWORD rc = GetFileAttributesW(wfile);
            return rc != INVALID_FILE_ATTRIBUTES;
        #else
            struct stat st;
            return ::stat(filename.data(), &st) == 0;
        #endif
    }

    bool file_is_directory(const std::string& filename) noexcept {
        #ifdef _WIN32
            std::u16string ufile;
            encode_utf16(filename, ufile);
            auto wfile = reinterpret_cast<const wchar_t*>(ufile.data());
            DWORD rc = GetFileAttributesW(wfile);
            return (rc & FILE_ATTRIBUTE_DIRECTORY) != 0;
        #else
            struct stat st;
            return ::stat(filename.data(), &st) == 0 && S_ISDIR(st.st_mode);
        #endif
    }

    bool file_is_symlink(const std::string& filename) noexcept {
        #ifdef _WIN32
            (void)filename;
            return false;
        #else
            struct stat st;
            return ::stat(filename.data(), &st) == 0 && S_ISLNK(st.st_mode);
        #endif
    }

    std::vector<std::string> list_directory(const std::string& dir, int flags) {

        std::vector<std::string> files;
        if (dir.empty())
            return files;
        bool recurse = (flags & recursive) != 0;
        bool follow = (flags & symlinks) != 0;

        #ifdef _WIN32

            struct FindHandle {
                HANDLE handle = nullptr;
                ~FindHandle() noexcept { if (handle) FindClose(handle); }
            };

            // FindFirstFile() gives a false positive for "file\*" if "file" exists but is not a directory

            if (! file_is_directory(dir))
                return;
            std::u16string uglob;
            encode_utf16(merge_paths(filename, "*"), uglob);
            auto wglob = reinterpret_cast<const wchar_t*>(uglob.data());
            FindHandle fh;
            WIN32_FIND_DATAW info;
            fh.handle = FindFirstFileW(wglob, &info);
            if (! fh.handle)
                return files;
            std::string file;

            for (;;) {
                file = info.cFileName;
                if (file != "." && file != "..")
                    files.push_back(file);
                if (! FindNextFile(fh.handle, &info))
                    break;
            }

        #else

            struct Dirptr {
                DIR* ptr = nullptr;
                ~Dirptr() noexcept { if (ptr) ::closedir(ptr); }
            };

            Dirptr dp;
            dp.ptr = ::opendir(dir.data());
            if (! dp.ptr)
                return files;
            dirent* entry;
            std::string file;

            for (;;) {
                entry = ::readdir(dp.ptr);
                if (! entry)
                    break;
                file = entry->d_name;
                if (file != "." && file != "..")
                    files.push_back(file);
            }

        #endif

        if (recurse) {
            std::vector<std::string> more;
            for (auto& child: files) {
                std::string path = merge_paths(dir, child);
                if (follow || ! file_is_symlink(path))
                    for (auto& grandchild: list_directory(path, flags))
                        more.push_back(merge_paths(child, grandchild));
            }
            files.insert(files.end(), more.begin(), more.end());
        }

        return files;

    }

    bool load_file(const std::string& filename, std::string& content, size_t maxlen) {
        Cstdio io(filename, "rb");
        return load_file(io, content, maxlen);
    }

    bool load_file(FILE* fptr, std::string& content, size_t maxlen) {
        if (! fptr)
            return false;
        if (maxlen == npos) {
            std::fseek(fptr, 0, SEEK_END);
            maxlen = size_t(std::ftell(fptr));
            std::fseek(fptr, 0, SEEK_SET);
        }
        content.resize(maxlen);
        size_t rc = std::fread(content.data(), 1, maxlen, fptr);
        return rc == maxlen;
    }

}
