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

#define MAX_RING_BUFF_SIZE				2048

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
	uint32_t					size;
	bool						is_consumer_locked;
	struct ring					ring[2];
};

void ppbuff_init(struct ppbuff * buff, uint32_t size,
		void (* fn_full)(struct ppbuff *), struct acq_sample * a_storage,
		struct acq_sample * b_storage);


static inline
uint32_t ppbuff_size(const struct ppbuff * buff)
{
	return (buff->size);
}


struct acq_sample * ppbuff_producer_current_sample(const struct ppbuff * buff);
void ppbuff_producer_push(struct ppbuff * buff, uint32_t elements);
void * ppbuff_consumer_base(struct ppbuff * buff);

static inline
void ppbuff_lock_consumer(struct ppbuff * buff)
{
	buff->is_consumer_locked = true;
}

static inline
void ppbuff_unlock_consumer(struct ppbuff * buff)
{
	buff->is_consumer_locked = false;
}

static inline
bool ppbuff_is_consumer_locked(const struct ppbuff * buff)
{
	return (buff->is_consumer_locked);
}

void ppbuff_copy(struct ppbuff * destination, const struct ppbuff * source);

#endif /* APPLICATION_INCLUDE_PPBUFF_H_ */
