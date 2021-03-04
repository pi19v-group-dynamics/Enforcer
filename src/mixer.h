#ifndef MIXER_H
#define MIXER_H

#include <stdbool.h>

typedef enum mix_state
{
	MIX_PLAYING,
	MIX_STOPPED,
	MIX_PAUSED,
	MIX_ERASING
}
mix_state_t;

typedef struct mix_source mix_source_t;

typedef struct mix_source
{
	mix_state_t state;  /* source state (playing, stopped, paused) */
	bool loop;          /* loop flag */
	float* samples;     /* raw PCM 32-bit float audio samples */
	double length;      /* length of source */
	double position;    /* position in current source */
	float left_gain;    /* left gain */
	float right_gain;   /* right gain */
	double pitch;       /* pitch */
	float gain;         /* total gain */
	float pan;          /* total pan */
	mix_source_t* next; /* next source */
}
mix_source_t;

/******************************************************************************
 * Mixer lock
 *****************************************************************************/

void mix_lock(void);
void mix_unlock(void);

/******************************************************************************
 * Reset, process
 *****************************************************************************/

void mix_reset(void);
void mix_process(float* buffer, int len);
void mix_master_gain(float gain);

/******************************************************************************
 * Source make, load, free 
 *****************************************************************************/

mix_source_t* mix_make_source(void* data, unsigned size);
mix_source_t* mix_load_source(const char filename[static 2]);
void mix_free_source(mix_source_t* src);

/******************************************************************************
 * Source effects 
 *****************************************************************************/

void mix_pan(mix_source_t* src, float pan);
void mix_gain(mix_source_t* src, float gain);
void mix_pitch(mix_source_t* src, double pitch);
void mix_loop(mix_source_t* src, bool loop);

/******************************************************************************
 * Play, stop, pause 
 *****************************************************************************/

void mix_play(mix_source_t* src);
void mix_stop(mix_source_t* src);
void mix_pause(mix_source_t* src);

#endif /* MIXER_H */

