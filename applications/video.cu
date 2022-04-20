#include <cstdint>
#include <iostream>

#include <astray/api.hpp>

#include "common.hpp"

using scalar_type = float;

std::int32_t main(std::int32_t argc, char** argv)
{
  settings_type<scalar_type, ast::metrics::kerr<scalar_type>> settings;
  settings.position = {5, 0, -5};
  settings.background_image.load("../data/backgrounds/checkerboard_gray.png");

  const auto ray_tracer = make_ray_tracer(settings);

  std::optional<ast::video> video(std::nullopt);
  if (ray_tracer->communicator().rank() == 0)
    video.emplace("../data/outputs/applications/video.mp4", ray_tracer->image_size(), 60);
  
  constexpr auto frames(1000);
  for (auto i = 0; i < frames; ++i)
  {
    std::cout << i << "/" << frames - 1 << "\n";

    auto image = ray_tracer->render_frame();
    if (ray_tracer->communicator().rank() == 0)
      video->append(image);

    ray_tracer->observer().transform().translation[2] += 0.01f;
    ray_tracer->observer().transform().look_at({0, 0, 0});
  }

  return 0;
}