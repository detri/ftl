#ifdef FTL_REPLACE_STL
#include <memory_resource>
namespace tested = std;
#else
#include <ftl/memory_resource>
namespace tested = ftl;
#endif

struct value {
  int number;
  explicit value(int n) : number(n) {}
};

struct alignas(128) over_aligned {
  int value;
};

class counting_resource final : public tested::pmr::memory_resource {
public:
  tested::size_t allocations{};
  tested::size_t deallocations{};
  tested::size_t outstanding{};

private:
  void *do_allocate(tested::size_t bytes, tested::size_t alignment) override {
    ++allocations;
    ++outstanding;
    return tested::pmr::new_delete_resource()->allocate(bytes, alignment);
  }

  void do_deallocate(void *pointer, tested::size_t bytes,
                     tested::size_t alignment) override {
    ++deallocations;
    --outstanding;
    tested::pmr::new_delete_resource()->deallocate(pointer, bytes, alignment);
  }

  bool do_is_equal(
      const tested::pmr::memory_resource &other) const noexcept override {
    return this == &other;
  }
};

bool polymorphic_allocator_works() {
  alignas(value) unsigned char storage[64];

  tested::pmr::monotonic_buffer_resource arena(storage, sizeof storage);
  tested::pmr::polymorphic_allocator<value> allocator(&arena);

  value *result = allocator.new_object<value>(42);

  const bool works =
      result->number == 42 && allocator.resource() == &arena &&
      tested::pmr::new_delete_resource() == tested::pmr::new_delete_resource();

  allocator.delete_object(result);

  return works;
}

bool pool_reuses_blocks() {
  counting_resource upstream;

  tested::pmr::pool_options options{};
  options.max_blocks_per_chunk = 4;
  options.largest_required_pool_block = 64;

  tested::pmr::unsynchronized_pool_resource pool(options, &upstream);

  void *first = pool.allocate(16, alignof(int));
  pool.deallocate(first, 16, alignof(int));

  const auto allocations_before = upstream.allocations;

  void *second = pool.allocate(16, alignof(int));

  const bool reused =
      second == first && upstream.allocations == allocations_before;

  pool.deallocate(second, 16, alignof(int));
  pool.release();

  return reused && upstream.outstanding == 0;
}

bool pool_handles_small_requested_alignment() {
  counting_resource upstream;

  tested::pmr::unsynchronized_pool_resource pool(&upstream);

  void *pointer = pool.allocate(1, 1);

  const auto address = reinterpret_cast<decltype(sizeof(0))>(pointer);

  const bool aligned = address % alignof(void *) == 0;

  pool.deallocate(pointer, 1, 1);
  pool.release();

  return aligned && upstream.outstanding == 0;
}

bool pool_release_reclaims_oversized_allocations() {
  counting_resource upstream;

  tested::pmr::pool_options options{};
  options.largest_required_pool_block = 32;

  tested::pmr::unsynchronized_pool_resource pool(options, &upstream);

  void *pointer = pool.allocate(512, alignof(tested::max_align_t));
  (void)pointer;

  if (upstream.outstanding == 0)
    return false;

  /*
   * Deliberately do not deallocate pointer.
   * release() is required to release all memory obtained from upstream.
   */
  pool.release();

  return upstream.outstanding == 0 &&
         upstream.allocations == upstream.deallocations;
}

bool pool_individual_oversized_deallocation_works() {
  counting_resource upstream;

  tested::pmr::pool_options options{};
  options.largest_required_pool_block = 32;

  tested::pmr::unsynchronized_pool_resource pool(options, &upstream);

  void *pointer = pool.allocate(512, alignof(tested::max_align_t));

  pool.deallocate(pointer, 512, alignof(tested::max_align_t));

  const bool released_immediately = upstream.outstanding == 0;

  pool.release();

  return released_immediately && upstream.outstanding == 0;
}

bool monotonic_release_restores_initial_buffer() {
  counting_resource upstream;

  alignas(tested::max_align_t) unsigned char storage[128];

  tested::pmr::monotonic_buffer_resource arena(storage, sizeof storage,
                                               &upstream);

  void *first = arena.allocate(16, alignof(tested::max_align_t));

  /*
   * Force an upstream allocation.
   */
  (void)arena.allocate(1024, alignof(tested::max_align_t));

  if (upstream.outstanding == 0)
    return false;

  arena.release();

  if (upstream.outstanding != 0)
    return false;

  void *after_release = arena.allocate(16, alignof(tested::max_align_t));

  /*
   * release() should reset the resource back to the original user buffer.
   */
  return after_release == first;
}

bool over_aligned_new_delete_resource_works() {
  auto *resource = tested::pmr::new_delete_resource();

  void *pointer =
      resource->allocate(sizeof(over_aligned), alignof(over_aligned));

  const auto address = reinterpret_cast<decltype(sizeof(0))>(pointer);

  const bool aligned = address % alignof(over_aligned) == 0;

  resource->deallocate(pointer, sizeof(over_aligned), alignof(over_aligned));

  return aligned;
}

bool default_resource_works() {
  alignas(tested::max_align_t) unsigned char storage[64];
  tested::pmr::monotonic_buffer_resource arena(storage, sizeof storage);

  auto *old = tested::pmr::set_default_resource(&arena);

  const bool result = tested::pmr::get_default_resource() == &arena;

  tested::pmr::set_default_resource(old);

  return result;
}

bool synchronized_pool_basic_works() {
  counting_resource upstream;

  tested::pmr::synchronized_pool_resource pool(&upstream);

  void *pointer = pool.allocate(32, alignof(tested::max_align_t));

  pool.deallocate(pointer, 32, alignof(tested::max_align_t));
  pool.release();

  return upstream.outstanding == 0;
}

bool ftl_test() {
  return polymorphic_allocator_works() && pool_reuses_blocks() &&
         pool_handles_small_requested_alignment() &&
         pool_release_reclaims_oversized_allocations() &&
         pool_individual_oversized_deallocation_works() &&
         monotonic_release_restores_initial_buffer() &&
         over_aligned_new_delete_resource_works() && default_resource_works() &&
         synchronized_pool_basic_works();
}
