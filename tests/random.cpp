#ifdef FTL_REPLACE_STL
#include <random>
#include <type_traits>
namespace tested = std;
#else
#include <ftl/random>
#include <ftl/type_traits>
namespace tested = ftl;
#endif

static_assert(tested::uniform_random_bit_generator<tested::minstd_rand0>);
static_assert(tested::uniform_random_bit_generator<tested::mt19937>);
static_assert(tested::uniform_random_bit_generator<tested::ranlux48_base>);
static_assert(tested::is_same_v<tested::seed_seq::result_type,
                                tested::uint_least32_t>);

struct maximum_engine {
  using result_type = tested::uint64_t;
  static constexpr result_type min() { return 0; }
  static constexpr result_type max() { return tested::uint64_t(-1); }
  result_type operator()() { return max(); }
};
static_assert(tested::uniform_random_bit_generator<maximum_engine>);

struct malformed_seed_sequence {
  using result_type = int;
  int conversions{};
  int generations{};
  operator tested::mt19937::result_type() {
    ++conversions;
    return 0;
  }
  void generate(tested::uint32_t *, tested::uint32_t *) { ++generations; }
};

class random_input_buffer : public tested::streambuf {
public:
  random_input_buffer(char *first, char *last) { setg(first, first, last); }
};

class random_output_buffer : public tested::streambuf {
public:
  tested::string text;
protected:
  int_type overflow(int_type value) override {
    if (traits_type::eq_int_type(value, traits_type::eof()))
      return traits_type::not_eof(value);
    text.push_back(traits_type::to_char_type(value));
    return value;
  }
  tested::streamsize xsputn(const char *source,
                            tested::streamsize count) override {
    text.append(source, static_cast<tested::size_t>(count));
    return count;
  }
};

template <class Distribution>
concept complete_distribution_surface = requires(
    Distribution distribution, const Distribution constant_distribution,
    typename Distribution::param_type parameters, tested::mt19937 &engine) {
  typename Distribution::result_type;
  typename Distribution::param_type;
  typename Distribution::param_type::distribution_type;
  requires tested::is_same_v<
      typename Distribution::param_type::distribution_type, Distribution>;
  Distribution{};
  Distribution{parameters};
  { distribution.reset() } -> tested::same_as<void>;
  { distribution(engine) } -> tested::same_as<typename Distribution::result_type>;
  { distribution(engine, parameters) } -> tested::same_as<typename Distribution::result_type>;
  { constant_distribution.param() } -> tested::same_as<typename Distribution::param_type>;
  { distribution.param(parameters) } -> tested::same_as<void>;
  { constant_distribution.min() } -> tested::same_as<typename Distribution::result_type>;
  { constant_distribution.max() } -> tested::same_as<typename Distribution::result_type>;
  { constant_distribution == constant_distribution } -> tested::same_as<bool>;
  { tested::declval<tested::ostream &>() << constant_distribution } ->
      tested::same_as<tested::ostream &>;
  { tested::declval<tested::istream &>() >> distribution } ->
      tested::same_as<tested::istream &>;
};

static_assert(complete_distribution_surface<tested::uniform_int_distribution<>>);
static_assert(complete_distribution_surface<tested::uniform_real_distribution<>>);
static_assert(complete_distribution_surface<tested::bernoulli_distribution>);
static_assert(complete_distribution_surface<tested::binomial_distribution<>>);
static_assert(complete_distribution_surface<tested::geometric_distribution<>>);
static_assert(complete_distribution_surface<tested::negative_binomial_distribution<>>);
static_assert(complete_distribution_surface<tested::poisson_distribution<>>);
static_assert(complete_distribution_surface<tested::exponential_distribution<>>);
static_assert(complete_distribution_surface<tested::gamma_distribution<>>);
static_assert(complete_distribution_surface<tested::weibull_distribution<>>);
static_assert(complete_distribution_surface<tested::extreme_value_distribution<>>);
static_assert(complete_distribution_surface<tested::normal_distribution<>>);
static_assert(complete_distribution_surface<tested::lognormal_distribution<>>);
static_assert(complete_distribution_surface<tested::chi_squared_distribution<>>);
static_assert(complete_distribution_surface<tested::cauchy_distribution<>>);
static_assert(complete_distribution_surface<tested::fisher_f_distribution<>>);
static_assert(complete_distribution_surface<tested::student_t_distribution<>>);
static_assert(complete_distribution_surface<tested::discrete_distribution<>>);
static_assert(complete_distribution_surface<tested::piecewise_constant_distribution<>>);
static_assert(complete_distribution_surface<tested::piecewise_linear_distribution<>>);

