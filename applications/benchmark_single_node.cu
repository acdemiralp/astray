#include <cstdint>

#include <astray/api.hpp>

#include "common.hpp"

using scalar_type = float;

template <typename scalar_type, typename metric_type, typename motion_type>
constexpr void run_benchmark  (const settings_type<scalar_type, metric_type, motion_type>& settings, std::size_t repeats, const std::string& file_postfix)
{
  using ray_tracer_type = ast::ray_tracer<metric_type, motion_type>;
  using image_type      = typename ray_tracer_type::image_type;

  std::unique_ptr<ray_tracer_type>  ray_tracer;
  const image_type*                 image     ;
  
  auto session = ast::benchmark<scalar_type, std::milli, std::chrono::high_resolution_clock>([&] (auto& recorder)
  {
    recorder.record("initialization"  , [&]
    {
      ray_tracer = make_ray_tracer(settings);
    });
    ray_tracer->set_image_size({240, 135});
    recorder.record("render_240x135"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({320, 180});
    recorder.record("render_320x180"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({480, 270});
    recorder.record("render_480x270"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({960, 540});
    recorder.record("render_960x540"  , [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({1920, 1080});
    recorder.record("render_1920x1080", [&]
    {
      image = &ray_tracer->render_frame();
    });
    ray_tracer->set_image_size({3840, 2160});
    recorder.record("render_3840x2160", [&]
    {
      image = &ray_tracer->render_frame();
    });
  }, repeats);
  
  std::string filepath("../data/outputs/performance/benchmark_single_");
  if      constexpr (ast::shared_device == ast::shared_device_type::cpp )
    filepath += "cpp" ;
  else if constexpr (ast::shared_device == ast::shared_device_type::cuda)
    filepath += "cuda";
  else if constexpr (ast::shared_device == ast::shared_device_type::omp )
    filepath += "omp" ;
  else if constexpr (ast::shared_device == ast::shared_device_type::tbb )
    filepath += "tbb" ;
  filepath += "_" + file_postfix;
  
  session. to_csv(filepath + ".csv");
  image  ->save  (filepath + ".png");
}

std::int32_t main(std::int32_t argc, char** argv)
{
  ast::mpi::environment environment;
  
  run_benchmark(settings_type<scalar_type, ast::metrics::minkowski          <scalar_type>>(), 10, "minkowski"          );
  run_benchmark(settings_type<scalar_type, ast::metrics::schwarzschild      <scalar_type>>(), 10, "schwarzschild"      );
  run_benchmark(settings_type<scalar_type, ast::metrics::kerr               <scalar_type>>(), 10, "kerr"               );
  run_benchmark(settings_type<scalar_type, ast::metrics::reissner_nordstroem<scalar_type>>(), 10, "reissner_nordstroem");
  run_benchmark(settings_type<scalar_type, ast::metrics::morris_thorne      <scalar_type>>(), 10, "morris_thorne"      );
  run_benchmark(settings_type<scalar_type, ast::metrics::kastor_traschen    <scalar_type>>(), 10, "kastor_traschen"    );

  return 0;
}