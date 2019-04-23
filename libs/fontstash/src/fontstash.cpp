//
// Copyright (c) 2011-2013 Andreas Krinke andreas.krinke@gmx.de
// Copyright (c) 2009 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//


// Oriol Ferrer Mesià made tiny modifications to make this render text flipped vertically
// Also changed the include ifdefs

// 31st July 2014 - Danoli3 - Daniel Rosser made minor macro modifictions to support Android and another iPhone Target.

/*
 #ifdef __MACOSX__
 #include <OpenGL/gl.h>
 #else
 #include <GL/gl.h>
 #endif
 */

//oriol replacing platform includes to work better in OpenFrameworks
//from http://stackoverflow.com/questions/5919996/how-to-detect-reliably-mac-os-x-ios-linux-windows-in-c-preprocessor
#if defined(_WIN32) || defined(_WIN64)
	//#include <gl/gl.h> //oriol making it work for win32
	#include "GL\glew.h"
	#include "GL\wglew.h"
#elif __ANDROID__   // Danoli3 - Adding Android target as it is supported with OpenGLES
#include <GLES/gl.h>
#include <GLES/glext.h>
#elif __APPLE__
#include "TargetConditionals.h"
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE_SIMULATOR) || (TARGET_IPHONE_SIMULATOR == 1) || (TARGET_OS_IPHONE == 1) || defined(TARGET_IPHONE)
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#elif (TARGET_OS_MAC)
#include <OpenGL/gl.h>
#else
// Unsupported platform
#endif
#elif __linux
#include <GL/gl.h>
#elif __unix // all unices not caught above
#include <GL/gl.h>
#elif __posix
#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/* @rlyeh: removed STB_TRUETYPE_IMPLENTATION. We link it externally */
#include "stb_truetype.h"
#include "fontstash.h"

#define HASH_LUT_SIZE 256
#define MAX_ROWS 128
#define VERT_COUNT (6*128 * 100)  /* oriol! x 100 to avoid repeated flush_draw() calls while drawing */
#define VERT_STRIDE (sizeof(float)*4)

#define TTFONT_FILE 1
#define TTFONT_MEM  2
#define BMFONT      3

static int idx = 1;


