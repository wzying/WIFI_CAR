typedef bit                   BIT;
typedef unsigned char         UINT8;
typedef unsigned int          UINT16;
typedef unsigned long         UINT32;

typedef unsigned char         uchar;
typedef unsigned int          ushort;

typedef unsigned char         uint8_t;
typedef unsigned int          uint16_t;
typedef unsigned long         uint32_t;
typedef xdata unsigned char   EUINT8;
typedef xdata unsigned int    EUINT16;
typedef xdata unsigned long   EUINT32;
typedef code unsigned char    ROUINT8;
typedef code unsigned int     ROUINT16;
typedef code unsigned long    ROUINT32;
typedef void (*FN_INT_HANDLER)(void);

typedef char  s8;
typedef int   s16;
typedef long  s32;

#define u8  EUINT8
#define u16 EUINT16
#define u32 EUINT32



#define     CID_READ				0x0B
#define     DID_READ				0x0C

#define     ERASE_APROM				0x22
#define     READ_APROM				0x00
#define     PROGRAM_APROM			0x21
#define     ERASE_LDROM				
#define     READ_LDROM				
#define     PROGRAM_LDROM			
#define     READ_CFG					0xC0
#define     PROGRAM_CFG				0xE1
#define			READ_UID					0x04

#define 		EXTI_INT0					0
#define     EXTI_INT1         1
#define     EXTI_Px5          7
#define     EXTI_Px6          8

void GlobalInterruptEnable(void);
void DisableInterrupts(void);
void Send_Data_To_UART0 (UINT8 c);
void MyPrintf(uint8_t *buf);
void SendDataUart(uint8_t *buf,uint8_t len);
void InitialUART0_Timer3(UINT32 u32Baudrate); //use timer3 as Baudrate generator
void InitialUART0_Timer1(UINT32 u32Baudrate); 
//extern bit BIT_TMP;
