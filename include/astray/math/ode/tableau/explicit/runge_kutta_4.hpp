#pragma once

#include <astray/math/ode/tableau/tableau_traits.hpp>

namespace ast
{
template <typename type = double>
struct runge_kutta_4_tableau
{
  using value_type = type;

  static constexpr std::size_t         stages {4};
  static constexpr std::array<type, 6> a      {type(0.5      ),
                                               type(0.0      ), type(0.5      ),
                                               type(0.0      ), type(0.0      ), type(1.0      )                 };
  static constexpr std::array<type, 4> b      {type(1.0 / 6.0), type(1.0 / 3.0), type(1.0 / 3.0), type(1.0 / 6.0)};
  static constexpr std::array<type, 4> c      {type(0.0      ), type(0.5      ), type(0.5      ), type(1.0      )};
};

template <typename type>
constexpr std::size_t order_v<runge_kutta_4_tableau<type>> = 4;
}