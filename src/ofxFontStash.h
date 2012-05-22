//
//  ofxFontStash.h
//  fontsExample
//
//  Created by Oriol Ferrer Mesià on 22/05/12.
//  Copyright (c) 2012 uri.cat. All rights reserved.
//


// TODO: implement getBoundingBoxSize() for text with multiple lines!

#ifndef fontsExample_ofxFontStash_h
#define fontsExample_ofxFontStash_h

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
		void draw( string text, float size, float x, float y, bool automaticBeginEnd = true );

		//same as draw, but scans the text for '\n' and starts a new line if found
		//if automaticBeginEnd == false you will need to manually call begin() and end(). This improves
		//performance if you want to make several consecutive draw() calls
		void drawMultiLine( string text, float size, float x, float y, bool automaticBeginEnd = true );
	
		//to be used when drawing with automaticBeginEnd == false
		inline void begin(){ sth_begin_draw(stash);	}

		//to be used when drawing with automaticBeginEnd == false
		inline void end(){ sth_end_draw(stash); }
	
		ofRectangle getBoundingBoxSize( string text, float size, float x, float y );

	private:
		
		float lineHeight;
		struct sth_stash* stash;
		int stashFontID;
	
};


#endif
