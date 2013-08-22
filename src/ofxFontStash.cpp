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

#include "Poco/UnicodeConverter.h"


ofxFontStash::ofxFontStash(){
	stashFontID = 0;
	lineHeight = 1.0f;
	stash = NULL;
	batchDrawing = false;
}

ofxFontStash::~ofxFontStash(){
	if(stash != NULL) sth_delete(stash);
}

bool ofxFontStash::setup( string fontFile, float lineHeightPercent , int texDimension /*has to be powerfOfTwo!*/){
	
	if (stash == NULL){
		lineHeight = lineHeightPercent;
		texDimension = ofNextPow2(texDimension);
		stash = sth_create(texDimension,texDimension);
		stashFontID = sth_add_font( stash, ofToDataPath( fontFile ).c_str() );
		if ( stashFontID != 0){
			printf("ofxFontStash : loaded font %s in texture (%d x %d)\n", fontFile.c_str(), texDimension, texDimension );
			return true;
		}else{
			printf("ofxFontStash : Can't load font! %s\n", fontFile.c_str() );
		}
	}else{
		printf("ofxFontStash : don't call setup() more than once! %s\n", fontFile.c_str() );
	}
	return false;
}


void ofxFontStash::draw( string text, float size, float x, float y){
	
	if (stash != NULL){
		float dx = 0;
		
		glPushMatrix();
		glTranslatef(x, y, 0.0);
		sth_begin_draw(stash);
		sth_draw_text( stash, stashFontID, size, 0, 0 , text.c_str(), &dx ); //this might draw
		sth_end_draw(stash); // this actually draws
		glPopMatrix();
	}else{
		printf("ofxFontStash : can't draw() without having been setup first!\n");
	}		
}

void ofxFontStash::draw(wstring text, float size, float x, float y){
	string utf8text;
	Poco::UnicodeConverter::toUTF8(text, utf8text);

	draw(utf8text, size, x, y);
}

void ofxFontStash::drawMultiLine( string text, float size, float x, float y){
	
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
				sth_draw_text( stash, stashFontID, size, 0.0f, size * lineHeight * line, s.c_str(), &dx );
				line ++;
			}
			sth_end_draw(stash);
		glPopMatrix();

	}else{
		printf("ofxFontStash : can't drawMultiLine() without having been setup first!\n");
	}		
}

void ofxFontStash::drawMultiLine(wstring text, float size, float x, float y){
	string utf8text;
	Poco::UnicodeConverter::toUTF8(text, utf8text);

	drawMultiLine(utf8text, size, x, y);
}

void ofxFontStash::beginBatch(){
	if(stash != NULL){
		batchDrawing = TRUE;
		sth_begin_draw(stash);
	}
}

void ofxFontStash::endBatch(){
	if(stash != NULL){
		batchDrawing = FALSE;
		sth_end_draw(stash);
	}
}

void ofxFontStash::drawBatch( string text, float size, float x, float y){
	if (stash != NULL){
		if(batchDrawing){
			float dx = 0;
			sth_begin_draw(stash);
			sth_draw_text( stash, stashFontID, size, x, y, text.c_str(), &dx ); //this might draw
			sth_end_draw(stash); // this actually draws
		}else{
			printf("ofxFontStash : can't drawBatch() without calling beginBatch() first!\n");
		}
	}else{
		printf("ofxFontStash : can't drawBatch() without having been setup first!\n");
	}
}


void ofxFontStash::drawMultiLineBatch( string text, float size, float x, float y ){
	if (stash != NULL){
		if(batchDrawing){
			float dx = 0;
			stringstream ss(text);
			string s;
			int line = 0;
			while ( getline(ss, s, '\n') ) {
				//cout << s << endl;
				float dx = 0;
				sth_draw_text( stash, stashFontID, size, 0.0f, size * lineHeight * line, s.c_str(), &dx );
				line ++;
			}
		}else{
			printf("ofxFontStash : can't drawMultiLineBatch() without calling beginBatch() first!\n");
		}
	}else{
		printf("ofxFontStash : can't drawMultiLineBatch() without having been setup first!\n");
	}

}


ofRectangle ofxFontStash::getBoundingBoxSize( string text, float size, float xx, float yy ){

	ofRectangle r;

	if (stash != NULL){
		stringstream ss(text);
		string s;
		int line = 0;
		float totalH = 0;
		vector<ofRectangle> rects;
		while ( getline(ss, s, '\n') ) {
			float dx = 0;
			float w, h, x, y;
			sth_dim_text( stash, stashFontID, size, s.c_str(), &x, &y, &w, &h);
			r.x = x + xx;
			r.y = yy + y ;
			w = fabs (w - x);
			h = fabs(y - h);
			if(w > r.width) r.width = w;
			if(h > r.height) r.height = h;
			ofRectangle r2 = r;
			r2.y -= r2.height;
			r2.y += size * lineHeight * line;
			rects.push_back(r2);
			//ofSetColor(255,32); //debug
			//ofRect(r2);
			line ++;
		}

		if(line > 1){ //if multiline
			r.y -= rects[0].height;
			for(int i = 0; i < rects.size(); i++){
				r = r.getUnion(rects[i]);
			}
		}else{
			r.y -= r.height;
		}

	}else{
		printf("ofxFontStash : can't getBoundingBoxSize() without having been setup first!\n");
	}
	return r;
}


void ofxFontStash::setLineHeight(float percent){
	lineHeight = percent;
}