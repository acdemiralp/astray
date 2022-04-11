#include <cstdint>
#include <ctime>
#include <memory>

#include <astray/api.hpp>

using scalar_type     = float;
using pixel_type      = ast::vector3<std::uint8_t>;
using image_type      = ast::image<pixel_type>;
using image_size_type = image_type::size_type;

struct settings_type
{
  std::size_t     repeats   ;
  image_size_type image_size;
  std::size_t     iterations;
  scalar_type     step_size ;
};

template <
  typename metric_type           = ast::metrics::kerr                   <scalar_type>, 
  typename tableau_type          = ast::runge_kutta_4_tableau           <scalar_type>,
  typename error_controller_type = ast::proportional_integral_controller<scalar_type, tableau_type>>
void run_benchmark(const std::string& name, const settings_type& settings)
{
  using geodesic_type   = ast::geodesic<scalar_type, tableau_type, error_controller_type>;
  using ray_tracer_type = ast::ray_tracer<metric_type, geodesic_type>;

  std::unique_ptr<ray_tracer_type> ray_tracer;
  image_type                       image     ;

#ifdef ASTRAY_USE_MPI
  auto session = ast::benchmark_mpi<float, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
#else
  auto session = ast::benchmark    <float, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
#endif
  {
    recorder.record("Initialization", [&] ()
    {
      ray_tracer = std::make_unique<ray_tracer_type>(settings.image_size, metric_type(), settings.iterations, settings.step_size);
      ray_tracer->observer.projection = ast::perspective_projection<scalar_type> 
      {
        ast::to_radians(120.0f),
        static_cast<scalar_type>(settings.image_size[0]) / settings.image_size[1]
      };
      ray_tracer->observer.transform.translation = {0.1f, 0.1f, 5.0f};
      ray_tracer->observer.transform.look_at({0, 0, 0});
      ray_tracer->background        .load   ("../data/backgrounds/checkerboard.png");
    });
    recorder.record("Render"        , [&] ()
    {
      image = ray_tracer->render_frame();
    });
  }, settings.repeats);

  const std::string filepath("../data/outputs/performance/benchmark_" + name + "_" + std::to_string(std::time(nullptr)));
  session.to_csv(filepath + ".csv");
  if (ray_tracer->communicator.rank() == 0)
    image.save(filepath + ".png");
}

std::int32_t main(std::int32_t argc, char** argv)
{
  const settings_type   settings  {10, {1920, 1080}, 1000, 0.01f}; // TODO: Load from command line.
  ast::mpi::environment environment;
  run_benchmark<ast::metrics::minkowski      <scalar_type>>("minkowski"      , settings);
  run_benchmark<ast::metrics::goedel         <scalar_type>>("goedel"         , settings);
  run_benchmark<ast::metrics::schwarzschild  <scalar_type>>("schwarzschild"  , settings);
  run_benchmark<ast::metrics::kerr           <scalar_type>>("kerr"           , settings);
  run_benchmark<ast::metrics::kastor_traschen<scalar_type>>("kastor_traschen", settings);
  return 0;
}