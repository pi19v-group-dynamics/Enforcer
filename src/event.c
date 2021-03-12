#include "event.h"
#include "memalloc.h"
#include "utils.h"
#include <stdatomic.h>

typedef struct event_listener event_listener_t;

struct event_listener
{
	void* udata;
	event_handler_t handler;
	event_listener_t* next;
};

struct event_bus
{
	atomic_flag lock_flag;
	unsigned num_events;
	event_listener_t* listeners[];
};

/******************************************************************************
 * Event bus create/destroy
 *****************************************************************************/

event_bus_t* event_bus_create(unsigned num_events)
{
	event_bus_t* bus = calloc(1, sizeof(event_bus_t) + sizeof(event_listener_t*[num_events]));
	spinlock_unlock(&bus->lock_flag);
	bus->num_events = num_events;
	return bus;
}

void event_bus_destroy(event_bus_t* bus)
{
	for (unsigned i = 0; i < bus->num_events; ++i)
	{
		event_unsubscribe(bus, i);
	}
	free(bus);
}

/******************************************************************************
 * Event handler subsciption
 *****************************************************************************/

void event_subscribe(event_bus_t* bus, unsigned evt, event_handler_t hdl, void* udata)
{
	assert(evt < bus->num_events, "Wrong event identifier!");
	event_listener_t* lst = malloc(sizeof(event_listener_t));
	lst->udata = udata;
	lst->handler = hdl;
	spinlock_lock(&bus->lock_flag);
	lst->next = bus->listeners[evt];
	bus->listeners[evt] = lst;
	spinlock_unlock(&bus->lock_flag);
}

void event_unsubscribe(event_bus_t* bus, unsigned evt)
{
	assert(evt < bus->num_events, "Wrong event identifier!");
	spinlock_lock(&bus->lock_flag);
	for (event_listener_t* lst = bus->listeners[evt]; lst != NULL; )
	{
		event_listener_t* next = lst->next;
		free(lst);	
		lst = next;
	}
	bus->listeners[evt] = NULL;
	spinlock_unlock(&bus->lock_flag);
}

void event_notify(event_bus_t* bus, unsigned evt, void* args)
{
	assert(evt < bus->num_events, "Wrong event identifier!");
	spinlock_lock(&bus->lock_flag);
	for (event_listener_t* lst = bus->listeners[evt]; lst != NULL; lst = lst->next)
	{
		lst->handler(args, lst->udata);
	}
	spinlock_unlock(&bus->lock_flag);
}

