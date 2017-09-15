/*
**********************************************************************
*完成红外发送，接收，处理功能
*1. 每隔400ms,发送 1000Hz红外，时间长度是200ms
*2. 在发送开始后，开始检测P03管脚的电压，与设定最低电压比较
*3. 发送前执行检测，如果发现有脉冲从P04管脚进入，则表明可能有其它发送源
*
***********************************************************************
*/

#include "Common.h"
#include "Board.h"
#include "IRComm.h"
/***********************Local function ***********************/
/***********************Local Defination***********************/

/* Private defines -----------------------------------------------------------*/
#define IR_CARNUM_MAX  1
// Rx status 
#define IR_IDLE      0
#define IR_RX_HEAD_0 1
#define IR_RX_HEAD_1 2
#define IR_RX_DATA_0 3
#define IR_RX_DATA_1 4

// Tx status

#define TX_WAITSENDING 1
#define TX_SEND_HEAD_0 2
#define TX_SEND_HEAD_1 3
#define TX_SEND_DATA_0_HIGH 4
#define TX_SEND_DATA_0_LOW 5
#define TX_SEND_DATA_1 6

//event define
#define IR_EVENT_INT  1
#define IR_EVENT_TIMER 2
// Rx timing 
#define PW_HEAD_LOW    3000 //us
#define PW_HEAD_HIGH   2000 //us
#define PW_DATA_0_HIGH  1000//us logic 1 preble
#define PW_DATA_0_LOW    500//us logic 0 preble
#define PW_DATA_1       500//us data phase1 
#define PW_TOLERANCE   200// us timing tolerance

// Tx timing
#define TX_LC_PRD     (150000l)//us location sending period
#define TX_LC_CDMA    100//max us

// Rx Flag
#define RX_FLG_NEWCAR  1
#define RX_FLG_ATTACK  2

// ctrl mode
#define CTRL_APPMODE   0x00
#define CTRL_RMTMODE   0x01
 
/***********************Global Variables ***********************/
ushort gIRLastTick[4];
ushort gIntSavedTick[4];

EUINT8 gIRSta[4];
EUINT8 gRxBitCnt[4];

EUINT8 gRxBitSet;
EUINT8 gLcTxDataCursor;
EUINT8 gLcTxSta;//location sending status
EUINT8 gattackpending;
EUINT8 grxflg;//to indicate the rx flg, new car is found=1, attact is found=2
EUINT16 gRxData[4];
EUINT16 gTxSendData;

EUINT16 glcCDSATick;
EUINT16 gAttackData;


EUINT8 gNeighborCar;//[IR_CARNUM_MAX];//
EUINT8 gAttackInfo;
EUINT8 gCarNum;
EUINT8 gCarID=0x01;//0x03;
EUINT8 gCtrlMode=CTRL_APPMODE;
 
/* Private functions ---------------------------------------------------------*/
//static uchar IR_CH1_RecvOperate(uchar event,uchar level,ushort tick);
static void IR_RxProcess(void);
static void IR_TxProcess(void);
static uchar IR_RecvOperate(uchar i,uchar event,uchar level,ushort tick);
static uchar ASSER_PULSE(ushort pwcnt,ushort refcnt,ushort tolerance );
static void SaveCarID(uchar dir,ushort rxdata);
static void SaveAttackInfo(uchar dir,ushort rxdata);
static void TxTimerCount(void);
static ushort GetMinus(ushort tick1,ushort tick2);
static uchar CheckData(uchar dir, uchar bitlen,ushort  rxdata);
static void CheckFragData(uchar dir,uchar bitlen,ushort  rxdata);
static uchar IsIRBusy(void);

