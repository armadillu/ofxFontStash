//
//  ofxFontStash.cpp
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


#include <iostream>
#include "ofxFontStash.h"

std::string searchAndReplace(std::string &s,
							 std::string toReplace,
							 std::string replaceWith);

std::string searchAndReplace(std::string &s,
                      std::string toReplace,
                      std::string replaceWith)
{
    return(s.replace(s.find(toReplace), toReplace.length(), replaceWith));
}

/* *********************************************************************** */

ofxFontStash::ofxFontStash(){
//	stashFontID = 0;
	lineHeight = 1.0f;
	stash = NULL;
	batchDrawing = false;
}

ofxFontStash::~ofxFontStash(){
	if(stash != NULL) sth_delete(stash);
}

bool ofxFontStash::setup(string firstFontFile, float lineHeightPercent , int _texDimension /*has to be powerfOfTwo!*/,
						 bool createMipMaps, int intraCharPadding, float dpiScale_){

	if (stash == NULL){
		dpiScale = dpiScale_;
		extraPadding = intraCharPadding;
		lineHeight = lineHeightPercent;
		texDimension = ofNextPow2(_texDimension);
		stash = sth_create(texDimension,texDimension, createMipMaps, intraCharPadding, dpiScale);
		stash->doKerning = 0; //kerning disabled by default
		stash->charSpacing = 0.0; //spacing neutral by default
		addFont(firstFontFile);
	}else{
		ofLogError("ofxFontStash") << "don't call setup() more than once!";
	}

	return false;
}

void ofxFontStash::addFont(const std::string &fontFile)
{
	if (stash == NULL) {
		ofLogError("ofxFontStash") << "addFont() error: font stash not initialized, call setup first";
		return;
	}

	string fontPath = ofToDataPath(fontFile);
	int fontId = sth_add_font(stash, fontPath.c_str());
	if (fontId <= 0) {
		ofLogError("ofxFontStash") << "Can't load font! \"" << fontPath.c_str() << "\"";
		return;
	}

	fontIds.push_back(fontId);

	ofLogNotice("ofxFontStash") << "loaded font '" << fontFile << "' in texture ("<<texDimension<<" x "<<texDimension<<")";
}


void ofxFontStash::draw( const string& text, float size, float x, float y){
	
	if (stash != NULL){
		float dx = 0;
		
		glPushMatrix();
		glTranslatef(x, y, 0.0);
		sth_begin_draw(stash);
		sth_draw_text( stash, fontIds[0], size, 0, 0 , text.c_str(), &dx ); //this might draw
		sth_end_draw(stash); // this actually draws
		glPopMatrix();
	}else{
		ofLogError("ofxFontStash") << "can't draw() without having been setup first!";
	}		
}


void ofxFontStash::drawMultiLine( const string& text, float size, float x, float y){
	
	if (stash != NULL){

		glPushMatrix();
			glTranslatef(x, y, 0.0f);
			sth_begin_draw(stash);
			
			stringstream ss(text);
			string s;
			int line = 0;
			while ( getline(ss, s, '\n') ) {
				//cout << s << endl;
				float dx = 0;
				sth_draw_text(stash,
							  fontIds[0],
							  size,
							  0.0f,
							  size * lineHeight * OFX_FONT_STASH_LINE_HEIGHT_MULT * line * dpiScale,
							  s.c_str(),
							  &dx
							  );
				line ++;
			}
			sth_end_draw(stash);
		glPopMatrix();

	}else{
		ofLogError("ofxFontStash") << "can't draw() without having been setup first!";
	}		
}


