#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	font.setup("Vera.ttf"); //load verdana font, set lineHeight to be 130%
	unicodeFont.setup("Arial Unicode.ttf"); //load verdana font, set lineHeight to be 130%
}


//--------------------------------------------------------------
void testApp::draw(){


	float x = 30 + mouseX * 0.05;
	float y = 40 + mouseY * 0.05;
	
	string demoText = "This is my text in BitStream Vera font.";
	float fontSize = 28;

	// simple demo //////////////////////////////////////////////////////////
	
	drawPoint(x, y);		//draw insertion point	
	
	ofSetColor(255);
	TIME_SAMPLE_START("simple draw");
	font.draw(
				  demoText,	//text to draw
				  fontSize,	//font size
				  x,		//x coord where to draw
				  y			//y coord where to draw
			  );
	TIME_SAMPLE_STOP("simple draw");
	
	// bounding box demo ///////////////////////////////////////////////////
	
	ofSetColor(255, 0, 0, 32);
	TIME_SAMPLE_START("bbox");
	ofRectangle bbox = font.getBoundingBoxSize( demoText, fontSize, x, y);
	TIME_SAMPLE_STOP("bbox");
	ofRect( bbox );
		
	
	// draw multiline text /////////////////////////////////////////////////
	
	y += 75;
	drawPoint(x, y);		//draw insertion point	
	
	ofSetColor(255);
	string s = (string)"ofxFontStash can draw multiline text" + "\n" +
	"It also supports unicode strings: " + "\n" +
	"ㄦ润榧 盤毚, إعلان, 藡覶譒, 한국어/조선말, ฉันหิว";

	TIME_SAMPLE_START("drawMultiLine");
	unicodeFont.drawMultiLine( s,  fontSize, x, y);
	TIME_SAMPLE_STOP("drawMultiLine");
	
	// multiline bbox /////////////////////////////////////////////////////

	ofSetColor(0, 255, 0, 32);
	TIME_SAMPLE_START("bbox MultiLine");
	ofRectangle bboxMultiline = unicodeFont.getBoundingBoxSize( s, fontSize, x, y);
	TIME_SAMPLE_STOP("bbox MultiLine");
	ofRect( bboxMultiline );

	// batch drawing, optimized for multiple drawing calls /////////////////

	y += 200;
	drawPoint(x, y);		//draw insertion point

	ofSetColor(255);
	TIME_SAMPLE_START("drawBatch");
	font.beginBatch();	//call "begin" before drawing fonts
	for (int i = 0; i < 10; i++){
		font.drawBatch("my line #" + ofToString(i+1), fontSize, x, y + i * fontSize );
	}
	font.endBatch();		//call "end" once finished
	TIME_SAMPLE_STOP("drawBatch");

	// rotating text /////////////////////////////////////////////////////
	ofPushMatrix();
		ofTranslate(x + 400, y + 150);
		ofRotate( -200 * ofGetElapsedTimef(), 0, 0, 1);
		ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
		font.draw("surrealismoooooo!", fontSize, 0, 0 );
		drawPoint(0,0);
	ofPopMatrix();

	ofSetColor(255);
	TIME_SAMPLE_DRAW_BOTTOM_RIGHT();
}

void testApp::drawPoint(float x, float y){
	ofSetColor(0, 255, 0, 128);	
	ofCircle(x, y, 2);
}