void FrontIntHandler(void);
void BackIntHandler(void);
void LeftIntHandler(void);
void RightIntHandler(void);
//static uchar GetIOLevel(uchar i);
/*****************************************************************************
brief: IR IO and variables init , called in main.c 
param:  void
return: void
*********************************************************************************/
void IRComm_Init(void)
{
  ushort tmp;
  uchar i;
 
  gIRSta[0]=0;gIRSta[1]=0;gIRSta[2]=0;gIRSta[3]=0;
  
  gLcTxSta=0;//glcpending=0;
	gattackpending=0;//gisLcTx=1;
 
 // assemble the Leading INFO_ID and CAR_ID 
  gAttackData=0;//&=((IR_INFOID_MASK)|(IR_CARID_MASK));
  gAttackData|=0xC000;//((0x0002<<14));
  
  tmp=gCarID;
  tmp<<=10;//IR_CARID_POS;
  gAttackData|=tmp;
 
  grxflg=0;
 
  // clear all buffer of the neibor

  gNeighborCar=0;
	
	RegGPIOIntHandler(EXTI_INT1,FrontIntHandler);
	RegGPIOIntHandler(EXTI_INT0,BackIntHandler);
	RegGPIOIntHandler(EXTI_Px5,LeftIntHandler);
	RegGPIOIntHandler(EXTI_Px6,RightIntHandler);
 
}

void IRCommSta(void)
{
	IR_TxProcess();
	IR_RxProcess();
}
/*****************************************************************************
brief: GetCarNum , get the neighborhood cars num called in protocol.c 
param:  void
return: gCarNum
*********************************************************************************/
uchar GetCarNum(void)
{
  return gCarNum;
}

/*****************************************************************************
brief: GetNeighborCarData , get the neighborhood cars data pointer, called in protocol.c 
param:  void
return: pointer of gNeighborCar
*********************************************************************************/
uchar GetNeighborCarData(void)
{
  return gNeighborCar;

}
/*****************************************************************************
brief: SaveCarID , get the neighborhood cars data from the rxdata, called in protocol.c 
param:  void
return: void
*********************************************************************************/
static void SaveCarID(uchar dir,ushort rxdata)
{
 // uchar i,j;
  uchar id;
  uchar dirmsk;
 // uchar flg;
 // uchar *plife;
  
  id=(uchar)(rxdata>>10);
  id&=0x3F;
  
  if(id==gCarID) return;//
//  if(id>(IR_CARNUM_MAX-1)) return;// id 号超过预定的数值
  dirmsk=(0x01<<(dir-1));
  gNeighborCar=dirmsk;
  grxflg=RX_FLG_NEWCAR; 
}

/*****************************************************************************
brief: SaveAttackInfo , get the attack information from the rxdata, called in protocol.c 
param:  void
return:  void
*********************************************************************************/
static void SaveAttackInfo(uchar dir,ushort rxdata)
{
  uchar info, id;
  rxdata>>=5;
  info=(uchar)(rxdata & 0x001F);
  rxdata>>=5;
  id=(uchar)(rxdata & 0x000F);
  
  if(id==gCarID) return;//
//  gAttackInfo.Dir=dir;
//  gAttackInfo.AttackInfo=info;
//  gAttackInfo.AttackCarID=id;
  gAttackInfo=info;
  grxflg=RX_FLG_ATTACK;
  return;
  
}
/*****************************************************************************
brief: SetAttack ,set the IR module to send out an attack signal, called in protocol.c 
param:  void
return: void
*********************************************************************************/
void SetAttack(uchar attackprofile)
{
  ushort tmp=0;
  
  tmp=attackprofile&0x6F;
  tmp<<=5;//IR_POS_ATTACK_INFO;
  gAttackData&=0xFC1F;//IR_ATTACK_INFO_MASK;
  gAttackData|=tmp;
   
  gattackpending=1;
}
/*****************************************************************************
brief: SetCarID ,set the car ID for intelligent tracing, called in protocol.c 
param:  void
return: void
*********************************************************************************/
void SetCarID(uchar id)
{
  ushort tmp;
 
  gCarID=id;
  
  tmp=id;
  tmp<<=10;//IR_CARID_POS;
 
  
  gAttackData=0;//&=((IR_INFOID_MASK)|(IR_CARID_MASK));
  gAttackData|=0xC000;//((0x0002<<14));
  gAttackData|=tmp;
}

