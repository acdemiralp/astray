#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <numeric>
#include <vector>

#include <astray/third_party/mdspan/mdspan>

namespace ast
{
template <
  typename    type      ,
  std::size_t dimensions,
  typename    layout    = std::experimental::layout_right,
  typename    accessor  = std::experimental::default_accessor<type>,
  typename    allocator = std::allocator<type>>
class multi_vector
{
public:
  using storage_type           = std::vector<type, allocator>;
  using span_type              = std::experimental::mdspan<type, std::experimental::dextents<dimensions>, layout, accessor>;

  using value_type             = typename storage_type::value_type;
  using allocator_type         = typename storage_type::allocator_type;
  using size_type              = typename storage_type::size_type;
  using difference_type        = typename storage_type::difference_type;
  using reference              = typename storage_type::reference;
  using const_reference        = typename storage_type::const_reference;
  using pointer                = typename storage_type::pointer;
  using const_pointer          = typename storage_type::const_pointer;
  using iterator               = typename storage_type::iterator;
  using const_iterator         = typename storage_type::const_iterator;
  using reverse_iterator       = typename storage_type::reverse_iterator;
  using const_reverse_iterator = typename storage_type::const_reverse_iterator;

  using multi_size_type        = std::array<size_type      , dimensions>;
  using multi_difference_type  = std::array<difference_type, dimensions>;

  // TODO: Multi versions of constructors, operator=, operator[], at, size.

  constexpr          multi_vector() noexcept(noexcept(allocator_type())) = default;
  constexpr explicit multi_vector(                                                       const allocator_type& alloc) noexcept
  : storage_(alloc)
  {

  }
  
  constexpr          multi_vector(size_type count, const_reference value = value_type(), const allocator_type& alloc = allocator_type())
  : storage_(count, value, alloc), span_(storage_.data(), storage_.size())
  {
    // TODO
  }

  constexpr explicit multi_vector(size_type count,                                       const allocator_type& alloc = allocator_type())
  {
    // TODO
  }
  template <typename input_iterator>
  constexpr          multi_vector(input_iterator first, input_iterator last,             const allocator_type& alloc = allocator_type())
  {
    // TODO
  }

  constexpr          multi_vector(std::initializer_list<value_type> list,                const allocator_type& alloc = allocator_type())
  {
    // TODO
  }
  
  constexpr          multi_vector(const multi_vector&  that)
  : storage_(that.storage_),        span_(storage_.data(), that.span_.mapping(), that.span_.accessor())
  {

  }
  constexpr          multi_vector(const multi_vector&  that,                             const allocator_type& alloc)
  : storage_(that.storage_, alloc), span_(storage_.data(), that.span_.mapping(), that.span_.accessor())
  {

  }
  constexpr          multi_vector(      multi_vector&& temp) noexcept
  : storage_(temp.storage_),        span_(storage_.data(), temp.span_.mapping(), temp.span_.accessor())
  {
    temp.storage_ = {};
    temp.span_    = {};
  }
  constexpr          multi_vector(      multi_vector&& temp,                             const allocator_type& alloc)
  : storage_(temp.storage_, alloc), span_(storage_.data(), temp.span_.mapping(), temp.span_.accessor())
  {
    temp.storage_ = {};
    temp.span_    = {};
  }
  
  constexpr         ~multi_vector() = default;

  constexpr multi_vector&                                operator=    (const multi_vector&  that)
  {
    if (this != &that)
    {
      storage_      = that.storage_;
      span_         = span_type(storage_.data(), that.span_.mapping(), that.span_.accessor());
    }
    return *this;
  }
  constexpr multi_vector&                                operator=    (      multi_vector&& temp) noexcept
  {
    if (this != &temp)
    {
      storage_      = temp.storage_;
      span_         = span_type(storage_.data(), temp.span_.mapping(), temp.span_.accessor());

      temp.storage_ = {};
      temp.span_    = {};
    }
    return *this;
  }
  
  constexpr multi_vector&                                operator=    (std::initializer_list<type> list)
  {
    // TODO
    return *this;
  }

  constexpr std::enable_if_t<dimensions == 1>            assign       (size_type count, const_reference value)
  {
    storage_.assign(count, value);
    span_ = span_type(storage_.data(), storage_.size());
  }
  template <typename input_iterator>
  constexpr std::enable_if_t<dimensions == 1>            assign       (input_iterator first, input_iterator last)
  {
    storage_.assign(first, last);
    span_ = span_type(storage_.data(), storage_.size());
  }
  constexpr std::enable_if_t<dimensions == 1>            assign       (std::initializer_list<type> list)
  {
    storage_.assign(list);
    span_ = span_type(storage_.data(), storage_.size());
  }
  
