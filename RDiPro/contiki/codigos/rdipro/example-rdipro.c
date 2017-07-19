
#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h"

#include "net/rime/rdipro.h"
#include "dev/button-sensor.h"

#include <stdio.h>


/* This is the structure of broadcast messages. */
struct broadcast_message {
  uint8_t seqno;
};

/* This structure holds information about neighbors. */
struct neighbor {
  /* The ->next pointer is needed since we are placing these on a
     Contiki list. */
  struct neighbor *next;

  /* The ->addr field holds the Rime address of the neighbor. */
  linkaddr_t addr;

  /* The ->last_rssi and ->last_lqi fields hold the Received Signal
     Strength Indicator (RSSI) and CC2420 Link Quality Indicator (LQI)
     values that are received for the incoming broadcast packets. */
  uint16_t last_rssi, last_lqi;

  /* Each broadcast packet contains a sequence number (seqno). The
     ->last_seqno field holds the last sequenuce number we saw from
     this neighbor. */
  uint8_t last_seqno;

  /* The ->avg_gap contains the average seqno gap that we have seen
     from this neighbor. */
  uint32_t avg_seqno_gap;

  uint16_t distance;

};

/* This #define defines the maximum amount of neighbors we can remember. */
#define MAX_NEIGHBORS 16

/* This MEMB() definition defines a memory pool from which we allocate
   neighbor entries. */
MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS);

/* The neighbors_list is a Contiki list that holds the neighbors we
   have seen thus far. */
LIST(neighbors_list);

/* These hold the broadcast and rdipro structures, respectively. */
static struct broadcast_conn broadcast;
static struct rdipro_conn rdiproc;

/* These two defines are used for computing the moving average for the
   broadcast sequence number gaps. */
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040

/*---------------------------------------------------------------------------*/
/* We first declare our two processes. */
PROCESS(broadcast_process, "Broadcast process");
PROCESS(rdipro_process, "RD-PRO process");

/* The AUTOSTART_PROCESSES() definition specifices what processes to
   start when this module is loaded. We put both our processes
   there. */