/*****************************************************************************
brief: SetCtrlMode ,setting the car work mode, , called in protocol.c 
param:  mode, =0x00 is app control mode; =0x01 is remotor control mode
return: void
*********************************************************************************/
void SetCtrlMode(uchar mode)
{
  gCtrlMode=mode;
}
/*****************************************************************************
brief: NeighborCarMgr,manage the neighbor of the car, called per 0.5s to performace the 
counting action,  called in main.c 
param:  void
return: void
*********************************************************************************/
//void NeighborCarMgr(void)
//{
/////////////test////////////
//    if(gtesttick)
//    {
//       if(GetMinus(GetIRTick(),gtesttick)>50000)
//       {
////          p_Test=0;
//          gtesttick=0;
//       }
//    
//    }
//  
////////////////////////////////
//}

/*****************************************************************************
brief: IR_TxProcess , for sending a location signal or an attack signal , called in mainloop 
param:  void
return: void
NOTE_1: the location signal will send per 150ms, when the channel is in free. before
sending , the tx will wait for a random time for about 1~2ms to avoid conflicting with
the other ones.

NOTO_2: MSB is first to send
*********************************************************************************/
static void IR_TxProcess(void)
{
  
 
  ushort tick16;
  uchar  tmp;
  // for the timer1 counter is only 65.535ms max with 1us resolution
  // TxTimerCount will extend the counting range to 4294967.295 ms with about 1~10us resolution
  // according to the CPU processing speed
//  TxTimerCount();// 
  
  DisableInterrupts();
  
  if(gCtrlMode!=CTRL_APPMODE)
  {
    gLcTxSta=IR_IDLE;
  }
  
  switch(gLcTxSta)
  {
    case IR_IDLE:
      if(gattackpending)
      {
          if(IsIRBusy())//gIRSta[0] |gIRSta[1]|gIRSta[2]|gIRSta[3]
          {//busying in rx
            break;
          }
          else
          {//send for not waiting
            gLcTxSta=TX_SEND_HEAD_0;
            glcCDSATick=GetIRTick();
						SetAttackPin(1);// p_AttackTx=1;//open the 38KHz output
            gattackpending=0;
         //   gisLcTx=0;
          }
      }
 
    break;
////////////// sending HEAD LOW period////////////////////    
    case TX_SEND_HEAD_0:
 
      tick16=GetMinus(GetIRTick(),glcCDSATick);
      
      if(tick16>PW_HEAD_LOW)
      {
        gLcTxSta=TX_SEND_HEAD_1;
        SetAttackPin(0);//p_AttackTx=0;
        glcCDSATick=GetIRTick();
      }
    break;
////////////// sending HEAD HIGH period////////////////////        
    case TX_SEND_HEAD_1:
 
      tick16=GetMinus(GetIRTick(),glcCDSATick);
      
      if(tick16>PW_HEAD_HIGH)
      {// send MSB
 
        SetAttackPin(1);//p_AttackTx=1;
        gTxSendData=gAttackData;
        
        if(gTxSendData & 0x8000)
        {// to send logic '1'
          gLcTxSta=TX_SEND_DATA_0_HIGH;
        }
        else
        {// to send logic '0'
          gLcTxSta=TX_SEND_DATA_0_LOW;
        }
        
        gLcTxDataCursor=0;
        glcCDSATick=GetIRTick();
      }
    break;
////////////// sending Data Phase0 branch: logic '1' about 1ms////////////////////      
    case TX_SEND_DATA_0_HIGH:
 
      tick16=GetIRTick();
      tick16=GetMinus(tick16,glcCDSATick);
      if(tick16>PW_DATA_0_HIGH)
      {
        SetAttackPin(0);// p_AttackTx=0;
        gLcTxSta=TX_SEND_DATA_1;
        gLcTxDataCursor++;
        glcCDSATick=GetIRTick();
      }
    break;
////////////// sending Data Phase0 branch: logic '0' about 0.5ms////////////////////      
    case TX_SEND_DATA_0_LOW:
 
      tick16=GetMinus(GetIRTick(),glcCDSATick);
      if(tick16>PW_DATA_0_LOW)
      {
 
        SetAttackPin(0);//  p_AttackTx=0;
 
        gLcTxSta=TX_SEND_DATA_1;
        gLcTxDataCursor++;
        glcCDSATick=GetIRTick();
      }
    break;
////////////// sending Data Phase1 0.5ms////////////////////      
    case TX_SEND_DATA_1:
 
      tick16=GetMinus(GetIRTick(),glcCDSATick);
      if(tick16>PW_DATA_1)
      {
        if(gLcTxDataCursor>=16)
        {//complet the sending
          gLcTxSta=IR_IDLE;
        }
        else
        {//sending next bit   send MSB
          gTxSendData<<=1;
          if(gTxSendData & 0x8000)
          {// to send logic '1'
            gLcTxSta=TX_SEND_DATA_0_HIGH;
          }
          else
          {// to send logic '0'
            gLcTxSta=TX_SEND_DATA_0_LOW;
          }
 
          SetAttackPin(1);//  p_AttackTx=1;
 
          glcCDSATick=GetIRTick();
        }
        
      }
    break;
  }
  
  GlobalInterruptEnable();//使能中断全局GIE。

}

