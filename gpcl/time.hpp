#ifndef GPCL_TIME_HPP
#define GPCL_TIME_HPP

#include <gpcl/assert.hpp>
#include <gpcl/detail/config.hpp>
#include <gpcl/error.hpp>
#include <gpcl/inttypes.hpp>
#include <gpcl/narrow_cast.hpp>
#include <limits>
#include <stdexcept>
#include <time.h>

namespace gpcl {

inline namespace time {

class duration;
class instant;
class system_time;
class system_time_error;

class duration
{
  u64 secs_{};
  u32 nanos_{};

public:
  constexpr duration() = default;

  GPCL_DECL_INLINE constexpr duration(u64 secs, u32 nanos)
      : secs_(secs),
        nanos_(nanos)
  {
    GPCL_ASSERT(nanos < 999'999'999);
  }

  GPCL_DECL_INLINE static constexpr duration from_secs(u64 secs)
  {
    return duration(secs, 0);
  }

  GPCL_DECL_INLINE static constexpr duration from_millis(u64 millis)
  {
    return duration(millis / 1000, millis % 1000 * 1'000'000);
  }

  GPCL_DECL_INLINE static constexpr duration from_micros(u64 micros)
  {
    return duration(micros / 1'000'000, micros % 1'000'000 * 100);
  }

  GPCL_DECL_INLINE static constexpr duration from_nanos(u64 nanos)
  {
    return duration(nanos / 1'000'000'000, nanos % 1'000'000'000);
  }

  GPCL_DECL_INLINE constexpr bool is_zero() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);
    return secs_ == 0 && nanos_ == 0;
  }

  GPCL_DECL_INLINE constexpr u64 as_secs() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);

    return secs_;
  }

  GPCL_DECL_INLINE constexpr u32 subsec_millis() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);

    return nanos_ / 1'000;
  }

  GPCL_DECL_INLINE constexpr u32 subsec_micros() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);

    return nanos_ / 1'000'000;
  }

  GPCL_DECL_INLINE constexpr u32 subsec_nanos() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);

    return nanos_;
  }

  GPCL_DECL_INLINE constexpr u128 as_millis() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);
    return u128(secs_) * 1000 + nanos_ / 1'000'000;
  }

  GPCL_DECL_INLINE constexpr u128 as_micros() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);
    return u128(secs_) * 1'000'000 + nanos_ / 1'000;
  }

  GPCL_DECL_INLINE constexpr u128 as_nanos() const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);
    return u128(secs_) * 1'000'000'000 + nanos_;
  }

  GPCL_DECL_INLINE constexpr duration checked_add(const duration &rhs) const
  {
    GPCL_ASSERT(nanos_ < 1'000'000'000);
    GPCL_ASSERT(rhs.nanos_ < 1'000'000'000);
    u64 secs = secs_ + rhs.secs_;
    u32 nanos = nanos_ + rhs.nanos_;
    ;
    secs += nanos / 1'000'000'000;
    nanos = nanos % 1'000'000'000;
    if (secs < secs_ || secs < rhs.secs_)
    {
      GPCL_THROW(std::overflow_error("checked_add"));
    }
    return {secs, nanos};
  }

  GPCL_DECL_INLINE constexpr timespec to_timespec() const
  {
    timespec retv{};
    retv.tv_sec = narrow<time_t>(secs_);
    retv.tv_nsec = narrow<long>(nanos_);
    return retv;
  }

  static const duration second;
  static const duration millisecond;
  static const duration microsecond;
  static const duration nanosecond;
  static const duration zero;
  static const duration max;
};

inline const duration duration::second = duration(1, 0);
inline const duration duration::millisecond = duration(0, 1000000);
inline const duration duration::microsecond = duration(0, 1000);
inline const duration duration::nanosecond = duration(0, 1);
inline const duration duration::zero = duration(0, 0);

inline const duration duration::max =
    duration((std::numeric_limits<u64>::max)(), 999'999'999);

} // namespace time

} // namespace gpcl

#endif // !GPCL_TIME_HPP
