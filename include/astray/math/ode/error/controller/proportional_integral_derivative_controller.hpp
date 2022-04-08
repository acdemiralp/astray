#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <cmath>
#include <complex>

#include <astray/math/ode/algebra/quantity_operations.hpp>
#include <astray/math/ode/error/error_evaluation.hpp>
#include <astray/math/ode/error/extended_result.hpp>
#include <astray/math/ode/tableau/tableau_traits.hpp>
#include <astray/parallel/thrust.hpp>

namespace ast
{
template <typename method_type_, typename problem_type_>
struct proportional_integral_derivative_controller
{
  using method_type  = method_type_                       ;
  using problem_type = problem_type_                      ;
  using tableau_type = typename method_type ::tableau_type;
  using time_type    = typename problem_type::time_type   ;
  using value_type   = typename problem_type::value_type  ;
  using operations   = quantity_operations<value_type>    ;

  // Reference: https://arxiv.org/pdf/2104.06836.pdf Section: 2.2 Error-Based Step Size Control, Equation: 2.6
  error_evaluation<time_type> evaluate(const problem_type& problem, const time_type step_size, const extended_result<value_type>& result)
  {
    time_type squared_sum(0);
    operations::for_each([&] (const auto& p, const auto& r, const auto& e)
    {
      squared_sum += std::pow(std::abs(e) / (absolute_tolerance + relative_tolerance * std::max(std::abs(p), std::abs(r))), 2);
    }, problem.value, result.value, result.error);

    error[0]          = time_type(1) / std::sqrt(std::real(squared_sum) / operations::size(problem.value));
    time_type optimal = std::pow(error[0], beta[0] / ceschino_exponent) * 
                        std::pow(error[1], beta[1] / ceschino_exponent) * 
                        std::pow(error[2], beta[2] / ceschino_exponent);
    time_type limited = limiter(optimal);

    const bool accept = limited >= accept_safety;
    if (accept)
      std::rotate(error.rbegin(), error.rbegin() + 1, error.rend());

    return {accept, step_size * limited};
  }
  
  const time_type                             absolute_tolerance = time_type(1e-6);
  const time_type                             relative_tolerance = time_type(1e-3);
  const time_type                             accept_safety      = time_type(0.81);
  const device_function<time_type(time_type)> limiter            = [ ] (time_type value) { return time_type(1) + std::atan(value - time_type(1)); };
  const std::array<time_type, 3>              beta               = { time_type(1)   , time_type(0)   , time_type(0)    };
  std::array<time_type, 3>                    error              = { time_type(1e-3), time_type(1e-3), time_type(1e-3) };
  
  static constexpr time_type                  ceschino_exponent  = time_type(1) / (std::min(order_v<tableau_type>, extended_order_v<tableau_type>) + time_type(1));
};
}