#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL_endian.h>
#include <stdint.h>

#ifndef REN_WIDTH
#define REN_WIDTH  136
#endif /* !REN_WIDTH */

#ifndef REN_HEIGHT
#define REN_HEIGHT 128
#endif /* !REN_HEIGHT */

/******************************************************************************
 * Typedef's
 *****************************************************************************/

typedef union ren_pixel
{
	uint_least32_t raw; /* raw 32-bit value */
	struct /* ARGB color */
	{
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
		uint_least8_t b, g, r, a;
#else /* SDL_BYTEORDER == SDL_BIG_ENDIAN */
		uint_least8_t a, r, g, b;
#endif /* SDL_BYTEORDER == SDL_LIL_ENDIAN */
	};
}
ren_pixel_t;

typedef void (*ren_blend_t)(ren_pixel_t*, ren_pixel_t);

typedef struct ren_rect
{
	int x, y, w, h;
}
ren_rect_t;

typedef struct ren_transform
{
	/* Direct access fields */
	float ang;        /* rotation angle */
	float sx, sy;     /* scaling factor */
	float ox, oy;     /* relative offset (in pixels) */
	/* Auto calculated fields */
	float sin, cos;   /* sin(ang), cos(ang) */
	int beg_x, beg_y; /* top left corner of bound rectangle */
	int end_x, end_y; /* bottom right corner of bound rectangle */
}
ren_transform_t;

typedef struct ren_buffer
{
	int width, height; /* width and height of buffer*/
	ren_pixel_t* data; /* buffer pixels */
}
ren_buffer_t;

typedef struct ren_font
{
	int pitch;                  /* number glyphs in line */
	int glyph_w, glyph_h;       /* size of glyph (monospace font) */
	const ren_buffer_t* buffer; /* source font image */
}
ren_font_t;

typedef struct ren_state
{
	struct { int x, y; } translate;
	ren_pixel_t color;
	ren_blend_t blend;
	ren_rect_t clip;
	ren_font_t* font;
	ren_buffer_t* target;
}
ren_state_t;

/******************************************************************************
 * Renderer state
 *****************************************************************************/

extern ren_buffer_t* const ren_screen; /* virtual framebuffer */
extern ren_state_t ren_state; /* render state */

ren_state_t ren_begin(void); /* lock renderer state */
void ren_end(ren_state_t st); /* unlock render state */
void ren_reset(void); /* resets render state */
void ren_flip(void); /* flip ren_screen buffers and render front buffer */

/******************************************************************************
 * Blend functions
 *****************************************************************************/

void ren_blend_alpha(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_replace(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_add(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_subtract(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_multiply(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_lighten(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_darken(ren_pixel_t* dst, ren_pixel_t src);
void ren_blend_screen(ren_pixel_t* dst, ren_pixel_t src);

/******************************************************************************
 * Image buffer
 *****************************************************************************/

ren_buffer_t* ren_blank_buffer(int width, int height);
ren_buffer_t* ren_copy_buffer(const ren_buffer_t* src);
ren_buffer_t* ren_shared_buffer(void* data, int width, int height);
ren_buffer_t* ren_load_buffer(const char filename[static 2]);
void ren_free_buffer(ren_buffer_t* buf);

/******************************************************************************
 * Font
 *****************************************************************************/

ren_font_t* ren_make_font(const ren_buffer_t* buf, int glyph_w, int glyph_h);
void ren_free_font(ren_font_t* font);

/******************************************************************************
 * Rendering routines
 *****************************************************************************/

void ren_fill(ren_pixel_t col); /* doesn't apply blending mode */
ren_pixel_t ren_peek(int x, int y);
void ren_plot(int x, int y);
void ren_rect(int x, int y, int w, int h);
void ren_box(int x, int y, int w, int h);
void ren_line(int x0, int y0, int x1, int x2);
void ren_circ(int x, int y, int r); 
void ren_ring(int x, int y, int r); 
void ren_recalc_transform(ren_transform_t* tr, const ren_rect_t* rect);
void ren_buffer(const ren_buffer_t* buf, int x, int y, const ren_rect_t* rect, const ren_transform_t* tr);
void ren_text(const char* text, int x, int y, int hsp, int vsp, int sx, int sy);

#endif /* RENDERER_H */

