#include "../../../src/event.h"
#include "../../../src/input.h"
#include "../../../src/system.h"
#include <stddef.h>

enum
{
	MOUSE_CLICK = 0,
	LCTRL_CLICK,
	KP_0,
	EVENT_COUNT
};

void mouse_click_handler(void* args, void* udata)
{
	(void) udata;
	log_trace("Handler 1 mouse clicked at x: %i, y: %i", *(int*)args, *((int*)args + 1));
}

void ctrl_click_handler(void* args, void* udata)
{
	(void) args;
	(void) udata;

	log_trace("Handled Left Ctrl clicked");
}

void numpad_click_handler(void* args, void* udata)
{
	(void) args;
	(void) udata;

	log_trace("Handled Numpad0 clicked");
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	event_bus_t* bus = event_make_bus(EVENT_COUNT);
	event_subscribe(bus, MOUSE_CLICK, mouse_click_handler, NULL);
	event_subscribe(bus, LCTRL_CLICK, ctrl_click_handler, NULL);
	event_subscribe(bus, KP_0, numpad_click_handler, NULL);

	double dt;
	int state;

	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		if (inp_ms_down(RIGHT))
		{
			event_notify(bus, MOUSE_CLICK, (int[2]){ inp_mouse_x, inp_mouse_y });
		}
		if (inp_kb_down(LCTRL))
		{
			event_notify(bus, LCTRL_CLICK, NULL);
		}
		if (inp_kb_down(KP_0))
		{
			event_notify(bus, KP_0, NULL);
		}
		if (inp_kb_down(ESCAPE))
		{
			break;
		}
	}

	event_free_bus(bus);

	return 0;
}

