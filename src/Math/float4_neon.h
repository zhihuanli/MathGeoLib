/* Copyright Jukka Jyl�nki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file float4_neon.h
	@author Jukka Jyl�nki
	@brief ARM NEON code for float4-related computations. */

#pragma once

#ifdef MATH_SIMD

FORCE_INLINE simd4f vec4_add_float(simd4f vec, float f)
{
#ifdef MATH_SSE
	return _mm_add_ps(vec, _mm_set1_ps(f));
#elif defined(MATH_NEON)
	return vaddq_f32(vec, vdupq_n_f32(f));
#endif
}

FORCE_INLINE simd4f vec4_add_vec4(simd4f vec, simd4f vec2)
{
#ifdef MATH_SSE
	return _mm_add_ps(vec, vec2);
#elif defined(MATH_NEON)
	return vaddq_f32(vec, vec2);
#endif
}

FORCE_INLINE simd4f vec4_sub_float(simd4f vec, float f)
{
#ifdef MATH_SSE
	return _mm_sub_ps(vec, _mm_set1_ps(f));
#elif defined(MATH_NEON)
	return vsubq_f32(vec, vdupq_n_f32(f));
#endif
}

FORCE_INLINE simd4f float_sub_vec4(float f, simd4f vec)
{
#ifdef MATH_SSE
	return _mm_sub_ps(_mm_set1_ps(f), vec);
#elif defined(MATH_NEON)
	return vsubq_f32(vdupq_n_f32(f), vec);
#endif
}

FORCE_INLINE simd4f vec4_sub_vec4(simd4f vec, simd4f vec2)
{
#ifdef MATH_SSE
	return _mm_sub_ps(vec, vec2);
#elif defined(MATH_NEON)
	return vsubq_f32(vec, vec2);
#endif
}

#ifdef MATH_NEON
FORCE_INLINE simd4f negate_ps(simd4f vec)
{
	return float_sub_vec4(0.f, vec);
}
#endif

FORCE_INLINE simd4f vec4_mul_float(simd4f vec, float f)
{
#ifdef MATH_SSE
	return _mm_mul_ps(vec, _mm_set1_ps(f));
#elif defined(MATH_NEON)
	return vmulq_f32(vec, vdupq_n_f32(f));
#endif
}

FORCE_INLINE simd4f vec4_mul_vec4(simd4f vec, simd4f vec2)
{
#ifdef MATH_SSE
	return _mm_mul_ps(vec, vec2);
#elif defined(MATH_NEON)
	return vmulq_f32(vec, vec2);
#endif
}

FORCE_INLINE simd4f vec4_div_float(simd4f vec, float f)
{
#ifdef MATH_SSE
	return _mm_div_ps(vec, _mm_set1_ps(f));
#elif defined(MATH_NEON)
	simd4f v = vdupq_n_f32(f);
	simd4f rcp = vrecpeq_f32(v);
	rcp = vmulq_f32(vrecpsq_f32(v, rcp), rcp);
	rcp = vmulq_f32(vrecpsq_f32(v, rcp), rcp);
	return vmulq_f32(vec, rcp);
#endif
}

FORCE_INLINE simd4f float_div_vec4(float f, simd4f vec)
{
#ifdef MATH_SSE
	return _mm_div_ps(_mm_set1_ps(f), vec);
#elif defined(MATH_NEON)
	simd4f rcp = vrecpeq_f32(vec);
	rcp = vmulq_f32(vrecpsq_f32(vec, rcp), rcp);
	rcp = vmulq_f32(vrecpsq_f32(vec, rcp), rcp);
	return vmulq_f32(vdupq_n_f32(f), rcp);
#endif
}

FORCE_INLINE simd4f vec4_recip(simd4f vec)
{
#ifdef MATH_SSE
	__m128 e = _mm_rcp_ps(vec); // Do one iteration of Newton-Rhapson: e_n = 2*e - x*e^2
	return _mm_sub_ps(_mm_add_ps(e, e), _mm_mul_ps(vec, _mm_mul_ps(e,e)));
#elif defined(MATH_NEON)
	simd4f rcp = vrecpeq_f32(vec);
	rcp = vmulq_f32(vrecpsq_f32(vec, rcp), rcp);
	rcp = vmulq_f32(vrecpsq_f32(vec, rcp), rcp);
	return rcp;
#endif
}

FORCE_INLINE simd4f vec4_div_vec4(simd4f vec, simd4f vec2)
{
#ifdef MATH_SSE
	return _mm_div_ps(vec, vec2);
#elif defined(MATH_NEON)
	simd4f rcp = vrecpeq_f32(vec2);
	rcp = vmulq_f32(vrecpsq_f32(vec2, rcp), rcp);
	rcp = vmulq_f32(vrecpsq_f32(vec2, rcp), rcp);
	return vmulq_f32(vec, rcp);
#endif
}

#ifdef MATH_NEON
inline std::string ToString(uint8x8x2_t vec)
{
	uint8_t *v = (uint8_t*)&vec;
	char str[256];
	sprintf(str, "[%02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X | %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X]", 
		(int)v[15], (int)v[14], (int)v[13], (int)v[12], (int)v[11], (int)v[10], (int)v[9], (int)v[8], (int)v[7], (int)v[6], (int)v[5], (int)v[4], (int)v[3], (int)v[2], (int)v[1], (int)v[0]);
	return str;
}
#endif