template <class Engine>
concept complete_engine_surface =
    tested::uniform_random_bit_generator<Engine> &&
    requires(Engine engine, const Engine constant_engine,
             typename Engine::result_type seed, tested::seed_seq &sequence) {
      typename Engine::result_type;
      Engine{};
      Engine{seed};
      Engine{sequence};
      { engine.seed() } -> tested::same_as<void>;
      { engine.seed(seed) } -> tested::same_as<void>;
      { engine.seed(sequence) } -> tested::same_as<void>;
      { engine() } -> tested::same_as<typename Engine::result_type>;
      { engine.discard(1) } -> tested::same_as<void>;
      { constant_engine == constant_engine } -> tested::same_as<bool>;
      { tested::declval<tested::ostream &>() << constant_engine } ->
          tested::same_as<tested::ostream &>;
      { tested::declval<tested::istream &>() >> engine } ->
          tested::same_as<tested::istream &>;
    };

static_assert(complete_engine_surface<tested::minstd_rand0>);
static_assert(complete_engine_surface<tested::mt19937>);
static_assert(complete_engine_surface<tested::ranlux48_base>);
static_assert(complete_engine_surface<tested::ranlux24>);
static_assert(complete_engine_surface<tested::knuth_b>);
static_assert(complete_engine_surface<
    tested::independent_bits_engine<tested::minstd_rand, 16, unsigned int>>);

static_assert(tested::ranlux24::block_size == 223);
static_assert(tested::ranlux24::used_block == 23);
static_assert(tested::knuth_b::table_size == 256);
static_assert(tested::mt19937::word_size == 32);
static_assert(tested::mt19937::state_size == 624);
static_assert(tested::ranlux24_base::word_size == 24);
static_assert(tested::ranlux24_base::short_lag == 10);
static_assert(tested::ranlux24_base::long_lag == 24);

template <class Engine, class Expected>
bool required_sequence(Expected expected) {
  Engine engine;
  typename Engine::result_type value{};
  for (int i = 0; i < 10000; ++i)
    value = engine();
  return value == expected;
}

bool engine_sequences_match_n4950() {
  return required_sequence<tested::minstd_rand0>(1043618065u) &&
         required_sequence<tested::minstd_rand>(399268537u) &&
         required_sequence<tested::mt19937>(4123659995u) &&
         required_sequence<tested::mt19937_64>(9981545732273789042ull) &&
         required_sequence<tested::ranlux24_base>(7937952u) &&
         required_sequence<tested::ranlux48_base>(61839128582725ull) &&
         required_sequence<tested::ranlux24>(9901578u) &&
         required_sequence<tested::ranlux48>(249142670248501ull) &&
         required_sequence<tested::knuth_b>(1112339016u);
}

bool seed_sequence_matches_known_vectors() {
  constexpr tested::uint32_t expected[] = {
      885567924u,  1497546072u, 535818408u,  125127419u,  1352728756u,
      1388689836u, 3943292234u, 2948707708u, 3339389554u, 3065408602u};
  tested::seed_seq sequence{1u, 2u, 3u, 4u};
  tested::uint32_t generated[10]{};
  sequence.generate(generated, generated + 10);
  for (int i = 0; i < 10; ++i)
    if (generated[i] != expected[i])
      return false;

  tested::seed_seq lcg_seed{1u, 2u, 3u, 4u};
  tested::minstd_rand lcg(lcg_seed);
  constexpr tested::uint_fast32_t expected_lcg[] = {
      1631472685u, 297674851u, 249650544u, 1350666107u, 400128077u};
  for (auto value : expected_lcg)
    if (lcg() != value)
      return false;

  tested::seed_seq mt_seed{1u, 2u, 3u, 4u};
  tested::mt19937 mt(mt_seed);
  constexpr tested::uint_fast32_t expected_mt[] = {
      2103621173u, 3113074417u, 3119520880u, 1733660703u, 1996723807u};
  for (auto value : expected_mt)
    if (mt() != value)
      return false;
  return true;
}

