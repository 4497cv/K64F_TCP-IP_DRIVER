
#ifndef LWIP_TCPECHO_H
#define LWIP_TCPECHO_H

void tcp_ip_init(tcp_ip_config config);
boolean_t verify_message_received(void);
void increase_message_count(void);
boolean_t verify_message_count(uint8_t number_of_messages);
void send_message_string(void* data, uint16_t len);

#endif /* LWIP_TCPECHO_H */
