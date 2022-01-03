# Thing

_[2D Graphics Library by Ross Smith](index.html)_

```c++
#include "rs-graphics-2d/image.hpp"
namespace RS::Graphics::Plane;
```

## Contents

* TOC
{:toc}

## Supporting types

```c++
using Point = Core::Int2;
```

Used for pixel coordinates.

```c++
namespace ImageFlags {
    constexpr int bottom_up;
    constexpr int premultiplied;
};
```

Bitmask flags indicating properties of an image. Images are normally stored
top down; they will be stored bottom up if the `bottom_up` flag is used.
Pixel data will be assumed to include premultiplied alpha if the
`premultiplied` flag is used.

```c++
class ImageIoError:
public std::runtime_error {
    const std::string& filename() const noexcept;
};
```

This is thrown when one of the image I/O functions encounters an error. The
name of the offending file will be supplied if possible.

```c++
struct ImageInfo {
    Point shape = Point::null();
    int channels = 0;
    int bits_per_channel = 0;
    bool has_alpha = false;
    bool is_hdr = false;
    explicit operator bool() const noexcept;
    std::string str() const;
};
std::ostream& operator<<(std::ostream& out, const ImageInfo& info);
```

Information about an image file. The boolean operator is true if the shape is
not a null vector, indicating that a file has been successfully queried.

## Image class

```c++
template <typename Colour, int Flags = 0> class Image;
```

The image class. This is only defined when `Colour` is an instantiation of
`Core::Colour`.

### Type aliases

```c++
using Image8 = Image<Core::Rgba8>;
using Image16 = Image<Core::Rgba16>;
using HdrImage = Image<Core::Rgbaf>;
using sImage8 = Image<Core::sRgba8>;
using sImage16 = Image<Core::sRgba16>;
using sHdrImage = Image<Core::sRgbaf>;
using PmaImage8 = Image<Core::Rgba8, ImageFlags::premultiplied>;
using PmaImage16 = Image<Core::Rgba16, ImageFlags::premultiplied>;
using PmaHdrImage = Image<Core::Rgbaf, ImageFlags::premultiplied>;
```

Some common image formats.

### Member types

```c++
class Image::iterator {
    iterator& move(int axis, int distance = 1) noexcept;
    Point pos() const noexcept;
};
class Image::const_iterator {
    const_iterator& move(int axis, int distance = 1) noexcept;
    Point pos() const noexcept;
};
```

Forward iterators over the image's pixels. Iterators have two additional
member functions:

* `move()` moves the iterator along the given axis by the given number of pixels.
    The axis is 0 for x, 1 for y; you can also use 'x' and 'y'.
* `pos()` returns the position vector corresponding to the iterator's current position.

Iterators can be considered to exist in an infinite plane (bounded in practise
by the range of an `int`); moving an iterator outside the bounds of the image
using `move()` is safe. Behaviour is undefined if any of `operator*()`,
`operator++()`, `pos()`, or the comparison operators are called on an
off-image iterator.

```c++
using Image::channel_type = Colour::value_type;
using Image::colour_space = Colour::colour_space;
using Image::colour_type = Colour;
```

Properties of the pixel type.

### Constants

```c++
static constexpr int channels = Colour::channels;
static constexpr Core::ColourLayout colour_layout = CL;
static constexpr bool has_alpha = Colour::has_alpha;
static constexpr bool is_hdr = Colour::is_hdr;
```

Properties of the pixel type.

```c++
static constexpr bool is_bottom_up;
static constexpr bool is_top_down;
```

Indicate whether the image is laid out top-down or bottom-up in memory.

```c++
static constexpr bool is_premultiplied;
```

True if the image uses premultiplied alpha.

### Life cycle functions

```c++
Image::Image() noexcept;
```

The default constructor creates an empty image with zero width and height.

```c++
explicit Image::Image(Point shape);
Image::Image(int w, int h);
```

Create an image with the specified dimensions. The image data is
uninitialized.

```c++
Image::Image(Point shape, Colour c);
Image::Image(int w, int h, Colour c);
```

Create an image with the specified dimensions, setting all pixels to the same
colour.

```c++
Image::Image(const Image& img);
Image::Image(Image&& img) noexcept;
Image::~Image() noexcept;
Image& Image::operator=(const Image& img);
Image& Image::operator=(Image&& img) noexcept;
```

Other life cycle functions.

### Pixel access functions

```c++
Colour& Image::operator[](Point p) noexcept;
const Colour& Image::operator[](Point p) const noexcept;
Colour& Image::operator()(int x, int y) noexcept;
const Colour& Image::operator()(int x, int y) const noexcept;
```

