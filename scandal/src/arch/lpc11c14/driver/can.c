/****************************************************************************
 *   $Id:: can.c 3635 2010-06-02 00:31:46Z usb00423                         $
 *   Project: NXP LPC11xx CAN example
 *
 *   Description:
 *     This file contains CAN code example which include CAN 
 *     initialization, CAN interrupt handler, and APIs for CAN
 *     access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/

/*	Progress on editing (Porting of SCANDAL)
 * 		-Sending data:
 * 			-Sending packet of SCANDAL format with timestamp (working 13 Aug 2011)
 * 			-Buffering of TX messages in the message objects (Using MSG OBJ 21-32)
 *
 * 		-Receiving data:
 * 			-Basic interrupt based receiving (working)
 * 			-Conversion of received data into SCANDAL format and handling this externally (depending on each users needs)
 * 			-Receiving timestamps and syncing the clock
 *
 * 		-Configuration of registers:
 * 			-Initialisation routine	(written, need to test)
 * 			-Setting the initialisation parameters from an external function (that each user writes depending on their needs)
 *
 */
#include "driver_config.h"
#if CONFIG_ENABLE_DRIVER_CAN==1
#include "can.h"

#include "gpio.h"
#include "timer32.h"

#include <scandal/can.h>

#define CUSTOM_CONFIG 1
#define REORDER_DATA 1
/* statistics of all the interrupts */
volatile uint32_t BOffCnt = 0;
volatile uint32_t EWarnCnt = 0;
volatile uint32_t EPassCnt = 0;

uint32_t CANRxDone[MSG_OBJ_MAX];

message_object can_buff[MSG_OBJ_MAX];
int32_t SCANDALClockOffset=0;
#if CAN_DEBUG
uint32_t CANStatusLog[100];
uint32_t CANStatusLogCount = 0;
#endif

/*********************************
 * Scandal implementation wrappers
 */

void init_can(void) {
	/* do some init stuff */
}

/*! Get a message from the CAN controller. */
u08  can_get_msg(can_msg* msg) {
	return 0;
}

/*! Send a message using the CAN controller */
u08  can_send_msg(can_msg* msg, u08 priority) {
	return 0;
}

/*! Register a message ID/mask. This guarantees that these messages will
  not be filtered out by hardware filters. Other messages are not
  guaranteed */
u08  can_register_id(u32 mask, u32 data, u08 priority) {
	return 0;
}

/*! Should be called when the CAN controller has an interrupt */
/*! \todo This is probably not the right location for this */
void can_interrupt(void) {

}

/*! Should be called when there is idle time available and the CAN
	controller is able to do some housekeeping */
void can_poll(void) {

}

/* Parameter settings */
u08  can_baud_rate(u08 mode) {
	return 0;
}

/* Enable and disable CAN interrupts */
void  enable_can_interrupt(void) {

}

void  disable_can_interrupt(void) {

}

/* END SCANDAL WRAPPERS
 **********************/

