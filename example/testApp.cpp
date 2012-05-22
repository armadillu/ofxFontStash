#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	font.setup("verdana.ttf", 1.3); //load verdana font, set lineHeight to be 130%

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	int x = 50;
	int y = 50;
	
	string demoText = "This is my text in verdana font.";
	float fontSize = 22.0f;
	
	// simple demo //////////////////////////////////////////////////////////
	
	ofSetColor(255);
	font.draw(
				  demoText,	//text to draw
				  fontSize,	//font size
				  x,		//x coord where to draw
				  y			//y coord where to draw
			  );

	
	// bounding box demo ///////////////////////////////////////////////////
	
	ofSetColor(255, 0, 0, 64);
	ofRectangle bbox = font.getBoundingBoxSize( demoText, fontSize, x, y);
	ofRect( bbox );
		
	
	// draw multiline text /////////////////////////////////////////////////
	
	ofSetColor(255);
	string s = (string)"this paragraph has" + "\n" + "multiple lines.";
	font.drawMultiLine( s,  fontSize, x, 3 * y);
	
	
	// batch drawing, optimized for multiple drawing calls /////////////////
	
	font.begin();
	for (int i = 0; i < 11; i++){
		font.draw("my line #" + ofToString(i+1), fontSize, x, 6 * y + i * fontSize );
	}
	font.end();
	
}

