#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <astray/core/geodesic.hpp>
#include <astray/core/observer.hpp>
#include <astray/math/constants.hpp>
#include <astray/math/coordinate_system.hpp>
#include <astray/media/image.hpp>
#include <astray/parallel/mpi/mpi.hpp>
#include <astray/parallel/distributed_device.hpp>
#include <astray/parallel/partitioner.hpp>
#include <astray/parallel/shared_device.hpp>
#include <astray/parallel/thrust.hpp>

namespace ast
{
// Still has room for improvement.
template <typename metric_type, typename motion_type>
class ray_tracer
{
public:
  using scalar_type          = typename motion_type::scalar_type;
  using vector_type          = vector4<scalar_type>;

  using observer_type        = observer<scalar_type>;

  using pixel_type           = vector3<std::uint8_t>;
  using image_type           = image<pixel_type>;
  using image_size_type      = image_type::size_type;

  using bounds_type          = typename motion_type::bounds_type;
  using error_evaluator_type = typename motion_type::error_evaluator_type;

  using partitioner_type     = partitioner<2, std::int32_t, image_size_type, true>;

  struct device_data
  {
    vector_type          observer_position ;
    pixel_type*          background        ;
    image_size_type      background_size   ;

    metric_type          metric            ;
    std::size_t          iterations        ;
    scalar_type          lambda_step_size  ;
    scalar_type          lambda            ;
    bounds_type          bounds            ;
    //error_evaluator_type error_evaluator   ;
    
    pixel_type*          result            ;
    image_size_type      result_size       ;
    image_size_type      result_offset     ;

    bool                 debug             ;
  };

  explicit ray_tracer  (
    const image_size_type&      image_size       = {1920, 1080},
    const metric_type&          metric           = metric_type(),
    const std::size_t           iterations       = static_cast<std::size_t>(1e3),
    const scalar_type           lambda_step_size = static_cast<scalar_type>(1e-3),
    const scalar_type           lambda           = static_cast<scalar_type>(0),
    const bounds_type&          bounds           = thrust::nullopt,
    const error_evaluator_type& error_evaluator  = error_evaluator_type(),
    const bool                  debug            = false)
  // Integration parameters.
  : metric            (metric)
  , iterations        (iterations)
  , lambda_step_size  (lambda_step_size)
  , lambda            (lambda)
  , bounds            (bounds)
  , error_evaluator   (error_evaluator)
  // Parallelization.
  , partitioner       (communicator.rank(), communicator.size(), image_size)
#ifdef ASTRAY_USE_MPI
  , pixel_data_type   (mpi::data_type(MPI_UNSIGNED_CHAR), 3)
  , subarray_data_type(pixel_data_type   , partitioner.domain_size(), partitioner.block_size(), image_size_type::Zero().eval(), true)
  , resized_data_type (subarray_data_type, 0, partitioner.block_size()[0] * sizeof(pixel_type))
#endif
  , debug             (debug)
  {
    if constexpr (shared_device == shared_device_type::cuda)
      cudaDeviceSetLimit(cudaLimitMallocHeapSize, static_cast<std::size_t>(2e+8));
  }
  ray_tracer           (const ray_tracer&  that) = delete ;
  ray_tracer           (      ray_tracer&& temp) = default;
 ~ray_tracer           ()                        = default;
  ray_tracer& operator=(const ray_tracer&  that) = delete ;
  ray_tracer& operator=(      ray_tracer&& temp) = default;

