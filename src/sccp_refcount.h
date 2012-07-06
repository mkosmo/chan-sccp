
/*!
 * \file 	sccp_refcount.h
 * \brief 	SCCP RefCount Header
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 *
 * $Date$
 * $Revision$
 */

#ifndef __SCCP_REFCOUNT_H
#    define __SCCP_REFCOUNT_H

#include <setjmp.h>

typedef struct refcount_object RefCountedObject;

void sccp_refcount_init(void);
void sccp_refcount_destroy(void);
int sccp_refcount_isRunning(void);
int sccp_refcount_schedule_cleanup(const void *data);
void *sccp_refcount_object_alloc(size_t size, const char *datatype, const char *identifier, void *destructor);
void sccp_refcount_updateIdentifier(void *ptr, char *identifier);
inline void *sccp_refcount_retain(void *ptr, const char *filename, int lineno, const char *func);
inline void *sccp_refcount_release(const void *ptr, const char *filename, int lineno, const char *func);

/* *INDENT-OFF* */
#define __WITH_REF(_a,_b,_c,_d) 										\
  int with_counter_##_a##_c=3;										\
  while (with_counter_##_a##_c-- > 0)									\
          if (2 == with_counter_##_a##_c) {		/* ENTRY */					\
                  if (!(_a = sccp_refcount_retain(_a,_b,_c,_d))) {					\
                          pbx_log(LOG_NOTICE, "[%s:%d] %s: Failed to retain (%p)\n",  _b,_c,_d,_a);	\
                          with_counter_##_a##_c=0;							\
                          break;									\
                  }											\
          } else											\
          if (1 == with_counter_##_a##_c) {		/* EXIT */					\
                  if ((_a = sccp_refcount_release(_a,_b,_c,_d)) != NULL) {				\
                          pbx_log(LOG_NOTICE, "[%s:%d] %s: Failed to release (%p)\n", _b,_c,_d,_a);	\
                  }											\
                  break;										\
          } else          				/* DO */

#define with_ref(_a) __WITH_REF(_a,__FILE__,__LINE__,__PRETTY_FUNCTION__)
/* *INDENT-ON* */
#endif										// __SCCP_REFCOUNT_H