ofRectangle ofxFontStash::drawMultiLineColumn( string & text, float size, float x, float y,
											  float maxW, int &numLines, bool dontDraw, int maxLines,
											  bool giveBackNewLinedText, bool* wordsWereTruncated){

	ofRectangle totalArea = ofRectangle(x,y,0,0);

	if(wordsWereTruncated){
		*wordsWereTruncated = false;
	}

	if (stash != NULL){

		numLines = 0;
		if(!dontDraw){
			glPushMatrix();
			glTranslatef(x, y, 0.0f);
		}
		//ofLine(0, 0, maxW, 0);

		vector<string>splitLines;
		ofRectangle r;

		//ofUTF8Ptr start = ofUTF8::beginPtr(text);
		ofUTF8Ptr iter = ofUTF8::beginPtr(text);
		ofUTF8Ptr lineStart = iter;
		ofUTF8Ptr lastSpace;
        ofUTF8Ptr stop = ofUTF8::endPtr(text);

        string thisLine = "";
		bool foundSpace = false;
		bool foundNewLine = false;
        while(iter < stop) {

			ofUniChar c = ofUTF8::getNext(iter); // get the next unichar and iterate
			if ( ofUnicode::isSpace(c) ){
				foundSpace = true;
				lastSpace = iter;
			}
			if ( ofTextConverter::toUTF8(c) == "\n" ){
				foundNewLine = true;
			}
            thisLine += ofTextConverter::toUTF8(c);
			r = getBBox(thisLine.c_str(), size, 0,0);
			if ( r.width > maxW || foundNewLine ) { //we went too far, lets jump back to our closest space
				if(foundNewLine){
					if (thisLine == "\n"){ //if the whole line is only \n, replace with a space to avoid weird things
						thisLine = " ";
					}else{	//otherwise remove the "\n"
						thisLine = thisLine.substr(0, thisLine.length()-1);
					}
					splitLines.push_back(thisLine);
					
				}else{
					if (foundSpace){
						string finalLine = walkAndFill(lineStart, iter, lastSpace);
						splitLines.push_back(finalLine);
						
						// Edge case where if max width is met and first character is space
						if(!ofUnicode::isSpace(ofUTF8::get(lineStart))){
							iter = lastSpace;
						}
					}else{
						splitLines.push_back(thisLine);
						if(wordsWereTruncated){
							*wordsWereTruncated = true;
						}
					}
				}
				//reset counter vars
				lineStart = iter;
				r.width = 0;
				thisLine = "";
				foundSpace = foundNewLine = false;
			}else{
				if(iter == stop){ //last line!
					string finalLine = walkAndFill(lineStart, iter, stop);
					splitLines.push_back(finalLine);
					break;
				}
			}
        }

		if(!dontDraw) beginBatch();
		numLines = splitLines.size();
		int linesToDraw = 0;
		if (maxLines > 0 ){
			linesToDraw = MIN(splitLines.size(), maxLines);
			numLines = splitLines.size();
		}else{
			linesToDraw = splitLines.size();
		}

		for(int i = 0; i < linesToDraw; i++){
			float yy = lineHeight * OFX_FONT_STASH_LINE_HEIGHT_MULT * size * i;
			if(!dontDraw){
				ofPushMatrix();
				ofTranslate(0, yy);
				drawBatch(splitLines[i], size, 0, 0 );
				ofPopMatrix();
			}
			#if OF_VERSION_MAJOR == 0 && OF_VERSION_MINOR >= 8
			totalArea = totalArea.getUnion( getBBox(splitLines[i], size, x, y + yy));
			#else
			totalArea = getBBox(splitLines[i], size, x, y + yy); //TODO!
			#endif
		}
		if(!dontDraw){
			endBatch();
			glPopMatrix();
		}

		//return through reference the edited text (with newLines!)
		if(giveBackNewLinedText){
			text = "";
			for (int i = 0; i < numLines; i++){
				if (i < maxLines){
					text += splitLines[i];
					if (i != numLines-1) text += "\n";
				}
			}
		}

	}else{
		ofLogError("ofxFontStash") << "can't draw() without having been setup first!";
	}
	return totalArea;
}