bool random_device_uses_platform_entropy() {
  tested::random_device first;
  tested::random_device second;
#if defined(_WIN32) || defined(__APPLE__) ||                              \
    (defined(__linux__) && defined(__x86_64__))
  if (first.entropy() <= 0.0)
    return false;
  for (int i = 0; i < 8; ++i)
    if (first() != second())
      return true;
  return false;
#else
  return first.entropy() == 0.0;
#endif
}

bool uniform_distributions_stay_in_range() {
  tested::mt19937 engine(42);
  tested::uniform_int_distribution<int> integers(-7, 13);
  tested::uniform_real_distribution<double> reals(-2.5, 4.0);
  tested::uniform_int_distribution<unsigned int> full(
      0, tested::numeric_limits<unsigned int>::max());
  for (int i = 0; i < 1000; ++i) {
    const int integer = integers(engine);
    const double real = reals(engine);
    (void)full(engine);
    if (integer < -7 || integer > 13 || real < -2.5 || real >= 4.0)
      return false;
  }
  return true;
}

bool generate_canonical_stays_below_one() {
  maximum_engine engine;
  const double value = tested::generate_canonical<double, 53>(engine);
  return value >= 0.0 && value < 1.0;
}

bool convertible_seed_uses_value_constructor() {
  malformed_seed_sequence source;
  tested::mt19937 engine(source);
  (void)engine;
  return source.conversions == 1 && source.generations == 0;
}

bool scalar_distributions_work() {
  tested::mt19937 engine(123);
  tested::bernoulli_distribution bernoulli(0.25);
  tested::geometric_distribution<int> geometric(0.25);
  tested::exponential_distribution<double> exponential(2.0);
  tested::normal_distribution<double> normal(3.0, 2.0);
  tested::gamma_distribution<double> gamma(2.0, 3.0);
  tested::lognormal_distribution<double> lognormal(0.0, 1.0);
  tested::weibull_distribution<double> weibull(2.0, 3.0);
  tested::extreme_value_distribution<double> extreme(1.0, 2.0);
  tested::cauchy_distribution<double> cauchy(1.0, 2.0);
  tested::binomial_distribution<int> binomial(100, 0.4);
  tested::negative_binomial_distribution<int> negative_binomial(5, 0.4);
  tested::poisson_distribution<int> poisson_small(4.0);
  tested::poisson_distribution<int> poisson_large(100.0);
  tested::chi_squared_distribution<double> chi_squared(4.0);
  tested::fisher_f_distribution<double> fisher(4.0, 6.0);
  tested::student_t_distribution<double> student(8.0);
  tested::discrete_distribution<int> discrete{1.0, 2.0, 3.0};
  const double boundaries[] = {0.0, 1.0, 3.0};
  const double weights[] = {1.0, 2.0};
  tested::piecewise_constant_distribution<double> piecewise(
      boundaries, boundaries + 3, weights);
  const double linear_weights[] = {1.0, 2.0, 1.0};
  tested::piecewise_linear_distribution<double> piecewise_linear(
      boundaries, boundaries + 3, linear_weights);
  bool saw_true = false;
  bool saw_false = false;
  for (int i = 0; i < 1000; ++i) {
    const bool bit = bernoulli(engine);
    saw_true |= bit;
    saw_false |= !bit;
    if (geometric(engine) < 0 || exponential(engine) < 0.0)
      return false;
    (void)normal(engine);
    if (gamma(engine) < 0.0 || lognormal(engine) < 0.0 || weibull(engine) < 0.0)
      return false;
    (void)extreme(engine);
    (void)cauchy(engine);
    const int binomial_value = binomial(engine);
    if (binomial_value < 0 || binomial_value > 100 ||
        negative_binomial(engine) < 0 || poisson_small(engine) < 0 ||
        poisson_large(engine) < 0)
      return false;
    if (chi_squared(engine) < 0.0 || fisher(engine) < 0.0)
      return false;
    (void)student(engine);
    const int category = discrete(engine);
    const double piecewise_value = piecewise(engine);
    const double linear_value = piecewise_linear(engine);
    if (category < 0 || category > 2 || piecewise_value < 0.0 ||
        piecewise_value >= 3.0 || linear_value < 0.0 || linear_value >= 3.0)
      return false;
  }
  normal.reset();
  return saw_true && saw_false;
}

