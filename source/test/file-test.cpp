#include "rs-graphics-2d/file.hpp"
#include "rs-unit-test.hpp"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using namespace RS::Graphics::Plane;
using namespace RS::UnitTest;

void test_rs_graphics_2d_file_io() {

    static const std::string good_file = "../source/CMakeLists.txt";
    static const std::string bad_file = "NoSuchFile.txt";

    std::string text;
    bool ok = false;

    TRY(ok = load_file(good_file, text));
    TEST(ok);
    TEST(! text.empty());
    TEST_EQUAL(text.substr(0, 22), "cmake_minimum_required");

    text.clear();
    TRY(ok = load_file(good_file, text, 100));
    TEST(ok);
    TEST(! text.empty());
    TEST_EQUAL(text.size(), 100u);
    TEST_EQUAL(text.substr(0, 22), "cmake_minimum_required");

    text.clear();
    TRY(ok = load_file(bad_file, text));
    TEST(! ok);
    TEST(text.empty());

}

void test_rs_graphics_2d_file_paths() {

    std::string path;

    TRY(path = merge_paths("", ""));                                          TEST_EQUAL(path, "");
    TRY(path = merge_paths("", "xyz"));                                       TEST_EQUAL(path, "xyz");
    TRY(path = merge_paths("", "uvw/xyz"));                                   TEST_EQUAL(path, "uvw/xyz");
    TRY(path = merge_paths("", "rst/uvw/xyz"));                               TEST_EQUAL(path, "rst/uvw/xyz");
    TRY(path = merge_paths("", "/"));                                         TEST_EQUAL(path, "/");
    TRY(path = merge_paths("", "/xyz"));                                      TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("", "/uvw/xyz"));                                  TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("", "/rst/uvw/xyz"));                              TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("/", ""));                                         TEST_EQUAL(path, "/");
    TRY(path = merge_paths("/", "xyz"));                                      TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("/", "uvw/xyz"));                                  TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("/", "rst/uvw/xyz"));                              TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("/", "/"));                                        TEST_EQUAL(path, "/");
    TRY(path = merge_paths("/", "/xyz"));                                     TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("/", "/uvw/xyz"));                                 TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("/", "/rst/uvw/xyz"));                             TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc", ""));                                       TEST_EQUAL(path, "abc");
    TRY(path = merge_paths("abc", "xyz"));                                    TEST_EQUAL(path, "abc/xyz");
    TRY(path = merge_paths("abc", "uvw/xyz"));                                TEST_EQUAL(path, "abc/uvw/xyz");
    TRY(path = merge_paths("abc", "rst/uvw/xyz"));                            TEST_EQUAL(path, "abc/rst/uvw/xyz");
    TRY(path = merge_paths("abc", "/"));                                      TEST_EQUAL(path, "/");
    TRY(path = merge_paths("abc", "/xyz"));                                   TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("abc", "/uvw/xyz"));                               TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("abc", "/rst/uvw/xyz"));                           TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def", ""));                                   TEST_EQUAL(path, "abc/def");
    TRY(path = merge_paths("abc/def", "xyz"));                                TEST_EQUAL(path, "abc/def/xyz");
    TRY(path = merge_paths("abc/def", "uvw/xyz"));                            TEST_EQUAL(path, "abc/def/uvw/xyz");
    TRY(path = merge_paths("abc/def", "rst/uvw/xyz"));                        TEST_EQUAL(path, "abc/def/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def", "/"));                                  TEST_EQUAL(path, "/");
    TRY(path = merge_paths("abc/def", "/xyz"));                               TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("abc/def", "/uvw/xyz"));                           TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("abc/def", "/rst/uvw/xyz"));                       TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi", ""));                               TEST_EQUAL(path, "abc/def/ghi");
    TRY(path = merge_paths("abc/def/ghi", "xyz"));                            TEST_EQUAL(path, "abc/def/ghi/xyz");
    TRY(path = merge_paths("abc/def/ghi", "uvw/xyz"));                        TEST_EQUAL(path, "abc/def/ghi/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi", "rst/uvw/xyz"));                    TEST_EQUAL(path, "abc/def/ghi/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi", "/"));                              TEST_EQUAL(path, "/");
    TRY(path = merge_paths("abc/def/ghi", "/xyz"));                           TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("abc/def/ghi", "/uvw/xyz"));                       TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi", "/rst/uvw/xyz"));                   TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc/", ""));                                      TEST_EQUAL(path, "abc/");
    TRY(path = merge_paths("abc/", "xyz"));                                   TEST_EQUAL(path, "abc/xyz");
    TRY(path = merge_paths("abc/", "uvw/xyz"));                               TEST_EQUAL(path, "abc/uvw/xyz");
    TRY(path = merge_paths("abc/", "rst/uvw/xyz"));                           TEST_EQUAL(path, "abc/rst/uvw/xyz");
    TRY(path = merge_paths("abc/", "/"));                                     TEST_EQUAL(path, "/");
    TRY(path = merge_paths("abc/", "/xyz"));                                  TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("abc/", "/uvw/xyz"));                              TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("abc/", "/rst/uvw/xyz"));                          TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def/", ""));                                  TEST_EQUAL(path, "abc/def/");
    TRY(path = merge_paths("abc/def/", "xyz"));                               TEST_EQUAL(path, "abc/def/xyz");
    TRY(path = merge_paths("abc/def/", "uvw/xyz"));                           TEST_EQUAL(path, "abc/def/uvw/xyz");
    TRY(path = merge_paths("abc/def/", "rst/uvw/xyz"));                       TEST_EQUAL(path, "abc/def/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def/", "/"));                                 TEST_EQUAL(path, "/");
    TRY(path = merge_paths("abc/def/", "/xyz"));                              TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("abc/def/", "/uvw/xyz"));                          TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("abc/def/", "/rst/uvw/xyz"));                      TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi/", ""));                              TEST_EQUAL(path, "abc/def/ghi/");
    TRY(path = merge_paths("abc/def/ghi/", "xyz"));                           TEST_EQUAL(path, "abc/def/ghi/xyz");
    TRY(path = merge_paths("abc/def/ghi/", "uvw/xyz"));                       TEST_EQUAL(path, "abc/def/ghi/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi/", "rst/uvw/xyz"));                   TEST_EQUAL(path, "abc/def/ghi/rst/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi/", "/"));                             TEST_EQUAL(path, "/");
    TRY(path = merge_paths("abc/def/ghi/", "/xyz"));                          TEST_EQUAL(path, "/xyz");
    TRY(path = merge_paths("abc/def/ghi/", "/uvw/xyz"));                      TEST_EQUAL(path, "/uvw/xyz");
    TRY(path = merge_paths("abc/def/ghi/", "/rst/uvw/xyz"));                  TEST_EQUAL(path, "/rst/uvw/xyz");
    TRY(path = merge_paths("abc", "def", "ghi", "jkl"));                      TEST_EQUAL(path, "abc/def/ghi/jkl");
    TRY(path = merge_paths("abc/def", "ghi/jkl", "mno/pqr", "stu/vwx"));      TEST_EQUAL(path, "abc/def/ghi/jkl/mno/pqr/stu/vwx");
    TRY(path = merge_paths("abc/def/", "ghi/jkl/", "mno/pqr/", "stu/vwx/"));  TEST_EQUAL(path, "abc/def/ghi/jkl/mno/pqr/stu/vwx/");
    TRY(path = merge_paths("/abc/def", "/ghi/jkl", "/mno/pqr", "/stu/vwx"));  TEST_EQUAL(path, "/stu/vwx");

    #ifdef _WIN32

        TRY(path = merge_paths("", ""));                                                          TEST_EQUAL(path, "");
        TRY(path = merge_paths("", "xyz"));                                                       TEST_EQUAL(path, "xyz");
        TRY(path = merge_paths("", "uvw\\xyz"));                                                  TEST_EQUAL(path, "uvw\\xyz");
        TRY(path = merge_paths("", "rst\\uvw\\xyz"));                                             TEST_EQUAL(path, "rst\\uvw\\xyz");
        TRY(path = merge_paths("", "C:\\"));                                                      TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("", "C:\\xyz"));                                                   TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("", "C:\\uvw\\xyz"));                                              TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("", "C:\\rst\\uvw\\xyz"));                                         TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("C:\\", ""));                                                      TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("C:\\", "xyz"));                                                   TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("C:\\", "uvw\\xyz"));                                              TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("C:\\", "rst\\uvw\\xyz"));                                         TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("C:\\", "C:\\"));                                                  TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("C:\\", "C:\\xyz"));                                               TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("C:\\", "C:\\uvw\\xyz"));                                          TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("C:\\", "C:\\rst\\uvw\\xyz"));                                     TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc", ""));                                                       TEST_EQUAL(path, "abc");
        TRY(path = merge_paths("abc", "xyz"));                                                    TEST_EQUAL(path, "abc\\xyz");
        TRY(path = merge_paths("abc", "uvw\\xyz"));                                               TEST_EQUAL(path, "abc\\uvw\\xyz");
        TRY(path = merge_paths("abc", "rst\\uvw\\xyz"));                                          TEST_EQUAL(path, "abc\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc", "C:\\"));                                                   TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("abc", "C:\\xyz"));                                                TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("abc", "C:\\uvw\\xyz"));                                           TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("abc", "C:\\rst\\uvw\\xyz"));                                      TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def", ""));                                                  TEST_EQUAL(path, "abc\\def");
        TRY(path = merge_paths("abc\\def", "xyz"));                                               TEST_EQUAL(path, "abc\\def\\xyz");
        TRY(path = merge_paths("abc\\def", "uvw\\xyz"));                                          TEST_EQUAL(path, "abc\\def\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def", "rst\\uvw\\xyz"));                                     TEST_EQUAL(path, "abc\\def\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def", "C:\\"));                                              TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("abc\\def", "C:\\xyz"));                                           TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("abc\\def", "C:\\uvw\\xyz"));                                      TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def", "C:\\rst\\uvw\\xyz"));                                 TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi", ""));                                             TEST_EQUAL(path, "abc\\def\\ghi");
        TRY(path = merge_paths("abc\\def\\ghi", "xyz"));                                          TEST_EQUAL(path, "abc\\def\\ghi\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi", "uvw\\xyz"));                                     TEST_EQUAL(path, "abc\\def\\ghi\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi", "rst\\uvw\\xyz"));                                TEST_EQUAL(path, "abc\\def\\ghi\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi", "C:\\"));                                         TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("abc\\def\\ghi", "C:\\xyz"));                                      TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi", "C:\\uvw\\xyz"));                                 TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi", "C:\\rst\\uvw\\xyz"));                            TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\", ""));                                                     TEST_EQUAL(path, "abc\\");
        TRY(path = merge_paths("abc\\", "xyz"));                                                  TEST_EQUAL(path, "abc\\xyz");
        TRY(path = merge_paths("abc\\", "uvw\\xyz"));                                             TEST_EQUAL(path, "abc\\uvw\\xyz");
        TRY(path = merge_paths("abc\\", "rst\\uvw\\xyz"));                                        TEST_EQUAL(path, "abc\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\", "C:\\"));                                                 TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("abc\\", "C:\\xyz"));                                              TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("abc\\", "C:\\uvw\\xyz"));                                         TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("abc\\", "C:\\rst\\uvw\\xyz"));                                    TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\", ""));                                                TEST_EQUAL(path, "abc\\def\\");
        TRY(path = merge_paths("abc\\def\\", "xyz"));                                             TEST_EQUAL(path, "abc\\def\\xyz");
        TRY(path = merge_paths("abc\\def\\", "uvw\\xyz"));                                        TEST_EQUAL(path, "abc\\def\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\", "rst\\uvw\\xyz"));                                   TEST_EQUAL(path, "abc\\def\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\", "C:\\"));                                            TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("abc\\def\\", "C:\\xyz"));                                         TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("abc\\def\\", "C:\\uvw\\xyz"));                                    TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\", "C:\\rst\\uvw\\xyz"));                               TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi\\", ""));                                           TEST_EQUAL(path, "abc\\def\\ghi\\");
        TRY(path = merge_paths("abc\\def\\ghi\\", "xyz"));                                        TEST_EQUAL(path, "abc\\def\\ghi\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi\\", "uvw\\xyz"));                                   TEST_EQUAL(path, "abc\\def\\ghi\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi\\", "rst\\uvw\\xyz"));                              TEST_EQUAL(path, "abc\\def\\ghi\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi\\", "C:\\"));                                       TEST_EQUAL(path, "C:\\");
        TRY(path = merge_paths("abc\\def\\ghi\\", "C:\\xyz"));                                    TEST_EQUAL(path, "C:\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi\\", "C:\\uvw\\xyz"));                               TEST_EQUAL(path, "C:\\uvw\\xyz");
        TRY(path = merge_paths("abc\\def\\ghi\\", "C:\\rst\\uvw\\xyz"));                          TEST_EQUAL(path, "C:\\rst\\uvw\\xyz");
        TRY(path = merge_paths("abc", "def", "ghi", "jkl"));                                      TEST_EQUAL(path, "abc\\def\\ghi\\jkl");
        TRY(path = merge_paths("abc\\def", "ghi\\jkl", "mno\\pqr", "stu\\vwx"));                  TEST_EQUAL(path, "abc\\def\\ghi\\jkl\\mno\\pqr\\stu\\vwx");
        TRY(path = merge_paths("abc\\def\\", "ghi\\jkl\\", "mno\\pqr\\", "stu\\vwx\\"));          TEST_EQUAL(path, "abc\\def\\ghi\\jkl\\mno\\pqr\\stu\\vwx\\");
        TRY(path = merge_paths("C:\\abc\\def", "C:\\ghi\\jkl", "C:\\mno\\pqr", "C:\\stu\\vwx"));  TEST_EQUAL(path, "C:\\stu\\vwx");

    #endif

}

