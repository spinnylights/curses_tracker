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

#endif
