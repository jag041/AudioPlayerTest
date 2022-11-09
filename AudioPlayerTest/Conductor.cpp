#include "Conductor.h"

Conductor::Conductor(uint16_t new_tempo) : cur_tempo(new_tempo)
{
	tStart = std::chrono::high_resolution_clock::time_point();
	tEnd = std::chrono::high_resolution_clock::time_point();

	set_current_beat(4);
}

Conductor::~Conductor()
{
}

void Conductor::start() {
	tStart = std::chrono::high_resolution_clock::now();
	tEnd = std::chrono::high_resolution_clock::now();
}

Conductor::Tempo Conductor::get_tempo() const
{
	return cur_tempo;
}

void Conductor::set_current_beat(uint16_t beat)
{
	cur_beat = beat_to_milliseconds((float)beat / 4);
}



uint16_t Conductor::get_time_delta()
{
	
	tEnd = std::chrono::high_resolution_clock::now();

	auto tElapsed = std::chrono::duration_cast<std::chrono::duration<uint16_t, std::milli>>(tEnd - tStart);

	uint16_t ret = tElapsed.count();

	return ret;
}

void Conductor::update_time_delta(){
	
	tStart = tEnd;
}

uint16_t Conductor::tempo_to_milliseconds() const {
	return 60000 / cur_tempo;
}

uint16_t Conductor::beat_to_milliseconds(float beat) const
{
	
	return beat * tempo_to_milliseconds();
}



bool Conductor::time_out() 
{
	if (tStart == std::chrono::high_resolution_clock::time_point())
	{
		//printf("timeout called before playing started\n");
		return false;
	}
	uint16_t delta = get_time_delta();

	if (delta  >= cur_beat)
	{
		printf("\n**********beat*********\ncur_beat %d\n\n", cur_beat);
		update_time_delta();
		return true;
	}
	return false;

}

