#include <cstdint>

#include <astray/api.hpp>

#include "common.hpp"

using scalar_type = float;

std::int32_t main(std::int32_t argc, char** argv)
{
  settings_type<scalar_type, ast::metrics::schwarzschild<scalar_type>> settings;
  settings.image_size = {960, 540};
  settings.debug      = true;
  settings.background_image.load("../data/backgrounds/checkerboard_gray.png");

  const auto ray_tracer = make_ray_tracer(settings);
  const auto image      = ray_tracer->render_frame();
  if (ray_tracer->communicator().rank() == 0)
    image.save("../data/outputs/applications/sandbox.png");
  return 0;
}