/*****************************************************************************
brief: IsIRBusy, check the IR Rx channel status ,call by Tx_Process
param:  void
return: 1=busy, 0=free
*********************************************************************************/
static uchar IsIRBusy(void)
{
  if(gIRSta[0] |gIRSta[1]|gIRSta[2]|gIRSta[3])
  {
    return 1;
  }
  else if(gRxBitSet)
  {
    return 1;
  }
  
  return 0;
}

/*****************************************************************************
brief: IR rx and decoder entrence, called in mainloop 
param:  void
return: void

NOTO_1: MSB is first to RX
*********************************************************************************/
static void IR_RxProcess(void)
{
    uchar level;
    //if(gLcTxSta>TX_IDLE) return;
    uchar i;
    uchar mask;
    
    mask=0x01;
    //gnewcardir=0;
    for(i=0;i<4;i++)
    { //i=0;
      if(gRxBitSet & mask)
      {
        level=GetIOLevel(i);
        IR_RecvOperate(i,IR_EVENT_INT,level,gIntSavedTick[i]);
        gRxBitSet&=(~mask);
      }
      else
      {
        if(gIRSta[i]>IR_IDLE)
        {
          IR_RecvOperate(i,IR_EVENT_TIMER,level,GetIRTick());
        }
      }
      mask<<=1;
    }
     
    if(grxflg==RX_FLG_ATTACK)
    {//send out attack info to master
//      if(PT_PackResp(PT_CMD_ATTACKUPDATE,(uchar*)&gAttackInfo,0))
//      {//good sending
//        grxflg &=(~RX_FLG_ATTACK);
//      }
      
    }
    else if(grxflg==RX_FLG_NEWCAR)
    {//send out new found car to master
//      if(PT_PackResp(PT_CMD_CHECKCAR,(uchar*)gNeighborCar,0)) 
//      {// good sending 
//        // clear all buffer of the neibor
//        for(i=0;i<IR_CARNUM_MAX;i++)
//        {
//          gNeighborCar[i].location=0;
//        }
//        
//        grxflg=0;
//        
//        /////////test////////////      
//       p_Test=1;
//        gtesttick=GetIRTick();
////////////////////////////
//      }

      
    }
    
     
}


