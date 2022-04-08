#pragma once

#include <array>

#include <astray/math/ode/tableau/butcher_tableau.hpp>
#include <astray/math/ode/utility/sequence.hpp>

namespace ast
{
template <typename a, typename b, typename bs, typename c>
struct extended_butcher_tableau { };
template <typename type, type... a, type... b, type... bs_, type... c>
struct extended_butcher_tableau<sequence<type, a...>, sequence<type, b...>, sequence<type, bs_...>, sequence<type, c...>>
:      butcher_tableau         <sequence<type, a...>, sequence<type, b...>,                         sequence<type, c...>>
{
  static constexpr auto bs = std::array { bs_... };
};
}