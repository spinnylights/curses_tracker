#ifndef E0cafe4a083848ed8abf19ae2e8146fa
#define E0cafe4a083848ed8abf19ae2e8146fa

#include "curve.hpp"
#include "sdl_util.hpp"

#include <SDL2/SDL.h>

#include <memory>
#include <limits>

class Audio {
public:
    struct data {
        std::shared_ptr<Curve> curve;
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

    int sr() const { return sampr; }

private:
    static double g_pos;

    struct data       d;
    SDL_AudioDeviceID dev;
    int               sampr;

    template<typename T>
    static T conv_samp(double samp) // samp is from -1.0 to 1.0
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
    static void callback(void* userdata, Uint8* stream, int len)
    {
        auto data = reinterpret_cast<struct Audio::data*>(userdata);
        auto chans = reinterpret_cast<T*>(stream);

        //static const double freq1 = Note(7, 00, 0).freq();
        //static const double freq1 = 215.9744711355553;
        static const double freq1 = Note(4, 13, 0).freq();
        //static const double freq1 = 220.72051857472195;
        //static const double freq2 = Note(7, 14, 0).freq();
        //static const double freq2 = 256;
        static const double freq2 = Note(4, 0, 0).freq();
        //static const double freq2 = 261.62560999936;
        //static const double freq3 = Note(7, 31, 0).freq();
        //static const double freq3 = 323.94894327688235;
        static const double freq3 = Note(4, 44, 0).freq();
        static const double freq4 = Note(5, 9, 0).freq();
        //static const double freq3 = 331.06773395883755;

        for (int i = 0; i < data->samp_cnt*data->chan_cnt; i += data->chan_cnt) {
            auto samp = conv_samp<T>(data->curve->get(freq1*g_pos) * 0.2);
            samp += conv_samp<T>(data->curve->get(freq2*g_pos) * 0.2);
            samp += conv_samp<T>(data->curve->get(freq3*g_pos) * 0.2);
            samp += conv_samp<T>(data->curve->get(freq4*g_pos) * 0.2);
            //float samp = data->curve->get(data->pos) * 0.9;
            //float samp = std::sin(2*M_PI*freq*g_pos);
            for (int j = i; j < i + data->chan_cnt; ++j) {
                if (j <= i + 1) {
                    chans[j] = samp;
                } else {
                    chans[j] = 0.0;
                }
            }
            //chans[i] = samp;
            //chans[i+1] = samp;
            //chans[0] = samp;
            //chans[1] = samp;
            //data->pos += data->tick_len;
            //data->pos = std::fmod(data->pos, 1.0);
            g_pos += data->tick_len;
            //g_pos = std::fmod(g_pos, 1.0);
        }
    }

};

#endif
