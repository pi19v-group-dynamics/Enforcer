#include "renderer.h"
#include "system.h"
#include <string.h>
#include <SDL2/SDL.h>
#include "memalloc.h"
#include "utils.h"
#include "fastmath.h"

#define REN_WIDTH  136
#define REN_HEIGHT 128

__attribute__((always_inline, pure)) static inline int max(int a, int b)
{
	return a > b ? a : b;
}

__attribute__((always_inline, pure)) static inline int min(int a, int b)
{
	return a < b ? a : b;
}

static ren_pixel_t buffers[2][REN_WIDTH * REN_HEIGHT];
static int buf_cnt = 0;

ren_buffer_t* const ren_screen = &(ren_buffer_t)
{
	.width  = REN_WIDTH,
	.height = REN_HEIGHT,
	.data   = buffers[0]
};

const ren_transform_t* const REN_NULL_TRANSFORM = &(ren_transform_t)
{
	.ang = 0.0f, .sx = 1.0f, .sy = 1.0f, .ox = 0.0f, .oy = 0.0f
};

/******************************************************************************
 * Renderer state
 *****************************************************************************/

int ren_translate_x, ren_translate_y;
ren_pixel_t ren_color;
ren_blend_t ren_blend;
ren_rect_t ren_clip;
ren_buffer_t* ren_target;

void ren_reset(void)
{
	ren_translate_x = ren_translate_y = 0;
	ren_color = (ren_pixel_t){.a = 0xFF, .r = 0x00, .g = 0x00, .b = 0x00};
	ren_blend = ren_blend_replace;
	ren_clip = (ren_rect_t){.x = 0, .y = 0, .w = REN_WIDTH, .h = REN_HEIGHT};
	ren_target = ren_screen;
}

void ren_flip(void)
{
	const void* back = ren_screen->data;
	buf_cnt = (buf_cnt + 1) & 1;
	ren_screen->data = buffers[buf_cnt];
	extern SDL_Renderer* _sys_renderer;
	extern SDL_Texture* _sys_texture;
	SDL_UpdateTexture(_sys_texture, NULL, back, sizeof(ren_pixel_t[ren_screen->width]));
	SDL_RenderClear(_sys_renderer);
	SDL_RenderCopy(_sys_renderer, _sys_texture, NULL, NULL);
	SDL_RenderPresent(_sys_renderer);
}

/******************************************************************************
 * Blend functions
 *****************************************************************************/

__attribute__((always_inline, pure)) static inline uint_fast8_t color_add(uint_fast8_t a, uint_fast8_t b)
{
	return min(a + b, 0xFF);
}

__attribute__((always_inline, pure)) static inline uint_fast8_t color_sub(uint_fast8_t a, uint_fast8_t b)
{
	return max(a - b, 0x00);
}

__attribute__((always_inline, pure)) static inline uint_fast8_t color_mul(uint_fast8_t a, uint_fast8_t b)
{
	return (a * b) >> 8;
}

void ren_blend_alpha(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = color_add(color_mul(dst->r, 0xFF - src.a), color_mul(src.r, src.a));
	dst->g = color_add(color_mul(dst->g, 0xFF - src.a), color_mul(src.g, src.a));
	dst->b = color_add(color_mul(dst->b, 0xFF - src.a), color_mul(src.b, src.a));
	dst->a = color_add(color_mul(dst->a, 0xFF - src.a), src.a);
}

void ren_blend_replace(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = color_mul(src.r, src.a);
	dst->g = color_mul(src.g, src.a);
	dst->b = color_mul(src.b, src.a);
	dst->a = src.a;
}

void ren_blend_add(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = color_add(dst->r, color_mul(src.r, src.a));
	dst->g = color_add(dst->g, color_mul(src.g, src.a));
	dst->b = color_add(dst->b, color_mul(src.b, src.a));
}

void ren_blend_subtract(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = color_sub(dst->r, color_mul(src.r, src.a)); 
	dst->g = color_sub(dst->g, color_mul(src.g, src.a)); 
	dst->b = color_sub(dst->b, color_mul(src.b, src.a)); 
}

void ren_blend_multiply(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = color_mul(src.r, dst->r);
	dst->g = color_mul(src.g, dst->g);
	dst->b = color_mul(src.b, dst->b);
	dst->a = color_mul(src.a, dst->a);
}

