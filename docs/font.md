# Fonts

_[2D Graphics Library by Ross Smith](index.html)_

```c++
#include "rs-graphics-2d/font.hpp"
namespace RS::Graphics::Plane;
```

## Contents

* TOC
{:toc}

## Font class



<!--

Title: Graphics Library: Font
CSS: style.css

# [Graphics Library](index.html): Font #

By Ross Smith

* `#include "rs-graphics/font.hpp"`
* `namespace RS::Graphics`

## Contents ##

{{TOC}}

## Supporting types ##

* `namespace` **`FontStyle`**
    * `constexpr int FontStyle::`**`regular`** `= 0`
    * `constexpr int FontStyle::`**`bold`** `= 1`
    * `constexpr int FontStyle::`**`italic`** `= 2`
    * `constexpr int FontStyle::`**`fallback`** `= 4`

Style flags used in the `FontMap::find()` function.

## Font class ##

* `class` **`Font`**

A class that represents a single font. This may be loaded from an OTF or TTF
file, or may be one of several fonts loaded from a TTC file.

* `Font::`**`Font`**`() noexcept`

The default constructor creates a null font with no name or glyphs.

* `explicit Font::`**`Font`**`(const std::string& filename, int index = 0)`

Load a font from a file (OTF, TTF, or TTC). The index indicates which font to
load from a TTC file. This will create a null font if the index is out of
range for a TTC file, or if it is anything other than zero for an OTF or TTF
file.

* `Font::`**`Font`**`(const Font& f) noexcept`
* `Font::`**`Font`**`(Font&& f) noexcept`
* `virtual Font::`**`~Font`**`() noexcept`
* `Font& Font::`**`operator=`**`(const Font& f) noexcept`
* `Font& Font::`**`operator=`**`(Font&& f) noexcept`

Other life cycle functions.

* `Font::`**`operator bool`**`() const noexcept`

True if the font is not null (i.e. a font was successfully loaded from a
file).

* `std::string Font::`**`family`**`() const`
* `std::string Font::`**`subfamily`**`() const`
* `std::string Font::`**`name`**`() const`

These return the family and subfamily names, or the full name of the font.

* `bool Font::`**`has_glyph`**`(char32_t c) const noexcept`
* `bool Font::`**`has_glyphs`**`(char32_t first, char32_t last) const noexcept`
* `template <typename Range> bool Font::`**`has_glyphs`**`(const Range& range) const`

These report whether or not a glyph, or a range of glyphs, is available in the
font. A glyph range can be supplied either as the first and last glyph in a
range (inclusive), or as an explicit list. The `has_glyphs()` functions will
return true only if every glyph in the range is present. All of these
functions will return false if any of the characters being queried is not a
valid Unicode scalar value.

* `static std::vector<Font> Font::`**`load`**`(const std::string& filename)`

Loads all of the available fonts from a TTC file. For OTF and TTF files that
have only one font, this just calls the `Font` constructor.

## ScaledFont class ##

* `class` **`ScaledFont`**`: public Font`

This class represents a font scaled to a particular size.

* `ScaledFont::`**`ScaledFont`**`() noexcept`

The default constructor creates a null font with no name, glyphs, or metrics.

* `ScaledFont::`**`ScaledFont`**`(const Font& font, int scale) noexcept`
* `ScaledFont::`**`ScaledFont`**`(const Font& font, Int2 scale) noexcept`

Construct a `ScaledFont` from a `Font` and a scale factor. The scale factor
indicates the scaled font's em size in pixels. This can be a single value, or
separate values for the X and Y scales. Behaviour is undefined if either scale
factor is less than or equal to zero.

* `ScaledFont::`**`ScaledFont`**`(const ScaledFont& sf) noexcept`
* `ScaledFont::`**`ScaledFont`**`(ScaledFont&& sf) noexcept`
* `virtual ScaledFont::`**`~ScaledFont`**`() noexcept`
* `ScaledFont& ScaledFont::`**`operator=`**`(const ScaledFont& sf) noexcept`
* `ScaledFont& ScaledFont::`**`operator=`**`(ScaledFont&& sf) noexcept`

