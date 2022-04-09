#pragma once

#include <astray/core/geodesic.hpp>
#include <astray/core/observer.hpp>
#include <astray/math/constants.hpp>
#include <astray/math/coordinate_system.hpp>
#include <astray/media/image.hpp>
#include <astray/parallel/partitioner.hpp>
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
  using partitioner_type = partitioner<2, std::int32_t, vector2<std::int32_t>, true>;
  using observer_type    = observer<scalar_type>;


  image<vector3<std::uint8_t>> render_frame()
  {
    image_type frame(partitioner_.block_size());



    return     frame;
  }

protected:
  metric_type      metric_         ;
  partitioner_type partitioner_    ;
  observer_type    observer_       ;

  image_type       environment_map_;
};
}