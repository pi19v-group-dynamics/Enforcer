#include "input.h"

bool inp_keyboard_down[SDL_NUM_SCANCODES];
bool inp_mouse_down[5];
int  inp_mouse_x  = 0, inp_mouse_y  = 0;
int  inp_mouse_dx = 0, inp_mouse_dy = 0;
int  inp_wheel_x  = 0,  inp_wheel_y = 0;
int  inp_wheel_dx = 0, inp_wheel_dy = 0;
char inp_text[32];

inline int inp_first_pressed(void)
{
	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		if (inp_keyboard_down[i] == true)
		{
			return i;
		}
	}
	return inp_mouse_down[0] || inp_mouse_down[1]
	    || inp_mouse_down[2] || inp_mouse_down[3]
			|| inp_mouse_down[4];
}


