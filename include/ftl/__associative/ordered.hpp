#ifndef FTL_ASSOCIATIVE_ORDERED_HPP
#define FTL_ASSOCIATIVE_ORDERED_HPP

#ifdef FTL_REPLACE_STL
#include <__associative/tree.hpp>
#else
#include <ftl/__associative/tree.hpp>
#endif

FTL_BEGIN_NAMESPACE
namespace detail {

template <class Key> struct set_key {
  const Key &operator()(const Key &value) const noexcept { return value; }
};

template <class Key, class T> struct map_key {
  const Key &operator()(const pair<const Key, T> &value) const noexcept {
    return value.first;
  }
};

template <class Key, class Compare> class ordered_value_compare {
  Compare compare_;

public:
  explicit ordered_value_compare(Compare compare) : compare_(compare) {}
  template <class Left, class Right>
  bool operator()(const Left &left, const Right &right) const {
    return compare_(left.first, right.first);
  }
};

template <class Key, class T, class Compare, class Allocator, bool Multi>
class ordered_map_base {
protected:
  using tree_type = associative_tree<pair<const Key, T>, Key, map_key<Key, T>,
                                     Compare, Allocator, Multi>;
  tree_type tree_;

public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = pair<const Key, T>;
  using key_compare = Compare;
  using value_compare = ordered_value_compare<Key, Compare>;
  using allocator_type = Allocator;
  using pointer = typename allocator_traits<Allocator>::pointer;
  using const_pointer = typename allocator_traits<Allocator>::const_pointer;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = typename tree_type::size_type;
  using difference_type = typename tree_type::difference_type;
  using iterator = typename tree_type::iterator;
  using const_iterator = typename tree_type::const_iterator;
  using reverse_iterator =
      FTL_ASSOCIATIVE_NAMESPACE::reverse_iterator<iterator>;
  using const_reverse_iterator =
      FTL_ASSOCIATIVE_NAMESPACE::reverse_iterator<const_iterator>;
  using node_type = typename tree_type::node_type;
  using insert_return_type = typename tree_type::insert_return_type;

  ordered_map_base() = default;
  explicit ordered_map_base(const Compare &compare,
                            const Allocator &allocator = Allocator())
      : tree_(compare, allocator) {}
  explicit ordered_map_base(const Allocator &allocator) : tree_(allocator) {}
  template <input_iterator InputIterator>
  ordered_map_base(InputIterator first, InputIterator last,
                   const Compare &compare = Compare(),
                   const Allocator &allocator = Allocator())
      : tree_(compare, allocator) {
    insert(first, last);
  }
  template <ranges::input_range Range>
  ordered_map_base(from_range_t, Range &&range,
                   const Compare &compare = Compare(),
                   const Allocator &allocator = Allocator())
      : tree_(compare, allocator) {
    insert_range(static_cast<Range &&>(range));
  }
  ordered_map_base(initializer_list<value_type> values,
                   const Compare &compare = Compare(),
                   const Allocator &allocator = Allocator())
      : ordered_map_base(values.begin(), values.end(), compare, allocator) {}
  ordered_map_base(const ordered_map_base &) = default;
  ordered_map_base(ordered_map_base &&) = default;
  ordered_map_base(const ordered_map_base &other, const Allocator &allocator)
      : tree_(other.tree_, allocator) {}
  ordered_map_base(ordered_map_base &&other, const Allocator &allocator)
      : tree_(move(other.tree_), allocator) {}
  ordered_map_base &operator=(const ordered_map_base &) = default;
  ordered_map_base &operator=(ordered_map_base &&) = default;
  ordered_map_base &operator=(initializer_list<value_type> values) {
    clear();
    insert(values);
    return *this;
  }