#ifdef __cplusplus
extern "C" {
#endif

static unsigned int hashint(unsigned int a)
{
	a += ~(a<<15);
	a ^=  (a>>10);
	a +=  (a<<3);
	a ^=  (a>>6);
	a += ~(a<<11);
	a ^=  (a>>16);
	return a;
}


struct ofx_sth_quad
{
	float x0,y0,s0,t0;
	float x1,y1,s1,t1;
};

struct ofx_sth_row
{
	short x,y,h;
};

struct ofx_sth_glyph
{
	unsigned int codepoint;
	short size;
	struct ofx_sth_texture* texture;
	int x0,y0,x1,y1;
	float xadv,xoff,yoff;
	int next;
};

struct ofx_sth_font
{
	int idx;
	int type;
	ofx_stbtt_fontinfo font;
	unsigned char* data;
	struct ofx_sth_glyph* glyphs;
	int lut[HASH_LUT_SIZE];
	int nglyphs;
	float ascender;
	float descender;
	float lineh;
	struct ofx_sth_font* next;
};

struct ofx_sth_texture
{
	GLuint id;
	// TODO: replace rows with pointer
	struct ofx_sth_row rows[MAX_ROWS];
	int nrows;
	float verts[4*VERT_COUNT];
	int nverts;
	struct ofx_sth_texture* next;
};


// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const unsigned char utf8d[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
	8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
	0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
	0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
	0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
	1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
	1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
	1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

static unsigned int decutf8(unsigned int* state, unsigned int* codep, unsigned int byte)
{
	unsigned int type = utf8d[byte];
	*codep = (*state != UTF8_ACCEPT) ?
		(byte & 0x3fu) | (*codep << 6) :
		(0xff >> type) & (byte);
	*state = utf8d[256 + *state*16 + type];
	return *state;
}



struct ofx_sth_stash* ofx_sth_create(int cachew, int cacheh, int createMipmaps, int charPadding, float dpiScale){

	struct ofx_sth_stash* stash = NULL;
	GLubyte* empty_data = NULL;
	struct ofx_sth_texture* texture = NULL;

	// Allocate memory for the font stash.
	stash = (struct ofx_sth_stash*)malloc(sizeof(struct ofx_sth_stash));
	if (stash == NULL) goto error;
	memset(stash,0,sizeof(struct ofx_sth_stash));

	// Create data for clearing the textures
	empty_data = (GLubyte*)	malloc(cachew * cacheh);
	if (empty_data == NULL) goto error;
	memset(empty_data, 0, cachew * cacheh);

	// Allocate memory for the first texture
	texture = (struct ofx_sth_texture*)malloc(sizeof(struct ofx_sth_texture));
	if (texture == NULL) goto error;
	memset(texture,0,sizeof(struct ofx_sth_texture));

	// Create first texture for the cache.
	stash->tw = cachew;
	stash->th = cacheh;
	stash->itw = 1.0f/cachew;
	stash->ith = 1.0f/cacheh;
	stash->empty_data = empty_data;
	stash->tt_textures = texture;
	stash->dpiScale = dpiScale;
	glGenTextures(1, &texture->id);
	if (!texture->id) goto error;
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, cachew, cacheh, 0, GL_ALPHA, GL_UNSIGNED_BYTE, empty_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stash->hasMipMap = createMipmaps;
	stash->padding = charPadding;
	stash->charSpacing = 0.0f;
	stash->doKerning = 0;
	return stash;
	
error:
	if (stash != NULL)
		free(stash);
	if (texture != NULL)
		free(texture);
	return NULL;
}

int ofx_sth_add_font_from_memory(struct ofx_sth_stash* stash, unsigned char* buffer)
{
	int i, ascent, descent, fh, lineGap;
	struct ofx_sth_font* fnt = NULL;

	fnt = (struct ofx_sth_font*)malloc(sizeof(struct ofx_sth_font));
	if (fnt == NULL) goto error;
	memset(fnt,0,sizeof(struct ofx_sth_font));

	// Init hash lookup.
	for (i = 0; i < HASH_LUT_SIZE; ++i) fnt->lut[i] = -1;

	fnt->data = buffer;


	// Init stb_truetype
	if (!ofx_stbtt_InitFont(&fnt->font, fnt->data, 0)) goto error;
	
	// Store normalized line height. The real line height is got
	// by multiplying the lineh by font size.
	ofx_stbtt_GetFontVMetrics(&fnt->font, &ascent, &descent, &lineGap);
	fh = ascent - descent;
	fnt->ascender = (float)ascent / (float)fh;
	fnt->descender = (float)descent / (float)fh;
	fnt->lineh = (float)(fh + lineGap) / (float)fh;

	fnt->idx = idx;
	fnt->type = TTFONT_MEM;
	fnt->next = stash->fonts;
	stash->fonts = fnt;
	
	return idx++;

error:
	if (fnt) {
		if (fnt->glyphs) free(fnt->glyphs);
		free(fnt);
	}
	return 0;
}

int ofx_sth_add_font(struct ofx_sth_stash* stash, const char* path)
{
	FILE* fp = 0;
	int datasize;
	unsigned char* data = NULL;
	int idx;
	
	// Read in the font data.
	fp = fopen(path, "rb");
	if (!fp) goto error;
	fseek(fp,0,SEEK_END);
	datasize = (int)ftell(fp);
	fseek(fp,0,SEEK_SET);
	data = (unsigned char*)malloc(datasize);
	if (data == NULL) goto error;
	fread(data, 1, datasize, fp);
	fclose(fp);
	fp = 0;
	
	idx = ofx_sth_add_font_from_memory(stash, data);
	// Modify type of the loaded font.
	if (idx)
		stash->fonts->type = TTFONT_FILE;
	else
		free(data);

	return idx;
	
error:
	if (data) free(data);
	if (fp) fclose(fp);
	return 0;
}

int ofx_sth_add_bitmap_font(struct ofx_sth_stash* stash, int ascent, int descent, int line_gap)
{
	int i, fh;
	struct ofx_sth_font* fnt = NULL;

	fnt = (struct ofx_sth_font*)malloc(sizeof(struct ofx_sth_font));
	if (fnt == NULL) goto error;
	memset(fnt,0,sizeof(struct ofx_sth_font));

	// Init hash lookup.
	for (i = 0; i < HASH_LUT_SIZE; ++i) fnt->lut[i] = -1;

	// Store normalized line height. The real line height is got
	// by multiplying the lineh by font size.
	fh = ascent - descent;
	fnt->ascender = (float)ascent / (float)fh;
	fnt->descender = (float)descent / (float)fh;
	fnt->lineh = (float)(fh + line_gap) / (float)fh;

	fnt->idx = idx;
	fnt->type = BMFONT;
	fnt->next = stash->fonts;
	stash->fonts = fnt;
	
	return idx++;

error:
	if (fnt) free(fnt);
	return 0;
}

void ofx_sth_add_glyph(struct ofx_sth_stash* stash,
                  int idx,
                  GLuint id,
                  const char* s,
                  short size, short base,
                  int x, int y, int w, int h,
                  float xoffset, float yoffset, float xadvance)
{
	struct ofx_sth_texture* texture = NULL;
	struct ofx_sth_font* fnt = NULL;
	struct ofx_sth_glyph* glyph = NULL;
	unsigned int codepoint;
	unsigned int state = 0;

	if (stash == NULL) return;
	texture = stash->bm_textures;
	while (texture != NULL && texture->id != id) texture = texture->next;
	if (texture == NULL)
	{
		// Create new texture
		texture = (struct ofx_sth_texture*)malloc(sizeof(struct ofx_sth_texture));
		if (texture == NULL) return;
		memset(texture, 0, sizeof(struct ofx_sth_texture));
		texture->id = id;
		texture->next = stash->bm_textures;
		stash->bm_textures = texture;
	}

	fnt = stash->fonts;
	while (fnt != NULL && fnt->idx != idx) fnt = fnt->next;
	if (fnt == NULL) return;
	if (fnt->type != BMFONT) return;
	
	for (; *s; ++s)
	{
		if (!decutf8(&state, &codepoint, *(unsigned char*)s)) break;
	}
	if (state != UTF8_ACCEPT) return;

	// Alloc space for new glyph.
	fnt->nglyphs++;
	fnt->glyphs = (struct ofx_sth_glyph *)realloc(fnt->glyphs, fnt->nglyphs*sizeof(struct ofx_sth_glyph)); /* @rlyeh: explicit cast needed in C++ */
	if (!fnt->glyphs) return;

	// Init glyph.
	glyph = &fnt->glyphs[fnt->nglyphs-1];
	memset(glyph, 0, sizeof(struct ofx_sth_glyph));
	glyph->codepoint = codepoint;
	glyph->size = size;
	glyph->texture = texture;
	glyph->x0 = x;
	glyph->y0 = y;
	glyph->x1 = glyph->x0+w;
	glyph->y1 = glyph->y0+h;
	glyph->xoff = xoffset;
	glyph->yoff = yoffset - base;
	glyph->xadv = xadvance;
	
	// Find code point and size.
	h = hashint(codepoint) & (HASH_LUT_SIZE-1);
	// Insert char to hash lookup.
	glyph->next = fnt->lut[h];
	fnt->lut[h] = fnt->nglyphs-1;
}

static struct ofx_sth_glyph* get_glyph(struct ofx_sth_stash* stash, struct ofx_sth_font* fnt, unsigned int codepoint, short isize)
{
	int i,g,advance,lsb,x0,y0,x1,y1,gw,gh;
	float scale;
	struct ofx_sth_texture* texture = NULL;
	struct ofx_sth_glyph* glyph = NULL;
	unsigned char* bmp = NULL;
	unsigned int h;
	float size = isize/10.0f;
	int rh;
	struct ofx_sth_row* br = NULL;

	// Find code point and size.
	h = hashint(codepoint) & (HASH_LUT_SIZE-1);
	i = fnt->lut[h];
	while (i != -1)
	{
		if (fnt->glyphs[i].codepoint == codepoint && (fnt->type == BMFONT || fnt->glyphs[i].size == isize))
			return &fnt->glyphs[i];
		i = fnt->glyphs[i].next;
	}
	// Could not find glyph.
	
	// For bitmap fonts: ignore this glyph.
	if (fnt->type == BMFONT) return 0;
	
	// For truetype fonts: create this glyph.
	scale = stash->dpiScale * ofx_stbtt_ScaleForPixelHeight(&fnt->font, size);
	g = ofx_stbtt_FindGlyphIndex(&fnt->font, codepoint);
	if(!g) return 0; /* @rlyeh: glyph not found, ie, arab chars */
	ofx_stbtt_GetGlyphHMetrics(&fnt->font, g, &advance, &lsb);
	ofx_stbtt_GetGlyphBitmapBox(&fnt->font, g, scale,scale, &x0,&y0,&x1,&y1);

	gw = x1-x0 + stash->padding;
	gh = y1-y0 + stash->padding;
	
	// Check if glyph is larger than maximum texture size
	if (gw >= stash->tw || gh >= stash->th)
		return 0;

	// Find texture and row where the glyph can be fit.
	br = NULL;
	rh = (gh+7) & ~7;
	texture = stash->tt_textures;
	while(br == NULL)
	{
		for (i = 0; i < texture->nrows; ++i)
		{
			if (texture->rows[i].h == rh && texture->rows[i].x+gw+1 <= stash->tw)
				br = &texture->rows[i];
		}
	
		// If no row is found, there are 3 possibilities:
		//   - add new row
		//   - try next texture
		//   - create new texture
		if (br == NULL)
		{
			short py = 0;
			// Check that there is enough space.
			if (texture->nrows)
			{
				py = texture->rows[texture->nrows-1].y + texture->rows[texture->nrows-1].h+1;
				if (py+rh > stash->th)
				{
					if (texture->next != NULL)
					{
						texture = texture->next;
					}
					else
					{
						// Create new texture
						texture->next = (struct ofx_sth_texture*)malloc(sizeof(struct ofx_sth_texture));
						texture = texture->next;
						if (texture == NULL) goto error;
						memset(texture,0,sizeof(struct ofx_sth_texture));
						//oriol counting how many we have created so far!
						int numTex = 1;
						struct ofx_sth_texture* tex = stash->tt_textures;
						while (tex->next != NULL) {
							numTex++;
							tex = tex->next;
						}
						printf("fontStash allocating a new texture of %d x %d (%d used so far)\n", stash->tw, stash->th, numTex );
						glGenTextures(1, &texture->id);
						if (!texture->id) goto error;
						glBindTexture(GL_TEXTURE_2D, texture->id);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, stash->tw,stash->th, 0, GL_ALPHA, GL_UNSIGNED_BYTE, stash->empty_data);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					}
					continue;
				}
			}
			// Init and add row
			br = &texture->rows[texture->nrows];
			br->x = 0;
			br->y = py;
			br->h = rh;
			texture->nrows++;	
		}
	}
	
	// Alloc space for new glyph.
	fnt->nglyphs++;
	fnt->glyphs = (struct ofx_sth_glyph *)realloc(fnt->glyphs, fnt->nglyphs*sizeof(struct ofx_sth_glyph)); /* @rlyeh: explicit cast needed in C++ */
	if (!fnt->glyphs) return 0;

	// Init glyph.
	glyph = &fnt->glyphs[fnt->nglyphs-1];
	memset(glyph, 0, sizeof(struct ofx_sth_glyph));
	glyph->codepoint = codepoint;
	glyph->size = isize;
	glyph->texture = texture;
	glyph->x0 = br->x;
	glyph->y0 = br->y;
	glyph->x1 = glyph->x0+gw;
	glyph->y1 = glyph->y0+gh;
	glyph->xadv = scale * advance;
	glyph->xoff = (float)x0;
	glyph->yoff = (float)y0;
	glyph->next = 0;

	// Advance row location.
	br->x += gw+1;
	
	// Insert char to hash lookup.
	glyph->next = fnt->lut[h];
	fnt->lut[h] = fnt->nglyphs-1;

	// Rasterize
	bmp = (unsigned char*)malloc(gw*gh);
	if (bmp)
	{
		ofx_stbtt_MakeGlyphBitmap(&fnt->font, bmp, gw,gh,gw, scale,scale, g);
		// Update texture
		glBindTexture(GL_TEXTURE_2D, texture->id);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//oriol trying to get rid of halos when rotating font
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//
		glTexSubImage2D(GL_TEXTURE_2D, 0, glyph->x0,glyph->y0, gw,gh, GL_ALPHA,GL_UNSIGNED_BYTE,bmp);
		if(stash->hasMipMap > 0){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8); //TODO check for hw support!
			#if defined(__ANDROID__) || defined(TARGET_OPENGLES) || defined(TARGET_RASPBERRY_PI)
				// OpenGLES 1.0 does not support the following.
			#else
//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.0); //shoot for sharper test
//			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3); // pick mipmap level 7 or lower
//			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.0);
			glGenerateMipmap(GL_TEXTURE_2D);
			#endif
		}
		free(bmp);
	}
	
	return glyph;