/*****************************************************************************
** Function name:		CAN_CustomConfigureMessages
**
** Descriptions:		Customisation of the original ConfigureMessages function
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
#if CUSTOM_CONFIG
void CAN_CustomConfigureMessages( void )
{
  uint32_t i;
  uint32_t ext_frame = 1; //Originally 0, set to 1
  uint32_t filtermask;
  uint32_t filteraddr;
  uint8_t eob=1;

  /* It's organized in such a way that:
	    obj(x+1)	standard	receive
	    obj(x+2)	standard	transmit
	    obj(x+3)	extended	receive
	    obj(x+4)	extended	transmit	where x is 0 to 7
	    obj31 is not used.
		obj32 is for remote date request test only */

  for ( i = 0; i < MSG_OBJ_MAX; i++ )
  {
	  CAN_MsgConfigParam(i, &eob, &filtermask, &filteraddr); //Fetches the mask and the address to store in the CAN message object
	  LPC_CAN->IF1_CMDMSK = WR|MASK|ARB|CTRL|DATAA|DATAB; //Configuring (Writing to) the message objects

	  /* Mxtd: 1, Mdir: 0, Mask is 0x1FFFFFFF */
	  LPC_CAN->IF1_MSK1 = filtermask & 0xFFFF; //Filtermask used to be ID_EXT_MASK
	  LPC_CAN->IF1_MSK2 = MASK_MXTD | (filtermask >> 16);

	  /* MsgVal: 1, Mtd: 1, Dir: 0, ID = 0x100000 */
	  LPC_CAN->IF1_ARB1 = (filteraddr) & 0xFFFF; //filteraddr used to be RX_EXT_MSG_ID + i
	  LPC_CAN->IF1_ARB2 = ID_MVAL | ID_MTD | (filteraddr >> 16); //Transmit (1<<13)

	  LPC_CAN->IF1_MCTRL = UMSK|RXIE|EOB|DLC_MAX;

	LPC_CAN->IF1_DA1 = 0x0000;
	LPC_CAN->IF1_DA2 = 0x0000;
	LPC_CAN->IF1_DB1 = 0x0000;
	LPC_CAN->IF1_DB2 = 0x0000;

	/* Transfer data to message RAM */
#if BASIC_MODE
	LPC_CAN->IF1_CMDREQ = IFCREQ_BUSY;
#else
	LPC_CAN->IF1_CMDREQ = i+1;
#endif
	while( LPC_CAN->IF1_CMDREQ & IFCREQ_BUSY );
  }
  return;
}


void CAN_MsgConfigParam(uint8_t msg_no, uint8_t *eob, uint32_t *filtermask, uint32_t *filteraddr){

#define MASK_PRIO 	(0x07 << 26)
#define MASK_TYPE	(0xFF << 18)
#define MASK_ADDR	(0xFF << 10)
#define MASK_CHNL	(0x03FF) //Note, this becomes

#define FLT_TYPE_CHNL (0 << 18) //Channel Message
#define FLT_TYPE_CONF (1 << 18) //Configuration Message
#define FLT_TYPE_HTBT (2 << 18) //Heartbeat Message
#define FLT_TYPE_EROR (3 << 18) //Error
#define FLT_TYPE_RSET (4 << 18) //Reset
#define FLT_TYPE_NONE (10 << 18) //Filters out a type of message that is undefined and shouldn't receive anything.


	//(Ext << 30) | (Pri << 26) | ( MsgType << 18) | (NodAddr << 10) | NodTyp
	//Ext will not be needed for sending (I THINK) so testing without it

	//*filteraddr format: ( << 26) | ( << 18) | ( << 10) | CHNL

	switch(msg_no){
		case 0: //Timesync
			*filtermask = (MASK_TYPE | MASK_ADDR | MASK_CHNL);
			*filteraddr = 0;
			*eob = 1;
			break;

		case 1: //Velocity
			*filtermask = (MASK_TYPE | MASK_ADDR | MASK_CHNL);
			*filteraddr = 0;
			*eob = 1;
			break;

		default:
			*filtermask= (MASK_PRIO | MASK_TYPE | MASK_ADDR | MASK_CHNL);
			*filteraddr = FLT_TYPE_NONE;
			*eob = 1;
			break;
		}

}
#endif

