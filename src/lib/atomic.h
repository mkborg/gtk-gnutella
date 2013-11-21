/*
 * Copyright (c) 2011, 2013 Raphael Manfredi
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

/**
 * @ingroup lib
 * @file
 *
 * Atomic memory operations.
 *
 * These a low-level operations that are required to implement spinlocks
 * and mutexes.
 *
 * @author Raphael Manfredi
 * @date 2011, 2013
 */

#ifndef _atomic_h_
#define _atomic_h_

#include "common.h"

/*
 * To avoid mistakes, the "volatile" attribute is made part of the typedef:
 * no access to the lock memory location should be optimized by the compiler,
 * or we might loop forever.
 */

typedef volatile uint8 atomic_lock_t;

/*
 * Public interface.
 */

#ifdef HAS_SYNC_ATOMIC
#define atomic_mb()					__sync_synchronize()
#define atomic_ops_available()		1

static inline ALWAYS_INLINE void
atomic_release(atomic_lock_t *p) {
	/* Prefer this to __sync_lock_release(p) which is obscurely documented */
	*p = 0;
	atomic_mb();
}

static inline ALWAYS_INLINE bool
atomic_test_and_set(atomic_lock_t *p)
{
	return __sync_bool_compare_and_swap(p, 0, 1);
}

static inline ALWAYS_INLINE int
atomic_int_inc(int *p)
{
	return __sync_fetch_and_add(p, 1);		/* Previous value */
}

static inline ALWAYS_INLINE int
atomic_int_dec(int *p)
{
	return __sync_fetch_and_sub(p, 1);		/* Previous value */
}

static inline ALWAYS_INLINE uint
atomic_uint_inc(unsigned *p)
{
	return __sync_fetch_and_add(p, 1);		/* Previous value */
}

static inline ALWAYS_INLINE uint
atomic_uint_dec(unsigned *p)
{
	return __sync_fetch_and_sub(p, 1);		/* Previous value */
}

static inline ALWAYS_INLINE bool
atomic_int_dec_is_zero(int *p)
{
	return 1 == __sync_fetch_and_sub(p, 1);
}

static inline ALWAYS_INLINE bool
atomic_uint_dec_is_zero(unsigned *p)
{
	return 1 == __sync_fetch_and_sub(p, 1);
}

/*
 * These can be used on "opaque" types like sig_atomic_t
 * Otherwise, use the type-safe inline routines whenever possible.
 *
 * The ATOMIC_INC() and ATOMIC_DEC() macros return the previous value.
 */

#define ATOMIC_INC(p)		__sync_fetch_and_add(p, 1)
#define ATOMIC_DEC(p)		__sync_fetch_and_sub(p, 1)

#else	/* !HAS_SYNC_ATOMIC */

#define atomic_mb()					(void) 0

static inline bool
atomic_test_and_set(atomic_lock_t *p)
{
	int ok;
	if ((ok = (0 == *(p))))	
		*(p) = 1;
	return ok;
}

#define ATOMIC_INC(p)				((*(p))++)
#define ATOMIC_DEC(p)				((*(p))--)

#define atomic_int_inc(p)			((*(p))++)		/* Previous value */
#define atomic_int_dec(p)			((*(p))--)		/* Previous value */
#define atomic_uint_inc(p)			((*(p))++)		/* Previous value */
#define atomic_uint_dec(p)			((*(p))--)		/* Previous value */
#define atomic_int_dec_is_zero(p)	(0 == --(*(p)))
#define atomic_uint_dec_is_zero(p)	(0 == --(*(p)))
#define atomic_release(p)			(*(p) = 0)
#define atomic_ops_available()		0
#endif	/* HAS_SYNC_ATOMIC */

/**
 * Attempt to acquire the lock.
 *
 * @return TRUE if lock was acquired.
 */
