#pragma once

#include <stdbool.h>
#include "renderer.h"

enum
{
	UI_CALM_COLOR,
	UI_HOT_COLOR,
	UI_ACTIVE_COLOR,
	UI_TEXT_COLOR,
	UI_COLORS_COUNT
};

typedef struct ui_context
{
	int hot, active;
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

bool ui_button(ui_context_t* ctx, int id, const char* restrict text, int x, int y, int w, int h);
bool ui_slider(ui_context_t* ctx, int id, int x, int y, int w, int h, float* value);

