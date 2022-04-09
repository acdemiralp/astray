#pragma once

#include <algorithm>
#include <cmath>
#include <complex>
#include <type_traits>

#include <astray/math/ode/algebra/quantity_operations.hpp>
#include <astray/math/ode/error/error_evaluation.hpp>
#include <astray/math/ode/error/extended_result.hpp>
#include <astray/math/ode/tableau/tableau_traits.hpp>

namespace ast
{
template <typename tableau_type>
struct integral_controller
{
  using value_type = typename tableau_type::value_type;

  // Reference: https://doi.org/10.1007/978-3-540-78862-1 Chapter: II.4, Section: Automatic Step Size Control, Equations: 4.11, 4.12, 4.13
  template <typename problem_type, typename extended_result_type>
  error_evaluation<value_type> evaluate(const problem_type& problem, const value_type step_size, const extended_result_type& result)
  {
    using operations = quantity_operations<std::remove_cv_t<std::remove_reference_t<typename problem_type::value_type>>>;

    value_type squared_sum(0);
    operations::for_each([&] (const auto& p, const auto& r, const auto& e)
    {
      squared_sum += std::pow(std::abs(e) / (absolute_tolerance + relative_tolerance * std::max(std::abs(p), std::abs(r))), 2);
    }, problem.value, result.value, result.error);

    value_type error   = std::sqrt(std::real(squared_sum) / operations::size(problem.value));
    value_type optimal = factor * std::pow (value_type(1) / error, ceschino_exponent);
    value_type limited = std::min (factor_maximum, std::max(factor_minimum, optimal));

    return {error <= value_type(1), step_size * limited};
  }
  
  const value_type            absolute_tolerance = value_type(1e-6);
  const value_type            relative_tolerance = value_type(1e-3);
  const value_type            factor             = value_type(0.8 );
  const value_type            factor_minimum     = value_type(1e-2);
  const value_type            factor_maximum     = value_type(1e+2);

  static constexpr value_type ceschino_exponent  = value_type(1) / (std::min(order_v<tableau_type>, extended_order_v<tableau_type>) + value_type(1));
};
}