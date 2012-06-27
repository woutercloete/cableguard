/**
 * \file    w5300.c
 * Implementation of W5300 I/O fucntions
 *
 * This file implements the basic I/O fucntions that access a register of W5300( IINCHIP_REG).
 * 
 * Revision History :
 * ----------  -------  -----------  ----------------------------
 * date        Version  Author       Description
 * ----------  -------  -----------  ----------------------------
 * 24/03/2008  1.0.0    MidnightCow  Release with W5300 launching
 * ----------  -------  -----------  ----------------------------
 * 01/05/2008  1.0.1    MidnightCow  Modify a logical error in iinchip_irq(). Refer to M_01052008
 * ----------  -------  -----------  ----------------------------
 * 15/05/2008  1.1.0    MidnightCow  Refer to M_15052008
 *                                   Delete getSn_DPORTR() because \ref Sn_DPORTR is write-only. 
 *                                   Replace 'Sn_DHAR2' with 'Sn_DIPR' in \ref getSn_DIPR().
 * ----------  -------  -----------  ----------------------------
 */

#include <stdio.h>
#include <avr/interrupt.h>

#include "md5.h"
#include "w5300.h"
#include "types.h"

/** 
 * TX memory size variables
 */
uint32 TXMEM_SIZE[MAX_SOCK_NUM];

/** 
 * RX memory size variables
 */
uint32 RXMEM_SIZE[MAX_SOCK_NUM];

/** 
 * The variables for SOCKETn interrupt
 */
uint8 SOCK_INT[MAX_SOCK_NUM];


/***********************
 * Basic I/O  Function *
 ***********************/
uint8 IINCHIP_READ(uint16 addr)
{
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)
	 return ({*((vuint8*)(addr));});
#else
   uint8 data;
      *((vuint8*)IDM_AR) = addr >> 8;
      *((vuint8*)IDM_AR1) = addr;
	  if(addr &0x01) data = *((vuint8*)IDM_DR1);
	  else data = *((vuint8*)IDM_DR);

   return data;
#endif
}

void IINCHIP_WRITE(uint16 addr,uint8 data)
{
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__) 
	 *((vuint8*)(addr)) = data;
#else
    *((vuint8*)IDM_AR) = addr >> 8;
	  *((vuint8*)IDM_AR1) = addr;
	  if(addr &0x01) *((vuint8*)IDM_DR1) = data;
	  else *((vuint8*)IDM_DR) = data;
#endif
}



uint16   getMR(void)
{
   return (IINCHIP_READ(MR_) << 8 | IINCHIP_READ(MR));
}
void     setMR(uint16 val)
{
   *((volatile uint8*)MR_) = val >> 8;
   *((volatile uint8*)MR) = val & 0xff;
}


/***********************************
 * COMMON Register Access Function *
 ***********************************/

/* Interrupt */ 

uint16 getIR(void)
{
   return (IINCHIP_READ(IR0) << 8 | IINCHIP_READ(IR1)); 
}

void setIR(uint8 s, uint16 val)
{
   IINCHIP_WRITE(IR,val&0xFF00);
}

uint16   getIMR(void)
{
   return ((IINCHIP_READ(IMR0) << 8) | IINCHIP_READ(IMR1));
}
void     setIMR(uint16 mask)
{
	IINCHIP_WRITE(IMR0, mask >> 8); 
	IINCHIP_WRITE(IMR1, mask & 0xff); 
}


/* Network Information */

void getSHAR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SHAR);
	addr[1] = IINCHIP_READ(SHAR1);
	addr[2] = IINCHIP_READ(SHAR2);
	addr[3] = IINCHIP_READ(SHAR3);
	addr[4] = IINCHIP_READ(SHAR4);
	addr[5] = IINCHIP_READ(SHAR5);
}
void setSHAR(uint8 * addr)
{
	uint8 k;

	for(k = 0; k < 6; k++) {
		IINCHIP_WRITE(SHAR+k, addr[k]);
	}
}
void getGAR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(GAR);
	addr[1] = IINCHIP_READ(GAR1);
	addr[2] = IINCHIP_READ(GAR2);
	addr[3] = IINCHIP_READ(GAR3);
}
void setGAR(uint8 * addr)
{
	IINCHIP_WRITE((GAR),addr[0]);
	IINCHIP_WRITE((GAR1),addr[1]);
	IINCHIP_WRITE((GAR2),addr[2]);
	IINCHIP_WRITE((GAR3),addr[3]);
}

void getSUBR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SUBR);
	addr[1] = IINCHIP_READ(SUBR1);
	addr[2] = IINCHIP_READ(SUBR2);
	addr[3] = IINCHIP_READ(SUBR3);
}
void setSUBR(uint8 * addr)
{
	IINCHIP_WRITE((SUBR),addr[0]);
	IINCHIP_WRITE((SUBR1),addr[1]);
	IINCHIP_WRITE((SUBR2),addr[2]);
	IINCHIP_WRITE((SUBR3),addr[3]);
}

