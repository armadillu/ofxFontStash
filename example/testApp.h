#pragma once

#include "ofMain.h"
#include "ofxFontStash.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void draw();

		void drawPoint(float x, float y);
		ofxFontStash font;
};
