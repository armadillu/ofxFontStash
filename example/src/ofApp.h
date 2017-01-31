#pragma once

#include "ofMain.h"
#include "ofxFontStash.h"

//comment this line to ignore "ofxTimeMeasurements" calls
//enabling it requires the ofxTimeMeasurements addon
//https://github.com/armadillu/ofxTimeMeasurements
//#define MEASURE_PERFORMANCE

#ifdef MEASURE_PERFORMANCE
	#include "ofxTimeMeasurements.h"
#else
	#define TIME_SAMPLE_START ;
	#define TIME_SAMPLE_STOP ;
#endif

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void keyPressed(int);

		void drawPoint(float x, float y);

		ofxFontStash font;
		ofxFontStash unicodeFont;
};
