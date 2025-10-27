#ifndef SIMD_ARM64_H
#define SIMD_ARM64_H

#include "arm_neon.h"
#include <cstdint>
#include <array>

namespace bla_ga
{

  // --- mask64 SIMD specialization ---
  template <>
  class SIMD<mask64, 2>
  {
    int64x2_t val;

  public:
    SIMD() = default;
    SIMD(int64x2_t _val) : val(_val) {}
    SIMD(mask64 v0, mask64 v1)
    {
      int64_t arr[2] = {static_cast<int64_t>(v0), static_cast<int64_t>(v1)};
      val = vld1q_s64(arr);
    }

    auto Val() const { return val; }

    mask64 operator[](size_t i) const
    {
      return reinterpret_cast<const int64_t *>(&val)[i] != 0;
    }

    static constexpr int Size() { return 2; }
  };

  // Logical NOT
  inline SIMD<mask64, 2> operator!(const SIMD<mask64, 2> &m)
  {
    uint64x2_t ones = vdupq_n_u64(~0ULL);
    return SIMD<mask64, 2>(vreinterpretq_s64_u64(veorq_u64(vreinterpretq_u64_s64(m.Val()), ones)));
  }

  // Logical AND
  inline SIMD<mask64, 2> operator&(const SIMD<mask64, 2> &a, const SIMD<mask64, 2> &b)
  {
    return SIMD<mask64, 2>(vandq_s64(a.Val(), b.Val()));
  }

  // Logical OR
  inline SIMD<mask64, 2> operator|(const SIMD<mask64, 2> &a, const SIMD<mask64, 2> &b)
  {
    return SIMD<mask64, 2>(vorrq_s64(a.Val(), b.Val()));
  }

  // --- int64_t SIMD specialization ---
  template <>
  class SIMD<int64_t, 2>
  {
    int64x2_t val;

  public:
    SIMD() = default;
    SIMD(int64x2_t _val) : val(_val) {}
    SIMD(int64_t v0, int64_t v1)
    {
      int64_t arr[2] = {v0, v1};
      val = vld1q_s64(arr);
    }
    SIMD(int64_t x) : val(vdupq_n_s64(x)) {}

    auto Val() const { return val; }

    static constexpr int Size() { return 2; }

    int64_t operator[](size_t i) const
    {
      return reinterpret_cast<const int64_t *>(&val)[i];
    }
  };

  // Comparisons
  inline SIMD<mask64, 2> operator<(const SIMD<int64_t, 2> &a, const SIMD<int64_t, 2> &b)
  {
    return SIMD<mask64, 2>(vreinterpretq_s64_u64(vcltq_s64(a.Val(), b.Val())));
  }

  inline SIMD<mask64, 2> operator>(const SIMD<int64_t, 2> &a, const SIMD<int64_t, 2> &b)
  {
    return SIMD<mask64, 2>(vreinterpretq_s64_u64(vcgtq_s64(a.Val(), b.Val())));
  }

  inline SIMD<mask64, 2> operator==(const SIMD<int64_t, 2> &a, const SIMD<int64_t, 2> &b)
  {
    return SIMD<mask64, 2>(vreinterpretq_s64_u64(vceqq_s64(a.Val(), b.Val())));
  }

  // --- double SIMD specialization ---
  template <>
  class SIMD<double, 2>
  {
    float64x2_t val;

  public:
    SIMD() = default;
    SIMD(float64x2_t _val) : val(_val) {}
    SIMD(double a, double b)
    {
      double arr[2] = {a, b};
      val = vld1q_f64(arr);
    }
    SIMD(double x) : val(vdupq_n_f64(x)) {}

    static SIMD<double, 2> set(double a, double b)
    {
      return SIMD<double, 2>(a, b);
    }

    auto Val() const { return val; }
    static constexpr int Size() { return 2; }

    void Store(double *p) const { vst1q_f64(p, val); }
  };

  // Arithmetic
  inline SIMD<double, 2> operator+(const SIMD<double, 2> &a, const SIMD<double, 2> &b)
  {
    return SIMD<double, 2>(vaddq_f64(a.Val(), b.Val()));
  }

  inline SIMD<double, 2> operator-(const SIMD<double, 2> &a, const SIMD<double, 2> &b)
  {
    return SIMD<double, 2>(vsubq_f64(a.Val(), b.Val()));
  }

  inline SIMD<double, 2> operator*(const SIMD<double, 2> &a, const SIMD<double, 2> &b)
  {
    return SIMD<double, 2>(vmulq_f64(a.Val(), b.Val()));
  }

  // Fused multiply-add
  inline SIMD<double, 2> FMA(const SIMD<double, 2> &a, const SIMD<double, 2> &b, const SIMD<double, 2> &c)
  {
    return SIMD<double, 2>(vfmaq_f64(c.Val(), a.Val(), b.Val()));
  }

  // Select (mask ? b : c)
  inline SIMD<double, 2> Select(const SIMD<mask64, 2> &mask,
                                const SIMD<double, 2> &b,
                                const SIMD<double, 2> &c)
  {
    return SIMD<double, 2>(vbslq_f64(vreinterpretq_u64_s64(mask.Val()), b.Val(), c.Val()));
  }

} // namespace bla_ga

#endif // SIMD_ARM64_H
