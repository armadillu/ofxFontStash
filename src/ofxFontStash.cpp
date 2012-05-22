//
//  ofxFontStash.cpp
//  fontsExample
//
//  Created by Oriol Ferrer Mesià on 22/05/12.
//  Copyright (c) 2012 uri.cat. All rights reserved.
//

#include <iostream>
#include "ofxFontStash.h"

ofxFontStash::ofxFontStash(){
	stashFontID = 0;
	lineHeight = 1.0f;
	stash = NULL; 
}

ofxFontStash::~ofxFontStash(){
	if(stash != NULL) sth_delete(stash);
}

bool ofxFontStash::setup( string fontFile, float lineHeightPercent ){
	
	if (stash == NULL){
		lineHeight = lineHeightPercent;
		stash = sth_create(512,512);
		stashFontID = sth_add_font( stash, ofToDataPath( fontFile ).c_str() );			
		if ( stashFontID != 0){
			printf("ofxFontStash : loaded font %s\n", fontFile.c_str() );
			return true;
		}else{
			printf("ofxFontStash : Can't load font! %s\n", fontFile.c_str() );
		}
	}else{
		printf("ofxFontStash : don't call setup() more than once! %s\n", fontFile.c_str() );
	}
	return false;
}


void ofxFontStash::draw( string text, float size, float x, float y, bool automaticBeginEnd ){
	
	if (stash != NULL){
		
		if (automaticBeginEnd) sth_begin_draw(stash);
		
		float dx = 0;
		sth_draw_text( stash, stashFontID, size, x, y , text.c_str(), &dx );
		
		if (automaticBeginEnd) sth_end_draw(stash);
		
	}else{
		printf("ofxFontStash : can't draw() without having been setup first!\n");
	}		
}

void ofxFontStash::drawMultiLine( string text, float size, float x, float y, bool automaticBeginEnd ){
	
	if (stash != NULL){
		
		if (automaticBeginEnd) sth_begin_draw(stash);
		
		stringstream ss(text);
		string s;
		int line = 0;
		while ( getline(ss, s, '\n') ) {
			//cout << s << endl;
			float dx = 0;
			sth_draw_text( stash, stashFontID, size, x, y + size * lineHeight * line, s.c_str(), &dx );
			line ++;
		}
		
		if (automaticBeginEnd) sth_end_draw(stash);
		
	}else{
		printf("ofxFontStash : can't drawMultiLine() without having been setup first!\n");
	}		
}

ofRectangle ofxFontStash::getBoundingBoxSize( string text, float size, float x, float y ){

	ofRectangle r;
	sth_dim_text( stash, stashFontID, size, text.c_str(), &r.x, &r.y, &r.width, &r.height);
	r.width = fabs (r.width - r.x);
	r.height = fabs(r.x - r.height);
	r.x = x;
	r.y = y - r.height;
	return r;
}
