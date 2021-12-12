/*****************************************************************************
* bsp.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 27,2019
*****************************************************************************/

/**/
#include "qpn_port.h"
#include "bsp.h"
#include "lab2a.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xgpio.h" 		// LED driver, used for General purpose I/i
#include "xspi.h"
#include "xspi_l.h"
#include "xtmrctr_l.h"
#include "xparameters.h"
#include "xtmrctr.h"
#include <stdlib.h>
#include "lcd.h"

/*****************************/

/* Define all variables and Gpio objects here  */
XIntc sys_intc;
XTmrCtr sys_tmrctr;
Xuint32 data;
static int btn_read = 0;
XGpio btn;


int debounce = 0;
int debounceTimer = 0;



#define GPIO_CHANNEL1 1

void debounceInterrupt(); // Write This function

// Create ONE interrupt controllers XIntc
// Create two static XGpio variables
// Suggest Creating two int's to use for determining the direction of twist

/*..........................................................................*/
void BSP_init(void) {
	//static XIntc intc;
	//static XTmrCtr axiTimer;

	u32 Status;
	Status = XST_SUCCESS;
/* Setup LED's, etc */
/* Setup interrupts and reference to interrupt handler function(s)  */


	/*
	 * Initialize the interrupt controller driver so that it's ready to use.
	 * specify the device ID that was generated in xparameters.h
	 *
	 * Initialize GPIO and connect the interrupt controller to the GPIO.
	 *
	 */

	// Press Knob

	// Twist Knob





	Status = XIntc_Initialize(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);

	if ( Status != XST_SUCCESS )
	{
		if( Status == XST_DEVICE_NOT_FOUND )
		{
			xil_printf("XST_DEVICE_NOT_FOUND...\r\n");
		}
		else
		{
			xil_printf("a different error from XST_DEVICE_NOT_FOUND...\r\n");
		}


		xil_printf("Interrupt controller driver failed to be initialized...\r\n");
	}
	xil_printf("Interrupt controller driver initialized!\r\n");
	/*
	 * Connect the application handler that will be called when an interrupt
	 * for the timer occurs
	 */
	Status = XIntc_Connect(&sys_intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR,
			(XInterruptHandler)extra_handler, &sys_tmrctr);
	if ( Status != XST_SUCCESS )
	{
		xil_printf("Failed to connect the application handlers to the interrupt controller...\r\n");
	}
	xil_printf("Connected to Interrupt Controller!\r\n");

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts.
	 */
	Status = XIntc_Start(&sys_intc, XIN_REAL_MODE);
	if ( Status != XST_SUCCESS )
	{
		xil_printf("Interrupt controller driver failed to start...\r\n");
	}
	xil_printf("Started Interrupt Controller!\r\n");

	// Initialize device drivers and enable external interrupts if applicable---------------------------------------/


	// Initialize timer;

	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	/*
	 * Initialize the timer counter so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	Status = XTmrCtr_Initialize(&sys_tmrctr, XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
	if ( Status != XST_SUCCESS )
	{
		xil_printf("Timer initialization failed...\r\n");
	}
	xil_printf("Initialized Timer!\r\n");



// Initialize push buttons driver;
	Status = XIntc_Connect(&sys_intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR,
				(XInterruptHandler)btn_handler, &btn);
		if ( Status != XST_SUCCESS )
		{
			xil_printf("Failed to connect the application handlers to the btn controller...\r\n");
		}
		xil_printf("Connected to btn Controller!\r\n");

		Status = XIntc_Start(&sys_intc, XIN_REAL_MODE);
			if ( Status != XST_SUCCESS )
			{
				xil_printf("Interrupt btn driver failed to start...\r\n");
			}
			xil_printf("Started btn Controller!\r\n");

	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR);

	Status = XGpio_Initialize(&btn, XPAR_AXI_GPIO_BTN_DEVICE_ID);
	if ( Status != XST_SUCCESS )
	{
		xil_printf("Push buttons initialization failed...\r\n");
	}
	xil_printf("Initialized Gpio push buttons!\r\n");

	// Enable the interrupt from the push buttons;


	// Connect interrupt handler to interrupt controller-------------------------------------------------------/

	// Connect interrupt handler from push buttons;
		Status = XIntc_Connect(&sys_intc,XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR,
							   (XInterruptHandler)btn_handler, &btn);
		if ( Status != XST_SUCCESS )
		{
			xil_printf("Failed to connect the push button interrupt handlers to the interrupt controller...\r\n");
		}
		xil_printf("push button interrupt handler connected to Interrupt Controller!\r\n");






	/*
	 * Enable the interrupt of the timer counter so interrupts will occur
	 * and use auto reload mode such that the timer counter will reload
	 * itself automatically and continue repeatedly, without this option
	 * it would expire once only
	 */
	XTmrCtr_SetOptions(&sys_tmrctr, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
	/*
	 * Set a reset value for the timer counter such that it will expire
	 * eariler than letting it roll over from 0, the reset value is loaded
	 * into the timer counter when it is started
	 */
	XTmrCtr_SetResetValue(&sys_tmrctr, 0, (0xFFFFFFFF-0.2*0x17D7840));		// 0x17D7840 = 25*10^6 clk cycles @ 50MHz = 500ms
	/*
	 * Start the timer counter such that it's incrementing by default,
	 * then wait for it to timeout a number of times
	 */
	XTmrCtr_Start(&sys_tmrctr, 0);
	/*
	 * Register the intc device driver’s handler with the Standalone
	 * software platform’s interrupt table
	 */


	//XGpio_SetDataDirection(&encoder, 1, 0xFFFFFFFF);

	XGpio_InterruptEnable(&btn, 1);
	XGpio_InterruptGlobalEnable(&btn);


	microblaze_register_handler((XInterruptHandler)XIntc_DeviceInterruptHandler,
			(void*)XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
//	microblaze_register_handler((XInterruptHandler)XIntc_DeviceInterruptHandler,
//			(void*)PUSHBUTTON_DEVICE_ID);
	//xil_printf("Registers handled!\r\n");

	/*
	 * Enable interrupts on MicroBlaze
	 */
	microblaze_enable_interrupts();
	xil_printf("Interrupts enabled!\r\n");
	/*
	 * At this point, the system is ready to respond to interrupts from the timer
	 */
		
}
/*..........................................................................*/
void QF_onStartup(void) {                 /* entered with interrupts locked */

/* Enable interrupts */
	xil_printf("\n\rQF_onStartup\n"); // Comment out once you are in your complete program

	// Press Knob
	// Enable interrupt controller
	// Start interupt controller
	// register handler with Microblaze
	// Global enable of interrupt
	// Enable interrupt on the GPIO

	// Twist Knob

	// General
	// Initialize Exceptions
	// Press Knob
	// Register Exception
	// Twist Knob
	// Register Exception
	// General
	// Enable Exception

	// Variables for reading Microblaze registers to debug your interrupts.
//	{
//		u32 axi_ISR =  Xil_In32(intcPress.BaseAddress + XIN_ISR_OFFSET);
//		u32 axi_IPR =  Xil_In32(intcPress.BaseAddress + XIN_IPR_OFFSET);
//		u32 axi_IER =  Xil_In32(intcPress.BaseAddress + XIN_IER_OFFSET);
//		u32 axi_IAR =  Xil_In32(intcPress.BaseAddress + XIN_IAR_OFFSET);
//		u32 axi_SIE =  Xil_In32(intcPress.BaseAddress + XIN_SIE_OFFSET);
//		u32 axi_CIE =  Xil_In32(intcPress.BaseAddress + XIN_CIE_OFFSET);
//		u32 axi_IVR =  Xil_In32(intcPress.BaseAddress + XIN_IVR_OFFSET);
//		u32 axi_MER =  Xil_In32(intcPress.BaseAddress + XIN_MER_OFFSET);
//		u32 axi_IMR =  Xil_In32(intcPress.BaseAddress + XIN_IMR_OFFSET);
//		u32 axi_ILR =  Xil_In32(intcPress.BaseAddress + XIN_ILR_OFFSET) ;
//		u32 axi_IVAR = Xil_In32(intcPress.BaseAddress + XIN_IVAR_OFFSET);
//		u32 gpioTestIER  = Xil_In32(sw_Gpio.BaseAddress + XGPIO_IER_OFFSET);
//		u32 gpioTestISR  = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_ISR_OFFSET ) & 0x00000003; // & 0xMASK
//		u32 gpioTestGIER = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_GIE_OFFSET ) & 0x80000000; // & 0xMASK
//	}
}


void QF_onIdle(void) {        /* entered with interrupts locked */

    QF_INT_UNLOCK();                       /* unlock interrupts */

    {
    	// Write code to increment your interrupt counter here.
    	// QActive_postISR((QActive *)&AO_Lab2A, ENCODER_DOWN); is used to post an event to your FSM



// 			Useful for Debugging, and understanding your Microblaze registers.
//    		u32 axi_ISR =  Xil_In32(intcPress.BaseAddress + XIN_ISR_OFFSET);
//    	    u32 axi_IPR =  Xil_In32(intcPress.BaseAddress + XIN_IPR_OFFSET);
//    	    u32 axi_IER =  Xil_In32(intcPress.BaseAddress + XIN_IER_OFFSET);
//
//    	    u32 axi_IAR =  Xil_In32(intcPress.BaseAddress + XIN_IAR_OFFSET);
//    	    u32 axi_SIE =  Xil_In32(intcPress.BaseAddress + XIN_SIE_OFFSET);
//    	    u32 axi_CIE =  Xil_In32(intcPress.BaseAddress + XIN_CIE_OFFSET);
//    	    u32 axi_IVR =  Xil_In32(intcPress.BaseAddress + XIN_IVR_OFFSET);
//    	    u32 axi_MER =  Xil_In32(intcPress.BaseAddress + XIN_MER_OFFSET);
//    	    u32 axi_IMR =  Xil_In32(intcPress.BaseAddress + XIN_IMR_OFFSET);
//    	    u32 axi_ILR =  Xil_In32(intcPress.BaseAddress + XIN_ILR_OFFSET) ;
//    	    u32 axi_IVAR = Xil_In32(intcPress.BaseAddress + XIN_IVAR_OFFSET);
//
//    	    // Expect to see 0x00000001
//    	    u32 gpioTestIER  = Xil_In32(sw_Gpio.BaseAddress + XGPIO_IER_OFFSET);
//    	    // Expect to see 0x00000001
//    	    u32 gpioTestISR  = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_ISR_OFFSET ) & 0x00000003;
//
//    	    // Expect to see 0x80000000 in GIER
//    		u32 gpioTestGIER = Xil_In32(sw_Gpio.BaseAddress  + XGPIO_GIE_OFFSET ) & 0x80000000;


    }
}

/* Do not touch Q_onAssert */
/*..........................................................................*/
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    (void)file;                                   /* avoid compiler warning */
    (void)line;                                   /* avoid compiler warning */
    QF_INT_LOCK();
    for (;;) {
    }
}

/* Interrupt handler functions here.  Do not forget to include them in lab2a.h!
To post an event from an ISR, use this template:
QActive_postISR((QActive *)&AO_Lab2A, SIGNALHERE);
Where the Signals are defined in lab2a.h  */

/******************************************************************************
*
* This is the interrupt handler routine for the GPIO for this example.
*
******************************************************************************/

void btn_handler () {
	//extra_disable();
	XGpio_InterruptDisable(&btn, 1);
	//xil_printf("I'm in the btn_handler() method\r\n");
	if(debounceTimer - debounce < 2){
		XGpio_InterruptClear(&btn, 1);
		XGpio_InterruptEnable(&btn, 1);
		return;
	}
	btn_read = XGpio_DiscreteRead(&btn, 1);


	setColor(255, 165, 0);
	setFont(BigFont);
	if(btn_read%2){ //up btn
		QActive_postISR((QActive *)&AO_Lab2A, UP);
	}
	if((btn_read >> 1) % 2){ //left btn
		QActive_postISR((QActive *)&AO_Lab2A, LEFT);
	}
	if((btn_read >> 2) % 2){ //right btn
		QActive_postISR((QActive *)&AO_Lab2A, RIGHT);
	}
	if((btn_read >> 3) % 2){	//down  btn
		QActive_postISR((QActive *)&AO_Lab2A, DOWN);
	}
	if((btn_read >> 4) % 2){	//middle  btn
		QActive_postISR((QActive *)&AO_Lab2A, MIDDLE);

	}
	debounce = debounceTimer;
	XGpio_InterruptClear(&btn, 1);		// Clear interrupt;
	XGpio_InterruptEnable(&btn, 1);
	//extra_enable();
}

void extra_handler()
{
	//extra_disable();
	//xil_printf("I'm in the extra_handler() method\r\n");
	// This is the interrupt handler function
	// Do not print inside of this function.
//	btn_read = XGpio_DiscreteRead(&btn, 1);

	Xuint32 ControlStatusReg;
	//Read the new Control/Status Register content.
	ControlStatusReg = XTimerCtr_ReadReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET);
	 //xil_printf("Timer interrupt occurred. Count= %d\r\n", count);
	// XGpio_DiscreteWrite(&led,1,count);
	debounceTimer++;
	QActive_postISR((QActive *)&AO_Lab2A, TICK);

	 //(int)tmptime += 1;
	/*
	 * Acknowledge the interrupt by clearing the interrupt
	 * bit in the timer control status register
	 */
	XTmrCtr_WriteReg(sys_tmrctr.BaseAddress, 0, XTC_TCSR_OFFSET, ControlStatusReg |XTC_CSR_INT_OCCURED_MASK);
	//extra_enable();
}

void extra_disable()
{
	XIntc_Disable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	XIntc_Disable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR);
}

void extra_enable()
{
	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_TIMER_0_INTERRUPT_INTR);
	XIntc_Enable(&sys_intc, XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_BTN_IP2INTC_IRPT_INTR);
}





/*void GpioHandler(void *CallbackRef) {
	// Increment A counter
}

void TwistHandler(void *CallbackRef) {
	//XGpio_DiscreteRead( &twist_Gpio, 1);

}

void debounceTwistInterrupt(){
	// Read both lines here? What is twist[0] and twist[1]?
	// How can you use reading from the two GPIO twist input pins to figure out which way the twist is going?
}

void debounceInterrupt() {
	QActive_postISR((QActive *)&AO_Lab2A, ENCODER_CLICK);
	// XGpio_InterruptClear(&sw_Gpio, GPIO_CHANNEL1); // (Example, need to fill in your own parameters
}*/
