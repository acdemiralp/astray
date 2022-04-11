#pragma once

#include <astray/math/ode/tableau/tableau_traits.hpp>

namespace ast
{
template <typename type = double>
struct runge_kutta_4_tableau {};

template <typename type>
__constant__ std::array<type, 6> tableau_a<runge_kutta_4_tableau<type>> =
{
  type(0.5),
  type(0.0), type(0.5),
  type(0.0), type(0.0), type(1.0)
};
template <typename type>
__constant__ std::array<type, 4> tableau_b<runge_kutta_4_tableau<type>> = 
{
  type(1.0 / 6.0), type(1.0 / 3.0), type(1.0 / 3.0), type(1.0 / 6.0)
};
template <typename type>
__constant__ std::array<type, 4> tableau_c<runge_kutta_4_tableau<type>> = 
{
  type(0.0), type(0.5), type(0.5), type(1.0)
};

template <typename type>
constexpr std::size_t            stages_v <runge_kutta_4_tableau<type>> = 4;
template <typename type>
constexpr std::size_t            order_v  <runge_kutta_4_tableau<type>> = 4;
}