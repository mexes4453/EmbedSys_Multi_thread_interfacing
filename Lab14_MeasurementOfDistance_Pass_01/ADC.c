// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result. 
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "tm4c123gh6pm.h"

#define SYSCTL_RCGC2_R    		(*((volatile unsigned long *)0x400FE108))	// Port clock gating
#define PE2   								(*((volatile unsigned long *)0x40024010)) // specific bit add
#define GPIO_PORTE_DATA_R 		(*((volatile unsigned long *)0x400243FC)) // Analog func register
#define GPIO_PORTE_AMSEL_R 		(*((volatile unsigned long *)0x40024528)) // Analog func register
#define GPIO_PORTE_AFSEL_R 		(*((volatile unsigned long *)0x40024420))	// Alternate Func reg
#define GPIO_PORTE_PCTL_R 		(*((volatile unsigned long *)0x4002452C))	// Peripheral func reg
#define GPIO_PORTE_DIR_R 		  (*((volatile unsigned long *)0x40024400))	// Direction reg I/O
#define GPIO_PORTE_DEN_R 		  (*((volatile unsigned long *)0x4002451C)) // Enable Digital reg
#define ADC0_SSPRI_R 				  (*((volatile unsigned long *)0x40038020))			// 8. Sequencer 3 is highest priority
#define ADC0_ACTSS_R 				  (*((volatile unsigned long *)0x40038000))		  // 9. Disable sample sequencer3
#define ADC0_EMUX_R 				  (*((volatile unsigned long *)0x40038014))			// 10 Seq3 is software triggered
#define ADC0_SSMUX3_R 				(*((volatile unsigned long *)0x400380A0))		  // 11 Clear ss3 field
#define ADC0_SSCTL3_R 				(*((volatile unsigned long *)0x400380A4))		  // 12 No TS0; D0, Yes IE0; END0
#define ADC0_PSSI_R 				  (*((volatile unsigned long *)0x40038028))
#define ADC0_RIS_R 				    (*((volatile unsigned long *)0x40038004))
#define ADC0_SSFIFO3_R 				(*((volatile unsigned long *)0x400380A8))
#define ADC0_ISC_R 						(*((volatile unsigned long *)0x4003800C))


	
/*
#define NVIC_SYS_PRI0_R 		  (*((volatile unsigned long *)0xE000E400))	// System clock interrupt priority reg PortA
#define NVIC_EN0_R      		  (*((volatile unsigned long *)0xE000E100))	// Interrupt enable reg
#define NVIC_ST_CTRL_R 		    (*((volatile unsigned long *)0xE000E010)) // System clock reg
#define NVIC_ST_RELOAD_R 		  (*((volatile unsigned long *)0xE000E014))	// System clock reload reg
#define NVIC_ST_CURRENT_R 		(*((volatile unsigned long *)0xE000E018)) // System clock current reg
#define NVIC_SYS_PRI3_R 		  (*((volatile unsigned long *)0xE000ED20))	// System clock interrupt priority reg SysTick
*/


// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;										// 1. Activate clock for Port E
	delay = SYSCTL_RCGC2_R;										// 		Allow time for clock to stabilize
	GPIO_PORTE_DIR_R &= ~0x04;								// 2. Make PE2 input
	GPIO_PORTE_AFSEL_R |= 0x04;								// 3. Enable alternate func
	GPIO_PORTE_DEN_R &= ~0x04;								// 4. Disable digital I/O
	GPIO_PORTE_AMSEL_R |= 0x04;								// 5. Enable analog func
	SYSCTL_RCGC0_R |= 0x00010000;							// 6. Activate ADC clock
	delay = SYSCTL_RCGC0_R;										// 		Allow time for clock to stabilize
	SYSCTL_RCGC0_R &= ~0x00000300;						// 7. Select sampling speed (125k)
	ADC0_SSPRI_R = 0x0123;										// 8. Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;									// 9. Disable sample sequencer3
	ADC0_EMUX_R &= ~0xF000;										// 10 Seq3 is software triggered
	ADC0_SSMUX3_R &= ~0x000F;									// 11 Clear ss3 field
	ADC0_SSMUX3_R += 1;												//	  set channel (Ain1 - PE2)
	ADC0_SSCTL3_R = 0x0006;										// 12 No TS0; D0, Yes IE0; END0
	ADC0_ACTSS_R |= 0x0008;									  // 13 Enable sample sequencer3
	
  
}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){  unsigned long result;
	ADC0_PSSI_R = 0x0008;										// 1. Initiate SS3
	while((ADC0_RIS_R&0x08)==0){}; 					// 2. Wait for conversion done
	result = ADC0_SSFIFO3_R&0xFFF;					// 3. Read result
	ADC0_ISC_R = 0x0008;										// 4. Acknowledge completion
	return result;
	
}
