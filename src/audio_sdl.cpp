#include "audio_sdl.hpp"
#include "sdl_util.hpp"

#include <cstring>

double Audio::g_pos = 0.0;

auto open_dev(const SDL_AudioSpec& want, SDL_AudioSpec& have)
{
    const char* pick_most_suitable_dev = NULL;
    constexpr int open_for_playback = 0;

    auto out = SDL_OpenAudioDevice(pick_most_suitable_dev,
                                   open_for_playback,
                                   &want,
                                   &have,
                                   SDL_AUDIO_ALLOW_ANY_CHANGE);

    if (out == 0) {
        throw std::runtime_error("couldn't open audio device: "
                                 + std::string(SDL_GetError()));
    }

    return out;
};

Audio::Audio(struct data nd)
    : d {nd}
{
    char vdriver[] = "SDL_VIDEODRIVER=dummy";
    putenv(vdriver);

    sdl_check(SDL_Init(SDL_INIT_AUDIO), "failed to initialize");

    SDL_AudioSpec want {};
    SDL_AudioSpec have {};

    want.freq     = 48000;
    want.format   = AUDIO_F32;
    want.channels = 2;
    want.samples  = 1 << 12;

    dev = open_dev(want, have);

    switch (have.format) {
    case AUDIO_S8:
        want.callback = callback<std::int8_t>;
        break;
    case AUDIO_U8:
        want.callback = callback<std::uint8_t>;
        break;
    case AUDIO_S16LSB:
    case AUDIO_S16MSB:
        want.callback = callback<std::int16_t>;
        break;
    case AUDIO_U16LSB:
    case AUDIO_U16MSB:
        want.callback = callback<std::uint16_t>;
        break;
    case AUDIO_S32LSB:
    case AUDIO_S32MSB:
        want.callback = callback<std::int32_t>;
        break;
    case AUDIO_F32LSB:
    case AUDIO_F32MSB:
        want.callback = callback<float>;
        break;
    default:
        throw std::logic_error("unhandled SDL audio format: "
                               + std::to_string(have.format));
    }

    want.userdata = &d;

    SDL_CloseAudioDevice(dev);

    dev = open_dev(want, have);

#ifdef NDEBUG
    std::cout << "freq: " << have.freq << "\n"
              << "want freq: " << want.freq << "\n"
              << "format: " << have.format << "\n"
              << "want format: " << want.format << "\n"
              << "    AUDIO_F32 " << AUDIO_F32 << "\n"
              << "channels: " << static_cast<unsigned>(have.channels) << "\n"
              << "want channels: " << static_cast<unsigned>(want.channels) << "\n"
              << "samples: " << have.samples << "\n"
              << "want samples: " << want.samples << "\n"
              << "callback: " << have.callback << "\n"
              << "want callback: " << want.callback << "\n"
              << "userdata: " << have.userdata << "\n"
              << "want userdata: " << want.userdata << "\n";
#endif

    sampr = have.freq;

    d.tick_len = 1.0/static_cast<double>(sampr);

    d.samp_cnt = have.samples;
    d.chan_cnt = have.channels;

    SDL_PauseAudioDevice(dev, 0);
}

Audio::~Audio() noexcept
{
    SDL_CloseAudioDevice(dev);
    SDL_Quit();
}
