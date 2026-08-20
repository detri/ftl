#ifndef FTL_VECTOR_IMPL_HEADER
#define FTL_VECTOR_IMPL_HEADER

#ifdef FTL_REPLACE_STL
#include <__vector/iterator.hpp>
#include <algorithm>
#include <detail/rapidhash>
#include <initializer_list>
#include <limits>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>
#define FTL_VECTOR_NAMESPACE std
#else
#include <ftl/__vector/iterator.hpp>
#include <ftl/algorithm>
#include <ftl/detail/rapidhash>
#include <ftl/initializer_list>
#include <ftl/limits>
#include <ftl/ranges>
#include <ftl/stdexcept>
#include <ftl/type_traits>
#include <ftl/utility>
#define FTL_VECTOR_NAMESPACE ftl
#endif

#if defined(_MSC_VER) && !defined(__clang__)
extern "C" void __cdecl __fastfail(unsigned int);
#pragma intrinsic(__fastfail)
#define FTL_VECTOR_TRAP() __fastfail(7)
#else
#define FTL_VECTOR_TRAP() __builtin_trap()
#endif

FTL_BEGIN_NAMESPACE

namespace detail {
[[noreturn]] inline void vector_length_failure() {
#if FTL_HAS_EXCEPTIONS
  throw length_error("vector");
#else
  FTL_VECTOR_TRAP();
#endif
}

[[noreturn]] inline void vector_range_failure() {
#if FTL_HAS_EXCEPTIONS
  throw out_of_range("vector");
#else
  FTL_VECTOR_TRAP();
#endif
}

template <class Range, class T>
concept vector_compatible_range =
    ranges::input_range<Range> &&
    convertible_to<ranges::range_reference_t<Range>, T>;

template <class T> struct vector_compare {
  constexpr auto operator()(const T &left, const T &right) const {
    if constexpr (three_way_comparable<T>)
      return left <=> right;
    else {
      if (left < right)
        return weak_ordering::less;
      if (right < left)
        return weak_ordering::greater;
      return weak_ordering::equivalent;
    }
  }
};
} // namespace detail

