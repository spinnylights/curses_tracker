#ifndef m97a6d76838d4d9fb8fd7b298863659a
#define m97a6d76838d4d9fb8fd7b298863659a

#include <cstdint>
#include <chrono>
#include <tuple>

template<std::intmax_t N>
using time_samps = std::chrono::duration<std::uintmax_t, std::ratio<1,N>>;

typedef std::chrono::duration<double, std::ratio<1,1>> time_f;

template<std::intmax_t N>
constexpr time_f samp_len_f = time_f(time_samps<N>(1));

typedef double sample;

typedef std::tuple<sample, sample> stereo_sample;

template<std::intmax_t SampleRate>
using ticks_sr = std::chrono::duration<
    std::uintmax_t,
    std::ratio<SampleRate, (1l << 62)>
>;

// fs:       10.4083408558608
// hours:    11 (approx.)
//
// 440 Hz cast:    0.00227272727272229
// 440 Hz 1/440.0: 0.00227272727272727
constexpr std::intmax_t inner_sr = 48000;
typedef ticks_sr<inner_sr> ticks;

#endif
