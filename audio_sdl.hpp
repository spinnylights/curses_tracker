#ifndef E0cafe4a083848ed8abf19ae2e8146fa
#define E0cafe4a083848ed8abf19ae2e8146fa

#include <SDL2/SDL.h>

class Audio {
public:
    Audio();

    ~Audio() noexcept;

    int sr() const { return sampr; }

private:
    static void callback(void* userdata, Uint8* stream, int len);
    int sampr;
};

#endif
