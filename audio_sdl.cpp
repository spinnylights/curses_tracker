#include "audio_sdl.hpp"
#include "sdl_util.hpp"

#include <SDL2/SDL.h>

Audio::Audio()
{
    char vdriver[] = "SDL_VIDEODRIVER=dummy";
    putenv(vdriver);

    sdl_check(SDL_Init(SDL_INIT_AUDIO), "failed to initialize");
}

Audio::~Audio() noexcept
{
    SDL_Quit();
}