template <class T, class Allocator = allocator<T>> class vector {
  using traits = allocator_traits<Allocator>;

public:
  static_assert(is_same_v<typename traits::value_type, T>);
  static_assert(is_object_v<T>);

  using value_type = T;
  using allocator_type = Allocator;
  using pointer = typename traits::pointer;
  using const_pointer = typename traits::const_pointer;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = typename traits::size_type;
  using difference_type = typename traits::difference_type;
  using iterator = vector_iterator<vector>;
  using const_iterator = vector_const_iterator<vector>;
  using reverse_iterator = FTL_VECTOR_NAMESPACE::reverse_iterator<iterator>;
  using const_reverse_iterator =
      FTL_VECTOR_NAMESPACE::reverse_iterator<const_iterator>;

  constexpr vector() noexcept(noexcept(Allocator())) : vector(Allocator()) {}
  constexpr explicit vector(const Allocator &allocator) noexcept
      : allocator_(allocator) {}
  constexpr explicit vector(size_type count,
                            const Allocator &allocator = Allocator())
      : allocator_(allocator) {
    append_default(count);
  }
  constexpr vector(size_type count, const T &value,
                   const Allocator &allocator = Allocator())
      : allocator_(allocator) {
    append_fill(count, value);
  }
  template <input_iterator InputIterator>
  constexpr vector(InputIterator first, InputIterator last,
                   const Allocator &allocator = Allocator())
      : allocator_(allocator) {
    append_iterators(first, last);
  }
  template <detail::vector_compatible_range<T> Range>
  constexpr vector(from_range_t, Range &&range,
                   const Allocator &allocator = Allocator())
      : allocator_(allocator) {
    append_range(static_cast<Range &&>(range));
  }
  constexpr vector(const vector &other)
      : allocator_(
            traits::select_on_container_copy_construction(other.allocator_)) {
    append_iterators(other.begin(), other.end());
  }
  constexpr vector(vector &&other) noexcept
      : allocator_(move(other.allocator_)) {
    steal(other);
  }
  constexpr vector(const vector &other,
                   const type_identity_t<Allocator> &allocator)
      : allocator_(allocator) {
    append_iterators(other.begin(), other.end());
  }
  constexpr vector(vector &&other, const type_identity_t<Allocator> &allocator)
      : allocator_(allocator) {
    if (allocator_ == other.allocator_)
      steal(other);
    else
      append_iterators(make_move_iterator(other.begin()),
                       make_move_iterator(other.end()));
  }
  constexpr vector(initializer_list<T> values,
                   const Allocator &allocator = Allocator())
      : allocator_(allocator) {
    append_iterators(values.begin(), values.end());
  }
  constexpr ~vector() { release(); }

  constexpr vector &operator=(const vector &other) {
    if (this == &other)
      return *this;
    if constexpr (traits::propagate_on_container_copy_assignment::value) {
      if (!(allocator_ == other.allocator_)) {
        release();
        allocator_ = other.allocator_;
      }
    }
    assign(other.begin(), other.end());
    return *this;
  }
  constexpr vector &operator=(vector &&other) noexcept(
      traits::propagate_on_container_move_assignment::value ||
      traits::is_always_equal::value) {
    if (this == &other)
      return *this;
    if constexpr (traits::propagate_on_container_move_assignment::value) {
      release();
      allocator_ = move(other.allocator_);
      steal(other);
    } else if (allocator_ == other.allocator_) {
      release();
      steal(other);
    } else {
      assign(make_move_iterator(other.begin()),
             make_move_iterator(other.end()));
      other.clear();
    }
    return *this;
  }
  constexpr vector &operator=(initializer_list<T> values) {
    assign(values);
    return *this;
  }

  template <input_iterator InputIterator>
  constexpr void assign(InputIterator first, InputIterator last) {
    vector replacement(first, last, allocator_);
    swap_storage(replacement);
  }
  template <detail::vector_compatible_range<T> Range>
  constexpr void assign_range(Range &&range) {
    vector replacement(from_range, static_cast<Range &&>(range), allocator_);
    swap_storage(replacement);
  }
  constexpr void assign(size_type count, const T &value) {
    vector replacement(count, value, allocator_);
    swap_storage(replacement);
  }
  constexpr void assign(initializer_list<T> values) {
    assign(values.begin(), values.end());
  }
  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept {
    return allocator_;
  }

  [[nodiscard]] constexpr iterator begin() noexcept { return iterator(first_); }
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator(first_);
  }
  [[nodiscard]] constexpr iterator end() noexcept { return iterator(last_); }
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return const_iterator(last_);
  }
  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  [[nodiscard]] constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return begin();
  }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
    return rend();
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    return first_ == last_;
  }
  [[nodiscard]] constexpr size_type size() const noexcept {
    return first_ ? static_cast<size_type>(last_ - first_) : 0;
  }
  [[nodiscard]] constexpr size_type max_size() const noexcept {
    const auto allocator_limit = traits::max_size(allocator_);
    const auto difference_limit =
        static_cast<size_type>((numeric_limits<difference_type>::max)());
    return allocator_limit < difference_limit ? allocator_limit
                                              : difference_limit;
  }
  [[nodiscard]] constexpr size_type capacity() const noexcept {
    return first_ ? static_cast<size_type>(end_ - first_) : 0;
  }
  constexpr void resize(size_type count) {
    if (count < size())
      erase(begin() + static_cast<difference_type>(count), end());
    else
      append_default(count - size());
  }
  constexpr void resize(size_type count, const T &value) {
    if (count < size())
      erase(begin() + static_cast<difference_type>(count), end());
    else
      append_fill(count - size(), value);
  }
  constexpr void reserve(size_type count) {
    if (count > capacity())
      reallocate_exact(count);
  }
  constexpr void shrink_to_fit() {
    if (size() != capacity())
      reallocate_exact(size());
  }

  [[nodiscard]] constexpr reference operator[](size_type index) {
    return first_[index];
  }
  [[nodiscard]] constexpr const_reference operator[](size_type index) const {
    return first_[index];
  }
  [[nodiscard]] constexpr reference at(size_type index) {
    if (index >= size())
      detail::vector_range_failure();
    return first_[index];
  }
  [[nodiscard]] constexpr const_reference at(size_type index) const {
    if (index >= size())
      detail::vector_range_failure();
    return first_[index];
  }
  [[nodiscard]] constexpr reference front() { return *first_; }
  [[nodiscard]] constexpr const_reference front() const { return *first_; }
  [[nodiscard]] constexpr reference back() { return last_[-1]; }
  [[nodiscard]] constexpr const_reference back() const { return last_[-1]; }
  [[nodiscard]] constexpr T *data() noexcept {
    return first_ ? to_address(first_) : nullptr;
  }
  [[nodiscard]] constexpr const T *data() const noexcept {
    return first_ ? to_address(first_) : nullptr;
  }

  template <class... Args> constexpr reference emplace_back(Args &&...args) {
    if (last_ == end_)
      return reallocate_emplace_back(forward<Args>(args)...);
    construct_value(last_, forward<Args>(args)...);
    ++last_;
    return back();
  }
  constexpr void push_back(const T &value) { emplace_back(value); }
  constexpr void push_back(T &&value) { emplace_back(move(value)); }
  template <detail::vector_compatible_range<T> Range>
  constexpr void append_range(Range &&range) {
    if constexpr (ranges::sized_range<Range>)
      reserve_for(static_cast<size_type>(ranges::size(range)));
    for (auto &&value : range)
      emplace_back(static_cast<decltype(value) &&>(value));
  }
  constexpr void pop_back() {
    --last_;
    destroy_value(last_);
  }

  template <class... Args>
  constexpr iterator emplace(const_iterator position, Args &&...args) {
    const size_type index = static_cast<size_type>(position - cbegin());
    T value(forward<Args>(args)...);
    if (last_ == end_)
      grow_for(1);
    pointer place = first_ + static_cast<difference_type>(index);
    if (place == last_) {
      construct_value(last_, move(value));
      ++last_;
    } else {
      construct_value(last_, move(last_[-1]));
      ++last_;
      for (pointer current = last_ - 2; current != place; --current)
        *current = move(current[-1]);
      *place = move(value);
    }
    return iterator(place);
  }
  constexpr iterator insert(const_iterator position, const T &value) {
    return emplace(position, value);
  }
  constexpr iterator insert(const_iterator position, T &&value) {
    return emplace(position, move(value));
  }
  constexpr iterator insert(const_iterator position, size_type count,
                            const T &value) {
    const size_type index = static_cast<size_type>(position - cbegin());
    vector inserted(count, value, allocator_);
    return insert_temporary(index, inserted);
  }
  template <input_iterator InputIterator>
  constexpr iterator insert(const_iterator position, InputIterator first,
                            InputIterator last) {
    const size_type index = static_cast<size_type>(position - cbegin());
    vector inserted(first, last, allocator_);
    return insert_temporary(index, inserted);
  }
  template <detail::vector_compatible_range<T> Range>
  constexpr iterator insert_range(const_iterator position, Range &&range) {
    const size_type index = static_cast<size_type>(position - cbegin());
    vector inserted(from_range, static_cast<Range &&>(range), allocator_);
    return insert_temporary(index, inserted);
  }
  constexpr iterator insert(const_iterator position,
                            initializer_list<T> values) {
    return insert(position, values.begin(), values.end());
  }
  constexpr iterator erase(const_iterator position) {
    return erase(position, position + 1);
  }
  constexpr iterator erase(const_iterator first, const_iterator last) {
    pointer output = first_ + (first - cbegin());
    pointer input = first_ + (last - cbegin());
    pointer result = output;
    while (input != last_)
      *output++ = move(*input++);
    while (last_ != output) {
      --last_;
      destroy_value(last_);
    }
    return iterator(result);
  }
  constexpr void
  swap(vector &other) noexcept(traits::propagate_on_container_swap::value ||
                               traits::is_always_equal::value) {
    if constexpr (traits::propagate_on_container_swap::value)
      FTL_VECTOR_NAMESPACE::swap(allocator_, other.allocator_);
    swap_storage(other);
  }
  constexpr void clear() noexcept {
    destroy(first_, last_);
    last_ = first_;
  }

