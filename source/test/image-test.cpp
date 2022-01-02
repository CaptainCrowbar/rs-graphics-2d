#include "rs-graphics-2d/image.hpp"
#include "rs-graphics-core/colour.hpp"
#include "rs-unit-test.hpp"
#include "test/vector-test.hpp"
#include <string>

using namespace RS::Graphics::Core;
using namespace RS::Graphics::Plane;

void test_rs_graphics_2d_image_construction() {

    Image8 rgb;
    HdrImage hdr;

    TEST_EQUAL(rgb.width(),   0);
    TEST_EQUAL(rgb.height(),  0);
    TEST_EQUAL(rgb.size(),    0u);
    TEST_EQUAL(hdr.width(),   0);
    TEST_EQUAL(hdr.height(),  0);
    TEST_EQUAL(hdr.size(),    0u);

    TRY(rgb.reset(100, 200));
    TRY(hdr.reset(300, 400));

    TEST_EQUAL(rgb.width(),   100);
    TEST_EQUAL(rgb.height(),  200);
    TEST_EQUAL(rgb.size(),    20'000u);
    TEST_EQUAL(hdr.width(),   300);
    TEST_EQUAL(hdr.height(),  400);
    TEST_EQUAL(hdr.size(),    120'000u);

}

void test_rs_graphics_2d_image_pixel_access() {

    Image8 rgb;
    HdrImage hdr;

    TRY(rgb.reset(100, 200, Rgba8::red()));
    TRY(hdr.reset(300, 400, Rgbaf::green()));

    TEST_EQUAL(*rgb.top_left(),      Rgba8::red());
    TEST_EQUAL(*rgb.top_right(),     Rgba8::red());
    TEST_EQUAL(*rgb.bottom_left(),   Rgba8::red());
    TEST_EQUAL(*rgb.bottom_right(),  Rgba8::red());
    TEST_EQUAL(*hdr.top_left(),      Rgbaf::green());
    TEST_EQUAL(*hdr.top_right(),     Rgbaf::green());
    TEST_EQUAL(*hdr.bottom_left(),   Rgbaf::green());
    TEST_EQUAL(*hdr.bottom_right(),  Rgbaf::green());

    TRY(*rgb.top_left()      = Rgba8::yellow());
    TRY(*rgb.top_right()     = Rgba8::green());
    TRY(*rgb.bottom_left()   = Rgba8::blue());
    TRY(*rgb.bottom_right()  = Rgba8::cyan());
    TRY(*hdr.top_left()      = Rgbaf::magenta());
    TRY(*hdr.top_right()     = Rgbaf::red());
    TRY(*hdr.bottom_left()   = Rgbaf::yellow());
    TRY(*hdr.bottom_right()  = Rgbaf::blue());

    TEST_EQUAL(*rgb.top_left(),      Rgba8::yellow());
    TEST_EQUAL(*rgb.top_right(),     Rgba8::green());
    TEST_EQUAL(*rgb.bottom_left(),   Rgba8::blue());
    TEST_EQUAL(*rgb.bottom_right(),  Rgba8::cyan());
    TEST_EQUAL(*hdr.top_left(),      Rgbaf::magenta());
    TEST_EQUAL(*hdr.top_right(),     Rgbaf::red());
    TEST_EQUAL(*hdr.bottom_left(),   Rgbaf::yellow());
    TEST_EQUAL(*hdr.bottom_right(),  Rgbaf::blue());

    TEST_EQUAL(rgb(0, 0),      Rgba8::yellow());
    TEST_EQUAL(rgb(99, 0),     Rgba8::green());
    TEST_EQUAL(rgb(0, 199),    Rgba8::blue());
    TEST_EQUAL(rgb(99, 199),   Rgba8::cyan());
    TEST_EQUAL(hdr(0, 0),      Rgbaf::magenta());
    TEST_EQUAL(hdr(299, 0),    Rgbaf::red());
    TEST_EQUAL(hdr(0, 399),    Rgbaf::yellow());
    TEST_EQUAL(hdr(299, 399),  Rgbaf::blue());

}

void test_rs_graphics_2d_image_premultiplied_alpha() {

    static const Rgba8 bc1 = {50,100,150,200};
    static const Rgba8 bc2 = {39,78,118,200};
    static const Rgbaf fc1 = {0.2,0.4,0.6,0.8};
    static const Rgbaf fc2 = {0.16,0.32,0.48,0.8};

    Image8 rgb1, rgb2;
    HdrImage hdr1, hdr2;
    PmaImage8 prgb;
    PmaHdrImage phdr;

    TRY(rgb1.reset(100, 200, bc1));
    TRY(hdr1.reset(300, 400, fc1));

    TRY(prgb = rgb1.multiply_alpha());
    TRY(phdr = hdr1.multiply_alpha());

    TEST_VECTORS(*prgb.top_left(),      bc2, 0);
    TEST_VECTORS(*prgb.top_right(),     bc2, 0);
    TEST_VECTORS(*prgb.bottom_left(),   bc2, 0);
    TEST_VECTORS(*prgb.bottom_right(),  bc2, 0);
    TEST_VECTORS(*phdr.top_left(),      fc2, 1e-5);
    TEST_VECTORS(*phdr.top_right(),     fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_left(),   fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_right(),  fc2, 1e-5);

    TRY(rgb2 = prgb.unmultiply_alpha());
    TRY(hdr2 = phdr.unmultiply_alpha());

    TEST_VECTORS(*rgb2.top_left(),      bc1, 1);
    TEST_VECTORS(*rgb2.top_right(),     bc1, 1);
    TEST_VECTORS(*rgb2.bottom_left(),   bc1, 1);
    TEST_VECTORS(*rgb2.bottom_right(),  bc1, 1);
    TEST_VECTORS(*hdr2.top_left(),      fc1, 1e-5);
    TEST_VECTORS(*hdr2.top_right(),     fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_left(),   fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_right(),  fc1, 1e-5);

}

void test_rs_graphics_2d_image_conversion() {

    static const Rgba8 bc1 = {50,100,150,200};
    static const Rgba8 bc2 = {39,78,118,200};
    static const Rgbaf fc1 = {0.2,0.4,0.6,0.8};
    static const Rgbaf fc2 = {0.16,0.32,0.48,0.8};

    Image8 rgb1, rgb2;
    HdrImage hdr1, hdr2;
    PmaImage8 prgb;
    PmaHdrImage phdr;
    Image<Rgba8, ImageFlags::bottom_up> burgb;
    Image<Rgbaf, ImageFlags::bottom_up> buhdr;

    TRY(rgb1.reset(20, 20, bc1));
    TRY(hdr1.reset(20, 20, fc1));

    TRY(convert_image(rgb1, prgb));
    TRY(convert_image(hdr1, phdr));

    TEST_VECTORS(*prgb.top_left(),      bc2, 0);
    TEST_VECTORS(*prgb.top_right(),     bc2, 0);
    TEST_VECTORS(*prgb.bottom_left(),   bc2, 0);
    TEST_VECTORS(*prgb.bottom_right(),  bc2, 0);
    TEST_VECTORS(*phdr.top_left(),      fc2, 1e-5);
    TEST_VECTORS(*phdr.top_right(),     fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_left(),   fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_right(),  fc2, 1e-5);

    TRY(convert_image(prgb, rgb2));
    TRY(convert_image(phdr, hdr2));

    TEST_VECTORS(*rgb2.top_left(),      bc1, 1);
    TEST_VECTORS(*rgb2.top_right(),     bc1, 1);
    TEST_VECTORS(*rgb2.bottom_left(),   bc1, 1);
    TEST_VECTORS(*rgb2.bottom_right(),  bc1, 1);
    TEST_VECTORS(*hdr2.top_left(),      fc1, 1e-5);
    TEST_VECTORS(*hdr2.top_right(),     fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_left(),   fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_right(),  fc1, 1e-5);

    TRY(convert_image(rgb1, rgb2));
    TRY(convert_image(hdr1, hdr2));

    TEST_VECTORS(*rgb2.top_left(),      bc1, 0);
    TEST_VECTORS(*rgb2.top_right(),     bc1, 0);
    TEST_VECTORS(*rgb2.bottom_left(),   bc1, 0);
    TEST_VECTORS(*rgb2.bottom_right(),  bc1, 0);
    TEST_VECTORS(*hdr2.top_left(),      fc1, 1e-5);
    TEST_VECTORS(*hdr2.top_right(),     fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_left(),   fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_right(),  fc1, 1e-5);

    TRY(convert_image(rgb2, prgb));
    TRY(convert_image(hdr2, phdr));

    TEST_VECTORS(*prgb.top_left(),      bc2, 0);
    TEST_VECTORS(*prgb.top_right(),     bc2, 0);
    TEST_VECTORS(*prgb.bottom_left(),   bc2, 0);
    TEST_VECTORS(*prgb.bottom_right(),  bc2, 0);
    TEST_VECTORS(*phdr.top_left(),      fc2, 1e-5);
    TEST_VECTORS(*phdr.top_right(),     fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_left(),   fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_right(),  fc2, 1e-5);

}

void test_rs_graphics_2d_image_file_info() {

    static const std::string test_dir = "../source/test/";
    static const std::string png_file = test_dir + "test-image.png";
    static const std::string jpg_file = test_dir + "test-image.jpg";
    static const std::string no_such_file = test_dir + "no-such-file.png";

    ImageInfo info;
    std::string s;

    TRY(info = query_image(png_file));
    TEST(info);
    TEST_EQUAL(info.shape.x(), 20);
    TEST_EQUAL(info.shape.y(), 20);
    TEST_EQUAL(info.channels, 4);
    TEST_EQUAL(info.bits_per_channel, 8);
    TEST(info.has_alpha);
    TEST(! info.is_hdr);
    TRY(s = info.str());
    TEST_EQUAL(s, "Image:20x20,4ch@8bit,alpha");

    TRY(info = query_image(jpg_file));
    TEST(info);
    TEST_EQUAL(info.shape.x(), 20);
    TEST_EQUAL(info.shape.y(), 20);
    TEST_EQUAL(info.channels, 3);
    TEST_EQUAL(info.bits_per_channel, 8);
    TEST(! info.has_alpha);
    TEST(! info.is_hdr);
    TRY(s = info.str());
    TEST_EQUAL(s, "Image:20x20,3ch@8bit");

    TRY(info = query_image(no_such_file));
    TEST(! info);

}
