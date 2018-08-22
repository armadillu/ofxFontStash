# ofxFontStash: Dynamic font glyph cache for OpenGL

[![Build Status](https://travis-ci.org/armadillu/ofxFontStash.svg?branch=master)](https://travis-ci.org/armadillu/ofxFontStash)
[![Build status](https://ci.appveyor.com/api/projects/status/0cne779u0mdp8mvp/branch/master?svg=true)](https://ci.appveyor.com/project/armadillu/ofxremoteui/branch/master)

Quick and simple Addon wrapping [Font-Stash](https://github.com/akrinke/Font-Stash). 

"Font Stash enables easy string rendering in OpenGL applications. It supports truetype fonts and UTF-8 encoded localized strings. All glyphs are cached in OpenGL texture atlases. Font rasterization is done using Sean Barrett’s stb_truetype.h."

Main Features:

* Loads TTF (sorry, no OFT!)
* Much faster than ofTrueTypeFont
* Draw Unicode strings
* Mip mapped font textures, scale down your text and it still looks ok
* Retina - render the tex atlases at higher internal DPI - nice with mipmaps!
* Kerning, line height and intra-character spacing
* Flows paragraphs in boxes, allowing you to limit the number of lines
* Shitty & confusing api (sorry, I add new stuff when I need it!)

I made tiny modifications to FontStash to render text flipped vertically, to play nicer in the OpenFrameworks universe in general. It will also report every time a new texture is created. Other modifications have been made to support mipmaps, character spacing, kerning, and retina support.

ofxFontStash supports basic one-line text drawing, lineHeight, multiline text rendering based on '\n', and also word wraping to draw a paragraph constrained to a certain width. It also offers bbox calculations on all the drawing options. ~~It requires a particular branch of [ofxUnicode](https://github.com/local-projects/ofxUnicode/tree/ofxFontStash).~~

I made some further modifications to fontstash to allow the creation of mipmaps, and to enable the use of the font-specified kerning information, so spacing between character pairs is shown as its designer intended.

ofxFontStash is quite [fast](http://www.flickr.com/photos/armadillu/7268071284/sizes/o/in/photostream/) at drawing text. If you need to draw a left-aligned text column of a certain width, ofxFontStash can be [~10x faster](http://www.flickr.com/photos/armadillu/9574047566/sizes/o/in/photostream/) than ofxFTGL.

![ofxFontStash screenshot](http://farm8.staticflickr.com/7421/9573999560_aa8c876d15_o.png)

Font Stash was originally created and [published](http://digestingduck.blogspot.com/2009/08/font-stash.html) by [Mikko Mononen](http://digestingduck.blogspot.com).

## Notes

To get this to work on a Raspberry Pi, you need to rename "libs/fontstash/src/fontstash.c" and "libs/fontstash/src/stb_truetype.c" from *.c to *.cpp.

## License

ofxFontStash.cpp and ofxFontStash.h have been created by Oriol Ferrer Mesià and released under [MIT license](http://www.opensource.org/licenses/mit-license.php).

	Copyright (c) 2012 Oriol Ferrer Mesià
	
	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

The files fontstash.c, fontstash.h and main.c are licensed under the zlib license:

    Copyright (c) 2011 Andreas Krinke andreas.krinke@gmx.de
    Copyright (c) 2009 Mikko Mononen memon@inside.org

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

For UTF-8 decoding, Font Stash uses Björn Höhrmann's [Flexible and Economical UTF-8 Decoder](http://bjoern.hoehrmann.de/utf-8/decoder/dfa/) (included in fontstash.c).
His code is licensed under the MIT license:

    Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
    THE SOFTWARE.
    
Finally, Font Stash uses Sean Barrett's public domain truetype font rasterizer [stb_truetype.h](http://nothings.org/).

