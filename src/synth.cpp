#include "synth.hpp"
#include "defs.hpp"

#include <fstream>

//const std::vector<double> freqs {
//    Note(4, 0, 0).freq(),  // -0.13, -0.44, -1.09
//    Note(4, 13, 0).freq(), // +0.13, +31
//    Note(4, 44, 0).freq(),
//    Note(5, 9, 0).freq(),
//};

const Note freq_root {4, 0, 0};

const std::vector<double> freqs {
    freq_root.freq(),  // -0.13, -0.44, -1.09
    (freq_root + Note(0, 13)).freq(), // +0.13, +31
    (freq_root + Note(0, 44)).freq(),
    (freq_root + Note(0, 10)).freq(),
    (freq_root + Note(1,  9)).freq(),
};

//const std::vector<double> freqs_2 {
//    Note(4, 8).freq(),   // 0.31
//    Note(4, 29).freq(),  // 0.52
//    Note(3, 30).freq(),  // 0.0
//    Note(5, 23).freq(),  // 1.46
//};

//const std::vector<double> freqs_2 {
//    Note(3, 30).freq(),  // 0.00
//    Note(4,  8).freq(),  // 0.31
//    Note(4, 29).freq(),  // 0.52
//    Note(5, 23).freq(),  // 1.46
//};

const Note freq_2_root {3, 30};

const std::vector<double> freqs_2 {
    freq_2_root.freq(),
    (freq_2_root + Note(0, 23)).freq(),
    (freq_2_root + Note(0, 52)).freq(),
    (freq_2_root + Note(1, 24)).freq(),
    (freq_2_root + Note(1, 46)).freq(),
    //(freq_2_root + Note(1, 51)).freq(),
    //(freq_2_root + Note(1, 8)).freq(),
    //(freq_2_root + Note(1, 6)).freq(),
    //(freq_2_root + Note(1, 5)).freq(),
    //(freq_2_root + Note(1, 1)).freq(),
    //(freq_2_root + Note(2, 0)).freq(),
};

const std::vector<double> freqs_2_2 {
    //(freq_2_root + Note(1, 6)).freq(),
    (freq_2_root + Note(1, 3)).freq(),
    (freq_2_root + Note(1, 9)).freq(),
    (freq_2_root + Note(1, 49)).freq(),
};

//const std::vector<double> freqs {
//    Note(4, 0, 0).freq(),
//    Note(4, 9, 0).freq(),
//    Note(5, 0, 0).freq(),
//    Note(4, 45, 0).freq(),
//};
//
////const std::vector<double> freqs_2 = freqs;
//
//const std::vector<double> freqs_2 {
//    Note(3, 49, 0).freq(),
//    Note(4, 40, 0).freq(),
//    Note(3, 31, 0).freq(),
//    Note(5, 44, 0).freq(),
//};

Synth::Synth(Curves& cs,
             std::string curve_desc_low,
             std::string curve_desc_high,
             time_f::rep sample_rate)
//    : tick_len {1.0/(sample_rate * ticks_per_samp)},
    : upramp {cs.newc()},
      downupramp {cs.newc()},
      downramp {cs.newc()},
      delay_2_start_env {cs.newc()},
      sine (cs.newc()),
      sr {static_cast<unsigned long>(std::round(sample_rate))},
      delay_1 {delay_len_1, delay_rate_1},
      delay_2 {delay_len_2, delay_rate_2},
      delay_3 {delay_len_3, delay_rate_3},
      chord_switch {tics(4.0)},
      chord_switch_del {tics(0.6666)}
{
    for (auto&& f : freqs) {
        auto cl = cs.newc();
        cl.parse(curve_desc_low + " " + std::to_string(f));
        cs1_low.push_back({std::move(cl), f});
        auto ch = cs.newc();
        ch.parse(curve_desc_high + " " + std::to_string(f));
        cs1_high.push_back({std::move(ch), f});
    }

    for (auto&& f : freqs_2) {
        auto cl = cs.newc();
        cl.parse(curve_desc_low + " " + std::to_string(f));
        cs2_low.push_back({std::move(cl), f});
        auto ch = cs.newc();
        ch.parse(curve_desc_high + " " + std::to_string(f));
        cs2_high.push_back({std::move(ch), f});
    }

    for (auto&& f : freqs_2_2) {
        auto cl = cs.newc();
        cl.parse(curve_desc_low + " " + std::to_string(f));
        cs2_2_low.push_back({std::move(cl), f});
        auto ch = cs.newc();
        ch.parse(curve_desc_high + " " + std::to_string(f));
        cs2_2_high.push_back({std::move(ch), f});
    }

    upramp.parse("segs");
    upramp.wrapm(Curve::sticky);
    downupramp.parse("segs 0 1 0");
    //downupramp.wrapm(Curve::ping_pong_sticky);
    downupramp.wrapm(Curve::ping_pong_looping);
    downramp.parse("segs 0 1 0");
    downramp.wrapm(Curve::sticky);
    sine.parse("soid");
    sine.wrapm(Curve::looping);
    delay_2_start_env.parse("segs -3 0 1");
    delay_2_start_env.wrapm(Curve::sticky);
}

