#ifndef fdf6dd971a60454b980dd4e3fe2ed6d5
#define fdf6dd971a60454b980dd4e3fe2ed6d5

#include "curve.hpp"
#include "curves.hpp"
#include "delay.hpp"
#include "pulse_train.hpp"
#include "pulse_delay.hpp"
#include "toggle.hpp"
#include "defs.hpp"
#include "stopwatch.hpp"
#include "sequencer.hpp"
#include "wavetable.hpp"
#include "latch.hpp"

#include <memory>
#include <queue>

class Synth {
public:
    //static constexpr double ticks_per_samp = 256;

    typedef double samp_t;
    typedef std::tuple<samp_t, samp_t> stereo_sample;

    Synth(Curves& curves,
          std::string curve_desc_low,
          std::string curve_desc_high,
          double sample_rate);

    stereo_sample sample();

    bool shutdown();

    bool high_chord() const { return high_chd; }
    bool high_chord_seq() const { return high_chd_seq; }
    bool quiet() const { return quet; }
    bool amp_ceil() const { return max_amp; }

private:
    Curve upramp;
    Curve downupramp;
    Curve downramp;
    Curve sine;
    Curve delay_2_start_env;
    //std::queue<samp_t> delay;
    //std::queue<samp_t> delay_2;
    //std::queue<samp_t> delay_3;
    unsigned long sr;
    Delay::buffer_ndx delay_len_1 = std::round(sr * 6.0);
    Delay::buffer_ndx delay_rate_1 = 1;
    Delay::buffer_ndx delay_len_2 = std::round(sr * 0.05);
    Delay::buffer_ndx delay_rate_2 = 2;
    Delay::buffer_ndx delay_len_3 = std::round(sr * 0.025);
    Delay::buffer_ndx delay_rate_3 = 3;
    Delay delay_1;
    Delay delay_2;
    Delay delay_3;
    PulseTrain chord_switch;
    PulseDelay chord_switch_del;
    Toggle chord_toggle;
    Toggle chord_del_toggle;
    Latch<bool> chord_del_latch;
    Stopwatch env_pos;
    Stopwatch pos;
    Sequencer seq;
    Wavetable high;
    Wavetable low;
    std::queue<samp_t>::size_type delay_len = std::round(sr * 4.2);
    Synth::samp_t max_amp = 0.0;
    //double pos = 0.0; // in the long run maybe not
    //double tick_len;
    //double env_pos = 0.0;
    //unsigned samps_to_wait = sr * 10;
    unsigned samps_to_wait = 0;
    bool high_chd = false;
    bool high_chd_seq = false;
    bool shut_down_started = false;
    bool shutting_down = false;
    bool finished = false;
    bool quet = false;
};

#endif