Reference to a specific pixel. Behaviour is undefined if the coordinates are
out of bounds.

```c++
iterator Image::begin() noexcept;
const_iterator Image::begin() const noexcept;
iterator Image::end() noexcept;
const_iterator Image::end() const noexcept;
```

Iterators over the image's pixels. The `begin()` iterator is the same pixel
returned by `locate(0,0)`.

```c++
iterator Image::bottom_left() noexcept;
const_iterator Image::bottom_left() const noexcept;
iterator Image::bottom_right() noexcept;
const_iterator Image::bottom_right() const noexcept;
iterator Image::top_left() noexcept;
const_iterator Image::top_left() const noexcept;
iterator Image::top_right() noexcept;
const_iterator Image::top_right() const noexcept;
```

These return iterators pointing to the pixel at one corner of the image.
Behaviour is undefined if the image is empty.

```c++
T* Image::data() noexcept;
const T* Image::data() const noexcept;
```

Pointers to the image data.

```c++
iterator Image::locate(Point p) noexcept;
const_iterator Image::locate(Point p) const noexcept;
iterator Image::locate(int x, int y) noexcept;
const_iterator Image::locate(int x, int y) const noexcept;
```

These return an iterator pointing to the pixel at the specified coordinates.
The iterator returned by `locate(0,0)` will match either `top_left()` or
`bottom_left()`, depending on whether the image is stored top-down or
bottom-up. Behaviour is undefined if the coordinates are out of bounds.

### Conversion functions

```c++
Image<Colour, [modified flags]> Image::multiply_alpha() const;
Image<Colour, [modified flags]> Image::unmultiply_alpha() const;
```

Convert a non-premultiplied image into a premultiplied-alpha image, or vice
versa. The returned image type has the opposite premultiplication flag. These
are only defined if `Colour::can_premultiply` is true.

```c++
template <typename C1, int F1, typename C2, int F2>
    void convert_image(const Image<C1, F1>& in, Image<C2, F2>& out);
```

Convert an image from one format to another.

### I/O functions

The current implementation uses
[Sean Barrett's STB library](https://github.com/nothings/stb)
for image I/O.

```c++
void Image::load(const std::string& filename);
```

Load an image from a file. Supported image types are BMP, GIF, HDR/RGBE, JPEG,
PIC, PNG, PNM, PSD, and TGA (not all features are supported for some
formats). Input channel data can be 32-bit floating point for HDR/RGBE, 8-bit
or 16-bit integer for all other formats. This will throw `ImageIoError` if
the operation fails (this may mean that the file was not found, that it was
not in a supported format, or that the image was too big for the STB library
to load; the size limit is 1-2 GB depending on format).

```c++
void Image::save(const std::string& filename, int quality = 90) const;
```

Save an image to a file. The image format is deduced from the file name.
Supported formats are BMP, HDR/RGBE, JPEG, PNG, and TGA. This will throw
`ImageIoError` if the image format is not supported or an I/O error is
encountered.

```c++
ImageInfo query_image(const std::string& filename) noexcept;
```

Queries an image file for information about the stored image. File formats
supported are the same as for `Image::load()`. This will return a null
`ImageInfo` if the file is not found or is not in a recognisable format.

### Other member functions

```c++
void Image::clear() noexcept;
```

Resets the `Image` object to an empty image. Equivalent to `reset(0,0)`.

```c++
bool Image::empty() const noexcept;
```

True if the image is empty (both dimensions are zero).

```c++
void Image::fill(Colour c) noexcept;
```

Fills all pixels with a uniform colour.

```c++
void Image::reset(Point shape);
void Image::reset(Point shape, Colour c);
void Image::reset(int w, int h);
void Image::reset(int w, int h, Colour c);
```

Changes the image shape to the specified dimensions. If a colour is supplied,
the new image will be filled with that colour; otherwise, the pixel data is
left uninitialized.

```c++
Point Image::shape() const noexcept;
int Image::width() const noexcept;
int Image::height() const noexcept;
```

Query the dimensions of the image.

```c++
size_t Image::size() const noexcept;
size_t Image::bytes() const noexcept;
```

The size of the image, in pixels or bytes.

```c++
void Image::swap(Image& img) noexcept;
void swap(Image& a, Image& b) noexcept;
```

Swap two images.

### Comparison operators

```c++
bool operator==(const Image& a, const Image& b) noexcept;
bool operator!=(const Image& a, const Image& b) noexcept;
```

Comparison operators. These perform a full comparison on the pixel data of the
images.
