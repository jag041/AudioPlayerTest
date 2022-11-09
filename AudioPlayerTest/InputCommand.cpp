#include "InputCommand.h"



InputCommand::InputCommand(Composer* out, uint8_t in) : composer(out), data(in)
{
}

void InputCommand::execute()
{
	composer->enqueue_input(data);
}

void InputCommand::reverse_execute() {


}
