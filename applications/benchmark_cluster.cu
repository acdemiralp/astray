#include <cstdint>

#include <astray/api.hpp>

#include "common.hpp"

using scalar_type = float;

template <typename scalar_type, typename metric_type, typename motion_type>
constexpr void run_benchmark  (const settings_type<scalar_type, metric_type, motion_type>& settings, std::size_t repeats, const std::string& metric_name)
{
  std::unique_ptr<ast::ray_tracer<metric_type, motion_type>> ray_tracer;
  ast::image<ast::vector3<std::uint8_t>>                     image     ;
  
#ifdef ASTRAY_USE_MPI
  auto session = ast::benchmark_mpi<float, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
#else
  auto session = ast::benchmark    <float, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
#endif
  {
    recorder.record("Initialization", [&] ()
    {
      ray_tracer = make_ray_tracer(settings);
    });
    recorder.record("Render"        , [&] ()
    {
      image = ray_tracer->render_frame();
    });
  }, repeats);

  const std::string filepath("../data/outputs/performance/benchmark_" + metric_name + "_" + std::to_string(std::time(nullptr)));
  session.to_csv(filepath + ".csv");
  if (ray_tracer->communicator().rank() == 0)
    image.save(filepath + ".png");
}

// Usage mpiexec -n [NUMBER_OF_NODES] ./benchmark [METRIC_NAME] [IMAGE_SIZE_X] [IMAGE_SIZE_Y]
std::int32_t main(std::int32_t argc, char** argv)
{
  ast::mpi::environment environment;


  std::cout << "Argument count " << argc;
  for (auto i = 0; i < argc; ++i)
    std::cout << "Argument " << i << ": " << argv[i] << "\n";
  
  int x;
  std::cin >> x;

  //run_benchmark(settings_type<scalar_type, ast::metrics::minkowski          <scalar_type>>(), 10, "minkowski"          );
  //run_benchmark(settings_type<scalar_type, ast::metrics::schwarzschild      <scalar_type>>(), 10, "schwarzschild"      );
  //run_benchmark(settings_type<scalar_type, ast::metrics::kerr               <scalar_type>>(), 10, "kerr"               );
  //run_benchmark(settings_type<scalar_type, ast::metrics::reissner_nordstroem<scalar_type>>(), 10, "reissner_nordstroem");
  //run_benchmark(settings_type<scalar_type, ast::metrics::morris_thorne      <scalar_type>>(), 10, "morris_thorne"      );
  //run_benchmark(settings_type<scalar_type, ast::metrics::kastor_traschen    <scalar_type>>(), 10, "kastor_traschen"    );

  return 0;
}