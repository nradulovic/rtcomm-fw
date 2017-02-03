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

static void ring_init(struct ring * ring, uint32_t size)
{
	if (size > MAX_RING_BUFF_SIZE) {
		size = MAX_RING_BUFF_SIZE;
	}
	ring->curr 		= 0;
	ring->free 		= size;
	ring->size 		= size;
	ring->is_locked = false;
	memset(&ring->sample[0], 0, MAX_RING_BUFF_SIZE * sizeof(struct acq_sample));
}

/**************** vraca dimenziju ringa ***********/
static uint32_t ring_size(const struct ring * ring)
{
	return (ring->size);
}

/** vraca kolicinu preostale memorije za smestanje semplova **/
static uint32_t ring_free(const struct ring * ring)
{
	return (ring->free);
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


static void ring_restart(struct ring * ring)
{
	ring->curr = 0;
	ring->free = ring->size;
}

/****************** vraca adresu trenutnog sampla u ringu *****************/
static struct acq_sample * ring_current_sample(struct ring * const ring)
{
	return (&ring->sample[ring->curr]);
}

/****** povecava adresu pokazivaca trenutnog sempla i 
smanjuje broj slobodnih semplova *******************/
static void ring_push_sample(struct ring * ring)
{
	ring->curr++;
	ring->free--;
}

static bool ring_is_locked(const struct ring * ring)
{
	return (ring->is_locked);
}

static void ring_lock(struct ring * ring)
{
	ring->is_locked = true;
}

static void ring_unlock(struct ring * ring)
{
	ring->is_locked = false;
}


/* -------------------------------------------------------------------------- *
 * PPBUFF
 * -------------------------------------------------------------------------- */

void ppbuff_init(struct ppbuff * buff, uint32_t size, uint32_t enabled_mask)
{
	ring_init(&buff->ring[0], size);
	ring_init(&buff->ring[1], size);
	buff->consumer = &buff->ring[0];
	buff->producer = &buff->ring[1];
	buff->enabled_mask = enabled_mask;
	buff->sampled_mask = 0;
}



uint32_t ppbuff_size(const struct ppbuff * buff)
{
	return (ring_size(&buff->ring[0]));
}



uint32_t ppbuff_free(const struct ppbuff * buff)
{
	return (ring_free(buff->producer));
}


/***** menja mesta consumer i  producer baferima *****/
void ppbuff_swap(struct ppbuff * buff)
{
	struct ring *   			tmp;

	tmp            = buff->consumer;
	buff->consumer = buff->producer;
	buff->producer = tmp;

	ring_restart(buff->producer);
}


/***** vraca adresu trenutnog sampla *****/
struct acq_sample * ppbuff_current_sample(const struct ppbuff * buff)
{
	return (ring_current_sample(buff->producer));
}



/**
 * Put item to producer queue
 *
 * If there is a completion_level set it will notify when _producer_ queue size
 * is greater or equal to it.
 */
void ppbuff_push_sample(struct ppbuff * buff, uint32_t channel_mask)
{
	buff->sampled_mask |= (buff->enabled_mask & channel_mask);

	if (buff->sampled_mask == buff->enabled_mask) {
		buff->sampled_mask = 0;
        ppbuff_one_sample_callback(ppbuff_current_sample(buff)); //menja opseg i ukljucuje matematiku u zavisnosti od statusa flegova
		ring_push_sample(buff->producer); 											 //racuna adresu trenutnog i kolicinu slobone memorije u ringu
		if (ring_free(buff->producer) == (ring_size(buff->producer) / 2)) {
		    ppbuff_half_full_callback();
		} else if (ring_is_full(buff->producer)) {
		    bool consumer_is_locked;

		    ring_restart(buff->producer);
		    consumer_is_locked = ring_is_locked(buff->consumer);

		    if (!consumer_is_locked) { // ako buff->consumer nije zakljucan zameni bafere
				ppbuff_swap(buff);
			}
			ppbuff_full_callback(consumer_is_locked);
		}
	}
}



void * ppbuff_consumer_base(struct ppbuff * buff)
{
	return (ring_base(buff->consumer));
}


/********* zakljucava se consumer bafer **********/
void ppbuff_lock_consumer(struct ppbuff * buff)
{
	ring_lock(buff->consumer);
}



void ppbuff_unlock_consumer(struct ppbuff * buff)
{
	ring_unlock(buff->consumer);
}
