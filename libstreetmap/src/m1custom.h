/*
#pragma once //protects against multiple inclusions of this header file

#include <nanoflann.hpp>  //M_PI and floor included there
//#include <ctime>
//#include <cstdlib>

#define EXTRA_PRECISION 1


//custom cosine - manages to fail 4 tests due to precision error :(
template<typename T>
inline T cosine(T x) noexcept
{
    constexpr T tp = 1./(2.*M_PI);
    x *= tp;
    x -= T(.25) + std::floor(x + T(.25));
    x *= T(16.) * (std::abs(x) - T(.5));
    #if EXTRA_PRECISION
    x += T(.225) * x * (std::abs(x) - T(1.));
    #endif
    return x;
}
*/
