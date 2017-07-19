
#include "net/rime/netflood.h"
#include "contiki.h"
#include "dev/button-sensor.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_netflood_process, "");
AUTOSTART_PROCESSES(&example_netflood_process);
/*---------------------------------------------------------------------------*/
static int
recv(struct netflood_conn *c, const linkaddr_t *from,
	       const linkaddr_t *originator, uint8_t seqno, uint8_t hops)
{
  printf("recv '%s', from %d.%d (origin %d.%d) in %u hops (seq %u) \n", (char *)packetbuf_dataptr(), from->u8[0], from->u8[1],
          originator->u8[0], originator->u8[1], hops, seqno);
return 1;
}
static void
sent(struct netflood_conn *c)
{
  printf("sent\n");
}
static void
dropped(struct netflood_conn *c)
{
  printf("dropped\n");
}
static const struct netflood_callbacks callbacks = { recv, sent, dropped };
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_netflood_process, ev, data)
{
  static struct netflood_conn c;
  static uint8_t seq = 0;

  PROCESS_EXITHANDLER(netflood_close(&c));
  
  PROCESS_BEGIN();
  netflood_open(&c, CLOCK_SECOND, 136, &callbacks);
  SENSORS_ACTIVATE(button_sensor);

  while(1) {
    static struct etimer et;
    PROCESS_WAIT_EVENT_UNTIL(button_sensor.value(1));
    etimer_set(&et, CLOCK_SECOND * 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    packetbuf_copyfrom("Hej", 4);
	/*int netflood_send(struct netflood_conn *c, uint8_t seqno); */
    if (netflood_send(&c, seq) != 0) {
	seq++;
    }
  }

  SENSORS_DEACTIVATE(button_sensor);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