  image<vector3<std::uint8_t>> render_frame()
  {
    using constants = constants<scalar_type>;

    image_type                         result           (partitioner.block_size());
    thrust::device_vector<pixel_type>  device_background(background.data);
    thrust::device_vector<pixel_type>  device_result    (result    .data);
    thrust::device_vector<device_data> data             (std::vector<device_data>
    {{
      vector_type(observer.coordinate_time, observer.transform.translation[0], observer.transform.translation[1], observer.transform.translation[2]),
      device_background.data().get(),
      background.size               ,

      metric                        ,
      iterations                    ,
      lambda_step_size              ,
      lambda                        ,
      bounds                        ,
      //error_evaluator               ,

      device_result.data().get()    ,
      result.size                   ,
      partitioner.rank_offset()     ,

      debug
    }});

    auto rays = observer.generate_rays(partitioner.domain_size(), partitioner.block_size(), partitioner.rank_offset());
    thrust::for_each(
      thrust::make_zip_iterator(make_tuple(thrust::counting_iterator<std::size_t>(0)          , rays.begin())),
      thrust::make_zip_iterator(make_tuple(thrust::counting_iterator<std::size_t>(rays.size()), rays.end  ())),
      [data = data.data().get()] __device__ (const auto& iteratee)
      {
        auto  index = thrust::get<0>(iteratee);
        auto& ray   = thrust::get<1>(iteratee);
        auto  metric(data->metric); // A copy is necessary for correct creation of virtual function table.

        if constexpr (metric_type::coordinate_system() == coordinate_system::boyer_lindquist || metric_type::coordinate_system() == coordinate_system::prolate_spheroidal)
          convert_ray<coordinate_system::cartesian, metric_type::coordinate_system()>(ray, metric.coordinate_system_parameter());
        else
          convert_ray<coordinate_system::cartesian, metric_type::coordinate_system()>(ray);
      
        const auto termination = motion_type::integrate(ray, metric, data->iterations, data->lambda_step_size, data->lambda, data->bounds); //, data->error_evaluator);
        
        if (termination == thrust::nullopt || termination == termination_reason::out_of_bounds)
        {
          if constexpr (metric_type::coordinate_system() == coordinate_system::boyer_lindquist || metric_type::coordinate_system() == coordinate_system::prolate_spheroidal)
            convert<metric_type::coordinate_system(), coordinate_system::cartesian>(ray.position, metric.coordinate_system_parameter());
          else
            convert<metric_type::coordinate_system(), coordinate_system::cartesian>(ray.position);

          ray.position -= data->observer_position; // Environment map is relative to observer.

          convert<coordinate_system::cartesian, coordinate_system::spherical>(ray.position);
          
          const image_size_type background_index(
            std::floor(ray.position[3] / constants::_2pi * static_cast<scalar_type>(data->background_size[0] - 1)),
            std::floor(ray.position[2] / constants::pi   * static_cast<scalar_type>(data->background_size[1] - 1)));
          
          //data->result[index] = data->background[ravel_multi_index<image_size_type, true>(background_index, data->background_size)];
        }
        
        if (data->debug)
        {
          if      (termination == termination_reason::constraint_violation)
            data->result[index] = pixel_type(255, 128, 128);
          else if (termination == termination_reason::numeric_error       )
            data->result[index] = pixel_type(128, 255, 128);
          else if (termination == termination_reason::spacetime_breakdown )
            data->result[index] = pixel_type(128, 128, 255);
        }
      });

    thrust::copy(device_result.begin(), device_result.end(), result.data.begin());
    if constexpr (shared_device == shared_device_type::cuda)
      cudaDeviceSynchronize();

    if constexpr (distributed_device == distributed_device_type::mpi)
    {
      image_type                gathered_result(partitioner.domain_size());
      std::vector<std::int32_t> counts         (communicator.size(), 1);
      std::vector<std::int32_t> displacements  (communicator.size());
      for (auto y = 0; y < partitioner.grid_size()[1]; ++y)
        for (auto x = 0; x < partitioner.grid_size()[0]; ++x)
          displacements[x + y * partitioner.grid_size()[0]] = x + y * (partitioner.block_size()[1] * partitioner.grid_size()[0]);

      communicator.gatherv(
        result         .data.data(), static_cast<std::int32_t>(result.data.size()), pixel_data_type  ,
        gathered_result.data.data(), counts.data(), displacements.data()          , resized_data_type);

      if (partitioner.communicator_rank() != 0)
        return result; // Workers return their partial results.
      return gathered_result;
    }
    else
      return result;
  }

  // Visualization.
  observer_type        observer          ;
  image_type           background        ;
  
  // Integration.
  metric_type          metric            ;
  std::size_t          iterations        ;
  scalar_type          lambda_step_size  ;
  scalar_type          lambda            ;
  bounds_type          bounds            ;
  error_evaluator_type error_evaluator   ;

  // Parallelization.
  mpi::environment     environment       ;
  mpi::communicator    communicator      ;
  partitioner_type     partitioner       ;
  mpi::data_type       pixel_data_type   ;
  mpi::data_type       subarray_data_type;
  mpi::data_type       resized_data_type ;

  // Debugging.
  bool                 debug             ;
};
}