  constexpr std::enable_if_t<dimensions != 1>            assign       (multi_size_type count, const_reference value)
  {
    storage_.assign(std::accumulate(count.begin(), count.end(), static_cast<size_type>(1), std::multiplies<size_type>()), value);
    span_ = span_type(storage_.data(), count);
  }
  template <typename input_iterator>
  constexpr std::enable_if_t<dimensions != 1>            assign       (multi_size_type count, input_iterator first, input_iterator last)
  {
    // TODO
    assert(std::accumulate(count.begin(), count.end(), static_cast<size_type>(1), std::multiplies<size_type>()) == last - first);
    storage_.assign(first, last);
    span_ = span_type(storage_.data(), count);
  }
  constexpr std::enable_if_t<dimensions != 1>            assign       (multi_size_type count, std::initializer_list<type> list)
  {
    // TODO
    assert(std::accumulate(count.begin(), count.end(), static_cast<size_type>(1), std::multiplies<size_type>()) == list.size());
    storage_.assign(list);
    span_ = span_type(storage_.data(), count);
  }

  constexpr allocator_type                               get_allocator() const noexcept
  {
    return storage_.get_allocator();
  }

  // Element access.

  constexpr reference                                    operator[]   (size_type position)
  {
    return storage_[position];
  }
  constexpr const_reference                              operator[]   (size_type position) const
  {
    return storage_[position];
  }
  
  constexpr reference                                    at           (size_type position)
  {
    return storage_.at(position);
  }
  constexpr const_reference                              at           (size_type position) const
  {
    return storage_.at(position);
  }
  
  constexpr reference                                    front        ()
  {
    return storage_.front();
  }
  constexpr const_reference                              front        () const
  {
    return storage_.front();
  }

  constexpr reference                                    back         ()
  {
    return storage_.back();
  }
  constexpr const_reference                              back         () const
  {
    return storage_.back();
  }

  constexpr pointer                                      data         () noexcept
  {
    return storage_.data();
  }
  constexpr const_pointer                                data         () const noexcept
  {
    return storage_.data();
  }

  // Iterators.

  constexpr iterator                                     begin        () noexcept
  {
    return storage_.begin ();
  }
  constexpr const_iterator                               begin        () const noexcept
  {
    return storage_.begin ();
  }
  constexpr const_iterator                               cbegin       () const noexcept
  {
    return storage_.cbegin();
  }
  
  constexpr iterator                                     end          () noexcept
  {
    return storage_.end   ();
  }
  constexpr const_iterator                               end          () const noexcept
  {
    return storage_.end   ();
  }
  constexpr const_iterator                               cend         () const noexcept
  {
    return storage_.cend  ();
  }

  constexpr reverse_iterator                             rbegin       () noexcept
  {
    return storage_.rbegin ();
  }
  constexpr const_reverse_iterator                       rbegin       () const noexcept
  {
    return storage_.rbegin ();
  }
  constexpr const_reverse_iterator                       crbegin      () const noexcept
  {
    return storage_.crbegin();
  }
  
  constexpr reverse_iterator                             rend         () noexcept
  {
    return storage_.rend   ();
  }
  constexpr const_reverse_iterator                       rend         () const noexcept
  {
    return storage_.rend   ();
  }
  constexpr const_reverse_iterator                       crend        () const noexcept
  {
    return storage_.crend  ();
  }

  // Capacity.

  constexpr bool                                         empty        () const noexcept
  {
    return storage_.empty   ();
  }
  constexpr size_type                                    size         () const noexcept
  {
    return storage_.size    ();
  }
  constexpr size_type                                    max_size     () const noexcept
  {
    return storage_.max_size();
  }
  constexpr void                                         reserve      (size_type capacity)
  {
    storage_.reserve(capacity);
  }
  constexpr size_type                                    capacity     () const noexcept
  {
    return storage_.capacity();
  }
  constexpr void                                         shrink_to_fit()
  {
    return storage_.shrink_to_fit();
  }

  // Modifiers.

  constexpr void                                         clear        ()
  {
    storage_.clear();
    span_ = {};
  }

