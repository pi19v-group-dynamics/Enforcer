#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef P_WIDTH
#define P_WIDTH 136
#endif /* P_WIDTH */

#ifndef P_HEIGHT
#define P_HEIGHT 128
#endif /* P_HEIGHT */

typedef uint32_t p_pixel;

/******************************************************************************
 * Setup / close platform layer
 *****************************************************************************/

extern atomic_int P_SCALE; /* virtual screen scale */

/* Initialize platform layer */
void p_setup(void);
/* Close platform layer (free resources) */
void p_close(void);

/******************************************************************************
 * Time control
 *****************************************************************************/

/* Caps framerate and returns time between two calls */
double p_step(void);

/******************************************************************************
 * Graphics
 *****************************************************************************/

extern p_pixel* p_pixels; /* actual framebuffer */

/* Flip virtual framebuffer and display it */
void p_flip(void);

/******************************************************************************
 * Input
 *****************************************************************************/

/* Input caches */
extern bool p_key_down[];
extern bool p_mouse_down[];
extern int p_mouse_x, p_mouse_y;
extern int p_mouse_dx, p_mouse_dy;
extern int p_wheel_x, p_wheel_y;
extern int p_wheel_dx, p_wheel_dy;
extern char p_text[];

/* Shortcuts (only for SDL) */
#include <SDL2/SDL_Scancode.h>
#define p_keyup(key) (!p_keydown(key))
#define p_keydown(key) (p_key_down[SDL_SCANCODE_##key])
#define p_mouseup(key) (!p_mousedown(key))
#define p_mousepress(key) (p_mouse_press[SDL_BUTTON_##key])

/* App states */
enum {P_CLOSED, P_HIDDEN, P_SHOWN};

/* Process system events, return state of app */
int p_process(void);

/******************************************************************************
 * Audio
 *****************************************************************************/

/* Turn on/off audio device */
void p_pause(int state);
/* Init device and set audio callback, returns actual frequency */
int p_sound(void (*callback)(void*, uint8_t*, int));

#endif /* PLATFORM_H */

