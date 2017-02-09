/*
 * ppbuff.h
 *
 *  Created on: Jan 27, 2015
 *      Author: nenad
 */

#ifndef APPLICATION_INCLUDE_PPBUFF_H_
#define APPLICATION_INCLUDE_PPBUFF_H_

#include <stdint.h>
#include <stdbool.h>

#include "cdi/io.h"

struct ring
{
	uint32_t					curr;
	uint32_t					free;
	struct acq_sample * 		sample;
};

struct ppbuff
{
	struct ring * 				consumer;
	struct ring * 				producer;
	void					 (* fn_full)(struct ppbuff *);
	void					 (* fn_half)(struct ppbuff *);
	uint32_t					size;
	bool						is_consumer_locked;
	struct ring					ring[2];
};

void ppbuff_init(struct ppbuff * buff, uint32_t size,
		void (* fn_full)(struct ppbuff *), void (* fn_half)(struct ppbuff *),
		struct acq_sample * a_storage, struct acq_sample * b_storage);


static inline
uint32_t ppbuff_size(const struct ppbuff * buff)
{
	return (buff->size);
}


struct acq_sample * ppbuff_producer_current_sample(const struct ppbuff * buff);
void ppbuff_producer_push(struct ppbuff * buff);
void * ppbuff_consumer_base(struct ppbuff * buff);

static inline
bool ppbuff_lock_consumer(struct ppbuff * buff)
{
	if (buff->is_consumer_locked) {
		return (false);
	} else {
		buff->is_consumer_locked = true;

		return (true);
	}
}

static inline
void ppbuff_unlock_consumer(struct ppbuff * buff)
{
	buff->is_consumer_locked = false;
}

#endif /* APPLICATION_INCLUDE_PPBUFF_H_ */
