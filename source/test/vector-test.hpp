#pragma once

#include "rs-unit-test.hpp"
#include <cmath>
#include <cstdlib>

// Check that every element of two vectors matches within epsilon

#define TEST_VECTORS(lhs, rhs, epsilon) \
    try { \
        auto rs_unit_test_lhs = lhs; \
        auto rs_unit_test_rhs = rhs; \
        REQUIRE(rs_unit_test_lhs.size() == rs_unit_test_rhs.size()); \
        auto rs_unit_test_epsilon = double(epsilon); \
        for (size_t i = 0; i < rs_unit_test_lhs.size(); ++i) { \
            if (std::abs(double(rs_unit_test_rhs[i] - rs_unit_test_lhs[i])) > rs_unit_test_epsilon) { \
                FAIL_TEST("Vectors are not close enough: " \
                    << # lhs << " = " << rs_unit_test_lhs << ", " \
                    << # rhs << " = " << rs_unit_test_rhs << ", " \
                    << "epsilon = " << rs_unit_test_epsilon); \
                break; \
            } \
        } \
    } \
    catch (const std::exception& ex) { \
        FAIL_TEST("Unexpected exception: " << ex.what()); \
    } \
    catch (...) { \
        FAIL_TEST("Unexpected exception"); \
    }

// HSL/HSV colours need special treatment because the hue can go haywire if the saturation is tiny

#define TEST_VECTORS_HSPACE(lhs, rhs, epsilon) \
    try { \
        auto rs_unit_test_lhs = lhs; \
        auto rs_unit_test_rhs = rhs; \
        REQUIRE(rs_unit_test_lhs.size() == rs_unit_test_rhs.size()); \
        auto rs_unit_test_epsilon = double(epsilon); \
        bool rs_unit_test_unsaturated = rs_unit_test_lhs[1] < epsilon && rs_unit_test_rhs[1] < epsilon; \
        for (auto i = size_t(rs_unit_test_unsaturated); i < rs_unit_test_lhs.size(); ++i) { \
            if (std::abs(double(rs_unit_test_rhs[i] - rs_unit_test_lhs[i])) > rs_unit_test_epsilon) { \
                FAIL_TEST("Vectors are not close enough: " \
                    << # lhs << " = " << rs_unit_test_lhs << ", " \
                    << # rhs << " = " << rs_unit_test_rhs << ", " \
                    << "epsilon = " << rs_unit_test_epsilon); \
                break; \
            } \
        } \
    } \
    catch (const std::exception& ex) { \
        FAIL_TEST("Unexpected exception: " << ex.what()); \
    } \
    catch (...) { \
        FAIL_TEST("Unexpected exception"); \
    }
