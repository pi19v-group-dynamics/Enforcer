#include "ui.h"
#include "input.h"
#include "memalloc.h"

/******************************************************************************
 * UI context
 *****************************************************************************/

ui_context_t* ui_make_context(void)
{
	ui_context_t* ui = malloc(sizeof(ui_context_t));
	ui->hot = -1;
	ui->active = -1;
	ui->colors[UI_CALM_COLOR]   = (ren_pixel_t){.a = 0xFF, .r = 0x40, .g = 0x40, .b = 0x40};
	ui->colors[UI_HOT_COLOR]    = (ren_pixel_t){.a = 0xFF, .r = 0x80, .g = 0x80, .b = 0x80};
	ui->colors[UI_ACTIVE_COLOR] = (ren_pixel_t){.a = 0xFF, .r = 0xFF, .g = 0xFF, .b = 0xFF};
	ui->colors[UI_TEXT_COLOR]   = (ren_pixel_t){.a = 0xFF, .r = 0xAA, .g = 0xAA, .b = 0xAA};
	return ui;
}

void ui_free_context(ui_context_t* ctx)
{
	free(ctx);
}

/******************************************************************************
 * UI objects
 *****************************************************************************/

bool ui_button(ui_context_t* ctx, int id, const char* restrict text, int x, int y, int w, int h)
{
	/* Resolve state */
	if (inp_mouse_x >= x && inp_mouse_y >= y && inp_mouse_x < x + w && inp_mouse_y < y + h)
	{
		if (ctx->active == -1 && inp_ms_down(LEFT)) ctx->active = id;
		ctx->hot = id;
	}
	else if (ctx->hot == id) ctx->hot = -1;
	if (ctx->active == id && !inp_ms_down(LEFT)) ctx->active = -1;
	/* Render object */
	ren_push();
	ren_state->color = ctx->colors[ctx->active == id ? UI_ACTIVE_COLOR : ctx->hot == id ? UI_HOT_COLOR : UI_CALM_COLOR];
	ren_rect(x, y, w, h);
	ren_state->color = ctx->colors[UI_TEXT_COLOR];
	ren_print(text, x, y, &(ren_transform_t)
	{
		.sx = 1,
		.sy = 1,
		.ox = (w - strlen(text) * (ren_state->font->glyph_w + ren_state->font->hspacing)) / 2,
		.oy = (h - (ren_state->font->glyph_h + ren_state->font->vspacing)) / 2 
	});
	ren_pop();
	return ctx->active == id;
}

bool ui_slider(ui_context_t* ctx, int id, int x, int y, int w, int h, float* value);