void getSIPR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SIPR);
	addr[1] = IINCHIP_READ(SIPR1);
	addr[2] = IINCHIP_READ(SIPR2);
	addr[3] = IINCHIP_READ(SIPR3);
}
void setSIPR(uint8 * addr)
{
	IINCHIP_WRITE((SIPR),addr[0]);
	IINCHIP_WRITE((SIPR1),addr[1]);
	IINCHIP_WRITE((SIPR2),addr[2]);
	IINCHIP_WRITE((SIPR3),addr[3]);
}


/* Retransmittion */

uint16   getRTR(void)
{
   return ((IINCHIP_READ(RTR0) << 8) | IINCHIP_READ(RTR1));
}
void     setRTR(uint16 timeout)
{
	IINCHIP_WRITE(RTR0, timeout >> 8);
	IINCHIP_WRITE(RTR1, timeout);
}

uint8    getRCR(void)
{
   return (uint8)IINCHIP_READ(RCR);
}
void     setRCR(uint8 retry)
{
   IINCHIP_WRITE(RCR,retry);
}

/* PPPoE */
uint16   getPATR(void)
{
   return ((IINCHIP_READ(PATR0) << 8) | IINCHIP_READ(PATR1));
}

uint8    getPTIMER(void)
{
   return (uint8)IINCHIP_READ(PTIMER);
}
void     setPTIMER(uint8 time)
{
   IINCHIP_WRITE(PTIMER,time);
}

uint8    getPMAGICR(void)
{
   return (uint8)IINCHIP_READ(PMAGICR);
}
void     setPMAGICR(uint8 magic)
{
   IINCHIP_WRITE(PMAGICR,magic);
}

uint16   getPSIDR(void)
{
   return ((IINCHIP_READ(PSIDR0) << 8) | IINCHIP_READ(PSIDR1));
}

void     getPDHAR(uint8* addr)
{
   addr[0] = (uint8)(IINCHIP_READ(PDHAR) >> 8);
   addr[1] = (uint8)IINCHIP_READ(PDHAR);
   addr[2] = (uint8)(IINCHIP_READ(PDHAR2) >> 8);
   addr[3] = (uint8)IINCHIP_READ(PDHAR2);
   addr[4] = (uint8)(IINCHIP_READ(PDHAR4) >> 8);
   addr[5] = (uint8)IINCHIP_READ(PDHAR4);
}


/* ICMP packets */

void     getUIPR(uint8* addr)
{
   addr[0] = (uint8)(IINCHIP_READ(UIPR) >> 8);
   addr[1] = (uint8)IINCHIP_READ(UIPR);
   addr[2] = (uint8)(IINCHIP_READ(UIPR2) >> 8);
   addr[3] = (uint8)IINCHIP_READ(UIPR2);   
}

uint16   getUPORTR(void)
{
   return ((IINCHIP_READ(UPORTR0) << 8) | IINCHIP_READ(UPORTR1));
}

uint16   getFMTUR(void)
{
   return ((IINCHIP_READ(FMTUR0) << 8) | IINCHIP_READ(FMTUR1));
}


/* PIN "BRYDn" */

uint8    getPn_BRDYR(uint8 p)
{
   return (uint8)IINCHIP_READ(Pn_BRDYR(p));
}
void     setPn_BRDYR(uint8 p, uint8 cfg)
{
   IINCHIP_WRITE(Pn_BRDYR(p),cfg);   
}


uint16   getPn_BDPTHR(uint8 p)
{
   return ((IINCHIP_READ(Pn_BDPTHR0(p) << 8)) | IINCHIP_READ(Pn_BDPTHR1(p)));   
}
void     setPn_BDPTHR(uint8 p, uint16 depth)
{
   IINCHIP_WRITE(Pn_BDPTHR0(p),depth >> 8);
   IINCHIP_WRITE(Pn_BDPTHR1(p),depth);
}


/* IINCHIP ID */
uint16   getIDR(void)
{
   return ((IINCHIP_READ(IDR) << 8) | IINCHIP_READ(IDR1));
}


/***********************************
 * SOCKET Register Access Function *
 ***********************************/

/* SOCKET control */

uint16   getSn_MR(SOCKET s)
{
   return ((IINCHIP_READ(Sn_MR_(s) << 8) | IINCHIP_READ(Sn_MR(s))));
}
void     setSn_MR(SOCKET s, uint8 mode)
{
   IINCHIP_WRITE(Sn_MR(s),mode);
}

uint8    getSn_CR(SOCKET s)
{
   return IINCHIP_READ(Sn_CR(s));
}
void     setSn_CR(SOCKET s, uint8 com)
{
   IINCHIP_WRITE(Sn_CR(s),com);
   while(IINCHIP_READ(Sn_CR(s))); // wait until Sn_CR is cleared.
}

uint8    getSn_IMR(SOCKET s)
{
   return (uint8)IINCHIP_READ(Sn_IMR(s));
}
void     setSn_IMR(SOCKET s, uint8 mask)
{
   IINCHIP_WRITE(Sn_IMR(s),mask);
}

