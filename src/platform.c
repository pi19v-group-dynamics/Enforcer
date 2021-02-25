#include "platform.h"
#include <SDL2/SDL.h>
#include <string.h>

#ifndef P_TARGET_FPS
#define P_TARGET_FPS 60.0
#endif /* P_TARGET_FPS */

#ifndef P_INIT_SCALE
#define P_INIT_SCALE 4
#endif /* P_INIT_SCALE */

/******************************************************************************
 * Internals
 *****************************************************************************/

static uint32_t window_id;
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static SDL_AudioDeviceID device = 0;

/******************************************************************************
 * Setup / clean
 *****************************************************************************/

atomic_int P_SCALE = P_INIT_SCALE;

void p_setup(void)
{
	/* Init SDL */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fatal("SDL initalization error! SDL_Error: %s", SDL_GetError());
	}
	/* Init window */
	if (!(window = SDL_CreateWindow(
		"Tetrix",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		P_WIDTH * P_SCALE,
		P_HEIGHT* P_SCALE,
		SDL_WINDOW_SHOWN)))
	{
		fatal("window creation error! SDL error: %s", SDL_GetError());
	}
	window_id = SDL_GetWindowID(window);
	/* Init renderer */
	if (!(renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)))
	{
		fatal("renderer creation error! SDL error: %s", SDL_GetError());
	}
	/* Init texture */
	if (!(texture = SDL_CreateTexture(
		renderer,
		SDL_GetWindowPixelFormat(window),
		SDL_TEXTUREACCESS_STREAMING,
		P_WIDTH,
		P_HEIGHT)))
	{
		fatal("texture creation error! SDL error: %s", SDL_GetError());
	}
}

void p_close(void)
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	if (device != 0)
	{
		SDL_CloseAudioDevice(device);
	}
	SDL_Quit();
}

/******************************************************************************
 * Time control
 *****************************************************************************/

#define TIME_STEP  (1.0 / P_TARGET_FPS)
#define INV_MILLIS (1.0 / 1000.0)

double p_step(void)
{
	static double last = 0.0, now = 0.0, wait = 0.0, delta = 0.0;
	now = (double)SDL_GetTicks() * INV_MILLIS;
	delta = now - last;
	wait = TIME_STEP - delta;
	last += TIME_STEP;
	if (wait > 0)
	{
		SDL_Delay(wait * 1000.0);
	}
	else
	{
		last = now;
	}
	return delta;
}

/******************************************************************************
 * Graphics
 *****************************************************************************/

static p_pixel framebuffers[2][P_WIDTH * P_HEIGHT]; /* front and back buffers */ 
p_pixel* p_pixels = framebuffers[0];

void p_flip(void)
{
	static int buf_idx = 0;
	/* swap buffers */
	p_pixel* old = p_pixels;
	buf_idx = (buf_idx + 1) & 1;
	p_pixels = framebuffers[buf_idx];
	/* render front buffer */
	SDL_UpdateTexture(texture, NULL, old, sizeof(p_pixel[P_WIDTH]));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/******************************************************************************
 * Input
 *****************************************************************************/

bool p_key_down[SDL_NUM_SCANCODES];
bool p_mouse_down[5];
int p_mouse_x = 0, p_mouse_y = 0;
int p_mouse_dx = 0, p_mouse_dy = 0;
int p_wheel_x = 0, p_wheel_y = 0;
int p_wheel_dx = 0, p_wheel_dy = 0;
char p_text[32] = {0};

int p_process(void)
{
	static SDL_Event e;
	int state = P_SHOWN;
	p_mouse_dx = p_mouse_x;
	p_mouse_dy = p_mouse_y;
	p_wheel_dx = p_wheel_x;
	p_wheel_dy = p_wheel_y;
	p_text[0] = '\0';
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
				return P_CLOSED;
			case SDL_KEYDOWN:
				p_key_down[e.key.keysym.scancode] = true;
				break;
			case SDL_KEYUP:
				p_key_down[e.key.keysym.scancode] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				p_mouse_down[e.button.button] = true;
				break;
			case SDL_MOUSEBUTTONUP:
				p_mouse_down[e.button.button] = false;
				break;
			case SDL_TEXTINPUT:
				memcpy(p_text, e.text.text, 32); 
				break;
			case SDL_TEXTEDITING:
				memcpy(p_text, e.edit.text, 32); 
				break;
			case SDL_MOUSEMOTION:
				p_mouse_x = e.motion.x / P_SCALE;
				p_mouse_y = e.motion.y / P_SCALE;
				break;
			case SDL_MOUSEWHEEL:
				p_wheel_x = e.wheel.x;
				p_wheel_y = e.wheel.y;
				break;
			default:
				break;
		}
	}
	p_mouse_dx = p_mouse_x - p_mouse_dx;
	p_mouse_dy = p_mouse_y - p_mouse_dy;
	p_wheel_dx = p_wheel_x - p_wheel_dx;
	p_wheel_dy = p_wheel_y - p_wheel_dy;
	return state;
}

/******************************************************************************
 * Audio
 *****************************************************************************/

inline void p_pause(int state)
{
	SDL_PauseAudioDevice(device, state);
}

int p_sound(void (*callback)(void*, uint8_t*, int))
{
	if (device != 0)
	{
		SDL_CloseAudioDevice(device);
	}
	SDL_AudioSpec fmt, got;
	fmt.freq = 48000; /* 48KHz */
	fmt.format = AUDIO_S16;
	fmt.channels = 2;
	fmt.samples = 1024;
	fmt.callback = callback;
	device = SDL_OpenAudioDevice(NULL, 0, &fmt, &got, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if (device == 0)
	{
		fatal("failed to open audio device! SDL error: %s", SDL_GetError());
	}
	return got.freq;
}

