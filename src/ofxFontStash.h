//
//  ofxFontStash.h
//  fontsExample
//
//  Created by Oriol Ferrer Mesià on 22/05/12.
//  Copyright (c) 2012 uri.cat. All rights reserved.
//

/*
 
 The MIT License
 
 Copyright (c) 2012, Oriol Ferrer Mesià.
 
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
 */


// TODO: implement getBoundingBoxSize() for text with multiple lines!

#ifndef ofxFontStash_h
#define ofxFontStash_h

#include "ofMain.h"

extern "C" {
	#include "fontstash.h"
}


class ofxFontStash{

	public:
		
		ofxFontStash();
		~ofxFontStash();
	
		//call this to set your font file (.ttf, etc)
		bool setup( string fontFile, float lineHeightPercent = 1.0f );
			
		//if automaticBeginEnd == false you will need to manually call begin() and end(). This improves
		//performance if you want to make several consecutive draw() calls
		void draw( string text, float size, int x, int y, bool automaticBeginEnd = true );

		//same as draw, but scans the text for '\n' and starts a new line if found
		//if automaticBeginEnd == false you will need to manually call begin() and end(). This improves
		//performance if you want to make several consecutive draw() calls
		void drawMultiLine( string text, float size, int x, int y, bool automaticBeginEnd = true );
	
		//to be used when drawing with automaticBeginEnd == false
		inline void begin(){ sth_begin_draw(stash);	}

		//to be used when drawing with automaticBeginEnd == false
		inline void end(){ sth_end_draw(stash); }
	
		ofRectangle getBoundingBoxSize( string text, float size, int x, int y );

	private:
		
		float lineHeight;
		struct sth_stash* stash;
		int stashFontID;
	
};


#endif
