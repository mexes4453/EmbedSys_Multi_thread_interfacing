// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
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
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SDECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"

#define SYSCTL_RCGC2_R    		(*((volatile unsigned long *)0x400FE108))	// Port clock gating
#define PA2_HEADPHONE_OUTPUT								(*((volatile unsigned long *)0x40004010)) // specific bit add
#define PA3_SW_INPUT						 		(*((volatile unsigned long *)0x40004020))	// specific bit add
#define GPIO_PORTA_DATA_R 		(*((volatile unsigned long *)0x400043FC)) // Analog func register
#define GPIO_PORTA_AMSEL_R 		(*((volatile unsigned long *)0x40004528)) // Analog func register
#define GPIO_PORTA_AFSEL_R 		(*((volatile unsigned long *)0x40004420))	// Alternate Func reg
#define GPIO_PORTA_PCTL_R 		(*((volatile unsigned long *)0x4000452C))	// Peripheral func reg
#define GPIO_PORTA_DIR_R 		  (*((volatile unsigned long *)0x40004400))	// Direction reg I/O
#define GPIO_PORTA_DEN_R 		  (*((volatile unsigned long *)0x4000451C)) // Enable Digital reg
//#define GPIO_PORTA_DR8R_R 		(*((volatile unsigned long *)0x40004508))	// Drive up 8mA
//#define GPIO_PORTA_PDR_R 		  (*((volatile unsigned long *)0x40004514))	// Drive up 8mA
// #define GPIO_PORTA_IS_R 	  	(*((volatile unsigned long *)0x40004404)) // Interrupt sense reg
// #define GPIO_PORTA_IBE_R 	  	(*((volatile unsigned long *)0x40004408))	// Interrupt both edge reg
// #define GPIO_PORTA_IEV_R 	  	(*((volatile unsigned long *)0x4000440C)) // Interrupt event reg
#define GPIO_PORTA_ICR_R 	  	(*((volatile unsigned long *)0x4000441C))	// Interrupt clear reg
#define GPIO_PORTA_IM_R 		  (*((volatile unsigned long *)0x40004410))	// Interrupt mask reg - arm
#define NVIC_SYS_PRI0_R 		  (*((volatile unsigned long *)0xE000E400))	// System clock interrupt priority reg PortA
#define NVIC_EN0_R      		  (*((volatile unsigned long *)0xE000E100))	// Interrupt enable reg
#define NVIC_ST_CTRL_R 		    (*((volatile unsigned long *)0xE000E010)) // System clock reg
#define NVIC_ST_RELOAD_R 		  (*((volatile unsigned long *)0xE000E014))	// System clock reload reg
#define NVIC_ST_CURRENT_R 		(*((volatile unsigned long *)0xE000E018)) // System clock current reg
#define NVIC_SYS_PRI3_R 		  (*((volatile unsigned long *)0xE000ED20))	// System clock interrupt priority reg SysTick
	

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void SysTick_Init(unsigned long);
void Sound_Init(void);
void Toggle(void);
void NoToggle(void);
void SwitchPressState(void);


unsigned long SD, SU, SS;
// SD => switch up
// SU => switch down
// SS => switch state

// input from PA3, output from PA2, SysTick interrupts
// called at 880 Hz
void SysTick_Handler(void){
	// SD => switch down	(Pushed)    (Global variable counter)
	// SU => switch up		(Released)  (Global variable counter)
	// the global variables counts, each time switch state occurs 

	if (SD==0){NoToggle();}					// switch has never been pushed - no toggle
	else if (SD==1){Toggle();}			// switch has been pushed or released once (first time)
	else if (SD%2==1){Toggle();}		// switch has been pushed to activate sound
	else if (SD%2==0){NoToggle();}  // switch has been pushed to deactivate sound
}


int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
	Sound_Init();
	SysTick_Init(90909);		// 1ms => 80000 counts
  EnableInterrupts();     // enable after all initialization are done
	SD=0; SU=0; SS=0;
	//DisableInterrupts(); // Disable interrupts

	
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
		// WaitForInterrupt();

		// check current switch state against previous switch state 
		SwitchPressState();
	}
}


void Sound_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x01;															// Enable clock for port A
	delay = SYSCTL_RCGC2_R;						 									// allow 4cycles for clock to start
	GPIO_PORTA_AMSEL_R &= ~0x0C;												// Disable analog function for bit 2,3
	GPIO_PORTA_AFSEL_R &= ~0x0C;												// Disable alternate function for bit 2,3
	GPIO_PORTA_PCTL_R &= ~0x0000FF00;		 								// Disable peripheral function for bit 2, 3 (PA3 - Input)
	GPIO_PORTA_DIR_R |= 0x04;														// Set bit 2 => Output
	GPIO_PORTA_DIR_R &= ~0x08; 													// Set bit 3 => Input
	GPIO_PORTA_PDR_R |= 0x08;
	GPIO_PORTA_DEN_R |= 0x0C;														// Enable digital signal for bit 2, 3
	// GPIO_PORTA_IS_R &= ~0x08;												// Set Interrupt Sense to 0 for bit 3
	// GPIO_PORTA_IBE_R &= ~0x08;												// Set Interrupt both edge to 0 for bit 3
	// GPIO_PORTA_IEV_R &= ~0x08;												// Set Interrupt event to 0 for bit 3
	GPIO_PORTA_ICR_R |= 0x08; 													// clear interrupt flag
	//GPIO_PORTA_IM_R |= 0x08;													// Arm interrupt for bit 3 PA3
	// NVIC_SYS_PRI0_R = NVIC_SYS_PRI0_R&0x00FFFFFF; 		// Priority 0
	NVIC_EN0_R = 0x01;																	// Enable interrupt at bit 0 in NVIC
}

void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;							// Disable systick during setup
	NVIC_ST_RELOAD_R = period-1;		// Reload value
	NVIC_ST_CURRENT_R = 0;					// any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // Priority 0
	NVIC_ST_CTRL_R = 0x07; 					// Enable the clock with interrupt =>0x07

}

void Toggle(void){
	//unsigned long count = 0;
	PA2_HEADPHONE_OUTPUT ^=0x04;
}

void NoToggle(void){
	//unsigned long count = 0;
	PA2_HEADPHONE_OUTPUT =0x00;
}


void SwitchPressState(void){
		// check current switch state against previous switch state 
		if (PA3_SW_INPUT!=SS){			// when switch state change is detected
			if (PA3_SW_INPUT > 0){		// when switch goes high 
				SD++;										// increase switch down count by 1 
				
			}else{										// when switch goes low
				SU++;										// increase switch up count by 1 
			}
			SS=PA3_SW_INPUT;					// update the switch state with its current state
		}

}

