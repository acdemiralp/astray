#include <cstdint>
#include <string>

#include <astray/api.hpp>

using scalar_type     = double;
using vector_type     = ast::vector3<scalar_type>;
using pixel_type      = ast::vector3<std::uint8_t>;
using image_type      = ast::image<pixel_type>;
using image_size_type = image_type::size_type;
using geodesic_type   = ast::geodesic<scalar_type, ast::runge_kutta_4_tableau<scalar_type>>;

struct teaser_settings
{
  image_size_type image_size;
  std::size_t     iterations;
  scalar_type     step_size ;
  image_type      background;
  vector_type     position  ;
  scalar_type     fov       ;
};

template <typename metric_type>
void make_teaser(const std::string& name, const teaser_settings& settings)
{
  using ray_tracer_type = ast::ray_tracer<metric_type, geodesic_type>;

  auto ray_tracer                           = ray_tracer_type(settings.image_size, metric_type(), settings.iterations, settings.step_size, 0.0, {}, {}, true);
  ray_tracer.background                     = settings.background;
  ray_tracer.observer.projection            = ast::perspective_projection<scalar_type> {settings.fov, static_cast<scalar_type>(settings.image_size[0]) / settings.image_size[1]};
  ray_tracer.observer.transform.translation = settings.position;
  ray_tracer.observer.transform.look_at(vector_type::Zero());

  const auto image = ray_tracer.render_frame();
  if (ray_tracer.communicator.rank() == 0)
    image.save("../data/outputs/teaser_" + name + ".png");
}

std::int32_t main(std::int32_t argc, char** argv)
{
  const auto settings = teaser_settings
  {
    {320, 240},
    3000,
    0.01,
    image_type("../data/backgrounds/tycho_2_t5_wright_bridgman.jpg"),
    {1.0, 10.0, 0.01},
    ast::to_radians(75.0)
  };

  MPI_Init(nullptr, nullptr);
  make_teaser<ast::metrics::minkowski      <scalar_type>>("minkowski"      , settings);
  make_teaser<ast::metrics::goedel         <scalar_type>>("goedel"         , settings);
  make_teaser<ast::metrics::schwarzschild  <scalar_type>>("schwarzschild"  , settings);
  make_teaser<ast::metrics::kerr           <scalar_type>>("kerr"           , settings);
  make_teaser<ast::metrics::kastor_traschen<scalar_type>>("kastor_traschen", settings);
  MPI_Finalize();
  
  return 0;
}