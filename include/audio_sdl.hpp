#ifndef E0cafe4a083848ed8abf19ae2e8146fa
#define E0cafe4a083848ed8abf19ae2e8146fa

#include "synth.hpp"
#include "sdl_util.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <limits>

class Audio {
public:
    struct data {
        std::shared_ptr<Synth> synth;
        double pos = 0.0;
        double tick_len;
        unsigned samp_cnt;
        unsigned chan_cnt;
    };

    Audio(struct data d);

    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    Audio(Audio&&) = delete;
    Audio& operator=(Audio&&) = delete;

    ~Audio() noexcept;

    // these functions may not stick around for very long
    void add_synth(std::shared_ptr<Synth> synth) { d.synth = synth; }
    void start_playback() { SDL_PauseAudioDevice(dev, 0); }

    int sr() const { return sampr; }
    double srf() const { return static_cast<double>(sampr); }

private:
    static double g_pos;

    struct data       d;
    SDL_AudioDeviceID dev;
    int               sampr;

    template<typename T>
    static T conv_samp(Synth::samp_t samp) // samp is from -1.0 to 1.0
    {
        T out;

        if (std::is_integral<T>::value) {
            constexpr T max = std::numeric_limits<T>::max();
            constexpr T min = std::numeric_limits<T>::min();

            double pre_out;
            if (std::is_signed<T>::value) {
                if (samp >= 0.0) {
                    pre_out = samp * max;
                } else {
                    pre_out = -samp * min;
                }
            } else {
                pre_out = (samp + 1.0) * (max/2);
            }

            out = static_cast<T>(std::round(pre_out));
        } else {
            out = static_cast<T>(samp);
        }

        return out;
    }

    template<typename T>
    static std::tuple<T, T> conv_stereo_samp(Synth::stereo_sample samp)
    {
        return {
            conv_samp<T>(std::get<0>(samp)),
            conv_samp<T>(std::get<1>(samp))
        };
    }

    template<typename T>
    static void callback(void* userdata, Uint8* stream, int len)
    {
        auto data = reinterpret_cast<struct Audio::data*>(userdata);
        auto chans = reinterpret_cast<T*>(stream);

        for (int i = 0; i < data->samp_cnt*data->chan_cnt; i += data->chan_cnt) {
            Synth::stereo_sample samp
                = conv_stereo_samp<T>(data->synth->sample());

            for (int j = i; j < i + data->chan_cnt; ++j) {
                if (j == i) {
                    chans[j] = std::get<0>(samp);
                } else if (j == i + 1) {
                    chans[j] = std::get<1>(samp);
                } else {
                //if (j <= 1) {
                //    chans[j] = std::get<0>(samp);
                //} else {
                    chans[j] = 0.0;
                }
            }

            g_pos += data->tick_len;
        }
    }

};

#endif
