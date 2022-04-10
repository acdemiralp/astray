#pragma once

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
  using scalar_type          = typename metric_type::scalar_type;
  using observer_type        = observer<scalar_type>;
  using pixel_type           = vector3<std::uint8_t>;
  using image_type           = image<pixel_type>;
  using image_size_type      = typename image_type::size_type;
  using bounds_type          = typename motion_type::bounds_type;
  using error_evaluator_type = typename motion_type::error_evaluator_type;
  using partitioner_type     = partitioner<2, std::int32_t, image_size_type, true>;

  ray_tracer           (
    const image_size_type&      image_size       = {1920, 1080},
    const metric_type&          metric           = metric_type(),
    const std::size_t           iterations       = static_cast<scalar_type>(1e3),
    const scalar_type           lambda_step_size = static_cast<scalar_type>(1e-3),
    const scalar_type           lambda           = static_cast<scalar_type>(0),
    const bounds_type&          bounds           = thrust::nullopt,
    const error_evaluator_type& error_evaluator  = error_evaluator_type())
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
  , subarray_data_type(pixel_data_type   , partitioner.domain_size(), partitioner.block_size(), image_size_type::Zero(), true)
  , resized_data_type (subarray_data_type, 0, partitioner.block_size()[0] * sizeof(pixel_type))
#endif
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
    image_type frame(partitioner.block_size());

    // TODO

    if constexpr (distributed_device == distributed_device_type::mpi)
    {
      image_type                gathered_frame(partitioner.domain_size());
      std::vector<std::int32_t> counts        (communicator.size(), 1);
      std::vector<std::int32_t> displacements (communicator.size());
      for (auto y = 0; y < partitioner.grid_size()[1]; ++y)
        for (auto x = 0; x < partitioner.grid_size()[0]; ++x)
          displacements[x + y * partitioner.grid_size()[0]] = x + y * (partitioner.block_size()[1] * partitioner.grid_size()[0]);

      communicator.gatherv(
        frame         .data.data(), static_cast<std::int32_t>(frame.data.size()), pixel_data_type  ,
        gathered_frame.data.data(), counts.data(), displacements.data()         , resized_data_type);

      if (partitioner.communicator_rank() != 0)
        return frame; // Workers return their partial results.
      return gathered_frame;
    }
    else
      return frame;
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
};
}