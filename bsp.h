/*****************************************************************************
* bsp.h for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/
#ifndef bsp_h
#define bsp_h


                                              


/* bsp functions ..........................................................*/

void BSP_init(void);
void ISR_gpio(void);
void ISR_timer(void);

void encoder_handler();
void btn_handler();
void extra_handler();
void extra_disable();
void extra_enable();

#define BSP_showState(prio_, state_) ((void)0)
#define INTC_DEVICE_ID                 XPAR_INTC_0_DEVICE_ID


#endif                                                             /* bsp_h */