void test_rs_graphics_2d_file_system() {

    std::vector<std::string> files;
    std::string str;

    TEST(! file_exists(""));
    TEST(file_exists("."));
    TEST(file_exists(".."));
    TEST(file_exists("../source"));
    TEST(file_exists("../source/CMakeLists.txt"));
    TEST(! file_exists("NoSuchFile"));

    TEST(! file_is_directory(""));
    TEST(file_is_directory("."));
    TEST(file_is_directory(".."));
    TEST(file_is_directory("../source"));
    TEST(! file_is_directory("../source/CMakeLists.txt"));
    TEST(! file_is_directory("NoSuchFile"));

    #ifdef _WIN32
        TEST(file_exists("C:\\"));
        TEST(file_is_directory("C:\\"));
    #else
        TEST(file_exists("/"));
        TEST(file_is_directory("/"));
    #endif

    TRY(files = list_directory("../source"));
    TEST_EQUAL(files.size(), 4u);
    std::sort(files.begin(), files.end());
    str = format_range(files);
    TEST_EQUAL(str, "[CMakeLists.txt,rs-graphics-2d,rs-graphics-2d.hpp,test]");

    TRY(files = list_directory("../source", ListDir::recursive));
    TEST(files.size() >= 30);
    TEST(std::find(files.begin(), files.end(), "CMakeLists.txt") != files.end());
    TEST(std::find(files.begin(), files.end(), "rs-graphics-2d") != files.end());
    TEST(std::find(files.begin(), files.end(), "rs-graphics-2d/file.hpp") != files.end());
    TEST(std::find(files.begin(), files.end(), "rs-graphics-2d/file.cpp") != files.end());
    TEST(std::find(files.begin(), files.end(), "test") != files.end());
    TEST(std::find(files.begin(), files.end(), "test/file-test.cpp") != files.end());
    TEST(std::find(files.begin(), files.end(), "test/unit-test.cpp") != files.end());

    TRY(files = list_directory("."));                         TEST(! files.empty());
    TRY(files = list_directory(".."));                        TEST(! files.empty());
    TRY(files = list_directory("../source/CMakeLists.txt"));  TEST(files.empty());
    TRY(files = list_directory("NoSuchFile"));                TEST(files.empty());

}

