#pragma once

#include <algorithm>
#include <cmath>
#include <complex>

#include <astray/math/ode/algebra/quantity_operations.hpp>
#include <astray/math/ode/error/error_evaluation.hpp>
#include <astray/math/ode/error/extended_result.hpp>
#include <astray/math/ode/tableau/order.hpp>

namespace ast
{
template <typename method_type_, typename problem_type_>
struct integral_controller
{
  using method_type  = method_type_                       ;
  using problem_type = problem_type_                      ;
  using tableau_type = typename method_type ::tableau_type;
  using time_type    = typename problem_type::time_type   ;
  using value_type   = typename problem_type::value_type  ;
  using operations   = quantity_operations<value_type>    ;

  // Reference: https://doi.org/10.1007/978-3-540-78862-1 Chapter: II.4, Section: Automatic Step Size Control, Equations: 4.11, 4.12, 4.13
  error_evaluation<time_type> evaluate(const problem_type& problem, const time_type step_size, const extended_result<value_type>& result)
  {
    time_type squared_sum(0);
    operations::for_each([&] (const auto& p, const auto& r, const auto& e)
    {
      squared_sum += std::pow(std::abs(e) / (absolute_tolerance + relative_tolerance * std::max(std::abs(p), std::abs(r))), 2);
    }, problem.value, result.value, result.error);

    time_type error   = std::sqrt(std::real(squared_sum) / operations::size(problem.value));
    time_type optimal = factor * std::pow (time_type(1) / error, ceschino_exponent);
    time_type limited = std::min (factor_maximum, std::max(factor_minimum, optimal));

    return {error <= time_type(1), step_size * limited};
  }
  
  const time_type            absolute_tolerance = time_type(1e-6);
  const time_type            relative_tolerance = time_type(1e-3);
  const time_type            factor             = time_type(0.8 );
  const time_type            factor_minimum     = time_type(1e-2);
  const time_type            factor_maximum     = time_type(1e+2);

  static constexpr time_type ceschino_exponent  = time_type(1) / (std::min(order<tableau_type>, extended_order<tableau_type>) + time_type(1));
};
}