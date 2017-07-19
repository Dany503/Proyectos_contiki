#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>
extern process_event_t serial_line_event_message;
/*---------------------------------------------------------------------------*/
PROCESS(example_netflood_process, "HP-Flood algorithm");
AUTOSTART_PROCESSES(&example_netflood_process);
/*---------------------------------------------------------------------------*/
static int netflood_recv(struct netflood_conn *c, const linkaddr_t * from,
			 const linkaddr_t * originator, uint8_t seqno, uint8_t hops);
static void netflood_sent(struct netflood_conn *c);
static void netflood_dropped(struct netflood_conn *c);
/*---------------------------------------------------------------------------*/
static const struct netflood_callbacks netflood_call =
    { netflood_recv, netflood_sent, netflood_dropped };
static struct netflood_conn netflood;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_netflood_process, ev, data)
{
	static int pktcount = 0;
	PROCESS_EXITHANDLER(netflood_close(&netflood);
	    )
	    SENSORS_ACTIVATE(button_sensor);
	    PROCESS_BEGIN();

	netflood_open(&netflood, CLOCK_SECOND * 2, 128, &netflood_call);

	while (1) {

		//PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message);
		PROCESS_WAIT_EVENT_UNTIL(sensors_event);
		// 5B: HOLA - 10B: HOLA HOLA - 15B: HOLA HOLA HOLA - 20B: HOLA HOLA HOLA HOLA
		packetbuf_copyfrom("HOLA HOLA HOLA HOLA", sizeof("HOLA HOLA HOLA HOLA"));
		netflood_send(&netflood, (uint8_t) pktcount);
		pktcount++;
	}

	PROCESS_END();
}

/*---------------------------------------------------------------------------*/

static int netflood_recv(struct netflood_conn *c, const linkaddr_t * from,
			 const linkaddr_t * originator, uint8_t seqno, uint8_t hops)
{
	printf
	    ("Paquete recibido de %u.%u a través de  %u.%u, seqno=%u, saltos=%u\n",
	     originator->u8[0], originator->u8[1], from->u8[0], from->u8[1],
	     seqno, hops);

	if (originator->u8[0] == 206 && originator->u8[1] == 178) {
		// emitter is senslab node 1
		printf("senal recibida, encendiendo LEDS !!!\n");
		leds_on(LEDS_ALL);
	}
	// return 1 means do forward
	return 1;
}

static void netflood_sent(struct netflood_conn *c)
{
	printf("Paquete enviado !\n");
}

static void netflood_dropped(struct netflood_conn *c)
{
	printf("Paquete tirado !\n");
}