/*****************************************************************************
** Function name:		MessageProcess
**
** Descriptions:		A valid message received, process message
**
** parameters:			Message Object number
** Returned value:		None
** 
*****************************************************************************/
void CAN_MessageProcess( uint8_t MsgNo )
{
  uint32_t MsgID;
  uint32_t *p_add;

#if BASIC_MODE
  LPC_CAN->IF2_CMDMSK = RD|MASK|ARB|TREQ|DATAA|DATAB;
  LPC_CAN->IF2_CMDREQ = IFCREQ_BUSY;    /* Start message transfer */
#else
  while ( LPC_CAN->IF2_CMDREQ & IFCREQ_BUSY );
  LPC_CAN->IF2_CMDMSK = RD|MASK|ARB|CTRL|INTPND|TREQ|DATAA|DATAB;	
  LPC_CAN->IF2_CMDREQ = MsgNo+1;    /* Start message transfer */
#endif	
  while ( LPC_CAN->IF2_CMDREQ & IFCREQ_BUSY );	/* Check new data bit */

  p_add = (uint32_t *)&can_buff[MsgNo];
  if( LPC_CAN->IF2_ARB2 & ID_MTD )	/* bit 28-0 is 29 bit extended frame */
  {
	/* mask off MsgVal and Dir */ 
	MsgID = (LPC_CAN->IF2_ARB1|((LPC_CAN->IF2_ARB2&0x5FFF)<<16));
  }
  else
  {
	/* bit 28-18 is 11-bit standard frame */
	MsgID = (LPC_CAN->IF2_ARB2 &0x1FFF) >> 2;
  }

  *p_add++ = MsgID;
  *p_add++ = LPC_CAN->IF2_MCTRL & 0x000F;	// Get Msg Obj Data length


#if REORDER_DATA
  uint32_t msghold;
  uint32_t msgleftshift;
  uint32_t msgrightshift;

	  msghold=LPC_CAN->IF2_DA1; //Stores the next two bytes in msghold
	  msgleftshift = (msghold & 0x000000FF)<<8; //Shifts the last byte left by 8 bits
	  msgrightshift = (msghold & 0x0000FF00)>>8; //Shifts the second last byte right by 8 bits
	  *p_add++ = msgleftshift | msgrightshift; //OR's the two shifted results to give the data in the right order

	  msghold=LPC_CAN->IF2_DA2; //Stores the next two bytes in msghold
	  msgleftshift = (msghold & 0x000000FF)<<8; //Shifts the last byte left by 8 bits
	  msgrightshift = (msghold & 0x0000FF00)>>8; //Shifts the second last byte right by 8 bits
	  *p_add++ = msgleftshift | msgrightshift; //OR's the two shifted results to give the data in the right order

	  msghold=LPC_CAN->IF2_DB1; //Stores the next two bytes in msghold
	  msgleftshift = (msghold & 0x000000FF)<<8; //Shifts the last byte left by 8 bits
	  msgrightshift = (msghold & 0x0000FF00)>>8; //Shifts the second last byte right by 8 bits
	  *p_add++ = msgleftshift | msgrightshift; //OR's the two shifted results to give the data in the right order

	  msghold=LPC_CAN->IF2_DB2; //Stores the next two bytes in msghold
	  msgleftshift = (msghold & 0x000000FF)<<8; //Shifts the last byte left by 8 bits
	  msgrightshift = (msghold & 0x0000FF00)>>8; //Shifts the second last byte right by 8 bits
	  *p_add++ = msgleftshift | msgrightshift; //OR's the two shifted results to give the data in the right order

#else
  *p_add++ = LPC_CAN->IF2_DA1;
  *p_add++ = LPC_CAN->IF2_DA2;
  *p_add++ = LPC_CAN->IF2_DB1;
  *p_add++ = LPC_CAN->IF2_DB2;
#endif
  return;
}

#if !POLLING
#if CONFIG_CAN_DEFAULT_CAN_IRQHANDLER==1
/*****************************************************************************
** Function name:		CAN_IRQHandler
**
** Descriptions:		Processing CAN interrupt
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_IRQHandler(void) 
{
  uint32_t canstat = canstat;
  uint32_t can_int, msg_no;

  while ( (can_int = LPC_CAN->INT) != 0 )
  {
	if ( can_int & CAN_STATUS_INTERRUPT )
	{
	  canstat = LPC_CAN->STAT;
#if CAN_DEBUG
	  CANStatusLog[CANStatusLogCount++] = canstat;
#endif
	  if ( canstat & STAT_EWARN )
	  {
		EWarnCnt++;
		return;
	  }
	  if ( canstat & STAT_BOFF )
	  {
		BOffCnt++;
		return;
	  }
	}
	else
	{
      if ( (canstat & STAT_LEC) == 0 ) 	/* NO ERROR */
	  {
		/* deal with RX only for now. */
		msg_no = can_int & 0x7FFF;
		if ( (msg_no >= 0x01) && (msg_no <= 0x20) )
		{
		  LPC_CAN->STAT &= ~STAT_RXOK;
		  CAN_MessageProcess( msg_no-1 );
		  CANRxDone[msg_no-1] = TRUE;
		}
	  }
	}
  }	
  return;
}
#endif
#endif