static inline bool
atomic_acquire(atomic_lock_t *lock)
{
	/*
	 * Our locking protocol issues a memory barrier after a lock has been
	 * released, to make sure the changes to the locking object are widely
	 * visible to all processors.
	 *
	 * Therefore, it is not necessary to issue a memory barrier here.
	 */

	return atomic_test_and_set(lock);
}

static inline ALWAYS_INLINE bool
atomic_bool_get(bool *p)
{
	atomic_mb();
	return *p;
}

static inline ALWAYS_INLINE void
atomic_bool_set(bool *p, bool v)
{
	*p = v;
	atomic_mb();
}

static inline ALWAYS_INLINE int
atomic_int_get(int *p)
{
	atomic_mb();
	return *p;
}

static inline ALWAYS_INLINE void
atomic_int_set(int *p, int v)
{
	*p = v;
	atomic_mb();
}

static inline ALWAYS_INLINE uint
atomic_uint_get(uint *p)
{
	atomic_mb();
	return *p;
}

static inline ALWAYS_INLINE void
atomic_uint_set(uint *p, uint v)
{
	*p = v;
	atomic_mb();
}

/***
 *** Atomic 64-bit counters.
 ***
 *** These are convenience macros allowing us to safely handle 64-bit atomic
 *** operations on 32-bit machines.
 ***
 *** AU64() is a macro to define atomic 64-bit fields.
 *** AU64_INC() and AU64_DEC() can atomically increment or decrement counters.
 *** AU64_VALUE() atomically reads the 64-bit counter.
 ***/

#if 4 == LONGSIZE
/*
 * On a 32-bit machine, the 64-bit count is split between a "lo" and a "hi"
 * 32-bit counter, being updated atomically using 32-bit operations on each
 * counter.
 */

#define AU64(x) 	uint x ## _lo; uint x ## _hi

#define AU64_INC(x) G_STMT_START { \
	if G_UNLIKELY(-1U == atomic_uint_inc(x ## _lo)) \
		atomic_uint_inc(x ## _hi); \
} G_STMT_END

#define AU64_DEC(x) G_STMT_START { \
	if G_UNLIKELY(0 == atomic_uint_dec(x ## _lo)) \
		atomic_uint_dec(x ## _hi); \
} G_STMT_END

/**
 * Assemble 64-bit value from the high and low 32-bit parts of the counter.
 *
 * @param hi		pointer to the high 32-bit part
 * @param lo		pointer to the low 32-bit part
 *
 * @return 64-bit value reconstructed from two 32-bit counters which are
 * atomically updated.
 */
static inline uint64
au64_value(const uint volatile *hi, const uint volatile *lo)
{
	uint64 v;
	register uint low, low2, high, high2;

	atomic_mb();
	low = *lo; high = *hi;

retry:
	v = (((uint64) high) << 32) + low;

	/*
	 * If `low' is within a zone at risk, where fast increments or decrements
	 * could modify the high counter, then re-read and update value.
	 */

	if G_LIKELY(low > 0x1000 && low < 0xfffff000)
		return v;

	atomic_mb();
	low2 = *lo; high2 = *hi;

	if G_LIKELY(low2 == low && high2 == high)
		return v;

	low = low2; high = high2;
	goto retry;
}

#define AU64_VALUE(x) au64_value(x ## _hi, x ## _lo)

#elif 8 == LONGSIZE
/*
 * On 64-bit machines, we can expect atomic operations on 64-bit quantities.
 *
 * We append the _64 token to make sure these fields are only handled through
 * the following macros and not as plain uint64 variables because that would
 * not work on 32-bit platforms and we want to catch the potential error even
 * when compiling on 64-bit machines.
 */

#define AU64(x) 		uint64 x ## _64
#define AU64_INC(x) 	ATOMIC_INC(x ## _64)
#define AU64_DEC(x) 	ATOMIC_DEC(x ## _64)
#define AU64_VALUE(x) 	(atomic_mb(), *(x ## _64))

#else
#error "unexpected value for LONGSIZE"
#endif

#endif /* _atomic_h_ */

/* vi: set ts=4 sw=4 cindent: */
