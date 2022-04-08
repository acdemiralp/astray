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
class kastor_traschen : public metric<coordinate_system::cartesian, scalar_type, vector_type, christoffel_symbols_type>
{
public:
  using constants = constants<scalar_type>;

  thrust::optional<termination_reason> check_termination  (const vector_type& position, const vector_type& direction) const override
  {
    return thrust::nullopt;
  }

  christoffel_symbols_type             christoffel_symbols(const vector_type& position) const override
  {
    const auto p1_2_p2_2 = static_cast<scalar_type>(std::pow(position[1], 2)) + static_cast<scalar_type>(std::pow(position[2], 2));

    const auto r1        = std::sqrt(p1_2_p2_2 + static_cast<scalar_type>(std::pow(position[3] - position1_, 2)));
    const auto r2        = std::sqrt(p1_2_p2_2 + static_cast<scalar_type>(std::pow(position[3] - position2_, 2)));
    const auto a         = std::exp(h_ * position[0]);
    const auto omega     = static_cast<scalar_type>(1) + mass1_ / (r1 * a) + mass2_ / (r2 * a);

    const auto r1_2_a    = static_cast<scalar_type>(std::pow(r1, 2)) * a;
    const auto r2_2_a    = static_cast<scalar_type>(std::pow(r2, 2)) * a;
    const auto m1_r1_2_a = -mass1_ / r1_2_a;
    const auto m2_r2_2_a = -mass2_ / r2_2_a;

    const auto dr1dx     = position[1] / r1;
    const auto dr1dy     = position[2] / r1;
    const auto dr1dz     = (position[3] - position1_) / r1;
    const auto dr2dx     = position[1] / r2;
    const auto dr2dy     = position[2] / r2;
    const auto dr2dz     = (position[3] - position2_) / r2;
    const auto dodx      = m1_r1_2_a * dr1dx + m2_r2_2_a * dr2dx;
    const auto dody      = m1_r1_2_a * dr1dy + m2_r2_2_a * dr2dy;
    const auto dodz      = m1_r1_2_a * dr1dz + m2_r2_2_a * dr2dz;
    const auto dadt      = h_ * a;
    const auto dodt      = -mass1_ * dadt / (r1 * static_cast<scalar_type>(std::pow(a, 2))) - mass2_ * dadt / (r2 * static_cast<scalar_type>(std::pow(a, 2)));

    const auto t1        = omega;
    const auto t2        = static_cast<scalar_type>(1) / t1;
    const auto t3        = dodt;
    const auto t5        = static_cast<scalar_type>(std::pow(t1, 2));
    const auto t6        = static_cast<scalar_type>(std::pow(t5, 2));
    const auto t9        = a;
    const auto t10       = static_cast<scalar_type>(std::pow(t9, 2));
    const auto t12       = static_cast<scalar_type>(1) / t6 / t1 / t10;
    const auto t13       = dodx;
    const auto t15       = dody;
    const auto t17       = dodz;
    const auto t19       = t2 * t13;
    const auto t23       = dadt;
    const auto t25       = t9 * t3 + t1 * t23;
    const auto t26       = t2 / t9 * t25;
    const auto t27       = t2 * t15;
    const auto t28       = t2 * t17;
    const auto t31       = t5 * t1 * t9 * t25;

    christoffel_symbols_type symbols;
    symbols.setZero();
    symbols(0, 0, 0) = -t2  * t3;
    symbols(0, 0, 1) = -t12 * t13;
    symbols(0, 0, 2) = -t12 * t15;
    symbols(0, 0, 3) = -t12 * t17;
    symbols(0, 1, 0) = -t19;
    symbols(0, 1, 1) =  t26;
    symbols(0, 2, 0) = -t27;
    symbols(0, 2, 2) =  t26;
    symbols(0, 3, 0) = -t28;
    symbols(0, 3, 3) =  t26;
    symbols(1, 0, 0) = -t19;
    symbols(1, 0, 1) =  t26;
    symbols(1, 1, 0) =  t31;
    symbols(1, 1, 1) =  t19;
    symbols(1, 1, 2) = -t27;
    symbols(1, 1, 3) = -t28;
    symbols(1, 2, 1) =  t27;
    symbols(1, 2, 2) =  t19;
    symbols(1, 3, 1) =  t28;
    symbols(1, 3, 3) =  t19;
    symbols(2, 0, 0) = -t27;
    symbols(2, 0, 2) =  t26;
    symbols(2, 1, 1) =  t27;
    symbols(2, 1, 2) =  t19;
    symbols(2, 2, 0) =  t31;
    symbols(2, 2, 1) = -t19;
    symbols(2, 2, 2) =  t27;
    symbols(2, 2, 3) = -t28;
    symbols(2, 3, 2) =  t28;
    symbols(2, 3, 3) =  t27;
    symbols(3, 0, 0) = -t28;
    symbols(3, 0, 3) =  t26;
    symbols(3, 1, 1) =  t28;
    symbols(3, 1, 3) =  t19;
    symbols(3, 2, 2) =  t28;
    symbols(3, 2, 3) =  t27;
    symbols(3, 3, 0) =  t31;
    symbols(3, 3, 1) = -t19;
    symbols(3, 3, 2) = -t27;
    symbols(3, 3, 3) =  t28;
    return symbols;
  }
  
protected:
  scalar_type mass1_     = static_cast<scalar_type>( 1);
  scalar_type position1_ = static_cast<scalar_type>( 1);
  scalar_type mass2_     = static_cast<scalar_type>( 1);
  scalar_type position2_ = static_cast<scalar_type>(-1);
  scalar_type h_         = static_cast<scalar_type>(constants::eps);
};
}