/*****************************************************************************
** Function name:		CAN_ConfigureMessages
**
** Descriptions:		Configure all the message buffers(32) that
**           			starting from message object one, the RX and TX 
**                      alternate. It's configured to support both standard
**						and extended frame type. 
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_ConfigureMessages( void )
{
  uint32_t i;
  uint32_t ext_frame = 1; //Originally 0, set to 1

  /* It's organized in such a way that:
	    obj(x+1)	standard	receive
	    obj(x+2)	standard	transmit 
	    obj(x+3)	extended	receive
	    obj(x+4)	extended	transmit	where x is 0 to 7
	    obj31 is not used. 
		obj32 is for remote date request test only */
  
  for ( i = 0; i < MSG_OBJ_MAX; i++ )
  {
	LPC_CAN->IF1_CMDMSK = WR|MASK|ARB|CTRL|DATAA|DATAB; //Configuring (Writing to) the message objects

	if ( ext_frame == 0 )
	{
	  /* Mxtd: 0, Mdir: 0, Mask is 0x7FF */
	  LPC_CAN->IF1_MSK1 = 0x0000;
	  LPC_CAN->IF1_MSK2 = ID_STD_MASK << 2; //Masking 11 MSB out of 13 bits

	  /* MsgVal: 1, Mtd: 0, Dir: 0, ID = 0x100 */
	  LPC_CAN->IF1_ARB1 = 0x0000;
	  LPC_CAN->IF1_ARB2 = ID_MVAL | ((RX_MSG_ID+i) << 2); //Receiving (0<<13)

	  if ( (i % 0x02) == 0 )
	  {
		LPC_CAN->IF1_MCTRL = UMSK|DLC_MAX;//UMSK|EOB|DLC_MAX;
	  }
	  else
	  {
		LPC_CAN->IF1_MCTRL = UMSK|RXIE|DLC_MAX;//UMSK|RXIE|EOB|DLC_MAX;
		ext_frame = 1;
	  }
	}
	else
	{
	  /* Mxtd: 1, Mdir: 0, Mask is 0x1FFFFFFF */
	  LPC_CAN->IF1_MSK1 = ID_EXT_MASK & 0xFFFF;
	  LPC_CAN->IF1_MSK2 = MASK_MXTD | (ID_EXT_MASK >> 16);

	  /* MsgVal: 1, Mtd: 1, Dir: 0, ID = 0x100000 */
	  LPC_CAN->IF1_ARB1 = (RX_EXT_MSG_ID) & 0xFFFF;
	  LPC_CAN->IF1_ARB2 = ID_MVAL | ID_MTD | ((RX_EXT_MSG_ID) >> 16); //Transmit (1<<13)

	  if ( i<19 )//(i % 0x02) == 0
	  {
		LPC_CAN->IF1_MCTRL = UMSK|RXIE|DLC_MAX;//UMSK|EOB|DLC_MAX; Previously didnt take extended headers
	  }
	  else
	  {
		LPC_CAN->IF1_MCTRL = UMSK|RXIE|EOB|DLC_MAX;//UMSK|RXIE|EOB|DLC_MAX;
		//ext_frame = 0;
	  }
	}
	LPC_CAN->IF1_DA1 = 0x0000;
	LPC_CAN->IF1_DA2 = 0x0000;
	LPC_CAN->IF1_DB1 = 0x0000;
	LPC_CAN->IF1_DB2 = 0x0000;

	/* Transfer data to message RAM */
#if BASIC_MODE
	LPC_CAN->IF1_CMDREQ = IFCREQ_BUSY;
#else
	LPC_CAN->IF1_CMDREQ = i+1;
#endif
	while( LPC_CAN->IF1_CMDREQ & IFCREQ_BUSY );
  }
  return;
}

