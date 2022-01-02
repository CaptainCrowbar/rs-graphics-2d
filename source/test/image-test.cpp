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

    Image8 rgb1, rgb2;
    HdrImage hdr1, hdr2;
    PmaImage8 prgb;
    PmaHdrImage phdr;

    Rgba8 bc1 = {50,100,150,200};
    Rgba8 bc2 = {39,78,118,200};
    Rgbaf fc1 = {0.2,0.4,0.6,0.8};
    Rgbaf fc2 = {0.16,0.32,0.48,0.8};

    TRY(rgb1.reset(100, 200, bc1));
    TRY(hdr1.reset(300, 400, fc1));

    TRY(prgb = rgb1.premultiply());
    TRY(phdr = hdr1.premultiply());

    TEST_VECTORS(*prgb.top_left(),      bc2, 0);
    TEST_VECTORS(*prgb.top_right(),     bc2, 0);
    TEST_VECTORS(*prgb.bottom_left(),   bc2, 0);
    TEST_VECTORS(*prgb.bottom_right(),  bc2, 0);
    TEST_VECTORS(*phdr.top_left(),      fc2, 1e-5);
    TEST_VECTORS(*phdr.top_right(),     fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_left(),   fc2, 1e-5);
    TEST_VECTORS(*phdr.bottom_right(),  fc2, 1e-5);

    TRY(rgb2 = prgb.unmultiply());
    TRY(hdr2 = phdr.unmultiply());

    TEST_VECTORS(*rgb2.top_left(),      bc1, 1);
    TEST_VECTORS(*rgb2.top_right(),     bc1, 1);
    TEST_VECTORS(*rgb2.bottom_left(),   bc1, 1);
    TEST_VECTORS(*rgb2.bottom_right(),  bc1, 1);
    TEST_VECTORS(*hdr2.top_left(),      fc1, 1e-5);
    TEST_VECTORS(*hdr2.top_right(),     fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_left(),   fc1, 1e-5);
    TEST_VECTORS(*hdr2.bottom_right(),  fc1, 1e-5);

}
