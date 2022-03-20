#include "rs-graphics-2d/image.hpp"
#include "rs-graphics-core/colour.hpp"
#include "rs-unit-test.hpp"
#include "test/vector-test.hpp"

using namespace RS::Graphics::Core;
using namespace RS::Graphics::Plane;

void test_rs_graphics_2d_image_resize() {

    Image8 in8, out8;
    Image16 in16, out16;
    HdrImage in32, out32;
    sImage8 in8s, out8s;
    sImage16 in16s, out16s;
    sHdrImage in32s, out32s;
    PmaImage8 in8sp, out8sp;
    PmaImage16 in16sp, out16sp;
    PmaHdrImage in32sp, out32sp;

    TRY(in8.reset({200, 100}, Rgba8::red()));
    TRY(in16.reset({200, 100}, Rgba16::green()));
    TRY(in32.reset({200, 100}, Rgbaf::blue()));
    TRY(in8s.reset({200, 100}, sRgba8::red()));
    TRY(in16s.reset({200, 100}, sRgba16::green()));
    TRY(in32s.reset({200, 100}, sRgbaf::blue()));
    TRY(in8sp.reset({200, 100}, Rgba8::red()));
    TRY(in16sp.reset({200, 100}, Rgba16::green()));
    TRY(in32sp.reset({200, 100}, Rgbaf::blue()));

    TRY(out8 = in8.resized({100, 50}));
    TRY(out16 = in16.resized({100, 50}));
    TRY(out32 = in32.resized({100, 50}));
    TRY(out8s = in8s.resized({100, 50}));
    TRY(out16s = in16s.resized({100, 50}));
    TRY(out32s = in32s.resized({100, 50}));
    TRY(out8sp = in8sp.resized({100, 50}));
    TRY(out16sp = in16sp.resized({100, 50}));
    TRY(out32sp = in32sp.resized({100, 50}));

    // TODO

}
