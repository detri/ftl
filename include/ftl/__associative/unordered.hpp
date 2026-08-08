#ifndef FTL_ASSOCIATIVE_UNORDERED_HPP
#define FTL_ASSOCIATIVE_UNORDERED_HPP

#ifdef FTL_REPLACE_STL
#include <__associative/hash_table.hpp>
#else
#include <ftl/__associative/hash_table.hpp>
#endif

FTL_BEGIN_NAMESPACE
namespace detail {
template <class Key> struct unordered_set_key {
  const Key &operator()(const Key &value) const noexcept { return value; }
};
template <class Key, class T> struct unordered_map_key {
  const Key &operator()(const pair<const Key, T> &value) const noexcept {
    return value.first;
  }
};

template <class Key, class T, class Hash, class Equal, class Allocator,
          bool Multi>
class unordered_map_base {
protected:
  using table_type =
      hash_table<pair<const Key, T>, Key, unordered_map_key<Key, T>, Hash,
                 Equal, Allocator, Multi>;
  table_type table_;

public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = pair<const Key, T>;
  using hasher = Hash;
  using key_equal = Equal;
  using allocator_type = Allocator;
  using pointer = typename allocator_traits<Allocator>::pointer;
  using const_pointer = typename allocator_traits<Allocator>::const_pointer;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = typename table_type::size_type;
  using difference_type = typename table_type::difference_type;
  using iterator = typename table_type::iterator;
  using const_iterator = typename table_type::const_iterator;
  using local_iterator = typename table_type::local_iterator;
  using const_local_iterator = typename table_type::const_local_iterator;
  using node_type = typename table_type::node_type;
  using insert_return_type = typename table_type::insert_return_type;

