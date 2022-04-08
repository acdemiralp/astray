#pragma once

#define _USE_MATH_DEFINES

#include <limits>
#include <math.h>

namespace ast
{
template <typename type>
struct constants
{
static constexpr type c     = static_cast<type>(1);
static constexpr type G     = static_cast<type>(1);
static constexpr type pi    = static_cast<type>(M_PI);
static constexpr type _2_pi = static_cast<type>(M_PI * 2.0);
static constexpr type eps   = std::numeric_limits<type>::epsilon();
};
}