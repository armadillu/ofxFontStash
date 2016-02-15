//
// Copyright (c) 2011 Andreas Krinke andreas.krinke@gmx.de
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

#ifndef OFX_FONTSTASH_H
#define OFX_FONTSTASH_H

#if __cplusplus
extern "C" {
#endif

struct ofx_sth_stash
{int tw,th;
	float itw,ith;
	GLubyte *empty_data;
	struct ofx_sth_texture* tt_textures;
	struct ofx_sth_texture* bm_textures;
	struct ofx_sth_font* fonts;
	int drawing;
	int padding; //oriol adding texture padding around chars to avoid mipmap neighbor leaks
	int hasMipMap; //oriol adding optional mipmap generation to each char
	int doKerning; //calc kerning on the fly and offset letters when drawing and / calcing box sizes
	float charSpacing;
	float dpiScale;
};


struct ofx_sth_stash* ofx_sth_create(int cachew, int cacheh, int createMipmaps, int charPadding, float dpiScale);

int ofx_sth_add_font(struct ofx_sth_stash* stash, const char* path);
int ofx_sth_add_font_from_memory(struct ofx_sth_stash* stash, unsigned char* buffer);

int  ofx_sth_add_bitmap_font(struct ofx_sth_stash* stash, int ascent, int descent, int line_gap);
void ofx_sth_add_glyph(struct ofx_sth_stash* stash, int idx, GLuint id, const char* s,  /* @rlyeh: function does not return int */
                  short size, short base, int x, int y, int w, int h,
                  float xoffset, float yoffset, float xadvance);

void ofx_sth_begin_draw(struct ofx_sth_stash* stash);
void ofx_sth_end_draw(struct ofx_sth_stash* stash);

void ofx_sth_draw_text(struct ofx_sth_stash* stash,
				   int idx, float size,
				   float x, float y, const char* string, float* dx);

void ofx_sth_dim_text(struct ofx_sth_stash* stash, int idx, float size, const char* string,
				  float* minx, float* miny, float* maxx, float* maxy);

void ofx_sth_vmetrics(struct ofx_sth_stash* stash,
				  int idx, float size,
				  float* ascender, float* descender, float * lineh);

void ofx_sth_delete(struct ofx_sth_stash* stash);

void set_lod_bias(struct ofx_sth_stash* stash, float bias);

#if __cplusplus
}
#endif

#endif // OFX_FONTSTASH_H

