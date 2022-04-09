#pragma once

#include <astray/core/geodesic.hpp>
#include <astray/core/observer.hpp>
#include <astray/math/constants.hpp>
#include <astray/math/coordinate_system.hpp>
#include <astray/media/image.hpp>
#include <astray/parallel/mpi/mpi.hpp>
#include <astray/parallel/partitioner.hpp>
#include <astray/parallel/shared_device.hpp>
#include <astray/parallel/thrust.hpp>

namespace ast
{
template <typename metric_type, typename motion_type>
class ray_tracer
{
public:
  using scalar_type      = typename metric_type::scalar_type;
  
  using pixel_type       = vector3<std::uint8_t>;
  using image_type       = image<pixel_type>;
  using observer_type    = observer<scalar_type>;
  using partitioner_type = partitioner<2, std::int32_t, vector2<std::int32_t>, true>;

  ray_tracer           ()
  {
    if constexpr (ast::shared_device == ast::shared_device_type::cuda)
      cudaDeviceSetLimit(cudaLimitMallocHeapSize, static_cast<std::size_t>(2e+8));
  }
  ray_tracer           (const ray_tracer&  that) = delete ;
  ray_tracer           (      ray_tracer&& temp) = default;
 ~ray_tracer           ()
  {
    
  }
  ray_tracer& operator=(const ray_tracer&  that) = delete ;
  ray_tracer& operator=(      ray_tracer&& temp) = default;

  image<vector3<std::uint8_t>>       render_frame()
  {
    image_type frame(partitioner_.block_size());



    return     frame;
  }

  constexpr       metric_type&       metric      ()
  {
    return metric_;
  }
  constexpr const metric_type&       metric      () const
  {
    return metric_;
  }
  constexpr       observer_type&     observer    ()
  {
    return observer_;
  }
  constexpr const observer_type&     observer    () const
  {
    return observer_;
  }
  
  constexpr       mpi::environment&  environment ()
  {
    return environment_;
  }
  constexpr const mpi::environment&  environment () const
  {
    return environment_;
  }
  constexpr       mpi::communicator& communicator()
  {
    return communicator_;
  }
  constexpr const mpi::communicator& communicator() const
  {
    return communicator_;
  }
  constexpr       partitioner_type&  partitioner ()
  {
    return partitioner_;
  }
  constexpr const partitioner_type&  partitioner () const
  {
    return partitioner_;
  }

protected:
  metric_type       metric_         ;
  observer_type     observer_       ;
  image_type        background_     ;

  mpi::environment  environment_    ;
  mpi::communicator communicator_   ;
  partitioner_type  partitioner_    ;
};
}