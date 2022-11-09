
#include <stdio.h>
#include <iostream>
#include <math.h>

#include <dsound.h>
#include <dsconf.h>

#include <portaudio.h>

#include <SDL.h>
#undef main


#include "AudioPlayer.h"
#include "WAVParser.h"
#include "PCMSample.h"
#include "AlignedAllocator.h"
#include "InputCommandQueue.h"
#include "Conductor.h"




typedef struct
{
    float left_phase;
    float right_phase;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{

    printf("callback called ");
    /* Cast data passed through stream to our structure. */
    paTestData* data = (paTestData*)userData;
    float* out = (float*)outputBuffer;
    unsigned int i;
    (void)inputBuffer; /* Prevent unused variable warning. */

    for (i = 0; i < framesPerBuffer; i++)
    {
        *out++ = data->left_phase;  /* left */
        *out++ = data->right_phase;  /* right */
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if (data->left_phase >= 1.0f) data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if (data->right_phase >= 1.0f) data->right_phase -= 2.0f;
    }
    return 0;
}

static paTestData testData;

/*******************************************************************/


int main()
{

    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVENTS) < 0 || SDL_Init(SDL_INIT_VIDEO) < 0) {

        std::cout << "failed to init sdl" << std::endl;

        return 1;
    }

    SDL_Window* const window = SDL_CreateWindow(
        "Input Command Test",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_VULKAN);

    if (!window)
    {
        std::cout << "failed to create window" << std::endl;
        return 1;
    }



    try {

        // forgive this incomprehensible mess that isn't fit to be called an API

        std::vector<std::string> guitar_file_names = {
                                    "slimer_guitar-001.wav",
                                    "slimer_guitar-002.wav",
                                    "slimer_guitar-003.wav",
                                    "slimer_guitar-004.wav",
                                    "slimer_guitar-005.wav",
                                    "slimer_guitar-006.wav",
                                    "slimer_guitar-007.wav",
                                    "slimer_guitar-008.wav",
                                    "slimer_guitar-009.wav",
                                    "slimer_guitar-010.wav",
                                    "slimer_guitar-011.wav",
                                    "slimer_guitar-012.wav",
                                    "slimer_guitar-013.wav",
                                    "slimer_guitar-014.wav",
                                    "slimer_guitar-015.wav",
                                    "slimer_guitar-016.wav",
                                    "slimer_guitar-017.wav",
                                    "slimer_guitar-018.wav",
                                    "slimer_guitar-019.wav",
                                    "slimer_guitar-020.wav",
                                    "slimer_guitar-021.wav",
                                    "slimer_guitar-022.wav",
                                    "slimer_guitar-023.wav",
                                    "slimer_guitar-024.wav"
        };

        std::vector<std::string> drum_file_names = {
                                    "slimer_drums-001.wav",
                                    "slimer_drums-002.wav",
                                    "slimer_drums-003.wav",
                                    "slimer_drums-004.wav",
                                    "slimer_drums-005.wav",
                                    "slimer_drums-006.wav",
                                    "slimer_drums-007.wav",
                                    "slimer_drums-008.wav",
                                    "slimer_drums-009.wav",
                                    "slimer_drums-010.wav",
                                    "slimer_drums-011.wav",
                                    "slimer_drums-012.wav"
        };

        std::vector<std::string> bass_file_names = {
                                    "slimer_bass-001.wav",
                                    "slimer_bass-002.wav",
                                    "slimer_bass-003.wav",
                                    "slimer_bass-004.wav",
                                    "slimer_bass-005.wav",
                                    "slimer_bass-006.wav",
                                    "slimer_bass-007.wav",
                                    "slimer_bass-008.wav",
                                    "slimer_bass-009.wav",
                                    "slimer_bass-010.wav",
                                    "slimer_bass-011.wav",
                                    "slimer_bass-012.wav"
        };

        std::vector<std::string> instruments = {
            "guitar/",
            "drums/",
            "bass/"
        };

        std::vector<std::vector<std::string>> file_names = { guitar_file_names, drum_file_names, bass_file_names };

        WavDirInputFormat wav_dir_format;
        wav_dir_format.root_file_path = "./../samples/";
        wav_dir_format.instruments = instruments;
        wav_dir_format.files = file_names;
        wav_dir_format.num_files = guitar_file_names.size() + drum_file_names.size() + bass_file_names.size();


        // tempo is argument into conductor
        Conductor conductor(144);

        Composer composer{};

        AlignedAllocator sample_allocator;
        SampleQueue buffer_queue(&sample_allocator, wav_dir_format);

        AlignedAllocator input_allocator;
        InputCommandQueue command_queue(&input_allocator);

        SDL_Event e;
        bool bQuit = false;

        AudioPlayer sound_player(&buffer_queue);

        printf("querying devices on host side: \n");

        sound_player.query_devices();

        sound_player.open_stream();
        if(sound_player.is_playing())
            sound_player.stop_streaming();
        //main loop
        while (!bQuit)
        {
            bool playing = sound_player.is_playing();
            //Handle events on queue
            //does not enter loop unless an SDL event is registered
            while (SDL_PollEvent(&e))
            {

                // try using SDL_scancodes

                int key = e.key.keysym.sym;

                //close the window when user alt-f4s or clicks the X button	
                if (e.type == SDL_QUIT || key == SDLK_ESCAPE)
                {
                    std::cout << "quitting" << std::endl;
                    bQuit = true;
                    break;
                }

                if (key == SDLK_SPACE) {
                    if (!playing) {
                        printf("***************************************starting stream****************************************************\n");
                        buffer_queue.write_samples(composer.compose_beat());
                        conductor.start();
                        sound_player.start_streaming();
                    }
                }

                if (playing) {

                    // enqueue commands based on arrow key pressed
                    if (e.type == SDL_KEYDOWN)
                    {
#ifndef N_DEBUG
                        std::cout << "button press detected" << std::endl;
#endif
                        //if (key == SDLK_LSHIFT)
                        //{
                        //    // reverse iterators :D
                        //  
                        //    if (key == SDLK_UP)
                        //        printf("shift up******");
                        //}
                        if (key == SDLK_UP) {
                            printf("enqueuing up\n");
                            InputCommand input(&composer, 0); // 00
                            command_queue.add_command(&input);
                        }
                        if (key == SDLK_RIGHT) {
                            printf("enqueuing right\n");
                            InputCommand input(&composer, 1); // 01
                            command_queue.add_command(&input);
                        }
                        if (key == SDLK_DOWN) {
                            printf("enqueuing down\n");
                            InputCommand input(&composer, 3); // 11
                            command_queue.add_command(&input);
                        }
                        if (key == SDLK_LEFT) {
                            printf("enqueuing left\n");
                            InputCommand input(&composer, 2); // 10
                            command_queue.add_command(&input);
                        }

                    }

                }

            }
            if (conductor.time_out() && playing)
            {
                //printf("enqueuing commands\n");
                command_queue.execute_commands();
                printf("writing samples\n");
                conductor.set_current_beat(
                    buffer_queue.write_samples(
                        composer.compose_beat()
                    )
                );
                
                printf("post writing samples\n");
            }
        }
        sound_player.stop_streaming();
        sound_player.close_stream();
    }
    catch (ErrorHandler& e)
    {
        std::cout << e.what() << std::endl;
    }

    printf("done... exiting... \n");
    return 0;

}


