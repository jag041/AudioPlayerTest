#include "AudioPlayer.h"
#include <iostream>
#include "ErrorHandler.h"
#include "WAVParser.h"

//#include <pa_hostapi.h>


AudioPlayer::AudioPlayer(SampleQueue* bQueue) : sample_queue(bQueue)
{

	printf("entering audio player constructor\n");

	data = paBufferData();
	data.buff_queue = sample_queue;


	std::cout << "initializing PortAudio" << std::endl;
	err = Pa_Initialize();
	if (err != paNoError)
		printf("PortAudio initializer error in AudioPlayer constructor: %s\n", Pa_GetErrorText(err));

}


AudioPlayer::~AudioPlayer()
{


	printf("entering audio player destructor\n");
	err = Pa_Terminate();
	if (err != paNoError)
		printf("PortAudio Termination error in AudioPlayer destructor: %s\n", Pa_GetErrorText(err));


}

void AudioPlayer::query_devices()
{
	int numDevices{};
	numDevices = Pa_GetDeviceCount();
	const PaDeviceInfo* deviceInfo;
	printf("there are %d devices\n", numDevices);
	for (size_t i = 0; i < numDevices; ++i)
	{
		deviceInfo = Pa_GetDeviceInfo(i);
		printf("device %d: %s\n", i,  deviceInfo->name);
	}

	numDevices = Pa_GetHostApiCount();
	const PaHostApiInfo* apiInfo;
	printf("there are %d apis\n", numDevices);
	for (size_t i = 0; i < numDevices; ++i)
	{
		apiInfo = Pa_GetHostApiInfo(i);
		printf("api %d: %s\n", i, apiInfo->name);

	}

}

void AudioPlayer::open_stream()
{
	std::cout << "entering audio player open stream" << std::endl;
	data.frameIndex = 0;
	PaStreamParameters outParams;
	//PaStreamParameters inParams;

	outParams.channelCount = 2;
	outParams.hostApiSpecificStreamInfo = NULL;
	outParams.device = Pa_GetDefaultOutputDevice();
	outParams.sampleFormat = paInt16;
	outParams.suggestedLatency =
	Pa_GetDeviceInfo(outParams.device)->defaultHighOutputLatency;




	std::cout << "opening PA stream" << std::endl;
	//err = Pa_OpenDefaultStream(&stream, 0, 2, paInt16, SAMPLE_RATE, SAMPLES_PER_BUFFER, paBufferReadCallback, &data);
	
	err = Pa_OpenStream(&stream, 
						NULL, 
						&outParams, 
						SAMPLE_RATE, 
						SAMPLES_PER_BUFFER, 
						0,
						paBufferReadCallback,
						&data);

	std::cout << "post opening PA stream" << std::endl;
	if (err != paNoError)
		printf("PortAudio stream opening error in openStream function: %s\n", Pa_GetErrorText(err));
}

void AudioPlayer::start_streaming()
{
	printf("entering audio player start streaming\n");


	err = Pa_StartStream(stream);

	if (err != paNoError)
		ErrorHandler(std::string("error starting stream!: %s", Pa_GetErrorText(err)));

	//int i = 0;


	// move into pause / play functionality?
	// move outside of start stream function in any case to expose looping
	//while (Pa_IsStreamActive(stream))
	//{
	//	printf("pa sleep %d\n", i++);
	//	Pa_Sleep(48);
	//}

	//printf("0\n");
	//Pa_Sleep(1000);

	//printf("1\n");
	//Pa_Sleep(1000);

	//printf("2\n");
	//Pa_Sleep(1000);

	//printf("3\n");
	//Pa_Sleep(1000);

	//printf("4\n");
	//Pa_Sleep(1000);

	//printf("5\n");
	printf("exiting audio player start streaming\n");
}

void AudioPlayer::stop_streaming()
{
	printf("entering audio player stop streaming\n");
	err = Pa_StopStream(stream);
	if(err != paNoError)
		printf("PortAudio stream stopping error in stop Streaming function: %s\n", Pa_GetErrorText(err));


}

void AudioPlayer::close_stream()
{

	printf("entering audio player closeStream\n");
	err = Pa_CloseStream(stream);
	if (err != paNoError)
		printf("PortAudio stream closing error in closeStream function: %s\n", Pa_GetErrorText(err));

}

bool AudioPlayer::is_playing() const
{
	return Pa_IsStreamActive(stream);
}



//int AudioPlayer::BufferReadCallback(const void* inputBuffer, void* outputBuffer,
//	unsigned long framesPerBuffer,
//	const PaStreamCallbackTimeInfo* timeInfo,
//	PaStreamCallbackFlags statusFlags)
//{
//	//printf("callback called");
//	uint16_t* sample_ptr = data.sample[data.sampleIndex];
//	uint16_t* rptr = sample_ptr + data.frameIndex;
//	uint16_t* wptr = (uint16_t*)outputBuffer;
//	unsigned int i;
//	int finished;
//	uint32_t framesLeft = data.maxFrameIndex[data.sampleIndex] - data.frameIndex;
//
//	(void)inputBuffer; /* Prevent unused variable warnings. */
//	(void)timeInfo;
//	(void)statusFlags;
//
//	//printf("writing buffers in callback");
//	if (framesLeft < framesPerBuffer)
//	{
//		/* final buffer... */
//		for (i = 0; i < framesLeft; i++)
//		{
//			*wptr++ = *rptr++;  /* left */
//			*wptr++ = *rptr;  /* right */
//		}
//		for (; i < framesPerBuffer; i++)
//		{
//			*wptr++ = 0;  /* left */
//			*wptr++ = 0;  /* right */
//		}
//
//		data.frameIndex += framesLeft;
//		finished = paComplete;
//	}
//	else
//	{
//		for (i = 0; i < framesPerBuffer; i++)
//		{
//			*wptr++ = *rptr++;  /* left */
//			*wptr++ = *rptr;  /* right */
//		}
//		data.frameIndex += framesPerBuffer;
//		finished = paContinue;
//	}
//	return finished;
//}