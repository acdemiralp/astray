#pragma once

#include <array>
#include <type_traits>
#include <utility>

#include <astray/math/ode/error/extended_result.hpp>
#include <astray/math/ode/tableau/tableau_traits.hpp>
#include <astray/math/ode/utility/constexpr_for.hpp>
#include <astray/math/ode/utility/triangular_number.hpp>

namespace ast
{
template <typename tableau_type_>
class explicit_method
{
public:
  using tableau_type = tableau_type_;

  template <typename problem_type>
  static constexpr auto apply(const problem_type& problem, const typename problem_type::time_type step_size)
  {
    using value_type = std::remove_cv_t<std::remove_reference_t<typename problem_type::value_type>>;

    std::array<value_type, tableau_type::stages> stages;
    constexpr_for<0, tableau_type::stages, 1>([&problem, &step_size, &stages] (auto i)
    {
      value_type sum;
      constexpr_for<0, i.value, 1>([&stages, &sum, &i] (auto j)
      {
        sum += std::get<j>(stages) * std::get<triangular_number<i.value - 1> + j.value>(tableau_type::a);
      });
      std::get<i>(stages) = problem.function(problem.time + std::get<i>(tableau_type::c) * step_size, problem.value + sum * step_size);
    });

    if constexpr (is_extended_butcher_tableau_v<tableau_type>)
    {
      value_type higher, lower;
      constexpr_for<0, tableau_type::stages, 1>([&stages, &higher, &lower] (auto i)
      {
        higher += std::get<i>(stages) * std::get<i>(tableau_type::b );
        lower  += std::get<i>(stages) * std::get<i>(tableau_type::bs);
      });
      return extended_result<value_type> {problem.value + higher * step_size, (higher - lower) * step_size};
    }
    else
    {
      value_type sum;
      constexpr_for<0, tableau_type::stages, 1>([&stages, &sum] (auto i)
      {
        sum += std::get<i>(stages) * std::get<i>(tableau_type::b);
      });
      return value_type(problem.value + sum * step_size);
    }
  }
};
}