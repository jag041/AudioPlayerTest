#pragma once
#include <vector>

// my includes


struct Beat {
	std::vector<uint16_t> sample_handles{};
	std::vector<float> scale{};
	uint16_t length{};
};

class Composer
{
public:

	Composer();
	~Composer();

	Beat compose_beat();

	void enqueue_input(uint8_t data);

private:
	
	uint16_t beat_scale[8] = {4, 5, 6, 5, 6, 5, 5, 4};
	std::vector<uint8_t> input_queue;
	uint8_t seed{};
	size_t top_input;
	size_t bot_input;
	uint8_t concat;

};

