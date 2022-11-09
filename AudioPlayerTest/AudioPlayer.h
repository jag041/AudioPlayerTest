#pragma once
// std includes
#include <string>
#include <vector>
#include <functional>

// portaudio
#include <portaudio.h>


// my includes
#include "GlobalConsts.h"
#include "SampleQueue.h"

#define MAX_NAMES 8


class AudioPlayer
{
public:
	typedef struct
	{
		PCMSample* sample;
		SampleQueue* buff_queue;
		uint32_t     maxFrameIndex[MAX_ENQUEUED_SAMPLES];
		size_t       frameIndex;  /* Index into sample array. */
	}
	paBufferData;
		

	AudioPlayer(SampleQueue* bQueue);

	~AudioPlayer();

	void query_devices();

	void open_stream();

	void start_streaming();

	void stop_streaming();

	void close_stream();

	bool is_playing() const;



private:
	PaStream* stream;
	SampleQueue* sample_queue;
	uint16_t writesPerTick;
	paBufferData data;



#ifndef N_DEBUG
	PaError err;
#endif





public:

	//int BufferReadCallback(const void* inputBuffer, void* outputBuffer,
	//	unsigned long framesPerBuffer,
	//	const PaStreamCallbackTimeInfo* timeInfo,
	//	PaStreamCallbackFlags statusFlags);

	static int paBufferReadCallback(const void* inputBuffer, void* outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void* userData)
	{
		paBufferData* data = (paBufferData*)userData;
		// printf("callback called");
		if (data->frameIndex == 0)
		{
			printf("frameIndex is zero at the top of the buffer callback\n");
			data->sample = data->buff_queue->get_next_sample();

		}
		uint16_t* sample_ptr = data->sample->sample;
		uint16_t* rptr = sample_ptr + data->frameIndex;
		uint16_t* wptr = (uint16_t*)outputBuffer;
		unsigned int i;
		int finished;
		uint32_t framesLeft = data->sample->maxFrameIndex - data->frameIndex;

		(void)inputBuffer; /* Prevent unused variable warnings. */
		(void)timeInfo;
		(void)statusFlags;
		// printf("writing buffers in callback\n");
		if (framesLeft < framesPerBuffer)
		{
			/* final buffer... */
			for (i = 0; i < framesLeft; ++i)
			{
				*wptr++ = *rptr++;  /* left */
				*wptr++ = *rptr;  /* right */
			}

			data->sample = data->buff_queue->get_next_sample();
			uint16_t* sample_ptr = data->sample->sample;
			data->frameIndex = 0;
			if (!sample_ptr)
			{
				for (; i < framesPerBuffer; ++i) {
					*wptr++ = 0;
					*wptr++ = 0;
				}
				return paContinue;
			}
			size_t j = 0;
			for (; j + i < framesPerBuffer; ++j)
			{
				*wptr++ = *rptr++;  /* left */
				*wptr++ = *rptr;  /* right */
			}
			data->frameIndex += j;
			return paContinue;
		}
		for (i = 0; i < framesPerBuffer; ++i)
		{

			// think about interleaving for spatialization

			*wptr++ = *rptr++;  /* left */
			*wptr++ = *rptr;  /* right */
		}
		data->frameIndex += framesPerBuffer;
		finished = paContinue;
		return finished;
	}
};

