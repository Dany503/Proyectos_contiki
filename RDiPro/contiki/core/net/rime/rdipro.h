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
 *         Header file for the best-effort network flooding (rdipro)
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/**
 * \addtogroup rime
 * @{
 */

/**
 * \defgroup rimerdipro Best-effort network flooding
 * @{
 *
 * The rdipro module does best-effort flooding.
 *
 * The rdipro primitive sends a single packet to all nodes in the
 * network. The rdipro primitive uses polite broadcasts at every hop
 * to reduce the number of redundant transmissions.  The rdipro
 * primitive does not perform retransmissions of flooded packets and
 * packets are not tagged with version numbers.  Instead, the rdipro
 * primitive sets the end-to-end sender and end-to-end packet ID
 * attributes on the packets it sends.  A forwarding node saves the
 * end-to-end sender and packet ID of the last packet it forwards and
 * does not forward a packet if it has the same end-to-end sender and
 * packet ID as the last packet.  This reduces the risk of routing
 * loops, but does not eliminate them entirely as the rdipro
 * primitive saves the attributes of the latest packet seen only.
 * Therefore, the rdipro primitive also uses the time to live
 * attribute, which is decreased by one before forwarding a packet.
 * If the time to live reaches zero, the primitive does not forward
 * the packet.
 *
 * \section rdipro-channels Channels
 *
 * The rdipro module uses 1 channel.
 *
 */

#ifndef rdipro_H_
#define rdipro_H_

#include "net/queuebuf.h"
#include "net/rime/ipolite.h"

struct rdipro_conn;

#define rdipro_ATTRIBUTES   { PACKETBUF_ADDR_ESENDER, PACKETBUF_ADDRSIZE }, \
                              { PACKETBUF_ATTR_HOPS, PACKETBUF_ATTR_BIT * 5 }, \
                              { PACKETBUF_ATTR_EPACKET_ID, PACKETBUF_ATTR_BIT * 4 }, \
                                IPOLITE_ATTRIBUTES

struct rdipro_callbacks {
  int (* recv)(struct rdipro_conn *c, const linkaddr_t *from,
	       const linkaddr_t *originator, uint8_t seqno, uint8_t hops);
  void (* sent)(struct rdipro_conn *c);
  void (* dropped)(struct rdipro_conn *c);
};

struct rdipro_conn {
  struct ipolite_conn c;
  const struct rdipro_callbacks *u;
  clock_time_t queue_time;
  linkaddr_t last_originator;
  uint8_t last_originator_seqno;
  uint16_t longest_distance_neighbor;
};

void rdipro_open(struct rdipro_conn *c, clock_time_t queue_time,
	     uint16_t channel, const struct rdipro_callbacks *u);
void rdipro_close(struct rdipro_conn *c);

int rdipro_send(struct rdipro_conn *c, uint8_t seqno, uint16_t longest_distance_neighbor);

#endif /* SIBC_H_ */
/** @} */
/** @} */
