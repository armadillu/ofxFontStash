#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	font.setup("Vera.ttf");
	unicodeFont.setup("Arial Unicode.ttf", //font file, ttf only
					  1.0,					//lineheight percent
					  1024,					//texture atlas dimension
					  true,					//create mipmaps of the font, useful to scale down the font at smaller sizes
					  8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
					  2.0f					//dpi scaleup, render textures @2x the reso
					  );					//lower res mipmaps wil bleed into each other

}

void ofApp::update(){
	//you can set a mipmap bias for fonts for which you created mipmaps
	unicodeFont.setLodBias(ofMap(ofGetMouseX(), 0, ofGetWidth(), -1, 1));
}

//--------------------------------------------------------------
void ofApp::draw(){

	float lineHeight = ofMap(mouseY, 0, ofGetHeight(), 0, 2, true);
	unicodeFont.setLineHeight(lineHeight);

	float x = 30;
	float y = 40;
	
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
	ofRectangle bbox;
	ofRectangle bboxMultiline;

	ofSetColor(255, 0, 0, 32);
	TIME_SAMPLE_START("bbox");
	bbox = font.getBBox( demoText, fontSize, x, y);
	TIME_SAMPLE_STOP("bbox");
	ofRect( bbox );

	// draw multiline text /////////////////////////////////////////////////
	
	y += 25 + bbox.height;
	drawPoint(x, y); //draw insertion point	
	
	ofSetColor(255);
	string s = (string)"ofxFontStash can draw multiline text" + "\n" +
	"It also supports unicode strings: " + "\n" +
	"槊監しゅ祟䤂לרפובליקה. אם מיזם 銆銌 憉 圩芰敔 तकनिकल कार्यलय";

	TIME_SAMPLE_START("drawMultiLine");
	unicodeFont.drawMultiLine( s,  fontSize, x, y);
	TIME_SAMPLE_STOP("drawMultiLine");
	
	// multiline bbox /////////////////////////////////////////////////////

	ofSetColor(0, 255, 0, 32);
	TIME_SAMPLE_START("getBoundingBoxSize");
	bboxMultiline = unicodeFont.getBBox( s, fontSize, x, y);
	TIME_SAMPLE_STOP("getBoundingBoxSize");
	ofRect( bboxMultiline );


	// draw multiline column with a fixed width ///////////////////////////

	y += 25 + bboxMultiline.height;
	drawPoint(x, y); //draw insertion point

	ofSetColor(255);
	s = "And you can wrap text to a certain (mouseX) width:\n\nLorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam.";
	//s = "international bananas";

	int numLines = 0;
	bool wordsWereCropped;
	ofRectangle column;
	TIME_SAMPLE_START("drawMultiLineColumn");
	column = font.drawMultiLineColumn(	s,			/*string*/
										fontSize,	/*size*/
										x, y,		/*where*/
										MAX( 10 ,mouseX - x), /*column width*/
										numLines,	/*get back the number of lines*/
										false,		/* if true, we wont draw (just get bbox back) */
										5,			/* max number of lines to draw, crop after that */
										true,		/*get the final text formatting (by adding \n's) in the supplied string;
													 BE ARWARE that using TRUE in here will modify your supplied string! */
										&wordsWereCropped /* this bool will b set to true if the box was to small to fit all text*/
									 );
	TIME_SAMPLE_STOP("drawMultiLineColumn");


	//report if some words had to be cropped to fit in the column when using drawMultiLineColumn()
	if(!wordsWereCropped) ofSetColor(255,32);
	else (ofGetFrameNum()%6 <= 2) ? ofSetColor(255,32):ofSetColor(255,0,0,32); //flash if cropped
	ofRect(column);


	// batch drawing, optimized for multiple drawing calls /////////////////

	y += column.height + 25;
	drawPoint(x, y); //draw insertion point

	ofSetColor(255);
	TIME_SAMPLE_START("drawBatch");
	font.beginBatch();	//call "begin" before drawing fonts
	for (int i = 0; i < 5; i++){
		font.drawBatch("batch mode #" + ofToString(i+1), fontSize, x, y + i * fontSize );
	}
	font.endBatch();		//call "end" once finished
	TIME_SAMPLE_STOP("drawBatch");

	// rotating text ///////////////////////////////////////////////////////
	
	ofPushMatrix();
		ofTranslate(x + 400, y + 50);
		ofRotate( -200 * ofGetElapsedTimef(), 0, 0, 1);
		ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
		font.draw("surrealismoooo!", fontSize, 0, 0 );
		drawPoint(0,0);
	ofPopMatrix();

	// scaling text with mipmaps ///////////////////////////////////////////////////////

	ofPushMatrix();
	ofTranslate(600, 40);
	float scale = mouseY /(float) ofGetHeight();
	ofPushMatrix();
	ofScale(scale, scale);
	ofSetColor(255);
	unicodeFont.draw("MIPMAPS :)", fontSize * 2, 0, 0 );
	drawPoint(0,0);
	ofPopMatrix();

	ofTranslate(0, 30);
	ofScale(scale, scale);
	font.draw("NO MIPMAPS :(", fontSize * 2, 0, 0 );
	drawPoint(0,0);
	ofPopMatrix();


}


void ofApp::drawPoint(float x, float y){
	ofSetColor(0, 255, 0, 128);	
	ofCircle(x, y, 2);
}


void ofApp::keyPressed(int k){
	font.setKerning(!font.getKerning());
	unicodeFont.setKerning(!unicodeFont.getKerning());
}
