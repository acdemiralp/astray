#pragma once

#include <cmath>

#include <astray/math/angle.hpp>
#include <astray/math/constants.hpp>
#include <astray/math/linear_algebra.hpp>

namespace ast
{
enum class coordinate_system
{
  boyer_lindquist   , // t [-INF, INF], r     [   0, INF], theta [   0,  PI], phi [0   , 2PI)
  cartesian         , // t [-INF, INF], x     [-INF, INF], y     [-INF, INF], z   [-INF, INF]
  cylindrical       , // t [-INF, INF], rho   [   0, INF], phi   [   0, 2PI), z   [-INF, INF]
  prolate_spheroidal, // t [-INF, INF], sigma [   1, INF], tau   [  -1,   1], phi [0   , 2PI)
  spherical           // t [-INF, INF], r     [   0, INF], theta [   0,  PI], phi [0   , 2PI)
};

template <coordinate_system system, typename type>
void wrap_angles(type& value)
{
  if      constexpr (system == coordinate_system::boyer_lindquist)
  {
    wrap_angle(value[2], constants::pi    <typename type::value_type>, true );
    wrap_angle(value[3], constants::two_pi<typename type::value_type>, false);
  }
  else if constexpr (system == coordinate_system::cylindrical)
  {
    wrap_angle(value[2], constants::two_pi<typename type::value_type>, false);
  }
  else if constexpr (system == coordinate_system::prolate_spheroidal)
  {
    wrap_angle(value[3], constants::two_pi<typename type::value_type>, false);
  }
  else if constexpr (system == coordinate_system::spherical)
  {
    wrap_angle(value[2], constants::pi    <typename type::value_type>, true );
    wrap_angle(value[3], constants::two_pi<typename type::value_type>, false);
  }
}

template<coordinate_system source, coordinate_system target, typename type, typename scalar = typename type::value_type>
void convert    (type& value)
{
  if        constexpr (source == coordinate_system::cartesian)
  {
    if      constexpr (target == coordinate_system::cylindrical)
    {
      const scalar rho = std::sqrt (static_cast<scalar>(std::pow(value[1], 2)) + static_cast<scalar>(std::pow(value[2], 2)));
      const scalar phi = std::atan2(value[2], value[1]);

      value[1] = rho;
      value[2] = phi;

      wrap_angles<target>(value);
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      const scalar r     = std::sqrt (
        static_cast<scalar>(std::pow(value[1], 2)) + 
        static_cast<scalar>(std::pow(value[2], 2)) + 
        static_cast<scalar>(std::pow(value[3], 2)));
      const scalar theta = std::acos (value[3] / r);
      const scalar phi   = std::atan2(value[2], value[1]);

      value[1] = r;
      value[2] = theta;
      value[3] = phi;

      wrap_angles<target>(value);
    }
  }
  else if   constexpr (source == coordinate_system::cylindrical)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      wrap_angles<source>(value);

      const scalar x = value[1] * std::cos(value[2]);
      const scalar y = value[1] * std::sin(value[2]);

      value[1] = x;
      value[2] = y;
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      wrap_angles<source>(value);

      const scalar r     = std::sqrt(
        static_cast<scalar>(std::pow(value[1], 2)) +
        static_cast<scalar>(std::pow(value[3], 2)));
      const scalar theta = std::atan2(value[1], value[3]);
      const scalar phi   = value[2];

      value[1] = r;
      value[2] = theta;
      value[3] = phi;

      wrap_angles<target>(value);
    }
  }
  else if   constexpr (source == coordinate_system::spherical)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      wrap_angles<source>(value);

      const scalar x = value[1] * std::sin(value[2]) * std::cos(value[3]);
      const scalar y = value[1] * std::sin(value[2]) * std::sin(value[3]);
      const scalar z = value[1] * std::cos(value[2]);

      value[1] = x;
      value[2] = y;
      value[3] = z;
    }
    else if constexpr (target == coordinate_system::cylindrical)
    {
      wrap_angles<source>(value);

      const scalar rho = value[1] * std::sin(value[2]);
      const scalar phi = value[3];
      const scalar z   = value[1] * std::cos(value[2]);

      value[1] = rho;
      value[2] = phi;
      value[3] = z;

      wrap_angles<target>(value);
    }
  }
}
template<coordinate_system source, coordinate_system target, typename type, typename scalar = typename type::value_type>
void convert    (type& value, const scalar free_parameter)
{
  if        constexpr (source == coordinate_system::boyer_lindquist)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      wrap_angles<source>(value);

      const scalar sq_r2_p_a2 = std::sqrt(
        static_cast<scalar>(std::pow(value[1]      , 2)) + 
        static_cast<scalar>(std::pow(free_parameter, 2)));

      const scalar x = sq_r2_p_a2 * std::sin(value[2]) * std::cos(value[3]);
      const scalar y = sq_r2_p_a2 * std::sin(value[2]) * std::sin(value[3]);
      const scalar z = value[1] * std::cos(value[2]);

      value[1] = x;
      value[2] = y;
      value[3] = z;
    }
    else if constexpr (target == coordinate_system::cylindrical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
  }
  else if   constexpr (source == coordinate_system::cartesian)
  {
    if      constexpr (target == coordinate_system::boyer_lindquist)
    {
      const scalar w = 
        static_cast<scalar>(std::pow(value[1]      , 2)) + 
        static_cast<scalar>(std::pow(value[2]      , 2)) + 
        static_cast<scalar>(std::pow(value[3]      , 2)) - 
        static_cast<scalar>(std::pow(free_parameter, 2));

      const scalar r     = std::sqrt (static_cast<scalar>(0.5) * (w + std::sqrt(static_cast<scalar>(std::pow(w, 2)) + static_cast<scalar>(4) * static_cast<scalar>(std::pow(free_parameter, 2)) * static_cast<scalar>(std::pow(value[3], 2)))));
      const scalar theta = std::acos (value[3] / r);
      const scalar phi   = std::atan2(value[2], value[1]);

      value[1] = r;
      value[2] = theta;
      value[3] = phi;

      wrap_angles<target>(value);
    }
    else if constexpr (target == coordinate_system::prolate_spheroidal)
    {
      const scalar x_squared        = static_cast<scalar>(std::pow(value[1], 2));
      const scalar y_squared        = static_cast<scalar>(std::pow(value[2], 2));
      const scalar first_component  = std::sqrt(x_squared + y_squared + static_cast<scalar>(std::pow(value[3] + free_parameter, 2)));
      const scalar second_component = std::sqrt(x_squared + y_squared + static_cast<scalar>(std::pow(value[3] - free_parameter, 2)));
      const scalar _2a              = 2 * free_parameter;

      const scalar sigma = (first_component + second_component) / _2a;
      const scalar tau   = (first_component - second_component) / _2a; 
      const scalar phi   = std::atan2(value[2], value[1]);

      value[1] = sigma;
      value[2] = tau;
      value[3] = phi;

      wrap_angles<target>(value);
    }
  }
  else if   constexpr (source == coordinate_system::cylindrical)
  {
    if      constexpr (target == coordinate_system::boyer_lindquist)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
    else if constexpr (target == coordinate_system::prolate_spheroidal)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
  }
  else if   constexpr (source == coordinate_system::prolate_spheroidal)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      wrap_angles<source>(value);

      const scalar common = free_parameter * std::sqrt((static_cast<scalar>(std::pow(value[1], 2)) - static_cast<scalar>(1)) * (static_cast<scalar>(1) - static_cast<scalar>(std::pow(value[2], 2))));
      const scalar x      = common * std::cos(value[3]);
      const scalar y      = common * std::sin(value[3]);
      const scalar z      = free_parameter * value[1] * value[2];

      value[1] = x;
      value[2] = y;
      value[3] = z;
    }
    else if constexpr (target == coordinate_system::cylindrical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
  }
  else if   constexpr (source == coordinate_system::spherical)
  {
    if      constexpr (target == coordinate_system::boyer_lindquist)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
    else if constexpr (target == coordinate_system::prolate_spheroidal)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
  }
}

