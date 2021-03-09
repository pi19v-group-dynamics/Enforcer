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
	float ang;    /* angle */
	// float shx, shy; /* shearing */
	float sx, sy; /* scale */
	float ox, oy; /* offset */
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
	int glyph_w, glyph_h; /* size of glyph (monospace font) */
	const ren_buffer_t* buffer; /* source font image */
}
ren_font_t;

typedef struct ren_batch ren_batch_t;

typedef unsigned ren_tile_t;

typedef struct ren_tileset
{
	const ren_buffer_t* buffer;
	int tile_w, tile_h;
	int num_tiles;
	int (*tiles)[2]; /* [0] - x, [1] - y position of tile */
}
ren_tileset_t;

typedef struct ren_tilemap
{
	ren_tile_t** layers;
	int num_layers;
	int width, height; /* map width and height in tiles */
	const ren_tileset_t* tileset;
}
ren_tilemap_t;

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
extern const ren_transform_t* const REN_NULL_TRANSFORM; /* default transform */
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
 * Buffer batch
 *****************************************************************************/

ren_batch_t* ren_make_batch(int size_w, int size_h);
void ren_recalc_batch(const ren_transform_t* tr);
void ren_free_batch(ren_batch_t* bat);
void ren_push_batch(ren_batch_t* bat, int pos_x, int pos_y, int buf_x, int buf_y, const ren_buffer_t* buf);

/******************************************************************************
 * Tilemap
 *****************************************************************************/

ren_tileset_t* ren_make_tileset(const ren_buffer_t* buf, int tile_w, int tile_h);
ren_tilemap_t* ren_make_tilemap(const ren_tileset_t* ts, int width, int height, int num_layers);
void ren_free_tileset(ren_tileset_t* ts); 
void ren_free_tilemap(ren_tilemap_t* tm); 

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
void ren_buffer(const ren_buffer_t* buf, int x, int y, const ren_rect_t* rect, const ren_transform_t* tr);
void ren_text(const char* text, int x, int y, const ren_transform_t* tr);
void ren_batch(const ren_batch_t* bat, int x, int y);
void ren_tilemap(const ren_tilemap_t* tm, int x, int y, const ren_rect_t* rect, const ren_transform_t* tr);

#endif /* RENDERER_H */

