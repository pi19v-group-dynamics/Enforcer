#include "ui.h"
#include "input.h"
#include "memalloc.h"
#include <math.h>

/******************************************************************************
 * UI context
 *****************************************************************************/

ui_context_t* ui_make_context(void)
{
	ui_context_t* ui = malloc(sizeof(ui_context_t));
	ui->hover = -1;
	ui->press = -1;
	ui->hold  = -1;
	ui->slider_size = 3;
	ui->check_border = 2;
	ui->colors[UI_NONE_COLOR]  = (ren_pixel_t){.a = 0xFF, .r = 0x40, .g = 0x40, .b = 0x40};
	ui->colors[UI_HOVER_COLOR] = (ren_pixel_t){.a = 0xFF, .r = 0x80, .g = 0x80, .b = 0x80};
	ui->colors[UI_PRESS_COLOR] = (ren_pixel_t){.a = 0xFF, .r = 0xFF, .g = 0xFF, .b = 0xFF};
	ui->colors[UI_TEXT_COLOR]  = (ren_pixel_t){.a = 0xFF, .r = 0xAA, .g = 0xAA, .b = 0xAA};
	return ui;
}

void ui_free_context(ui_context_t* ctx)
{
	free(ctx);
}

/******************************************************************************
 * UI objects
 *****************************************************************************/

static inline int update(ui_context_t* ctx, int id, int x, int y, int w, int h)
{
	x += ren_state->translate.x;
	y += ren_state->translate.y;
	int state = UI_NONE;
	if (ctx->press == id) ctx->press = -1;
	if (inp_mouse_x >= x && inp_mouse_x < x + w && inp_mouse_y >= y && inp_mouse_y < y + h && ctx->hold == -1)
	{
		state |= UI_HOVER;
		ctx->hover = id;
		if (inp_ms_down(LEFT))
		{
			if (ctx->hold == -1)
			{
				state |= UI_PRESS;
				ctx->hold = id;
				ctx->press = id;
			}
		}
		else if (ctx->hold == id)
		{
			ctx->hold = -1;
		}
	}
	else if (!inp_ms_down(LEFT) && ctx->hold == id)
	{
		ctx->hold = -1;
	}
	if (ctx->hold == id) state |= UI_HOLD;
	return state;
}

void ui_label(ui_context_t* ctx, const char* restrict text, int x, int y)
{
	ren_push();
	ren_state->color = ctx->colors[UI_TEXT_COLOR];
	ren_print(text, x - (strlen(text) * (ren_state->font->glyph_w + ren_state->font_spacing)) / 2, y - ren_state->font->glyph_h / 2, &(ren_transform_t){.sx = 1, .sy = 1});
	ren_pop();
}

void ui_string(ui_context_t* ctx, const char* restrict text, int x, int y)
{
	ren_push();
	ren_state->color = ctx->colors[UI_TEXT_COLOR];
	ren_print(text, x, y - ren_state->font->glyph_h / 2, &(ren_transform_t){.sx = 1, .sy = 1});
	ren_pop();
}

ui_result_t ui_button(ui_context_t* ctx, int id, const char* restrict text, int x, int y, int w, int h)
{
	ui_result_t state = update(ctx, id, x, y, w, h);
	ren_push();
	ren_state->color = ctx->colors[state & (UI_PRESS | UI_HOLD) ? UI_PRESS_COLOR : state & UI_HOVER ? UI_HOVER_COLOR : UI_NONE_COLOR];
	ren_rect(x, y, w, h);
	ui_label(ctx, text, x + w / 2, y + h / 2);
	ren_pop();
	return state; 
}

ui_result_t ui_slider(ui_context_t* ctx, int id, int x, int y, int w, int h, float* value)
{
	ui_result_t state = update(ctx, id, x, y, w, h);
	if (state & UI_HOLD)
	{
		*value = fmin(fmax(w > h ? (float)(inp_mouse_x - x) / (float)w : (float)(inp_mouse_y - y) / (float)h, 0.0), 1.0);
	}
	ren_push();
	ren_state->color = ctx->colors[UI_NONE_COLOR];
	ren_rect(x, y, w, h);
	ren_state->color = ctx->colors[state & (UI_PRESS | UI_HOLD) ? UI_PRESS_COLOR : UI_HOVER_COLOR];
	ren_rect(x + *value * (w - ctx->slider_size), y, ctx->slider_size, h);
	ren_pop();
	return state;
}

ui_result_t ui_checkbox(ui_context_t* ctx, int id, int x, int y, int w, int h, bool* value)
{
	ui_result_t state = update(ctx, id, x, y, w, h);
	if (state & UI_PRESS) *value = !*value;
	ren_push();
	ren_state->color = ctx->colors[UI_NONE_COLOR];
	ren_rect(x, y, w, h);
	if (*value == true)
	{
		ren_state->color = ctx->colors[UI_HOVER_COLOR];
		ren_rect(x + ctx->check_border, y + ctx->check_border, w - ctx->check_border * 2, h - ctx->check_border * 2);
	}
	ren_pop();
	return state;
}

