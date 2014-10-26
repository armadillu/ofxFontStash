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

#define OFX_FONT_STASH_LINE_HEIGHT_MULT	0.9

#include "ofMain.h"
#include "ofUTF8.h"
#include "ofTextConverter.h"

extern "C" {
	#include "fontstash.h"
}


class ofxFontStash{

	public:
		
		ofxFontStash();
		~ofxFontStash();
	
		//call this to set your font file (.ttf, etc)
		bool setup( string fontFile,
				   float lineHeightPercent = 1.0f,
				   int textureDimension = 512,	//texture atlas size, must be PowerOfTwo (512, 1024, 2048, etc)
				   bool createMipMaps = false,	//create mipmaps for the texture atlasas; if you do
												//you will need some extra padding between the characters
												//in the altases, otherwise the mipmaps will leak when
												//using smaller sizes, and characters will have white
												//outlines around them
				   int intraCharPadding = 0,	//padding around each character in the texture atlas;
												//wastes texture space, but makes mipmaps work.
				   float dpiScale = 1.0f		//character texture is rendered internally at this scale
				   );

		//will draw text in one line, ignoring "\n"'s
		void draw( string text, float size, float x, float y);

		//text with "\n" will produce line breaks
		void drawMultiLine( string text, float fontSize, float x, float y );

		//fits text in a column of a certain width
		//if you only want to find out the bbox size, send in dontDraw=true
		//numLines will return the number of lines this has been split in
		ofRectangle drawMultiLineColumn( string &text, float fontSize, float x, float y,
											float columnWidth, int &numLines, bool dontDraw = false,
											int maxLines = 0, bool giveBackNewLinedText = false,
											bool * wordsWereTruncated = NULL );

		//if the text has newlines, it will be treated as if was called into drawMultiLine()
		ofRectangle getBBox( string text, float size, float x, float y );
		ofRectangle getBBox( string text, float size, float x, float y, float columnWidth );


		//interleave drawBatch* calls between begin() and end()
		void beginBatch();
		void drawBatch( string text, float size, float x, float y);
		void drawMultiLineBatch( string text, float size, float x, float y );
		void endBatch();

		void setLineHeight(float percent);

		void setKerning(bool enabled); //use ttf supplied kerning info at draw time or not
		bool getKerning();

		sth_stash* getStash(){return stash;}; //you probably dont need to mess with that
		float getDpiScale(){return dpiScale;}
		void setLodBias(float bias); //only makes sense when using mipmaps!

        // ofTrueTypeFont parity methods
        bool loadFont(string filename, int fontsize, float lineHeightPercent = 1.0f, int textureDimension = 512);
        bool isLoaded();
    
        void setSize(int fontsize);
        int getSize();
    
        float getLineHeight();
        float getSpaceSize();

		float getCharacterSpacing(){return stash->charSpacing;}
		void setCharacterSpacing(float spacing){stash->charSpacing = spacing;}
    
        float stringWidth(const string& s);
        float stringHeight(const string& s);
    
        ofRectangle getStringBoundingBox(const string& s, float x, float y);
    
        void drawString(const string& s, float x, float y);
    
	private:

		int					extraPadding; //used for mipmaps
		float				lineHeight; // as percent, 1.0 would be normal
		struct sth_stash*	stash;
		int					stashFontID;
		bool				batchDrawing;

		//fill in a string
		string walkAndFill(ofUTF8Ptr being, ofUTF8Ptr & iter, ofUTF8Ptr end);
    
        // ofTrueTypeFont parity attributes
        int					fontSize;
		float				dpiScale;
};


#endif
