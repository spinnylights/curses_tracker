#include "synth.hpp"
#include "defs.hpp"

#include <fstream>

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
      chord_switch_del {tics(0.6666)},
      low {curve_desc_low},
      high {curve_desc_high}
{

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

    for (Sequencer::track_ndx i = 0; i < 8; ++i) {
        seq.add_track();
    }

    const Note freq_root {4, 0, 0};
    seq.add(0, ticks(0), freq_root);
    seq.add(1, ticks(0), freq_root + Note(0, 13));
    seq.add(2, ticks(0), freq_root + Note(0, 44));
    //seq.add(2, ticks(0), freq_root + Note(0, 34));
    seq.add(3, ticks(0), freq_root + Note(0, 10));
    seq.add(4, ticks(0), freq_root + Note(1,  9));
    seq.add(5, ticks(0), Note(Note::off));
    seq.add(6, ticks(0), Note(Note::off));
    seq.add(7, ticks(0), Note(Note::off));

    const Note freq_2_root {3, 30};
    seq.add(0, ticks(1), freq_2_root);
    seq.add(1, ticks(1), freq_2_root + Note(0, 23));
    seq.add(2, ticks(1), freq_2_root + Note(0, 52));
    //seq.add(2, ticks(1), freq_2_root + Note(0, 42));
    seq.add(3, ticks(1), freq_2_root + Note(1, 24));
    seq.add(4, ticks(1), freq_2_root + Note(1, 46));
    seq.add(5, ticks(1), freq_2_root + Note(1,  3));
    seq.add(6, ticks(1), freq_2_root + Note(1,  9));
    seq.add(7, ticks(1), freq_2_root + Note(1, 49));
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

    constexpr double lfo_rate = 1.0 / 1.75;
    //constexpr double lfo_rate = 4.0 / 9.0;
    //constexpr double lfo_rate = 2.0 / 3.0;
    //constexpr double lfo_rate = 4.0 / 7.0;
    //constexpr double lfo_rate = (M_PI) / (2*M_E);
    double lfo_sig = sine.get(pos*lfo_rate);
    double ramp_time = 0.3333 + 0.2*lfo_sig;
    double ramp_time_2 = ramp_time * 2;
    double delay_3_ramp_time = 24.0;
    double ramp_rate = 1.0 / ramp_time;
    double ramp_rate_2 = 1.0 / ramp_time_2;
    double delay_3_ramp_rate = 1.0 / delay_3_ramp_time;

    // signals (start)

    chord_switch.update(pos);

    chord_switch_del.length(tics(ramp_time_2));
    chord_switch_del.update(chord_switch.get(), pos);

    chord_toggle.update(chord_switch.get());
    chord_switch.rate(tics(chord_toggle.get(4.0, 1.0)));
    //chord_switch.rate(tics(chord_toggle.get(1.5, 1.0)));

    env_pos.reset(chord_switch.get());

    high.update(pos);
    low.update(pos);

    chord_del_latch.update(chord_switch_del.get(),
                           chord_toggle.get(false, true));

    // signals (temp end)

    // this is sort of a placeholder hack
    if (shut_down_started && shutting_down == false) {
        env_pos.reset();
        shutting_down = true;
    }

    // signals (continue)

    high_chd = chord_del_latch.get();
    chord_del_toggle.update(chord_del_latch.get());
    //seq.setpos(chord_del_toggle.get(ticks(0), ticks(1)));
    high_chd_seq = chord_del_toggle.get(false, true);
    seq.setpos(ticks(chord_del_latch.get()));

    // signals (end)

    if (chord_del_latch.get()) {
        for (Sequencer::track_ndx i = 0; i < 5; ++i) {
            high.note(seq.get_note(i));
            out += high.get() * 0.9;
        }
        for (Sequencer::track_ndx i = 5; i < 8; ++i) {
            high.note(seq.get_note(i));
            out += high.get() * 0.1;
        }
    } else {
        for (Sequencer::track_ndx i = 0; i < 5; ++i) {
            low.note(seq.get_note(i));
            out += low.get();
        }
    }

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

    out *= 0.9/5; // number of non-supplement tracks...rather a hack

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

    pos.update();
    env_pos.update();
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
