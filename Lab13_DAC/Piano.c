// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"






// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ 
 	
    unsigned long volatile delay;           // clock variable
    SYSCTL_RCGC2_R |= 0x00000010;   				// activate port E;
    delay = SYSCTL_RCGC2_R;                 // allow time to clock startup and activation
    GPIO_PORTE_AMSEL_R &= ~0x1F;            // no analog
	  GPIO_PORTE_PCTL_R &= ~0x0000FFFF;       // regular GPIO function
    GPIO_PORTE_DIR_R &= ~0x0F;              // make PB3-0 Input keys
		GPIO_PORTE_DIR_R |= 0x10;               // make PB4 Output keys
    GPIO_PORTE_AFSEL_R &= ~0x1F;            // disable alternate func on Input and output bits
    GPIO_PORTE_DEN_R |= 0x1F;               // enable digital I/O on PB3-0


}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
  unsigned long input;
	input = 0;
	input = GPIO_PORTE_DATA_R;
	
  return input; // remove this, replace with input
}

void Tester(unsigned long data){
	data = data&0x01;
	if (data == 0x01){GPIO_PORTE_DATA_R |= 0x10;}
	else{GPIO_PORTE_DATA_R &= ~0x10;}
	
}

