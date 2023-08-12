#ifndef m97a6d76838d4d9fb8fd7b298863659a
#define m97a6d76838d4d9fb8fd7b298863659a

#include <cstdint>
#include <chrono>
#include <tuple>

template<std::intmax_t N>
using time_samps = std::chrono::duration<std::uintmax_t, std::ratio<1,N>>;

typedef std::chrono::duration<double> time_f;

template<std::intmax_t N>
constexpr time_f samp_len_f = time_f(time_samps<N>(1));

typedef double sample;

typedef std::tuple<sample, sample> stereo_sample;

constexpr unsigned       samp_frac_bits = 52;
constexpr std::uintmax_t samp_frac_sz   = 1ul << samp_frac_bits;

template<std::uintmax_t SampleRate>
using ticks_sr = std::chrono::duration<
    std::uintmax_t,
    std::ratio<1, samp_frac_sz>
>;

//min (attoscs): 222.044604925031
//max (minutes): 68 (approx.)
//
//440 Hz:   0.0022727272727272
//440 Hz f: 0.00227272727272727
constexpr std::uintmax_t inner_sr = 48000;
typedef ticks_sr<inner_sr> ticks;

constexpr ticks ticks_per_sec =
    std::chrono::round<ticks>(std::chrono::duration<std::uintmax_t>(1));
constexpr ticks ticks_per_samp =
    std::chrono::round<ticks>(std::chrono::duration<std::uintmax_t,
                                                    std::ratio<1,
                                                               inner_sr>>(1));

constexpr ticks tics(long double f)
{
    return std::chrono::round<ticks>(std::chrono::duration<long double>(f));
}

#endif