uint8    getSn_IR(SOCKET s)
{
   #ifdef __DEF_IINCHIP_INT__    // In case of using ISR routine of iinchip
      return (uint8)IINCHIP_READ(Sn_IR(s));
   #else                         // In case of processing directly
      return (uint8)IINCHIP_READ(Sn_IR(s));
   #endif   
}
void     setSn_IR(SOCKET s, uint8 ir)
{
   #ifdef __DEF_IINCHIP_INT__    // In case of using ISR routine of iinchip
      IINCHIP_WRITE(Sn_IR(s),ir);
   #else                         // In case of processing directly
      IINCHIP_WRITE(Sn_IR(s),ir);
   #endif   
}


/* SOCKET information */

uint8    getSn_SSR(SOCKET s)
{
   uint8 ssr, ssr1;
   ssr = (uint8)IINCHIP_READ(Sn_SSR(s));     // first read

   while(1)
   {
      ssr1 = (uint8)IINCHIP_READ(Sn_SSR(s)); // second read
      if(ssr == ssr1) break;                 // if first == sencond, Sn_SSR value is valid.
      ssr = ssr1;                            // if first <> second, save second value into first.
   }
   return ssr;
}

void     getSn_DHAR(SOCKET s, uint8* addr)
{
   addr[0] = (uint8)(IINCHIP_READ(Sn_DHAR(s))>>8);
   addr[1] = (uint8)IINCHIP_READ(Sn_DHAR(s));
   addr[2] = (uint8)(IINCHIP_READ(Sn_DHAR2(s))>>8);
   addr[3] = (uint8)IINCHIP_READ(Sn_DHAR2(s));
   addr[4] = (uint8)(IINCHIP_READ(Sn_DHAR4(s))>>8);
   addr[5] = (uint8)IINCHIP_READ(Sn_DHAR4(s));
}

void     setSn_DHAR(SOCKET s, uint8* addr)
{
   IINCHIP_WRITE(Sn_DHAR(s),  ((uint16)(addr[0]<<8)) + addr[1]);
   IINCHIP_WRITE(Sn_DHAR2(s), ((uint16)(addr[2]<<8)) + addr[3]);
   IINCHIP_WRITE(Sn_DHAR4(s), ((uint16)(addr[4]<<8)) + addr[5]);
}

// M_15052008 : Delete this function
//uint16   getSn_DPORTR(SOCKET s)
//{
//   return IINCHIP_READ(Sn_DPORTR(s));
//}


void     setSn_DPORTR(SOCKET s, uint16 port)
{
 	 IINCHIP_WRITE(Sn_DPORTR0(s), port >> 8);
	 IINCHIP_WRITE(Sn_DPORTR1(s), port);
}

void     getSn_DIPR(SOCKET s, uint8* addr)
{
   addr[0] = (uint8)(IINCHIP_READ(Sn_DIPR(s))>>8);
   addr[1] = (uint8)IINCHIP_READ(Sn_DIPR(s));
   addr[2] = (uint8)(IINCHIP_READ(Sn_DIPR2(s))>>8);
// M_15052008 : Replace Sn_DHAR2 with Sn_DIPR.   
// addr[3] = (uint8)IINCHIP_READ(Sn_DHAR2(s));   
   addr[3] = (uint8)IINCHIP_READ(Sn_DIPR2(s));   
}
void     setSn_DIPR(SOCKET s, uint8* addr)
{
   IINCHIP_WRITE(Sn_DIPR(s),  ((uint16)(addr[0]<<8)) + addr[1]);
   IINCHIP_WRITE(Sn_DIPR2(s), ((uint16)(addr[2]<<8)) + addr[3]);  
}

uint16   getSn_MSSR(SOCKET s)
{
	 uint16 ret = 0;
	 ret = IINCHIP_READ(Sn_MSSR0(s)) << 8;
	 ret |= IINCHIP_READ(Sn_MSSR1(s));
   return ret;
}

void     setSn_MSSR(SOCKET s, uint16 mss)
{
   IINCHIP_WRITE(Sn_MSSR0(s), mss >> 8);
   IINCHIP_WRITE(Sn_MSSR1(s), mss);
}


/* SOCKET communication */

uint8    getSn_KPALVTR(SOCKET s)
{
   return (uint8)(IINCHIP_READ(Sn_KPALVTR(s)) >> 8);
}

void     setSn_KPALVTR(SOCKET s, uint8 time)
{
   uint16 keepalive=0;
   keepalive = (IINCHIP_READ(Sn_KPALVTR(s)) & 0x00FF) + ((uint16)time<<8);
   IINCHIP_WRITE(Sn_KPALVTR(s),keepalive);
}

uint32   getSn_TX_WRSR(SOCKET s)
{
	union {
		uint32 s32;
		uint8 s8[4];
	} u32;

	u32.s8[3] = IINCHIP_READ(Sn_TX_WRSR(s));
	u32.s8[2] = IINCHIP_READ(Sn_TX_WRSR1(s));
	u32.s8[1] = IINCHIP_READ(Sn_TX_WRSR2(s));
	u32.s8[0] = IINCHIP_READ(Sn_TX_WRSR3(s));

	return u32.s32;
}
void     setSn_TX_WRSR(SOCKET s, uint32 size)
{
	union {
		uint32 s32;
		uint8 s8[4];
	} u32;

	u32.s32 = size;

	IINCHIP_WRITE(Sn_TX_WRSR(s), u32.s8[3]);
	IINCHIP_WRITE(Sn_TX_WRSR1(s), u32.s8[2]);
	IINCHIP_WRITE(Sn_TX_WRSR2(s), u32.s8[1]);
	IINCHIP_WRITE(Sn_TX_WRSR3(s), u32.s8[0]);
}

