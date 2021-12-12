/*****************************************************************************
* lab2a.c for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#define AO_LAB2A
#define _BSD_SOURCE

#include "qpn_port.h"
#include "bsp.h"
#include "lab2a.h"
#include "lcd.h"
#include "sys/time.h"
#include "time.h"
#include <stdlib.h>


int count = 0;
int score;

int board[10][16];
int block1x;
int block1y;
int block2x;
int block2y;
int block3x;
int block3y;
int block4x;
int block4y;

int blockType;
int rotation = 1;
int difficulty = 10;

extern XGpio Gpio_leds;


typedef struct Lab2ATag  {               //Lab2A State machine
	QActive super;
}  Lab2A;

/* Setup state machines */
/**********************************************************************/
static QState Lab2A_initial (Lab2A *me);
static QState Lab2A_on      (Lab2A *me);
static QState Lab2A_stateA  (Lab2A *me);
static QState Lab2A_stateB  (Lab2A *me);
static QState Lab2A_stateC  (Lab2A *me);
static QState Lab2A_stateD  (Lab2A *me);


/**********************************************************************/


Lab2A AO_Lab2A;


void Lab2A_ctor(void)  {
	Lab2A *me = &AO_Lab2A;
	QActive_ctor(&me->super, (QStateHandler)&Lab2A_initial);
}


QState Lab2A_initial(Lab2A *me) {
	xil_printf("\n\rInitialization");
    return Q_TRAN(&Lab2A_on);
}

QState Lab2A_on(Lab2A *me) {
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			xil_printf("\n\rOn");
			}
			
		case Q_INIT_SIG: {
			return Q_TRAN(&Lab2A_stateA);
			}
	}
	
	return Q_SUPER(&QHsm_top);
}


/* Create Lab2A_on state and do any initialization code if needed */
/******************************************************************/

QState Lab2A_stateA(Lab2A *me) { //main menu
	switch (Q_SIG(me)) {
		case Q_ENTRY_SIG: {
			//xil_printf("entered state a\n");
			//XGpio_DiscreteWrite(&Gpio_leds, 1, 1);
			extra_disable();
			drawTetris();
			drawMenu();
			extra_enable();
			score = 0;
			return Q_HANDLED();
		}
		case TICK: {
			//XGpio_DiscreteWrite(&Gpio_leds, 1, 2);
			return Q_HANDLED();
		}
		case UP: {
			if(difficulty > 5){
				difficulty -= 5;
				drawDifficulty(difficulty);
			}
			return Q_HANDLED();
		}
		case DOWN:  {
			if(difficulty < 15){
				difficulty += 5;
				drawDifficulty(difficulty);
			}
			return Q_HANDLED();
		}
		case LEFT: {
			return Q_HANDLED();
		}
		case RIGHT:  {
			return Q_HANDLED();
		}
		case MIDDLE:{
			for(int i = 0; i < 10; i++){
				for(int j = 0; j < 16; j++){
					board[i][j] = 0;
				}
			}
			block1x = 0;
			block1y = 0;
			block2x = 0;
			block2y = 0;
			block3x = 0;
			block3y = 0;
			block4x = 0;
			block4y = 0;
			blockType = 0;
			count = 0;
			return Q_TRAN(&Lab2A_stateB);
		}
	}
	return Q_SUPER(&Lab2A_on);
}

