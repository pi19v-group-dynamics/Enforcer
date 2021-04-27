#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

/* Input caches */
extern bool inp_keyboard_down[SDL_NUM_SCANCODES];
extern bool inp_mouse_down[5];
extern int  inp_mouse_x,  inp_mouse_y;
extern int  inp_mouse_dx, inp_mouse_dy;
extern int  inp_wheel_x,  inp_wheel_y;
extern int  inp_wheel_dx, inp_wheel_dy;
extern char inp_text[32];

/* Shortcut to check is key up/down */
#define inp_kb_down(key) (inp_keyboard_down[SDL_SCANCODE_##key])
#define inp_kb_up(key)   (!inp_kb_down(key))
#define inp_ms_down(key) (inp_mouse_down[SDL_BUTTON_##key])
#define inp_ms_up(key)   (!in_ms_down(key))

/* Old position of mouse, wheel state */
#define inp_mouse_old_x (inp_mouse_x - inp_mouse_dx)
#define inp_mouse_old_y (inp_mouse_y - inp_mouse_dy)
#define inp_wheel_old_x (inp_mouse_x - inp_mouse_dx)
#define inp_wheel_old_y (inp_mouse_y - inp_mouse_dy)

/* Find first pressed keyboard/mouse key */
int inp_first_pressed(void);