#if defined(MATH_AVX) || defined(MATH_NEON)
FORCE_INLINE simd4f vec4_permute(simd4f vec, int i, int j, int k, int l)
{
#ifdef MATH_AVX
	return _mm_permutevar_ps(vec, _mm_set_epi32(l, k, j, i));
#elif defined(MATH_NEON)
	// N.B. Don't use: This has been benchmarked to be 3x slower than scalar CPU version!
	const uint8_t I = (uint8_t)i << 2;
	const uint8_t J = (uint8_t)j << 2;
	const uint8_t K = (uint8_t)k << 2;
	const uint8_t L = (uint8_t)l << 2;
	const ALIGN16 uint8_t indexData[16] = { I, I+1, I+2, I+3, J, J+1, J+2, J+3, K, K+1, K+2, K+3, L, L+1, L+2, L+3 };
	uint8x8x2_t indices;// = *(uint8x8x2_t*)&indexData;//vld2_u8(indexData);
	indices.val[0] = vld1_u8(indexData);
	indices.val[1] = vld1_u8(indexData+8);
	uint8x8x2_t src = *(uint8x8x2_t*)&vec;//vreinterpretq_s8_f32(vec);
	uint8x8x2_t dst;
	dst.val[0] = vtbl2_u8(src, indices.val[0]);  
	dst.val[1] = vtbl2_u8(src, indices.val[1]);
	return *(simd4f*)&dst;
#endif
}
#endif

#ifdef MATH_NEON

FORCE_INLINE float sum_xyzw_float(simd4f vec)
{
	float32x2_t r = vadd_f32(vget_high_f32(vec), vget_low_f32(vec));
	return vget_lane_f32(vpadd_f32(r, r), 0);
}

FORCE_INLINE float sum_xyz_float(simd4f vec)
{
	return sum_xyzw_float(vsetq_lane_f32(0.f, vec, 3));
}

FORCE_INLINE float dot4_float(simd4f a, simd4f b)
{
	simd4f mul = vmulq_f32(a, b);
	return sum_xyzw_float(mul);
}

FORCE_INLINE float dot3_float(simd4f a, simd4f b)
{
	simd4f mul = vmulq_f32(a, b);
	return sum_xyz_float(mul);
}

FORCE_INLINE simd4f dot4_ps(simd4f a, simd4f b)
{
	return vdupq_n_f32(dot4_float(a, b));
}

FORCE_INLINE simd4f dot3_ps(simd4f a, simd4f b)
{
	return vdupq_n_f32(dot3_float(a, b));
}
#endif

FORCE_INLINE float vec4_length_sq_float(simd4f vec)
{
	return dot4_float(vec, vec);
}

FORCE_INLINE simd4f vec4_length_sq_ps(simd4f vec)
{
	return dot4_ps(vec, vec);
}

FORCE_INLINE float vec3_length_sq_float(simd4f vec)
{
	return dot3_float(vec, vec);
}

FORCE_INLINE simd4f vec3_length_sq_ps(simd4f vec)
{
	return dot3_ps(vec, vec);
}

#ifdef MATH_NEON
#define SIMD4F_TO_FLOAT(vec) vget_lane_f32(vget_low_f32(vec), 0)
#define mul_ps(vec, vec2) vmulq_f32(vec, vec2)
#define sub_ps(vec, vec2) vsubq_f32(vec, vec2)
#elif defined (MATH_SSE)
#define SIMD4F_TO_FLOAT(vec) M128_TO_FLOAT(vec)
#define mul_ps(vec, vec2) _mm_mul_ps(vec, vec2)
#define sub_ps(vec, vec2) _mm_sub_ps(vec, vec2)
#endif

FORCE_INLINE simd4f vec4_rsqrt(simd4f vec)
{
#ifdef MATH_SSE
	__m128 e = _mm_rsqrt_ps(vec); // Initial estimate
	__m128 e3 = _mm_mul_ps(_mm_mul_ps(e,e), e); // Do one iteration of Newton-Rhapson: e_n = e + 0.5 * (e - x * e^3)
	return _mm_add_ps(e, _mm_mul_ps(_mm_set1_ps(0.5f), _mm_sub_ps(e, _mm_mul_ps(vec, e3))));
#elif defined(MATH_NEON)
	float32x4_t r = vrsqrteq_f32(vec);
	return vmulq_f32(vrsqrtsq_f32(vmulq_f32(r, r), vec), r);
#endif
}

FORCE_INLINE simd4f vec4_sqrt(simd4f vec)
{
	return mul_ps(vec, vec4_rsqrt(vec));
}

FORCE_INLINE float vec4_length_float(simd4f vec)
{
	return SIMD4F_TO_FLOAT(vec4_sqrt(dot4_ps(vec, vec)));
}

FORCE_INLINE simd4f vec4_length_ps(simd4f vec)
{
	return vec4_sqrt(dot4_ps(vec, vec));
}

FORCE_INLINE simd4f vec4_normalize(simd4f vec)
{
	return mul_ps(vec, vec4_rsqrt(vec4_length_sq_ps(vec)));
}

FORCE_INLINE float vec3_length_float(simd4f vec)
{
	return SIMD4F_TO_FLOAT(vec4_sqrt(dot3_ps(vec, vec)));
}

FORCE_INLINE simd4f vec3_length_ps(simd4f vec)
{
	return vec4_sqrt(dot3_ps(vec, vec));
}

FORCE_INLINE simd4f vec3_normalize(simd4f vec)
{
	return mul_ps(vec, vec4_rsqrt(vec3_length_sq_ps(vec)));
}

#endif // ~MATH_SIMD
