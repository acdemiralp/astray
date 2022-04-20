#include <cstddef>
#include <cstdint>

#include <astray/api.hpp>

template <
  typename scalar_type ,
  typename metric_type = ast::metrics::kerr<scalar_type>, 
  typename motion_type = ast::geodesic<scalar_type, ast::runge_kutta_4_tableau<scalar_type>>>
struct settings_type
{
  using pixel_type           = ast::vector3<std::uint8_t>;
  using image_type           = ast::image<pixel_type>;
  using image_size_type      = image_type::size_type;
  using bounds_type          = typename motion_type::bounds_type;
  using error_evaluator_type = typename motion_type::error_evaluator_type;
  using transform_type       = ast::transform<scalar_type>;
  using vector_type          = typename transform_type::vector_type;
  using projection_type      = ast::projection<scalar_type>;
  
  image_size_type      image_size       = {3840, 2160};
  metric_type          metric           = {};
  std::size_t          iterations       = 40000;
  scalar_type          lambda_step_size = static_cast<scalar_type>(0.001);
  scalar_type          lambda           = static_cast<scalar_type>(0);
  bounds_type          bounds           = {};
  error_evaluator_type error_evaluator  = {};
  bool                 debug            = false;

  vector_type          position         = vector_type(8.0, 0.0, 0.0);
  vector_type          rotation         = vector_type(  0,   0,   0);
  bool                 look_at_origin   = true;
  scalar_type          coordinate_time  = static_cast<scalar_type>(0);
  projection_type      projection       = ast::perspective_projection<scalar_type> {ast::to_radians<scalar_type>(100), static_cast<scalar_type>(image_size[0]) / image_size[1]};
  image_type           background_image = image_type("../data/backgrounds/checkerboard_gray.png");
};

template <typename scalar_type, typename metric_type, typename motion_type>
constexpr auto make_ray_tracer(const settings_type<scalar_type, metric_type, motion_type>& settings)
{
  auto ray_tracer = std::make_unique<ast::ray_tracer<metric_type, motion_type>>(
    settings.image_size      ,
    settings.metric          ,
    settings.iterations      ,
    settings.lambda_step_size,
    settings.lambda          ,
    settings.bounds          ,
    settings.error_evaluator ,
    settings.debug           );
  ray_tracer->observer.transform.translation = settings.position;
  ray_tracer->observer.transform.rotation_from_euler(settings.rotation);
  if (settings.look_at_origin)
    ray_tracer->observer.transform.look_at(ast::transform<scalar_type>::vector_type::Zero());
  ray_tracer->observer.coordinate_time       = settings.coordinate_time;
  ray_tracer->observer.projection            = settings.projection;
  ray_tracer->background                     = settings.background_image;
  return std::move(ray_tracer);
}

std::int32_t main(std::int32_t argc, char** argv)
{
  ast::mpi::environment environment;

  using scalar_type = double;

  {
    const auto ray_tracer = make_ray_tracer(settings_type<scalar_type, ast::metrics::minkowski<scalar_type>>());
    const auto image      = ray_tracer->render_frame();
    if (ray_tracer->communicator.rank() == 0)
      image.save("../data/outputs/applications/teaser_minkowski.png");
  }
  {
    const auto ray_tracer = make_ray_tracer(settings_type<scalar_type, ast::metrics::schwarzschild<scalar_type>>());
    const auto image      = ray_tracer->render_frame();
    if (ray_tracer->communicator.rank() == 0)
      image.save("../data/outputs/applications/teaser_schwarzschild.png");
  }
  {
    const auto ray_tracer = make_ray_tracer(settings_type<scalar_type, ast::metrics::kerr<scalar_type>>());
    const auto image      = ray_tracer->render_frame();
    if (ray_tracer->communicator.rank() == 0)
      image.save("../data/outputs/applications/teaser_kerr.png");
  }
  {
    const auto ray_tracer = make_ray_tracer(settings_type<scalar_type, ast::metrics::reissner_nordstroem<scalar_type>>());
    const auto image      = ray_tracer->render_frame();
    if (ray_tracer->communicator.rank() == 0)
      image.save("../data/outputs/applications/teaser_reissner_nordstroem.png");
  }
  {
    settings_type<scalar_type, ast::metrics::morris_thorne<scalar_type>> settings;
    settings.position[0] = 1.0;

    const auto ray_tracer = make_ray_tracer(settings);
    const auto image      = ray_tracer->render_frame();
    if (ray_tracer->communicator.rank() == 0)
      image.save("../data/outputs/applications/teaser_morris_thorne.png");
  }
  {
    settings_type<scalar_type, ast::metrics::kastor_traschen<scalar_type>> settings;
    settings.position[0] = 5.0;

    const auto ray_tracer = make_ray_tracer(settings);
    const auto image      = ray_tracer->render_frame();
    if (ray_tracer->communicator.rank() == 0)
      image.save("../data/outputs/applications/teaser_kastor_traschen.png");
  }

  return 0;
}