  unordered_map_base() = default;
  explicit unordered_map_base(size_type count, const Hash &hash = Hash(),
                              const Equal &equal = Equal(),
                              const Allocator &allocator = Allocator())
      : table_(count, hash, equal, allocator) {}
  explicit unordered_map_base(const Allocator &allocator) : table_(allocator) {}
  unordered_map_base(size_type count, const Allocator &allocator)
      : table_(count, allocator) {}
  unordered_map_base(size_type count, const Hash &hash,
                     const Allocator &allocator)
      : table_(count, hash, allocator) {}
  template <input_iterator I>
  unordered_map_base(I first, I last, size_type count = 0,
                     const Hash &hash = Hash(), const Equal &equal = Equal(),
                     const Allocator &allocator = Allocator())
      : table_(count, hash, equal, allocator) {
    insert(first, last);
  }
  template <ranges::input_range R>
  unordered_map_base(from_range_t, R &&range, size_type count = 0,
                     const Hash &hash = Hash(), const Equal &equal = Equal(),
                     const Allocator &allocator = Allocator())
      : table_(count, hash, equal, allocator) {
    insert_range(static_cast<R &&>(range));
  }
  unordered_map_base(initializer_list<value_type> values, size_type count = 0,
                     const Hash &hash = Hash(), const Equal &equal = Equal(),
                     const Allocator &allocator = Allocator())
      : unordered_map_base(values.begin(), values.end(), count, hash, equal,
                           allocator) {}
  unordered_map_base(const unordered_map_base &) = default;
  unordered_map_base(unordered_map_base &&) = default;
  unordered_map_base(const unordered_map_base &other,
                     const Allocator &allocator)
      : table_(other.table_, allocator) {}
  unordered_map_base(unordered_map_base &&other, const Allocator &allocator)
      : table_(move(other.table_), allocator) {}
  unordered_map_base &operator=(const unordered_map_base &) = default;
  unordered_map_base &operator=(unordered_map_base &&) = default;
  unordered_map_base &operator=(initializer_list<value_type> values) {
    clear();
    insert(values);
    return *this;
  }
  allocator_type get_allocator() const noexcept {
    return table_.get_allocator();
  }
  iterator begin() noexcept { return table_.begin(); }
  const_iterator begin() const noexcept { return table_.begin(); }
  iterator end() noexcept { return table_.end(); }
  const_iterator end() const noexcept { return table_.end(); }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  [[nodiscard]] bool empty() const noexcept { return table_.empty(); }
  size_type size() const noexcept { return table_.size(); }
  size_type max_size() const noexcept { return table_.max_size(); }
  template <class... Args> auto emplace(Args &&...args) {
    return table_.emplace(forward<Args>(args)...);
  }
  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args) {
    return table_.emplace_hint(hint, forward<Args>(args)...);
  }
  auto insert(const value_type &value) { return table_.insert(value); }
  auto insert(value_type &&value) { return table_.insert(move(value)); }
  template <class Pair> auto insert(Pair &&value) {
    return table_.insert(static_cast<Pair &&>(value));
  }
  iterator insert(const_iterator hint, const value_type &value) {
    return table_.insert(hint, value);
  }
  iterator insert(const_iterator hint, value_type &&value) {
    return table_.insert(hint, move(value));
  }
  template <input_iterator I> void insert(I first, I last) {
    table_.insert(first, last);
  }
  template <ranges::input_range R> void insert_range(R &&range) {
    for (auto &&value : range)
      insert(static_cast<decltype(value) &&>(value));
  }
  void insert(initializer_list<value_type> values) {
    insert(values.begin(), values.end());
  }
  auto insert(node_type &&node) { return table_.insert(move(node)); }
  iterator insert(const_iterator hint, node_type &&node) {
    return table_.insert(hint, move(node));
  }
  template <class... Args>
  pair<iterator, bool> try_emplace(const Key &key, Args &&...args)
    requires(!Multi)
  {
    auto found = find(key);
    if (found != end())
      return {found, false};
    return emplace(piecewise_construct, forward_as_tuple(key),
                   forward_as_tuple(forward<Args>(args)...));
  }
  template <class... Args>
  pair<iterator, bool> try_emplace(Key &&key, Args &&...args)
    requires(!Multi)
  {
    auto found = find(key);
    if (found != end())
      return {found, false};
    return emplace(piecewise_construct, forward_as_tuple(move(key)),
                   forward_as_tuple(forward<Args>(args)...));
  }
  template <class... Args>
  iterator try_emplace(const_iterator, const Key &key, Args &&...args)
    requires(!Multi)
  {
    return try_emplace(key, forward<Args>(args)...).first;
  }
  template <class... Args>
  iterator try_emplace(const_iterator, Key &&key, Args &&...args)
    requires(!Multi)
  {
    return try_emplace(move(key), forward<Args>(args)...).first;
  }
  template <class M>
  pair<iterator, bool> insert_or_assign(const Key &key, M &&value)
    requires(!Multi)
  {
    auto found = find(key);
    if (found != end()) {
      found->second = static_cast<M &&>(value);
      return {found, false};
    }
    return emplace(key, static_cast<M &&>(value));
  }
  template <class M>
  pair<iterator, bool> insert_or_assign(Key &&key, M &&value)
    requires(!Multi)
  {
    auto found = find(key);
    if (found != end()) {
      found->second = static_cast<M &&>(value);
      return {found, false};
    }
    return emplace(move(key), static_cast<M &&>(value));
  }
  template <class M>
  iterator insert_or_assign(const_iterator, const Key &key, M &&value)
    requires(!Multi)
  {
    return insert_or_assign(key, static_cast<M &&>(value)).first;
  }
  template <class M>
  iterator insert_or_assign(const_iterator, Key &&key, M &&value)
    requires(!Multi)
  {
    return insert_or_assign(move(key), static_cast<M &&>(value)).first;
  }
  iterator erase(iterator position) { return table_.erase(position); }
  iterator erase(const_iterator position) { return table_.erase(position); }
  iterator erase(const_iterator first, const_iterator last) {
    return table_.erase(first, last);
  }
  size_type erase(const Key &key) { return table_.erase_key(key); }
  template <class K>
  size_type erase(K &&key)
    requires requires {
      typename Hash::is_transparent;
      typename Equal::is_transparent;
    }
  {
    return table_.erase_key(key);
  }
  void clear() noexcept { table_.clear(); }
  void swap(unordered_map_base &other) noexcept(
      noexcept(table_.swap(other.table_))) {
    table_.swap(other.table_);
  }
  node_type extract(const_iterator position) {
    return table_.extract(position);
  }
  node_type extract(const Key &key) { return table_.extract(key); }
  template <class K>
  node_type extract(K &&key)
    requires requires {
      typename Hash::is_transparent;
      typename Equal::is_transparent;
    }
  {
    return table_.extract(key);
  }
  void merge(unordered_map_base &other) { table_.merge(other.table_); }
  void merge(unordered_map_base &&other) { merge(other); }
  template <class K> iterator find(const K &key) { return table_.find(key); }
  template <class K> const_iterator find(const K &key) const {
    return table_.find(key);
  }
  template <class K> size_type count(const K &key) const {
    return table_.count(key);
  }
  template <class K> bool contains(const K &key) const {
    return table_.contains(key);
  }
  template <class K> auto equal_range(const K &key) {
    return table_.equal_range(key);
  }
  template <class K> auto equal_range(const K &key) const {
    return table_.equal_range(key);
  }
  local_iterator begin(size_type bucket) { return table_.begin(bucket); }
  const_local_iterator begin(size_type bucket) const {
    return table_.begin(bucket);
  }
  local_iterator end(size_type bucket) { return table_.end(bucket); }
  const_local_iterator end(size_type bucket) const {
    return table_.end(bucket);
  }
  const_local_iterator cbegin(size_type bucket) const {
    return table_.cbegin(bucket);
  }
  const_local_iterator cend(size_type bucket) const {
    return table_.cend(bucket);
  }
  size_type bucket_count() const noexcept { return table_.bucket_count(); }
  size_type max_bucket_count() const noexcept {
    return table_.max_bucket_count();
  }
  size_type bucket_size(size_type bucket) const {
    return table_.bucket_size(bucket);
  }
  size_type bucket(const Key &key) const { return table_.bucket(key); }
  float load_factor() const noexcept { return table_.load_factor(); }
  float max_load_factor() const noexcept { return table_.max_load_factor(); }
  void max_load_factor(float value) { table_.max_load_factor(value); }
  void rehash(size_type count) { table_.rehash(count); }
  void reserve(size_type count) { table_.reserve(count); }
  hasher hash_function() const { return table_.hash_function(); }
  key_equal key_eq() const { return table_.key_eq(); }
};