QState Lab2A_stateB(Lab2A *me) { //playing
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		//xil_printf("entered state b\n");
		//XGpio_DiscreteWrite(&Gpio_leds, 1, 2);
		drawStateB();
		drawScore(score);
		if(!blockType){
			generateBlock();
		}
		return Q_HANDLED();
	}
	case TICK: {
		//XGpio_DiscreteWrite(&Gpio_leds, 1, 3);
		//xil_printf("tick b\n");
		count++;
		if(count > difficulty){
			/*if(can move down){

			}else{
				if(is in play){
					//score++
					if(any row is completed){
						score += 10;
						//remove row
					}
				}else{
					return Q_TRAN(&Lab2A_stateD);
				}
			}*/
			//xil_printf("start, ");
			if(canLower()){
				block1y--;
				block2y--;
				block3y--;
				block4y--;
				extra_disable();
				if(block1y < 14){
					drawBlock(block1x + 1, block1y + 1, blockType);
					drawBlock(block1x + 1, block1y + 2, 0);
				}
				if(block2y < 14){
					drawBlock(block2x + 1, block2y + 1, blockType);
					drawBlock(block2x + 1, block2y + 2, 0);
				}
				if(block3y < 14){
					drawBlock(block3x + 1, block3y + 1, blockType);
					drawBlock(block3x + 1, block3y + 2, 0);
				}
				if(block4y < 14){
					drawBlock(block4x + 1, block4y + 1, blockType);
					drawBlock(block4x + 1, block4y + 2, 0);
				}
				extra_enable();
			}else{
				board[block1x][block1y] = blockType;
				board[block2x][block2y] = blockType;
				board[block3x][block3y] = blockType;
				board[block4x][block4y] = blockType;
				if(block1y > 13 || block2y > 13 || block3y > 13 || block4y > 13){
					return Q_TRAN(&Lab2A_stateD);
				}
				checkRemoveRow(block1y, block2y, block3y, block4y);
				generateBlock();
				//xil_printf("here\n");
			}
			count = 0;
		}
		//xil_printf("handled tick");
		//XGpio_DiscreteWrite(&Gpio_leds, 1, 4);
		return Q_HANDLED();
	}
	case UP: {
		int tmpblock1x;
		int tmpblock1y;
		int tmpblock2x;
		int tmpblock2y;
		int tmpblock3x;
		int tmpblock3y;
		int tmpblock4x;
		int tmpblock4y;
		switch(blockType){
		case 1:	// T
			switch(rotation){
			case 1:
				tmpblock1x = block2x;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x;
				tmpblock2y = block2y;
				tmpblock3x = block2x + 1;
				tmpblock3y = block2y;
				tmpblock4x = block2x;
				tmpblock4y = block2y + 1;
 				break;
			case 2:
				tmpblock1x = block2x;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x - 1;
				tmpblock2y = block2y;
				tmpblock3x = block2x;
				tmpblock3y = block2y;
				tmpblock4x = block2x + 1;
				tmpblock4y = block2y;
				break;
			case 3:
				tmpblock1x = block1x;
				tmpblock1y = block1y;
				tmpblock2x = block2x;
				tmpblock2y = block2y;
				tmpblock3x = block3x;
				tmpblock3y = block3y;
				tmpblock4x = block3x;
				tmpblock4y = block3y + 1;
				break;
			case 4:
				tmpblock1x = block3x - 1;
				tmpblock1y = block3y;
				tmpblock2x = block3x;
				tmpblock2y = block3y;
				tmpblock3x = block3x + 1;
				tmpblock3y = block3y;
				tmpblock4x = block3x;
				tmpblock4y = block3y + 1;
				break;
			}
			break;
		case 2:	// cube
			return Q_HANDLED();
			break;
		case 3:  // stick
			switch(rotation){
			case 1:
				tmpblock1x = block2x - 1;
				tmpblock1y = block2y;
				tmpblock2x = block2x;
				tmpblock2y = block2y;
				tmpblock3x = block2x + 1;
				tmpblock3y = block2y;
				tmpblock4x = block2x + 2;
				tmpblock4y = block2y;
				break;
			case 2:
				tmpblock1x = block2x;
				tmpblock1y = block2y - 2;
				tmpblock2x = block2x;
				tmpblock2y = block2y - 1;
				tmpblock3x = block2x;
				tmpblock3y = block2y;
				tmpblock4x = block2x;
				tmpblock4y = block2y + 1;
				break;
			case 3:
				tmpblock1x = block3x - 2;
				tmpblock1y = block3y;
				tmpblock2x = block3x - 1;
				tmpblock2y = block3y;
				tmpblock3x = block3x;
				tmpblock3y = block3y;
				tmpblock4x = block3x + 1;
				tmpblock4y = block3y;
				break;
			case 4:
				tmpblock1x = block3x;
				tmpblock1y = block3y - 1;
				tmpblock2x = block3x;
				tmpblock2y = block3y;
				tmpblock3x = block3x;
				tmpblock3y = block3y + 1;
				tmpblock4x = block3x;
				tmpblock4y = block3y + 2;
				break;
			}
			break;
		case 4:	// L
			switch(rotation){
			case 1:
				tmpblock1x = block3x - 1;
				tmpblock1y = block3y - 1;
				tmpblock2x = block3x;
				tmpblock2y = block3y - 1;
				tmpblock3x = block3x;
				tmpblock3y = block3y;
				tmpblock4x = block3x;
				tmpblock4y = block3y + 1;
				break;
			case 2:
				tmpblock1x = block3x - 1;
				tmpblock1y = block3y;
				tmpblock2x = block3x;
				tmpblock2y = block3y;
				tmpblock3x = block3x + 1;
				tmpblock3y = block3y;
				tmpblock4x = block3x - 1;
				tmpblock4y = block3y + 1;
				break;
			case 3:
				tmpblock1x = block2x;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x;
				tmpblock2y = block2y;
				tmpblock3x = block2x;
				tmpblock3y = block2y + 1;
				tmpblock4x = block2x + 1;
				tmpblock4y = block2y + 1;
				break;
			case 4:
				tmpblock1x = block2x + 1;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x - 1;
				tmpblock2y = block2y;
				tmpblock3x = block2x;
				tmpblock3y = block2y;
				tmpblock4x = block2x + 1;
				tmpblock4y = block2y;
				break;
			}
			break;
		case 5: // backwards L
			switch(rotation){
			case 1:
				tmpblock1x = block2x;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x + 1;
				tmpblock2y = block2y - 1;
				tmpblock3x = block2x;
				tmpblock3y = block2y;
				tmpblock4x = block2x;
				tmpblock4y = block2y + 1;
				break;
			case 2:
				tmpblock1x = block3x - 1;
				tmpblock1y = block3y - 1;
				tmpblock2x = block3x - 1;
				tmpblock2y = block3y;
				tmpblock3x = block3x;
				tmpblock3y = block3y;
				tmpblock4x = block3x + 1;
				tmpblock4y = block3y;
				break;
			case 3:
				tmpblock1x = block3x;
				tmpblock1y = block3y - 1;
				tmpblock2x = block3x;
				tmpblock2y = block3y;
				tmpblock3x = block3x - 1;
				tmpblock3y = block3y + 1;
				tmpblock4x = block3x;
				tmpblock4y = block3y + 1;
				break;
			case 4:
				tmpblock1x = block2x - 1;
				tmpblock1y = block2y;
				tmpblock2x = block2x;
				tmpblock2y = block2y;
				tmpblock3x = block2x + 1;
				tmpblock3y = block2y;
				tmpblock4x = block2x + 1;
				tmpblock4y = block2y + 1;
				break;
			}
			break;
		case 6:	// left isomer
			switch(rotation){
			case 1:
				tmpblock1x = block1x;
				tmpblock1y = block1y - 1;
				tmpblock2x = block1x;
				tmpblock2y = block1y;
				tmpblock3x = block1x + 1;
				tmpblock3y = block1y;
				tmpblock4x = block1x + 1;
				tmpblock4y = block1y + 1;
				break;
			case 2:
				tmpblock1x = block2x;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x + 1;
				tmpblock2y = block2y - 1;
				tmpblock3x = block2x - 1;
				tmpblock3y = block2y;
				tmpblock4x = block2x;
				tmpblock4y = block2y;
				break;
			case 3:
				tmpblock1x = block4x - 1;
				tmpblock1y = block4y - 1;
				tmpblock2x = block4x - 1;
				tmpblock2y = block4y;
				tmpblock3x = block4x;
				tmpblock3y = block4y;
				tmpblock4x = block4x;
				tmpblock4y = block4y + 1;
				break;
			case 4:
				tmpblock1x = block3x;
				tmpblock1y = block3y;
				tmpblock2x = block3x + 1;
				tmpblock2y = block3y;
				tmpblock3x = block3x - 1;
				tmpblock3y = block3y + 1;
				tmpblock4x = block3x;
				tmpblock4y = block3y + 1;
				break;
			}
			break;
		case 7:	// right isomer
			switch(rotation){
			case 1:
				tmpblock1x = block2x + 1;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x;
				tmpblock2y = block2y;
				tmpblock3x = block2x + 1;
				tmpblock3y = block2y;
				tmpblock4x = block2x;
				tmpblock4y = block2y + 1;
				break;
			case 2:
				tmpblock1x = block2x - 1;
				tmpblock1y = block2y - 1;
				tmpblock2x = block2x;
				tmpblock2y = block2y - 1;
				tmpblock3x = block2x;
				tmpblock3y = block2y;
				tmpblock4x = block2x + 1;
				tmpblock4y = block2y;
				break;
			case 3:
				tmpblock1x = block3x;
				tmpblock1y = block3y - 1;
				tmpblock2x = block3x - 1;
				tmpblock2y = block3y;
				tmpblock3x = block3x;
				tmpblock3y = block3y;
				tmpblock4x = block3x - 1;
				tmpblock4y = block3y + 1;
				break;
			case 4:
				tmpblock1x = block3x - 1;
				tmpblock1y = block3y;
				tmpblock2x = block3x;
				tmpblock2y = block3y;
				tmpblock3x = block3x;
				tmpblock3y = block3y + 1;
				tmpblock4x = block3x + 1;
				tmpblock4y = block3y + 1;
				break;
			}
			break;
		}
		if(tmpblock1x < 0 || tmpblock1x > 9 || tmpblock1y < 0 || tmpblock1y > 15 || tmpblock2x < 0 || tmpblock2x > 9 || tmpblock2y < 0 || tmpblock2y > 15 || tmpblock3x < 0 || tmpblock3x > 9 || tmpblock3y < 0 || tmpblock3y > 15 || tmpblock4x < 0 || tmpblock4x > 9 || tmpblock4y < 0 || tmpblock4y > 15)
			return Q_HANDLED();
		if(!board[tmpblock1x][tmpblock1y] && !board[tmpblock2x][tmpblock2y] && !board[tmpblock3x][tmpblock3y] && !board[tmpblock4x][tmpblock4y]){
			rotation++;
			if(rotation == 5)
				rotation = 1;
			extra_disable();
			if(block1y < 14)
				drawBlock(block1x + 1, block1y + 1, 0);
			if(block2y < 14)
			drawBlock(block2x + 1, block2y + 1, 0);
			if(block3y < 14)
			drawBlock(block3x + 1, block3y + 1, 0);
			if(block4y < 14)
			drawBlock(block4x + 1, block4y + 1, 0);
			block1x = tmpblock1x;
			block1y = tmpblock1y;
			block2x = tmpblock2x;
			block2y = tmpblock2y;
			block3x = tmpblock3x;
			block3y = tmpblock3y;
			block4x = tmpblock4x;
			block4y = tmpblock4y;
			if(block1y < 14)
				drawBlock(block1x + 1, block1y + 1, blockType);
			if(block2y < 14)
				drawBlock(block2x + 1, block2y + 1, blockType);
			if(block3y < 14)
				drawBlock(block3x + 1, block3y + 1, blockType);
			if(block4y < 14)
				drawBlock(block4x + 1, block4y + 1, blockType);
			extra_enable();

		}
		return Q_HANDLED();
	}
	case DOWN:  {
		if(canLower()){
			block1y--;
			block2y--;
			block3y--;
			block4y--;
			extra_disable();
			if(block1y < 14){
				drawBlock(block1x + 1, block1y + 1, blockType);
				drawBlock(block1x + 1, block1y + 2, 0);
			}
			if(block2y < 14){
				drawBlock(block2x + 1, block2y + 1, blockType);
				drawBlock(block2x + 1, block2y + 2, 0);
			}
			if(block3y < 14){
				drawBlock(block3x + 1, block3y + 1, blockType);
				drawBlock(block3x + 1, block3y + 2, 0);
			}
			if(block4y < 14){
				drawBlock(block4x + 1, block4y + 1, blockType);
				drawBlock(block4x + 1, block4y + 2, 0);
			}
			extra_enable();
		}else{
			board[block1x][block1y] = blockType;
			board[block2x][block2y] = blockType;
			board[block3x][block3y] = blockType;
			board[block4x][block4y] = blockType;
			if(block1y > 13 || block2y > 13 || block3y > 13 || block4y > 13){
				return Q_TRAN(&Lab2A_stateD);
			}
			checkRemoveRow(block1y, block2y, block3y, block4y);
			generateBlock();
			//xil_printf("here\n");
		}
		count = 0;
		return Q_HANDLED();
	}
	case LEFT: {
		if(canLeft()){
			block1x++;
			block2x++;
			block3x++;
			block4x++;

			extra_disable();
			if(block4y < 14){
				drawBlock(block4x + 1, block4y + 1, blockType);
				drawBlock(block4x, block4y + 1, 0);
			}
			if(block3y < 14){
				drawBlock(block3x + 1, block3y + 1, blockType);
				drawBlock(block3x, block3y + 1, 0);
			}
			if(block2y < 14){
				drawBlock(block2x + 1, block2y + 1, blockType);
				drawBlock(block2x, block2y + 1, 0);
			}
			if(block1y < 14){
				drawBlock(block1x + 1, block1y + 1, blockType);
				drawBlock(block1x, block1y + 1, 0);
			}
			extra_enable();
		}
		return Q_HANDLED();
	}
	case RIGHT:  {
		if(canRight()){
			block1x--;
			block2x--;
			block3x--;
			block4x--;
			extra_disable();
			if(block1y < 14){
				drawBlock(block1x + 1, block1y + 1, blockType);
				drawBlock(block1x + 2, block1y + 1, 0);
			}
			if(block2y < 14){
				drawBlock(block2x + 1, block2y + 1, blockType);
				drawBlock(block2x + 2, block2y + 1, 0);
			}
			if(block3y < 14){
				drawBlock(block3x + 1, block3y + 1, blockType);
				drawBlock(block3x + 2, block3y + 1, 0);
			}
			if(block4y < 14){
				drawBlock(block4x + 1, block4y + 1, blockType);
				drawBlock(block4x + 2, block4y + 1, 0);
			}
			extra_enable();
		}
		return Q_HANDLED();
	}
	case MIDDLE:{
		//return Q_HANDLED();
		return Q_TRAN(&Lab2A_stateC);
	}
}

	return Q_SUPER(&Lab2A_on);

}

