/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 */

/*
 * This file provides extensions for functions not defined in <string.h>
 */

#ifndef __STRING_EXT_H
#define __STRING_EXT_H

#include <stddef.h>
#include <sys/cdefs.h>

/*
 * Copy src to string dst of siz size.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);

/* A constant-time version of memcmp() */
int consttime_memcmp(const void *p1, const void *p2, size_t nb);

/* Deprecated. For backward compatibility. */
static inline int buf_compare_ct(const void *s1, const void *s2, size_t n)
{
	return consttime_memcmp(s1, s2, n);
}

/* Variant of strdup() that uses nex_malloc() instead of malloc() */
char *nex_strdup(const char *s);

/*
 * Like memset(s, 0, count) but prevents the compiler from optimizing the call
 * away. Such "dead store elimination" optimizations typically occur when
 * clearing a *local* variable that is not used after it is cleared; but
 * link-time optimization (LTO) can also trigger code elimination in other
 * circumstances. See "Dead Store Elimination (Still) Considered Harmful" [1]
 * for details and examples (and note that the Cland compiler enables LTO by
 * default!).
 *
 * [1] https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-yang.pdf
 *
 * Practically speaking:
 *
 * - Use memzero_explicit() to *clear* (as opposed to initialize) *sensitive*
 *   data (such as keys, passwords, cryptographic state);
 * - Otherwise, use memset().
 */
void memzero_explicit(void *s, size_t count);

/*
 * ins_array_elem() - insert an element in an array
 * @base:        start of the array
 * @elem_count:  the number of elements in the array
 * @elem_size:   the size of each element in the array
 * @pos:         element position counted in units of @elem_size
 * @elem:        pointer the the element to inser or NULL
 *
 * Makes room in the array at @pos by moving the element after this
 * position one position further back.
 *
 * If @elem is non-NULL it's copied into the array at the indicated
 * position.
 *
 * Returns a pointer to the inserted element.
 */
void *ins_array_elem(void *base, size_t elem_count, size_t elem_size,
		     size_t pos, const void *elem);

/*
 * ins_array_elem_zero_init() - insert a zero-initialized element in an array
 * @base:        start of the array
 * @elem_count:  the number of elements in the array
 * @elem_size:   the size of each element in the array
 * @pos:         element position counted in units of @elem_size
 *
 * Makes room in the array at @pos by moving the element after this
 * position one position further back. The free position is
 * zero-initialized.
 *
 * Returns a pointer to the free position.
 */
void *ins_array_elem_zero_init(void *base, size_t elem_count, size_t elem_size,
			       size_t pos);

/*
 * rem_array_elem() - remove an element from an array
 * @base:        start of the array
 * @elem_count:  the number of elements in the array
 * @elem_size:   the size of each element in the array
 * @pos:         element position counted in units of @elem_size
 *
 * Removes the element at @pos by advancing the element after this position
 * to fill the space.
 */
void rem_array_elem(void *base, size_t elem_count, size_t elem_size,
		    size_t pos);

/*
 * rem_array_elem_zero_pad() - remove an element from an array
 * @base:        start of the array
 * @elem_count:  the number of elements in the array
 * @elem_size:   the size of each element in the array
 * @pos:         element position counted in units of @elem_size
 *
 * Removes the element at @pos by advancing the element after this position
 * to fill the space. The now unused element at the end of the array is
 * zero-initialized.
 */
void rem_array_elem_zero_pad(void *base, size_t elem_count, size_t elem_size,
			     size_t pos);

#endif /* __STRING_EXT_H */