template<coordinate_system source, coordinate_system target, typename type, typename scalar = typename type::value_type>
void convert_ray(type& value)
{
  if        constexpr (source == coordinate_system::cartesian)
  {
    if      constexpr (target == coordinate_system::cylindrical)
    {
      convert<source, target, type, scalar>(value.position);

      const scalar& r = value.position[1];
      const scalar& p = value.position[2];

      matrix44<scalar> transform;
      transform << 
        1,                 0,               0, 0,
        0,   std::cos(p)    , std::sin(p)    , 0,
        0, - std::sin(p) / r, std::cos(p) / r, 0,
        0,                 0,               0, 1;

      value.direction = transform * value.direction;
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      convert<source, target, type, scalar>(value.position);
      
      const scalar& r = value.position[1];
      const scalar& t = value.position[2];
      const scalar& p = value.position[3];

      matrix44<scalar> transform;
      transform << 
        1,                               0,                             0,                0,
        0,   std::sin(t) * std::cos(p)    , std::sin(t) * std::sin(p)    ,  std::cos(t)    ,
        0,   std::cos(t) * std::cos(p) / r, std::cos(t) * std::sin(p) / r, -std::sin(t) / r,
        0, - std::sin(p) / std::sin(t) / r, std::cos(p) / std::sin(t) / r,                0;

      value.direction = transform * value.direction;
    }
  }
  else if   constexpr (source == coordinate_system::cylindrical)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      const scalar& r = value.position[1];
      const scalar& p = value.position[2];

      matrix44<scalar> transform;
      transform << 
        1,           0,                0, 0,
        0, std::cos(p), -r * std::sin(p), 0,
        0, std::sin(p),  r * std::cos(p), 0,
        0,           0,                0, 1;

      convert<source, target, type, scalar>(value.position);

      value.direction = transform * value.direction;
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      convert<source, target, type, scalar>(value.position);

      const scalar& r = value.position[1];
      const scalar& t = value.position[2];

      matrix44<scalar> transform;
      transform << 
        1,               0, 0,                0,
        0, std::sin(t)    , 0,  std::cos(t)    ,
        0, std::cos(t) / r, 0, -std::sin(t) / r,
        0,               0, 1,                0;

      value.direction = transform * value.direction;
    }
  }
  else if   constexpr (source == coordinate_system::spherical)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      const scalar& r = value.position[1];
      const scalar& t = value.position[2];
      const scalar& p = value.position[3];

      matrix44<scalar> transform;
      transform << 
        1,                         0,                              0,                              0,
        0, std::sin(t) * std::cos(p),  r * std::cos(t) * std::cos(p), -r * std::sin(t) * std::sin(p),
        0, std::sin(t) * std::sin(p),  r * std::cos(t) * std::sin(p),  r * std::sin(t) * std::cos(p),
        0, std::cos(t)              , -r * std::sin(t)              ,                              0;

      convert<source, target, type, scalar>(value.position);

      value.direction = transform * value.direction;
    }
    else if constexpr (target == coordinate_system::cylindrical)
    {
      const scalar& r = value.position[1];
      const scalar& t = value.position[2];

      matrix44<scalar> transform;
      transform << 
        1,           0,                0, 0,
        0, std::sin(t),  r * std::cos(t), 0,
        0,           0,                0, 1,
        0, std::cos(t), -r * std::sin(t), 0;

      convert<source, target, type, scalar>(value.position);

      value.direction = transform * value.direction;
    }
  }
}
template<coordinate_system source, coordinate_system target, typename type, typename scalar = typename type::value_type>
void convert_ray(type& value, const scalar free_parameter)
{
  if        constexpr (source == coordinate_system::boyer_lindquist)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      const scalar& r = value.position[1];
      const scalar& t = value.position[2];
      const scalar& p = value.position[3];
      const scalar  k = std::sqrt(static_cast<scalar>(std::pow(free_parameter, 2)) + static_cast<scalar>(std::pow(r, 2)));

      matrix44<scalar> transform;
      transform << 
        1,                                 0,                             0,                              0,
        0, r * std::sin(t) * std::cos(p) / k, std::cos(t) * std::cos(p) * k, -std::sin(t) * std::sin(p) * k,
        0, r * std::sin(t) * std::sin(p) / k, std::cos(t) * std::sin(p) * k,  std::sin(t) * std::cos(p) * k,
        0,                       std::cos(t),              -r * std::sin(t),                              0;

      convert<source, target, type, scalar>(value.position, free_parameter);

      value.direction = transform * value.direction;
    }
    else if constexpr (target == coordinate_system::cylindrical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
  }
  else if   constexpr (source == coordinate_system::cartesian)
  {
    if      constexpr (target == coordinate_system::boyer_lindquist)
    {
      convert<source, target, type, scalar>(value.position, free_parameter);

      const scalar& r = value.position[1];
      const scalar& t = value.position[2];
      const scalar& p = value.position[3];
      const scalar  k = std::sqrt(static_cast<scalar>(std::pow(free_parameter, 2)) + static_cast<scalar>(std::pow(r, 2)));

      matrix44<scalar> transform;
      transform << 
        1,                                 0,                             0,                              0,
        0, r * std::sin(t) * std::cos(p) / k, std::cos(t) * std::cos(p) * k, -std::sin(t) * std::sin(p) * k,
        0, r * std::sin(t) * std::sin(p) / k, std::cos(t) * std::sin(p) * k,  std::sin(t) * std::cos(p) * k,
        0,                       std::cos(t),              -r * std::sin(t),                              0;
      transform = transform.inverse().eval(); // TODO: Ideally inversion should be analytic without runtime overhead. See https://github.com/einsteinpy/einsteinpy/blob/main/src/einsteinpy/coordinates/utils.py

      value.direction = transform * value.direction;
    }
    else if constexpr (target == coordinate_system::prolate_spheroidal)
    {
      convert<source, target, type, scalar>(value.position, free_parameter);

      const scalar& s = value.position[1];
      const scalar& t = value.position[2];
      const scalar& p = value.position[3];
      const scalar& a = free_parameter;
      const scalar  k = std::sqrt(-(static_cast<scalar>(std::pow(s, 2)) - static_cast<scalar>(1)) * (static_cast<scalar>(std::pow(t, 2)) - static_cast<scalar>(1)));

      matrix44<scalar> transform;
      transform << 
        1,                                                                    0,                                                                    0,                    0,
        0, -a * s * (static_cast<scalar>(std::pow(t, 2)) - 1) * std::cos(p) / k, -a * t * (static_cast<scalar>(std::pow(s, 2)) - 1) * std::cos(p) / k, -a * std::sin(p) * k,
        0, -a * s * (static_cast<scalar>(std::pow(t, 2)) - 1) * std::sin(p) / k, -a * t * (static_cast<scalar>(std::pow(s, 2)) - 1) * std::sin(p) / k,  a * std::cos(p) * k,
        0,  a * t                                                              ,  a * s                                                              ,                    0;
      transform = transform.inverse().eval(); // TODO: Ideally inversion should be analytic without runtime overhead.

      value.direction = transform * value.direction;
    }
  }
  else if   constexpr (source == coordinate_system::cylindrical)
  {
    if      constexpr (target == coordinate_system::boyer_lindquist)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
    else if constexpr (target == coordinate_system::prolate_spheroidal)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
  }
  else if   constexpr (source == coordinate_system::prolate_spheroidal)
  {
    if      constexpr (target == coordinate_system::cartesian)
    {
      const scalar& s = value.position[1];
      const scalar& t = value.position[2];
      const scalar& p = value.position[3];
      const scalar& a = free_parameter;
      const scalar  k = std::sqrt(-(static_cast<scalar>(std::pow(s, 2)) - 1) * (static_cast<scalar>(std::pow(t, 2)) - 1));

      matrix44<scalar> transform;
      transform << 
        1,                                                                    0,                                                                    0,                    0,
        0, -a * s * (static_cast<scalar>(std::pow(t, 2)) - 1) * std::cos(p) / k, -a * t * (static_cast<scalar>(std::pow(s, 2)) - 1) * std::cos(p) / k, -a * std::sin(p) * k,
        0, -a * s * (static_cast<scalar>(std::pow(t, 2)) - 1) * std::sin(p) / k, -a * t * (static_cast<scalar>(std::pow(s, 2)) - 1) * std::sin(p) / k,  a * std::cos(p) * k,
        0,  a * t                                                              ,  a * s                                                              ,                    0;

      convert<source, target, type, scalar>(value.position, free_parameter);

      value.direction = transform * value.direction;
    }
    else if constexpr (target == coordinate_system::cylindrical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
    else if constexpr (target == coordinate_system::spherical)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value, free_parameter);
      convert<coordinate_system::cartesian, target, type, scalar>(value);
    }
  }
  else if   constexpr (source == coordinate_system::spherical)
  {
    if      constexpr (target == coordinate_system::boyer_lindquist)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
    else if constexpr (target == coordinate_system::prolate_spheroidal)
    {
      convert<source, coordinate_system::cartesian, type, scalar>(value);
      convert<coordinate_system::cartesian, target, type, scalar>(value, free_parameter);
    }
  }
}
}