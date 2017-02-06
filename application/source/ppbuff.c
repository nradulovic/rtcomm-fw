/*
 * ppbuff.c
 *
 *  Created on: Jan 27, 2015
 *      Author: nenad
 */

#include <stdbool.h>
#include <string.h>

#include "ppbuff.h"

/* -------------------------------------------------------------------------- *
 * RING
 * -------------------------------------------------------------------------- */

static void ring_init(struct ring * ring, uint32_t size, struct acq_sample * storage)
{
	ring->curr = 0;
	ring->free = size;
	memset(&ring->sample[0], 0, size * sizeof(struct acq_sample));
}

/****************** status ringa *****************/
static bool ring_is_full(const struct ring * ring)
{
	if (ring->free != 0) {
		return (false);
	} else {
		return (true);
	}
}

/**********************************************************************
  * @name    ring_base(const struct ring * ring)
  * @brief   vraca pokazivac na prvi sempl u ringu (adresu prvog sempla)
  * @param   const struct ring * ring: ring  
  * @retval  pokazivac na prvi sempl u ringu
  *********************************************************************/
static void * ring_base(const struct ring * ring)
{
	return ((void *)&ring->sample[0]);
}


static void ring_reinit(struct ring * ring, uint32_t size)
{
	ring->curr = 0;
	ring->free = size;
}

/****************** vraca adresu trenutnog sampla u ringu *****************/
static struct acq_sample * ring_current_sample(struct ring * const ring)
{
	return (&ring->sample[ring->curr]);
}

static void ring_push(struct ring * ring, uint32_t elements)
{
	ring->curr += elements;
	ring->free -= elements;
}

/****** povecava adresu pokazivaca trenutnog sempla i 
smanjuje broj slobodnih semplova *******************/
static void ring_push_sample(struct ring * ring)
{
	ring->curr++;
	ring->free--;
}

/* -------------------------------------------------------------------------- *
 * PPBUFF
 * -------------------------------------------------------------------------- */

void ppbuff_init(struct ppbuff * buff, uint32_t size,
		void (* fn_full)(struct ppbuff *), struct acq_sample * a_storage,
		struct acq_sample * b_storage)
{
	ring_init(&buff->ring[0], size, a_storage);
	ring_init(&buff->ring[1], size, b_storage);
	buff->consumer = &buff->ring[0];
	buff->producer = &buff->ring[1];
	buff->fn_full  = fn_full;
}



struct acq_sample * ppbuff_producer_current_sample(const struct ppbuff * buff)
{
	return (ring_current_sample(buff->producer));
}

/*
 * Put item to producer queue
 */
void ppbuff_producer_push(struct ppbuff * buff, uint32_t elements)
{
	ring_push(buff->producer, elements); 											 //racuna adresu trenutnog i kolicinu slobone memorije u ringu

	if (ring_is_full(buff->producer)) {
		ring_reinit(buff->producer, buff->size);

		if (!buff->is_consumer_locked) { // ako buff->consumer nije zakljucan zameni bafere
			struct ring *   	tmp;

			ring_reinit(buff->consumer, buff->size);
			tmp            = buff->consumer;
			buff->consumer = buff->producer;
			buff->producer = tmp;
		}
		buff->fn_full(buff);
	}
}



void * ppbuff_consumer_base(struct ppbuff * buff)
{
	return (ring_base(buff->consumer));
}



void ppbuff_copy(struct ppbuff * destination, const struct ppbuff * source)
{
	memcpy(ppbuff_producer_current_sample(destination), ppbuff_consumer_base(source),
			source->size * sizeof(struct acq_sample));
	ppbuff_producer_push(destination, source->size);
}