/*****************************************************************************
brief: to get the indicated io value  
param: 
return:  
*********************************************************************************/
//static uchar GetIOLevel(uchar i)
//{
//   uchar ret;
//   
//   switch(i)
//   {
//   case 0:
//     ret=p_RxCH1;
//   break;
//   case 1:
//     ret=p_RxCH2;
//   break;
//   case 2:
//     ret=p_RxCH3;
//   break;
//   case 3:
//     ret=p_RxCH4;
//   break;
//   }
//   
//   return ret;
//}


/*****************************************************************************
param: level is the rx io level,=0 for low level; =1 for high levle
       tick is the passed time , 1us per tick

return: if the all rx is ok, then return 1; else return 0;
*********************************************************************************/
//#pragma optimize=none
static uchar IR_RecvOperate(uchar i,uchar event,uchar level,ushort tick)
{
  ushort pw;// pulse width ,unit= us 
  uchar temp;
  
  switch(gIRSta[i])
  {
    case IR_IDLE:
      if(level==0)
      {//falling edge interrupt
        gIRSta[i]=IR_RX_HEAD_0;
        gIRLastTick[i]=tick;//gIRTick_CH1=tick;//the start tick
      }
    break;
 /////////////////// wait for the first low level=wait for rising edge, pulsewidth=3ms////////////////////
    case IR_RX_HEAD_0:
      if(IR_EVENT_INT==event)
      {
        if(level==0) 
        {//error, it should be 1, 
          gIRSta[i]=IR_IDLE;
          break;
        }
        pw=GetMinus(tick,gIRLastTick[i]);// 
        if(ASSER_PULSE(pw,PW_HEAD_LOW,PW_TOLERANCE))
        {
          gIRSta[i]=IR_RX_HEAD_1;
          gIRLastTick[i]=tick;//the start tick
        }
        else
        {
          gIRSta[i]=IR_IDLE;
        }
      }
       
      else if(IR_EVENT_TIMER==event)
      {
        pw=GetMinus(tick,gIRLastTick[i]);// 
        if(pw>(PW_HEAD_LOW+PW_TOLERANCE+1000))
        {//maybe other sources are interrupt the receiving
          gIRSta[i]=IR_IDLE;
        }
      } 
    break;
/////////////// wait for the following high level==wait for falling edge, pulsewidth=2ms/////////////    
    case IR_RX_HEAD_1:
      
      if(IR_EVENT_INT==event)
      {
        if(level==1) 
        {//error, it should be 0, 
          gIRSta[i]=IR_IDLE;
          break;
        }
        
        pw=GetMinus(tick,gIRLastTick[i]);// 
        
        if(ASSER_PULSE(pw,PW_HEAD_HIGH,PW_TOLERANCE))
        {
          gIRSta[i]=IR_RX_DATA_0;
          gIRLastTick[i]=tick;//the start tick
          gRxData[i]=0;
          gRxBitCnt[i]=0;
        }
        else
        {
          gIRSta[i]=IR_IDLE;
        }
      }
      else if(IR_EVENT_TIMER==event)
      {
        pw=GetMinus(tick,gIRLastTick[i]);// 
        if(pw>(PW_HEAD_HIGH+PW_TOLERANCE+100))
        {//maybe lost the signal source
          gIRSta[i]=IR_IDLE;
        }
      }
    break;
 /////////////// DataPhase0, wait for rising edge, pulsewidth=1ms or 0.5ms/////////////    
    case IR_RX_DATA_0:
      if(IR_EVENT_INT==event)
      {
        if(level==0) 
        {//error, it should be 1, 
          //maybe we can still get some infomation from this
          temp=gRxBitCnt[i];
          CheckFragData(i+1,temp,gRxData[i]);
          gIRSta[i]=IR_IDLE;
          break;
        }
        pw=GetMinus(tick,gIRLastTick[i]);// 
        
        if(ASSER_PULSE(pw,PW_DATA_0_HIGH,PW_TOLERANCE))
        {// logic 1 receving, 1ms
          gIRSta[i]=IR_RX_DATA_1;
          gIRLastTick[i]=tick;//the start tick
          gRxData[i]<<=1;
          gRxData[i]|=0x0001;
          gRxBitCnt[i]++;
          
          temp=gRxBitCnt[i];
          if(CheckData(i+1,temp,gRxData[i]))
          {
              gIRSta[i]=IR_IDLE;
              return 1;
          }
        }
        else if(ASSER_PULSE(pw,PW_DATA_0_LOW,PW_TOLERANCE))
        {// logic 0 receving, 0.5ms
          gIRSta[i]=IR_RX_DATA_1;
          gIRLastTick[i]=tick;//the start tick
          gRxData[i]<<=1;
          gRxData[i]&=0xFFFE;
          gRxBitCnt[i]++;
          temp=gRxBitCnt[i];
          if(CheckData(i+1,temp,gRxData[i]))
          {
              gIRSta[i]=IR_IDLE;
              return 1;
          }
          
        }
        else
        {
          temp=gRxBitCnt[i];
          CheckFragData(i+1,temp,gRxData[i]);
          gIRSta[i]=IR_IDLE;
        }
      }
      else if(IR_EVENT_TIMER==event)
      {
        pw=GetMinus(tick,gIRLastTick[i]);// 
        if(pw>(PW_DATA_0_HIGH+PW_TOLERANCE+100))
        {//maybe other sources are interrupt the receiving
          // we still can get some infomation from the error receiving ,if 
          temp=gRxBitCnt[i];
          CheckFragData(i+1,temp,gRxData[i]);
          gIRSta[i]=IR_IDLE;
        }
      }
    
    break;
    /////////////// DataPhase1, level=1,waiting for falling edge, pulsewidth=0.5ms/////////////    
    case IR_RX_DATA_1:
      if(IR_EVENT_INT==event)
      {  
          if(level==1) 
          {//error, it should be 0, 
            temp=gRxBitCnt[i];
            CheckFragData(i+1,temp,gRxData[i]);
            gIRSta[i]=IR_IDLE;
            break;
          }
          
          pw=GetMinus(tick,gIRLastTick[i]);// 
          
          if(ASSER_PULSE(pw,PW_DATA_1,PW_TOLERANCE))
          {
            gIRSta[i]=IR_RX_DATA_0;
            gIRLastTick[i]=tick;//the start tick
          }
          else
          {
            temp=gRxBitCnt[i];
            CheckFragData(i+1,temp,gRxData[i]);
            gIRSta[i]=IR_IDLE;
          }
      }
      else if(IR_EVENT_TIMER==event)
      {
        pw=GetMinus(tick,gIRLastTick[i]);// 
        if(pw>(PW_DATA_1+PW_TOLERANCE+100))
        {//maybe lost the signal source
          // we still can get some infomation from the error receiving ,if 
          temp=gRxBitCnt[i];
          CheckFragData(i+1,temp,gRxData[i]);
          
          gIRSta[i]=IR_IDLE;
        }
      }
    break;
  }
  
  return 0;
}

 
/*****************************************************************************
brief: CheckData , to check the rxdata information is completed, 
param:    
return: =0 for not complete, else ,=1 good data
*********************************************************************************/
static uchar CheckData(uchar dir, uchar bitlen,ushort  rxdata)
{
   
  if(bitlen==8)
  {
    if((rxdata & 0x00C0)==0)
    {//location
      SaveCarID(dir,(rxdata<<8));
      return 1;
    }
  }
  else if(bitlen==16)
  {
    SaveAttackInfo(dir,rxdata);
    return 1;
  }
  
  return 0;
}

