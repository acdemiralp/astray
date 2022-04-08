#pragma once

#include <astray/math/linear_algebra.hpp>

namespace ast
{
template <typename scalar_type, typename vector_type = vector4<scalar_type>>
struct ray
{
  vector_type position         = vector_type::Zero();
  vector_type direction        = vector_type::Zero();
  scalar_type lambda           = static_cast<scalar_type>(0);
  scalar_type lambda_step_size = static_cast<scalar_type>(1);
};
}