uint32   getSn_TX_FSR(SOCKET s)
{
   uint32 free_tx_size=0;
   uint32 free_tx_size1=0;
   while(1)
   {
      free_tx_size = IINCHIP_READ(Sn_TX_FSR(s));                           // read                                       
      free_tx_size = (free_tx_size << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
      free_tx_size = (free_tx_size << 8) + IINCHIP_READ(Sn_TX_FSR2(s));
      free_tx_size = (free_tx_size << 8) + IINCHIP_READ(Sn_TX_FSR3(s));                                                       
      if(free_tx_size == free_tx_size1) break;                             // if first == sencond, Sn_TX_FSR value is valid.                                                          
      free_tx_size1 = free_tx_size;                                        // save second value into firs                                                    
   }                                                                       
   return free_tx_size;                                                    
}                                                                          

uint32   getSn_RX_RSR(SOCKET s)
{
   uint32 received_rx_size=0;
   uint32 received_rx_size1=1;
   while(1)
   {
      received_rx_size = IINCHIP_READ(Sn_RX_RSR(s));
      received_rx_size = (received_rx_size << 8) + IINCHIP_READ(Sn_RX_RSR1(s));  // read    
      received_rx_size = (received_rx_size << 8) + IINCHIP_READ(Sn_RX_RSR2(s));
      received_rx_size = (received_rx_size << 8) + IINCHIP_READ(Sn_RX_RSR3(s));                                  
      if(received_rx_size == received_rx_size1) break;                                                                         
      received_rx_size1 = received_rx_size;                                      // if first == sencond, Sn_RX_RSR value is valid.
   }                                                                             // save second value into firs                
   return received_rx_size;   
}


void     setSn_TX_FIFOR(SOCKET s, uint16 data)
{
   IINCHIP_WRITE(Sn_TX_FIFOR(s), data >> 8);
   IINCHIP_WRITE(Sn_TX_FIFOR1(s), data);
}

uint16   getSn_RX_FIFOR(SOCKET s)
{
	 uint16 ret = 0;
	 ret = IINCHIP_READ(Sn_RX_FIFOR(s)) << 8;
	 ret |= IINCHIP_READ(Sn_RX_FIFOR1(s));
   return ret;
}


/* IP header field */

uint8    getSn_PROTOR(SOCKET s)
{
   return (uint8)IINCHIP_READ(Sn_PROTOR(s));
}
void     setSn_PROTOR(SOCKET s, uint8 pronum)
{
   uint16 protocolnum;
   protocolnum = (IINCHIP_READ(Sn_PROTOR(s)) & 0xFF00) + pronum;
   IINCHIP_WRITE(Sn_PROTOR(s),protocolnum);
}

uint8    getSn_TOSR(SOCKET s)
{
   return (uint8)IINCHIP_READ(Sn_TOSR(s));
}
void     setSn_TOSR(SOCKET s, uint8 tos)
{
   IINCHIP_WRITE(Sn_TOSR(s),tos);
}

uint8    getSn_TTLR(SOCKET s)
{
   return (uint8)IINCHIP_READ(Sn_TTLR(s));
}
void     setSn_TTLR(SOCKET s, uint8 ttl)
{
   IINCHIP_WRITE(Sn_TTLR(s),ttl);
}

uint8    getSn_FRAGR(SOCKET s)
{
   return (uint8)IINCHIP_READ(Sn_FRAGR(s));
}

void     setSn_FRAGR(SOCKET s, uint8 frag)
{
   IINCHIP_WRITE(Sn_FRAGR(s),frag);
}


/*******
 * ETC *
 *******/

/* Initialization & Interrupt request routine */

void     iinchip_init(void)
{
	*((volatile uint8*)MR) = MR_RST;
	wait_1ms(5);				// wait PLL lock
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_INDIRECT_MODE__)
		*((volatile uint8*)MR) = MR_IND;
    #ifndef __DEF_IINCHIP_DBG__	
	      printf("MR value is %04x\r\n",*((volatile uint8*)MR));
    #endif	
#endif
}



#ifdef __DEF_IINCHIP_INT__ 
/**
 * \todo You should do implement your interrupt request routine instead of this function.
 *       If you want to use ISR, this function should be mapped in your ISR handler.
 */

