#pragma once

#define _USE_MATH_DEFINES

#include <limits>
#include <math.h>

namespace ast
{
template <typename type>
struct constants
{
static constexpr type c    = static_cast<type>(1);
static constexpr type c_sq = static_cast<type>(c * c);

static constexpr type G    = static_cast<type>(1);
static constexpr type _2G  = static_cast<type>(2) * G;

static constexpr type pi   = static_cast<type>(M_PI);
static constexpr type _2pi = static_cast<type>(2) * pi;

static constexpr type eps  = std::numeric_limits<type>::epsilon();
};
}