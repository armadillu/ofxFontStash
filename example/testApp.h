#pragma once

#include "ofMain.h"
#include "ofxFontStash.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		ofxFontStash font;
};