void SIG_INTERRUPT4( void ) __attribute__ ((signal));
void SIG_INTERRUPT4( void )
//ISR(INT4_vect)
{
   uint16 int_val;
   uint16 idx;
   IINCHIP_CRITICAL_SECTION_ENTER();
   //M_01052008 : replaced '==' with '='.
   //while(int_val == IINCHIP_READ(IR))  // process all interrupt 
   printf("ISR routine enter\r\n");
   while((int_val = (IINCHIP_READ(IR0) << 8 | IINCHIP_READ(IR1))))     
   {          
      for(idx = 0 ; idx < MAX_SOCK_NUM ; idx++)
      {
         if (int_val & IR_SnINT(idx))  // check the SOCKETn interrupt
         {
            SOCK_INT[idx] |= (uint8)IINCHIP_READ(Sn_IR(idx)); // Save the interrupt stauts to SOCK_INT[idx]
            IINCHIP_WRITE(Sn_IR(idx),(uint16)SOCK_INT[idx]);  // Clear the interrupt status bit of SOCKETn
         }
      }
      
      if (int_val & (IR_IPCF << 8))    // check the IP conflict interrupt
      {
         printf("IP conflict : %04x\r\n", int_val);
      }
      if (int_val & (IR_DPUR << 8))    // check the unreachable destination interrupt
      {
         printf("INT Port Unreachable : %04x\r\n", int_val);
         printf("UIPR : %d.%d.%d.%d\r\n", (uint8)(IINCHIP_READ(UIPR)>>8),
                                          (uint8)IINCHIP_READ(UIPR),
                                          (uint8)(IINCHIP_READ(UIPR2)>>8),
                                          (uint8)IINCHIP_READ(UIPR2));
         printf("UPORTR : %04x\r\n", IINCHIP_READ(UPORTR));
      }
      IINCHIP_WRITE(IR, int_val & 0xFF00);
   }
   IINCHIP_CRITICAL_SECTION_EXIT();
}
#endif 


/* Internal memory operation */
 
uint8    sysinit(uint8* tx_size, uint8* rx_size)
{
   uint8 k;
   uint16 i;
   uint16 ssum=0,rsum=0;
   uint mem_cfg = 0;
   
   for(i=0; i < MAX_SOCK_NUM; i++)
   {
      if(tx_size[i] > 64)
      {
      #ifdef __DEF_IINCHIP_DBG__
         printf("Illegal Channel(%d) TX Memory Size.\r\n",i);
      #endif
         return 0;
      }
      if(rx_size[i] > 64)
      {
      #ifdef __DEF_IINCHIP_DBG__         
         printf("Illegal Channel(%d) RX Memory Size.\r\n",i);
      #endif
         return 0;
      }
      ssum += (uint16)tx_size[i];
      rsum += (uint16)rx_size[i];
      TXMEM_SIZE[i] = ((uint32)tx_size[i]) << 10;
      RXMEM_SIZE[i] = ((uint32)rx_size[i]) << 10;
   }
   if( (ssum % 8) || ((ssum + rsum) != 128) )
   {
   #ifdef __DEF_IINCHIP_DBG__
      printf("Illegal Memory Allocation\r\n");
   #endif
      return 0;
   }
   
   k = 0;
   for(i = TMSR0; i <= TMSR7; i++) {
      IINCHIP_WRITE(i, tx_size[k]);
   k++;
	 }
   
   k = 0;
   for(i = RMSR0; i <= RMSR7; i++) {
      IINCHIP_WRITE(i, rx_size[k]);
   k++;
   }
   
   for(i=0; i <ssum/8 ; i++)
   {
      mem_cfg <<= 1;
      mem_cfg |= 1;
   }
   
   IINCHIP_WRITE(MTYPER, mem_cfg >> 8);
   IINCHIP_WRITE(MTYPER1, mem_cfg & 0xff);
   
   #ifdef __DEF_IINCHIP_DBG__
      printf("Total TX Memory Size = %dKB\r\n",ssum);
      printf("Total RX Memory Size = %dKB\r\n",rsum);
      printf("Ch : TX SIZE : RECV SIZE\r\n");
      printf("%02d : %07dKB : %07dKB \r\n", 0, (uint8)(IINCHIP_READ(TMSR0)),(uint8)(IINCHIP_READ(RMSR0)));
      printf("%02d : %07dKB : %07dKB \r\n", 1, (uint8)(IINCHIP_READ(TMSR1)),(uint8)(IINCHIP_READ(RMSR1)));
      printf("%02d : %07dKB : %07dKB \r\n", 2, (uint8)(IINCHIP_READ(TMSR2)),(uint8)(IINCHIP_READ(RMSR2)));
      printf("%02d : %07dKB : %07dKB \r\n", 3, (uint8)(IINCHIP_READ(TMSR3)),(uint8)(IINCHIP_READ(RMSR3)));
      printf("%02d : %07dKB : %07dKB \r\n", 4, (uint8)(IINCHIP_READ(TMSR4)),(uint8)(IINCHIP_READ(RMSR4)));
      printf("%02d : %07dKB : %07dKB \r\n", 5, (uint8)(IINCHIP_READ(TMSR5)),(uint8)(IINCHIP_READ(RMSR5)));
      printf("%02d : %07dKB : %07dKB \r\n", 6, (uint8)(IINCHIP_READ(TMSR6)),(uint8)(IINCHIP_READ(RMSR6)));
      printf("%02d : %07dKB : %07dKB \r\n", 7, (uint8)(IINCHIP_READ(TMSR7)),(uint8)(IINCHIP_READ(RMSR7)));
      printf("\r\nMTYPER=%04x\r\n",IINCHIP_READ(MTYPER));
   #endif
   
   return 1;
}

