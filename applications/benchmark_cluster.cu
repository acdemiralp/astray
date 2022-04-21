#include <cstdint>

#include <astray/api.hpp>

#include "common.hpp"

using scalar_type = float;

template <typename scalar_type, typename metric_type, typename motion_type>
constexpr auto run_benchmark  (
  const settings_type<scalar_type, metric_type, motion_type>& settings   , 
  std::size_t                                                 repeats    , 
  const std::string&                                          device_name,
  const std::string&                                          metric_name)
{
  using ray_tracer_type = ast::ray_tracer<metric_type, motion_type>;
  using image_type      = typename ray_tracer_type::image_type;

  auto              ray_tracer = make_ray_tracer(settings);
  const image_type* image;
  
#ifdef ASTRAY_USE_MPI
  auto session = ast::benchmark_mpi<float, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
#else
  auto session = ast::benchmark    <float, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
#endif
  {
    recorder.record("Render"        , [&] ()
    {
      image = ray_tracer->render_frame();
    });
  }, repeats);

  const std::string filepath("../data/outputs/performance/benchmark_" + metric_name + "_" + std::to_string(std::time(nullptr)));
  session.to_csv(filepath + ".csv");
  if (ray_tracer->communicator().rank() == 0)
    image.save(filepath + ".png");

  return session;
}

std::int32_t main(std::int32_t argc, char** argv)
{
  ast::mpi::environment  environment ;
  ast::mpi::communicator communicator;
  
  constexpr auto runs = 10;
  
  std::string device_name;
  if      constexpr (ast::shared_device == ast::shared_device_type::cpp )
    device_name = "cpp" ;
  else if constexpr (ast::shared_device == ast::shared_device_type::cuda)
    device_name = "cuda";
  else if constexpr (ast::shared_device == ast::shared_device_type::omp )
    device_name = "omp" ;
  else if constexpr (ast::shared_device == ast::shared_device_type::tbb )
    device_name = "tbb" ;
  
  auto benchmark1 = run_benchmark(settings_type<scalar_type, ast::metrics::minkowski          <scalar_type>>(), runs, device_name, "minkowski"          );
  auto benchmark2 = run_benchmark(settings_type<scalar_type, ast::metrics::schwarzschild      <scalar_type>>(), runs, device_name, "schwarzschild"      );
  auto benchmark3 = run_benchmark(settings_type<scalar_type, ast::metrics::kerr               <scalar_type>>(), runs, device_name, "kerr"               );
  auto benchmark4 = run_benchmark(settings_type<scalar_type, ast::metrics::reissner_nordstroem<scalar_type>>(), runs, device_name, "reissner_nordstroem");
  auto benchmark5 = run_benchmark(settings_type<scalar_type, ast::metrics::morris_thorne      <scalar_type>>(), runs, device_name, "morris_thorne"      );
  auto benchmark6 = run_benchmark(settings_type<scalar_type, ast::metrics::kastor_traschen    <scalar_type>>(), runs, device_name, "kastor_traschen"    );

  if (communicator.rank() == 0)
  {
    std::ofstream stream("../data/outputs/performance/benchmark_cluster_" + device_name + "_" + std::to_string(communicator.size()) + ".csv");
    stream << "rank,metric,width,height,";
    for (auto i = 0; i < runs; ++i)
      stream << "run_" << i << ",";
    stream << "mean,variance,standard deviation\n";
    stream << benchmark1.to_string();
    stream << benchmark2.to_string();
    stream << benchmark3.to_string();
    stream << benchmark4.to_string();
    stream << benchmark5.to_string();
    stream << benchmark6.to_string();
  }

  return 0;
}