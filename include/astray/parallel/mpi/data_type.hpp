#pragma once

#include <cstdint>
#include <vector>

#ifdef ASTRAY_USE_MPI
#include <mpi.h>
#endif

namespace ast::mpi
{
class data_type
{
public:
  data_type           (std::int32_t native = 0, bool managed = false) 
  : native_(native), managed_(managed)
  {
  
  }
  data_type           (const data_type&  that, std::int32_t size) 
  : managed_(true)
  {
#ifdef ASTRAY_USE_MPI
    MPI_Type_contiguous(size, that.native(), &native_);
    MPI_Type_commit    (&native_);
#endif
  }
  data_type           (const data_type&  that, const std::int64_t lower_bound, const std::int64_t extent) 
  : managed_(true)
  {
#ifdef ASTRAY_USE_MPI
    MPI_Type_create_resized(that.native(), lower_bound, extent, &native_);
    MPI_Type_commit        (&native_);
#endif
  }
  template <typename type>
  data_type           (const data_type&  that, const type& sizes, const type& sub_sizes, const type& starts, const bool fortran_order = false)
  : managed_(true)
  {
#ifdef ASTRAY_USE_MPI
    MPI_Type_create_subarray(static_cast<std::int32_t>(sizes.size()), sizes.data(), sub_sizes.data(), starts.data(), fortran_order ? MPI_ORDER_FORTRAN : MPI_ORDER_C, that.native(), &native_);
    MPI_Type_commit         (&native_);
#endif
  }
  data_type           (const data_type&  that) = delete;
  data_type           (      data_type&& temp) = delete;
  virtual ~data_type  ()
  {
#ifdef ASTRAY_USE_MPI
    if (managed_)
      MPI_Type_free(&native_);
#endif
  }
  data_type& operator=(const data_type&  that) = delete;
  data_type& operator=(      data_type&& temp) = delete;
  
  constexpr std::int32_t native() const
  {
    return native_;
  }

protected:
  std::int32_t native_ ;
  bool         managed_;
};
}