Other life cycle functions.

* `Int2 ScaledFont::`**`scale`**`() const noexcept`

Returns the font scale (the em size in pixels).

* `int ScaledFont::`**`ascent`**`() const noexcept`
* `int ScaledFont::`**`descent`**`() const noexcept`
* `int ScaledFont::`**`line_gap`**`() const noexcept`
* `int ScaledFont::`**`line_offset`**`() const noexcept`

These return font metrics. The ascent is the maximum distance from the
baseline to the top of a glyph. The descent is the maximum distance from the
baseline to the bottom of a glyph, and is normally negative. The line gap is
the extra distance between lines, beyond what is implied by the ascent and
descent. The line offset is the distance between the baselines of successive
lines, and is equal to `ascent()-descent()+line_gap()`. All measures are in
pixels.

* `template <typename C> bool ScaledFont::`**`render`**`(Image<C>& image, Int2& offset, std::string_view text,
    int line_shift = 0, C text_colour = C::black(), C background = [see below]) const`

This function renders text to a new image. Multiple lines of text are
supported. The supplied `image` object will be reset to the minimum size
required to contain the rendered text. The `offset` vector will be set to the
position of the image's top left corner relative to the initial reference
point (beginning of the baseline of the first line of text).

If a `line_shift` is supplied, it will be added to the normal vertical spacing
between lines, in pixels (this can be negative if you want lines to be closer
together than usual).

The text colour defaults to black; the background colour defaults to
transparent if an alpha channel is present, otherwise white.

This will return true if the rendering was successful, false if an error
prevented rendering (either the font is null or the text contains invalid
UTF-8). It will still return true if nothing was rendered because the text is
empty or contains only whitespace characters.

This will fail to compile if the colour space is `sRGB`, which does not allow
linear compositing.

* `template <typename C> bool ScaledFont::`**`render_to`**`(WriteImage<C> image, Int2 ref_point, std::string_view text,
    int line_shift = 0, C text_colour = C::black()) const`

This function renders text to an existing image. Multiple lines of text are
supported. The supplied `ref_point` is used as the beginning of the baseline
of the first line of text (it should be at least `ascent()` pixels from the
top of the image to avoid clipping). Any part of the text that falls outside
the bounds of the image will be clipped.

If a `line_shift` is supplied, it will be added to the normal vertical spacing
between lines, in pixels (this can be negative if you want lines to be closer
together than usual).

The text colour defaults to black. This will be alpha blended with the
existing pixels of the image.

This will return true if the rendering was successful, false if an error
prevented rendering (either the font is null or the text contains invalid
UTF-8). It will still return true if nothing was rendered because the text is
empty or contains only whitespace characters, or the rendered glyphs fall
entirely outside the image.

This will fail to compile if the colour space is `sRGB` or the colour type has
no alpha channel.

* `Box_i2 ScaledFont::`**`text_box`**`(std::string_view text, int line_shift = 0) const`

Returns the bounding box of the supplied text, with the origin at the
beginning of the baseline of the first line of text.

If a `line_shift` is supplied, it will be added to the normal vertical spacing
between lines, in pixels (this can be negative if you want lines to be closer
together than usual).

This will return an empty box if the call fails because the font is null, the
text is empty, the text contains no non-whitespace characters, or the text
contains invalid UTF-8.

* `size_t ScaledFont::`**`text_fit`**`(std::string_view text, size_t max_pixels) const`

Indicates how much of the given single line of text will fit into a space of
the given width (`max_pixels`). This will return zero if the font is null, the
text contains invalid UTF-8, or the text contains any line feed characters. It
will return `npos` if the entire text will fit. Otherwise, it returns the
length of the longest substring that will fit into the space, in bytes (this
will always point to a UTF-8 character boundary).

* `int ScaledFont::`**`text_wrap`**`(std::string_view text_in, std::string& text_out, size_t max_pixels) const`