/*****************************************************************************
brief: CheckData , to check the fragment rxdata information is still can be used,and
get useful information from it
param:    
return: void
*********************************************************************************/
static void CheckFragData(uchar dir,uchar bitlen,ushort  rxdata)
{
  if(bitlen>=6)
  {// can get the car id, and   
      rxdata<<=(16-bitlen);// to alien the data
      SaveCarID(dir,rxdata);
      if(bitlen>=11)
      {// get the attack infomation
        SaveAttackInfo(dir,rxdata);
      }
  }
  
  return ;
}



/*****************************************************************************
brief: ASSER_PULSE , to check the pwcnt whether in range local in refcnt ± tolerance 
param:    
return: =0 for outsize the range, else ,=1 for inside the range
*********************************************************************************/
static uchar ASSER_PULSE(ushort pwcnt,ushort refcnt,ushort tolerance )
{
  ushort t1, t2;
  
  t1=refcnt+tolerance;
  t2=refcnt-tolerance;

  if((t1>=pwcnt) && (pwcnt>=t2) )
  {
    return 1;
  }
  else 
  {
    return 0;
  }
}

//#pragma optimize=high

/*****************************************************************************
brief:TxTimerCount ,extend the counting range from 65.535ms to 4294967.295 ms
   for the timer1 counter is only 65.535ms max with 1us resolution
   TxTimerCount will extend the counting range to 4294967.295 ms with about 1~10us resolution
   according to the CPU processing speed
param:    
return: =0 for outsize the range, else ,=1 for inside the range
*********************************************************************************/
//static void TxTimerCount(void)
//{
//  ushort curtick;
//  unsigned long tick;
//  
//  
//  curtick=GetIRTick();
//  
//  if(curtick>gTxTickLastNibble)
//  {
//    gTxTickCnt+=(curtick-gTxTickLastNibble);
//  }
//  else
//  {
//    gTxTickCnt+=((0xFFFF-gTxTickLastNibble) + curtick);
//  }
//  
//  gTxTickLastNibble=curtick;
//  
//  if(gCtrlMode==CTRL_APPMODE)
//  {
//    if(glcpending==0)
//    {
//      if(gTxTickCnt>gTxTickCntLast)
//      {
//        tick=gTxTickCnt-gTxTickCntLast;
//        if(tick>TX_LC_PRD)
//        {
//          glcpending=1;
//          gTxTickCntLast=gTxTickCnt;
//        }
//      }
//    }
//  }
//  else
//  {
//    glcpending=0;
//  }
//  
//   
//}

