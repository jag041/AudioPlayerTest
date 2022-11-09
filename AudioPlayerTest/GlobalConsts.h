#pragma once

#define SLEEP_SECONDS (3)
#define BIT_DEPTH (16)
#define BYTE_DEPTH (BIT_DEPTH / 8)

#define NUM_OUTPUT_CHANNELS (2)
#define MAX_ENQUEUED_SAMPLES (8)

#define SAMPLE_RATE (48000)		// samples per second
#define SAMPLES_PER_BUFFER (64) //
#define BUFFERS_PER_TICK (25)   // 30 ticks per sec @ 64 buffer size

#define SAMPLES_PER_TICK (SAMPLES_PER_BUFFER * BUFFERS_PER_TICK)  // 1600 @ 64 spb and 25 bpt
#define TICKS_PER_SECOND (30)
#define GET_SAMPLES_PER_BEAT(tempo) (SAMPLES_PER_TICK * SAMPLE_RATE * 60 / tempo)

#define DITHER_MOD_VAL (2)

#define GUITAR_OUTPUT_VALUE (0.22)
#define DRUM_OUTPUT_VALUE (0.2)
#define BASS_OUTPUT_VALUE (0.17)


#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (600)


#include <string>
#include <sstream>
#include <assert.h>
#include <limits>


#include "ErrorHandler.h"



inline uintptr_t alignAddress(uintptr_t addr, size_t alignment) {

	const size_t mask = alignment - 1;
	assert((alignment & mask) == 0);		// ensure it is a power of 2 for alignment

	// bumps the address up to the nearest 0x00
	return (addr + mask) & ~mask;

}


inline uintptr_t alignPointer(void* ptr, size_t alignment) {

	const uintptr_t addr = (uintptr_t)ptr;
	const uintptr_t addr_aligned = alignAddress(addr, alignment);

	return addr_aligned;

}


inline uint32_t Squirrel3(int position)
// if random seed is needed
//uint32_t Squirrel3( int position , uint32_t seed )
{
    constexpr unsigned int BIT_NOISE1 = 0xB5297A4D;
    constexpr unsigned int BIT_NOISE2 = 0x68E31DA4;
    constexpr unsigned int BIT_NOISE3 = 0x1B56C4E9;

    unsigned int mangled = position;
    mangled *= BIT_NOISE1;
    // add seed here, or partway through the func
    // mangled += seed;
    mangled ^= (mangled >> 8);
    mangled += BIT_NOISE2;
    mangled ^= (mangled << 8);
    mangled *= BIT_NOISE3;
    mangled ^= (mangled >> 8);
    return mangled;
}

inline uint16_t lazy_dither(int seed) {

    return (Squirrel3(seed) % DITHER_MOD_VAL);

}