error:
	if (texture)
		free(texture);
	return 0;
}

static int get_quad(struct ofx_sth_stash* stash, struct ofx_sth_font* fnt, struct ofx_sth_glyph* glyph, short isize, float* x, float* y, struct ofx_sth_quad* q)
{
	int rx,ry;
	float scale = 1.0f ;

	if (fnt->type == BMFONT) scale = isize/(glyph->size*10.0f);

	rx = floorf(*x + scale * glyph->xoff);
	//ry = floorf(*y - scale * glyph->yoff); //oriol flipped vertically to better match openFrameworks
	ry = floorf(*y + scale * glyph->yoff);

	q->x0 = rx;
	q->y0 = ry;
	q->x1 = rx + scale * (glyph->x1 - glyph->x0);
	//q->y1 = ry - scale * (glyph->y1 - glyph->y0);
	q->y1 = ry + scale * (glyph->y1 - glyph->y0); //oriol flipped vertically to better match openFrameworks

	q->s0 = (glyph->x0) * stash->itw;
	q->t0 = (glyph->y0) * stash->ith;
	q->s1 = (glyph->x1) * stash->itw;
	q->t1 = (glyph->y1) * stash->ith;
	
	*x += scale * glyph->xadv;
	
	return 1;
}

static float* setv(float* v, float x, float y, float s, float t)
{
	v[0] = x;
	v[1] = y;
	v[2] = s;
	v[3] = t;
	return v+4;
}