template <class Key, class Hash, class Equal, class Allocator, bool Multi>
class unordered_set_base {
protected:
  using table_type = hash_table<Key, Key, unordered_set_key<Key>, Hash, Equal,
                                Allocator, Multi>;
  table_type table_;

public:
  using key_type = Key;
  using value_type = Key;
  using hasher = Hash;
  using key_equal = Equal;
  using allocator_type = Allocator;
  using pointer = typename allocator_traits<Allocator>::const_pointer;
  using const_pointer = pointer;
  using reference = const Key &;
  using const_reference = const Key &;
  using size_type = typename table_type::size_type;
  using difference_type = typename table_type::difference_type;
  using iterator = typename table_type::const_iterator;
  using const_iterator = iterator;
  using local_iterator = typename table_type::const_local_iterator;
  using const_local_iterator = local_iterator;
  using node_type = typename table_type::node_type;
  using insert_return_type = typename table_type::insert_return_type;
  unordered_set_base() = default;
  explicit unordered_set_base(size_type count, const Hash &hash = Hash(),
                              const Equal &equal = Equal(),
                              const Allocator &allocator = Allocator())
      : table_(count, hash, equal, allocator) {}
  explicit unordered_set_base(const Allocator &allocator) : table_(allocator) {}
  unordered_set_base(size_type count, const Allocator &allocator)
      : table_(count, allocator) {}
  unordered_set_base(size_type count, const Hash &hash,
                     const Allocator &allocator)
      : table_(count, hash, allocator) {}
  template <input_iterator I>
  unordered_set_base(I first, I last, size_type count = 0,
                     const Hash &hash = Hash(), const Equal &equal = Equal(),
                     const Allocator &allocator = Allocator())
      : table_(count, hash, equal, allocator) {
    insert(first, last);
  }
  template <ranges::input_range R>
  unordered_set_base(from_range_t, R &&range, size_type count = 0,
                     const Hash &hash = Hash(), const Equal &equal = Equal(),
                     const Allocator &allocator = Allocator())
      : table_(count, hash, equal, allocator) {
    insert_range(static_cast<R &&>(range));
  }
  unordered_set_base(initializer_list<Key> values, size_type count = 0,
                     const Hash &hash = Hash(), const Equal &equal = Equal(),
                     const Allocator &allocator = Allocator())
      : unordered_set_base(values.begin(), values.end(), count, hash, equal,
                           allocator) {}
  unordered_set_base(const unordered_set_base &) = default;
  unordered_set_base(unordered_set_base &&) = default;
  unordered_set_base(const unordered_set_base &other,
                     const Allocator &allocator)
      : table_(other.table_, allocator) {}
  unordered_set_base(unordered_set_base &&other, const Allocator &allocator)
      : table_(move(other.table_), allocator) {}
  unordered_set_base &operator=(const unordered_set_base &) = default;
  unordered_set_base &operator=(unordered_set_base &&) = default;
  unordered_set_base &operator=(initializer_list<Key> values) {
    clear();
    insert(values);
    return *this;
  }
  allocator_type get_allocator() const noexcept {
    return table_.get_allocator();
  }
  iterator begin() const noexcept { return table_.begin(); }
  iterator end() const noexcept { return table_.end(); }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  [[nodiscard]] bool empty() const noexcept { return table_.empty(); }
  size_type size() const noexcept { return table_.size(); }
  size_type max_size() const noexcept { return table_.max_size(); }
  template <class... Args> auto emplace(Args &&...args) {
    return table_.emplace(forward<Args>(args)...);
  }
  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args) {
    return table_.emplace_hint(hint, forward<Args>(args)...);
  }
  auto insert(const Key &value) { return table_.insert(value); }
  auto insert(Key &&value) { return table_.insert(move(value)); }
  iterator insert(const_iterator hint, const Key &value) {
    return table_.insert(hint, value);
  }
  iterator insert(const_iterator hint, Key &&value) {
    return table_.insert(hint, move(value));
  }
  template <input_iterator I> void insert(I first, I last) {
    table_.insert(first, last);
  }
  template <ranges::input_range R> void insert_range(R &&range) {
    for (auto &&value : range)
      insert(static_cast<decltype(value) &&>(value));
  }
  void insert(initializer_list<Key> values) {
    insert(values.begin(), values.end());
  }
  auto insert(node_type &&node) { return table_.insert(move(node)); }
  iterator insert(const_iterator hint, node_type &&node) {
    return table_.insert(hint, move(node));
  }
  iterator erase(const_iterator position) { return table_.erase(position); }
  iterator erase(const_iterator first, const_iterator last) {
    return table_.erase(first, last);
  }
  size_type erase(const Key &key) { return table_.erase_key(key); }
  template <class K>
  size_type erase(K &&key)
    requires requires {
      typename Hash::is_transparent;
      typename Equal::is_transparent;
    }
  {
    return table_.erase_key(key);
  }
  void clear() noexcept { table_.clear(); }
  void swap(unordered_set_base &other) noexcept(
      noexcept(table_.swap(other.table_))) {
    table_.swap(other.table_);
  }
  node_type extract(const_iterator position) {
    return table_.extract(position);
  }
  node_type extract(const Key &key) { return table_.extract(key); }
  template <class K>
  node_type extract(K &&key)
    requires requires {
      typename Hash::is_transparent;
      typename Equal::is_transparent;
    }
  {
    return table_.extract(key);
  }
  void merge(unordered_set_base &other) { table_.merge(other.table_); }
  void merge(unordered_set_base &&other) { merge(other); }
  template <class K> iterator find(const K &key) const {
    return table_.find(key);
  }
  template <class K> size_type count(const K &key) const {
    return table_.count(key);
  }
  template <class K> bool contains(const K &key) const {
    return table_.contains(key);
  }
  template <class K> auto equal_range(const K &key) const {
    return table_.equal_range(key);
  }
  local_iterator begin(size_type bucket) const { return table_.begin(bucket); }
  local_iterator end(size_type bucket) const { return table_.end(bucket); }
  const_local_iterator cbegin(size_type bucket) const {
    return table_.cbegin(bucket);
  }
  const_local_iterator cend(size_type bucket) const {
    return table_.cend(bucket);
  }
  size_type bucket_count() const noexcept { return table_.bucket_count(); }
  size_type max_bucket_count() const noexcept {
    return table_.max_bucket_count();
  }
  size_type bucket_size(size_type bucket) const {
    return table_.bucket_size(bucket);
  }
  size_type bucket(const Key &key) const { return table_.bucket(key); }
  float load_factor() const noexcept { return table_.load_factor(); }
  float max_load_factor() const noexcept { return table_.max_load_factor(); }
  void max_load_factor(float value) { table_.max_load_factor(value); }
  void rehash(size_type count) { table_.rehash(count); }
  void reserve(size_type count) { table_.reserve(count); }
  hasher hash_function() const { return table_.hash_function(); }
  key_equal key_eq() const { return table_.key_eq(); }
};
} // namespace detail
FTL_END_NAMESPACE
#endif