Wrap text at word boundaries (determined by ASCII whitespace) to fit into the
given pixel width. Leading and trailing line feeds are stripped from the input
text, but internal line breaks will be retained.

The return value is the number of lines in the wrapped text, or -1 if the text
wrapping failed because the font is null or the input text contained a word
too long to fit in the allowed space. If -1 is returned, `text_out` will be
set to an empty string.

Behaviour is undefined if `text_in` and `text_out` are the same string.

## FontMap class ##

* `class` **`FontMap`**

A `FontMap` contains information about a set of fonts, typically all the fonts
in a given set of directories. Fonts are organised by family and subfamily
(the subfamily usually represents variations such as bold or italic).

All queries based on family or subfamily names are case insensitive for ASCII
characters.

* `FontMap::`**`FontMap`**`()`
* `FontMap::`**`FontMap`**`(const FontMap& fm) noexcept`
* `FontMap::`**`FontMap`**`(FontMap&& fm) noexcept`
* `FontMap::`**`~FontMap`**`() noexcept`
* `FontMap& FontMap::`**`operator=`**`(const FontMap& fm) noexcept`
* `FontMap& FontMap::`**`operator=`**`(FontMap&& fm) noexcept`

Life cycle functions.

* `void FontMap::`**`clear`**`() noexcept`

Clears all fonts from the `FontMap`.

* `bool FontMap::`**`contains`**`(const std::string& family) const noexcept`
* `bool FontMap::`**`contains`**`(const std::string& family, const std::string& subfamily) const noexcept`

True if the `FontMap` contains any fonts matching the given names.

* `bool FontMap::`**`empty`**`() const noexcept`

True if the `FontMap` contains no fonts (equivalent to `num_families()==0`).

* `std::vector<std::string> FontMap::`**`families`**`() const`

Returns a list of all font family names.

* `std::vector<std::string> FontMap::`**`subfamilies`**`(const std::string& family) const`

Returns a list of all subfamily names for the given family. This will return
an empty list if `contains(family)` is false.

* `Font FontMap::`**`find`**`(const std::vector<std::string>& families, int style = FontStyle::regular) const`

Loads the first matching font in the `FontMap`. This will first search the
family names, in the order given, for a font that matches the given style. If
this is not found, it may run through the family names again and return the
first font of any kind it finds; by default this fallback search is done only
if the style requested is regular. If the `FontStyle::fallback` option is
combined with one or both of the other style options, a fallback will be
accepted for those too.

* `Font FontMap::`**`load`**`(const std::string& family, const std::string& subfamily) const`

Loads the font with the given family and subfamily name. This will return a
null font if no matching font is present in the `FontMap`.

* `size_t FontMap::`**`num_families`**`() const noexcept`

Returns the number of font families in the `FontMap`.

* `void FontMap::`**`search`**`(const std::string& dir, int flags = 0)`

Searches the given directory for all readable font files. This will do nothing
if `dir` does not exist, or exists but is not a directory. Searches are
cumulative; previously loaded fonts are not cleared before a search.

The `flags` argument is a combination of `ListDir` bitflags from the
[utilities module](utility.html). If the `ListDir::recursive` flag is set,
subdirectories will be searched recursively. By default, symlinks to
directories are not followed; if the `ListDir::symlinks` flag is also present,
symlinks will be followed. The `symlinks` flag has no effect if not combined
with `recursive`; symlinks to font files will be opened regardless.

* `void FontMap::`**`search_system`**`()`

Searches the standard font directories for the operating system (this can take
several seconds if you have a lot of fonts).

* **Apple**
    * `/System/Library/Fonts`
    * `/Library/Fonts`
    * `~/Library/Fonts`
* **Linux** (and generic Unix)
    * `/usr/X11R6/lib/X11/fonts`
    * `/usr/share/fonts`
    * `/usr/local/share/fonts`
    * `~/.fonts`
    * `~/.local/share/fonts`
* **Windows**
    * `%WINDIR%\Fonts`

-->
