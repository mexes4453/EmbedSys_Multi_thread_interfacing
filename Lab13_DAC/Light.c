// Piano.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano, Jonathan Valvano
// December 29, 2014

#include "..//tm4c123gh6pm.h"
//#include <stdio.h>
#include "Light.h"
// 2. Declaration Section
//	Global Variables


// 3. Subroutine section

void Tester(unsigned long sw){
	
	// PortF_init();		// Initialize ports PF4, PF0, PF2
	do{ 
		FlashSOS();	
	}while (sw > 1);
	
}


// subroutine to initialize port F pins for input and output
// PF4 is input SW1 and PF2 is output 
void PortF_init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x0020; 					//	01. F clock
	delay = SYSCTL_RCGC2_R;							//	02. delay
	GPIO_PORTF_LOCK_R =	0x4c4F434B;			//	03. Unlock PortF PF0
	GPIO_PORTF_CR_R	|= 0x001F;					//	04. Allow changes to PF4-0
	GPIO_PORTF_AMSEL_R &= 0x00;					//	05. Disable analog function
	GPIO_PORTF_PCTL_R &= 0x00;					//	06. GPIO Clear bit PCTL
	//GPIO_PORTF_DIR_R &= ~0x11;					//	07. PF4 & PF0 Input
	GPIO_PORTF_DIR_R |= 0x08;						//	07. PF3 Output
	GPIO_PORTF_AFSEL_R &= ~0x1F;				//	08. No alternate functions
	GPIO_PORTF_PUR_R |= 0x11;						//	09. Enable pullup resistor for PF4 & PF0 
	GPIO_PORTF_DEN_R |= 0x1F;						//	10. Enable digital pins PF4 - PF0
	
	
}

// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E


// Subroutine to Flash a green LED SOS once
// PF3 is green LED: SOS
void FlashSOS(void){
	//S : Toggle light 3 times with 1/2 sec gap between ON..1/2sec...OFF
	GPIO_PORTF_DATA_R |= 0x04; 
}
