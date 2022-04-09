#pragma once

#include <astray/core/metric.hpp>
#include <astray/core/termination_reason.hpp>
#include <astray/math/ode/ode.hpp>
#include <astray/math/linear_algebra.hpp>
#include <astray/math/ray.hpp>
#include <astray/parallel/thrust.hpp>

namespace ast
{
template <
  typename scalar_type          , 
  typename metric_type          ,
  typename tableau_type         = dormand_prince_5_tableau<scalar_type>,
  typename error_evaluator_type = proportional_integral_controller<tableau_type>>
class geodesic
{
  using ray_type      = ray<scalar_type>;
  using bounds_type   = thrust::optional<aabb4<scalar_type>>;
  
  using vector_type   = vector<scalar_type, 8>;
  using problem_type  = initial_value_problem <scalar_type, mapped<vector_type>, device_function<vector_type(scalar_type, const vector_type&)>>;
  using iterator_type = adaptive_step_iterator<explicit_method<tableau_type>, problem_type, error_evaluator_type>;

  thrust::optional<termination_reason> integrate(const std::size_t iterations, const scalar_type lambda_step_size, const scalar_type lambda = static_cast<scalar_type>(0))
  {
    iterator_type iterator(problem_type
    {
      lambda,
      mapped<vector_type>(ray.position.data()), // Valid as long as ray.position and ray.direction are contiguous in memory. 
      [&] (const scalar_type t, const vector_type& y)
      {
        vector_type dydt;
        dydt.head(4) = y.tail(4);
        auto christoffel_symbols = metric.compute_christoffel_symbols(y.head(4));
        for (auto i = 0; i < 4; ++i)
          for (auto j = 0; j < 4; ++j)
            for (auto k = 0; k < 4; ++k)
              dydt.tail(4)[k] -= christoffel_symbols(i, j, k) * y.tail(4)[i] * y.tail(4)[j];
        return dydt;
      }
    }, lambda_step_size, error_evaluator);

    for (std::size_t iteration = 0; iteration < iterations; ++iteration)
    {
      iterator++;      
    
      auto termination = metric.check_termination(ray.position, ray.direction);
      if (termination)
        return termination;
      if (bounds && !bounds.contains(ray.position))
        return termination_reason::out_of_bounds;
      if (ray.position.hasNaN() || ray.direction.hasNaN())
        return termination_reason::numeric_error;
    }
    
    return thrust::nullopt;
  }

  ray_type&            ray            ;
  const metric_type&   metric         ;
  error_evaluator_type error_evaluator;
  bounds_type          bounds         ;
};
}