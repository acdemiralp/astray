#pragma once

#include <type_traits>

#include <astray/parallel/thrust.hpp>

namespace ast
{
template <typename type>
__constant__ type     tableau_a ;
template <typename type>
__constant__ type     tableau_b ;
template <typename type>
__constant__ type     tableau_bs;
template <typename type>
__constant__ type     tableau_c ;

template <typename tableau_type>
constexpr std::size_t stages_v                      = 0;

template <typename tableau_type>
constexpr bool        is_extended_butcher_tableau_v = false;

template <typename tableau_type>
constexpr std::size_t order_v                       = 0;
template <typename tableau_type>
constexpr std::size_t extended_order_v              = 0;
}