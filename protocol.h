#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define DATA_LEN	8
#define LEN_PACKET_MAX 15

extern uint8_t RX_Packet[LEN_PACKET_MAX];

void Rx_Package_Handle(uint8_t *uartBuf);
void RecievePacket(uint8_t d);
uint8_t CheckTimeOut(void);
void ResetIndexRx(void);
u8 Check_Recieve_Valid(void);

#	endif