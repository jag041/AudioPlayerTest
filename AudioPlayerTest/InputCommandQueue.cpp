#include "InputCommandQueue.h"

InputCommandQueue::InputCommandQueue(AlignedAllocator* new_allocator) : command_pool(new_allocator)
{
	command_pool->init(sizeof(InputCommand) * MAX_ENQUEUED_SAMPLES, 16, MAX_ENQUEUED_SAMPLES);
}

void InputCommandQueue::add_command(InputCommand* cmd)
{

	if (num_commands == MAX_ENQUEUED_SAMPLES) {
		throw ErrorHandler("ERROR: Too many input commands!");
	}

	commands[num_commands++] = (InputCommand*)command_pool->set_data(cmd, sizeof(InputCommand));

}

void InputCommandQueue::execute_commands()
{
	if (num_commands <= 0)
		return;

	for (size_t i = 0; i < num_commands; ++i) {
		InputCommand* cmd_ptr = commands[i];

		cmd_ptr->execute();
	}

	command_pool->clear_data();

	num_commands = 0;

}

void InputCommandQueue::clear_commands()
{
	command_pool->clear_data();

	num_commands = 0;
}
