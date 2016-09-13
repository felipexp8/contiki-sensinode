 

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "debug.h"

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

#define INTERVALO		10 * CLOCK_SECOND
#define MAX_PAYLOAD_LEN		40

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

static char buf[MAX_PAYLOAD_LEN];

#define PORTA 8802

#define FAZER_IPV6 (uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0x0212, 0x4b00, 0x07b9, 0x5e8d))

static struct uip_udp_conn *conexao;
 
#define LED_TOGGLE_REQUEST 	(0x79)
#define LED_SET_STATE 	    	(0x7A)
#define LED_GET_STATE 	    	(0x7B)
#define LED_STATE 		        (0x7C)
 
PROCESS(udp_client_process, "UDP client process");
 
AUTOSTART_PROCESSES(&udp_client_process);
 
 
static void aoReceber()
{
 
 

}
 
static void enviarChar(char a)
{
  uip_udp_packet_send(conexao, a, 1);
}

static void aoDispararTimer()
{  
  memset(buf, 0, MAX_PAYLOAD_LEN); //zera o buffer global

  PRINTF("Cliente para [");
  PRINT6ADDR(&g_conn->ripaddr);
  PRINTF("]:%u,", UIP_HTONS(g_conn->rport));
   
  sprintf(buf, "Mensagem Aqui");
  uip_udp_packet_send(g_conn, buf, MAX_PAYLOAD_LEN);
  PRINTF("\n")
  print_local_addresses();
}
 
static void print_local_addresses(void) // 
{
  int i;
  uint8_t state;
  PRINTF("Nodes's IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) 
  {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED))
    {
      PRINTF("  \n"); PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      if(state == ADDR_TENTATIVE) { uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;}
    }
  }
}

PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer et;
  uip_ipaddr_t ipaddr;

  PROCESS_BEGIN();
  
  PRINTF("Thread Inicializada\n");

  FAZER_IPV6;
  
  conexao = udp_new(&ipaddr, UIP_HTONS(PORTA), NULL);
  
  if(!conexao) 
  {
    PRINTF("erro ao iniciar udp"\n");
  }
  udp_bind(conexao, UIP_HTONS(GLOBAL_CONN_PORT));

  print_local_addresses();

  PRINTF("Socket com: ");
  PRINT6ADDR(&conexao->ripaddr);
  PRINTF("porta: %u/%u\n", UIP_HTONS(conexao->lport), UIP_HTONS(conexao->rport));

  etimer_set(&et, SEND_INTERVAL);

  while(1) 
  {
    PROCESS_WAIT_EVENT();
    
    if(etimer_expired(&et)) 
    {
      aoDispararTimer();
      etimer_restart(&et);
    } 
    else if(ev == tcpip_event)
    {
      aoReceber();
    }
  }

  PROCESS_END();
}

