#pragma once

#include <cmath>

#include <astray/core/metric.hpp>
#include <astray/math/constants.hpp>

namespace ast::metrics
{
template <
  typename scalar_type              , 
  typename vector_type              = vector4  <scalar_type>, 
  typename christoffel_symbols_type = tensor444<scalar_type>>
class bertotti_kasner : public metric<coordinate_system::spherical, scalar_type, vector_type, christoffel_symbols_type>
{
public:
  using constants = constants<scalar_type>;

  __device__ christoffel_symbols_type christoffel_symbols(const vector_type& position) const override
  {
    const auto sqrt_lambda = std::sqrt(constants::cosmological_constant);
    const auto cot_theta   = std::cos (position[2]) / std::sin(position[2]);

    christoffel_symbols_type symbols;
    symbols.setZero();
    symbols(0, 1, 1) = constants::speed_of_light * sqrt_lambda;
    symbols(1, 0, 1) = constants::speed_of_light * sqrt_lambda;
    symbols(1, 1, 0) = sqrt_lambda / constants::speed_of_light * std::exp(2.0 * sqrt_lambda * constants::speed_of_light * position[0]);
    symbols(2, 3, 3) = cot_theta;
    symbols(3, 2, 3) = cot_theta;
    symbols(3, 3, 2) = -std::sin(position[2]) * std::cos(position[2]);
    return symbols;
  }
};
}