#include "Composer.h"
#include "GlobalConsts.h"
#include <random>


Composer::Composer()
{
	concat = 0;
	input_queue.resize(MAX_ENQUEUED_SAMPLES * 2);
	input_queue[0] = '0';
	top_input = 1;
	bot_input = 0;
}

Composer::~Composer()
{
}



void Composer::enqueue_input(uint8_t data){

	//printf("enqueuing: %s %s\ntop_input: %d\n", data[0], data[1], top_input);
	input_queue[top_input++] = data & 2;
	input_queue[top_input++] = data & 1;
	//printf("top_input into input_queue: %d\n", top_input);


	top_input = (top_input >= MAX_ENQUEUED_SAMPLES * 2) ? 0 : top_input;

}

Beat Composer::compose_beat() {

	printf("composing...\n");
	Beat new_beat;

	// do elsewhere, just ensures there are at least 3 characters remaining in the queue, if there aren't it returns an empty beat
	size_t max_index = bot_input + 2;
	max_index = (max_index >= MAX_ENQUEUED_SAMPLES * 2) ? bot_input + 2 - (MAX_ENQUEUED_SAMPLES * 2) : bot_input + 2;
	//if (max_index >= top_input) {
	//	printf("max_index >= top input in compose_beat...\n");
	//	uint16_t beat_scl = beat_scale[0];
	//	uint16_t beat = beat_scl - 4;

	//	new_beat.length = beat_scl;

	//	// guitar samples from input
	//	// (silent sample)
	//	new_beat.sample_handles.push_back(24);

	//	// drum samples
	//	new_beat.sample_handles.push_back((beat * 4) + (Squirrel3(bot_input) % 4));

	//	// bass samples
	//	new_beat.sample_handles.push_back((beat * 4) + (Squirrel3(bot_input) % 4));


	//	printf("returning silent sample\n");
	//	return new_beat;
	//}


	// iterate through the first three characters of the input_queue and add them to concat
	printf("iterating through first three chars of input_queue in composer\nbot_input: %d\ntop input: %d\nmax_index = %d\n", bot_input, top_input, max_index);
	if ((max_index <= top_input) || (max_index >= top_input && top_input < bot_input)) {
		concat = 0;
		for (size_t i = 0; i < 3; i++) {
			size_t index = bot_input + i;
			index = index >= MAX_ENQUEUED_SAMPLES * 2 ? bot_input + i - (MAX_ENQUEUED_SAMPLES * 2) : bot_input + i;


			concat += input_queue[index] ? 1 << (2 - i) : 0;

			//printf("char num: %d\nindex num: %d\nconcat[i]: %s\n", i, index, concat[i]);

		}
		bot_input++;
		bot_input = (bot_input >= MAX_ENQUEUED_SAMPLES * 2) ? 0 : bot_input;
	}


	// null terminator

	printf("concat in composer: %d\n", concat);
	uint16_t beat = beat_scale[concat];
	size_t beat_index = beat - 4;
	float beat_scl = beat * 0.25;


	// guitar samples from input
	new_beat.sample_handles.push_back((concat * 3) + (Squirrel3(seed++) % 3));
	// guitar volume
	new_beat.scale.push_back(0.16);

	// drum samples based on beat index taken from input
	new_beat.sample_handles.push_back((beat_index * 4) + (Squirrel3(seed++) % 4));
	// drum volume
	new_beat.scale.push_back(0.18);

	// bass samples based on beat index taken from input
	new_beat.sample_handles.push_back((beat_index * 4) + (Squirrel3(seed++) % 4));
	// bass volume
	new_beat.scale.push_back(0.12);

	new_beat.length = beat;
	//printf("incrementing bot_input in composer %d\n", bot_input);

	return new_beat;
}