QState Lab2A_stateC(Lab2A *me) { //paused
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		//xil_printf("entered state c\n");
		drawStateC();
		//say pause
		return Q_HANDLED();
	}
	case TICK: {
		return Q_HANDLED();
	}
	case UP: {
		return Q_TRAN(&Lab2A_stateA);
	}
	case DOWN:  {
		return Q_HANDLED();
	}
	case LEFT: {
		return Q_HANDLED();
	}
	case RIGHT:  {
		return Q_HANDLED();
	}
	case MIDDLE:{
		return Q_TRAN(&Lab2A_stateB);
	}
}

	return Q_SUPER(&Lab2A_on);

}

QState Lab2A_stateD(Lab2A *me) { //game over
	switch (Q_SIG(me)) {
	case Q_ENTRY_SIG: {
		drawStateD();
		//xil_printf("Game Over\n");
		//say Game Over
		return Q_HANDLED();
	}
	case TICK: {
		return Q_HANDLED();
	}
	case UP: {
		return Q_HANDLED();
	}
	case DOWN:  {
		return Q_HANDLED();
	}
	case LEFT: {
		return Q_HANDLED();
	}
	case RIGHT:  {
		return Q_HANDLED();
	}
	case MIDDLE:{
		return Q_TRAN(&Lab2A_stateA);
	}
}

	return Q_SUPER(&Lab2A_on);

}

