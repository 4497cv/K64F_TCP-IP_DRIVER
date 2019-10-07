
#include "tcp_ip.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"
#include "tiny-AES-c/aes.h"

#define MSG_LEN 2
#define MSG_NUM 4

static void *data_1 = "P1";
static void *data_2 = "P2";
static void *data_3 = "P3";
static void *data_4 = "P4";

// static uint8_t key[16] = { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
// struct AES_ctx ctx;
  //AES_init_ctx(&ctx, key);
//      AES_ECB_encrypt(&ctx, data_1);
 // AES_ECB_decrypt(&ctx, buf);
typedef struct 
{
   node_t node_type;
   uint8_t HOST_ADDR[4];
   uint8_t PORT;
   ip4_addr_t host_ip_addr;
} tcp_ip_config;

typedef enum
{
  CLIENT,
  SERVER
} node_t;

static uint8_t n_messages;

static void tcp_ip_client_thread(void *arg)
{
  struct netconn *conn;
  struct netbuf *buf;
  void *data;
  err_t err;
  uint16_t len;
  boolean_t message_received;
  boolean_t message_count;

  LWIP_UNUSED_ARG(arg);
  /* Create new connection */
  conn = netconn_new(NETCONN_TCP);
  /* Set message length */
  len = 2;

	printf("trying to connect to server...\n \r");

  while (1)
  {
    /* connecting to server */
	  err = netconn_connect(conn, &config.host_ip_addr, config.PORT);

    if (err == ERR_OK)
    {
      len = MSG_LEN;
      n_messages = 0;

      printf("connected to server \n \r");
      printf("\nSending data.. \n\r");

      /* Sending data 1 */
      send_message_string(data_1, len);
      /* sending data 2 */
      send_message_string(data_2, len);
      /* sending data 3 */
      send_message_string(data_3, len);
      /* sending data 4 */
      send_message_string(data_4, len);

      PRINTF("waiting for server's response...\n \r");

      /* verify if message is received */
      message_received = verify_message_received();
      /* verify if message is message count has reached its limit */
      message_count = verify_message_count(MSG_NUM);

      while ((TRUE == message_received) && (FALSE == message_count))
      {
          do 
          {
               PRINTF("\n data received: ");
               netbuf_data(buf, &data, &len);
			         PRINTF(data);
               increase_message_count();
          } while (netbuf_next(buf) >= 0);
         /* delete message buffer */
         netbuf_delete(buf);
       }

      /* Close connection and discard connection identifier. */
      netconn_close(conn);
      /* Delete connection */
      netconn_delete(conn);
    }
  }
}

static void tcp_ip_server_thread(void *arg)
{

}

void tcp_ip_init(tcp_ip_config config)
{

	/* Set network host's IP address */
	IP4_ADDR(&config.host_ip_addr, config.HOST_ADDR[0], config.HOST_ADDR[1], config.HOST_ADDR[2], config.HOST_ADDR[3]);

  switch(config.node_type)
  {
    case CLIENT:
      sys_thread_new("tcpecho_thread", tcp_ip_client_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
    break;
    case SERVER:
      sys_thread_new("tcpecho_thread", tcp_ip_server_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
    break;
    case default:
    break;
  }
}

boolean_t verify_message_received(void)
{
  boolean_t val;

  if(((netconn_recv(conn, &buf)) == ERR_OK)
  {
    val = TRUE;
  }
  else
  {
    val = FALSE;
  } 

  return val;
}

void increase_message_count(void)
{
  n_messages++;
}

boolean_t verify_message_count(uint8_t number_of_messages)
{
  boolean_t val;

  if(number_of_messages == n_messages)
  {
    val = TRUE;
  }
  else
  {
    val = FALSE;
  } 
  
  return val;
}

void send_message_string(void* data, uint16_t len)
{
  netconn_write(conn, data, len, NETCONN_NOFLAG); // Send first message 
  vTaskDelay(100);
}