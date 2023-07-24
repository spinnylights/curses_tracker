#include "sdl_util.hpp"

#include <SDL2/SDL.h>

#include <sstream>
#include <stdexcept>

void sdl_check(int status, std::string msg)
{
    if (status < 0) {
        std::stringstream errmsg;
        errmsg << "SDL: " << msg << ": " << SDL_GetError();
        throw std::runtime_error(errmsg.str());
    }
}