ofVec2f ofxFontStash::drawMultiColumnFormatted(const string &text, float size, float columnWidth, bool topLeftAlign, bool dryrun){

	float maxX=0;

	if (stash == NULL || fontIds.empty()) {
		ofLogError("ofxFontStash") << "error: stash not initialized or no font";
		return ofVec2f(0,0);
	}

	//TODO hack!
	//this is to fix the issue where two consecutive \n\n's are ignored
	string localText = text;
	ofStringReplace(localText, "\n", " \n");

	vector<std::string> allWords;
	vector<ofVec2f> wordSizes;
	vector<int> wordFonts;
	vector<ofColor> wordColors;
	vector<float> wordScales;

	int currentFontId = fontIds[0];
	ofColor currentColor = ofGetStyle().color;
	float currentScale = 1;

	// first, calculate the sizes of all the words
	//
	vector<std::string> lines = ofSplitString(localText, "\n");
	for (int i=0; i<lines.size(); i++) {

		vector<std::string> words = ofSplitString(lines[i], " ");

		for (int j=0; j<words.size(); j++) {

			// handle '@' code to change font id
			if (isFontCode(words[j])) {
				currentFontId = fontIds[ofToInt(words[j].substr(1, words[j].length()))];
				continue;
			}

			// handle '#' code to change color
			if (isColorCode(words[j])) {
				currentColor = ofColor::fromHex(ofHexToInt(words[j].substr(1, words[j].length())));
				continue;
			}

			// handle '%' code to change scale
			if (isScaleCode(words[j])) {
				currentScale = ofToFloat(words[j].substr(1, words[j].length()));
				continue;
			}

			std::string word = words[j];

			// add ' ' because we removed it when we did the split
			if (j != words.size()-1) {
				word += " ";
			}

			float x, y, w, h;
			sth_dim_text( stash, currentFontId, size * currentScale / dpiScale, word.c_str(), &x, &y, &w, &h);

			allWords.push_back(word);
			wordSizes.push_back(ofVec2f(w, h));
			wordFonts.push_back(currentFontId);
			wordColors.push_back(currentColor);
			wordScales.push_back(currentScale);

		}

		// all end of line
		allWords.push_back("\n");
		// a place holder to match indexes
		wordSizes.push_back(ofVec2f(0, 0));
		wordFonts.push_back(currentFontId);
		wordColors.push_back(currentColor);
		wordScales.push_back(currentScale);
	}

	// now draw the text
	//
	ofVec2f drawPointer(0, 0);
	float asc = 0.0f;

	if (topLeftAlign) {
		float desc, lineh;
		sth_vmetrics(stash, wordFonts[0], size, &asc, &desc, &lineh);

		ofPushMatrix();
		ofTranslate(0, asc);
	}

	if (!dryrun) {
		sth_begin_draw(stash);
	}

	for (int i=0; i<allWords.size(); i++) {

		// do we need to jump a line?
		if ((drawPointer.x + wordSizes[i].x > columnWidth ||
			allWords[i] == "\n") &&
			drawPointer.x != 0)
		{
			// jump one line down
			drawPointer.y += lineHeight * size * wordScales[i];
			drawPointer.x = 0;
		}

		// we need to flush the vertices if we change the color
		if (!dryrun) {
			if (wordColors[i] != ofGetStyle().color) {
				sth_end_draw(stash);
				sth_begin_draw(stash);

				ofSetColor(wordColors[i]);
			}
		}

		float dx = 0;
		if (!dryrun) {
			sth_draw_text( stash, wordFonts[i], size * wordScales[i], drawPointer.x, drawPointer.y, allWords[i].c_str(), &dx );
		}
		drawPointer.x += wordSizes[i].x;

		// save maxX so we'll return the size
		if (drawPointer.x > maxX) {
			maxX = drawPointer.x;
		}
	}

	if (!dryrun) {
		sth_end_draw(stash);
	}

	if (topLeftAlign) {
		ofPopMatrix();
	}

	return ofVec2f(maxX, drawPointer.y - (lineHeight * size / dpiScale - asc));
}


float ofxFontStash::getFontHeight(float fontSize)
{
	float asc, desc, lineh;

	sth_vmetrics(stash, fontIds[0], fontSize, &asc, &desc, &lineh);

	return asc - desc;
}




void ofxFontStash::beginBatch(){
	if(stash != NULL){
		batchDrawing = true;
		sth_begin_draw(stash);
	}
}

void ofxFontStash::endBatch(){
	if(stash != NULL){
		batchDrawing = false;
		sth_end_draw(stash);
	}
}

void ofxFontStash::setLodBias(float bias){
	if(stash != NULL){
		set_lod_bias(stash, bias);
	}
}

void ofxFontStash::drawBatch( const string& text, float size, float x, float y){
	if (stash != NULL){
		if(batchDrawing){
			float dx = 0;
			ofPushMatrix();
			sth_begin_draw(stash);
			ofTranslate(x, y);
			sth_draw_text( stash, fontIds[0], size, 0, 0, text.c_str(), &dx ); //this might draw
			sth_end_draw(stash); // this actually draws
			ofPopMatrix();
		}else{
			ofLogError("ofxFontStash") <<"can't drawBatch() without calling beginBatch() first!";
		}
	}else{
		ofLogError("ofxFontStash") << "can't drawBatch() without having been setup first!";
	}
}


void ofxFontStash::drawMultiLineBatch( const string& text, float size, float x, float y ){
	if (stash != NULL){
		if(batchDrawing){
			float dx = 0;
			stringstream ss(text);
			string s;
			int line = 0;
			while ( getline(ss, s, '\n') ) {
				//cout << s << endl;
				float dx = 0;
				sth_draw_text( stash, fontIds[0], size, 0.0f, size * lineHeight * OFX_FONT_STASH_LINE_HEIGHT_MULT * line, s.c_str(), &dx );
				line ++;
			}
		}else{
			ofLogError("ofxFontStash") <<"can't drawBatch() without calling beginBatch() first!";
		}
	}else{
		ofLogError("ofxFontStash") << "can't drawBatch() without having been setup first!";
	}
}