private:
  constexpr void check_add(size_type count) const {
    if (count > max_size() - size())
      detail::vector_length_failure();
  }
  constexpr void reserve_for(size_type count) {
    check_add(count);
    if (count > capacity() - size())
      grow_for(count);
  }
  constexpr void grow_for(size_type count) {
    check_add(count);
    reallocate_exact(grown_capacity(count));
  }
  [[nodiscard]] constexpr size_type grown_capacity(size_type count) const {
    const size_type required = size() + count;
    size_type grown = capacity() + capacity() / 2;
    if (grown < required)
      grown = required;
    if (grown > max_size())
      grown = max_size();
    return grown;
  }
  template <class... Args>
  constexpr reference reallocate_emplace_back(Args &&...args) {
    check_add(1);
    const size_type old_size = size();
    const size_type new_capacity = grown_capacity(1);
    pointer replacement = traits::allocate(allocator_, new_capacity);
    pointer current = replacement;
    bool appended = false;
#if FTL_HAS_EXCEPTIONS
    try {
#endif
      // Construct the new element while references into the old vector remain
      // valid, then relocate the old range before committing the allocation.
      construct_value(replacement + static_cast<difference_type>(old_size),
                      forward<Args>(args)...);
      appended = true;
      for (pointer source = first_; source != last_; ++source, ++current)
        construct_value(current, move_if_noexcept(*source));
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      destroy(replacement, current);
      if (appended)
        destroy_value(replacement + static_cast<difference_type>(old_size));
      traits::deallocate(allocator_, replacement, new_capacity);
      throw;
    }
#endif
    release();
    first_ = replacement;
    last_ = replacement + static_cast<difference_type>(old_size + 1);
    end_ = replacement + static_cast<difference_type>(new_capacity);
    return back();
  }
  constexpr void reallocate_exact(size_type count) {
    if (count > max_size())
      detail::vector_length_failure();
    pointer replacement{};
    if (count)
      replacement = traits::allocate(allocator_, count);
    pointer current = replacement;
#if FTL_HAS_EXCEPTIONS
    try {
#endif
      for (pointer source = first_; source != last_; ++source, ++current)
        construct_value(current, move_if_noexcept(*source));
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      destroy(replacement, current);
      if (replacement)
        traits::deallocate(allocator_, replacement, count);
      throw;
    }
#endif
    const size_type old_size = size();
    release();
    first_ = replacement;
    last_ = replacement + static_cast<difference_type>(old_size);
    end_ = replacement + static_cast<difference_type>(count);
  }
  constexpr void append_default(size_type count) {
    if (count > capacity() - size()) {
      reallocate_append(count, [](vector &owner, pointer place) {
        owner.construct_value(place);
      });
      return;
    }
    const bool owned_before = first_ != pointer{};
    reserve_for(count);
    pointer original = last_;
#if FTL_HAS_EXCEPTIONS
    try {
#endif
      while (count--) {
        construct_value(last_);
        ++last_;
      }
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      destroy(original, last_);
      last_ = original;
      if (!owned_before)
        release();
      throw;
    }
#endif
  }
  constexpr void append_fill(size_type count, const T &value) {
    if (count > capacity() - size()) {
      reallocate_append(count, [&value](vector &owner, pointer place) {
        owner.construct_value(place, value);
      });
      return;
    }
    const bool owned_before = first_ != pointer{};
    reserve_for(count);
    pointer original = last_;
#if FTL_HAS_EXCEPTIONS
    try {
#endif
      while (count--) {
        construct_value(last_, value);
        ++last_;
      }
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      destroy(original, last_);
      last_ = original;
      if (!owned_before)
        release();
      throw;
    }
#endif
  }
  template <class Construct>
  constexpr void reallocate_append(size_type count, Construct construct) {
    check_add(count);
    if (count == 0)
      return;
    const size_type old_size = size();
    const size_type new_capacity = grown_capacity(count);
    pointer replacement = traits::allocate(allocator_, new_capacity);
    pointer old_current = replacement;
    pointer appended_first = replacement + static_cast<difference_type>(old_size);
    pointer appended_current = appended_first;
#if FTL_HAS_EXCEPTIONS
    try {
#endif
      // Build the appended range first so a fill value that aliases an
      // existing element remains valid throughout its construction.
      for (size_type index = 0; index < count; ++index, ++appended_current)
        construct(*this, appended_current);
      for (pointer source = first_; source != last_; ++source, ++old_current)
        construct_value(old_current, move_if_noexcept(*source));
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      destroy(replacement, old_current);
      destroy(appended_first, appended_current);
      traits::deallocate(allocator_, replacement, new_capacity);
      throw;
    }
#endif
    release();
    first_ = replacement;
    last_ = replacement + static_cast<difference_type>(old_size + count);
    end_ = replacement + static_cast<difference_type>(new_capacity);
  }
  template <input_iterator InputIterator>
  constexpr void append_iterators(InputIterator first, InputIterator last) {
    const bool owned_before = first_ != pointer{};
#if FTL_HAS_EXCEPTIONS
    try {
#endif
      if constexpr (forward_iterator<InputIterator>)
        reserve_for(static_cast<size_type>(
            FTL_VECTOR_NAMESPACE::distance(first, last)));
      for (; first != last; ++first)
        emplace_back(*first);
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
      if (!owned_before)
        release();
      throw;
    }
#endif
  }
  constexpr iterator insert_temporary(size_type index, vector &inserted) {
    if (inserted.empty())
      return begin() + static_cast<difference_type>(index);
    const size_type count = inserted.size();
    if (count <= capacity() - size()) {
      pointer place = first_ + static_cast<difference_type>(index);
      const size_type tail = static_cast<size_type>(last_ - place);
      const pointer old_last = last_;
      if (count <= tail) {
        for (pointer source = old_last - static_cast<difference_type>(count); source != old_last; ++source) {
          construct_value(last_, move(*source));
          ++last_;
        }
        for (pointer source = old_last - static_cast<difference_type>(count); source != place;) {
          --source;
          source[static_cast<difference_type>(count)] = move(*source);
        }
        for (size_type current = 0; current < count; ++current)
          place[static_cast<difference_type>(current)] = move(inserted[current]);
      } else {
        const size_type extra = count - tail;
        for (size_type current = tail; current < count; ++current) {
          construct_value(last_, move(inserted[current]));
          ++last_;
        }
        for (pointer source = place; source != place + static_cast<difference_type>(tail); ++source) {
          construct_value(last_, move(*source));
          ++last_;
        }
        for (size_type current = 0; current < tail; ++current)
          place[static_cast<difference_type>(current)] = move(inserted[current]);
      }
      return iterator(place);
    }
    vector replacement(allocator_);
    replacement.reserve(size() + inserted.size());
    replacement.append_iterators(
        make_move_iterator(begin()),
        make_move_iterator(begin() + static_cast<difference_type>(index)));
    replacement.append_iterators(make_move_iterator(inserted.begin()),
                                 make_move_iterator(inserted.end()));
    replacement.append_iterators(
        make_move_iterator(begin() + static_cast<difference_type>(index)),
        make_move_iterator(end()));
    swap_storage(replacement);
    return begin() + static_cast<difference_type>(index);
  }
  constexpr void destroy(pointer first, pointer last) noexcept {
    while (last != first) {
      --last;
      destroy_value(last);
    }
  }
  template<class... Args>
  constexpr void construct_value(pointer location, Args&&... args) {
#if defined(_MSC_VER) || !defined(FTL_REPLACE_STL)
    if consteval {
      if constexpr (is_same_v<Allocator, allocator<T>> &&
                    is_trivially_default_constructible_v<T>) {
        *location = T(forward<Args>(args)...);
        return;
      }
    }
#endif
    traits::construct(allocator_, to_address(location), forward<Args>(args)...);
  }
  constexpr void destroy_value(pointer location) noexcept {
#if defined(_MSC_VER) || !defined(FTL_REPLACE_STL)
    if consteval {
      if constexpr (is_same_v<Allocator, allocator<T>> &&
                    is_trivially_default_constructible_v<T>)
        return;
    }
#endif
    traits::destroy(allocator_, to_address(location));
  }
  constexpr void release() noexcept {
    if (!first_)
      return;
    const size_type old_capacity = capacity();
    destroy(first_, last_);
    traits::deallocate(allocator_, first_, old_capacity);
    first_ = last_ = end_ = pointer{};
  }
  constexpr void steal(vector &other) noexcept {
    first_ = other.first_;
    last_ = other.last_;
    end_ = other.end_;
    other.first_ = other.last_ = other.end_ = pointer{};
  }
  constexpr void swap_storage(vector &other) noexcept {
    FTL_VECTOR_NAMESPACE::swap(first_, other.first_);
    FTL_VECTOR_NAMESPACE::swap(last_, other.last_);
    FTL_VECTOR_NAMESPACE::swap(end_, other.end_);
  }

  FTL_NO_UNIQUE_ADDRESS Allocator allocator_;
  pointer first_{};
  pointer last_{};
  pointer end_{};
};

