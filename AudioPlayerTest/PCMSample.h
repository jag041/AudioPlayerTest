#pragma once

// std includes
#include <string>

// individual wav parsed samples are stored as SampleData, PCMSamples

// samples which are written into the bufferQueue at the end of the write chain -
// in order to be subsequently read into the callback function

class PCMSample {
public:

	PCMSample();

	PCMSample(size_t num_frames, float norm);

	~PCMSample();

	// consider moving this state somewhere else
	// move iterator elsewhere

	uint32_t     maxFrameIndex;
	uint16_t*	 sample;
};