uint32   wiz_write_buf(SOCKET s,uint8* buf,uint32 len)
{
#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
				uint32 z;					
				for(z=0; z<len; z+=2) { 
					*((vuint8*)Sn_TX_FIFOR(s)) = *(buf+z);
					*((vuint8*)Sn_TX_FIFOR1(s)) = *(buf+z+1);
				}							
   #endif
  
#elif (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_INDIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
      uint32 idx=0;
	    *((vuint8*)IDM_AR) = Sn_TX_FIFOR(s) >> 8;
	    *((vuint8*)IDM_AR1) = Sn_TX_FIFOR(s);
     	for (idx = 0; idx < len; idx+=2) {
  			*((vuint8*)IDM_DR) = *(buf+idx);
  			*((vuint8*)IDM_DR1) = *(buf+idx+1);
  		}
   #else
      #error "Undefined __DEF_IINCHIP_BUF_OP__"
   #endif 
#else
   #error "Undefined __DEF_IINCHIP_ADDRESS_MODE__"   
#endif
    return len;   
}

uint32   wiz_read_buf(SOCKET s, uint8* buf,uint32 len)
{

#if (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_DIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
				uint32 z;	
				for(z=0; z<len; z+=2) { 
					*(buf+z) = *((vuint8*)Sn_RX_FIFOR(s));
					*(buf+z+1) = *((vuint8*)Sn_RX_FIFOR1(s));
				}								
   #else
      #error "Undefined __DEF_IINCHIP_BUF_OP__"
   #endif 
#elif (__DEF_IINCHIP_ADDRESS_MODE__ == __DEF_IINCHIP_INDIRECT_MODE__)
   #if (__DEF_IINCHIP_BUF_OP__ == __DEF_C__)
      uint32 idx=0;
    	*((vuint8*)IDM_AR) = Sn_RX_FIFOR(s) >> 8;
	    *((vuint8*)IDM_AR1) = Sn_RX_FIFOR(s);
    	for (idx = 0; idx < len; idx+=2) {
  			*(buf+idx) = *((vuint8*)IDM_DR);
	  		*(buf+idx+1) = *((vuint8*)IDM_DR1);
    	}
   #else
      #error "Undefined __DEF_IINCHIP_BUF_OP__"
   #endif 
#else
   #error "Undefined __DEF_IINCHIP_ADDRESS_MODE__"   
#endif
    return len;
}


uint32   getIINCHIP_TxMAX(SOCKET s)
{
   return TXMEM_SIZE[s];
}

uint32   getIINCHIP_RxMAX(SOCKET s)
{
   return RXMEM_SIZE[s];
}


#ifdef __DEF_IINCHIP_PPP__

/**
 * Max lenght of PPPoE message 
 */
#define PPP_OPTION_BUF_LEN 64 

   /**
    * \fn uint8    pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)
    *  Internal function.
    * \details It is called in  pppinit().\n
    *          It processes 4 phases as following. 
    *          - PPPoE Discovery
    *          - LCP process
    *          - PPPoE authentication
    *          - IPCP process
    *
    * \param id User ID for connecting to a PPPoE server
    * \param idlen Length of \em id
    * \param passwd User password for connecting to a PPPoE server
    * \param passwdlen Length of \em passwd
    * \return 1 - success \n
    *         2 - ID or password error \n
    *         3 - Timeout occurs while processing PPPoE \n
    *         4 - Not support authentication type.
    */
