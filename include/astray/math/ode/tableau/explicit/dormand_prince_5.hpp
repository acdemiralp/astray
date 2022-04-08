#pragma once

#include <astray/math/ode/tableau/extended_butcher_tableau.hpp>
#include <astray/math/ode/tableau/order.hpp>
#include <astray/math/ode/utility/sequence.hpp>

namespace ast
{
template <typename type = double>
using dormand_prince_5_tableau = extended_butcher_tableau<
  sequence<type,     1.0 /     5.0,
                     3.0 /    40.0,      9.0 /   40.0,
                    44.0 /    45.0, -   56.0 /   15.0,    32.0 /     9.0,
                 19372.0 /  6561.0, -25360.0 / 2187.0, 64448.0 /  6561.0, -212.0 / 729.0,
                  9017.0 /  3168.0, -  355.0 /   33.0, 46732.0 /  5247.0,   49.0 / 176.0, - 5103.0 /  18656.0,
                    35.0 /   384.0,               0.0,   500.0 /  1113.0,  125.0 / 192.0, - 2187.0 /   6784.0,  11.0 /   84.0>,
  sequence<type,    35.0 /   384.0,               0.0,   500.0 /  1113.0,  125.0 / 192.0, - 2187.0 /   6784.0,  11.0 /   84.0 , 0.0       >,
  sequence<type,  5179.0 / 57600.0,               0.0,  7571.0 / 16695.0,  393.0 / 640.0, -92097.0 / 339200.0, 187.0 / 2100.0 , 1.0 / 40.0>,
  sequence<type,               0.0,         1.0 / 5.0,     3.0 /    10.0,    4.0 /   5.0,      8.0 /      9.0,            1.0 , 1.0       >>;

template <typename type>
constexpr std::size_t order         <dormand_prince_5_tableau<type>> = 5;
template <typename type>
constexpr std::size_t extended_order<dormand_prince_5_tableau<type>> = 4;
}