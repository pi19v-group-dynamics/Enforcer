#include "mixer.h"
#include "memalloc.h"
#include <string.h>
#include <SDL2/SDL.h>
#include <tgmath.h>
#include <stdatomic.h>
#include "utils.h"

#ifndef MIX_BUFFER_SIZE
#define MIX_BUFFER_SIZE 512
#endif /* MIX_BUFFER_SIZE */

static float buffer[MIX_BUFFER_SIZE];
static mix_source_t* playing_sources = NULL;
static float master_gain = 1.0;
static atomic_flag lock = ATOMIC_FLAG_INIT;

/******************************************************************************
 * Reset, process
 *****************************************************************************/

void mix_reset(void)
{
	master_gain = 1.0f;
	while (playing_sources != NULL)
	{
		mix_source_t* that = playing_sources;
		playing_sources = playing_sources->next;
		mix_free_source(that);
	}
}

void mix_process(float* dst, int len)
{
	/* Process all subchunks */
	while (len > MIX_BUFFER_SIZE)
	{
		mix_process(dst, MIX_BUFFER_SIZE);
		dst += MIX_BUFFER_SIZE;
		len -= MIX_BUFFER_SIZE;
	}
	/* Fill buffer */
	mix_lock();
	memset(buffer, 0, sizeof(float[len]));
	for (mix_source_t** src = &playing_sources; *src != NULL; )
	{
		if ((*src)->state == MIX_PLAYING)
		{
			for (int idx = 0; idx < len; idx += 2)
			{
				int pos = (*src)->position;
				buffer[idx + 0] += (*src)->samples[pos] * (*src)->left_gain * master_gain;
				buffer[idx + 1] += (*src)->samples[pos] * (*src)->right_gain * master_gain;
				(*src)->position += (*src)->pitch;
				if ((*src)->position >= (*src)->length)
				{
					if ((*src)->loop)
					{
						(*src)->position = fmod((*src)->position, (*src)->length);
					}
					else
					{
						mix_stop(*src);
					}
				}	
			}
			src = &(*src)->next;
		}
		else
		{
			*src = (*src)->next;
		}
	}
	mix_unlock();
	/* Copy internal buffer to destination buffer */
	memcpy(dst, buffer, sizeof(float[len]));
}

void mix_master_gain(float gain)
{
	master_gain = gain;
}

/******************************************************************************
 * Mixer lock
 *****************************************************************************/

void mix_lock(void)
{
	while (atomic_flag_test_and_set(&lock));
}

void mix_unlock(void)
{
	atomic_flag_clear(&lock);
}

/******************************************************************************
 * Source make, free
 *****************************************************************************/

mix_source_t* mix_make_source(void* data, unsigned size)
{
	mix_source_t* src = malloc(sizeof(mix_source_t) + size);
	src->next = NULL;
	/* copy sample data */
	src->samples = (float*)((char*)src + sizeof(mix_source_t));
	memcpy(src->samples, data, size);
	src->length = size / sizeof(float);
	mix_gain(src, 1.0f);
	mix_pan(src, 0.0f);
	mix_pitch(src, 1.0);
	mix_loop(src, false);
	mix_stop(src);
	return src;
}

mix_source_t* mix_load_source(const char filename[static 2])
{
	Uint8* data;
	Uint32 size;
	mix_source_t* src;
	extern SDL_AudioSpec _sys_audio_spec;
	/* Load audio */
	SDL_AudioSpec* spec;
	confirm(spec = SDL_LoadWAV(filename, &_sys_audio_spec, &data, &size), "Failed on load audio file '%s'. SDL error: %s", filename, SDL_GetError());
	/* Convert audio and create source */
	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, spec->format, spec->channels, spec->freq, _sys_audio_spec.format, _sys_audio_spec.channels, _sys_audio_spec.freq);
	if (cvt.needed) /* should convert to? */
	{
		cvt.len = spec->samples * spec->channels * (SDL_AUDIO_BITSIZE(spec->format) >> 3);
		cvt.buf = (Uint8*)malloc(cvt.len * cvt.len_mult);
		SDL_ConvertAudio(&cvt);
		src = mix_make_source(cvt.buf, cvt.len_cvt);
		free(cvt.buf);
	}
	else
	{
		confirm(spec->format   == _sys_audio_spec.format
		     && spec->channels == _sys_audio_spec.channels
				 && spec->freq     == _sys_audio_spec.freq,
				 "Failed on load audio file '%s'. Unsupported audio format!", filename);
		src = mix_make_source(data, size);
	}
	/* Free loaded audio */
	SDL_FreeWAV(data);
	return src;
}

void mix_free_source(mix_source_t* src)
{
	mix_lock();
	if (src->state == MIX_PLAYING)
	{
		for (mix_source_t** it = &playing_sources; *it != NULL; it = &(*it)->next)
		{
			if (*it == src)
			{
				*it = src->next;
				break;
			}
		}
	}
	mix_unlock();
	free(src);
}

/******************************************************************************
 * Source effects 
 *****************************************************************************/

#define recalc_gains(src) \
	src->left_gain  = src->gain * (src->pan <= 0.0f ? 1.0f : 1.0f - src->pan); \
	src->right_gain = src->gain * (src->pan >= 0.0f ? 1.0f : 1.0f - src->pan)

inline void mix_pan(mix_source_t* src, float pan)
{
	src->pan = pan;
	recalc_gains(src);
}

inline void mix_gain(mix_source_t* src, float gain)
{
	src->gain = gain;
	recalc_gains(src);
}

inline void mix_pitch(mix_source_t* src, double pitch)
{
	src->pitch = pitch;
}

inline void mix_loop(mix_source_t* src, bool loop)
{
	src->loop = loop;
}

#undef recalc_gains

/******************************************************************************
 * Play, stop, pause 
 *****************************************************************************/

void mix_play(mix_source_t* src)
{
	mix_lock();
	if (src->state != MIX_PLAYING)
	{
		src->state = MIX_PLAYING;
		src->next = playing_sources;
		playing_sources = src;
	}
	mix_unlock();
}

void mix_stop(mix_source_t* src)
{
	src->state = MIX_STOPPED;
	src->position = 0.0;
}

void mix_pause(mix_source_t* src)
{
	src->state = MIX_PAUSED;
}

