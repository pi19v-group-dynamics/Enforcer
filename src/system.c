#include "system.h"
#include <string.h>
#include "renderer.h"
#include "mixer.h"
#include "input.h"
#include "utils.h"

#ifndef SYS_INIT_WINDOW_SCALE
#define SYS_INIT_WINDOW_SCALE 2
#endif /* SYS_INIT_WINDOWS_SCALE */

#ifndef SYS_INIT_WINDOW_TITLE
#define SYS_INIT_WINDOW_TITLE NULL
#endif /* SYS_INIT_WINDOWS_TITLE */

/* Internal usage variables */
SDL_Window* _sys_window = NULL;
SDL_Renderer* _sys_renderer = NULL;
SDL_Texture* _sys_texture = NULL;
SDL_AudioDeviceID _sys_device = 0;
SDL_AudioSpec _sys_audio_spec;

/* Audio callback (mixes sound sources) */
static void audio_callback(void* udata, Uint8* stream, int len)
{
	(void) udata;
	mix_process((float*)stream, len / sizeof(float));
}

/* Constructor */
__attribute__((constructor)) static void sys_setup(void)
{
	/* Init SDL */
	confirm(SDL_Init(SDL_INIT_EVERYTHING) == 0, "SDL initalization error! SDL_Error: %s", SDL_GetError());	
	/* Init window */
	confirm(_sys_window = SDL_CreateWindow(SYS_INIT_WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ren_screen->width, ren_screen->height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
		"Window creation error! SDL error: %s",
		SDL_GetError());
	/* Init renderer */
	confirm(_sys_renderer = SDL_CreateRenderer(_sys_window, -1, SDL_RENDERER_ACCELERATED),
		"Renderer creation error! SDL error: %s",
		SDL_GetError());
	SDL_RenderSetLogicalSize(_sys_renderer, REN_WIDTH, REN_HEIGHT);
	SDL_RenderSetIntegerScale(_sys_renderer, 1);
	/* Init texture */
	confirm(_sys_texture = SDL_CreateTexture(_sys_renderer, SYS_PIXEL_FMT, SDL_TEXTUREACCESS_STREAMING, ren_screen->width, ren_screen->height),
		"Texture creation error! SDL error: %s",
		SDL_GetError());
	/* Open audio device */
	SDL_AudioSpec fmt;
	fmt.freq = 44100; /* 48KHz */
	fmt.format = SYS_AUDIO_FMT;
	fmt.channels = 2;
	fmt.samples = 1024;
	fmt.callback = audio_callback;
	fmt.userdata = NULL;
	confirm(_sys_device = SDL_OpenAudioDevice(NULL, 0, &fmt, &_sys_audio_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE), "failed on open audio device! SDL error: %s", SDL_GetError());
	if (_sys_audio_spec.freq != fmt.freq)
	{
		log_debug("Audio device samplerate changed to %i Hz", _sys_audio_spec.freq);
	}
	SDL_PauseAudioDevice(_sys_device, 0);
	log_debug("Application started...");
}

/* Destructor */
__attribute__((destructor)) static void sys_close(void)
{
	/* Cleanup SDL objects */
	SDL_DestroyWindow(_sys_window);
	SDL_DestroyRenderer(_sys_renderer);
	SDL_DestroyTexture(_sys_texture);
	SDL_CloseAudioDevice(_sys_device);
	SDL_Quit();
	log_debug("Application closed!");
}

__attribute__((always_inline)) inline void sys_title(const char* title)
{
	SDL_SetWindowTitle(_sys_window, title);
}

__attribute__((always_inline)) inline void sys_fullscreen(bool enabled)
{
	SDL_SetWindowFullscreen(_sys_window, enabled ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

static int process_events(void)
{
	static SDL_Event e;
	static int state = SYS_OPENED; /* application state */
	inp_mouse_dx = inp_mouse_x;
	inp_mouse_dy = inp_mouse_y;
	inp_wheel_dx = inp_wheel_x;
	inp_wheel_dy = inp_wheel_y;
	inp_text[0] = '\0';
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
			case SDL_APP_TERMINATING:
				state = SYS_CLOSED;
				break;
			case SDL_APP_WILLENTERBACKGROUND:
			case SDL_APP_DIDENTERBACKGROUND:
				state = SYS_PAUSED;
				break;
			case SDL_APP_WILLENTERFOREGROUND:
			case SDL_APP_DIDENTERFOREGROUND:
				state = SYS_OPENED;
				break;
			case SDL_KEYDOWN:
				inp_keyboard_down[e.key.keysym.scancode] = true;
				break;
			case SDL_KEYUP:
				inp_keyboard_down[e.key.keysym.scancode] = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				inp_mouse_down[e.button.button] = true;
				break;
			case SDL_MOUSEBUTTONUP:
				inp_mouse_down[e.button.button] = false;
				break;
			case SDL_TEXTINPUT:
				memcpy(inp_text, e.text.text, 32); 
				break;
			case SDL_TEXTEDITING:
				memcpy(inp_text, e.edit.text, 32); 
				break;
			case SDL_MOUSEMOTION:
				inp_mouse_x = e.motion.x;
				inp_mouse_y = e.motion.y;
				break;
			case SDL_MOUSEWHEEL:
				inp_wheel_x = e.wheel.x;
				inp_wheel_y = e.wheel.y;
				break;
			default:
				break;
		}
	}
	/* Limit mouse position in screen */
	inp_mouse_x = inp_mouse_x < 0
	            ? 0 
	            : inp_mouse_x >= REN_WIDTH
	            ? REN_WIDTH - 1
	            : inp_mouse_x;
	inp_mouse_y = inp_mouse_y < 0
	            ? 0 
	            : inp_mouse_y >= REN_HEIGHT
	            ? REN_HEIGHT - 1
	            : inp_mouse_y;
	inp_mouse_dx = inp_mouse_x - inp_mouse_dx;
	inp_mouse_dy = inp_mouse_y - inp_mouse_dy;
	inp_wheel_dx = inp_wheel_x - inp_wheel_dx;
	inp_wheel_dy = inp_wheel_y - inp_wheel_dy;
	return state;
}

#define INV_MILLIS (1.0 / 1000.0)

int sys_step(double time_step, double* dt)
{
	static double last = 0.0, prev = 0.0;
	double now = (double)SDL_GetTicks() * INV_MILLIS;
	double wait = time_step - (now - last);
	last += time_step;
	if (wait > 0)
	{
		SDL_Delay(wait * 1000.0);
	}
	else
	{
		last = now;
	}
	*dt = now - prev;
	prev = now;
	return process_events();
}

void sys_display(const void* pixels, int pitch)
{
	SDL_UpdateTexture(_sys_texture, NULL, pixels, pitch);
	SDL_RenderClear(_sys_renderer);
	SDL_RenderCopy(_sys_renderer, _sys_texture, NULL, NULL);
	SDL_RenderPresent(_sys_renderer);
}

__attribute__((always_inline)) inline void sys_mute(bool state)
{
	SDL_PauseAudioDevice(_sys_device, state);
}

__attribute__((always_inline)) inline void sys_vsync(bool enabled)
{
	SDL_GL_SetSwapInterval(enabled);
}

