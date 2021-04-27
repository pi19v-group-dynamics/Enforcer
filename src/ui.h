#pragma once

#include <stdbool.h>
#include "renderer.h"

enum
{
	UI_NONE_COLOR,
	UI_HOVER_COLOR,
	UI_PRESS_COLOR,
	UI_TEXT_COLOR,
	UI_COLORS_COUNT
};

typedef struct ui_context
{
	int hover, press, hold;
	int slider_size;
	int check_border;
	ren_pixel_t colors[UI_COLORS_COUNT];
}
ui_context_t;

/******************************************************************************
 * UI context
 *****************************************************************************/

ui_context_t* ui_make_context(void);
void ui_free_context(ui_context_t* ctx);

/******************************************************************************
 * UI objects
 *****************************************************************************/

typedef enum ui_result
{
	UI_NONE  = 0,
	UI_HOVER = 1 << 0,
	UI_PRESS = 1 << 1,
	UI_HOLD  = 1 << 2
}
ui_result_t;

void ui_label(ui_context_t* ctx, const char* restrict text, int x, int y);
void ui_string(ui_context_t* ctx, const char* restrict text, int x, int y);
ui_result_t ui_button(ui_context_t* ctx, int id, const char* restrict text, int x, int y, int w, int h);
ui_result_t ui_slider(ui_context_t* ctx, int id, int x, int y, int w, int h, float* value);
ui_result_t ui_checkbox(ui_context_t* ctx, int id, int x, int y, int w, int h, bool* value);