void set_lod_bias(struct ofx_sth_stash* stash, float bias){

	struct ofx_sth_texture* texture = stash->tt_textures;
	if(stash->hasMipMap > 0){
		while (texture){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture->id);
			#ifndef TARGET_OPENGLES
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);
			#endif
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			texture = texture->next;
		}
	}
}

static void flush_draw(struct ofx_sth_stash* stash)
{
	struct ofx_sth_texture* texture = stash->tt_textures;
	short tt = 1;
	while (texture)
	{
		if (texture->nverts > 0)
		{
			glPushMatrix();
			float ss = 1.0f / stash->dpiScale;
			glScalef(ss, ss, ss);
			glBindTexture(GL_TEXTURE_2D, texture->id);
			glEnable(GL_TEXTURE_2D);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glVertexPointer(2, GL_FLOAT, VERT_STRIDE, texture->verts);
			glTexCoordPointer(2, GL_FLOAT, VERT_STRIDE, texture->verts+2);
			glDrawArrays(GL_TRIANGLES, 0, texture->nverts);
			glDisable(GL_TEXTURE_2D);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glPopMatrix();
			texture->nverts = 0;
		}
		texture = texture->next;
		if (!texture && tt)
		{
			texture = stash->bm_textures;
			tt = 0;
		}
	}
}

