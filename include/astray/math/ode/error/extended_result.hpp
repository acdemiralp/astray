#pragma once

namespace ast
{
template <typename type>
struct extended_result
{
  type value;
  type error;
};
}