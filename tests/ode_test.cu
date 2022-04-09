#include <doctest/doctest.h>

#include <astray/math/ode/ode.hpp>

#include <astray/metrics/cartesian/kastor_traschen.hpp>
#include <astray/metrics/cartesian/minkowski.hpp>
#include <astray/metrics/cartesian/goedel.hpp>
#include <astray/metrics/spherical/schwarzschild.hpp>
#include <astray/metrics/boyer_lindquist/kerr.hpp>
#include <astray/core/observer.hpp>
#include <astray/core/geodesic.hpp>
#include <astray/core/ray_tracer.hpp>

TEST_CASE("ast::ode")
{
  using method_type  = ast::explicit_method<ast::runge_kutta_4_tableau<float>>;
  using problem_type = ast::initial_value_problem<float, ast::vector3<float>>;
  
  const auto sigma   = 10.0f;
  const auto rho     = 28.0f;
  const auto beta    = 8.0f / 3.0f;
  
  const auto problem = problem_type
  {
    0.0f,                                             /* t0 */
    ast::vector3<float>(16.0f, 16.0f, 16.0f),         /* y0 */
    [&] (const float t, const ast::vector3<float>& y) /* dy/dt = f(t, y) */
    {
      return ast::vector3<float>(sigma * (y[1] - y[0]), y[0] * (rho - y[2]) - y[1], y[0] * y[1] - beta * y[2]);
    }
  };
    
  auto iterator = ast::fixed_step_iterator<method_type, problem_type> {problem, 1.0f};
  for (auto i = 0; i < 1000; ++i)
    ++iterator;
}