uint8    pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)
{
   uint8 loop_idx = 0;
   uint8 isr = 0;
   uint8 buf[PPP_OPTION_BUF_LEN];
   uint32 len;
   uint8 str[PPP_OPTION_BUF_LEN];
   uint8 str_idx,dst_idx;
   
   /////////////////////////////
   // PHASE1. PPPoE Discovery //
   /////////////////////////////
   // start to connect pppoe connection
   printf("-- PHASE 1. PPPoE Discovery process --");
   printf(" ok\r\n");
   printf("\r\n");
   setSn_CR(0,Sn_CR_PCON);
   wait_10ms(100);
   
   // check if PPPoE discovery phase is success or not
   loop_idx = 0;
   while (!(getSn_IR(0) & Sn_IR_PNEXT)) 
   {
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout before LCP\r\n"); 
         return 3;
      }
      wait_10ms(100);
   }
   setSn_IR(0,Sn_IR_PNEXT);
   
   /////////////////////////
   // PHASE2. LCP process //
   /////////////////////////
   printf("-- PHASE 2. LCP process --");
   {
      // Magic number option
      // option format (type value + length value + data)
      // write magic number value
      buf[0] = 0x05; // type value
      buf[1] = 0x06; // length value
      buf[2] = 0x01; buf[3] = 0x01; buf[4] = 0x01; buf[5]= 0x01; // data
      // for MRU option, 1492 0x05d4  
      // buf[6] = 0x01; buf[7] = 0x04; buf[8] = 0x05; buf[9] = 0xD4;
   }
   wiz_write_buf(0,buf, 0x06);
   setSn_TX_WRSR(0,0x06);
   setSn_CR(0,Sn_CR_PCR); // send LCP request to PPPoE server
   wait_10ms(100);
   
   while (!((isr = getSn_IR(0)) & Sn_IR_PNEXT))
   {
      if (isr & Sn_IR_PRECV)     // Not support option
      {
         len = getSn_RX_RSR(0);
         if ( len > 2 )
         {
            wiz_read_buf(0,str, 2);
            len = ((uint16)str[0] << 8) + str[1];
            wiz_read_buf(0,(str+2), len);
            setSn_CR(0,Sn_CR_RECV);
            // get option length
            len = (uint32)str[4]; len = ((len & 0xff) << 8) + (uint32)str[5];
            len += 2;
            str_idx = 6; dst_idx = 0; // PPP header is 6 byte, so starts at 6.
            do 
            {
               if ((str[str_idx] == 0x01) || (str[str_idx] == 0x02) || (str[str_idx] == 0x03) || (str[str_idx] == 0x05))
               {
                  // skip as length of support option. str_idx+1 is option's length.
                  str_idx += str[str_idx+1];
               }
               else
               {
                  // not support option , REJECT
                  memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
                  dst_idx += str[str_idx+1]; str_idx += str[str_idx+1];
               }
            } while (str_idx != len);
         
            // send LCP REJECT packet
            wiz_write_buf(0,buf, dst_idx);
            setSn_TX_WRSR(0,dst_idx);
            setSn_CR(0,Sn_CR_PCJ);
            setSn_IR(0,Sn_IR_PRECV);
         }
      }
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after LCP\r\n");
         return 3;
      }
      wait_10ms(100);
   }
   setSn_IR(0,Sn_IR_PNEXT);
   printf(" ok\r\n");
   printf("\r\n");

   ///////////////////////////////////
   // PHASE 3. PPPoE Authentication //
   ///////////////////////////////////
   printf("-- PHASE 3. PPPoE Authentication mode --\r\n");
   printf("Authentication protocol : %04x, ", getPATR());
   loop_idx = 0;
   if (getPATR() == 0xC023)         // PAP type
   {
      printf("PAP\r\n"); // in case of adsl normally supports PAP.
      // send authentication data
      // copy (idlen + id + passwdlen + passwd)
      buf[loop_idx] = idlen; loop_idx++;
      memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen); loop_idx += idlen;
      buf[loop_idx] = passwdlen; loop_idx++;
      memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen); loop_idx += passwdlen;
      wiz_write_buf(0,buf, loop_idx);
      setSn_TX_WRSR(0,loop_idx);
      setSn_CR(0,Sn_CR_PCR);
      wait_10ms(100);
   }
   else if (getPATR() == 0xC223)    // CHAP type
   {
      uint8 chal_len;
      md5_ctx context;
      uint8  digest[16];
      
      len = getSn_RX_RSR(0);
      if ( len > 2 )
      {
         wiz_read_buf(0,str,2);
         len = ((uint32)str[0] << 8) + (uint32)str[1];
         wiz_read_buf(0, str, len);
         setSn_CR(0,Sn_CR_RECV);
         #ifdef __DEF_IINCHIP_DBG__
         {
            int16 i;
            printf("recv CHAP\r\n");
            for (i = 0; i < 32; i++) printf ("%02X ", str[i]);
            printf("\r\n");
         }
         #endif
         // str is C2 23 xx CHAL_ID xx xx CHAP_LEN CHAP_DATA
         // index  0  1  2  3       4  5  6        7 ...
         
         memset(buf,0x00,64);
         buf[loop_idx] = str[3]; loop_idx++; // chal_id
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen); loop_idx += passwdlen; //passwd
         chal_len = str[6]; // chal_id
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(str+7), chal_len); loop_idx += chal_len; //challenge
         buf[loop_idx] = 0x80;
         #ifdef __DEF_IINCHIP_DBG__
         {
            int16 i;
            printf("CHAP proc d1\r\n");
            
            for (i = 0; i < 64; i++) printf ("%02X ", buf[i]);
            printf("\r\n");
         }
         #endif
         
         md5_init(&context);
         md5_update(&context, buf, loop_idx);
         md5_final(digest, &context);
         
         #ifdef __DEF_IINCHIP_DBG__
         {
            uint i;
            printf("CHAP proc d1\r\n");
            for (i = 0; i < 16; i++) printf ("%02X", digest[i]);
            printf("\r\n");
         }
         #endif
         loop_idx = 0;
         buf[loop_idx] = 16; loop_idx++; // hash_len
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(digest), 16); loop_idx += 16; // hashed value
         memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen); loop_idx += idlen; // id
         wiz_write_buf(0,buf, loop_idx);
         setSn_TX_WRSR(0,loop_idx);
         setSn_CR(0,Sn_CR_PCR);
         wait_10ms(100);
      }
   }
   else
   {
      printf("Not support\r\n");
      #ifdef __DEF_IINCHIP_DBG__
         printf("Not support PPP Auth type: %.4x\r\n",getPATR());
      #endif
      return 4;
   }
   printf("\r\n");
   
   printf("-- Waiting for PPPoE server's admission --");
   loop_idx = 0;
   while (!((isr = getSn_IR(0)) & Sn_IR_PNEXT))
   {
      if (isr & Sn_IR_PFAIL)
      {
         printf("failed\r\nReinput id, password..\r\n");
         return 2;
      }
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after PAP\r\n");
         return 3;
      }
      wait_10ms(100);
   }
   setSn_IR(0,Sn_IR_PNEXT);
   printf("ok\r\n");
   printf("\r\n");

   ///////////////////////////
   // PHASE 4. IPCP process //
   ///////////////////////////
   printf("-- PHASE 4. IPCP process --");
   // IP Address
   buf[0] = 0x03; buf[1] = 0x06; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x00; buf[5] = 0x00;
   wiz_write_buf(0,buf, 6);
   setSn_TX_WRSR(0,6);
   setSn_CR(0,Sn_CR_PCR);
   wait_10ms(100);
   
   loop_idx = 0;
   while (1)
   {
      if (getSn_IR(0) & Sn_IR_PRECV)
      {
         len = getSn_RX_RSR(0);
         if ( len > 2 )
         {
            wiz_read_buf(0,str,2);
            len = ((uint32)str[0] << 8) + (uint32)str[1];
            wiz_read_buf(0, str, len);
            setSn_CR(0,Sn_CR_RECV);
            str_idx = 6; dst_idx = 0;
            if (str[2] == 0x03) // in case of NAK
            {
               do 
               {
                  if (str[str_idx] == 0x03) // request only ip information
                  {
                     memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
                     dst_idx += str[str_idx+1]; str_idx += str[str_idx+1];
                  }
                  else
                  {
                     // skip byte
                     str_idx += str[str_idx+1];
                  }
                  // for debug
                  //printf("s: %d, d: %d, l: %d", str_idx, dst_idx, len);
               } while (str_idx != len);
               wiz_write_buf(0,buf, dst_idx);
               setSn_TX_WRSR(0,dst_idx);
               setSn_CR(0,Sn_CR_PCR); // send ipcp request
               wait_10ms(100);
               break;
            }
         }
         setSn_IR(0,Sn_IR_PRECV);
      }
      
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after IPCP\r\n");
         return 3;
      }
      wait_10ms(100);
      wiz_write_buf(0, buf, 6);
      setSn_TX_WRSR(0,6);
      setSn_CR(0,Sn_CR_PCR); //ipcp re-request
   }
   
   loop_idx = 0;
   while (!(getSn_IR(0) & Sn_IR_PNEXT))
   {
      printf(".");
      if (loop_idx++ == 10) // timeout
      {
         printf("timeout after IPCP NAK\r\n");
         return 3;
      }
      wait_10ms(100);
      setSn_CR(0,Sn_CR_PCR); // send ipcp request
   }
   setSn_IR(0,Sn_IR_PNEXT);
   printf("ok\r\n");
   printf("\r\n");
   return 1;
   // after this function, the pppoe server's mac address and pppoe session id is saved in PHAR and PSIDR repectly.
}

