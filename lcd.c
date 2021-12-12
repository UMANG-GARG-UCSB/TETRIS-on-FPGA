/*
 * lcd.c
 *
 *  Created on: Oct 21, 2015
 *      Author: atlantis
 */

/*
  UTFT.cpp - Multi-Platform library support for Color TFT LCD Boards
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  This library is the continuation of my ITDB02_Graph, ITDB02_Graph16
  and RGB_GLCD libraries for Arduino and chipKit. As the number of 
  supported display modules and controllers started to increase I felt 
  it was time to make a single, universal library as it will be much 
  easier to maintain in the future.

  Basic functionality of this library was origianlly based on the 
  demo-code provided by ITead studio (for the ITDB02 modules) and 
  NKC Electronics (for the RGB GLCD module/shield).

  This library supports a number of 8bit, 16bit and serial graphic 
  displays, and will work with both Arduino, chipKit boards and select 
  TI LaunchPads. For a full list of tested display modules and controllers,
  see the document UTFT_Supported_display_modules_&_controllers.pdf.

  When using 8bit and 16bit display modules there are some 
  requirements you must adhere to. These requirements can be found 
  in the document UTFT_Requirements.pdf.
  There are no special requirements when using serial displays.

  You can find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#include "lcd.h"
#include "bsp.h"
#include <stdio.h>

// Global variables
int fch;
int fcl;
int bch;
int bcl;
struct _current_font cfont;


// Read data from LCD controller
// FIXME: not work
u32 LCD_Read(char VL)
{
    u32 retval = 0;
    int index = 0;

    Xil_Out32(SPI_DC, 0x0);
    Xil_Out32(SPI_DTR, VL);
    
    //while (0 == (Xil_In32(SPI_SR) & XSP_SR_TX_EMPTY_MASK));
    while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
    Xil_Out32(SPI_DC, 0x01);

    while (1 == (Xil_In32(SPI_SR) & XSP_SR_RX_EMPTY_MASK));
    xil_printf("SR = %x\n", Xil_In32(SPI_SR));


    while (0 == (Xil_In32(SPI_SR) & XSP_SR_RX_EMPTY_MASK)) {
       retval = (retval << 8) | Xil_In32(SPI_DRR);
       xil_printf("receive %dth byte\n", index++);
    }

    xil_printf("SR = %x\n", Xil_In32(SPI_SR));
    xil_printf("SR = %x\n", Xil_In32(SPI_SR));
    return retval;
}


// Write command to LCD controller
void LCD_Write_COM(char VL)  
{
    Xil_Out32(SPI_DC, 0x0);
    Xil_Out32(SPI_DTR, VL);
    
    while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


void LCD2_Write_COM(char VL)
{
    Xil_Out32(SPI2_DC, 0x0);
    Xil_Out32(SPI2_DTR, VL);

    while (0 == (Xil_In32(SPI2_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI2_IISR, Xil_In32(SPI2_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


// Write 16-bit data to LCD controller
void LCD_Write_DATA16(char VH, char VL)
{

    Xil_Out32(SPI_DC, 0x01);
    Xil_Out32(SPI_DTR, VH);
    Xil_Out32(SPI_DTR, VL);

    while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


void LCD2_Write_DATA16(char VH, char VL)
{
    Xil_Out32(SPI2_DC, 0x01);
    Xil_Out32(SPI2_DTR, VH);
    Xil_Out32(SPI2_DTR, VL);

    while (0 == (Xil_In32(SPI2_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI2_IISR, Xil_In32(SPI2_IISR) | XSP_INTR_TX_EMPTY_MASK);
}

// Write 8-bit data to LCD controller
void LCD_Write_DATA(char VL)
{
    Xil_Out32(SPI_DC, 0x01);
    Xil_Out32(SPI_DTR, VL);

    while (0 == (Xil_In32(SPI_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI_IISR, Xil_In32(SPI_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


void LCD2_Write_DATA(char VL)
{
    Xil_Out32(SPI2_DC, 0x01);
    Xil_Out32(SPI2_DTR, VL);

    while (0 == (Xil_In32(SPI2_IISR) & XSP_INTR_TX_EMPTY_MASK));
    Xil_Out32(SPI2_IISR, Xil_In32(SPI2_IISR) | XSP_INTR_TX_EMPTY_MASK);
}


// Initialize LCD controller
void initLCD(void)
{
    int i;

    // Reset
    LCD_Write_COM(0x01);
    for (i = 0; i < 500000; i++); //Must wait > 5ms


    LCD_Write_COM(0xCB);
    LCD_Write_DATA(0x39);
    LCD_Write_DATA(0x2C);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x34);
    LCD_Write_DATA(0x02);

    LCD_Write_COM(0xCF); 
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0XC1);
    LCD_Write_DATA(0X30);

    LCD_Write_COM(0xE8); 
    LCD_Write_DATA(0x85);
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x78);

    LCD_Write_COM(0xEA); 
    LCD_Write_DATA(0x00);
    LCD_Write_DATA(0x00);
 
    LCD_Write_COM(0xED); 
    LCD_Write_DATA(0x64);
    LCD_Write_DATA(0x03);
    LCD_Write_DATA(0X12);
    LCD_Write_DATA(0X81);

    LCD_Write_COM(0xF7); 
    LCD_Write_DATA(0x20);
  
    LCD_Write_COM(0xC0);   //Power control 
    LCD_Write_DATA(0x23);  //VRH[5:0] 
 
    LCD_Write_COM(0xC1);   //Power control 
    LCD_Write_DATA(0x10);  //SAP[2:0];BT[3:0] 

    LCD_Write_COM(0xC5);   //VCM control 
    LCD_Write_DATA(0x3e);  //Contrast
    LCD_Write_DATA(0x28);
 
    LCD_Write_COM(0xC7);   //VCM control2 
    LCD_Write_DATA(0x86);  //--
 
    LCD_Write_COM(0x36);   // Memory Access Control 
    LCD_Write_DATA(0x48);  

    LCD_Write_COM(0x3A);   
    LCD_Write_DATA(0x55);

    LCD_Write_COM(0xB1);   
    LCD_Write_DATA(0x00); 
    LCD_Write_DATA(0x18);
 
    LCD_Write_COM(0xB6);   // Display Function Control 
    LCD_Write_DATA(0x08);
    LCD_Write_DATA(0x82);
    LCD_Write_DATA(0x27); 

    LCD_Write_COM(0x11);   //Exit Sleep 
    for (i = 0; i < 100000; i++);
                
    LCD_Write_COM(0x29);   //Display on 
    LCD_Write_COM(0x2c); 

    //for (i = 0; i < 100000; i++);

    // Default color and fonts
    fch = 0xFF;
    fcl = 0xFF;
    bch = 0x00;
    bcl = 0x00;
    setFont(SmallFont);
}


void initLCD2(void)
{
    int i;

    // Reset
    LCD2_Write_COM(0x01);
    for (i = 0; i < 500000; i++); //Must wait > 5ms


    LCD2_Write_COM(0xCB);
    LCD2_Write_DATA(0x39);
    LCD2_Write_DATA(0x2C);
    LCD2_Write_DATA(0x00);
    LCD2_Write_DATA(0x34);
    LCD2_Write_DATA(0x02);

    LCD2_Write_COM(0xCF);
    LCD2_Write_DATA(0x00);
    LCD2_Write_DATA(0XC1);
    LCD2_Write_DATA(0X30);

    LCD2_Write_COM(0xE8);
    LCD2_Write_DATA(0x85);
    LCD2_Write_DATA(0x00);
    LCD2_Write_DATA(0x78);

    LCD2_Write_COM(0xEA);
    LCD2_Write_DATA(0x00);
    LCD2_Write_DATA(0x00);

    LCD2_Write_COM(0xED);
    LCD2_Write_DATA(0x64);
    LCD2_Write_DATA(0x03);
    LCD2_Write_DATA(0X12);
    LCD2_Write_DATA(0X81);

    LCD2_Write_COM(0xF7);
    LCD2_Write_DATA(0x20);

    LCD2_Write_COM(0xC0);   //Power control
    LCD2_Write_DATA(0x23);  //VRH[5:0]

    LCD2_Write_COM(0xC1);   //Power control
    LCD2_Write_DATA(0x10);  //SAP[2:0];BT[3:0]

    LCD2_Write_COM(0xC5);   //VCM control
    LCD2_Write_DATA(0x3e);  //Contrast
    LCD2_Write_DATA(0x28);

    LCD2_Write_COM(0xC7);   //VCM control2
    LCD2_Write_DATA(0x86);  //--

    LCD2_Write_COM(0x36);   // Memory Access Control
    LCD2_Write_DATA(0x48);

    LCD2_Write_COM(0x3A);
    LCD2_Write_DATA(0x55);

    LCD2_Write_COM(0xB1);
    LCD2_Write_DATA(0x00);
    LCD2_Write_DATA(0x18);

    LCD2_Write_COM(0xB6);   // Display Function Control
    LCD2_Write_DATA(0x08);
    LCD2_Write_DATA(0x82);
    LCD2_Write_DATA(0x27);

    LCD2_Write_COM(0x11);   //Exit Sleep
    for (i = 0; i < 100000; i++);

    LCD2_Write_COM(0x29);   //Display on
    LCD2_Write_COM(0x2c);

    //for (i = 0; i < 100000; i++);

    // Default color and fonts
    fch = 0xFF;
    fcl = 0xFF;
    bch = 0x00;
    bcl = 0x00;
    setFont(SmallFont);
}

// Set boundary for drawing
void setXY(int x1, int y1, int x2, int y2)
{
    LCD_Write_COM(0x2A); 
    LCD_Write_DATA(x1 >> 8);
    LCD_Write_DATA(x1);
    LCD_Write_DATA(x2 >> 8);
    LCD_Write_DATA(x2);
    LCD_Write_COM(0x2B); 
    LCD_Write_DATA(y1 >> 8);
    LCD_Write_DATA(y1);
    LCD_Write_DATA(y2 >> 8);
    LCD_Write_DATA(y2);
    LCD_Write_COM(0x2C);
}

void set2XY(int x1, int y1, int x2, int y2)
{
    LCD2_Write_COM(0x2A);
    LCD2_Write_DATA(x1 >> 8);
    LCD2_Write_DATA(x1);
    LCD2_Write_DATA(x2 >> 8);
    LCD2_Write_DATA(x2);
    LCD2_Write_COM(0x2B);
    LCD2_Write_DATA(y1 >> 8);
    LCD2_Write_DATA(y1);
    LCD2_Write_DATA(y2 >> 8);
    LCD2_Write_DATA(y2);
    LCD2_Write_COM(0x2C);
}

// Remove boundry
void clrXY(void)
{
    setXY(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
}

void clr2XY(void)
{
    set2XY(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
}


// Set foreground RGB color for next drawing
void setColor(u8 r, u8 g, u8 b)
{
    // 5-bit r, 6-bit g, 5-bit b
    fch = (r & 0x0F8) | g >> 5;
    fcl = (g & 0x1C) << 3 | b >> 3;
}

void setColor2(u8 r, u8 g, u8 b)
{
    // 5-bit r, 6-bit g, 5-bit b
    fch = (r & 0x0F8) | g >> 5;
    fcl = (g & 0x1C) << 3 | b >> 3;
}


// Set background RGB color for next drawing
void setColorBg(u8 r, u8 g, u8 b)
{
    // 5-bit r, 6-bit g, 5-bit b
    bch = (r & 0x0F8) | g >> 5;
    bcl = (g & 0x1C) << 3 | b >> 3;
}

void setColorBg2(u8 r, u8 g, u8 b)
{
    // 5-bit r, 6-bit g, 5-bit b
    bch = (r & 0x0F8) | g >> 5;
    bcl = (g & 0x1C) << 3 | b >> 3;
}


// Clear display
void clrScr(void)
{
    // Black screen
    setColor(0, 0, 0);

    fillRect(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
}

void clrScr2(void)
{
    // Black screen
    setColor2(0, 0, 0);

    fillRect2(0, 0, DISP_X_SIZE, DISP_Y_SIZE);
}


// Draw horizontal line
void drawHLine(int x, int y, int l)
{
    int i;

    if (l < 0) {
        l = -l;
        x -= l;
    }

    setXY(x, y, x + l, y);
    for (i = 0; i < l + 1; i++) {
        LCD_Write_DATA16(fch, fcl);
    }

    clrXY();
}

void drawHLine2(int x, int y, int l)
{
    int i;

    if (l < 0) {
        l = -l;
        x -= l;
    }

    set2XY(x, y, x + l, y);
    for (i = 0; i < l + 1; i++) {
        LCD2_Write_DATA16(fch, fcl);
    }

    clr2XY();
}


// Fill a rectangular 
void fillRect(int x1, int y1, int x2, int y2)
{
    int i;

    if (x1 > x2)
        swap(int, x1, x2);

    if (y1 > y2)
        swap(int, y1, y2);

    setXY(x1, y1, x2, y2);
    for (i = 0; i < (x2 - x1 + 1) * (y2 - y1 + 1); i++) {
        // write your own code here
    	LCD_Write_DATA16(fch, fcl);
    }

   clrXY();
}


// Fill a rectangular
void fillRect2(int x1, int y1, int x2, int y2)
{
    int i;

    if (x1 > x2)
        swap(int, x1, x2);

    if (y1 > y2)
        swap(int, y1, y2);

    set2XY(x1, y1, x2, y2);
    for (i = 0; i < (x2 - x1 + 1) * (y2 - y1 + 1); i++) {
        // write your own code here
    	LCD2_Write_DATA16(fch, fcl);
    }

   clr2XY();
}


// Select the font used by print() and printChar()
void setFont(u8* font)
{
	cfont.font=font;
	cfont.x_size = font[0];
	cfont.y_size = font[1];
	cfont.offset = font[2];
	cfont.numchars = font[3];
}


// Print a character
void printChar(u8 c, int x, int y)
{
    u8 ch;
    int i, j, pixelIndex;


    setXY(x, y, x + cfont.x_size - 1,y + cfont.y_size - 1);

    pixelIndex =
            (c - cfont.offset) * (cfont.x_size >> 3) * cfont.y_size + 4;
    for(j = 0; j < (cfont.x_size >> 3) * cfont.y_size; j++) {
        ch = cfont.font[pixelIndex];
        for(i = 0; i < 8; i++) {   
            if ((ch & (1 << (7 - i))) != 0)   
                LCD_Write_DATA16(fch, fcl);
            else
                LCD_Write_DATA16(bch, bcl);
        }
        pixelIndex++;
    }

    clrXY();
}

void printChar2(u8 c, int x, int y)
{
    u8 ch;
    int i, j, pixelIndex;


    set2XY(x, y, x + cfont.x_size - 1,y + cfont.y_size - 1);

    pixelIndex =
            (c - cfont.offset) * (cfont.x_size >> 3) * cfont.y_size + 4;
    for(j = 0; j < (cfont.x_size >> 3) * cfont.y_size; j++) {
        ch = cfont.font[pixelIndex];
        for(i = 0; i < 8; i++) {
            if ((ch & (1 << (7 - i))) != 0)
                LCD2_Write_DATA16(fch, fcl);
            else
                LCD2_Write_DATA16(bch, bcl);
        }
        pixelIndex++;
    }

    clr2XY();
}



// Print string
void lcdPrint2(char *st, int x, int y)
{
    int i = 0;

    while(*st != '\0')
        printChar2(*st++, x + cfont.x_size * i++, y);
}

void drawBackground(){
	int x = 1;
	for(int width = 0; width < 6; width++){
		for(int height = 0; height < 8; height++){
			for(int i = 0; i < 40; i++){
				setColor(0, 255, 255);
				drawHLine(width*40, height*40 + i, 20-x);
				setColor(0, 0, 255);
				drawHLine(width*40 + 20 - x, height*40 + i, 2*x);
				setColor(0, 255, 255);
				drawHLine(width*40 + 20 + x, height*40 + i, 20-x);
				if(i%2)
					x++;
			}
			x = 1;
		}
	}
}

void drawBackground2(){
	int x = 1;
	for(int width = 0; width < 6; width++){
		for(int height = 0; height < 8; height++){
			for(int i = 0; i < 40; i++){
				setColor2(0, 255, 255);
				drawHLine2(width*40, height*40 + i, 20-x);
				setColor2(0, 0, 255);
				drawHLine2(width*40 + 20 - x, height*40 + i, 2*x);
				setColor2(0, 255, 255);
				drawHLine2(width*40 + 20 + x, height*40 + i, 20-x);
				if(i%2)
					x++;
			}
			x = 1;
		}
	}
}

void drawTetris(){
	//int x = 1;
	for(int width = 0; width < 12; width++){
		for(int height = 0; height < 16; height++){
			if(width == 0 || height == 0 || width == 11 || height == 15){
				drawBlock(width, height, 8);
			}else{
				drawBlock(width, height, 0);
			}
		}
	}
}

void drawBlock(int x, int y, int z){
	//extra_disable();
	if(y>14 && !z){
		return;
	}
	int red = 0;
	int green = 0;
	int blue = 0;
	switch(z){
	case 0: //completely black
		setColor(0,0,0);
		fillRect(x*20,y*20,x*20+20,y*20+19);
		return;
	case 1:
		red = 170;
		break;
	case 2:
		green = 170;
		break;
	case 3:
		blue = 170;
		break;
	case 4:
		red = 170;
		green = 170;
		break;
	case 5:
		red = 170;
		blue = 170;
		break;
	case 6:
		green = 170;
		blue = 170;
		break;
	case 7:
		red = 170;
		green = 110;
		break;
	case 8: //light grey / walls
		red = 120;
		green = 120;
		blue = 120;
		break;
	}
	setColor(red*0.5,green*0.5,blue*0.5);
	drawHLine(x*20, y*20, 20);
	drawHLine(x*20, y*20 + 1, 20);
	for(int i = 2; i < 18; i++){
		setColor(red*0.5,green*0.5,blue*0.5);
		drawHLine(x*20, y*20+i, 2);
		setColor(red*1.5,green*1.5,blue*1.5);
		drawHLine(x*20+2, y*20+i, 16);
		setColor(red,green,blue);
		drawHLine(x*20+18, y*20+i, 2);
	}
	drawHLine(x*20, y*20+18, 20);
	drawHLine(x*20, y*20+19, 20);
	//extra_enable();
}
extern int difficulty;
void drawMenu(){
	for(int width = 0; width < 12; width++){
		for(int height = 0; height < 16; height++){
			if(width == 0 || height == 0 || width == 11 || height == 15){
				drawBlock2(width, height, 8);
			}else{
				drawBlock2(width, height, 0);
			}
		}
	}
	//setColor(250, 0, 0);
	//setFont(BigFont);
	//lcdPrint2("Tetris", 40, 80);
	drawTitle();
	drawDifficulty(difficulty);

	drawBlock2(16,30,3);
	drawBlock2(16,31,3);
	drawBlock2(17,30,3);
	drawBlock2(17,31,3);

	drawBlock2(16,35,1);
	drawBlock2(16,36,1);
	drawBlock2(17,35,1);
	drawBlock2(17,36,1);

	drawBlock2(16,26,2);
	drawBlock2(16,27,2);
	drawBlock2(17,26,2);
	drawBlock2(17,27,2);

	drawBlock2(12,30,4);
	drawBlock2(12,31,4);
	drawBlock2(13,30,4);
	drawBlock2(13,31,4);

	drawBlock2(20,30,7);
	drawBlock2(20,31,7);
	drawBlock2(21,30,7);
	drawBlock2(21,31,7);

	setColor(255, 255, 255);
	setFont(SmallFont);
	lcdPrint2("Start", 105, 230);
	lcdPrint2("Difficulty Up", 70, 160);
	lcdPrint2("Difficulty Down", 60, 270);
}
void drawStateB(){
	setColor(255, 255, 255);
	setFont(BigFont);
	lcdPrint2("      ", 75, 90);
	setFont(SmallFont);
	lcdPrint2("             ", 70, 160);
	lcdPrint2("               ", 60, 270);
	lcdPrint2("Rotate", 100, 160);
	lcdPrint2("Pause", 105, 230);
	lcdPrint2("Right", 165, 215);
	lcdPrint2("Left", 50, 215);
	lcdPrint2("Down", 105, 270);
}
void drawStateC(){
	setColor(255, 255, 255);
	setFont(SmallFont);
	lcdPrint2("     ", 165, 215);
	lcdPrint2("    ", 50, 215);
	lcdPrint2("    ", 105, 270);
	lcdPrint2("Main Menu", 85, 160);
	lcdPrint2("Start", 105, 230);
	setFont(BigFont);
	lcdPrint2("Paused", 75, 90);
}
void drawStateD(){
	setColor(255, 255, 255);
	setFont(SmallFont);
	lcdPrint2("Main Menu", 85, 230);
	lcdPrint2("      ", 100, 160);
	lcdPrint2("     ", 165, 215);
	lcdPrint2("    ", 50, 215);
	lcdPrint2("    ", 105, 270);
	setFont(BigFont);
	lcdPrint2("Game Over!", 45, 90);
}

void drawBlock2(int x, int y, int z){
	int red = 0;
	int green = 0;
	int blue = 0;
	switch(z){
	case 0: //completely black
		setColor2(0,0,0);
		fillRect2(x*20,y*20,x*20+20,y*20+19);
		return;
	case 1:
		red = 170;
		break;
	case 2:
		green = 170;
		break;
	case 3:
		blue = 170;
		break;
	case 4:
		red = 170;
		green = 170;
		break;
	case 5:
		red = 170;
		blue = 170;
		break;
	case 6:
		green = 170;
		blue = 170;
		break;
	case 7:
		red = 170;
		green = 110;
		break;
	case 8: //light grey / walls
		red = 120;
		green = 120;
		blue = 120;
		break;
	}
	setColor2(red*0.5,green*0.5,blue*0.5);
	if(z == 8){
		drawHLine2(x*20, y*20, 20);
		drawHLine2(x*20, y*20 + 1, 20);
		for(int i = 2; i < 18; i++){
			setColor2(red*0.5,green*0.5,blue*0.5);
			drawHLine2(x*20, y*20+i, 2);
			setColor2(red*1.5,green*1.5,blue*1.5);
			drawHLine2(x*20+2, y*20+i, 16);
			setColor2(red,green,blue);
			drawHLine2(x*20+18, y*20+i, 2);
		}
		drawHLine2(x*20, y*20+18, 20);
		drawHLine2(x*20, y*20+19, 20);
	}else{
		drawHLine2(x*7, y*7, 7);
		for(int i = 2; i < 6; i++){
			setColor2(red*0.5,green*0.5,blue*0.5);
			drawHLine2(x*7, y*7+i, 1);
			setColor2(red*1.5,green*1.5,blue*1.5);
			drawHLine2(x*7+2, y*7+i, 5);
			setColor2(red,green,blue);
			drawHLine2(x*7+6, y*7+i, 1);
		}
		drawHLine2(x*7, y*7+6, 7);
	}
	//extra_enable();
}

void drawTitle(){
	drawBlock2(5,6,1);
	drawBlock2(6,6,1);
	drawBlock2(7,6,1);
	drawBlock2(8,6,1);
	drawBlock2(9,6,1);
	drawBlock2(7,7,1);
	drawBlock2(7,8,1);
	drawBlock2(7,9,1);
	drawBlock2(7,10,1);

	drawBlock2(11,6,6);
	drawBlock2(12,6,6);
	drawBlock2(13,6,6);
	drawBlock2(11,7,6);
	drawBlock2(11,8,6);
	drawBlock2(12,8,6);
	drawBlock2(13,8,6);
	drawBlock2(11,9,6);
	drawBlock2(11,10,6);
	drawBlock2(12,10,6);
	drawBlock2(13,10,6);

	//drawBlock2(14,6,3);
	drawBlock2(15,6,3);
	drawBlock2(16,6,3);
	drawBlock2(17,6,3);
	//drawBlock2(18,6,3);
	drawBlock2(16,7,3);
	drawBlock2(16,8,3);
	drawBlock2(16,9,3);
	drawBlock2(16,10,3);

	drawBlock2(19,6,4);
	drawBlock2(20,6,4);
	drawBlock2(21,6,4);
	drawBlock2(19,7,4);
	drawBlock2(21,7,4);
	drawBlock2(19,8,4);
	drawBlock2(20,8,4);
	drawBlock2(21,8,4);
	drawBlock2(19,9,4);
	drawBlock2(20,9,4);
	drawBlock2(19,10,4);
	drawBlock2(21,10,4);

	drawBlock2(23,6,2);
	drawBlock2(23,8,2);
	drawBlock2(23,9,2);
	drawBlock2(23,10,2);

	drawBlock2(25,6,7);
	drawBlock2(26,6,7);
	drawBlock2(27,6,7);
	drawBlock2(25,7,7);
	drawBlock2(25,8,7);
	drawBlock2(26,8,7);
	drawBlock2(27,8,7);
	drawBlock2(27,9,7);
	drawBlock2(25,10,7);
	drawBlock2(26,10,7);
	drawBlock2(27,10,7);

}

void drawDifficulty(int d){
	if(d == 5){
		setColor(255, 255, 255);
		setFont(BigFont);
		lcdPrint2(" Hard ", 70, 120);
	}else if(d == 10){
		setColor(255, 255, 255);
		setFont(BigFont);
		lcdPrint2("Medium", 70, 120);
	}else{
		setColor(255, 255, 255);
		setFont(BigFont);
		lcdPrint2(" Easy ", 70, 120);
	}
}
void drawScore(int score){
	setColor(255, 255, 255);
	setFont(BigFont);
	char text[20];
	sprintf(text, "%d", score);
	//string sc = "Score: "
	lcdPrint2("Score:     ", 40, 120);
	lcdPrint2(text, 140, 120);
}