  constexpr std::enable_if_t<dimensions == 1, iterator>  insert       (const_iterator position, const_reference value)
  {
    auto result = storage_.insert(position, value);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  constexpr std::enable_if_t<dimensions == 1, iterator>  insert       (const_iterator position, value_type&& value)
  {
    auto result = storage_.insert(position, value);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  constexpr std::enable_if_t<dimensions == 1, iterator>  insert       (const_iterator position, size_type count, const_reference value)
  {
    auto result = storage_.insert(position, count, value);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  template <typename input_iterator>
  constexpr std::enable_if_t<dimensions == 1, iterator>  insert       (const_iterator position, input_iterator first, input_iterator last)
  {
    auto result = storage_.insert(position, first, last);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  constexpr std::enable_if_t<dimensions == 1, iterator>  insert       (const_iterator position, std::initializer_list<value_type> list)
  {
    auto result = storage_.insert(position, list);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  template <typename... argument_types>
  constexpr std::enable_if_t<dimensions == 1, iterator>  emplace      (const_iterator position, argument_types&&... arguments)
  {
    auto result = storage_.emplace(position, arguments...);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  constexpr std::enable_if_t<dimensions == 1, iterator>  erase        (const_iterator position)
  {
    auto result = storage_.erase(position);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  constexpr std::enable_if_t<dimensions == 1, iterator>  erase        (const_iterator first, const_iterator last)
  {
    auto result = storage_.erase(first, last);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  
  constexpr std::enable_if_t<dimensions == 1>            push_back    (const_reference value)
  {
    storage_.push_back(value);
    span_ = span_type(storage_.data(), storage_.size());
  }
  constexpr std::enable_if_t<dimensions == 1>            push_back    (value_type&&    value)
  {
    storage_.push_back(value);
    span_ = span_type(storage_.data(), storage_.size());
  }
  template <typename... argument_types>
  constexpr std::enable_if_t<dimensions == 1, reference> emplace_back (argument_types&&... arguments)
  {
    auto result = storage_.emplace_back(arguments...);
    span_ = span_type(storage_.data(), storage_.size());
    return result;
  }
  constexpr std::enable_if_t<dimensions == 1>            pop_back     ()
  {
    storage_.pop_back();
    span_ = span_type(storage_.data(), storage_.size());
  }

  constexpr std::enable_if_t<dimensions == 1>            resize       (size_type count)
  {
    storage_.resize(count);
    span_ = span_type(storage_.data(), count);
  }
  constexpr std::enable_if_t<dimensions == 1>            resize       (size_type count, const_reference value)
  {
    storage_.resize(count, value);
    span_ = span_type(storage_.data(), count);
  }
  constexpr std::enable_if_t<dimensions != 1>            resize       (multi_size_type count)
  {
    storage_.resize(std::accumulate(count.begin(), count.end(), static_cast<size_type>(1), std::multiplies<size_type>()));
    span_ = span_type(storage_.data(), count);
  }
  constexpr std::enable_if_t<dimensions != 1>            resize       (multi_size_type count, const_reference value)
  {
    storage_.resize(std::accumulate(count.begin(), count.end(), static_cast<size_type>(1), std::multiplies<size_type>()), value);
    span_ = span_type(storage_.data(), count);
  }
  
  constexpr void                                         swap         (multi_vector& other) noexcept
  {
    std::swap(storage_, other.storage_);
    std::swap(span_   , other.span_   );
  }

  // Member access.

  constexpr const storage_type&                          storage      () noexcept
  {
    return storage_;
  }
  constexpr const span_type&                             span         () noexcept
  {
    return span_;
  }

protected:
  storage_type storage_;
  span_type    span_   ;
};

// Non-member functions.

template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr bool operator==(
  const multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
  const multi_vector<type, dimensions, layout, accessor, allocator>& rhs)
{
  return lhs.span().mapping() == rhs.span().mapping() && lhs.storage() == rhs.storage();
}
template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr bool operator!=(
  const multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
  const multi_vector<type, dimensions, layout, accessor, allocator>& rhs)
{
  return !(lhs.span().mapping() == rhs.span().mapping() && lhs.storage() == rhs.storage());
}
template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr bool operator< (
  const multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
  const multi_vector<type, dimensions, layout, accessor, allocator>& rhs)
{
  return lhs.storage() <  rhs.storage();
}
template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr bool operator<=(
  const multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
  const multi_vector<type, dimensions, layout, accessor, allocator>& rhs)
{
  return lhs.storage() <= rhs.storage();
}
template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr bool operator> (
  const multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
  const multi_vector<type, dimensions, layout, accessor, allocator>& rhs)
{
  return lhs.storage() >  rhs.storage();
}
template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr bool operator>=(
  const multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
  const multi_vector<type, dimensions, layout, accessor, allocator>& rhs)
{
  return lhs.storage() >= rhs.storage();
}

template <typename type, std::size_t dimensions, typename layout, typename accessor, typename allocator>
constexpr void swap(
        multi_vector<type, dimensions, layout, accessor, allocator>& lhs, 
        multi_vector<type, dimensions, layout, accessor, allocator>& rhs) noexcept
{
  lhs.swap(rhs);
}
}