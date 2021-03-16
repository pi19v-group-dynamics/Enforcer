#include "renderer.h"
#include <string.h>
#include <stdatomic.h>
#include <stdarg.h>
#include "system.h"
#include "memalloc.h"
#include "utils.h"
#include "fastmath.h"

/* Import stb_image.h, avoid warnings from it */
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
#define STBI_NO_GIF
#define STB_IMAGE_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "stb_image.h"
#pragma GCC diagnostic pop

/******************************************************************************
 * Internals
 *****************************************************************************/

__attribute__((always_inline, pure)) static inline int max(int a, int b)
{
	return a > b ? a : b;
}

__attribute__((always_inline, pure)) static inline int min(int a, int b)
{
	return a < b ? a : b;
}

/******************************************************************************
 * Renderer state
 *****************************************************************************/

static ren_pixel_t bitmaps[2][REN_WIDTH * REN_HEIGHT];
static int buf_cnt = 0;

ren_bitmap_t* const ren_screen = &(ren_bitmap_t)
{
	.width  = REN_WIDTH,
	.height = REN_HEIGHT,
	.data   = bitmaps[0]
};

const ren_transform_t* const REN_NULL_TRANSFORM = &(ren_transform_t)
{
	.ang = 0.0f, .sx = 1.0f, .sy = 1.0f, .ox = 0.0f, .oy = 0.0f
};

ren_state_t ren_state;
static atomic_flag lock_flag = ATOMIC_FLAG_INIT;

ren_state_t ren_begin(void)
{
	spinlock_lock(&lock_flag);
	return ren_state;
}

void ren_end(ren_state_t st)
{
	ren_state = st;
	spinlock_unlock(&lock_flag);
}

void ren_reset(void)
{
	ren_state = (ren_state_t)
	{
		.translate = {0, 0},
		.color = {.a = 0xFF, .r = 0x00, .g = 0x00, .b = 0x00},
		.blend = ren_blend_replace,
		.clip = {0, 0, REN_WIDTH, REN_HEIGHT},
		.font = NULL,
		.target = ren_screen
	};
}