/*****************************************************************************
** Function name:		CAN_Init
**
** Descriptions:		CAN clock, port initialization
**				
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void CAN_Init( uint32_t CANBitClk )
{
  LPC_SYSCON->PRESETCTRL |= (0x1<<3);
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<17);

  /* The USB D- and CAN RX share the dedicated pin. The USB D+ 
  and CAN TX share the dedicated pin. so, no IO configuration is 
  needed for CAN. */
  if ( !(LPC_CAN->CNTL & CTRL_INIT) )
  {
	/* If it's in normal operation already, stop it, reconfigure 
	everything first, then restart. */
	LPC_CAN->CNTL |= CTRL_INIT;		/* Default state */
  }

#if USE_DEFAULT_BIT_TIMING
  /* AHB clock is 48Mhz. The CAN clock divider is within CAN block, 
  set it to 8Mhz for now. Thus, default bit timing doesn't need to 
  be touched. */
  LPC_CAN->CLKDIV = 0x02;			/* Divided by 3 now for 16MHz */
   /* Start configuring bit timing */
  LPC_CAN->CNTL |= CTRL_CCE;
  LPC_CAN->BT = 0x2301;
  LPC_CAN->BRPE = 0x0000;
  /* Stop configuring bit timing */
  LPC_CAN->CNTL &= ~CTRL_CCE;
#else
  /* Be very careful with this setting because it's related to
  the input bitclock setting value in CANBitClk. */
  /* popular CAN clock setting assuming AHB clock is 48Mhz:
  CLKDIV = 1, CAN clock is 48Mhz/2 = 24Mhz
  CLKDIV = 2, CAN clock is 48Mhz/3 = 16Mhz
  CLKDIV = 3, CAN clock is 48Mhz/4 = 12Mhz
  CLKDIV = 5, CAN clock is 48Mhz/6 = 8Mhz */

  /* AHB clock is 48Mhz, the CAN clock is 1/3 AHB clock = 16Mhz */
  LPC_CAN->CLKDIV = 0x02;			/* Divided by 3 */
  
  /* Start configuring bit timing */
  LPC_CAN->CNTL |= CTRL_CCE;
  LPC_CAN->BT = CANBitClk;
  LPC_CAN->BRPE = 0x0000;
  /* Stop configuring bit timing */
  LPC_CAN->CNTL &= ~CTRL_CCE;
#endif

  /* Initialization finishes, normal operation now. */
  LPC_CAN->CNTL &= ~CTRL_INIT;
  while ( LPC_CAN->CNTL & CTRL_INIT );

#if (LOOPBACK_MODE | BASIC_MODE)
  LPC_CAN->CNTL |= CTRL_TEST;
  LPC_CAN->TEST &= ~((0x1<<2)|(0x1<<3)|(0x1<<4));
#if LOOPBACK_MODE
  LPC_CAN->TEST |= (0x1<<4);
#endif
#if BASIC_MODE
  LPC_CAN->TEST |= (0x1<<2);
#endif
#endif  

#if !BASIC_MODE
  /* Below is a critical module to configure all the messages */
  /* It's organized in such a way that:
	    obj(x+1)	standard	receive
	    obj(x+2)	standard	transmit 
	    obj(x+3)	extended	receive
	    obj(x+4)	extended	transmit	where x is 0 to 7
	    obj31 is not used. 
		obj32 is for remote date request test only */
