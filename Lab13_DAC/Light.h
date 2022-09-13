// Piano.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano, Jonathan Valvano
// December 29, 2014

// 1. Preprocessor directive section
// constant declaration to access port registers using
// symbolic names instead of addresses
#include <stdio.h> // diamond braces for sys lib: import standard I/O
//#include "uart.h"	 // quotes is used for user lib: UART Lib

#define GPIO_PORTF_DATA_R 	(*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R 		(*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R 	(*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R 		(*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R 		(*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R 	(*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R 		(*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R 	(*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R 	(*((volatile unsigned long *)0x4002552C))
#define SYSCTL_RCGC2_R		 	(*((volatile unsigned long *)0x400FE108))
	

// 2. Declaration Section
//	Global Variables


// 	Function Prototypes
void PortF_init(void);
void FlashSOS(void);
void Tester(unsigned long);



// 3. Subroutine section

