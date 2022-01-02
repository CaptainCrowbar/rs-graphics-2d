#include "rs-graphics-2d/image.hpp"
#include "rs-unit-test.hpp"
#include <string>

using namespace RS::Graphics::Core;
using namespace RS::Graphics::Plane;

void test_rs_graphics_2d_image() {

    std::string s;

    TRY(s = thing());
    TEST_EQUAL(s, "Hello world");

}