void ren_blend_lighten(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = src.r > dst->r ? src.r : dst->r;
	dst->g = src.g > dst->g ? src.g : dst->g;
	dst->b = src.b > dst->b ? src.b : dst->b;
	dst->a = src.a > dst->a ? src.a : dst->a;
}

void ren_blend_darken(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = src.r < dst->r ? src.r : dst->r;
	dst->g = src.g < dst->g ? src.g : dst->g;
	dst->b = src.b < dst->b ? src.b : dst->b;
	dst->a = src.a < dst->a ? src.a : dst->a;
}

void ren_blend_screen(ren_pixel_t* dst, ren_pixel_t src)
{
	dst->r = color_add(color_mul(dst->r, (0xFF - src.r)), color_mul(src.r, src.a));
	dst->g = color_add(color_mul(dst->g, (0xFF - src.g)), color_mul(src.g, src.a));
	dst->b = color_add(color_mul(dst->b, (0xFF - src.b)), color_mul(src.b, src.a));
	dst->a = color_add(color_mul(dst->a, (0xFF - src.a)), src.a);
}

/******************************************************************************
 * Constructors
 *****************************************************************************/

ren_buffer_t* ren_blank_buffer(int width, int height)
{
	ren_buffer_t* buf = malloc(sizeof(ren_buffer_t) + sizeof(ren_pixel_t[width * height]));
	buf->width = width;
	buf->height = height;
	buf->data = (ren_pixel_t*)((char*)buf + sizeof(ren_buffer_t)); 
	memset(buf->data, 0, sizeof(ren_pixel_t[width * height]));
	return buf;
}

ren_buffer_t* ren_copy_buffer(const ren_buffer_t* src)
{
	ren_buffer_t* restrict buf = malloc(sizeof(ren_buffer_t) + sizeof(ren_pixel_t[src->width * src->height]));
	memcpy(buf, src, sizeof(ren_buffer_t) + sizeof(ren_pixel_t[src->width * src->height]));
	return buf;
}

ren_buffer_t* ren_shared_buffer(void* data, int width, int height)
{
	ren_buffer_t* buf = malloc(sizeof(ren_buffer_t));
	buf->width = width;
	buf->height = height;
	buf->data = data;
	return buf;
}

ren_buffer_t* ren_load_buffer(const char filename[static 2])
{
	SDL_Surface* surf = SDL_LoadBMP(filename);
	confirm(surf, "Failed to load '%s' file!", filename);
	surf = SDL_ConvertSurfaceFormat(surf, SYS_PIXEL_FMT, 0);
	ren_buffer_t* buf = malloc(sizeof(ren_buffer_t) + sizeof(ren_pixel_t[surf->w][surf->h]));
	buf->width = surf->w;
	buf->height = surf->h;
	buf->data = (ren_pixel_t*)((char*)buf + sizeof(ren_buffer_t)); 
	memcpy(buf->data, surf->pixels, sizeof(ren_pixel_t[surf->w][surf->h]));
	SDL_FreeSurface(surf);
	return buf;
}

inline void ren_free_buffer(ren_buffer_t* buf)
{
	free(buf);
}

/******************************************************************************
 * Rendering routines
 *****************************************************************************/

void ren_fill(ren_pixel_t col)
{
	ren_pixel_t* p = ren_target->data + ren_clip.y;
	while (p < ren_target->data + (ren_clip.y + ren_clip.h) * ren_target->width)
	{
		for (int i = 0; i < ren_clip.w; ++i, *p++ = col);
	}
}

ren_pixel_t ren_peek(int x, int y)
{
	if (x < ren_clip.x || y < ren_clip.y || x >= ren_clip.x + ren_clip.w || y >= ren_clip.y + ren_clip.h)
	{
		return (ren_pixel_t){.a = 0xFF, .r = 0x00, .g = 0x00, .b = 0x00};
	}
	return ren_target->data[x + y * ren_target->width];	
}

void ren_plot(int x, int y)
{
	x += ren_translate_x;
	y += ren_translate_y;
	if (x >= ren_clip.x && y >= ren_clip.y && x < ren_clip.x + ren_clip.w && y < ren_clip.y + ren_clip.h)
	{
		ren_blend(ren_target->data + x + y * ren_target->width, ren_color);
	}
}

