// This file is generated by the rs-update-tests script

#include "rs-unit-test.hpp"

int main(int argc, char** argv) {

    RS::UnitTest::begin_tests(argc, argv);

    // version-test.cpp
    UNIT_TEST(rs_graphics_2d_version)

    // image-test.cpp
    UNIT_TEST(rs_graphics_2d_image_construction)
    UNIT_TEST(rs_graphics_2d_image_pixel_access)
    UNIT_TEST(rs_graphics_2d_image_premultiplied_alpha)

    // unit-test.cpp

    return RS::UnitTest::end_tests();

}