  allocator_type get_allocator() const noexcept {
    return tree_.get_allocator();
  }
  iterator begin() noexcept { return tree_.begin(); }
  const_iterator begin() const noexcept { return tree_.begin(); }
  iterator end() noexcept { return tree_.end(); }
  const_iterator end() const noexcept { return tree_.end(); }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }
  [[nodiscard]] bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  template <class... Args> auto emplace(Args &&...args) {
    return tree_.emplace(forward<Args>(args)...);
  }
  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args) {
    return tree_.emplace_hint(hint, forward<Args>(args)...);
  }
  auto insert(const value_type &value) { return tree_.insert(value); }
  auto insert(value_type &&value) { return tree_.insert(move(value)); }
  template <class Pair> auto insert(Pair &&value) {
    return tree_.insert(static_cast<Pair &&>(value));
  }
  iterator insert(const_iterator hint, const value_type &value) {
    return tree_.insert(hint, value);
  }
  iterator insert(const_iterator hint, value_type &&value) {
    return tree_.insert(hint, move(value));
  }
  template <class Pair> iterator insert(const_iterator hint, Pair &&value) {
    return tree_.emplace_hint(hint, static_cast<Pair &&>(value));
  }
  template <input_iterator InputIterator>
  void insert(InputIterator first, InputIterator last) {
    tree_.insert(first, last);
  }
  template <ranges::input_range Range> void insert_range(Range &&range) {
    for (auto &&value : range)
      insert(static_cast<decltype(value) &&>(value));
  }
  void insert(initializer_list<value_type> values) {
    insert(values.begin(), values.end());
  }
  auto insert(node_type &&node) { return tree_.insert(move(node)); }
  iterator insert(const_iterator hint, node_type &&node) {
    return tree_.insert(hint, move(node));
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

  iterator erase(iterator position) { return tree_.erase(position); }
  iterator erase(const_iterator position) { return tree_.erase(position); }
  iterator erase(const_iterator first, const_iterator last) {
    return tree_.erase(first, last);
  }
  size_type erase(const Key &key) { return tree_.erase_key(key); }
  template <class K>
  size_type erase(K &&key)
    requires requires { typename Compare::is_transparent; }
  {
    return tree_.erase_key(key);
  }
  void clear() noexcept { tree_.clear(); }
  void
  swap(ordered_map_base &other) noexcept(noexcept(tree_.swap(other.tree_))) {
    tree_.swap(other.tree_);
  }
  node_type extract(const_iterator position) { return tree_.extract(position); }
  node_type extract(const Key &key) { return tree_.extract(key); }
  template <class K>
  node_type extract(K &&key)
    requires requires { typename Compare::is_transparent; }
  {
    return tree_.extract(key);
  }
  void merge(ordered_map_base &other) { tree_.merge(other.tree_); }
  void merge(ordered_map_base &&other) { merge(other); }

  key_compare key_comp() const { return tree_.key_comp(); }
  value_compare value_comp() const { return value_compare(key_comp()); }
  iterator find(const Key &key) { return tree_.find(key); }
  const_iterator find(const Key &key) const { return tree_.find(key); }
  template <class K> iterator find(const K &key) { return tree_.find(key); }
  template <class K> const_iterator find(const K &key) const {
    return tree_.find(key);
  }
  size_type count(const Key &key) const { return tree_.count(key); }
  template <class K> size_type count(const K &key) const {
    return tree_.count(key);
  }
  bool contains(const Key &key) const { return tree_.contains(key); }
  template <class K> bool contains(const K &key) const {
    return tree_.contains(key);
  }
  iterator lower_bound(const Key &key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const Key &key) const {
    return tree_.lower_bound(key);
  }
  template <class K> iterator lower_bound(const K &key) {
    return tree_.lower_bound(key);
  }
  template <class K> const_iterator lower_bound(const K &key) const {
    return tree_.lower_bound(key);
  }
  iterator upper_bound(const Key &key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const Key &key) const {
    return tree_.upper_bound(key);
  }
  template <class K> iterator upper_bound(const K &key) {
    return tree_.upper_bound(key);
  }
  template <class K> const_iterator upper_bound(const K &key) const {
    return tree_.upper_bound(key);
  }
  auto equal_range(const Key &key) { return tree_.equal_range(key); }
  auto equal_range(const Key &key) const { return tree_.equal_range(key); }
  template <class K> auto equal_range(const K &key) {
    return tree_.equal_range(key);
  }
  template <class K> auto equal_range(const K &key) const {
    return tree_.equal_range(key);
  }
};

template <class Key, class Compare, class Allocator, bool Multi>
class ordered_set_base {
protected:
  using tree_type =
      associative_tree<Key, Key, set_key<Key>, Compare, Allocator, Multi>;
  tree_type tree_;

public:
  using key_type = Key;
  using value_type = Key;
  using key_compare = Compare;
  using value_compare = Compare;
  using allocator_type = Allocator;
  using pointer = typename allocator_traits<Allocator>::const_pointer;
  using const_pointer = typename allocator_traits<Allocator>::const_pointer;
  using reference = const value_type &;
  using const_reference = const value_type &;
  using size_type = typename tree_type::size_type;
  using difference_type = typename tree_type::difference_type;
  using iterator = typename tree_type::const_iterator;
  using const_iterator = typename tree_type::const_iterator;
  using reverse_iterator =
      FTL_ASSOCIATIVE_NAMESPACE::reverse_iterator<iterator>;
  using const_reverse_iterator =
      FTL_ASSOCIATIVE_NAMESPACE::reverse_iterator<const_iterator>;
  using node_type = typename tree_type::node_type;
  using insert_return_type = typename tree_type::insert_return_type;

