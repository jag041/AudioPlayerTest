#pragma once

// my includes
#include "AlignedAllocator.h"
#include "InputCommand.h"

class InputCommandQueue
{
public:
	InputCommandQueue(AlignedAllocator* new_allocator);

	void add_command(InputCommand* cmd);

	void execute_commands();

	void clear_commands();

private:

	AlignedAllocator* command_pool;

	InputCommand* commands[MAX_ENQUEUED_SAMPLES];

	size_t num_commands{ 0 };

};