#if CUSTOM_CONFIG  //Defining own buffer customisation
  CAN_CustomConfigureMessages();
#else
  CAN_ConfigureMessages();
#endif
#endif

#if !POLLING
  /* Enable the CAN Interrupt */
  NVIC_EnableIRQ(CAN_IRQn);
	
  /* By default, auto TX is enabled, enable all related interrupts */
  LPC_CAN->CNTL |= (CTRL_IE|CTRL_SIE|CTRL_EIE);
#endif
  return;
}

/*****************************************************************************
** Function name:		CAN_Send
**
** Descriptions:		Send a block of data to the CAN port, the 
**						first parameter is the message number, the 2nd 
**						parameter is the received_flag whether it sends received
**						data as the TX data field, the third is the pointer to 
**						the mgs field. When receive_flag is TRUE, the TX msg# is 
**						RX msg#+1, the TX msg ID is twice of the RX msg ID,
**						when received_flag is FALSE, no change in msg#.
**
** parameters:			msg #, received_flag, msg buffer pointer
** Returned value:		None
** 
*****************************************************************************/
void CAN_Send( uint8_t msg_no, uint8_t received_flag, uint32_t *msg_ptr )
{
  uint32_t msg_id, tx_id, Length;   
  
  if ( msg_ptr == NULL )
  {
	while( 1 );
  }

  /* first is the ID, second is length, the next four are data */
  msg_id = *msg_ptr++;
  if ( received_flag )
  {
	/* Mask off MsgVal, Xtd, and Dir, then make the TX msg ID twice the RX msg ID. */
	/* the msg id should be no large than half the max. extended ID allowed. */
	tx_id = (msg_id & 0x1FFFFFFF) << 1;
  }
  else
  {
	tx_id = msg_id;	
  }
  Length = *msg_ptr++;

  if ( Length > DLC_MAX )
  {
	Length = DLC_MAX;	
  }

  if ( !(msg_id & (0x1<<30)) )		/* bit 30 is 0, standard frame */
  {
	/* MsgVal: 1, Mtd: 0, Dir: 1, ID = 0x200 */
	LPC_CAN->IF1_ARB2 = ID_MVAL | ID_DIR | (tx_id << 2);
	LPC_CAN->IF1_ARB1 = 0x0000;

	/* Mxtd: 0, Mdir: 1, Mask is 0x7FF */
	LPC_CAN->IF1_MSK2 = MASK_MDIR | (ID_STD_MASK << 2);
	LPC_CAN->IF1_MSK1 = 0x0000;
  }
  else //If an extended ID, sets the tx_id and the Length
  {
	/* MsgVal: 1, Mtd: 1, Dir: 1, ID = 0x200000 */
	LPC_CAN->IF1_ARB2 = ID_MVAL | ID_MTD | ID_DIR | (tx_id >> 16);
	LPC_CAN->IF1_ARB1 = tx_id & 0x0000FFFF;

	/* Mxtd: 1, Mdir: 1, Mask is 0x7FF */
	LPC_CAN->IF1_MSK2 = MASK_MXTD | MASK_MDIR | (ID_EXT_MASK >> 16);
	LPC_CAN->IF1_MSK1 = ID_EXT_MASK & 0x0000FFFF;
  }
  LPC_CAN->IF1_MCTRL = UMSK|TXRQ|EOB|(Length & DLC_MASK);


#if REORDER_DATA
  uint32_t msghold;
  uint32_t msgleftshift;
  uint32_t msgrightshift;

	  msghold=*msg_ptr++; //Stores the next two bytes in msghold
	  msgleftshift = (msghold & 0x000000FF)<<8; //Shifts the last byte left by 8 bits
	  msgrightshift = (msghold & 0x0000FF00)>>8; //Shifts the second last byte right by 8 bits
  LPC_CAN->IF1_DA1 = msgleftshift | msgrightshift; //OR's the two shifted results to give the data in the right order

//Comment out from here to remove the rest
	  msghold=*msg_ptr++;
	  msgleftshift = (msghold & 0x000000FF)<<8; //Last byte, to be shifted left by 8
	  msgrightshift = (msghold & 0x0000FF00)>>8;
  LPC_CAN->IF1_DA2 = msgleftshift | msgrightshift;

  	  msghold=*msg_ptr++;
	  msgleftshift = (msghold & 0x000000FF)<<8; //Last byte, to be shifted left by 8
	  msgrightshift = (msghold & 0x0000FF00)>>8;
  LPC_CAN->IF1_DB1 = msgleftshift | msgrightshift;

  	  msghold=*msg_ptr;
	  msgleftshift = (msghold & 0x000000FF)<<8; //Last byte, to be shifted left by 8
	  msgrightshift = (msghold & 0x0000FF00)>>8;
  LPC_CAN->IF1_DB2 = msgleftshift | msgrightshift;

#else
  LPC_CAN->IF1_DA1 = *msg_ptr++;
  LPC_CAN->IF1_DA2 = *msg_ptr++;
  LPC_CAN->IF1_DB1 = *msg_ptr++;
  LPC_CAN->IF1_DB2 = *msg_ptr;
#endif

  LPC_CAN->IF1_CMDMSK = WR|MASK|ARB|CTRL|TREQ|DATAA|DATAB;
#if BASIC_MODE
  LPC_CAN->IF1_CMDREQ = IFCREQ_BUSY;
#else
  /* if receive_ flag is FALSE, the TX uses the normal message object (msg_no+1)
  for transmit. When received_flag is TRUE, msg_no+1 is the message object for RX,
  TX uses the message object next to RX(msg_no+2) for transmit. */ 
  if ( received_flag )
  {
	LPC_CAN->IF1_CMDREQ = msg_no+2;
  }
  else
  {
	LPC_CAN->IF1_CMDREQ = msg_no+1;
  }
  /*int i; //Removes LED Blinking
	for(i=0; i < 100000; i++)
		GPIOSetValue(2,8,0); //Yel LED, Low (on)
	for(i=0; i < 100000; i++)
		GPIOSetValue(2,8,1); //Yel LED, Low (off)*/

  while( LPC_CAN->IF1_CMDREQ & IFCREQ_BUSY ) {
	  ;
  }/* Could spin here forever */
#endif
  return;
}

