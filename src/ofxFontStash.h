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

#ifndef ofxFontStash_h
#define ofxFontStash_h

#include "ofMain.h"

//extern "C" {
	#include "fontstash.h"
//}


class ofxFontStash{

	public:
		
		ofxFontStash();
		~ofxFontStash();
	
		//call this to set your font file (.ttf, etc)
		bool setup( string fontFile, float lineHeightPercent = 1.0f, int textureDimension = 512);
			
		void draw( string text, float size, float x, float y);
		void draw( wstring text, float size, float x, float y);
		void drawMultiLine( string text, float size, float x, float y );
		void drawMultiLine( wstring text, float size, float x, float y);

		//if the text has newlines, it will be treated as if was called into drawMultiLine()
		ofRectangle getBoundingBoxSize( string text, float size, float x, float y );

		//interleave drawBatch* calls between begin() and end()
		void beginBatch();
		void drawBatch( string text, float size, float x, float y);
		void drawMultiLineBatch( string text, float size, float x, float y );
		void endBatch();

		void setLineHeight(float percent);

	private:
		
		float				lineHeight; // as percent, 1.0 would be normal
		struct sth_stash*	stash;
		int					stashFontID;
		bool				batchDrawing;
	
};


#endif