AUTOSTART_PROCESSES(&rdipro_process, &broadcast_process);
/*---------------------------------------------------------------------------*/
/* This function is called whenever a broadcast message is received. */
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  struct neighbor *n;
  struct broadcast_message *m;
  uint8_t seqno_gap;
  int i;
  struct neighbor *tabla_vecino;

  /* The packetbuf_dataptr() returns a pointer to the first data byte
     in the received packet. */
  m = packetbuf_dataptr();

  /* Check if we already know this neighbor. */
  for(n = list_head(neighbors_list); n != NULL; n = list_item_next(n)) {

    /* We break out of the loop if the address of the neighbor matches
       the address of the neighbor from which we received this
       broadcast message. */
    if(linkaddr_cmp(&n->addr, from)) {
      break;
    }
  }

  /* If n is NULL, this neighbor was not found in our list, and we
     allocate a new struct neighbor from the neighbors_memb memory
     pool. */
  if(n == NULL) {
    n = memb_alloc(&neighbors_memb);

    /* If we could not allocate a new neighbor entry, we give up. We
       could have reused an old neighbor entry, but we do not do this
       for now. */
    if(n == NULL) {
      return;
    }

    /* Initialize the fields. */
    linkaddr_copy(&n->addr, from);
    n->last_seqno = m->seqno - 1;
    n->avg_seqno_gap = SEQNO_EWMA_UNITY;

    /* Place the neighbor on the neighbor list. */
    list_add(neighbors_list, n);
  }

  /* We can now fill in the fields in our neighbor entry. */
  n->last_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  n->last_lqi = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
  n->distance = packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER)-packetbuf_attr(PACKETBUF_ATTR_RSSI);

  //si la distancia del nuevo vecino es mayor que la que había en la conexión de este nodo, la actualizamos
  if ((n->distance) > (rdiproc.longest_distance_neighbor)) {
    rdiproc.longest_distance_neighbor = n->distance;
  }

  /* Compute the average sequence number gap we have seen from this neighbor. */
  seqno_gap = m->seqno - n->last_seqno;
  n->avg_seqno_gap = (((uint32_t)seqno_gap * SEQNO_EWMA_UNITY) *
                      SEQNO_EWMA_ALPHA) / SEQNO_EWMA_UNITY +
                      ((uint32_t)n->avg_seqno_gap * (SEQNO_EWMA_UNITY -
                                                     SEQNO_EWMA_ALPHA)) /
    SEQNO_EWMA_UNITY;

  /* Remember last seqno we heard. */
  n->last_seqno = m->seqno;

  /* Print out a message. */
  printf("broadcast message received from %d.%d with seqno %d, RSSI %u, LQI %u, avg seqno gap %d.%02d, distance %u\n",
         from->u8[0], from->u8[1],
         m->seqno,
         packetbuf_attr(PACKETBUF_ATTR_RSSI),
         packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY),
         (int)(n->avg_seqno_gap / SEQNO_EWMA_UNITY),
         (int)(((100UL * n->avg_seqno_gap) / SEQNO_EWMA_UNITY) % 100),
         n->distance);
  /* vamos a impriminos la lista de vecinos que tenemos */
  tabla_vecino = list_head(neighbors_list);
  for (i = 0; i < list_length(neighbors_list); i++) {
    printf("[tabla de vecinos] %d.%d\n", tabla_vecino->addr.u8[0], tabla_vecino->addr.u8[1]);
    tabla_vecino = list_item_next(tabla_vecino);
  }
}
/* This is where we define what function to be called when a broadcast
   is received. We pass a pointer to this structure in the
   broadcast_open() call below. */
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
/*---------------------------------------------------------------------------*/
static int
recv(struct rdipro_conn *c, const linkaddr_t *from,
	       const linkaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  printf("recv '%s', from %d.%d (origin %d.%d) in %u hops (seq %u) \n", (char *)packetbuf_dataptr(), from->u8[0], from->u8[1],
          originator->u8[0], originator->u8[1], hops, seqno);
return 1;
}
static void
sent(struct rdipro_conn *c)
{
  printf("sent\n");
}
static void
dropped(struct rdipro_conn *c)
{
  printf("dropped\n");
}
static const struct rdipro_callbacks callbacks = { recv, sent, dropped };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(broadcast_process, ev, data)
{
  static struct etimer et;
  static struct etimer et2;

  static uint8_t seqno;
  struct broadcast_message msg;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast));

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);
  etimer_set(&et2, CLOCK_SECOND * 60);

  while(1) {
    if (etimer_expired(&et2)) {
      PROCESS_EXIT();
    }
    /* Send a broadcast every 16 - 32 seconds */
    etimer_set(&et, CLOCK_SECOND * 16 + random_rand() % (CLOCK_SECOND * 16));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    msg.seqno = seqno;
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&broadcast);
    seqno++;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rdipro_process, ev, data)
{
  static uint8_t seq = 0;

  PROCESS_EXITHANDLER(rdipro_close(&rdiproc));
  
  PROCESS_BEGIN();
  rdipro_open(&rdiproc, CLOCK_SECOND, 136, &callbacks);
  SENSORS_ACTIVATE(button_sensor);

  while(1) {
    static struct etimer et;
    //uint16_t longest_distance_neighbor = 0;
    //int i;
    //struct neighbor *tabla_vecino;

    PROCESS_WAIT_EVENT_UNTIL(button_sensor.value(1));
    etimer_set(&et, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    packetbuf_copyfrom("Hej", 4);
    
    /*tabla_vecino = list_head(neighbors_list);
    for (i = 0; i < list_length(neighbors_list); i++) {
      //recorremos cada vecino de la tabla y comprobamos si la distancia es mayor o menor
      //que la que ya tenemos, si es mayor actualizamos para quedarnos con la mayor
      if (tabla_vecino->distance > longest_distance_neighbor) {
        longest_distance_neighbor = tabla_vecino->distance;
      }
      tabla_vecino = list_item_next(tabla_vecino);
    }   */ 

    if (rdipro_send(&rdiproc, seq, rdiproc.longest_distance_neighbor) != 0) {
	    seq++;
    }
  }

  SENSORS_DEACTIVATE(button_sensor);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