void ofx_sth_begin_draw(struct ofx_sth_stash* stash)
{
	if (stash == NULL) return;
	if (stash->drawing)
		flush_draw(stash);
	stash->drawing = 1;
}

void ofx_sth_end_draw(struct ofx_sth_stash* stash)
{
	if (stash == NULL) return;
	if (!stash->drawing) return;

/*
	// Debug dump.
	if (stash->nverts+6 < VERT_COUNT)
	{
		float x = 500, y = 100;
		float* v = &stash->verts[stash->nverts*4];

		v = setv(v, x, y, 0, 0);
		v = setv(v, x+stash->tw, y, 1, 0);
		v = setv(v, x+stash->tw, y+stash->th, 1, 1);

		v = setv(v, x, y, 0, 0);
		v = setv(v, x+stash->tw, y+stash->th, 1, 1);
		v = setv(v, x, y+stash->th, 0, 1);

		stash->nverts += 6;
	}
*/
	
	flush_draw(stash);
	stash->drawing = 0;
}

void ofx_sth_draw_text(struct ofx_sth_stash* stash,
				   int idx, float size,
				   float x, float y,
				   const char* s, float* dx)
{
	unsigned int codepoint;
	struct ofx_sth_glyph* glyph = NULL;
	struct ofx_sth_texture* texture = NULL;
	unsigned int state = 0;
	struct ofx_sth_quad q;
	short isize = (short)(size*10.0f);
	float* v;
	struct ofx_sth_font* fnt = NULL;
	
	if (stash == NULL) return;

	fnt = stash->fonts;
	while(fnt != NULL && fnt->idx != idx) fnt = fnt->next;
	if (fnt == NULL) return;
	if (fnt->type != BMFONT && !fnt->data) return;

	int len = strlen(s);
	float scale = ofx_stbtt_ScaleForPixelHeight(&fnt->font, size);
	int c = 0;
	float spacing = stash->charSpacing;
	int doKerning = stash->doKerning;
	int p = stash->padding;
	float dpiScale = stash->dpiScale;
	float tw = stash->padding / (float)stash->tw;

	for (; *s; ++s)
	{
		if (decutf8(&state, &codepoint, *(unsigned char*)s)) continue;
		glyph = get_glyph(stash, fnt, codepoint, isize);
		if (!glyph) continue;
		texture = glyph->texture;
		if (texture->nverts+6 >= VERT_COUNT)
			flush_draw(stash);
		
		if (!get_quad(stash, fnt, glyph, isize, &x, &y, &q)) continue;

		int diff = 0;
		if (c < len && doKerning > 0){
			diff = ofx_stbtt_GetCodepointKernAdvance(&fnt->font, *(s), *(s+1));
			//printf("diff '%c' '%c' = %d\n", *(s-1), *s, diff);
			x += diff * scale;
		}
		x += dpiScale * spacing;

		v = &texture->verts[texture->nverts*4];

		v = setv(v, q.x0, q.y0, q.s0, q.t0);
		v = setv(v, q.x1 - p, q.y0, q.s1 - tw, q.t0);
		v = setv(v, q.x1 - p, q.y1 - p, q.s1 - tw, q.t1 - tw);

		v = setv(v, q.x0, q.y0, q.s0, q.t0);
		v = setv(v, q.x1 - p, q.y1 - p, q.s1 - tw, q.t1 - tw);
		v = setv(v, q.x0, q.y1 - p, q.s0, q.t1 - tw);
		
		texture->nverts += 6;
		c++;
	}
	
	if (dx) *dx = x / dpiScale;
}

