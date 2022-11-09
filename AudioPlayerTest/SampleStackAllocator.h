#pragma once

#define DITHER

// std includes
#include <assert.h>
#include <vector>

// my includes
#include "GlobalConsts.h"
#include "ErrorHandler.h"

template<typename T>
class SampleStackAllocator /* : public HandleArray ?? maybe add inheritence and get rid of the internal handles? */
{
public:

	SampleStackAllocator() {}


	~SampleStackAllocator() {

		free(data);

	}


	void init(size_t num_samps) {

		max_num_samps = num_samps;
		top = 0;

		data = malloc(sizeof(T) * max_num_samps);

	}

	void set_size(uint32_t size) {

		cur_size = size;
	}

	void write_data(T* source, uint32_t num_samples, float scale) {
		
		// consider adding spatialization here


		assert(num_samples <= max_num_samps);


		std::cout << "top == " << top << " in stack" << std::endl;
		if (top == 0) {
			for (size_t i = 0; i < num_samples; i++){
				((T*)data)[i] = (source[i]  
#ifdef DITHER 
					+ lazy_dither((uint16_t)source)
#endif
					) * scale;
			}
			top++;
			return;
		}

		for (size_t i = 0; i < num_samples; i++)
		{
			
			((T*)data)[i] += source[i] * scale;
		}
		top++;
	}

	uint32_t get_current_size() const {
		return cur_size;
	}

	T* get_data() {

		return (T*)data;

	}

	void clear_data() {
		
		top = 0;

	}

private:

	void* data{};
	size_t max_num_samps{};
	size_t top{};
	uint32_t cur_size{0};

};

#ifdef DITHER
#undef DITHER
#endif