#ifndef _UART_H
#define _UART_H

#define DATA_LEN	11 
#define LEN_PACKET_MAX 20

extern uint8_t RX_Packet[LEN_PACKET_MAX];

void Rx_Package_Handle(uint8_t *uartBuf);
void TestUartInit(void);
void UART0SendData(UINT8* pdin,UINT8 dlen);
uint8_t CheckTimeOut(void);
void ResetIndexRx(void);
u8 Check_Recieve_Valid(void);

#endif