void SCANDAL_Send(uint16_t Pri, uint16_t MsgType, uint16_t NodAddr, uint16_t Channel_NodeType, int32_t data){
	uint32_t tx_addr = 0x1FFFFFFF & SCANDAL_AddrGen(Pri, MsgType, NodAddr, Channel_NodeType);
	uint8_t length = 8;
	uint32_t Data1, Data2;
	uint8_t BufferPos; //Buffer Position (Counter variable to see if the buffer position is BUSY
	uint8_t BufferOffset=21; //The offset from which the buffer will start counting (Assumed 21 through 32)
	uint8_t BufferStat; //Status of the buffer being checked, 1 indicates busy
	uint32_t Buffers;

	FlipValues(data, &Data1, &Data2);

	/* MsgVal: 1, Mtd: 1, Dir: 1, ID = 0x200000 */
	LPC_CAN->IF1_ARB2 = ID_MVAL | ID_MTD | ID_DIR | (tx_addr >> 16);
	LPC_CAN->IF1_ARB1 = tx_addr & 0x0000FFFF;

	/* Mxtd: 1, Mdir: 1, Mask is 0x7FF */
	LPC_CAN->IF1_MSK2 = MASK_MXTD | MASK_MDIR | (ID_EXT_MASK >> 16);
	LPC_CAN->IF1_MSK1 = ID_EXT_MASK & 0x0000FFFF;

	LPC_CAN->IF1_MCTRL = UMSK|TXRQ|EOB|(length & DLC_MASK);

	LPC_CAN->IF1_DA1 = Data1;
	LPC_CAN->IF1_DA2 = Data2;

	//timer32_0_counter
	FlipValues(timer32_0_counter, &Data1, &Data2);

	LPC_CAN->IF1_DB1 = Data1;
	LPC_CAN->IF1_DB2 = Data2;
	LPC_CAN->IF1_CMDMSK = WR|MASK|ARB|CTRL|TREQ|DATAA|DATAB;


	//Buffers=(((LPC_CAN->TXREQ2) & (0x0000FFFF)) << 16) | ((LPC_CAN->TXREQ1) & (0x0000FFFF));
	BufferStat = BufferCheck(21);

	BufferPos=0;
	BufferFree=BufferCheck(BufferPos+BufferOffset);


	//BufferFree = (Buffer >> (20-1));

	//Spins here while BufferFree=1 (Current buffer is busy)
	while(BufferFree){
		BufferPos++;
		//BufferPos=BufferPos%13;

		if(BufferPos<12){ //Buffer Position should either be
			BufferFree=BufferCheck(BufferPos+BufferOffset);
		}else{
			BufferFree=1; //Force exit of the loop
		}


		//BufferFree=BufferCheck(BufferPos+BufferOffset);
	}

	if((BufferPos+BufferOffset)<=MSG_OBJ_MAX){ //Make sure we haven't gone beyond the maximum buffer size we're allowed to use.
		LPC_CAN->IF1_CMDREQ = (BufferPos+BufferOffset);
	}else{
		//Return some sort of error as all the buffers were full, maybe test by having an LED initially OFF
		//and turning it on here
	}
	while( LPC_CAN->IF1_CMDREQ & IFCREQ_BUSY ) {
	  ;/* Waits untill IF1 transfer thingy has done its duties */
	}

	//SCANDALClockOffset

	//Swap around the bits in data to suit the buffers properly
	//Copy the can_send stuff in and modify to be scandal specific



}

