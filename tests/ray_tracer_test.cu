#include <doctest/doctest.h>

#include <astray/api.hpp>

TEST_CASE("ast::ray_tracer")
{
  using scalar_type     = float;
  using metric_type     = ast::metrics::schwarzschild<scalar_type>;
  using geodesic_type   = ast::geodesic<scalar_type, ast::runge_kutta_4_tableau<scalar_type>>;
  using ray_tracer_type = ast::ray_tracer<metric_type, geodesic_type>;

  auto ray_tracer = ray_tracer_type
  (
    {1920, 1080},
    {},
    100,
    0.01,
    0,
    {},
    {},
    true
  );

  ray_tracer.background.load("../data/backgrounds/checkerboard.png");
  ray_tracer.observer.transform.translation = {0.0f, 0.0f, -10.0f};
  ray_tracer.observer.transform.look_at({0.0f, 0.0f, 0.0f});
  ray_tracer.observer.projection = ast::perspective_projection<scalar_type> {ast::to_radians(60.0f), 1920.0f / 1080.0f};

  const auto image = ray_tracer.render_frame();
  if (ray_tracer.partitioner.communicator_rank() == 0)
    image.save("../data/outputs/sandbox_cpu.jpg");
}