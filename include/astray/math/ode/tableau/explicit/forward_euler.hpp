#pragma once

#include <astray/math/ode/tableau/tableau_traits.hpp>

namespace ast
{
template <typename type = double>
struct forward_euler_tableau {};

template <typename type>
__constant__ std::array<type, 1> tableau_a<forward_euler_tableau<type>> = {type{0.0}};
template <typename type>
__constant__ std::array<type, 1> tableau_b<forward_euler_tableau<type>> = {type{1.0}};
template <typename type>
__constant__ std::array<type, 1> tableau_c<forward_euler_tableau<type>> = {type{0.0}};

template <typename type>
constexpr std::size_t            stages_v <forward_euler_tableau<type>> = 1;
template <typename type>
constexpr std::size_t            order_v  <forward_euler_tableau<type>> = 1;
}