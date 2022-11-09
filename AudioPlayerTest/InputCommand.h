#pragma once

// my includes
#include "AudioPlayer.h"
#include "Composer.h"

class InputCommand
{
public:
	InputCommand(Composer* out, uint8_t in);

	void execute();

	void reverse_execute();

private:
	uint8_t data;
	Composer* composer;

};

