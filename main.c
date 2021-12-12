/*****************************************************************************
* main.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  November 1,2020
*****************************************************************************/

#include "qpn_port.h"                                       /* QP-nano port */
#include "bsp.h"                             /* Board Support Package (BSP) */
#include "lab2a.h"                               /* application interface */
#include "xil_cache.h"		                /* Cache Drivers */
#include "lcd.h"
#include "xspi.h"
#include "xspi_l.h"
#include "xgpio.h"
#include <xbasic_types.h>
#include <xio.h>
#include <xil_cache.h>
#include "xparameters.h"

int volume = 0;
XGpio Gpio_leds;

static QEvent l_lab2aQueue[30];  

QActiveCB const Q_ROM Q_ROM_VAR QF_active[] = {
	{ (QActive *)0,            (QEvent *)0,          0                    },
	{ (QActive *)&AO_Lab2A,    l_lab2aQueue,         Q_DIM(l_lab2aQueue)  }
};

Q_ASSERT_COMPILE(QF_MAX_ACTIVE == Q_DIM(QF_active) - 1);

// Do not edit main, unless you have a really good reason
int main(void) {

	Xil_ICacheInvalidate();
	Xil_ICacheEnable();
	Xil_DCacheInvalidate();
	Xil_DCacheEnable();

	static XGpio dc;
	static XSpi spi;
	static XSpi spi2;

/**************************************
	 DEFINING SPI INTERFACE-1
***************************************/

	XSpi_Config *spiConfig;

	u32 Status;
	Status = XGpio_Initialize(&dc, XPAR_SPI_DC_DEVICE_ID);

	u32 controlReg;

	if (Status != XST_SUCCESS)  {
		xil_printf("Initialize GPIO dc fail!\n");
	}

	/*
	 * Set the direction for all signals to be outputs
	 */
	XGpio_SetDataDirection(&dc, 1, 0x0);

	/*
	 * Initialize the SPI driver so that it is  ready to use.
	 */
	spiConfig = XSpi_LookupConfig(XPAR_SPI_DEVICE_ID);
	if (spiConfig == NULL) {
		xil_printf("Can't find spi device!\n");
	}

	Status = XSpi_CfgInitialize(&spi, spiConfig, spiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialize spi fail!\n");
	}

	/*
	 * Reset the SPI device to leave it in a known good state.
	 */
	XSpi_Reset(&spi);

	/*
	 * Setup the control register to enable master mode
	 */
	controlReg = XSpi_GetControlReg(&spi);
	XSpi_SetControlReg(&spi,
			(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
			(~XSP_CR_TRANS_INHIBIT_MASK));

	// Select 1st slave device
	XSpi_SetSlaveSelectReg(&spi, ~0x01);
	initLCD();

	clrScr();

	drawTetris();


	/**************************************
		 DEFINING SPI INTERFACE-2
	***************************************/


	Status = XGpio_Initialize(&dc, XPAR_SPI_DC_2_DEVICE_ID);

	if (Status != XST_SUCCESS)  {
		xil_printf("Initialize GPIO dc fail!\n");
	}

	/*
	 * Set the direction for all signals to be outputs
	 */
	XGpio_SetDataDirection(&dc, 1, 0x0);

	/*
	 * Initialize the SPI driver so that it is  ready to use.
	 */
	spiConfig = XSpi_LookupConfig(XPAR_SPI_2_DEVICE_ID);
	if (spiConfig == NULL) {
		xil_printf("Can't find spi device!\n");
	}

	Status = XSpi_CfgInitialize(&spi2, spiConfig, spiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Initialize spi fail!\n");
	}

	/*
	 * Reset the SPI device to leave it in a known good state.
	 */
	XSpi_Reset(&spi2);

	/*
	 * Setup the control register to enable master mode
	 */
	controlReg = XSpi_GetControlReg(&spi2);
	XSpi_SetControlReg(&spi2,
			(controlReg | XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK) &
			(~XSP_CR_TRANS_INHIBIT_MASK));

	// Select 1st slave device
	XSpi_SetSlaveSelectReg(&spi2, ~0x01);
	initLCD2();

	clrScr2();

	//drawBackground2();


	//setColor(0, 0, 255);


	//setColor(0, 255, 255);
	//setColorBg(0, 0, 255);
	//lcdPrint("Hello !!!!!", 40, 60);
	XGpio_Initialize(&Gpio_leds, XPAR_AXI_GPIO_LED_DEVICE_ID);
	//XGpio_DiscreteWrite(&Gpio_leds, 1, 1);


	//setColor(255, 165, 0);
	//fillRect(39, 89, 201, 111);

	Lab2A_ctor(); // inside of lab2a.c

	BSP_init(); // inside of bsp.c, starts out empty!
	//xil_printf("Here\n");
	QF_run(); // inside of qfn.c
	//xil_printf("Here\n");



	while(1){

		//sleep(1);
		//xil_printf("volume:%d\r\n",volume);
	}
	return 0;
}
