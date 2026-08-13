#ifndef FTL_ASSOCIATIVE_TREE_HPP
#define FTL_ASSOCIATIVE_TREE_HPP

FTL_BEGIN_NAMESPACE
namespace detail {

template <class Value> struct associative_tree_node {
  associative_tree_node *parent{};
  associative_tree_node *left{};
  associative_tree_node *right{};
  int height{1};
  Value value;

  template <class... Args>
  explicit associative_tree_node(Args &&...args)
      : value(forward<Args>(args)...) {}
};

template <class Value, class Key, class Allocator>
class associative_tree_node_handle {
  using node = associative_tree_node<Value>;
  using value_traits = allocator_traits<Allocator>;
  using node_allocator = typename value_traits::template rebind_alloc<node>;
  using node_traits = allocator_traits<node_allocator>;

  node *value_{};
  optional<Allocator> allocator_;
  template <class, class, class, class, class, bool>
  friend class associative_tree;

  associative_tree_node_handle(node *value, const Allocator &allocator)
      : value_(value), allocator_(in_place, allocator) {}

public:
  using allocator_type = Allocator;
  using value_type = Value;
  associative_tree_node_handle() noexcept = default;
  associative_tree_node_handle(associative_tree_node_handle &&other) noexcept
      : value_(exchange(other.value_, nullptr)),
        allocator_(move(other.allocator_)) {
    other.allocator_.reset();
  }
  associative_tree_node_handle &
  operator=(associative_tree_node_handle &&other) noexcept {
    if (this != &other) {
      reset();
      value_ = exchange(other.value_, nullptr);
      if (!allocator_ ||
          value_traits::propagate_on_container_move_assignment::value)
        allocator_ = move(other.allocator_);
      other.allocator_.reset();
    }
    return *this;
  }
  associative_tree_node_handle(const associative_tree_node_handle &) = delete;
  associative_tree_node_handle &
  operator=(const associative_tree_node_handle &) = delete;
  ~associative_tree_node_handle() { reset(); }
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
  void swap(associative_tree_node_handle &other) noexcept(
      value_traits::propagate_on_container_swap::value ||
      value_traits::is_always_equal::value) {
    FTL_ASSOCIATIVE_NAMESPACE::swap(value_, other.value_);
    if constexpr (value_traits::propagate_on_container_swap::value)
      FTL_ASSOCIATIVE_NAMESPACE::swap(allocator_, other.allocator_);
  }

private:
  void reset() noexcept {
    if (!value_)
      return;
    node_allocator allocator(*allocator_);
    auto pointer = pointer_traits<typename node_traits::pointer>::pointer_to(*value_);
    node_traits::destroy(allocator, value_);
    node_traits::deallocate(allocator, pointer, 1);
    value_ = nullptr;
    allocator_.reset();
  }
};

template <class Value, class Key, class KeyOfValue, class Compare,
          class Allocator, bool Multi>
class associative_tree {
  using node = associative_tree_node<Value>;

  template <class, class, class, class, class, bool>
  friend class associative_tree;

  using value_traits = allocator_traits<Allocator>;
  using node_allocator = typename value_traits::template rebind_alloc<node>;
  using node_traits = allocator_traits<node_allocator>;

public:
  using value_type = Value;
  using key_type = Key;
  using allocator_type = Allocator;
  using size_type = typename value_traits::size_type;
  using difference_type = typename value_traits::difference_type;

  template <bool Constant> class basic_iterator {
    node *current_{};
    const associative_tree *owner_{};

    basic_iterator(node *current, const associative_tree *owner)
        : current_(current), owner_(owner) {}
    friend class associative_tree;
    template <bool> friend class basic_iterator;

  public:
    using iterator_concept = bidirectional_iterator_tag;
    using iterator_category = bidirectional_iterator_tag;
    using value_type = Value;
    using difference_type = typename associative_tree::difference_type;
    using reference = conditional_t<Constant, const Value &, Value &>;
    using pointer = conditional_t<Constant, const Value *, Value *>;

    basic_iterator() = default;
    template <bool Other>
      requires(Constant && !Other)
    basic_iterator(const basic_iterator<Other> &other)
        : current_(other.current_), owner_(other.owner_) {}

    reference operator*() const { return current_->value; }
    pointer operator->() const { return addressof(current_->value); }
    basic_iterator &operator++() {
      current_ = current_ ? successor(current_) : nullptr;
      return *this;
    }
    basic_iterator operator++(int) {
      auto copy = *this;
      ++*this;
      return copy;
    }
    basic_iterator &operator--() {
      current_ = current_ ? predecessor(current_) : maximum(owner_->root_);
      return *this;
    }
    basic_iterator operator--(int) {
      auto copy = *this;
      --*this;
      return copy;
    }
    template <bool Other>
    bool operator==(const basic_iterator<Other> &other) const {
      return current_ == other.current_;
    }
  };

  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;

