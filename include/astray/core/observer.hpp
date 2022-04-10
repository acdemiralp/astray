#pragma once

#include <astray/math/indexing.hpp>
#include <astray/math/projection.hpp>
#include <astray/math/ray.hpp>
#include <astray/math/transform.hpp>
#include <astray/parallel/thrust.hpp>

namespace ast
{
// Still has room for improvement.
template <typename scalar_type>
class observer
{
public:
  using projection_type = projection<scalar_type>;
  using transform_type  = transform <scalar_type>;
  using ray_type        = ray       <vector4<scalar_type>>;
  
  template <typename vector_type, typename image_size_type>
  struct device_data_perspective
  {
    vector_type     position         ;
    vector_type     direction_00     ;
    vector_type     u                ;
    vector_type     v                ;
    image_size_type global_size      ;
    image_size_type local_size       ;
    image_size_type local_offset     ;
    scalar_type     coordinate_time  ;
  };
  template <typename vector_type, typename image_size_type>
  struct device_data_orthographic
  {
    vector_type     direction        ;
    vector_type     position_00      ;
    vector_type     u                ;
    vector_type     v                ;
    image_size_type global_size      ;
    image_size_type local_size       ;
    image_size_type local_offset     ;
    scalar_type     coordinate_time  ;
  };
    

  template <typename image_size_type>
  thrust::device_vector<ray_type> generate_rays(
    const image_size_type& global_size ,
    const image_size_type& local_size  ,
    const image_size_type& local_offset)
  {
    if (std::holds_alternative<perspective_projection<scalar_type>>(projection))
      return generate_rays_perspective (global_size, local_size, local_offset);
    else
      return generate_rays_orthographic(global_size, local_size, local_offset);
  }
  
  template <typename image_size_type>
  thrust::device_vector<ray_type> generate_rays_perspective(
    const image_size_type& global_size ,
    const image_size_type& local_size  ,
    const image_size_type& local_offset)
  {
    using vector_type = typename transform_type::vector_type;

    const auto&       cast_projection = std::get<perspective_projection<scalar_type>>(projection);
    const scalar_type v_size          = static_cast<scalar_type>(2) * tan(static_cast<scalar_type>(0.5) * cast_projection.fov_y);
    const scalar_type u_size          = v_size * cast_projection.aspect_ratio;

    const vector_type u               = transform.right  () * u_size;
    const vector_type v               = transform.up     () * v_size;
    const vector_type w               = transform.forward() * cast_projection.focal_length;
    const vector_type direction_00    = w - static_cast<scalar_type>(0.5) * u + static_cast<scalar_type>(0.5) * v;
      
    thrust::device_vector<device_data_perspective<vector_type, image_size_type>> data(
    std   ::vector       <device_data_perspective<vector_type, image_size_type>>
    {{
      transform.translation,
      direction_00         ,
      u                    ,
      v                    ,
      global_size          ,
      local_size           ,
      local_offset         ,
      coordinate_time
    }});
    
    thrust::device_vector<ray_type> rays(local_size.prod());
    thrust::for_each(
      thrust::make_zip_iterator(thrust::make_tuple(thrust::counting_iterator<std::size_t>(0)          , rays.begin())),
      thrust::make_zip_iterator(thrust::make_tuple(thrust::counting_iterator<std::size_t>(rays.size()), rays.end  ())),
      [data = data.data().get()] __device__ (const auto& iteratee)
      {
        const auto local_index        = thrust::get<0>(iteratee);
        const auto local_multi_index  = unravel_index<image_size_type, true>(local_index, data->local_size);
        const auto global_multi_index = local_multi_index + data->local_offset;

        const auto direction          = (data->direction_00
          + data->u * static_cast<scalar_type>(global_multi_index[0]) / static_cast<scalar_type>(data->global_size[0])
          - data->v * static_cast<scalar_type>(global_multi_index[1]) / static_cast<scalar_type>(data->global_size[1])).normalized();

        auto& ray             = thrust::get<1>(iteratee);
        ray.position [0]      = data->coordinate_time;
        ray.position .tail(3) = data->position;
        ray.direction[0]      = static_cast<scalar_type>(-1);
        ray.direction.tail(3) = direction;
      });
    return rays;
  }

  template <typename image_size_type>
  thrust::device_vector<ray_type> generate_rays_orthographic(
    const image_size_type& global_size ,
    const image_size_type& local_size  ,
    const image_size_type& local_offset)
  {
    using vector_type = typename transform_type::vector_type;

    const auto&       cast_projection = std::get<orthographic_projection<scalar_type>>(projection);
    const vector_type u               = transform.right() * cast_projection.height * cast_projection.aspect_ratio;
    const vector_type v               = transform.up   () * cast_projection.height;
    const vector_type position_00     = transform.translation - static_cast<scalar_type>(0.5) * u + static_cast<scalar_type>(0.5) * v;
    
    thrust::device_vector<device_data_orthographic<vector_type, image_size_type>> data(
    std   ::vector       <device_data_orthographic<vector_type, image_size_type>>
    {{
      transform.forward(),
      position_00        ,
      u                  ,
      v                  ,
      global_size        ,
      local_size         ,
      local_offset       ,
      coordinate_time
    }});

    thrust::device_vector<ray_type> rays(local_size.prod());
    thrust::for_each(
      thrust::make_zip_iterator(thrust::make_tuple(thrust::counting_iterator<std::size_t>(0)          , rays.begin())),
      thrust::make_zip_iterator(thrust::make_tuple(thrust::counting_iterator<std::size_t>(rays.size()), rays.end  ())),
      [data = data.data().get()] __device__ (const auto& iteratee)
      {
        const auto local_index        = thrust::get<0>(iteratee);
        const auto local_multi_index  = unravel_index<image_size_type, true>(local_index, data->local_size);
        const auto global_multi_index = local_multi_index + data->local_offset;
        
        const auto position           = data->position_00
          + data->u * static_cast<scalar_type>(global_multi_index[0]) / static_cast<scalar_type>(data->global_size[0])
          - data->v * static_cast<scalar_type>(global_multi_index[1]) / static_cast<scalar_type>(data->global_size[1]);

        auto& ray             = thrust::get<1>(iteratee);
        ray.position [0]      = data->coordinate_time;
        ray.position .tail(3) = position;
        ray.direction[0]      = static_cast<scalar_type>(-1);
        ray.direction.tail(3) = data->direction;
      });
    return rays;
  }

  projection_type projection      ;
  transform_type  transform       ;
  scalar_type     coordinate_time = static_cast<scalar_type>(0);
};
}