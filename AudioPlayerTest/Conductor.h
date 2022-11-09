#pragma once
#include <vector>
#include <chrono>



// a class for managing sample timings
class Conductor
{
public:

	typedef uint16_t Tempo;

	Conductor(uint16_t new_tempo);
	~Conductor();

	void start();

	Tempo get_tempo() const;

	void set_current_beat(uint16_t beat);

	bool time_out();

private:

	Tempo cur_tempo;
	uint32_t cur_beat;
	std::chrono::high_resolution_clock::time_point tStart;
	std::chrono::high_resolution_clock::time_point tEnd;

	uint16_t tempo_to_milliseconds() const;
	uint16_t beat_to_milliseconds(float beat) const;
	uint16_t get_time_delta();
	void update_time_delta();
};