template <class Allocator> class vector<bool, Allocator> {
  using allocator_traits_type = allocator_traits<Allocator>;
  using block_type = unsigned char;
  using block_allocator =
      typename allocator_traits_type::template rebind_alloc<block_type>;
  using storage_type = vector<block_type, block_allocator>;
  static constexpr unsigned bits_per_block = 8;

public:
  static_assert(is_same_v<typename allocator_traits_type::value_type, bool>);
  using value_type = bool;
  using allocator_type = Allocator;
  using size_type = typename allocator_traits_type::size_type;
  using difference_type = typename allocator_traits_type::difference_type;

  class reference {
  public:
    using __ftl_vector_bool_reference = void;

    constexpr reference(const reference &) noexcept = default;
    constexpr ~reference() = default;
    constexpr operator bool() const noexcept { return (*block_ & mask_) != 0; }
    constexpr reference &operator=(bool value) noexcept {
      if (value)
        *block_ |= mask_;
      else
        *block_ &= static_cast<block_type>(~mask_);
      return *this;
    }
    constexpr reference &operator=(const reference &other) noexcept {
      return *this = static_cast<bool>(other);
    }
    constexpr const reference &operator=(bool value) const noexcept {
      if (value)
        *block_ |= mask_;
      else
        *block_ &= static_cast<block_type>(~mask_);
      return *this;
    }
    constexpr void flip() noexcept { *block_ ^= mask_; }
    friend constexpr void swap(reference left, reference right) noexcept {
      const bool value = left;
      left = right;
      right = value;
    }
    friend constexpr void swap(reference left, bool &right) noexcept {
      const bool value = left;
      left = right;
      right = value;
    }
    friend constexpr void swap(bool &left, reference right) noexcept {
      swap(right, left);
    }

  private:
    constexpr reference() noexcept : block_(nullptr), mask_(0) {}
    constexpr reference(block_type *block, block_type mask)
        : block_(block), mask_(mask) {}
    block_type *block_;
    block_type mask_;
    friend class vector;
  };

  using const_reference = bool;

  template <bool Constant> class bit_iterator {
    using owner_type = conditional_t<Constant, const vector, vector>;

  public:
    using iterator_concept = random_access_iterator_tag;
    using iterator_category = random_access_iterator_tag;
    using value_type = bool;
    using difference_type = typename vector::difference_type;
    using pointer = void;
    using reference = conditional_t<Constant, bool, typename vector::reference>;
    constexpr bit_iterator() = default;
    template <bool Other>
      requires(Constant && !Other)
    constexpr bit_iterator(const bit_iterator<Other> &other)
        : owner_(other.owner_), index_(other.index_) {}
    constexpr reference operator*() const { return (*owner_)[index_]; }
    constexpr bit_iterator &operator++() {
      ++index_;
      return *this;
    }
    constexpr bit_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }
    constexpr bit_iterator &operator--() {
      --index_;
      return *this;
    }
    constexpr bit_iterator operator--(int) {
      auto copy = *this;
      --*this;
      return copy;
    }
    constexpr bit_iterator &operator+=(difference_type offset) {
      index_ =
          static_cast<size_type>(static_cast<difference_type>(index_) + offset);
      return *this;
    }
    constexpr bit_iterator &operator-=(difference_type offset) {
      return *this += -offset;
    }
    constexpr bit_iterator operator+(difference_type offset) const {
      auto copy = *this;
      return copy += offset;
    }
    constexpr bit_iterator operator-(difference_type offset) const {
      auto copy = *this;
      return copy -= offset;
    }
    constexpr reference operator[](difference_type offset) const {
      return *(*this + offset);
    }
    template <bool Other>
    constexpr difference_type
    operator-(const bit_iterator<Other> &other) const {
      return static_cast<difference_type>(index_) -
             static_cast<difference_type>(other.index_);
    }
    template <bool Other>
    constexpr bool operator==(const bit_iterator<Other> &other) const {
      return index_ == other.index_;
    }
    template <bool Other>
    constexpr strong_ordering
    operator<=>(const bit_iterator<Other> &other) const {
      return index_ <=> other.index_;
    }
    friend constexpr bit_iterator operator+(difference_type offset,
                                            bit_iterator value) {
      return value += offset;
    }

  private:
    constexpr bit_iterator(owner_type *owner, size_type index)
        : owner_(owner), index_(index) {}
    owner_type *owner_{};
    size_type index_{};
    template <bool> friend class bit_iterator;
    friend class vector;
  };

  using iterator = bit_iterator<false>;
  using const_iterator = bit_iterator<true>;
  using pointer = iterator;
  using const_pointer = const_iterator;
  using reverse_iterator = FTL_VECTOR_NAMESPACE::reverse_iterator<iterator>;
  using const_reverse_iterator =
      FTL_VECTOR_NAMESPACE::reverse_iterator<const_iterator>;

  constexpr vector() noexcept(noexcept(Allocator())) : vector(Allocator()) {}
  constexpr explicit vector(const Allocator &allocator) noexcept
      : storage_(block_allocator(allocator)) {}
  constexpr explicit vector(size_type count,
                            const Allocator &allocator = Allocator())
      : vector(count, false, allocator) {}
  constexpr vector(size_type count, const bool &value,
                   const Allocator &allocator = Allocator())
      : vector(allocator) {
    assign(count, value);
  }
  template <input_iterator InputIterator>
  constexpr vector(InputIterator first, InputIterator last,
                   const Allocator &allocator = Allocator())
      : vector(allocator) {
    append_iterators(first, last);
  }
  template <detail::vector_compatible_range<bool> Range>
  constexpr vector(from_range_t, Range &&range,
                   const Allocator &allocator = Allocator())
      : vector(allocator) {
    append_range(static_cast<Range &&>(range));
  }
  constexpr vector(const vector &) = default;
  constexpr vector(vector &&other) noexcept
      : storage_(move(other.storage_)), size_(exchange(other.size_, 0)) {}
  constexpr vector(const vector &other,
                   const type_identity_t<Allocator> &allocator)
      : vector(other.begin(), other.end(), allocator) {}
  constexpr vector(vector &&other, const type_identity_t<Allocator> &allocator)
      : vector(allocator) {
    if (get_allocator() == other.get_allocator()) {
      storage_ = move(other.storage_);
      size_ = exchange(other.size_, 0);
    } else {
      append_iterators(other.begin(), other.end());
      other.clear();
    }
  }
  constexpr vector(initializer_list<bool> values,
                   const Allocator &allocator = Allocator())
      : vector(values.begin(), values.end(), allocator) {}
  constexpr ~vector() = default;
  constexpr vector &operator=(const vector &) = default;
  constexpr vector &operator=(vector &&other) noexcept(
      allocator_traits_type::propagate_on_container_move_assignment::value ||
      allocator_traits_type::is_always_equal::value) {
    storage_ = move(other.storage_);
    size_ = exchange(other.size_, 0);
    return *this;
  }
  constexpr vector &operator=(initializer_list<bool> values) {
    assign(values);
    return *this;
  }

  template <input_iterator InputIterator>
  constexpr void assign(InputIterator first, InputIterator last) {
    vector replacement(first, last, get_allocator());
    swap(replacement);
  }
  template <detail::vector_compatible_range<bool> Range>
  constexpr void assign_range(Range &&range) {
    vector replacement(from_range, static_cast<Range &&>(range),
                       get_allocator());
    swap(replacement);
  }
  constexpr void assign(size_type count, const bool &value) {
    if (count > max_size())
      detail::vector_length_failure();
    storage_.assign(block_count(count),
                    value ? block_type(~0u) : block_type(0));
    size_ = count;
    trim();
  }
  constexpr void assign(initializer_list<bool> values) {
    assign(values.begin(), values.end());
  }
  [[nodiscard]] constexpr allocator_type get_allocator() const noexcept {
    return allocator_type(storage_.get_allocator());
  }

  [[nodiscard]] constexpr iterator begin() noexcept {
    return iterator(this, 0);
  }
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return const_iterator(this, 0);
  }
  [[nodiscard]] constexpr iterator end() noexcept {
    return iterator(this, size_);
  }
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return const_iterator(this, size_);
  }
  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  [[nodiscard]] constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
    return begin();
  }
  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
    return rbegin();
  }
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
    return rend();
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return size_ == 0; }
  [[nodiscard]] constexpr size_type size() const noexcept { return size_; }
  [[nodiscard]] constexpr size_type max_size() const noexcept {
    const auto blocks = storage_.max_size();
    const auto limit = (numeric_limits<size_type>::max)() / bits_per_block;
    return (blocks < limit ? blocks : limit) * bits_per_block;
  }
  [[nodiscard]] constexpr size_type capacity() const noexcept {
    return storage_.capacity() * bits_per_block;
  }
  constexpr void resize(size_type count, bool value = false) {
    if (count < size_) {
      size_ = count;
      storage_.resize(block_count(count));
      trim();
    } else
      while (size_ < count)
        push_back(value);
  }
  constexpr void reserve(size_type count) {
    if (count > max_size())
      detail::vector_length_failure();
    storage_.reserve(block_count(count));
  }
  constexpr void shrink_to_fit() {
    storage_.resize(block_count(size_));
    storage_.shrink_to_fit();
  }

  [[nodiscard]] constexpr reference operator[](size_type index) {
    return reference(storage_.data() + index / bits_per_block, mask(index));
  }
  [[nodiscard]] constexpr bool operator[](size_type index) const {
    return (storage_[index / bits_per_block] & mask(index)) != 0;
  }
  [[nodiscard]] constexpr reference at(size_type index) {
    if (index >= size_)
      detail::vector_range_failure();
    return (*this)[index];
  }
  [[nodiscard]] constexpr bool at(size_type index) const {
    if (index >= size_)
      detail::vector_range_failure();
    return (*this)[index];
  }
  [[nodiscard]] constexpr reference front() { return (*this)[0]; }
  [[nodiscard]] constexpr bool front() const { return (*this)[0]; }
  [[nodiscard]] constexpr reference back() { return (*this)[size_ - 1]; }
  [[nodiscard]] constexpr bool back() const { return (*this)[size_ - 1]; }

  template <class... Args> constexpr reference emplace_back(Args &&...args) {
    push_back(bool(forward<Args>(args)...));
    return back();
  }
  constexpr void push_back(const bool &value) {
    if (size_ == max_size())
      detail::vector_length_failure();
    if (size_ == storage_.size() * bits_per_block)
      storage_.push_back(0);
    (*this)[size_++] = value;
  }
  template <detail::vector_compatible_range<bool> Range>
  constexpr void append_range(Range &&range) {
    for (auto &&value : range)
      push_back(static_cast<bool>(value));
  }
  constexpr void pop_back() {
    --size_;
    if (block_count(size_) < storage_.size())
      storage_.pop_back();
    else
      trim();
  }
  template <class... Args>
  constexpr iterator emplace(const_iterator position, Args &&...args) {
    return insert(position, bool(forward<Args>(args)...));
  }
  constexpr iterator insert(const_iterator position, const bool &value) {
    const auto index = static_cast<size_type>(position - cbegin());
    push_back(false);
    for (size_type current = size_ - 1; current > index; --current)
      (*this)[current] = (*this)[current - 1];
    (*this)[index] = value;
    return begin() + static_cast<difference_type>(index);
  }
  constexpr iterator insert(const_iterator position, size_type count,
                            const bool &value) {
    const auto index = static_cast<size_type>(position - cbegin());
    vector inserted(count, value, get_allocator());
    return insert_values(index, inserted);
  }
  template <input_iterator InputIterator>
  constexpr iterator insert(const_iterator position, InputIterator first,
                            InputIterator last) {
    const auto index = static_cast<size_type>(position - cbegin());
    vector inserted(first, last, get_allocator());
    return insert_values(index, inserted);
  }
  template <detail::vector_compatible_range<bool> Range>
  constexpr iterator insert_range(const_iterator position, Range &&range) {
    const auto index = static_cast<size_type>(position - cbegin());
    vector inserted(from_range, static_cast<Range &&>(range), get_allocator());
    return insert_values(index, inserted);
  }
  constexpr iterator insert(const_iterator position,
                            initializer_list<bool> values) {
    return insert(position, values.begin(), values.end());
  }
  constexpr iterator erase(const_iterator position) {
    return erase(position, position + 1);
  }
  constexpr iterator erase(const_iterator first, const_iterator last) {
    const auto index = static_cast<size_type>(first - cbegin());
    const auto count = static_cast<size_type>(last - first);
    for (size_type current = index; current + count < size_; ++current)
      (*this)[current] = (*this)[current + count];
    resize(size_ - count);
    return begin() + static_cast<difference_type>(index);
  }
  constexpr void swap(vector &other) noexcept(
      allocator_traits_type::propagate_on_container_swap::value ||
      allocator_traits_type::is_always_equal::value) {
    storage_.swap(other.storage_);
    FTL_VECTOR_NAMESPACE::swap(size_, other.size_);
  }
  static constexpr void swap(reference left, reference right) noexcept {
    const bool value = left;
    left = right;
    right = value;
  }
  constexpr void flip() noexcept {
    for (auto &block : storage_)
      block = static_cast<block_type>(~block);
    trim();
  }
  constexpr void clear() noexcept {
    storage_.clear();
    size_ = 0;
  }

