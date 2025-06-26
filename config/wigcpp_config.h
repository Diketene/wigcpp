#ifndef __WIGCPP_CONFIG_H__
#define __WIGCPP_CONFIG_H__

#include "auto_config.h"

/* If not generated from auto_config.h, define as 0 */
#ifndef WIGCPP_IMPL_LONG_DOUBLE
#define WIGCPP_IMPL_LONG_DOUBLE 0
#endif

/* If not generated from auto_config.h, define as 0 */
#ifndef WIGCPP_IMPL_FLOAT128
#define WIGCPP_IMPL_FLOAT128 0
#endif

/* Control semi-factorials */
#ifndef WIGCPP_IMPL_DOUBLE_FACTORIAL
#define WIGCPP_IMPL_DOUBLE_FACTORIAL 0
#endif


/* Size in bytes of each prime exponent.  2 or 4. */

#define PRIME_LIST_SIZEOF_ITEM 4

/* Size in bytes of the vector instances.  SSE: 16, AVX:32 */

/* Using vector_size(32), i.e. AVX instructions on x86-64 has been
 * tested, and gives ~0 difference.  We are memory/cache bandwidth
 * limited. --from wigxjpf
 */

#define PRIME_LIST_VECT_SIZE 16

/* Size in bytes of each word in multi-word integers.  4 or 8. */

/* We can efficiently only use words that have half the size of the
 * maximum multiplication product the machine can easily produce.
 *
 * For 64-bit x86_64 CPUs, there is 128-bit (16 byte) result multiply
 * instructions.  However, code generation of compilers does not
 * handle the carry handling nicely, such that performance currently
 * equals that of using 32-bit words. --from wigxjpf
 */

/*if not generated from auto_config.h, define as 4*/
#ifndef MULTI_WORD_INT_SIZEOF_ITEM  
#define MULTI_WORD_INT_SIZEOF_ITEM  4
#endif

/* Size in bytes of word used to multiply from first prime exponents.
 * Only used when item is smaller.
 */

#ifndef MULTI_WORD_INT_SIZEOF_MULW
#define MULTI_WORD_INT_SIZEOF_MULW  8
#endif

/* Array size for precalculated factorial used in fpsimple routines. */

#define FPSIMPLE_MAX_FACTORIAL 2500

/* Debug printing. */

#define DEBUG_PRINT 1

/* Accounting to find maximum factorial and iteration count. */

#define ACCOUNT_MAX_FACT_ITER 1

/* Special rules for MSVC. */

#ifdef _MSC_VER

# define __thread __declspec(thread)

# include <BaseTsd.h>
typedef SSIZE_T ssize_t;

#define WIGXJPF_NOINLINE  __declspec(noinline)

#endif

/* Avoid inlining of function. */

#ifndef WIGCPP_NOINLINE
#define WIGCPP_NOINLINE  __attribute__((noinline))
#endif

#endif/*__WIGCPP_CONFIG_H__*/

