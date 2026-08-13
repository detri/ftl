#ifndef FTL_ASSOCIATIVE_HASH_TABLE_HPP
#define FTL_ASSOCIATIVE_HASH_TABLE_HPP

FTL_BEGIN_NAMESPACE
namespace detail {

template <class Value> struct hash_table_node {
  hash_table_node *next{};
  hash_table_node *previous{};
  hash_table_node *bucket_next{};
  size_t hash{};
  Value value;
  template <class... Args>
  explicit hash_table_node(Args &&...args) : value(forward<Args>(args)...) {}
};

template <class Value, class Key, class Allocator> class hash_node_handle {
  using node = hash_table_node<Value>;
  using value_traits = allocator_traits<Allocator>;
  using node_allocator = typename value_traits::template rebind_alloc<node>;
  using node_traits = allocator_traits<node_allocator>;
  node *value_{};
  optional<Allocator> allocator_;
  template <class, class, class, class, class, class, bool>
  friend class hash_table;
  hash_node_handle(node *value, const Allocator &allocator)
      : value_(value), allocator_(in_place, allocator) {}
public:
  using allocator_type = Allocator;
  using value_type = Value;
  hash_node_handle() noexcept = default;
  hash_node_handle(hash_node_handle &&other) noexcept
      : value_(exchange(other.value_, nullptr)), allocator_(move(other.allocator_)) {
    other.allocator_.reset();
  }
  hash_node_handle &operator=(hash_node_handle &&other) noexcept {
    if (this != &other) {
      reset();
      value_ = exchange(other.value_, nullptr);
      if (!allocator_ || value_traits::propagate_on_container_move_assignment::value)
        allocator_ = move(other.allocator_);
      other.allocator_.reset();
    }
    return *this;
  }
  hash_node_handle(const hash_node_handle &) = delete;
  hash_node_handle &operator=(const hash_node_handle &) = delete;
  ~hash_node_handle() { reset(); }
  [[nodiscard]] bool empty() const noexcept { return !value_; }
  explicit operator bool() const noexcept { return value_ != nullptr; }
  allocator_type get_allocator() const { return *allocator_; }
  value_type &value() const { return value_->value; }
  auto &key() const requires requires(Value &item) { item.first; } {
    return const_cast<Key &>(value_->value.first);
  }
  auto &mapped() const requires requires(Value &item) { item.second; } {
    return value_->value.second;
  }
  void swap(hash_node_handle &other) noexcept(
      value_traits::propagate_on_container_swap::value ||
      value_traits::is_always_equal::value) {
    FTL_ASSOCIATIVE_NAMESPACE::swap(value_, other.value_);
    if constexpr (value_traits::propagate_on_container_swap::value)
      FTL_ASSOCIATIVE_NAMESPACE::swap(allocator_, other.allocator_);
  }
private:
  void reset() noexcept {
    if (!value_) return;
    node_allocator allocator(*allocator_);
    auto pointer = pointer_traits<typename node_traits::pointer>::pointer_to(*value_);
    node_traits::destroy(allocator, value_);
    node_traits::deallocate(allocator, pointer, 1);
    value_ = nullptr;
    allocator_.reset();
  }
};

template <class Value, class Key, class KeyOfValue, class Hash, class Equal,
          class Allocator, bool Multi>
class hash_table {
  using node = hash_table_node<Value>;
  template <class, class, class, class, class, class, bool>
  friend class hash_table;
  using value_traits = allocator_traits<Allocator>;
  using node_allocator = typename value_traits::template rebind_alloc<node>;
  using node_traits = allocator_traits<node_allocator>;
  using bucket_allocator = typename value_traits::template rebind_alloc<node *>;

public:
  using value_type = Value;
  using key_type = Key;
  using allocator_type = Allocator;
  using size_type = typename value_traits::size_type;
  using difference_type = typename value_traits::difference_type;