void ofx_sth_dim_text(struct ofx_sth_stash* stash,
				  int idx, float size,
				  const char* s,
				  float* minx, float* miny, float* maxx, float* maxy)
{
	unsigned int codepoint;
	struct ofx_sth_glyph* glyph = NULL;
	unsigned int state = 0;
	struct ofx_sth_quad q;
	short isize = (short)(size*10.0f);
	struct ofx_sth_font* fnt = NULL;
	float x = 0, y = 0;

	*minx = *maxx = *miny = *maxy = 0;	/* @rlyeh: reset vars before failing */

	if (stash == NULL) return;
	fnt = stash->fonts;
	while(fnt != NULL && fnt->idx != idx) fnt = fnt->next;
	if (fnt == NULL) return;
	if (fnt->type != BMFONT && !fnt->data) return;

	int len = strlen(s);
	float scale = ofx_stbtt_ScaleForPixelHeight(&fnt->font, size);
	int c = 0;
	float spacing = stash->charSpacing;
	int doKerning = stash->doKerning;
	float dpiScale = stash->dpiScale;
	
	for (; *s; ++s){
		if (decutf8(&state, &codepoint, *(unsigned char*)s)) continue;
		glyph = get_glyph(stash, fnt, codepoint, isize);
		if (!glyph) continue;
		if (!get_quad(stash, fnt, glyph, isize, &x, &y, &q)) continue;

		int diff = 0;
		if (c < len && doKerning > 0){
			diff = ofx_stbtt_GetCodepointKernAdvance(&fnt->font, *(s), *(s+1));
			//printf("diff '%c' '%c' = %d\n", *(s-1), *s, diff);
			x += diff * scale;
		}
		x += spacing;

		if (q.x0 < *minx) *minx = q.x0;
		if (q.x1 > *maxx) *maxx = q.x1;
		if (q.y1 > *miny) *miny = q.y1;		//oriol changed "<" direction bc its flipped to fit OF
		if (q.y0 < *maxy) *maxy = q.y0;		//idem
		c++;
	}
	if (x > *maxx) *maxx = x;
}

