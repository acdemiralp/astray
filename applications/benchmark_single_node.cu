#undef ASTRAY_USE_MPI

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
  auto session = ast::benchmark<scalar_type, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
  {
    ray_tracer->set_image_size({240, 135});
    recorder.record(metric_name + ",240,135"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({320, 180});
    recorder.record(metric_name + ",320,180"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({480, 270});
    recorder.record(metric_name + ",480,270"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({960, 540});
    recorder.record(metric_name + ",960,540"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({1920, 1080});
    recorder.record(metric_name + ",1920,1080", [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({3840, 2160});
    recorder.record(metric_name + ",3840,2160", [&]
    {
      image = &ray_tracer->render_frame();
    });
  }, repeats);

  image->save("../data/outputs/performance/benchmarks_single_" + device_name + "_" + metric_name + ".png");

  return session;
}

std::int32_t main(std::int32_t argc, char** argv)
{
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
  
  std::ofstream stream("../data/outputs/performance/benchmark_single_" + device_name + ".csv");
  stream << "metric,width,height,";
  for (auto i = 0; i < runs; ++i)
    stream << "run_" << i << ",";
  stream << "mean,variance,standard deviation\n";
  stream << run_benchmark(settings_type<scalar_type, ast::metrics::minkowski          <scalar_type>>(), runs, device_name, "minkowski"          ).to_string();
  stream << run_benchmark(settings_type<scalar_type, ast::metrics::schwarzschild      <scalar_type>>(), runs, device_name, "schwarzschild"      ).to_string();
  stream << run_benchmark(settings_type<scalar_type, ast::metrics::kerr               <scalar_type>>(), runs, device_name, "kerr"               ).to_string();
  stream << run_benchmark(settings_type<scalar_type, ast::metrics::reissner_nordstroem<scalar_type>>(), runs, device_name, "reissner_nordstroem").to_string();
  stream << run_benchmark(settings_type<scalar_type, ast::metrics::morris_thorne      <scalar_type>>(), runs, device_name, "morris_thorne"      ).to_string();
  stream << run_benchmark(settings_type<scalar_type, ast::metrics::kastor_traschen    <scalar_type>>(), runs, device_name, "kastor_traschen"    ).to_string();
  stream.close();

  return 0;
}