  ordered_set_base() = default;
  explicit ordered_set_base(const Compare &compare,
                            const Allocator &allocator = Allocator())
      : tree_(compare, allocator) {}
  explicit ordered_set_base(const Allocator &allocator) : tree_(allocator) {}
  template <input_iterator InputIterator>
  ordered_set_base(InputIterator first, InputIterator last,
                   const Compare &compare = Compare(),
                   const Allocator &allocator = Allocator())
      : tree_(compare, allocator) {
    insert(first, last);
  }
  template <ranges::input_range Range>
  ordered_set_base(from_range_t, Range &&range,
                   const Compare &compare = Compare(),
                   const Allocator &allocator = Allocator())
      : tree_(compare, allocator) {
    insert_range(static_cast<Range &&>(range));
  }
  ordered_set_base(initializer_list<value_type> values,
                   const Compare &compare = Compare(),
                   const Allocator &allocator = Allocator())
      : ordered_set_base(values.begin(), values.end(), compare, allocator) {}
  ordered_set_base(const ordered_set_base &) = default;
  ordered_set_base(ordered_set_base &&) = default;
  ordered_set_base(const ordered_set_base &other, const Allocator &allocator)
      : tree_(other.tree_, allocator) {}
  ordered_set_base(ordered_set_base &&other, const Allocator &allocator)
      : tree_(move(other.tree_), allocator) {}
  ordered_set_base &operator=(const ordered_set_base &) = default;
  ordered_set_base &operator=(ordered_set_base &&) = default;
  ordered_set_base &operator=(initializer_list<value_type> values) {
    clear();
    insert(values);
    return *this;
  }

  allocator_type get_allocator() const noexcept {
    return tree_.get_allocator();
  }
  iterator begin() const noexcept { return tree_.begin(); }
  iterator end() const noexcept { return tree_.end(); }
  reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }
  const_iterator cbegin() const noexcept { return begin(); }
  const_iterator cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }
  [[nodiscard]] bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }
  template <class... Args> auto emplace(Args &&...args) {
    return tree_.emplace(forward<Args>(args)...);
  }
  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args) {
    return tree_.emplace_hint(hint, forward<Args>(args)...);
  }
  auto insert(const value_type &value) { return tree_.insert(value); }
  auto insert(value_type &&value) { return tree_.insert(move(value)); }
  iterator insert(const_iterator hint, const value_type &value) {
    return tree_.insert(hint, value);
  }
  iterator insert(const_iterator hint, value_type &&value) {
    return tree_.insert(hint, move(value));
  }
  template <input_iterator InputIterator>
  void insert(InputIterator first, InputIterator last) {
    tree_.insert(first, last);
  }
  template <ranges::input_range Range> void insert_range(Range &&range) {
    for (auto &&value : range)
      insert(static_cast<decltype(value) &&>(value));
  }
  void insert(initializer_list<value_type> values) {
    insert(values.begin(), values.end());
  }
  auto insert(node_type &&node) { return tree_.insert(move(node)); }
  iterator insert(const_iterator hint, node_type &&node) {
    return tree_.insert(hint, move(node));
  }
  iterator erase(const_iterator position) { return tree_.erase(position); }
  iterator erase(const_iterator first, const_iterator last) {
    return tree_.erase(first, last);
  }
  size_type erase(const Key &key) { return tree_.erase_key(key); }
  template <class K>
  size_type erase(K &&key)
    requires requires { typename Compare::is_transparent; }
  {
    return tree_.erase_key(key);
  }
  void clear() noexcept { tree_.clear(); }
  void
  swap(ordered_set_base &other) noexcept(noexcept(tree_.swap(other.tree_))) {
    tree_.swap(other.tree_);
  }
  node_type extract(const_iterator position) { return tree_.extract(position); }
  node_type extract(const Key &key) { return tree_.extract(key); }
  template <class K>
  node_type extract(K &&key)
    requires requires { typename Compare::is_transparent; }
  {
    return tree_.extract(key);
  }
  void merge(ordered_set_base &other) { tree_.merge(other.tree_); }
  void merge(ordered_set_base &&other) { merge(other); }
  key_compare key_comp() const { return tree_.key_comp(); }
  value_compare value_comp() const { return tree_.key_comp(); }
  template <class K> iterator find(const K &key) const {
    return tree_.find(key);
  }
  template <class K> size_type count(const K &key) const {
    return tree_.count(key);
  }
  template <class K> bool contains(const K &key) const {
    return tree_.contains(key);
  }
  template <class K> iterator lower_bound(const K &key) const {
    return tree_.lower_bound(key);
  }
  template <class K> iterator upper_bound(const K &key) const {
    return tree_.upper_bound(key);
  }
  template <class K> auto equal_range(const K &key) const {
    return tree_.equal_range(key);
  }
};

} // namespace detail
FTL_END_NAMESPACE
#endif
