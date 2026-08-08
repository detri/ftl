#ifndef FTL_VECTOR_ITERATOR_HEADER
#define FTL_VECTOR_ITERATOR_HEADER

#ifdef FTL_REPLACE_STL
#include <compare>
#include <iterator>
#include <memory>
#define FTL_VECTOR_ITERATOR_NAMESPACE std
#else
#include <ftl/compare>
#include <ftl/iterator>
#include <ftl/memory>
#define FTL_VECTOR_ITERATOR_NAMESPACE ftl
#endif

FTL_BEGIN_NAMESPACE

template<class Vector>
class vector_const_iterator {
  using mutable_pointer = typename Vector::pointer;

public:
  using iterator_concept = contiguous_iterator_tag;
  using iterator_category = random_access_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;

  constexpr vector_const_iterator() = default;
  constexpr explicit vector_const_iterator(mutable_pointer value) : pointer_(value) {}

  [[nodiscard]] constexpr reference operator*() const noexcept { return *pointer_; }
  [[nodiscard]] constexpr pointer operator->() const noexcept { return pointer_; }
  constexpr vector_const_iterator& operator++() noexcept { ++pointer_; return *this; }
  constexpr vector_const_iterator operator++(int) noexcept { auto copy = *this; ++*this; return copy; }
  constexpr vector_const_iterator& operator--() noexcept { --pointer_; return *this; }
  constexpr vector_const_iterator operator--(int) noexcept { auto copy = *this; --*this; return copy; }
  constexpr vector_const_iterator& operator+=(difference_type offset) noexcept { pointer_ += offset; return *this; }
  constexpr vector_const_iterator& operator-=(difference_type offset) noexcept { pointer_ -= offset; return *this; }
  [[nodiscard]] constexpr vector_const_iterator operator+(difference_type offset) const noexcept { auto copy = *this; return copy += offset; }
  [[nodiscard]] constexpr vector_const_iterator operator-(difference_type offset) const noexcept { auto copy = *this; return copy -= offset; }
  [[nodiscard]] constexpr difference_type operator-(const vector_const_iterator& other) const noexcept { return pointer_ - other.pointer_; }
  [[nodiscard]] constexpr reference operator[](difference_type offset) const noexcept { return pointer_[offset]; }
  [[nodiscard]] constexpr bool operator==(const vector_const_iterator&) const noexcept = default;
  [[nodiscard]] constexpr strong_ordering operator<=>(const vector_const_iterator& other) const noexcept {
    if (pointer_ < other.pointer_) return strong_ordering::less;
    if (other.pointer_ < pointer_) return strong_ordering::greater;
    return strong_ordering::equal;
  }
  friend constexpr vector_const_iterator operator+(difference_type offset, vector_const_iterator value) noexcept { return value += offset; }

protected:
  mutable_pointer pointer_{};

  template<class> friend class vector_iterator;
  template<class, class> friend struct pointer_traits;
};

template<class Vector>
class vector_iterator : public vector_const_iterator<Vector> {
  using base = vector_const_iterator<Vector>;

public:
  using iterator_concept = contiguous_iterator_tag;
  using iterator_category = random_access_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;
  using base::base;

  [[nodiscard]] constexpr reference operator*() const noexcept { return *this->pointer_; }
  [[nodiscard]] constexpr pointer operator->() const noexcept { return this->pointer_; }
  constexpr vector_iterator& operator++() noexcept { ++this->pointer_; return *this; }
  constexpr vector_iterator operator++(int) noexcept { auto copy = *this; ++*this; return copy; }
  constexpr vector_iterator& operator--() noexcept { --this->pointer_; return *this; }
  constexpr vector_iterator operator--(int) noexcept { auto copy = *this; --*this; return copy; }
  constexpr vector_iterator& operator+=(difference_type offset) noexcept { this->pointer_ += offset; return *this; }
  constexpr vector_iterator& operator-=(difference_type offset) noexcept { this->pointer_ -= offset; return *this; }
  [[nodiscard]] constexpr vector_iterator operator+(difference_type offset) const noexcept { auto copy = *this; return copy += offset; }
  [[nodiscard]] constexpr vector_iterator operator-(difference_type offset) const noexcept { auto copy = *this; return copy -= offset; }
  [[nodiscard]] constexpr difference_type operator-(const base& other) const noexcept { return this->pointer_ - other.pointer_; }
  [[nodiscard]] constexpr reference operator[](difference_type offset) const noexcept { return this->pointer_[offset]; }
  friend constexpr vector_iterator operator+(difference_type offset, vector_iterator value) noexcept { return value += offset; }
};

template<class Vector>
struct pointer_traits<vector_const_iterator<Vector>, void> {
  using pointer = vector_const_iterator<Vector>;
  using element_type = const typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  [[nodiscard]] static constexpr element_type* to_address(const pointer& value) noexcept {
    return FTL_VECTOR_ITERATOR_NAMESPACE::to_address(value.pointer_);
  }
};

template<class Vector>
struct pointer_traits<vector_iterator<Vector>, void> {
  using pointer = vector_iterator<Vector>;
  using element_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  [[nodiscard]] static constexpr element_type* to_address(const pointer& value) noexcept {
    return FTL_VECTOR_ITERATOR_NAMESPACE::to_address(value.pointer_);
  }
};

FTL_END_NAMESPACE

#endif