  using node_type = associative_tree_node_handle<Value, Key, Allocator>;

  struct insert_return_type {
    iterator position;
    bool inserted;
    node_type node;
  };

  associative_tree() = default;
  explicit associative_tree(const Compare &compare,
                            const Allocator &allocator = Allocator())
      : compare_(compare), allocator_(allocator) {}
  explicit associative_tree(const Allocator &allocator)
      : allocator_(allocator) {}
  associative_tree(const associative_tree &other)
      : compare_(other.compare_),
        allocator_(value_traits::select_on_container_copy_construction(
            other.allocator_)) {
    initialize([&] { insert(other.begin(), other.end()); });
  }
  associative_tree(const associative_tree &other, const Allocator &allocator)
      : compare_(other.compare_), allocator_(allocator) {
    initialize([&] { insert(other.begin(), other.end()); });
  }
  associative_tree(associative_tree &&other) noexcept
      : root_(exchange(other.root_, nullptr)), size_(exchange(other.size_, 0)),
        compare_(move(other.compare_)), allocator_(move(other.allocator_)) {}
  associative_tree(associative_tree &&other, const Allocator &allocator)
      : compare_(move(other.compare_)), allocator_(allocator) {
    if (allocator_ == other.allocator_) {
      root_ = exchange(other.root_, nullptr);
      size_ = exchange(other.size_, 0);
    } else {
      initialize([&] { insert(make_move_iterator(other.begin()),
                              make_move_iterator(other.end())); });
      other.clear();
    }
  }
  ~associative_tree() { clear(); }

  associative_tree &operator=(const associative_tree &other) {
    if (this == &other)
      return *this;
    if constexpr (value_traits::propagate_on_container_copy_assignment::value) {
      if (!(allocator_ == other.allocator_))
        clear();
      allocator_ = other.allocator_;
    }
    compare_ = other.compare_;
    clear();
    insert(other.begin(), other.end());
    return *this;
  }
  associative_tree &operator=(associative_tree &&other) noexcept(
      value_traits::is_always_equal::value &&
      is_nothrow_move_assignable_v<Compare>) {
    if (this == &other)
      return *this;
    if constexpr (value_traits::propagate_on_container_move_assignment::value) {
      clear();
      allocator_ = move(other.allocator_);
      compare_ = move(other.compare_);
      root_ = exchange(other.root_, nullptr);
      size_ = exchange(other.size_, 0);
    } else if (allocator_ == other.allocator_) {
      clear();
      compare_ = move(other.compare_);
      root_ = exchange(other.root_, nullptr);
      size_ = exchange(other.size_, 0);
    } else {
      clear();
      insert(make_move_iterator(other.begin()),
             make_move_iterator(other.end()));
      other.clear();
    }
    return *this;
  }

  allocator_type get_allocator() const noexcept { return allocator_; }
  Compare key_comp() const { return compare_; }
  iterator begin() noexcept { return iterator(minimum(root_), this); }
  const_iterator begin() const noexcept {
    return const_iterator(minimum(root_), this);
  }
  iterator end() noexcept { return iterator(nullptr, this); }
  const_iterator end() const noexcept { return const_iterator(nullptr, this); }
  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept {
    node_allocator allocator(allocator_);
    return node_traits::max_size(allocator);
  }

