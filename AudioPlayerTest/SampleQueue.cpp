#include "SampleQueue.h"
#include <iostream>
#include <vector>
#include <cmath>



SampleQueue::SampleQueue(AlignedAllocator* new_allocator, WavDirInputFormat format) : allocator(new_allocator) {

	std::cout << "constructing sample queue" << std::endl;

	cur_stack = 0;


	double_stack[0] = SampleStackAllocator<uint16_t>();
	double_stack[0].init(SAMPLE_RATE);

	double_stack[1] = SampleStackAllocator<uint16_t>();
	double_stack[1].init(SAMPLE_RATE);

	WAVParser parser(format);

	std::vector<std::vector<SampleData>> sample_handles = parser.get_samples_with_beats();

	printf("total size after parsing %d \n", parser.get_total_size());
	allocator->init(parser.get_total_size(), 16, format.num_files);
	handles.resize(format.instruments.size());

	for (size_t i = 0; i < sample_handles.size(); i++) {
		
		for (size_t j = 0; j < sample_handles[i].size(); j++) {
			printf("sending sample to allocator %d %d\nsize: %d\n", i, j, sample_handles[i][j].data_size);
			handles[i].emplace_back(
				(uint16_t*)allocator->set_data(
					sample_handles[i][j].sample_ptr,
					sample_handles[i][j].data_size * BYTE_DEPTH
				),
				sample_handles[i][j].data_size
			);
			
		}
	} 



}


SampleQueue::~SampleQueue(){
}


void SampleQueue::write_sample(InstrumentHandle inst_handle,  uint16_t handle, float scale) {
	SampleData* sample = &handles[inst_handle][handle];
	std::cout << "writing sample out: " << sample->sample_ptr << " writing to stack: " << cur_stack << std::endl;
	double_stack[cur_stack].write_data(sample->sample_ptr, sample->data_size, scale);

}

PCMSample* SampleQueue::get_next_sample() {

	size_t temp_index = cur_stack;
	cur_stack = !cur_stack;
	printf("\nreading from stack: %d\n", temp_index);
	read_data.maxFrameIndex = double_stack[temp_index].get_current_size();
	read_data.sample = double_stack[temp_index].get_data();
	double_stack[temp_index].clear_data();

	printf("sending read_data in sample queue \n");

	return &read_data;
}

uint16_t SampleQueue::write_samples(Beat cur_beat){
	double_stack[cur_stack].set_size(handles[0][cur_beat.sample_handles[0]].data_size);
	for (size_t i = 0; i < cur_beat.sample_handles.size(); i++) {
		write_sample((InstrumentHandle)i, cur_beat.sample_handles[i], cur_beat.scale[i]);
	}
	return cur_beat.length;
}




