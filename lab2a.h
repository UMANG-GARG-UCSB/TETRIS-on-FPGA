/*****************************************************************************
* lab2a.h for Lab2A of ECE 153a at UCSB
* Date of the Last Update:  October 23,2014
*****************************************************************************/

#ifndef lab2a_h
#define lab2a_h

enum Lab2ASignals {
	MIDDLE = Q_USER_SIG,
	TICK,
	UP,
	DOWN,
	LEFT,
	RIGHT
};


extern struct Lab2ATag AO_Lab2A;


void Lab2A_ctor(void);
void GpioHandler(void *CallbackRef);
void TwistHandler(void *CallbackRef);
void generateBlock();
int canLower();
int canLeft();
int canRight();
void checkRemoveRow(int y1, int y2, int y3, int y4);

#endif  