void ofx_sth_vmetrics(struct ofx_sth_stash* stash,
				  int idx, float size,
				  float* ascender, float* descender, float* lineh)
{
	struct ofx_sth_font* fnt = NULL;

	if (stash == NULL) return;
	fnt = stash->fonts;
	while(fnt != NULL && fnt->idx != idx) fnt = fnt->next;
	if (fnt == NULL) return;
	if (fnt->type != BMFONT && !fnt->data) return;
	if (ascender)
		*ascender = fnt->ascender*size;
	if (descender)
		*descender = fnt->descender*size;
	if (lineh)
		*lineh = fnt->lineh*size;
}

void ofx_sth_delete(struct ofx_sth_stash* stash)
{
	struct ofx_sth_texture* tex = NULL;
	struct ofx_sth_texture* curtex = NULL;
	struct ofx_sth_font* fnt = NULL;
	struct ofx_sth_font* curfnt = NULL;

	if (!stash) return;

	tex = stash->tt_textures;
	while(tex != NULL) {
		curtex = tex;
		tex = tex->next;
		if (curtex->id)
			glDeleteTextures(1, &curtex->id);
		free(curtex);
	}

	tex = stash->bm_textures;
	while(tex != NULL) {
		curtex = tex;
		tex = tex->next;
		if (curtex->id)
			glDeleteTextures(1, &curtex->id);
		free(curtex);
	}

	fnt = stash->fonts;
	while(fnt != NULL) {
		curfnt = fnt;
		fnt = fnt->next;
		if (curfnt->glyphs)
			free(curfnt->glyphs);
		if (curfnt->type == TTFONT_FILE && curfnt->data)
			free(curfnt->data);
		free(curfnt);
	}
	free(stash->empty_data);
	free(stash);
}


#ifdef __cplusplus
}
#endif