  template <class V> auto insert(V &&value) {
    node *added = make_node(static_cast<V &&>(value));
#if FTL_HAS_EXCEPTIONS
    try {
      return insert_node(added);
    } catch (...) {
      destroy_node(added);
      throw;
    }
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
    try {
      return insert_node(added);
    } catch (...) {
      destroy_node(added);
      throw;
    }
#else
    return insert_node(added);
#endif
  }
  iterator insert(const_iterator hint, const Value &value) {
    return insert_hint_value(hint, value);
  }
  iterator insert(const_iterator hint, Value &&value) {
    return insert_hint_value(hint, move(value));
  }
  template <class... Args>
  iterator emplace_hint(const_iterator hint, Args &&...args) {
    node *added = make_node(forward<Args>(args)...);
#if FTL_HAS_EXCEPTIONS
    try { return insert_node_at_hint(hint, added); }
    catch (...) { destroy_node(added); throw; }
#else
    return insert_node_at_hint(hint, added);
#endif
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
      const Key &key = KeyOfValue{}(handle.value_->value);
      if (auto found = find_node(key))
        return insert_return_type{iterator(found, this), false, move(handle)};
      auto result = insert_node(handle.value_, false);
      handle.value_ = nullptr;
      return insert_return_type{result.first, true, {}};
    }
  }
  iterator insert(const_iterator hint, node_type &&handle) {
    if (!handle)
      return end();
    iterator result = insert_node_at_hint(hint, handle.value_, false);
    if constexpr (Multi)
      handle.value_ = nullptr;
    else if (result.current_ == handle.value_)
      handle.value_ = nullptr;
    return result;
  }