bool distribution_moments_are_sane() {
  tested::mt19937_64 engine(987654321);
  tested::binomial_distribution<int> binomial(100, 0.4);
  tested::poisson_distribution<int> poisson(100.0);
  tested::normal_distribution<double> normal(3.0, 2.0);
  tested::gamma_distribution<double> gamma(2.0, 3.0);
  constexpr int samples = 50000;
  double binomial_sum = 0;
  double poisson_sum = 0;
  double normal_sum = 0;
  double gamma_sum = 0;
  for (int i = 0; i < samples; ++i) {
    binomial_sum += binomial(engine);
    poisson_sum += poisson(engine);
    normal_sum += normal(engine);
    gamma_sum += gamma(engine);
  }
  const auto near = [](double value, double expected, double tolerance) {
    return value > expected - tolerance && value < expected + tolerance;
  };
  return near(binomial_sum / samples, 40.0, 0.15) &&
         near(poisson_sum / samples, 100.0, 0.35) &&
         near(normal_sum / samples, 3.0, 0.06) &&
         near(gamma_sum / samples, 6.0, 0.09);
}

template <class Random>
bool stream_round_trip(Random value) {
  random_output_buffer output;
  tested::ostream destination(&output);
  destination.setf(tested::ios_base::hex, tested::ios_base::basefield);
  destination.fill('_');
  const auto flags = destination.flags();
  destination << value;
  if (!destination || destination.flags() != flags || destination.fill() != '_')
    return false;
  random_input_buffer input(output.text.data(),
                            output.text.data() + output.text.size());
  tested::istream source(&input);
  Random restored;
  source >> restored;
  return source && restored == value;
}

bool stream_state_round_trips_work() {
  tested::mt19937 engine(42);
  engine.discard(37);
  tested::ranlux24 adaptor(17);
  adaptor.discard(13);
  tested::normal_distribution<double> normal(2.0, 3.0);
  (void)normal(engine);
  tested::lognormal_distribution<double> lognormal(1.0, 0.5);
  (void)lognormal(engine);
  tested::discrete_distribution<int> discrete{1.0, 2.0, 4.0};
  const double boundaries[] = {0.0, 1.0, 3.0};
  const double weights[] = {1.0, 2.0, 1.0};
  tested::piecewise_linear_distribution<double> piecewise(
      boundaries, boundaries + 3, weights);
  return stream_round_trip(engine) && stream_round_trip(adaptor) &&
         stream_round_trip(normal) && stream_round_trip(lognormal) &&
         stream_round_trip(discrete) && stream_round_trip(piecewise);
}

bool malformed_stream_preserves_state() {
  char text[] = {'n', 'o', 't', '-', 'a', '-', 's', 't', 'a', 't', 'e'};
  random_input_buffer input(text, text + sizeof(text));
  tested::istream source(&input);
  tested::minstd_rand value(123);
  tested::minstd_rand original = value;
  source >> value;
  return source.fail() && value == original;
}

bool ftl_test() {
  return engine_sequences_match_n4950() && seed_sequence_matches_known_vectors() &&
         random_device_uses_platform_entropy() &&
         generate_canonical_stays_below_one() &&
         convertible_seed_uses_value_constructor() &&
         uniform_distributions_stay_in_range() && scalar_distributions_work() &&
         distribution_moments_are_sane() && stream_state_round_trips_work() &&
         malformed_stream_preserves_state();
}
