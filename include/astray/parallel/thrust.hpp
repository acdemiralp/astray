#pragma once

#include <thrust/iterator/counting_iterator.h>
#include <thrust/device_vector.h>
#include <thrust/for_each.h>
#include <thrust/optional.h>

#ifdef __CUDACC__
#include <nvfunctional>
namespace ast
{
template <typename type>
using device_function = nvstd::function<type>;
}
#else
#include <functional>
namespace ast
{
template <typename type>
using device_function = std::function  <type>;
}
#endif

#ifndef __constant__
#define __constant__
#endif

#ifndef __device__
#define __device__
#endif

#ifndef __host__
#define __host__
#endif

namespace ast
{
enum class device_system
{
  cuda = THRUST_DEVICE_SYSTEM_CUDA,
  omp  = THRUST_DEVICE_SYSTEM_OMP ,
  tbb  = THRUST_DEVICE_SYSTEM_TBB ,
  cpp  = THRUST_DEVICE_SYSTEM_CPP
};

constexpr static device_system shared_device = static_cast<device_system>(THRUST_DEVICE_SYSTEM);
}