//int main(void);
//int main(void)
//{
//    PaStream* stream;
//    PaError err;
//
//    printf("PortAudio Test: output sawtooth wave.\n");
//    /* Initialize our data for use by callback. */
//    testData.left_phase = testData.right_phase = 0.0;
//    /* Initialize library before making any other calls. */
//    err = Pa_Initialize();
//    if (err != paNoError) goto error;
//
//    /* Open an audio I/O stream. */
//    err = Pa_OpenDefaultStream(&stream,
//        0,          /* no input channels */
//        2,          /* stereo output */
//        paFloat32,  /* 32 bit floating point output */
//        SAMPLE_RATE,
//        256,        /* frames per buffer */
//        patestCallback,
//        &testData);
//    if (err != paNoError) goto error;
//
//    err = Pa_StartStream(stream);
//    if (err != paNoError) goto error;
//
//    /* Sleep for several seconds. */
//    Pa_Sleep(NUM_SECONDS * 1000);
//
//    err = Pa_StopStream(stream);
//    if (err != paNoError) goto error;
//    err = Pa_CloseStream(stream);
//    if (err != paNoError) goto error;
//    Pa_Terminate();
//    printf("Test finished.\n");
//    return err;
//error:
//    Pa_Terminate();
//    fprintf(stderr, "An error occurred while using the portaudio stream\n");
//    fprintf(stderr, "Error number: %d\n", err);
//    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
//    return err;
//}