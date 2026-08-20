// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_DETAIL_WBUFFER_CONVERT_HPP
#define FTL_DETAIL_WBUFFER_CONVERT_HPP

FTL_BEGIN_NAMESPACE

template <class Codecvt, class Elem = wchar_t, class Tr = char_traits<Elem>>
class wbuffer_convert : public basic_streambuf<Elem, Tr> {
public:
  using state_type = typename Codecvt::state_type;
  using int_type = typename Tr::int_type;

  wbuffer_convert() : wbuffer_convert(nullptr) {}

  explicit wbuffer_convert(streambuf *byte_buffer,
                           Codecvt *conversion = new Codecvt,
                           state_type state = state_type())
      : byte_buffer_(byte_buffer), conversion_(conversion), state_(state),
        external_(conversion_buffer_size(conversion)),
        pending_(conversion_buffer_size(conversion)) {}

  ~wbuffer_convert() override {
#if FTL_HAS_EXCEPTIONS
    try {
#endif
    (void)flush_shift_state();
#if FTL_HAS_EXCEPTIONS
    } catch (...) {
    }
#endif
    delete conversion_;
  }

  wbuffer_convert(const wbuffer_convert &) = delete;
  wbuffer_convert &operator=(const wbuffer_convert &) = delete;

  streambuf *rdbuf() const { return byte_buffer_; }

  streambuf *rdbuf(streambuf *byte_buffer) {
    streambuf *previous = byte_buffer_;
    byte_buffer_ = byte_buffer;
    return previous;
  }

  state_type state() const { return state_; }

protected:
  int_type underflow() override {
    if (byte_buffer_ == nullptr)
      return Tr::eof();

    for (;;) {
      Elem *internal_next = input_;
      const char *external_next = pending_.data();
      const auto status = conversion_->in(
          state_, pending_.data(), pending_.data() + pending_size_, external_next,
          input_,
          input_ + input_capacity, internal_next);

      const size_t consumed =
          static_cast<size_t>(external_next - pending_.data());
      if (consumed != 0) {
        for (size_t index = consumed; index < pending_size_; ++index)
          pending_[index - consumed] = pending_[index];
        pending_size_ -= consumed;
      }

      if (internal_next != input_) {
        this->setg(input_, input_, internal_next);
        return Tr::to_int_type(*input_);
      }

      if (status == codecvt_base::noconv) {
        int external;
        if (pending_size_ != 0) {
          external = char_traits<char>::to_int_type(pending_[0]);
          for (size_t index = 1; index < pending_size_; ++index)
            pending_[index - 1] = pending_[index];
          --pending_size_;
        } else {
          external = byte_buffer_->sbumpc();
        }
        if (char_traits<char>::eq_int_type(external,
                                          char_traits<char>::eof()))
          return Tr::eof();
        input_[0] = static_cast<Elem>(
            char_traits<char>::to_char_type(external));
        this->setg(input_, input_, input_ + 1);
        return Tr::to_int_type(input_[0]);
      }

      if (status == codecvt_base::error)
        return Tr::eof();

      if (pending_size_ == pending_.size())
        pending_.resize(pending_.size() * 2);

      const auto external = byte_buffer_->sbumpc();
      if (char_traits<char>::eq_int_type(external, char_traits<char>::eof()))
        return Tr::eof();
      pending_[pending_size_++] = char_traits<char>::to_char_type(external);
    }
  }

  int_type overflow(int_type value = Tr::eof()) override {
    if (byte_buffer_ == nullptr)
      return Tr::eof();
    if (Tr::eq_int_type(value, Tr::eof()))
      return flush_shift_state() == 0 ? Tr::not_eof(value) : Tr::eof();

    const Elem input = Tr::to_char_type(value);
    const Elem *first = &input;
    const Elem *last = first + 1;
    while (first != last) {
      const Elem *from_next = first;
      char *to_next = external_.data();
      const auto status = conversion_->out(
          state_, first, last, from_next, external_.data(),
          external_.data() + external_.size(), to_next);
      const streamsize produced =
          static_cast<streamsize>(to_next - external_.data());
      if (produced != 0 &&
          byte_buffer_->sputn(external_.data(), produced) != produced)
        return Tr::eof();
      if (status == codecvt_base::noconv) {
        const char external = static_cast<char>(*first);
        if (char_traits<char>::eq_int_type(byte_buffer_->sputc(external),
                                           char_traits<char>::eof()))
          return Tr::eof();
        ++first;
        continue;
      }
      if (status == codecvt_base::error ||
          (from_next == first && produced == 0))
        return Tr::eof();
      first = from_next;
    }

    return value;
  }

  int sync() override {
    if (byte_buffer_ == nullptr)
      return -1;
    if (flush_shift_state() != 0)
      return -1;
    return byte_buffer_->pubsync();
  }

private:
  int flush_shift_state() {
    if (byte_buffer_ == nullptr || conversion_ == nullptr)
      return byte_buffer_ == nullptr ? -1 : 0;

    for (;;) {
      char *next = external_.data();
      const auto status = conversion_->unshift(
          state_, external_.data(), external_.data() + external_.size(), next);
      const streamsize produced =
          static_cast<streamsize>(next - external_.data());
      if (produced != 0 &&
          byte_buffer_->sputn(external_.data(), produced) != produced)
        return -1;
      if (status == codecvt_base::ok || status == codecvt_base::noconv)
        return 0;
      if (status == codecvt_base::error || produced == 0)
        return -1;
    }
  }

  static size_t conversion_buffer_size(Codecvt *conversion) {
    const int maximum = conversion->max_length();
    return static_cast<size_t>(maximum > 0 ? maximum : 1) + 16;
  }

  static constexpr size_t input_capacity = 32;
  streambuf *byte_buffer_{};
  Codecvt *conversion_{};
  state_type state_{};
  vector<char> external_;
  vector<char> pending_;
  Elem input_[input_capacity]{};
  size_t pending_size_{};
};

FTL_END_NAMESPACE

#endif