uint8    pppinit(uint8 *id, uint8 idlen, uint8 *passwd, uint8 passwdlen)
{
	uint8 ret;
	uint8 isr;
	
	// PHASE0. PPPoE setup

	printf("-- PHASE 0. PPPoE setup process --\r\n");
	printf("\r\n");
	setMR(getMR()|MR_PPPoE); 		             // set PPPoE mode and FIFO swap
	setMR(getMR()|MR_FS);						 // If little-endian, set MR_FS. Otherwise, comment.
	isr =  getSn_IR(0);
	setSn_IR(0,(uint16)isr);                     // clear the previous value of Sn_IR(0)   
	
	setPTIMER(200);                              // set LPC request time to 5 seconds
	setPMAGICR(0x01);                            // set the magic number
	setSn_MR(0, Sn_MR_PPPoE);                    // set Sn_MR(0) to PPPoE mode
	setSn_CR(0,Sn_CR_OPEN);                      //open SOCKET0 with PPPoE mode                      
	
	ret = pppinit_in(id, idlen, passwd, passwdlen); // process the PPPoE message

	setSn_CR(0, Sn_CR_CLOSE);                       // close PPPoE SOCKET0

	return ret;   
}
   
void    pppterm(uint8 *mac, uint16 sessionid)
{
   uint8 isr;
   #ifdef __DEF_IINCHIP_DBG__
      printf("pppterm()\r\n");
   #endif
   
   // set PPPoE mode
   setMR(getMR() | MR_PPPoE);                
   
   // set pppoe server's mac address and session id 
   // must be setted these value.
   setSn_DHAR(0, mac);
   setSn_DPORTR(0,sessionid);

   // clear the previous value of Sn_IR(0) 
	isr =  getSn_IR(0);
	setSn_IR(0,(uint16)isr);                  
   
   //open SOCKET0 with PPPoE mode
	setSn_MR(0, Sn_MR_PPPoE);                    
	setSn_CR(0,Sn_CR_OPEN);                      
   while(getSn_SSR(0) != SOCK_PPPoE)            
   
   // close PPPoE connection
   setSn_CR(0,Sn_CR_PDISCON);
   wait_10ms(100);
   // close socket
   setSn_CR(0,Sn_CR_CLOSE);
   
   #ifdef __DEF_IINCHIP_DBG__
      printf("pppterm() end ..\r\n");
   #endif
}
#endif
