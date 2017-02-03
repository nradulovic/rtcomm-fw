/*
 * prim_lock.h
 *
 *  Created on: Jan 19, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_PRIM_LOCK_H_
#define APPLICATION_INCLUDE_PRIM_LOCK_H_

#include <stdbool.h>

#include "neon_eds.h"

struct prim_lock
{
	bool						is_locked;
};

/* -------------------------------------------------------------------------- *
 * Locking primitive
 * -------------------------------------------------------------------------- */

static inline
bool lock_test_and_set(bool * is_locked)
{
	ncore_lock					sys_lock;
	bool						retval;

	ncore_lock_enter(&sys_lock);
	retval     = *is_locked;
	*is_locked = true;
	ncore_lock_exit(&sys_lock);

	return (retval);
}

static inline
void lock_init(struct prim_lock * lock)
{
	lock->is_locked = false;
}

static inline
void lock(struct prim_lock * lock)
{
	while (lock_test_and_set(&lock->is_locked));
}

static inline
bool try_lock_i(struct prim_lock * lock)
{
	if (lock->is_locked) {
		return (false);
	} else {
		lock->is_locked = true;

		return (true);
	}
}

static inline
void unlock(struct prim_lock * lock)
{
	lock->is_locked = false;
}

static inline
bool is_locked_i(const struct prim_lock * lock)
{
	return (lock->is_locked);
}

#endif /* APPLICATION_INCLUDE_PRIM_LOCK_H_ */
