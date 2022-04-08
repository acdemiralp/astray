#pragma once

#include <type_traits>

namespace ast
{
template <typename tableau_type>
constexpr bool        is_extended_butcher_tableau_v = false;

template <typename tableau_type>
constexpr std::size_t order_v                       = 0;
template <typename tableau_type>
constexpr std::size_t extended_order_v              = 0;
}