/*****************************************************************************
brief: GetMinus ,produce tick1-tick2, with boundary check
param:    
return:  tick1-tick2
*********************************************************************************/
static ushort GetMinus(ushort tick1,ushort tick2)
{
  ushort pw;
  
  if(tick1<tick2)
  {
     pw=(0xFFFF-tick2)+tick1;
  }
  else
  {
     pw=tick1-tick2;
  }
  
  return pw;

}



/*****************************************************************************
param: Rx CH1 Initerrupt server routine
return: void
*********************************************************************************/
void FrontIntHandler(void)
{
  gIntSavedTick[0]=GetIRTick();
  gRxBitSet|=0x01;
}

/*****************************************************************************
param: Rx CH2 Initerrupt server routine
return: void
*********************************************************************************/
void BackIntHandler(void)
{
  gIntSavedTick[1]=GetIRTick();
  gRxBitSet|=0x02;
}


/*****************************************************************************
param: Rx CH3 Initerrupt server routine
return: void
*********************************************************************************/
void LeftIntHandler(void)
{
  gIntSavedTick[2]=GetIRTick();
  gRxBitSet|=0x04;
}

/*****************************************************************************
param: Rx CH4 Initerrupt server routine
return: void
*********************************************************************************/
void RightIntHandler(void)
{
  gIntSavedTick[3]=GetIRTick();
  gRxBitSet|=0x08;
}
