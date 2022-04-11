#include <cstdint>

#include <astray/api.hpp>

using scalar_type     = double;
using metric_type     = ast::metrics::schwarzschild<scalar_type>;
using geodesic_type   = ast::geodesic              <scalar_type, ast::runge_kutta_4_tableau<scalar_type>>;
using ray_tracer_type = ast::ray_tracer            <metric_type, geodesic_type>;

std::int32_t main(std::int32_t argc, char** argv)
{
  auto image_size = ast::vector2<std::int32_t>(160, 120);
  
  auto ray_tracer                           = ray_tracer_type(image_size, metric_type(), 100, 0.1);
  ray_tracer.observer.projection            = ast::perspective_projection<scalar_type> {ast::to_radians(120.0), static_cast<scalar_type>(image_size[0]) / image_size[1]};
  ray_tracer.observer.transform.translation = {-5.0, -5.0, 0.1};
  ray_tracer.observer.transform.look_at({0.0, 0.0, 0.0});
  ray_tracer.background        .load   ("../data/backgrounds/tycho_2_t5_wright_bridgman.jpg");

  std::optional<ast::video> video(std::nullopt);
  if (ray_tracer.communicator.rank() == 0)
    video.emplace("../data/outputs/applications/video.mp4", image_size, 60);
  
  constexpr auto frames(100);
  for (auto i = 0; i < frames; ++i)
  {
    std::cout << i << "/" << frames - 1 << "\n";

    auto image = ray_tracer.render_frame();
    if (ray_tracer.communicator.rank() == 0)
      video->append(image);

    ray_tracer.observer.transform.translation[0] += 0.1;
    ray_tracer.observer.transform.look_at({0.0, 0.0, 0.0});
  }

  return 0;
}