private:
  static constexpr size_type block_count(size_type bits) {
    return bits / bits_per_block + (bits % bits_per_block != 0);
  }
  static constexpr block_type mask(size_type index) {
    return static_cast<block_type>(1u << (index % bits_per_block));
  }
  constexpr void trim() noexcept {
    if (size_ % bits_per_block && !storage_.empty())
      storage_.back() &=
          static_cast<block_type>((1u << (size_ % bits_per_block)) - 1u);
  }
  template <input_iterator InputIterator>
  constexpr void append_iterators(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
      push_back(static_cast<bool>(*first));
  }
  constexpr iterator insert_values(size_type index, const vector &inserted) {
    if (inserted.empty())
      return begin() + static_cast<difference_type>(index);
    vector replacement(get_allocator());
    replacement.reserve(size_ + inserted.size());
    for (size_type i = 0; i < index; ++i)
      replacement.push_back((*this)[i]);
    replacement.append_iterators(inserted.begin(), inserted.end());
    for (size_type i = index; i < size_; ++i)
      replacement.push_back((*this)[i]);
    swap(replacement);
    return begin() + static_cast<difference_type>(index);
  }

  storage_type storage_;
  size_type size_{};

  friend struct hash<vector<bool, Allocator>>;
};

template <class InputIterator,
          class Allocator =
              allocator<typename iterator_traits<InputIterator>::value_type>>