void generateBlock(){
	rotation = 1;
	blockType = rand() % 7 + 1;
	switch(blockType){
	case 1:	// T
		block1x = 4;
		block1y = 14;
		block2x = 5;
		block2y = 14;
		block3x = 6;
		block3y = 14;
		block4x = 5;
		block4y = 15;
		break;
	case 2:	// cube
		block1x = 4;
		block1y = 14;
		block2x = 5;
		block2y = 14;
		block3x = 4;
		block3y = 15;
		block4x = 5;
		block4y = 15;
		break;
	case 3:  // stick
		block1x = 5;
		block1y = 14;
		block2x = 5;
		block2y = 15;
		block3x = 5;
		block3y = 16;
		block4x = 5;
		block4y = 17;
		break;
	case 4:	// L
		block1x = 6;
		block1y = 14;
		block2x = 4;
		block2y = 15;
		block3x = 5;
		block3y = 15;
		block4x = 6;
		block4y = 15;
		break;
	case 5: // backwards L
		block1x = 4;
		block1y = 14;
		block2x = 5;
		block2y = 14;
		block3x = 6;
		block3y = 14;
		block4x = 6;
		block4y = 15;
		break;
	case 6:	// left isomer
		block1x = 5;
		block1y = 14;
		block2x = 6;
		block2y = 14;
		block3x = 4;
		block3y = 15;
		block4x = 5;
		block4y = 15;
		break;
	case 7:	// right isomer
		block1x = 4;
		block1y = 14;
		block2x = 5;
		block2y = 14;
		block3x = 5;
		block3y = 15;
		block4x = 6;
		block4y = 15;
		break;
	}
}

