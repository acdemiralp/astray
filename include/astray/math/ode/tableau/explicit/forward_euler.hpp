#pragma once

#include <astray/math/ode/tableau/tableau_traits.hpp>

namespace ast
{
template <typename type = double>
struct forward_euler_tableau
{
  static constexpr std::size_t         stages {1};
  static constexpr std::array<type, 1> a      {type{0.0}};
  static constexpr std::array<type, 1> b      {type{1.0}};
  static constexpr std::array<type, 1> c      {type{0.0}};
};

template <typename type>
constexpr std::size_t order_v<forward_euler_tableau<type>> = 1;
}