vector(InputIterator, InputIterator, Allocator = Allocator())
    -> vector<typename iterator_traits<InputIterator>::value_type, Allocator>;
template <ranges::input_range Range,
          class Allocator = allocator<ranges::range_value_t<Range>>>
vector(from_range_t, Range &&, Allocator = Allocator())
    -> vector<ranges::range_value_t<Range>, Allocator>;

template <class T, class Allocator>
[[nodiscard]] constexpr bool operator==(const vector<T, Allocator> &left,
                                        const vector<T, Allocator> &right) {
  return left.size() == right.size() &&
         FTL_VECTOR_NAMESPACE::equal(left.begin(), left.end(), right.begin());
}
template <class T, class Allocator>
[[nodiscard]] constexpr auto operator<=>(const vector<T, Allocator> &left,
                                         const vector<T, Allocator> &right) {
  return lexicographical_compare_three_way(left.begin(), left.end(),
                                           right.begin(), right.end(),
                                           detail::vector_compare<T>{});
}
template <class T, class Allocator>
constexpr void
swap(vector<T, Allocator> &left,
     vector<T, Allocator> &right) noexcept(noexcept(left.swap(right))) {
  left.swap(right);
}
template <class T, class Allocator, class U>
constexpr typename vector<T, Allocator>::size_type
erase(vector<T, Allocator> &values, const U &value) {
  auto result = remove(values.begin(), values.end(), value);
  auto count = static_cast<typename vector<T, Allocator>::size_type>(
      values.end() - result);
  values.erase(result, values.end());
  return count;
}
template <class T, class Allocator, class Predicate>
constexpr typename vector<T, Allocator>::size_type
erase_if(vector<T, Allocator> &values, Predicate predicate) {
  auto result = remove_if(values.begin(), values.end(), predicate);
  auto count = static_cast<typename vector<T, Allocator>::size_type>(
      values.end() - result);
  values.erase(result, values.end());
  return count;
}

template <class Allocator> struct hash<vector<bool, Allocator>> {
  constexpr size_t
  operator()(const vector<bool, Allocator> &values) const noexcept {
    return static_cast<size_t>(ftl_rapidhash::rapidhash_with_seed(
        values.storage_.data(),
        values.storage_.size() * sizeof(typename vector<bool, Allocator>::block_type),
        values.size_));
  }
};

FTL_END_NAMESPACE

#endif
