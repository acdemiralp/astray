#include <doctest/doctest.h>

#include <astray/math/angle.hpp>
#include <astray/parallel/thrust.hpp>

void range_test()
{
  using type = float;

  thrust::device_vector<type> values     (360);
  std   ::vector       <type> values_host(360);

  for_each(
    make_zip_iterator(make_tuple(thrust::counting_iterator<std::size_t>(0)            , values.begin())),
    make_zip_iterator(make_tuple(thrust::counting_iterator<std::size_t>(values.size()), values.end  ())),
    [ ] __device__ (const auto& iteratee)
    {
      const auto  index = thrust::get<0>(iteratee);
            auto& value = thrust::get<1>(iteratee);

      value = ast::to_radians(static_cast<type>(index));
    });
  
  thrust::copy(values.begin(), values.end(), values_host.begin());
#ifdef __CUDACC__
  cudaDeviceSynchronize();
#endif

  for (std::size_t i = 0; i < values_host.size(); ++i)
    REQUIRE(values_host[i] == doctest::Approx(ast::to_radians(static_cast<type>(i))));
  
  for_each(
    values.begin(), 
    values.end  (), 
    [ ] __device__ (auto& iteratee) 
    { 
      iteratee = ast::to_degrees(iteratee); 
    });
  
  thrust::copy(values.begin(), values.end(), values_host.begin());
#ifdef __CUDACC__
  cudaDeviceSynchronize();
#endif

  for (std::size_t i = 0; i < values_host.size(); ++i)
    REQUIRE(values_host[i] == doctest::Approx(i));
}

TEST_CASE("ast/angle.hpp")
{
  range_test();
}

