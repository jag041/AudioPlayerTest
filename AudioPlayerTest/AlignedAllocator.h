#pragma once

// std includes
#include <iostream>
#include <assert.h>
#include <vector>

// my includes
#include "GlobalConsts.h"
#include "ErrorHandler.h"

/*

	remember to implement a type of handle wrapper in the processing tables which update each time the -
	defragmenting sort happens so that no pointers become invalidated!!!!!

*/

class AlignedAllocator /* : public HandleArray ?? maybe add inheritence and get rid of the internal handles? */
{
public:
	AlignedAllocator() {
		// initialize with init function

	}

	void init(size_t num, size_t align, size_t num_allocations) {
		size = num;
		alignment = align;
		num_allocs = num_allocations;
		// assert alignment is a power of 2 and greater than a uint8_t which will store the alignment shift
		assert(((alignment & (alignment - 1)) == 0) && alignment >= sizeof(uint8_t) && alignment <= 256);
		data = malloc(size + (alignment * num_allocations));
		top = data;

	}

	~AlignedAllocator() {

		std::cout << "entering aligned allocator destructor" << std::endl;
		free(data);

	}

	void* set_data(void* source, size_t num_bytes) {

		size_t worst_case = num_bytes + alignment;

		assert((uint8_t*)top + worst_case <= (uint8_t*)data + (size + (alignment * num_allocs)));

		//std::cout << "raw pointer: " << top << std::endl;
		uint8_t* align_ptr = (uint8_t*)alignPointer(top, alignment);

		//std::cout << "aligned pointer after alignPointer call: " << (void*)align_ptr << std::endl;
		// moves the pointer up 1 alignment if the pointer shift is 0
		size_t shift = align_ptr - (uint8_t*)top;
		//std::cout << "ptrdiff shift: " << shift << std::endl;
		align_ptr += (shift ? 0 : alignment);
		// fix this because if buffer size is 256, shift will always = 0 when set to the length of alignment
		uint8_t* shift_ptr = align_ptr - 1;
		*shift_ptr = (uint8_t)shift;
		//std::cout << "aligned pointer: " << (void*)align_ptr << std::endl;
		//std::cout << "shift: " << shift << std::endl;

		// move top to end of worst case alignment
		top = (uint8_t*)top + worst_case;

		// align allocation
		memcpy((void*)align_ptr, source, num_bytes);


		return (void*)align_ptr;
	}


	void delete_data(void* at) {
		printf("deleteing aligned allocator at: %d\n", at);
		uint8_t* ptr = (uint8_t*)at;
		assert(ptr);
		ptrdiff_t shift = ptr[-1];
		top = (shift? ptr - shift : ptr - alignment);
		ptr = nullptr;
	}

	void clear_data() {

		//printf("clearing aligned allocator\n");
		top = data;

	}

private:
	void* data{};
	void* top{};
	size_t size{};
	size_t alignment{};
	size_t num_allocs{};

};