bool Synth::shutdown()
{
    shut_down_started = true;
    return finished;
}

Synth::stereo_sample Synth::sample()
{
    double out = 0.0;

    if (samps_to_wait > 0) {
        --samps_to_wait;
        return {out, out};
    }

    double pos = std::chrono::duration<double>(posd).count();

    constexpr double lfo_rate = 1.0 / 1.75;
    //constexpr double lfo_rate = (M_PI) / (2*M_E);
    double lfo_sig = sine.get(pos*lfo_rate);
    double ramp_time = 0.3333 + 0.2*lfo_sig;
    double ramp_time_2 = ramp_time * 2;
    double delay_3_ramp_time = 24.0;
    double ramp_rate = 1.0 / ramp_time;
    double ramp_rate_2 = 1.0 / ramp_time_2;
    double delay_3_ramp_rate = 1.0 / delay_3_ramp_time;

    // signals (start)

    chord_switch.update(posd);

    chord_switch_del.length(tics(ramp_time_2));
    chord_switch_del.update(chord_switch.get(), posd);

    chord_toggle.update(chord_switch.get());
    chord_switch.rate(tics(chord_toggle.get(4.0, 1.0)));

    // signals (end)

    if (chord_switch.get()) {
        env_posd = ticks(0);
    }

    if (shut_down_started && shutting_down == false) {
        env_posd = ticks(0);
        shutting_down = true;
    }

    if (chord_switch_del.get()) {
        cfs = chord_toggle.get(&cs1_low, &cs2_high);
        high_chd = chord_toggle.get(false, true);
    }

    size_t i = 0;
    for (auto&& cf : *cfs) {
        if (high_chd) {

            out += std::get<0>(cf).get(std::get<1>(cf)*pos)
                   //;
                   * 0.9
                   + std::get<0>(cs2_2_high.at(i)).get(std::get<1>(cs2_2_high.at(i))*pos) * 0.1;
        } else {
            out += std::get<0>(cf).get(std::get<1>(cf)*pos);
        }
    }

    double env_pos = std::chrono::duration<double>(env_posd).count();

    double env_seek;
    double env_amp;
    if (pos < 1.0) {
        env_seek = env_pos*ramp_rate;
        env_amp = upramp.get(env_seek);
        out *= env_amp;
    } else {
        env_seek = env_pos*ramp_rate_2;
        env_amp = downupramp.get(env_seek);
        out *= env_amp;
    }

    if (shutting_down) {
        out *= downramp.get(env_pos*ramp_rate);
        if (env_pos > ramp_time) {
            finished = true;
        }
    }

    out *= 0.9/freqs.size();

    //delay.push(out);
    //delay_2.push(out);
    //delay_3.push(out);

    //if (delay.size() > delay_len
    //    || delay_2.size() > delay_len_2
    //    || delay_3.size() > delay_len_3) {
    //    out *= 0.0;
    //}

    //if (delay.size() > delay_len) {
    //    out += delay.front() * 0.1;
    //    delay.pop();
    //}

    //if (delay_2.size() > delay_len_2) {
    //    out += delay_2.front() * 0.1;
    //    delay_2.pop();
    //}

    //if (delay_3.size() > delay_len_3) {
    //    out += delay_3.front() * 0.4;
    //    delay_3.pop();
    //    out += delay_3.front() * 0.4;
    //    delay_3.pop();
    //}

    if (out < 0.05) {
        delay_2.adjrate(5);
        //delay_3.adjrate(4);
        quet = true;
    } else {
        delay_2.adjrate(2);
        //delay_3.adjrate(3);
        quet = false;
    }

    auto raw = out;
    auto del_2_samp = delay_2.pushpop(out);
                      //* 0.55;
                      //* delay_2_start_env.get(pos*delay_3_ramp_rate);
    auto del_3_samp = delay_3.pushpop(del_2_samp * out);
                      //* 0.15;

    auto preout = (0.5*raw + 0.25*del_2_samp + 0.25*del_3_samp)/3.0;
    auto del_3_samp_2 = delay_3.pushpop(preout);
    preout = (preout*0.7 + del_3_samp_2*0.3)/2.0;

    constexpr double stereo_delay_short_lfo_rate = 1.0 / 0.15;
    constexpr double stereo_delay_long_lfo_rate  = 1.0 / 8.25;
    constexpr double stereo_delay_left_lfo_rate  = 1.0 / 0.33;
    constexpr double stereo_delay_right_lfo_rate = 1.0 / 0.37;

    auto del_1_samp = delay_1.pushpop(preout);
    auto stereo_delay_adj = 0.1;
    auto lr_adj = stereo_delay_adj
                  * sine.get(pos*lfo_rate)
                  * sine.get(pos*stereo_delay_long_lfo_rate);
    auto left_adj = (lr_adj * sine.get(pos*stereo_delay_left_lfo_rate) + lr_adj)/2;
    auto right_adj = (lr_adj * sine.get(pos*stereo_delay_right_lfo_rate) + lr_adj)/2;
    double preout_adj = 0.55;
    auto left_out_raw = (preout*preout_adj + del_1_samp*left_adj)/2.0;
    auto right_out_raw = (preout*preout_adj + del_1_samp*right_adj)/2.0;

    auto left_out = left_out_raw*0.7 + right_out_raw*0.3;
    auto right_out = right_out_raw*0.7 + left_out_raw*0.3;

    double final_amp_adj = 35.0;
    left_out *= final_amp_adj;
    right_out *= final_amp_adj;

    posd += ticks_per_samp;
    env_posd += ticks_per_samp;
    //pos += ticks_per_samp*tick_len;
    //env_pos += ticks_per_samp*tick_len;

    if (preout > max_amp) {
        max_amp = preout;
    }

    if (left_out > 1.0) {
        left_out = 1.0;
    } else if (left_out < -1.0) {
        left_out = -1.0;
    }

    if (right_out > 1.0) {
        right_out = 1.0;
    } else if (right_out < -1.0) {
        right_out = -1.0;
    }

    //if (out < 0.1) {
    //    quet = true;
    //} else {
    //    quet = false;
    //}
    
    //std::ofstream log;
    //log.open("samps_log.txt", std::ios::app);
    //log << "out       : " << out << "\n"
    //    << "preout    : " << preout << "\n"
    //    << "left_out  : " << left_out << "\n"
    //    << "right_out : " << right_out << "\n"
    //    << "\n";
    //log.close();

    //std::ofstream log;
    //log.open("lfo_log.txt", std::ios::app);
    //log << "pos       : " << pos << "\n"
    //    << "env_seek  : " << env_seek << "\n"
    //    << "env_amp   : " << env_amp << "\n"
    //    << "ramp_time : " << ramp_time << "\n"
    //    << "in_cs2    : " << in_cs2 << "\n"
    //    << "high_chd  : " << high_chd << "\n"
    //    << "\n";
    //log.close();

    //auto rawraw = raw;
    //if (rawraw > 1.0) { rawraw = 1.0; }
    //else if (rawraw < -1.0) { rawraw = -1.0; }

    //return {rawraw, rawraw};
    return {left_out, right_out};
    //return {out, out};
}
