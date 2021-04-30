#pragma once

#include <stdbool.h>

/* Pixel format of screen surface */
#define SYS_PIXEL_FMT SDL_PIXELFORMAT_ARGB8888

/* Pixel format of audio sources */
#define SYS_AUDIO_FMT AUDIO_F32

enum
{
	SYS_CLOSED, /* application closed */
	SYS_PAUSED, /* application lost focus/hided */
	SYS_OPENED  /* application has focus and active */
};

void sys_title(const char* title); /* set window title */
void sys_fullscreen(bool enabled); /* set window fullscreen */
int sys_step(double time_step, double* dt); /* do time step */
void sys_display(const void* pixels, int pitch); /* update screen pixels */
void sys_vsync(bool enabled); /* set vsync enabled/disabled */
void sys_mute(bool mute); /* mute/unmute audio device */
void sys_exit(void); /* close app */

