// MeasurementOfDistance.c
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to periodically initiate a software-
// triggered ADC conversion, convert the sample to a fixed-
// point decimal distance, and store the result in a mailbox.
// The foreground thread takes the result from the mailbox,
// converts the result to a string, and prints it to the
// Nokia5110 LCD.  The display is optional.
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

// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"
#include "UART.h"
#include "SysTick.h"
//#include "PLL.h"



void EnableInterrupts(void);  // Enable interrupts
unsigned long Convert(unsigned long sample);
void UART_ConvertDistance(unsigned long n);
void UART_ConvertUDec(unsigned long n);
//void SysTick_Init(unsigned long);
//void UART_OutString(unsigned char []);

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag;       // 1 means valid Distance, 0 means Distance is empty
unsigned long counter;
signed int error;

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
  //double A=0.45538;
  //double  B=143.582; 
	//unsigned long A=466;
  //unsigned long  B=147028;
	Distance = (sample+2)*2000/4094; // 2000mm => 2cm
	// Distance = ((sample-3870)/(4094-3870))*2000; // 2000mm => 2cm
	//Distance = ((A*ADCdata)>>10)+B;    // where A=466 and B=147028 are calibration constants
  return Distance;  // replace this line with real code
}


// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
  	ADCdata = ADC0_In();
		Flag = 1;
		counter++;
}


// once the ADC and convert to distance functions are operational,
// you should use this main to build the final solution with interrupts and mailbox
int main(void){ 
	//TExaS_Init(ADC0_AIN1_PIN_PE2, UART0_Emulate_Nokia5110_NoScope);
	//TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
	TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	//PLL_Init();
	ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
	SysTick_Init(2000000);  // 40Hz, 25ms
	ADCdata=0; counter=0; 
	Distance=0;
	error = 0;
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();

// initialize ADC0, channel 1, sequencer 3
// initialize Nokia5110 LCD (optional)
// initialize SysTick for 40 Hz interrupts
// initialize profiling on PF1 (optional)

 // EnableInterrupts();
// print a welcome message  (optional)
  while(1){ 
// read mailbox
// output to Nokia5110 LCD (optional)
		
		if (Flag){
			//ADCdata = ADC0_In();
			Nokia5110_Clear();
			Nokia5110_SetCursor(0, 0);
			Distance = Convert(ADCdata);
			UART_ConvertDistance(Distance); // from Lab 11
			//UART_OutString(String);    // output to Nokia5110 LCD (optional)
			Nokia5110_OutString(String);
			Flag=0;
		}
  }
}


//-----------------------UART_ConvertUDec-----------------------
// Converts a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: store the conversion in global variable String[10]
// Fixed format 4 digits, one space after, null termination
// Examples
//    4 to "   4 "  
//   31 to "  31 " 
//  102 to " 102 " 
// 2210 to "2210 "
//10000 to "**** "  any value larger than 9999 converted to "**** "
void UART_ConvertUDec(unsigned long n){
// as part of Lab 11 implement this function


	signed long cnt = 0;
	
	// clear all contents of String global variable.
	for (cnt=0;cnt<8;cnt++){
		String[cnt]=0;
		
	}
	cnt=0;
	if (n > 999 && n<=9999){
		cnt=3;
		while (cnt != -1){
			String[cnt] = n%10+0x30;
			n = n/10;
			cnt--;
		}
	}
	else if(n>99 && n<=999){ 
		String[0] = ' ';
		cnt = 3;
		while (cnt > 0){
			String[cnt] = n%10+0x30;
			n = n/10;
			cnt--;
		}
		
	}
	else if(n>9 && n <= 99){ 
		String[0] = ' '; 
		String[1] = ' '; 
		cnt = 3;
		while (cnt > 1){
			String[cnt] = n%10+0x30;
			n = n/10;
			cnt--;
		}		
	}
	else if(n <=9){ 
	// else if(n>=0 && n <=9){
		String[0] = ' '; 
		String[1] = ' '; 
		String[2] = ' '; 
		String[3] = n%10+0x30;	
	}
	else{
		while (cnt < 4){
			String[cnt] = '*';
			cnt++;
		}
	}	
	String[4] = ' ';										// review modification
	String[5] = 0;											// review modification
	// for (cnt=4; cnt<8; cnt++){
	// 	String[cnt] = ' ';
	// }

}


//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 implement this function
	unsigned long cnt=0;
	UART_ConvertUDec(n);

	while (cnt < 4){
		if (String[cnt] == ' '){
			String[cnt] = '0';
		}
		cnt++;
	}
	cnt = 4;
	while (cnt > 1){
		String[cnt] = String[cnt-1];
		cnt--;
		
	}
	String[1]='.';
	String[5]=' ';
	String[6]='c';
	String[7]='m';
  
}

/*
void UART_OutString(unsigned char buffer[]){
// as part of Lab 11 implement this function
	unsigned long idx=0;
	
	while (buffer[idx] != 0){
		UART_OutChar(buffer[idx]);
		idx++;
	}
}
*/