  template <bool Constant> class basic_iterator {
    node *current_{};
    explicit basic_iterator(node *current) : current_(current) {}
    friend class hash_table;
    template <bool> friend class basic_iterator;

  public:
    using iterator_concept = forward_iterator_tag;
    using iterator_category = forward_iterator_tag;
    using value_type = Value;
    using difference_type = typename hash_table::difference_type;
    using reference = conditional_t<Constant, const Value &, Value &>;
    using pointer = conditional_t<Constant, const Value *, Value *>;
    basic_iterator() = default;
    template <bool Other>
      requires(Constant && !Other)
    basic_iterator(const basic_iterator<Other> &other)
        : current_(other.current_) {}
    reference operator*() const { return current_->value; }
    pointer operator->() const { return addressof(current_->value); }
    basic_iterator &operator++() {
      current_ = current_->next;
      return *this;
    }
    basic_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }
    template <bool Other>
    bool operator==(const basic_iterator<Other> &other) const {
      return current_ == other.current_;
    }
  };

  template <bool Constant> class basic_local_iterator {
    node *current_{};
    explicit basic_local_iterator(node *current) : current_(current) {}
    friend class hash_table;
    template <bool> friend class basic_local_iterator;

  public:
    using iterator_concept = forward_iterator_tag;
    using iterator_category = forward_iterator_tag;
    using value_type = Value;
    using difference_type = typename hash_table::difference_type;
    using reference = conditional_t<Constant, const Value &, Value &>;
    using pointer = conditional_t<Constant, const Value *, Value *>;
    basic_local_iterator() = default;
    template <bool Other>
      requires(Constant && !Other)
    basic_local_iterator(const basic_local_iterator<Other> &other)
        : current_(other.current_) {}
    reference operator*() const { return current_->value; }
    pointer operator->() const { return addressof(current_->value); }
    basic_local_iterator &operator++() {
      current_ = current_->bucket_next;
      return *this;
    }
    basic_local_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }
    template <bool Other>
    bool operator==(const basic_local_iterator<Other> &other) const {
      return current_ == other.current_;
    }
  };

  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;
  using local_iterator = basic_local_iterator<false>;
  using const_local_iterator = basic_local_iterator<true>;

  using node_type = hash_node_handle<Value, Key, Allocator>;

  struct insert_return_type {
    iterator position;
    bool inserted;
    node_type node;
  };

  hash_table() : hash_table(0) {}
  explicit hash_table(size_type count, const Hash &hash = Hash(),
                      const Equal &equal = Equal(),
                      const Allocator &allocator = Allocator())
      : buckets_(bucket_allocator(allocator)), hash_(hash), equal_(equal),
        allocator_(allocator) {
    rehash(count);
  }
  explicit hash_table(const Allocator &allocator)
      : hash_table(0, Hash(), Equal(), allocator) {}
  hash_table(size_type count, const Allocator &allocator)
      : hash_table(count, Hash(), Equal(), allocator) {}
  hash_table(size_type count, const Hash &hash, const Allocator &allocator)
      : hash_table(count, hash, Equal(), allocator) {}
  hash_table(const hash_table &other)
      : hash_table(other.bucket_count(), other.hash_, other.equal_,
                   value_traits::select_on_container_copy_construction(
                       other.allocator_)) {
    initialize([&] { insert(other.begin(), other.end()); });
  }
  hash_table(const hash_table &other, const Allocator &allocator)
      : hash_table(other.bucket_count(), other.hash_, other.equal_, allocator) {
    initialize([&] { insert(other.begin(), other.end()); });
  }
  hash_table(hash_table &&other) noexcept
      : buckets_(move(other.buckets_)), first_(exchange(other.first_, nullptr)),
        last_(exchange(other.last_, nullptr)), size_(exchange(other.size_, 0)),
        max_load_(other.max_load_), hash_(move(other.hash_)),
        equal_(move(other.equal_)), allocator_(move(other.allocator_)) {}
  hash_table(hash_table &&other, const Allocator &allocator)
      : hash_table(other.bucket_count(), other.hash_, other.equal_, allocator) {
    max_load_ = other.max_load_;
    if (allocator_ == other.allocator_) {
      buckets_ = move(other.buckets_);
      first_ = exchange(other.first_, nullptr);
      last_ = exchange(other.last_, nullptr);
      size_ = exchange(other.size_, 0);
    } else {
      initialize([&] { insert(make_move_iterator(other.begin()),
                              make_move_iterator(other.end())); });
      other.clear();
    }
  }
  ~hash_table() { clear(); }
  hash_table &operator=(const hash_table &other) {
    if (this == &other)
      return *this;
    if constexpr (value_traits::propagate_on_container_copy_assignment::value) {
      if (!(allocator_ == other.allocator_))
        clear();
      allocator_ = other.allocator_;
    }
    hash_ = other.hash_;
    equal_ = other.equal_;
    max_load_ = other.max_load_;
    clear();
    rehash(other.bucket_count());
    insert(other.begin(), other.end());
    return *this;
  }
  hash_table &
  operator=(hash_table &&other) noexcept(
      value_traits::is_always_equal::value &&
      is_nothrow_move_assignable_v<Hash> &&
      is_nothrow_move_assignable_v<Equal>) {
    if (this == &other)
      return *this;
    if constexpr (value_traits::propagate_on_container_move_assignment::value) {
      clear();
      allocator_ = move(other.allocator_);
      buckets_ = move(other.buckets_);
      first_ = exchange(other.first_, nullptr);
      last_ = exchange(other.last_, nullptr);
      size_ = exchange(other.size_, 0);
      hash_ = move(other.hash_);
      equal_ = move(other.equal_);
      max_load_ = other.max_load_;
    } else if (allocator_ == other.allocator_) {
      clear();
      buckets_ = move(other.buckets_);
      first_ = exchange(other.first_, nullptr);
      last_ = exchange(other.last_, nullptr);
      size_ = exchange(other.size_, 0);
      hash_ = move(other.hash_);
      equal_ = move(other.equal_);
      max_load_ = other.max_load_;
    } else {
      clear();
      insert(make_move_iterator(other.begin()),
             make_move_iterator(other.end()));
      other.clear();
    }
    return *this;
  }

  allocator_type get_allocator() const noexcept { return allocator_; }
  iterator begin() noexcept { return iterator(first_); }
  const_iterator begin() const noexcept { return const_iterator(first_); }
  iterator end() noexcept { return iterator(); }
  const_iterator end() const noexcept { return const_iterator(); }
  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept {
    node_allocator allocator(allocator_);
    return node_traits::max_size(allocator);
  }

  template <class V> auto insert(V &&value) {
    node *added = make_node(static_cast<V &&>(value));
#if FTL_HAS_EXCEPTIONS
    try { return insert_node(added); }
    catch (...) { destroy_node(added); throw; }
#else
    return insert_node(added);
#endif
  }
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    for (; first != last; ++first)
      insert(*first);
  }
  template <class... Args> auto emplace(Args &&...args) {
    node *added = make_node(forward<Args>(args)...);
#if FTL_HAS_EXCEPTIONS
    try { return insert_node(added); }
    catch (...) { destroy_node(added); throw; }
#else
    return insert_node(added);
#endif
  }
  iterator insert(const_iterator, const Value &value) {
    if constexpr (Multi)
      return insert(value);
    else
      return insert(value).first;
  }
  iterator insert(const_iterator, Value &&value) {
    if constexpr (Multi)
      return insert(move(value));
    else
      return insert(move(value)).first;
  }
  template <class... Args>
  iterator emplace_hint(const_iterator, Args &&...args) {
    if constexpr (Multi)
      return emplace(forward<Args>(args)...);
    else
      return emplace(forward<Args>(args)...).first;
  }
  auto insert(node_type &&handle) {
    if constexpr (Multi) {
      if (!handle)
        return end();
      auto result = insert_node(handle.value_, false);
      handle.value_ = nullptr;
      return result;
    } else {
      if (!handle)
        return insert_return_type{end(), false, {}};
      auto found = find(KeyOfValue{}(handle.value_->value));
      if (found != end())
        return insert_return_type{found, false, move(handle)};
      auto result = insert_node(handle.value_, false);
      handle.value_ = nullptr;
      return insert_return_type{result.first, true, {}};
    }
  }
  iterator insert(const_iterator, node_type &&handle) {
    if constexpr (Multi)
      return insert(move(handle));
    else
      return insert(move(handle)).position;
  }

  iterator erase(const_iterator position) {
    node *next = position.current_->next;
    node *removed = detach(position.current_);
    destroy_node(removed);
    return iterator(next);
  }
  iterator erase(const_iterator first, const_iterator last) {
    while (first != last)
      first = erase(first);
    return iterator(last.current_);
  }
  template <class K> size_type erase_key(const K &key) {
    auto range = equal_range(key);
    size_type count{};
    while (range.first != range.second) {
      range.first = erase(range.first);
      ++count;
    }
    return count;
  }
  void clear() noexcept {
    while (first_)
      destroy_node(detach(first_));
  }
  void swap(hash_table &other) noexcept(
      value_traits::is_always_equal::value && is_nothrow_swappable_v<Hash> &&
      is_nothrow_swappable_v<Equal>) {
    if constexpr (value_traits::propagate_on_container_swap::value)
      FTL_ASSOCIATIVE_NAMESPACE::swap(allocator_, other.allocator_);
    buckets_.swap(other.buckets_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(first_, other.first_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(last_, other.last_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(size_, other.size_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(max_load_, other.max_load_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(hash_, other.hash_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(equal_, other.equal_);
  }
  node_type extract(const_iterator position) {
    return node_type(detach(position.current_), allocator_);
  }
  template <class K> node_type extract(const K &key) {
    auto found = find(key);
    return found == end() ? node_type{}
                          : node_type(detach(found.current_), allocator_);
  }
  template <class OtherHash, class OtherEqual, bool OtherMulti>
  void merge(hash_table<Value, Key, KeyOfValue, OtherHash, OtherEqual,
                        Allocator, OtherMulti> &other) {
    if (static_cast<const void *>(this) == static_cast<const void *>(&other))
      return;
    for (auto current = other.begin(); current != other.end();) {
      auto candidate = current++;
      insertion_slot slot = prepare_insertion(KeyOfValue{}(*candidate));
      if constexpr (!Multi)
        if (slot.equivalent)
          continue;
      node *moved = other.detach_iterator(candidate);
      attach(moved, slot);
    }
  }

  template <class K> iterator find(const K &key) {
    return iterator(find_node(key));
  }
  template <class K> const_iterator find(const K &key) const {
    return const_iterator(find_node(key));
  }
  template <class K> size_type count(const K &key) const {
    auto range = equal_range(key);
    return static_cast<size_type>(distance(range.first, range.second));
  }
  template <class K> bool contains(const K &key) const {
    return find_node(key) != nullptr;
  }
  template <class K> pair<iterator, iterator> equal_range(const K &key) {
    node *first = find_node(key);
    if (!first)
      return {end(), end()};
    node *last = first->next;
    while (last && equal_(KeyOfValue{}(last->value), key))
      last = last->next;
    return {iterator(first), iterator(last)};
  }
  template <class K>
  pair<const_iterator, const_iterator> equal_range(const K &key) const {
    node *first = find_node(key);
    if (!first)
      return {end(), end()};
    node *last = first->next;
    while (last && equal_(KeyOfValue{}(last->value), key))
      last = last->next;
    return {const_iterator(first), const_iterator(last)};
  }

  local_iterator begin(size_type bucket) {
    return local_iterator(buckets_[bucket]);
  }
  const_local_iterator begin(size_type bucket) const {
    return const_local_iterator(buckets_[bucket]);
  }
  local_iterator end(size_type) { return local_iterator(); }
  const_local_iterator end(size_type) const { return const_local_iterator(); }
  const_local_iterator cbegin(size_type bucket) const { return begin(bucket); }
  const_local_iterator cend(size_type bucket) const { return end(bucket); }
  size_type bucket_count() const noexcept { return buckets_.size(); }
  size_type max_bucket_count() const noexcept { return buckets_.max_size(); }
  size_type bucket_size(size_type bucket) const {
    size_type count{};
    for (auto *p = buckets_[bucket]; p; p = p->bucket_next)
      ++count;
    return count;
  }
  template <class K> size_type bucket(const K &key) const {
    return bucket_count() ? bucket_for(hash_(key), bucket_count())
                          : 0;
  }
  float load_factor() const noexcept {
    return bucket_count()
               ? static_cast<float>(size_) / static_cast<float>(bucket_count())
               : 0.0f;
  }
  float max_load_factor() const noexcept { return max_load_; }
  void max_load_factor(float value) { max_load_ = value; }
  void rehash(size_type count) {
    const size_type minimum =
        static_cast<size_type>(static_cast<float>(size_) / max_load_) +
        (size_ ? 1 : 0);
    if (count < minimum)
      count = minimum;
    if (count == bucket_count())
      return;
    if (!count) {
      buckets_.clear();
      return;
    }
    vector<node *, bucket_allocator> replacement(count, nullptr,
                                                 bucket_allocator(allocator_));
    for (node *current = first_; current; current = current->next) {
      const size_type index = bucket_for(current->hash, count);
      current->bucket_next = replacement[index];
      replacement[index] = current;
    }
    buckets_.swap(replacement);
  }
  void reserve(size_type count) {
    rehash(static_cast<size_type>(static_cast<float>(count) / max_load_) + 1);
  }
  Hash hash_function() const { return hash_; }
  Equal key_eq() const { return equal_; }

private:
  struct insertion_slot {
    size_t hash{};
    size_type index{};
    node *equivalent{};
  };
  insertion_slot prepare_insertion(const Key &key) {
    insertion_slot result;
    result.hash = hash_(key);
    result.equivalent = find_node_hashed(key, result.hash);
    if constexpr (!Multi)
      if (result.equivalent)
        return result;
    ensure_capacity();
    result.index = bucket_for(result.hash, bucket_count());
    return result;
  }
  void attach(node *added, insertion_slot slot) noexcept {
    added->hash = slot.hash;
    if constexpr (Multi) {
      if (slot.equivalent) {
        added->next = slot.equivalent->next;
        added->previous = slot.equivalent;
        if (added->next) added->next->previous = added;
        else last_ = added;
        slot.equivalent->next = added;
        added->bucket_next = slot.equivalent->bucket_next;
        slot.equivalent->bucket_next = added;
        ++size_;
        return;
      }
    }
    added->previous = last_;
    added->next = nullptr;
    if (last_) last_->next = added;
    else first_ = added;
    last_ = added;
    added->bucket_next = buckets_[slot.index];
    buckets_[slot.index] = added;
    ++size_;
  }
  template <class Operation> void initialize(Operation operation) {
#if FTL_HAS_EXCEPTIONS
    try { operation(); }
    catch (...) { clear(); throw; }
#else
    operation();
#endif
  }
  template <class... Args> node *make_node(Args &&...args) {
    node_allocator allocator(allocator_);
    auto storage = node_traits::allocate(allocator, 1);
    node *result = to_address(storage);
#if FTL_HAS_EXCEPTIONS
    try {
      node_traits::construct(allocator, result, forward<Args>(args)...);
    } catch (...) {
      node_traits::deallocate(allocator, storage, 1);
      throw;
    }
#else
    node_traits::construct(allocator, result, forward<Args>(args)...);
#endif
    return result;
  }
  void destroy_node(node *value) noexcept {
    node_allocator allocator(allocator_);
    auto pointer =
        pointer_traits<typename node_traits::pointer>::pointer_to(*value);
    node_traits::destroy(allocator, value);
    node_traits::deallocate(allocator, pointer, 1);
  }
  auto insert_node(node *added, bool destroy_duplicate = true) {
    added->hash = hash_(KeyOfValue{}(added->value));
    if constexpr (!Multi) {
      if (node *found =
              find_node_hashed(KeyOfValue{}(added->value), added->hash)) {
        if (destroy_duplicate)
          destroy_node(added);
        return pair<iterator, bool>{iterator(found), false};
      }
    }
    ensure_capacity();
    const size_type index = bucket_for(added->hash, bucket_count());
    node *equivalent =
        Multi ? find_node_hashed(KeyOfValue{}(added->value), added->hash)
              : nullptr;
    if (equivalent) {
      added->next = equivalent->next;
      added->previous = equivalent;
      if (added->next)
        added->next->previous = added;
      else
        last_ = added;
      equivalent->next = added;
      added->bucket_next = equivalent->bucket_next;
      equivalent->bucket_next = added;
    } else {
      added->previous = last_;
      added->next = nullptr;
      if (last_)
        last_->next = added;
      else
        first_ = added;
      last_ = added;
      added->bucket_next = buckets_[index];
      buckets_[index] = added;
    }
    ++size_;
    if constexpr (Multi)
      return iterator(added);
    else
      return pair<iterator, bool>{iterator(added), true};
  }
  node *detach_iterator(iterator value) { return detach(value.current_); }
  void ensure_capacity() {
    if (!bucket_count() ||
        static_cast<float>(size_ + 1) > max_load_ * bucket_count())
      rehash(bucket_count() ? bucket_count() * 2 : 8);
  }
  node *detach(node *value) {
    const size_type index = bucket_for(value->hash, bucket_count());
    node **link = &buckets_[index];
    while (*link != value)
      link = &(*link)->bucket_next;
    *link = value->bucket_next;
    if (value->previous)
      value->previous->next = value->next;
    else
      first_ = value->next;
    if (value->next)
      value->next->previous = value->previous;
    else
      last_ = value->previous;
    value->next = value->previous = value->bucket_next = nullptr;
    --size_;
    return value;
  }
  template <class K> node *find_node(const K &key) const {
    return bucket_count() ? find_node_hashed(key, hash_(key)) : nullptr;
  }
  template <class K>
  node *find_node_hashed(const K &key, size_t hash_value) const {
    if (!bucket_count())
      return nullptr;
    for (node *current = buckets_[bucket_for(hash_value, bucket_count())]; current;
         current = current->bucket_next)
      if (current->hash == hash_value &&
          equal_(KeyOfValue{}(current->value), key))
        return current;
    return nullptr;
  }
  static size_type bucket_for(size_t hash_value, size_type count) noexcept {
    if constexpr (sizeof(size_t) >= 8) {
      hash_value ^= hash_value >> 33;
      hash_value *= static_cast<size_t>(0xff51afd7ed558ccdULL);
      hash_value ^= hash_value >> 33;
      hash_value *= static_cast<size_t>(0xc4ceb9fe1a85ec53ULL);
      hash_value ^= hash_value >> 33;
    } else {
      hash_value ^= hash_value >> 16;
      hash_value *= static_cast<size_t>(0x7feb352dU);
      hash_value ^= hash_value >> 15;
      hash_value *= static_cast<size_t>(0x846ca68bU);
      hash_value ^= hash_value >> 16;
    }
    return static_cast<size_type>(hash_value % count);
  }

  vector<node *, bucket_allocator> buckets_{};
  node *first_{};
  node *last_{};
  size_type size_{};
  float max_load_{1.0f};
  FTL_NO_UNIQUE_ADDRESS Hash hash_{};
  FTL_NO_UNIQUE_ADDRESS Equal equal_{};
  FTL_NO_UNIQUE_ADDRESS Allocator allocator_{};
};

} // namespace detail
FTL_END_NAMESPACE
#endif
