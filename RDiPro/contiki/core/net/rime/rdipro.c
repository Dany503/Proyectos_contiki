/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Best-effort network flooding (rdipro)
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/**
 * \addtogroup rimerdipro
 * @{
 */

#include "net/rime/rdipro.h"
#include "lib/random.h"

#include <string.h>

#define HOPS_MAX 16

struct rdipro_hdr {
  uint16_t originator_seqno;
  linkaddr_t originator;
  uint16_t hops;
  uint16_t longest_distance_neighbor;
};

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifndef PROB_SUPERIOR
#define PROB_SUPERIOR 0.8
#endif

#ifndef PROB_INFERIOR
#define PROB_INFERIOR 0.2
#endif

#ifndef NORM_BASE
#define NORM_BASE 80
#endif

/*---------------------------------------------------------------------------*/
static int
send(struct rdipro_conn *c)
{
  PRINTF("%d.%d: rdipro send to ipolite\n",
	 linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
  return ipolite_send(&c->c, c->queue_time, 4);
}
/*---------------------------------------------------------------------------*/
static void
recv_from_ipolite(struct ipolite_conn *ipolite, const linkaddr_t *from)
{
  struct rdipro_conn *c = (struct rdipro_conn *)ipolite;
  struct rdipro_hdr hdr;
  uint8_t hops;
  struct queuebuf *queuebuf;
  uint16_t norm_distance;
  uint16_t random_num;
  uint16_t prob;
  uint16_t distance_sender = packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER)-packetbuf_attr(PACKETBUF_ATTR_RSSI);

  memset(&norm_distance, 0, sizeof(uint16_t));
  memset(&random_num, 0, sizeof(uint16_t));
  memset(&prob, 0, sizeof(uint16_t));

  memcpy(&hdr, packetbuf_dataptr(), sizeof(struct rdipro_hdr));
  hops = hdr.hops;

  /* Remember packet if we need to forward it. */
  queuebuf = queuebuf_new_from_packetbuf();

  packetbuf_hdrreduce(sizeof(struct rdipro_hdr));
  if(c->u->recv != NULL) {
    if(!(linkaddr_cmp(&hdr.originator, &c->last_originator) &&
	 hdr.originator_seqno <= c->last_originator_seqno)) {

      if(c->u->recv(c, from, &hdr.originator, hdr.originator_seqno,
		    hops)) {
	
	if(queuebuf != NULL) {
	  queuebuf_to_packetbuf(queuebuf);
	  queuebuf_free(queuebuf);
	  queuebuf = NULL;
	  memcpy(&hdr, packetbuf_dataptr(), sizeof(struct rdipro_hdr));
	  
	  /* Rebroadcast received packet. */
	  if(hops < HOPS_MAX) {
      //normalizamos la distancia con respecto al de mayor distancia
      norm_distance = ((float)distance_sender/(float)hdr.longest_distance_neighbor)*100;
      //calculamos un numero aleatorio que usaremos para ver cuan probable es que transmitan
      random_init(random_rand()+distance_sender);
      random_num = random_rand() % 100;

      //ponderamos dandole mayor probabilidad de transmitir al que está más lejos
      if (norm_distance > NORM_BASE) {
        prob = (float)norm_distance*(float)PROB_SUPERIOR;
      }
      else {
        prob = (float)norm_distance*(float)PROB_INFERIOR;
      }

      //transmitirá si el random_num es menor que la prob
      if (random_num < prob) {
	      PRINTF("%d.%d: rdipro rebroadcasting %d.%d/%d (%d.%d/%d) hops %d (won with %u vs %u) (my furthest neighbor is at %u)\n",
		     linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
		     hdr.originator.u8[0], hdr.originator.u8[1],
		     hdr.originator_seqno,
		     c->last_originator.u8[0], c->last_originator.u8[1],
		     c->last_originator_seqno,
		     hops,
         random_num,
         prob,
         c->longest_distance_neighbor);
	      hdr.hops++;
        hdr.longest_distance_neighbor = c->longest_distance_neighbor;
	      memcpy(packetbuf_dataptr(), &hdr, sizeof(struct rdipro_hdr));
	      send(c);
	      linkaddr_copy(&c->last_originator, &hdr.originator);
	      c->last_originator_seqno = hdr.originator_seqno;
      }
      else {
	      PRINTF("lost with %u vs %u\n",
         random_num,
         prob);
      }
	  }
	}
      }
    }
  }
  if(queuebuf != NULL) {
    queuebuf_free(queuebuf);
  }
}
/*---------------------------------------------------------------------------*/
static void
sent(struct ipolite_conn *ipolite)
{
  struct rdipro_conn *c = (struct rdipro_conn *)ipolite;
  if(c->u->sent != NULL) {
    c->u->sent(c);
  }
}
/*---------------------------------------------------------------------------*/
static void
dropped(struct ipolite_conn *ipolite)
{
  struct rdipro_conn *c = (struct rdipro_conn *)ipolite;
  if(c->u->dropped != NULL) {
    c->u->dropped(c);
  }
}
/*---------------------------------------------------------------------------*/
static const struct ipolite_callbacks rdipro = {recv_from_ipolite, sent, dropped};
/*---------------------------------------------------------------------------*/
void
rdipro_open(struct rdipro_conn *c, clock_time_t queue_time,
	uint16_t channel, const struct rdipro_callbacks *u)
{
  c->longest_distance_neighbor = 0;
  ipolite_open(&c->c, channel, 1, &rdipro);
  c->u = u;
  c->queue_time = queue_time;
}
/*---------------------------------------------------------------------------*/
void
rdipro_close(struct rdipro_conn *c)
{
  ipolite_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int
rdipro_send(struct rdipro_conn *c, uint8_t seqno, uint16_t longest_distance_neighbor)
{
  if(packetbuf_hdralloc(sizeof(struct rdipro_hdr))) {
    struct rdipro_hdr *hdr = packetbuf_hdrptr();
    linkaddr_copy(&hdr->originator, &linkaddr_node_addr);
    linkaddr_copy(&c->last_originator, &hdr->originator);
    c->last_originator_seqno = hdr->originator_seqno = seqno;
    hdr->longest_distance_neighbor = longest_distance_neighbor;
    hdr->hops = 0;
    PRINTF("%d.%d: rdipro sending '%s', with seq no. %u and longest-distance neighbor %u\n",
	   linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1],
	   (char *)packetbuf_dataptr(),
	   seqno,
     longest_distance_neighbor);
    return ipolite_send(&c->c, 0, 4);
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
void
rdipro_cancel(struct rdipro_conn *c)
{
  ipolite_cancel(&c->c);
}
/*---------------------------------------------------------------------------*/
/** @} */
