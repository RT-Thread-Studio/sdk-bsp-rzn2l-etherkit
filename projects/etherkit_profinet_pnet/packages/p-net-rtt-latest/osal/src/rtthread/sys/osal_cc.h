/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2017 rt-labs AB, Sweden.
 *
 * This software is licensed under the terms of the BSD 3-clause
 * license. See the file LICENSE distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef CC_H
#define CC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtthread.h>

#define CC_PACKED_BEGIN
#define CC_PACKED_END
#define CC_PACKED __attribute__ ((packed))

#define CC_FORMAT(str, arg) __attribute__ ((format (printf, str, arg)))

#if BYTE_ORDER == LITTLE_ENDIAN
#define CC_TO_LE16(x)   ((uint16_t)(x))
#define CC_TO_LE32(x)   ((uint32_t)(x))
#define CC_TO_LE64(x)   ((uint64_t)(x))
#define CC_FROM_LE16(x) ((uint16_t)(x))
#define CC_FROM_LE32(x) ((uint32_t)(x))
#define CC_FROM_LE64(x) ((uint64_t)(x))
#define CC_TO_BE16(x)   ((uint16_t)__builtin_bswap16 (x))
#if defined(__GNUC__) || defined(__clang__)
#define CC_FROM_BE16(x) ((uint16_t)__builtin_bswap16 (x))
#define CC_TO_BE32(x)   ((uint32_t)__builtin_bswap32 (x))
#define CC_FROM_BE32(x) ((uint32_t)__builtin_bswap32 (x))
#else
#define CC_FROM_BE16(x)   ( \
        (uint16_t)( \
            (((x) & 0xFF00U) >> 8) | \
            (((x) & 0x00FFU) << 8) \
        ) \
    )
#define CC_TO_BE32(x)   ( \
    (typeof(x)) ( \
        ((x) & 0xFF000000U) >> 24 | \
        ((x) & 0x00FF0000U) >> 8  | \
        ((x) & 0x0000FF00U) << 8  | \
        ((x) & 0x000000FFU) << 24 \
    ) \
)
#define CC_FROM_BE32(x)   ( \
    (typeof(x)) ( \
        ((x) & 0xFF000000U) >> 24 | \
        ((x) & 0x00FF0000U) >> 8  | \
        ((x) & 0x0000FF00U) << 8  | \
        ((x) & 0x000000FFU) << 24 \
    ) \
)
#endif
#define CC_TO_BE64(x)   ((uint64_t)__builtin_bswap64 (x))
#define CC_FROM_BE64(x) ((uint64_t)__builtin_bswap64 (x))
#else
#if defined(__GNUC__) || defined(__clang__)
#define CC_TO_LE16(x)   ((uint16_t)__builtin_bswap16 (x))
#define CC_FROM_LE16(x) ((uint16_t)__builtin_bswap16 (x))
#define CC_FROM_LE32(x) ((uint32_t)__builtin_bswap32 (x))
#define CC_TO_LE32(x)   ((uint32_t)__builtin_bswap32 (x))
#else
#define CC_TO_LE16(x)   ( \
        (uint16_t)( \
            (((x) & 0xFF00U) >> 8) | \
            (((x) & 0x00FFU) << 8) \
        ) \
    )
#define CC_FROM_LE16(x)   ( \
        (uint16_t)( \
            (((x) & 0xFF00U) >> 8) | \
            (((x) & 0x00FFU) << 8) \
        ) \
    )
#define CC_FROM_LE32(x)   ( \
    (typeof(x)) ( \
        ((x) & 0xFF000000U) >> 24 | \
        ((x) & 0x00FF0000U) >> 8  | \
        ((x) & 0x0000FF00U) << 8  | \
        ((x) & 0x000000FFU) << 24 \
    ) \
)
#define CC_TO_LE32(x)   ( \
    (typeof(x)) ( \
        ((x) & 0xFF000000U) >> 24 | \
        ((x) & 0x00FF0000U) >> 8  | \
        ((x) & 0x0000FF00U) << 8  | \
        ((x) & 0x000000FFU) << 24 \
    ) \
)
#endif
#define CC_TO_LE64(x)   ((uint64_t)__builtin_bswap64 (x))
#define CC_FROM_LE64(x) ((uint64_t)__builtin_bswap64 (x))
#define CC_TO_BE16(x)   ((uint16_t)(x))
#define CC_TO_BE32(x)   ((uint32_t)(x))
#define CC_TO_BE64(x)   ((uint64_t)(x))
#define CC_FROM_BE16(x) ((uint16_t)(x))
#define CC_FROM_BE32(x) ((uint32_t)(x))
#define CC_FROM_BE64(x) ((uint64_t)(x))
#endif

#define CC_ATOMIC_GET8(p)  (*p)
#define CC_ATOMIC_GET16(p) (*p)
#define CC_ATOMIC_GET32(p) (*p)
#define CC_ATOMIC_GET64(p)                                                     \
   ({                                                                          \
      uint64_t v;                                                              \
      RT_DEFINE_SPINLOCK(_spinlock);                                           \
      rt_base_t level;                                                         \
      level = rt_spin_lock_irqsave(&_spinlock);                                \
      v = *p;                                                                  \
      rt_spin_unlock_irqrestore(&_spinlock, level);                            \
      v;                                                                       \
   })

#define CC_ATOMIC_SET8(p, v)  ((*p) = (v))
#define CC_ATOMIC_SET16(p, v) ((*p) = (v))
#define CC_ATOMIC_SET32(p, v) ((*p) = (v))
#define CC_ATOMIC_SET64(p, v)                                                  \
   ({                                                                          \
      RT_DEFINE_SPINLOCK(_spinlock);                                           \
      rt_base_t level;                                                         \
      level = rt_spin_lock_irqsave(&_spinlock);                                \
      *p = v;                                                                  \
      rt_spin_unlock_irqrestore(&_spinlock, level);                            \
   })

#define CC_ASSERT(exp)        RT_ASSERT(exp)
// #define CC_STATIC_ASSERT(exp) _Static_assert(exp, "")
#define CC_STATIC_ASSERT(exp) typedef char pn_static_assertion_##__LINE__[(exp) ? 1 : -1]
#define CC_UNUSED(var) (void)(var)

#ifdef __cplusplus
}
#endif

#endif /* CC_H */
