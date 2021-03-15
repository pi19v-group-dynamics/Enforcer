#ifndef EVENT_H
#define EVENT_H

typedef struct event_bus event_bus_t;
typedef void (*event_handler_t)(void* args, void* udata);

/******************************************************************************
 * Event bus create/destroy
 *****************************************************************************/

event_bus_t* event_make_bus(unsigned num_events);
void event_free_bus(event_bus_t* bus);

/******************************************************************************
 * Event handler subsciption
 *****************************************************************************/

void event_subscribe(event_bus_t* bus, unsigned evt, event_handler_t hdl, void* udata);
void event_unsubscribe(event_bus_t* bus, unsigned evt);
void event_notify(event_bus_t* bus, unsigned evt, void* arg);

#endif /* EVENT_H */