int canLower(){
	if(!block1y || !block2y || !block3y || !block4y){
		//xil_printf("Can't lower 1\n");
		return 0;
	}
	if(board[block1x][block1y - 1] || board[block2x][block2y - 1] || board[block3x][block3y - 1]){
		//xil_printf("Can't lower 2\n");
		return 0;
	}
	if(board[block4x][block4y - 1] && blockType != 3)
		return 0;
	return 1;
}
int canLeft(){
	if(!(block1x < 9) || !(block2x < 9) || !(block3x < 9) || !(block4x < 9)){
		return 0;
	}
	if(board[block1x + 1][block1y] || board[block2x + 1][block2y] || board[block3x + 1][block3y] || board[block4x + 1][block4y]){
		return 0;
	}
	if(board[block4x][block4y - 1] && blockType != 3)
		return 0;
	return 1;
}
int canRight(){
	if(!block1x || !block2x || !block3x || !block4x){
		return 0;
	}
	if(board[block1x - 1][block1y] || board[block2x - 1][block2y] || board[block3x - 1][block3y] || board[block4x - 1][block4y]){
		return 0;
	}
	return 1;
}
void checkRemoveRow(int y1, int y2, int y3, int y4){
	int flag1 = 1;
	int flag2 = 1;
	int flag3 = 1;
	int flag4 = 1;
	int tmp = 0;
	//sorting

	if(y1>y2){
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y3>y4){
		tmp = y3;
		y3 = y4;
		y4 = tmp;
	}
	if(y1>y3){
		tmp = y1;
		y1 = y3;
		y3 = tmp;
	}
	if(y2>y4){
		tmp = y2;
		y2 = y4;
		y4 = tmp;
	}
	if(y2>y3){
		tmp = y2;
		y2 = y3;
		y3 = tmp;
	}

	//checking

	for(int i = 0; i < 10; i++){
		if(!board[i][y1]){
			flag1 = 0;
			break;
		}
	}
	if(y1 != y2){
		for(int i = 0; i < 10; i++){
			if(!board[i][y2]){
				flag2 = 0;
				break;
			}
		}
	}else{
		flag2 = 0;
	}
	if(y1 != y3 && y2 != y3){
		for(int i = 0; i < 10; i++){
			if(!board[i][y3]){
				flag3 = 0;
				break;
			}
		}
	}else{
		flag3 = 0;
	}
	if(y1 != y4 && y2 != y4 && y3 != y4){
		for(int i = 0; i < 10; i++){
			if(!board[i][y4]){
				flag4 = 0;
				break;
			}
		}
	}else{
		flag4 = 0;
	}
	switch(flag1+flag2+flag3+flag4){
	case 0:
		return;
	case 1:
		score += 10;
		extra_disable();
		drawScore(score);
		extra_enable();
		break;
	case 2:
		score += 25;
		extra_disable();
		drawScore(score);
		extra_enable();
		break;
	case 3:
		score += 100;
		extra_disable();
		drawScore(score);
		extra_enable();
		break;
	case 4:
		score += 1000;
		extra_disable();
		drawScore(score);
		extra_enable();
		break;
	}
	//removing
	if(flag4){
		for(int i = y4; i < 14; i++){
			for(int j = 0; j < 10; j++){
				if(board[j][i] != board[j][i+1]){
					extra_disable();
					drawBlock(j + 1, i + 1, board[j][i+1]);
					extra_enable();
					board[j][i] = board[j][i+1];
				}
			}
		}
	}

	if(flag3){
		for(int i = y3; i < 14; i++){
			for(int j = 0; j < 10; j++){
				if(board[j][i] != board[j][i+1]){
					extra_disable();
					drawBlock(j + 1, i + 1, board[j][i+1]);
					extra_enable();
					board[j][i] = board[j][i+1];
				}
			}
		}
	}
	if(flag2){
		for(int i = y2; i < 14; i++){
			for(int j = 0; j < 10; j++){
				if(board[j][i] != board[j][i+1]){
					extra_disable();
					drawBlock(j + 1, i + 1, board[j][i+1]);
					extra_enable();
					board[j][i] = board[j][i+1];
				}
			}
		}
	}
	if(flag1){
		for(int i = y1; i < 14; i++){
			for(int j = 0; j < 10; j++){
				if(board[j][i] != board[j][i+1]){
					extra_disable();
					drawBlock(j + 1, i + 1, board[j][i+1]);
					extra_enable();
					board[j][i] = board[j][i+1];
				}
			}
		}
	}

}
//board
