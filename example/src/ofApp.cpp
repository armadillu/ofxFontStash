#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(22, 22, 22, 255);

	font.setup("Vera.ttf"); //load verdana font, set lineHeight to be 130%
	unicodeFont.setup("Arial Unicode.ttf"); //load verdana font, set lineHeight to be 130%

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
	
	ofSetColor(255, 0, 0, 32);
	TIME_SAMPLE_START("bbox");
	ofRectangle bbox = font.getBBox( demoText, fontSize, x, y);
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
	ofRectangle bboxMultiline = unicodeFont.getBBox( s, fontSize, x, y);
	TIME_SAMPLE_STOP("getBoundingBoxSize");
	ofRect( bboxMultiline );


	// draw multiline column with a fixed width ///////////////////////////

	y += 25 + bboxMultiline.height;
	drawPoint(x, y); //draw insertion point

	ofSetColor(255);
	s = "And you can wrap text to a certain (mouseX) width:\n\nLorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam.";
	//s = "international bananas";

	TIME_SAMPLE_START("drawMultiLineColumn");
	int numLines = 0;
	bool wordsWereCropped;
	ofRectangle column = unicodeFont.drawMultiLineColumn(	s,			/*string*/
															fontSize,	/*size*/
															x, y,		/*where*/
															MAX( 10 ,mouseX - x), /*column width*/
															numLines,	/*get back the number of lines*/
															false,		/*if true, we wont draw (just get bbox back)*/
															5,			/* max number of lines to draw, crop after that */
															true,	/*get the final formatting with added \n's*/
															&wordsWereCropped
														 );
	cout << "##############" << endl;
	cout << s << endl;
	cout << "##############" << endl;
	TIME_SAMPLE_STOP("drawMultiLineColumn");
	if(!wordsWereCropped) ofSetColor(255,32);
	else ofSetColor(255,((ofGetFrameNum()%4 <= 1) ? 0:32));
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

	ofSetColor(255);
}


void ofApp::drawPoint(float x, float y){
	ofSetColor(0, 255, 0, 128);	
	ofCircle(x, y, 2);
}
