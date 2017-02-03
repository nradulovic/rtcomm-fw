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
	uint32_t					size;
	bool						is_locked;
	struct acq_sample   		sample[MAX_RING_BUFF_SIZE];
};

struct ppbuff
{
	struct ring					ring[2];
	struct ring * 				consumer;
	struct ring * 				producer;
	uint32_t					sampled_mask;
	uint32_t 					enabled_mask;
};

/**@brief		Initialize Ping-Pong buffer
 * @param 		buff
 * @param 		size
 * 				Number of samples. Range: [0 to MAX_RING_BUFF_SIZE]
 *  @arg		MAX_RING_BUFF_SIZE - use full size
 * @param 		enabled_mask
 *  @arg        ACQ_CHANNEL_X_MASK or 0x01 - X channel mask
 *  @arg		ACQ_CHANNEL_Y_MASK or 0x02 - Y channel mask
 *  @arg		ACQ_CHANNEL_Z_MASK or 0x04 - Z channel mask
 */
void ppbuff_init(struct ppbuff * buff, uint32_t size, uint32_t enabled_mask);
uint32_t ppbuff_size(const struct ppbuff * buff);
uint32_t ppbuff_free(const struct ppbuff * buff);
void ppbuff_swap(struct ppbuff * buff);
struct acq_sample * ppbuff_current_sample(const struct ppbuff * buff);
void ppbuff_push_sample(struct ppbuff * buff, uint32_t channel_mask);
void * ppbuff_consumer_base(struct ppbuff * buff);
void ppbuff_lock_consumer(struct ppbuff * buff);
void ppbuff_unlock_consumer(struct ppbuff * buff);

extern void ppbuff_one_sample_callback(struct acq_sample * sample);
extern void ppbuff_full_callback(bool consumer_is_locked);
extern void ppbuff_half_full_callback(void);

#endif /* APPLICATION_INCLUDE_PPBUFF_H_ */