void ren_rect(int x, int y, int w, int h)
{
	x += ren_translate_x;
	y += ren_translate_y;
	int x1 = x + w; w = ren_clip.x + ren_clip.w;
	if (x1 > w) x1 = w;
	int y1 = y + h; h = ren_clip.y + ren_clip.h;
	if (y1 > h) y1 = h;
	if (x < ren_clip.x) x = ren_clip.x; 	
	if (y < ren_clip.y) y = ren_clip.y; 
	while (y < y1)
	{
		for (ren_pixel_t* p = ren_target->data + x + y * ren_target->width;
		     p < ren_target->data + x1 + y * ren_target->width;
				 ren_blend(p++, ren_color));
		++y;
	}
}

void ren_box(int x, int y, int w, int h)
{
	ren_rect(x, y, 1, h);
	ren_rect(x + w - 1, y, 1, h);
	w -= 2;
	++x;
	ren_rect(x, y, w, 1);
	ren_rect(x, y + h - 1, w, 1);
}

void ren_line(int x0, int y0, int x1, int y1)
{
	int steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep)
	{
		int tmp = x0; x0 = y0; y0 = tmp;
		tmp = x1; x1 = y1; y1 = tmp;
	}
	if (x0 > x1)
	{
		int tmp = x0; x0 = x1; x1 = tmp;
		tmp = y0; y0 = y1; y1 = tmp;
	}
	int deltax = x1 - x0;
	int deltay = abs(y1 - y0);
	int error = deltax >> 1;
	int ystep = y0 < y1 ? 1 : -1;
	int y = y0;
	for (int x = x0; x <= x1; ++x)
	{
		if (steep)
		{
			ren_plot(y, x);
		}
		else
		{
			ren_plot(x, y);
		}
		error -= deltay;
		if (error < 0)
		{
			y += ystep;
			error += deltax;
		}
	}
}

void ren_circ(int x, int y, int r)
{
	int dx = abs(r);
	int dy = 0;
	int radius_err = 1 - dx;
	unsigned rows[512] = {0};
	if (x + dx < ren_clip.x || x - dx > ren_clip.x + ren_clip.w ||
	    y + dx < ren_clip.y || y - dx > ren_clip.y + ren_clip.h) return;
	for (int y_tmp; dx >= dy; )
	{
		#define draw_row(x, y, len) \
			y_tmp = y;                                                 \
			if (y_tmp >= 0 && ~rows[y_tmp >> 5] & (1 << (y_tmp & 31))) \
			{                                                          \
				ren_rect(x, y_tmp, len, 1);                              \
				rows[y_tmp >> 5] |= 1 << (y_tmp & 31);                   \
			}
		draw_row(x - dx, y + dy, dx << 1)
		draw_row(x - dx, y - dy, dx << 1)
		draw_row(x - dy, y + dx, dy << 1)
		draw_row(x - dy, y - dx, dy << 1)
		#undef draw_row
		dy++;
		if (radius_err < 0)
		{
			radius_err += 2 * dy + 1;
		}
		else
		{
			dx--;
			radius_err += 2 * (dy - dx + 1);
		}
	}
}


void ren_ring(int x, int y, int r)
{
	int dx = abs(r);
	int dy = 0;
	int radius_err = 1 - dx;
	if (x + dx < ren_clip.x || x - dx > ren_clip.x + ren_clip.w ||
			y + dx < ren_clip.y || y - dx > ren_clip.y + ren_clip.h) return;
	while (dx >= dy)
	{
		ren_plot( dx + x,  dy + y);
		ren_plot( dy + x,  dx + y);
		ren_plot(-dx + x,  dy + y);
		ren_plot(-dy + x,  dx + y);
		ren_plot(-dx + x, -dy + y);
		ren_plot(-dy + x, -dx + y);
		ren_plot( dx + x, -dy + y);
		ren_plot( dy + x, -dx + y);
		dy++;
		if (radius_err < 0)
		{
			radius_err += 2 * dy + 1;
		}
		else
		{
			--dx;
			radius_err += 2 * (dy - dx + 1);
		}
	}
}

void ren_blit(const ren_buffer_t* buf, int px, int py, const ren_rect_t* rect, const ren_transform_t* tr)
{
	(void) buf;
	(void) px;
	(void) py;
	(void) rect;
	(void) tr;
	/* TODO: implement */
}

