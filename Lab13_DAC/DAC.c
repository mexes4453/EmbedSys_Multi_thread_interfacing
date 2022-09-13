// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "..//tm4c123gh6pm.h"


#define GPIO_PORTB_DATA_R 	(*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R 		(*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R 	(*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R 		(*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R 		(*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_LOCK_R 	(*((volatile unsigned long *)0x40005520))
#define GPIO_PORTB_CR_R 		(*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R 	(*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R 	(*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTB_DR8R_R 	(*((volatile unsigned long *)0x40005508))
#define SYSCTL_RCGC2_R		 	(*((volatile unsigned long *)0x400FE108))

#define PB3 	(*((volatile unsigned long *)0x40005020))
#define PB2   (*((volatile unsigned long *)0x40005010))
#define PB1		(*((volatile unsigned long *)0x40005008))
#define PB0 	(*((volatile unsigned long *)0x40005004))



// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
	
    unsigned long volatile delay;           // clock variable
    SYSCTL_RCGC2_R |= 0x00000002;   // activate port B;
    delay = SYSCTL_RCGC2_R;                 // allow time to clock startup and activation
    GPIO_PORTB_AMSEL_R &= ~0x0F;            // no analog
		GPIO_PORTB_DR8R_R |= 0x0F; 
	  GPIO_PORTB_PCTL_R &= ~0x0000FFFF;       // regular GPIO function
    GPIO_PORTB_DIR_R |= 0x0F;               // make PB3-0 out
    GPIO_PORTB_AFSEL_R &= ~0x07;            // disable alternate func on output bits
    GPIO_PORTB_DEN_R |= 0x0F;               // enable digital I/O on PB3 

}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
	GPIO_PORTB_DATA_R = data;
  
}
