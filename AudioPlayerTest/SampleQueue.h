#pragma once

// std includes
#include <string>
#include <vector>

// portaudio
#include <portaudio.h>

// my includes
#include "GlobalConsts.h"
#include "ErrorHandler.h"
#include "AlignedAllocator.h"
#include "SampleStackAllocator.h"
#include "PCMSample.h"
#include "Composer.h"
#include "WAVParser.h"


enum InstrumentHandle {
	GUITAR_SAMPLES,
	DRUM_SAMPLES,
	BASS_SAMPLES
};

class SampleQueue
{
public:
	SampleQueue(AlignedAllocator* new_allocator, WavDirInputFormat format);
	~SampleQueue();

	PCMSample* get_next_sample();
	
	uint16_t write_samples(Beat cur_beat); // returns length of beat for some reason

private:

	SampleStackAllocator<uint16_t> double_stack[2];

	PCMSample read_data;

	AlignedAllocator* allocator;
	std::vector<std::vector<SampleData>> handles;

	size_t cur_stack;

	uint32_t cur_length;

	void swap_buffers();
	void write_sample(InstrumentHandle inst_handle, uint16_t handle, float scale);


};

