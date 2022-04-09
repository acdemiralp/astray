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
using device_function = std::function<type>;
}
#endif

#ifndef __constant__
#define __constant__
#endif