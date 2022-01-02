#include "rs-graphics-2d/image.hpp"
#include "rs-graphics-core/colour.hpp"
#include "rs-unit-test.hpp"
#include <string>

using namespace RS::Graphics::Core;
using namespace RS::Graphics::Plane;

void test_rs_graphics_2d_image() {

    Image8 rgb;
    HdrImage hdr;
    // Rgba8 bc;
    // Rgbaf fc;

    TEST_EQUAL(rgb.width(), 0);
    TEST_EQUAL(rgb.height(), 0);
    TEST_EQUAL(rgb.size(), 0u);
    TEST_EQUAL(hdr.width(), 0);
    TEST_EQUAL(hdr.height(), 0);
    TEST_EQUAL(hdr.size(), 0u);

    TRY(rgb.reset(100, 200, Rgba8::red()));
    TRY(hdr.reset(300, 400, Rgbaf::green()));

    TEST_EQUAL(rgb.width(), 100);
    TEST_EQUAL(rgb.height(), 200);
    TEST_EQUAL(rgb.size(), 20'000u);
    TEST_EQUAL(hdr.width(), 300);
    TEST_EQUAL(hdr.height(), 400);
    TEST_EQUAL(hdr.size(), 120'000u);

    TEST_EQUAL(*rgb.top_left(), Rgba8::red());
    TEST_EQUAL(*rgb.top_right(), Rgba8::red());
    TEST_EQUAL(*rgb.bottom_left(), Rgba8::red());
    TEST_EQUAL(*rgb.bottom_right(), Rgba8::red());
    TEST_EQUAL(*hdr.top_left(), Rgbaf::green());
    TEST_EQUAL(*hdr.top_right(), Rgbaf::green());
    TEST_EQUAL(*hdr.bottom_left(), Rgbaf::green());
    TEST_EQUAL(*hdr.bottom_right(), Rgbaf::green());

}
