#include "rs-graphics-2d/version.hpp"
#include "rs-unit-test.hpp"

using namespace RS::Graphics::Plane;

void test_rs_graphics_2d_version() {

    TEST_EQUAL(version()[0], 0);
    TEST_IN_RANGE(version()[1], 0, 100);
    TEST_IN_RANGE(version()[2], 0, 1000);
    TEST_MATCH(version_string(), R"(0\.\d+\.\d+)");

}
