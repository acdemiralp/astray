#pragma once

#define _USE_MATH_DEFINES

#include <limits>
#include <math.h>

namespace ast::constants
{
template <typename type>
constexpr type speed_of_light         = static_cast<type>(1);
template <typename type>
constexpr type gravitational_constant = static_cast<type>(1);

template <typename type>
constexpr type pi                     = static_cast<type>(M_PI);
template <typename type>
constexpr type two_pi                 = static_cast<type>(M_PI * 2.0);
template <typename type>
constexpr type epsilon                = std::numeric_limits<type>::epsilon();
}