  iterator erase(const_iterator position) {
    node *next = successor(position.current_);
    node *removed = detach(position.current_);
    destroy_node(removed);
    return iterator(next, this);
  }
  iterator erase(const_iterator first, const_iterator last) {
    while (first != last)
      first = erase(first);
    return iterator(last.current_, this);
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
    destroy_subtree(root_);
    root_ = nullptr;
    size_ = 0;
  }
  void
  swap(associative_tree &other) noexcept(value_traits::is_always_equal::value &&
                                         is_nothrow_swappable_v<Compare>) {
    if constexpr (value_traits::propagate_on_container_swap::value)
      FTL_ASSOCIATIVE_NAMESPACE::swap(allocator_, other.allocator_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(root_, other.root_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(size_, other.size_);
    FTL_ASSOCIATIVE_NAMESPACE::swap(compare_, other.compare_);
  }

  node_type extract(const_iterator position) {
    return node_type(detach(position.current_), allocator_);
  }
  template <class K> node_type extract(const K &key) {
    node *found = find_node(key);
    return found ? node_type(detach(found), allocator_) : node_type{};
  }
  template <class OtherCompare, bool OtherMulti>
  void merge(associative_tree<Value, Key, KeyOfValue, OtherCompare, Allocator,
                              OtherMulti> &other) {
    if (static_cast<const void *>(this) == static_cast<const void *>(&other))
      return;
    for (auto current = other.begin(); current != other.end();) {
      auto candidate = current++;
      insertion_slot slot = locate_insertion(KeyOfValue{}(*candidate));
      if constexpr (!Multi)
        if (slot.equivalent)
          continue;
      node *moved = other.detach_iterator(candidate);
      attach(moved, slot);
    }
  }

  template <class K> iterator find(const K &key) {
    return iterator(find_node(key), this);
  }
  template <class K> const_iterator find(const K &key) const {
    return const_iterator(find_node(key), this);
  }
  template <class K> size_type count(const K &key) const {
    if constexpr (!Multi)
      return find_node(key) ? 1 : 0;
    auto range = equal_range(key);
    return static_cast<size_type>(distance(range.first, range.second));
  }
  template <class K> bool contains(const K &key) const {
    return find_node(key) != nullptr;
  }
  template <class K> iterator lower_bound(const K &key) {
    return iterator(lower_node(key), this);
  }
  template <class K> const_iterator lower_bound(const K &key) const {
    return const_iterator(lower_node(key), this);
  }
  template <class K> iterator upper_bound(const K &key) {
    return iterator(upper_node(key), this);
  }
  template <class K> const_iterator upper_bound(const K &key) const {
    return const_iterator(upper_node(key), this);
  }
  template <class K> pair<iterator, iterator> equal_range(const K &key) {
    return {lower_bound(key), upper_bound(key)};
  }
  template <class K>
  pair<const_iterator, const_iterator> equal_range(const K &key) const {
    return {lower_bound(key), upper_bound(key)};
  }

private:
  struct insertion_slot {
    node *parent{};
    node *equivalent{};
    bool left{};
  };
  insertion_slot locate_insertion(const Key &key) const {
    insertion_slot result;
    node *current = root_;
    while (current) {
      result.parent = current;
      const Key &current_key = KeyOfValue{}(current->value);
      if (compare_(key, current_key)) {
        result.left = true;
        current = current->left;
      } else if constexpr (!Multi) {
        if (!compare_(current_key, key)) {
          result.equivalent = current;
          return result;
        }
        result.left = false;
        current = current->right;
      } else {
        result.left = false;
        current = current->right;
      }
    }
    return result;
  }
  void attach(node *added, insertion_slot slot) noexcept {
    added->parent = slot.parent;
    added->left = added->right = nullptr;
    added->height = 1;
    if (!slot.parent)
      root_ = added;
    else if (slot.left)
      slot.parent->left = added;
    else
      slot.parent->right = added;
    ++size_;
    rebalance(slot.parent);
  }
  template <class V> iterator insert_hint_value(const_iterator hint, V &&value) {
    node *added = make_node(static_cast<V &&>(value));
#if FTL_HAS_EXCEPTIONS
    try { return insert_node_at_hint(hint, added); }
    catch (...) { destroy_node(added); throw; }
#else
    return insert_node_at_hint(hint, added);
#endif
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
  void destroy_subtree(node *value) noexcept {
    if (!value)
      return;
    destroy_subtree(value->left);
    destroy_subtree(value->right);
    destroy_node(value);
  }

  auto insert_node(node *added, bool destroy_duplicate = true) {
    added->left = added->right = added->parent = nullptr;
    added->height = 1;
    if (!root_) {
      root_ = added;
      ++size_;
      if constexpr (Multi)
        return iterator(added, this);
      else
        return pair<iterator, bool>{iterator(added, this), true};
    }
    node *parent = root_;
    const Key &key = KeyOfValue{}(added->value);
    while (true) {
      const Key &current_key = KeyOfValue{}(parent->value);
      if (compare_(key, current_key)) {
        if (parent->left)
          parent = parent->left;
        else {
          parent->left = added;
          break;
        }
      } else if constexpr (!Multi) {
        if (!compare_(current_key, key)) {
          if (destroy_duplicate)
            destroy_node(added);
          return pair<iterator, bool>{iterator(parent, this), false};
        }
        if (parent->right)
          parent = parent->right;
        else {
          parent->right = added;
          break;
        }
      } else {
        if (parent->right)
          parent = parent->right;
        else {
          parent->right = added;
          break;
        }
      }
    }
    added->parent = parent;
    ++size_;
    rebalance(parent);
    if constexpr (Multi)
      return iterator(added, this);
    else
      return pair<iterator, bool>{iterator(added, this), true};
  }
  iterator insert_node_at_hint(const_iterator hint, node *added,
                               bool destroy_duplicate = true) {
    if (!root_) {
      auto result = insert_node(added, destroy_duplicate);
      if constexpr (Multi)
        return result;
      else
        return result.first;
    }
    const Key &key = KeyOfValue{}(added->value);
    node *next = hint.current_;
    node *previous = next ? predecessor(next) : maximum(root_);
    const bool after_previous =
        !previous || (Multi ? !compare_(key, KeyOfValue{}(previous->value))
                            : compare_(KeyOfValue{}(previous->value), key));
    const bool before_next =
        !next || (Multi ? !compare_(KeyOfValue{}(next->value), key)
                        : compare_(key, KeyOfValue{}(next->value)));
    if (after_previous && before_next) {
      added->left = added->right = nullptr;
      added->height = 1;
      if (next && !next->left) {
        next->left = added;
        added->parent = next;
      } else {
        previous->right = added;
        added->parent = previous;
      }
      ++size_;
      rebalance(added->parent);
      return iterator(added, this);
    }
    auto result = insert_node(added, destroy_duplicate);
    if constexpr (Multi)
      return result;
    else
      return result.first;
  }

  node *detach(node *value) {
    node *rebalance_from{};
    if (value->left && value->right) {
      node *replacement = minimum(value->right);
      node *replacement_parent = replacement->parent;
      if (replacement_parent != value) {
        transplant(replacement, replacement->right);
        replacement->right = value->right;
        replacement->right->parent = replacement;
        rebalance_from = replacement_parent;
      } else {
        rebalance_from = replacement;
      }
      transplant(value, replacement);
      replacement->left = value->left;
      replacement->left->parent = replacement;
      update(replacement);
    } else {
      node *child = value->left ? value->left : value->right;
      rebalance_from = value->parent;
      transplant(value, child);
    }
    value->parent = value->left = value->right = nullptr;
    value->height = 1;
    --size_;
    rebalance(rebalance_from);
    return value;
  }
  node *detach_iterator(iterator value) { return detach(value.current_); }
  void transplant(node *old_value, node *new_value) {
    if (!old_value->parent)
      root_ = new_value;
    else if (old_value == old_value->parent->left)
      old_value->parent->left = new_value;
    else
      old_value->parent->right = new_value;
    if (new_value)
      new_value->parent = old_value->parent;
  }
  static int height(node *value) { return value ? value->height : 0; }
  static void update(node *value) {
    value->height =
        1 + (height(value->left) > height(value->right) ? height(value->left)
                                                        : height(value->right));
  }
  node *rotate_left(node *value) {
    node *result = value->right;
    transplant(value, result);
    value->right = result->left;
    if (value->right)
      value->right->parent = value;
    result->left = value;
    value->parent = result;
    update(value);
    update(result);
    return result;
  }
  node *rotate_right(node *value) {
    node *result = value->left;
    transplant(value, result);
    value->left = result->right;
    if (value->left)
      value->left->parent = value;
    result->right = value;
    value->parent = result;
    update(value);
    update(result);
    return result;
  }
  void rebalance(node *value) {
    while (value) {
      update(value);
      if (height(value->left) - height(value->right) > 1) {
        if (height(value->left->right) > height(value->left->left))
          rotate_left(value->left);
        value = rotate_right(value);
      } else if (height(value->right) - height(value->left) > 1) {
        if (height(value->right->left) > height(value->right->right))
          rotate_right(value->right);
        value = rotate_left(value);
      }
      value = value->parent;
    }
  }

  template <class K> node *find_node(const K &key) const {
    node *current = root_;
    while (current) {
      const Key &current_key = KeyOfValue{}(current->value);
      if (compare_(key, current_key))
        current = current->left;
      else if (compare_(current_key, key))
        current = current->right;
      else
        return current;
    }
    return nullptr;
  }
  template <class K> node *lower_node(const K &key) const {
    node *current = root_;
    node *result{};
    while (current) {
      if (!compare_(KeyOfValue{}(current->value), key)) {
        result = current;
        current = current->left;
      } else
        current = current->right;
    }
    return result;
  }
  template <class K> node *upper_node(const K &key) const {
    node *current = root_;
    node *result{};
    while (current) {
      if (compare_(key, KeyOfValue{}(current->value))) {
        result = current;
        current = current->left;
      } else
        current = current->right;
    }
    return result;
  }
  static node *minimum(node *value) {
    if (!value)
      return nullptr;
    while (value->left)
      value = value->left;
    return value;
  }
  static node *maximum(node *value) {
    if (!value)
      return nullptr;
    while (value->right)
      value = value->right;
    return value;
  }
  static node *successor(node *value) {
    if (value->right)
      return minimum(value->right);
    while (value->parent && value == value->parent->right)
      value = value->parent;
    return value->parent;
  }
  static node *predecessor(node *value) {
    if (value->left)
      return maximum(value->left);
    while (value->parent && value == value->parent->left)
      value = value->parent;
    return value->parent;
  }

  node *root_{};
  size_type size_{};
  FTL_NO_UNIQUE_ADDRESS Compare compare_{};
  FTL_NO_UNIQUE_ADDRESS Allocator allocator_{};
};

} // namespace detail
FTL_END_NAMESPACE
#endif