uint32_t SCANDAL_AddrGen(uint16_t Pri, uint16_t MsgType, uint16_t NodAddr, uint16_t NodTyp){

	return (Pri << 26) | ( MsgType << 18) | (NodAddr << 10) | NodTyp;
	//(Ext << 30) | Will be re-written such that this isn't required
}

//Returns true of the buffer is BUSY so move on to the next buffer
uint8_t BufferCheck(uint8_t ToCheck){
	uint32_t BufferStatus;
	BufferStatus=(((LPC_CAN->TXREQ2) & (0x0000FFFF)) << 16) | ((LPC_CAN->TXREQ1) & (0x0000FFFF));

	//Return value to be 1 or 0 depending on if the buffer is free or not
	uint8_t ret = (Buffers >> (ToCheck -1) & 0x01);

	return ret;

}


/*****************************************************************************
** Function name:		FlipValues
**
** Descriptions:		The CAN data registers on the LPC11C15 are arranged
** 						in a very nonsensival format, ie - (10 32 54 76)
** 						where each number represents a byte (8 Bits or 2 Hex).
** 						This function flips the values into (01 23) and (45 67)
** 						and these are returned via the two pointers
**
** 						TODO: ponder about doing the converse in one function too
**
** parameters:			Input data (1x32 bits), two pointers to where the flipped data will be placed (2x16 bits)
** Returned value:		None
**
*****************************************************************************/
void FlipValues(uint32_t Input, uint32_t *D1, uint32_t *D2){
	uint32_t Set1;
	uint32_t Set2;
//	uint8_t LeftShift;
//	uint8_t RightShift;

	Set1 = (Input >> 16) & 0x0000FFFF;
	*D1 = (((Set1 & 0x000000FF) << 8) | ((Set1 & 0x0000FF00) >> 8));
	Set2 = Input & 0x0000FFFF;
	*D2 = (((Set2 & 0x000000FF) << 8) | ((Set2 & 0x0000FF00) >> 8));

}
// #if CONFIG_ENABLE_DRIVER_CAN==1
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/

