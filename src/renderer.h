#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL_endian.h>
#include <stdint.h>
#include <stdbool.h>

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
	float ang;      /* angle */
	float sx, sy;   /* scale */
	float ox, oy;   /* offset */
}
ren_transform_t;

typedef struct ren_buffer
{
	int width, height; /* width and height of buffer*/
	ren_pixel_t* data; /* buffer pixels */
}
ren_buffer_t;

extern ren_buffer_t* const ren_screen; /* virtual framebuffer */
extern const ren_transform_t* const REN_NULL_TRANSFORM; /* default transform */

/******************************************************************************
 * Renderer state
 *****************************************************************************/

extern int ren_translate_x,
           ren_translate_y;      /* actual translate */
extern ren_pixel_t ren_color;    /* actual render color */
extern ren_blend_t ren_blend;    /* actual blend mode */
extern ren_rect_t ren_clip;      /* actual clip area */
extern ren_buffer_t* ren_target; /* actual rendering target */

void ren_reset(void); /* reset rendering state */ 
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
 * Constructors
 *****************************************************************************/

ren_buffer_t* ren_blank_buffer(int width, int height);
ren_buffer_t* ren_copy_buffer(const ren_buffer_t* src);
ren_buffer_t* ren_shared_buffer(void* data, int width, int height);
ren_buffer_t* ren_load_buffer(const char filename[static 2]);
void ren_free_buffer(ren_buffer_t* buf);

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
void ren_blit(const ren_buffer_t* buf, int x, int y, const ren_rect_t* rect, const ren_transform_t* tr);

#endif /* RENDERER_H */