string ofxFontStash::walkAndFill(ofUTF8Ptr begin, ofUTF8Ptr & iter, ofUTF8Ptr end){

	string finalLine = "";
	ofUTF8Ptr i = begin;
	while (i < iter) { // re-fill the finalLine from the begining to the last Space
        // Ignore any spaces at the beginning of the line
        ofUniChar c = ofUTF8::getNext(i);
        if (finalLine.empty() && ofUnicode::isSpace(c))
            continue;

		finalLine += ofTextConverter::toUTF8(c); // get the next unichar and iterate
		if(i == end){
			break;
		}
	}
	return finalLine;
}


void ofxFontStash::setKerning(bool enabled){
	if (stash){
		stash->doKerning = enabled ? 1 : 0;
	}else{
		ofLogError("ofxFontStash") <<"can't setKerning() without having been setup() first!";
	}
}


bool ofxFontStash::getKerning(){
	if (stash){
		return stash->doKerning != 0;
	}else{
		ofLogError("ofxFontStash") << "can't getKerning() without having been setup() first!";
	}
	return false;
}

ofRectangle ofxFontStash::getBBox( const string& text, float size, float xx, float yy ){

	ofRectangle totalArea;

	if (stash != NULL){
		stringstream ss(text);
		string s;
		int line = 0;
		float totalH = 0;
		vector<ofRectangle> rects;
		while ( getline(ss, s, '\n') ) {
			float dx = 0;
			float w, h, x, y;

			sth_dim_text( stash, fontIds[0], size / dpiScale, s.c_str(), &x, &y, &w, &h);
			totalArea.x = x + xx;
			totalArea.y = yy + y ;
			w = fabs (w - x);
			h = fabs(y - h);
			if(w > totalArea.width) totalArea.width = w;
			if(h > totalArea.height) totalArea.height = h;
			ofRectangle r2 = totalArea;
			r2.y -= r2.height;
			r2.y += ((size * lineHeight) / dpiScale) * OFX_FONT_STASH_LINE_HEIGHT_MULT * line;
			rects.push_back(r2);
			//ofSetColor(255,32); //debug
			//ofRect(r2);
			line ++;
		}

		if(line > 1){ //if multiline
			totalArea.y -= rects[0].height;
			for(int i = 0; i < rects.size(); i++){
				#if OF_VERSION_MAJOR == 0 && OF_VERSION_MINOR >= 8
				totalArea = totalArea.getUnion(rects[i]);	//TODO
				#endif
			}
		}else{
			totalArea.y -= totalArea.height;
		}

	}else{
		ofLogError("ofxFontStash") << "can't getBoundingBoxSize() without having been setup first!";
	}

	if(extraPadding > 0){
		totalArea.width -= extraPadding;
		totalArea.height -= extraPadding;
	}

	return totalArea;
}


void ofxFontStash::setLineHeight(float percent){
	lineHeight = percent;
}

//--------------------------------------------------------------
// ofTrueTypeFont parity methods
bool ofxFontStash::loadFont(string filename, int fsize, float lineHeightPercent, int textureDimension){
    fontSize = fsize;
	bool ret = setup(filename,lineHeightPercent, textureDimension);
	if (!ret) {
		return false;
	}
	return ret;
}

bool ofxFontStash::isLoaded(){
    return (stash != NULL);
}

void ofxFontStash::setSize(int fsize){
    fontSize = fsize;
}

int ofxFontStash::getSize(){
    return fontSize;
}

float ofxFontStash::getLineHeight(){
    return lineHeight;
}

float ofxFontStash::getSpaceSize(){
    return stringWidth(" ");
}

float ofxFontStash::stringWidth(const string& s){
    ofRectangle rect = getStringBoundingBox(s, 0,0);
    return rect.width;
}

float ofxFontStash::stringHeight(const string& s){
    ofRectangle rect = getStringBoundingBox(s, 0,0);
    return rect.height;
}

ofRectangle ofxFontStash::getStringBoundingBox(const string& s, float x, float y){
    return getBBox(s, fontSize, x, y);
}

void ofxFontStash::drawString(const string& s, float x, float y){
    draw(s, fontSize, x, y);
}