void test_rs_graphics_2d_file_binary_io() {

    static const std::string file = "../source/CMakeLists.txt";

    uint8_t u8 = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint64_t u64 = 0;
    int8_t i8 = 0;
    int16_t i16 = 0;
    int32_t i32 = 0;
    int64_t i64 = 0;

    Cstdio io(file, "rb");

    std::rewind(io);  TRY(u8 = read_be<uint8_t>(io));    TEST_EQUAL(u8, 0x63);
    std::rewind(io);  TRY(u16 = read_be<uint16_t>(io));  TEST_EQUAL(u16, 0x636d);
    std::rewind(io);  TRY(u32 = read_be<uint32_t>(io));  TEST_EQUAL(u32, 0x636d'616bul);
    std::rewind(io);  TRY(u64 = read_be<uint64_t>(io));  TEST_EQUAL(u64, 0x636d'616b'655f'6d69ull);
    std::rewind(io);  TRY(i8 = read_be<int8_t>(io));     TEST_EQUAL(i8, 0x63);
    std::rewind(io);  TRY(i16 = read_be<int16_t>(io));   TEST_EQUAL(i16, 0x636d);
    std::rewind(io);  TRY(i32 = read_be<int32_t>(io));   TEST_EQUAL(i32, 0x636d'616bl);
    std::rewind(io);  TRY(i64 = read_be<int64_t>(io));   TEST_EQUAL(i64, 0x636d'616b'655f'6d69ll);
    std::rewind(io);  TRY(u8 = read_le<uint8_t>(io));    TEST_EQUAL(u8, 0x63);
    std::rewind(io);  TRY(u16 = read_le<uint16_t>(io));  TEST_EQUAL(u16, 0x6d63);
    std::rewind(io);  TRY(u32 = read_le<uint32_t>(io));  TEST_EQUAL(u32, 0x6b61'6d63ul);
    std::rewind(io);  TRY(u64 = read_le<uint64_t>(io));  TEST_EQUAL(u64, 0x696d'5f65'6b61'6d63ull);
    std::rewind(io);  TRY(i8 = read_le<int8_t>(io));     TEST_EQUAL(i8, 0x63);
    std::rewind(io);  TRY(i16 = read_le<int16_t>(io));   TEST_EQUAL(i16, 0x6d63);
    std::rewind(io);  TRY(i32 = read_le<int32_t>(io));   TEST_EQUAL(i32, 0x6b61'6d63l);
    std::rewind(io);  TRY(i64 = read_le<int64_t>(io));   TEST_EQUAL(i64, 0x696d'5f65'6b61'6d63ll);

}