void ren_flip(void)
{
	spinlock_lock(&lock_flag);
	const void* back = ren_screen->data;
	buf_cnt = (buf_cnt + 1) & 1;
	ren_screen->data = bitmaps[buf_cnt];
	sys_display(back, sizeof(ren_pixel_t[REN_WIDTH]));
	spinlock_unlock(&lock_flag);
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
 * Image bitmap
 *****************************************************************************/

ren_bitmap_t* ren_blank_bitmap(int width, int height)
{
	ren_bitmap_t* buf = malloc(sizeof(ren_bitmap_t) + sizeof(ren_pixel_t[width * height]));
	buf->width = width;
	buf->height = height;
	buf->data = (ren_pixel_t*)((char*)buf + sizeof(ren_bitmap_t)); 
	memset(buf->data, 0, sizeof(ren_pixel_t[width * height]));
	return buf;
}

ren_bitmap_t* ren_copy_bitmap(const ren_bitmap_t* src)
{
	ren_bitmap_t* restrict buf = malloc(sizeof(ren_bitmap_t) + sizeof(ren_pixel_t[src->width * src->height]));
	memcpy(buf, src, sizeof(ren_bitmap_t) + sizeof(ren_pixel_t[src->width * src->height]));
	return buf;
}

ren_bitmap_t* ren_shared_bitmap(void* data, int width, int height)
{
	ren_bitmap_t* buf = malloc(sizeof(ren_bitmap_t));
	buf->width = width;
	buf->height = height;
	buf->data = data;
	return buf;
}

ren_bitmap_t* ren_load_bitmap(const char filename[static 2])
{
	/* Load image */
	int w = 0, h = 0, comp = 0;
	unsigned char* image_data = stbi_load(filename, &w, &h, &comp, 0);
	confirm(image_data != NULL, "Failed to load '%s' file!", filename);
	/* Allocate new bitmap */
	ren_bitmap_t* buf = malloc(sizeof(ren_bitmap_t) + sizeof(ren_pixel_t[w][h]));
	buf->width = w;
	buf->height = h;
	buf->data = (ren_pixel_t*)((char*)buf + sizeof(ren_bitmap_t)); 
	/* Convert and copy pixel data */
	unsigned char* src = image_data;
	ren_pixel_t* dst = buf->data;
	switch (comp)
	{
		case 3: /* RGB */
			while (src < image_data + w * h * comp)
			{
				dst->a = 0xFF;
				dst->r = src[0];
				dst->g = src[1];
				dst->b = src[2];
			  src += comp, ++dst;
			}
			break;
		case 4: /* RGBA */
			while (src < image_data + w * h * comp)
			{
				dst->a = src[3];
				dst->r = src[0];
				dst->g = src[1];
				dst->b = src[2];
			  src += comp, ++dst;
			}
			break;
		default: /* not supported format! */
			error("Image '%s' has unsupported pixel format!", filename);
			break;
	}
	/* Free image data */
	stbi_image_free(image_data);
	return buf;
}

inline void ren_free_bitmap(ren_bitmap_t* buf)
{
	free(buf);
}

/******************************************************************************
 * Font
 *****************************************************************************/

ren_font_t* ren_make_font(const ren_bitmap_t* buf, int glyph_w, int glyph_h)
{
	ren_font_t* font = malloc(sizeof(ren_font_t));
	font->glyph_w = glyph_w;
	font->glyph_h = glyph_h;
	font->vspacing = 1;
	font->hspacing = 1;
	font->tabwidth = 1;
	font->bitmap = buf;
	font->pitch = buf->width / glyph_w;
	return font;
}

inline void ren_free_font(ren_font_t* font)
{
	free(font);
}

/******************************************************************************
 * Batch 
 *****************************************************************************/

ren_batch_t* ren_make_batch(const ren_bitmap_t* bmp, const ren_transform_t* tr, const ren_rect_t* rect, unsigned cap)
{
	ren_batch_t* bat = malloc(sizeof(ren_batch_t) + (sizeof(int[cap]) << 1));
	bat->bitmap = bmp;
	bat->transform = tr;
	bat->rect = rect;
	bat->capacity = cap;
	bat->count = 0;
	return bat;
}

void ren_batch_add(ren_batch_t* bat, int x, int y)
{
	if (bat->count >= bat->capacity)
	{
		++bat->capacity;
		bat = realloc(bat, sizeof(ren_batch_t) + (sizeof(int[bat->capacity]) << 1));	
	}
	bat->positions[(bat->count << 1)    ] = x;
	bat->positions[(bat->count << 1) + 1] = y;
	++bat->count;
}

inline void ren_free_batch(ren_batch_t* bat)
{
	free(bat);
}

/******************************************************************************
 * Rendering routines
 *****************************************************************************/

void ren_fill(ren_pixel_t col)
{
	ren_pixel_t* p = ren_state.target->data + ren_state.clip.y;
	while (p < ren_state.target->data + (ren_state.clip.y + ren_state.clip.h) * ren_state.target->width)
	{
		for (int i = 0; i < ren_state.clip.w; ++i, *p++ = col);
	}
}

ren_pixel_t ren_peek(int x, int y)
{
	if (x < ren_state.clip.x || x >= ren_state.clip.x + ren_state.clip.w 
	||  y < ren_state.clip.y || y >= ren_state.clip.y + ren_state.clip.h)
	{
		return (ren_pixel_t){.a = 0xFF, .r = 0x00, .g = 0x00, .b = 0x00};
	}
	return ren_state.target->data[x + y * ren_state.target->width];	
}

__attribute__((always_inline)) inline void ren_plot(int x, int y)
{
	x += ren_state.translate.x;
	y += ren_state.translate.y;
	if (x >= ren_state.clip.x && x < ren_state.clip.x + ren_state.clip.w
	&&  y >= ren_state.clip.y && y < ren_state.clip.y + ren_state.clip.h)
	{
		ren_state.blend(ren_state.target->data + x + y * ren_state.target->width, ren_state.color);
	}
}

void ren_rect(int x, int y, int w, int h)
{
	x += ren_state.translate.x;
	y += ren_state.translate.y;
	int x1 = x + w; w = ren_state.clip.x + ren_state.clip.w;
	if (x1 > w) x1 = w;
	int y1 = y + h; h = ren_state.clip.y + ren_state.clip.h;
	if (y1 > h) y1 = h;
	if (x < ren_state.clip.x) x = ren_state.clip.x; 	
	if (y < ren_state.clip.y) y = ren_state.clip.y; 
	while (y < y1)
	{
		for (ren_pixel_t* p = ren_state.target->data + x + y * ren_state.target->width;
		     p < ren_state.target->data + x1 + y * ren_state.target->width;
				 ren_state.blend(p++, ren_state.color));
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

void ren_circ(int px, int py, int r)
{
	int sqr_r = r * r;
	for (int y = -r; y <= r; ++y)
	{
		int sqr_y = y * y;
		for (int x = -r; x <= r; ++x)
		{
			if (x * x + sqr_y <= sqr_r)
			{
				ren_plot(x + px, y + py);
			}
		}
	}
}

void ren_ring(int x, int y, int r)
{
	int dx = abs(r);
	int dy = 0;
	int radius_err = 1 - dx;
	if (x + dx < ren_state.clip.x || x - dx > ren_state.clip.x + ren_state.clip.w ||
			y + dx < ren_state.clip.y || y - dx > ren_state.clip.y + ren_state.clip.h) return;
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
		++dy;
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

void ren_recalc_transform(ren_transform_t* tr, const ren_rect_t* rect)
{
	tr->sin = -fastsin(tr->ang);
	tr->cos = fastcos(tr->ang);
	/* Precalculate factors */
	float sin_sx = tr->sin * tr->sx, sin_sy = tr->sin * tr->sy;
	float cos_sx = tr->cos * tr->sx, cos_sy = tr->cos * tr->sy;
	/* Translate points to origin */
	float x0 =          -tr->ox, y0 =          -tr->oy;
	float x1 = rect->w - tr->ox, y1 =          -tr->oy;
	float x2 =          -tr->ox, y2 = rect->h - tr->oy;
	float x3 = rect->w - tr->ox, y3 = rect->h - tr->oy;
	/* */
	/* Rotate points around origin */
	#define rotate(n) do {                    \
			float tmpx = x##n;                    \
			x##n = cos_sx * tmpx + sin_sy * y##n; \
			y##n = cos_sy * y##n - sin_sx * tmpx; \
		} while (0)
	rotate(0); rotate(1); rotate(2); rotate(3);
	#undef rotate
	/* Find top left and bottom right points of boundary rectangle and clip */
	tr->beg_x = fmin(x0, fmin(x1, fmin(x2, x3)));
	tr->beg_y = fmin(y0, fmin(y1, fmin(y2, y3)));
	tr->end_x = fmax(x0, fmax(x1, fmax(x2, x3)));
	tr->end_y = fmax(y0, fmax(y1, fmax(y2, y3)));
}

void ren_blit(const ren_bitmap_t* bmp, int px, int py, const ren_rect_t* rect, const ren_transform_t* tr)
{
	/* Translate position */
	px += ren_state.translate.x;
	py += ren_state.translate.y;
	/* Clip bound rectangle to fit it on screen */
	int beg_x = max(-px, tr->beg_x);
	int beg_y = max(-py, tr->beg_y);
	int end_x = min(tr->end_x, REN_WIDTH - px);
	int end_y = min(tr->end_y, REN_HEIGHT - py);
	/* For each point of boundary */
	for (int ty = beg_y; ty < end_y; ++ty)
	{
		int tar_y = (ty + py) * ren_state.target->width; /* precalculated y-offset */
		for (int tx = beg_x; tx < end_x; ++tx)
		{
			/* Derotate point */
			float drx = (tx * tr->cos - ty * tr->sin) / tr->sx + tr->ox;
			float dry = (tx * tr->sin + ty * tr->cos) / tr->sy + tr->oy;
			if (drx >= 0.0f && dry >= 0.0f && drx < rect->w && dry < rect->h)
			{
				/* Blend pixel */
				ren_state.blend(ren_state.target->data + tx + px + tar_y,
					bmp->data[(int)drx + rect->x + ((int)dry + rect->y) * bmp->width]);
			}
		} 
	} 
}

static inline void blit_scaled(const ren_bitmap_t* buf, const ren_rect_t* rect, int px, int py, int sx, int sy)
{
	/* Clip scaled rect */
	int end_x = min(px + rect->w * sx, REN_WIDTH);
	int end_y = min(py + rect->h * sy, REN_HEIGHT);
	/* Blend pixels */
	for (int y = py; y < end_y; ++y)
	{
		ren_pixel_t* dst = ren_state.target->data + y * ren_state.target->width;
		const ren_pixel_t* src = buf->data + ((y - py) / sy + rect->y) * buf->width;
		for (int x = px; x < end_x; ++x)
		{
			if (src[(x - px) / sx + rect->x].a != 0)
			{
				ren_state.blend(dst + x, ren_state.color);
			}
		}
	}
}

void ren_print(const char txt[static 2], int len, int x, int y, const ren_transform_t* tr)
{
	if (len == 0) len = INT_MAX; 
	ren_rect_t rect = {.w = ren_state.font->glyph_w, .h = ren_state.font->glyph_h}; 	
	x += ren_state.translate.x - tr->ox;
	y += ren_state.translate.y - tr->oy;
	int shift = ren_state.font->hspacing + tr->sx * rect.w;
	for (int i = 0; txt[i] != '\0' && i < len; ++i)
	{
		rect.x = (txt[i] % ren_state.font->pitch) * ren_state.font->glyph_w;
		rect.y = (txt[i] / ren_state.font->pitch) * ren_state.font->glyph_h;
		blit_scaled(ren_state.font->bitmap, &rect, x + i * shift, y, tr->sx, tr->sy);
	}
}

void ren_printf(const char txt[static 2], int len, int limit, int x, int y, const ren_transform_t* tr, ...)
{
	if (len == 0) len = INT_MAX; 
	char parsed[strlen(txt) + 1];
	va_list va;
	va_start(va, tr);
	vsnprintf(parsed, sizeof(parsed), txt, va);
	va_end(va);
	ren_rect_t rect = {.w = ren_state.font->glyph_w, .h = ren_state.font->glyph_h}; 	
	x += ren_state.translate.x - tr->ox;
	y += ren_state.translate.y - tr->oy;
	int shift_w = ren_state.font->hspacing + tr->sx * rect.w;
	int shift_h = ren_state.font->vspacing + tr->sy * rect.h;
	int cur_x = x;
	int cur_y = y;
	for (int i = 0, wr = 1; i < len; ++i, ++wr)
	{
		switch (parsed[i])
		{
			case '\0':
				return;
			case '\t':
			{
				int num_spaces = (ren_state.font->tabwidth - (((cur_x - x) % shift_w) % ren_state.font->tabwidth));
				cur_x += num_spaces * shift_w;
				wr += num_spaces - 1;
				break;
			}
			case '\n':
				wr = 0;
				cur_x = x;
				cur_y += shift_h;
				break;
			default:
				rect.x = (txt[i] % ren_state.font->pitch) * ren_state.font->glyph_w;
				rect.y = (txt[i] / ren_state.font->pitch) * ren_state.font->glyph_h;
				blit_scaled(ren_state.font->bitmap, &rect, cur_x, cur_y, tr->sx, tr->sy);
				cur_x += shift_w;
				break;
		}
		if (limit != 0 && parsed[i] != '\n' && wr >= limit)
		{
			wr = 0;
			cur_x = x;
			cur_y += shift_h;
		}
	}
}

void ren_flush(const ren_batch_t* bat, int x, int y)
{
	/* For each point of boundary */
	for (int ty = bat->transform->beg_y; ty < bat->transform->end_y; ++ty)
	{
		for (int tx = bat->transform->beg_x; tx < bat->transform->end_x; ++tx)
		{
			/* Derotate point */
			float drx = (tx * bat->transform->cos - ty * bat->transform->sin) / bat->transform->sx + bat->transform->ox;
			float dry = (tx * bat->transform->sin + ty * bat->transform->cos) / bat->transform->sy + bat->transform->oy;
			if (drx >= 0.0f && dry >= 0.0f && drx < bat->rect->w && dry < bat->rect->h)
			{
				ren_state.color = bat->bitmap->data[(int)drx + bat->rect->x + ((int)dry + bat->rect->y) * bat->bitmap->width];
				for (unsigned i = 0; i < bat->count; ++i)
				{
					ren_plot(bat->positions[(i << 1)    ] + x + tx,
					         bat->positions[(i << 1) + 1] + y + ty);
				}
			}
		} 
	}
}

