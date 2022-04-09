#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <cmath>
#include <complex>
#include <type_traits>

#include <astray/math/ode/algebra/quantity_operations.hpp>
#include <astray/math/ode/error/error_evaluation.hpp>
#include <astray/math/ode/error/extended_result.hpp>
#include <astray/math/ode/tableau/tableau_traits.hpp>
#include <astray/parallel/thrust.hpp>

namespace ast
{
template <typename tableau_type>
struct proportional_integral_derivative_controller
{
  using value_type = typename tableau_type::value_type;

  // Reference: https://arxiv.org/pdf/2104.06836.pdf Section: 2.2 Error-Based Step Size Control, Equation: 2.6
  template <typename problem_type, typename extended_result_type>
  error_evaluation<value_type> evaluate(const problem_type& problem, const value_type step_size, const extended_result_type& result)
  {
    using operations = quantity_operations<std::remove_cv_t<std::remove_reference_t<typename problem_type::value_type>>>;

    value_type squared_sum(0);
    operations::for_each([&] (const auto& p, const auto& r, const auto& e)
    {
      squared_sum += std::pow(std::abs(e) / (absolute_tolerance + relative_tolerance * std::max(std::abs(p), std::abs(r))), 2);
    }, problem.value, result.value, result.error);

    error[0]           = value_type(1) / std::sqrt(std::real(squared_sum) / operations::size(problem.value));
    value_type optimal = std::pow(error[0], beta[0] / ceschino_exponent) * 
                         std::pow(error[1], beta[1] / ceschino_exponent) * 
                         std::pow(error[2], beta[2] / ceschino_exponent);
    value_type limited = limiter(optimal);

    const bool accept  = limited >= accept_safety;
    if (accept)
      std::rotate(error.rbegin(), error.rbegin() + 1, error.rend());

    return {accept, step_size * limited};
  }
  
  const value_type                              absolute_tolerance = value_type(1e-6);
  const value_type                              relative_tolerance = value_type(1e-3);
  const value_type                              accept_safety      = value_type(0.81);
  const device_function<value_type(value_type)> limiter            = [ ] (value_type value) { return value_type(1) + std::atan(value - value_type(1)); };
  const std::array<value_type, 3>               beta               = { value_type(1)   , value_type(0)   , value_type(0)    };
  std::array<value_type, 3>                     error              = { value_type(1e-3), value_type(1e-3), value_type(1e-3) };
  
  static constexpr value_type                   ceschino_exponent  = value_type(1) / (std::min(order_v<tableau_type>, extended_order_v<tableau_type>) + value_type(1));
};
}