#include "audio_sdl.hpp"
#include "sdl_util.hpp"

#include <cstring>

Audio::Audio()
{
    char vdriver[] = "SDL_VIDEODRIVER=dummy";
    putenv(vdriver);

    sdl_check(SDL_Init(SDL_INIT_AUDIO), "failed to initialize");

    SDL_AudioSpec want;
    SDL_AudioSpec have;
    SDL_AudioDeviceID dev;

    std::memset(&want, 0, sizeof(want));
    want.freq     = 48000;
    want.format   = AUDIO_F32;
    want.channels = 2;
    want.samples  = 4096;
    want.callback = callback;
    dev = SDL_OpenAudioDevice(NULL,
                              0,
                              &want,
                              &have,
                              SDL_AUDIO_ALLOW_FREQUENCY_CHANGE
                              | SDL_AUDIO_ALLOW_SAMPLES_CHANGE);
    sampr = have.freq;
}

Audio::~Audio() noexcept
{
    SDL_Quit();
}

void Audio::callback(void* userdata, Uint8* stream, int len)
{
    std::memset(stream, 0, len);
}
