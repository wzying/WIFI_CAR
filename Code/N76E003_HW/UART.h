#ifndef _UART_H
#define _UART_H

#define DATA_LEN	11 
void Rx_Package_Handle(uint8_t *uartBuf);
void TestUartInit(void);
void UART0SendData(UINT8* pdin,UINT8 dlen);
uint8_t CheckTimeOut(void);
void ResetIndexRx(void);
#endif