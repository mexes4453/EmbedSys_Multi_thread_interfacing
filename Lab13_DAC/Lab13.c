// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 3-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "DAC.h"
#include "Piano.h"
#include "TExaS.h"
#include <stdio.h>
#include "Light.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);

unsigned long GetPianoKeyIndexFromInput(unsigned long Input);
unsigned long GetKeyPeriod(unsigned long KeyPressed);

struct Key{
	unsigned long KeyPosNo;				  
	unsigned long KeyFreq;				  

};

typedef const struct Key PianoKey;

PianoKey PianoKeys[4] = {
	{0x01, C},
	{0x02, D},
	{0x04, E},
	{0x08, G},

};
// variables
unsigned long KeyPressed, period, SwitchState;
unsigned long Index = 0;
unsigned long KeyPressedIndex=0; 

// 4-bit 32-element sine wave
const unsigned short SoundWave[32] = {8,9,11,12,13,14,14,15,15,15,14,14,13,12,11,9,8,7,5,4,3,2,2,1,1,1,2,2,3,4,5,7};


int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
	// PortE used for piano keys, PortB used for DAC   

	KeyPressed = 0; SwitchState=0;
	period =1000;
  Sound_Init(period); // initialize SysTick timer and DAC
  Piano_Init();
	//PortF_init(); // LED light for testing switch
  EnableInterrupts();  // enable after all initialization are done
	

  while(1){                
		// input from keys to select tone
		KeyPressed = Piano_In();								// get value for key pressed
		//delay(50);

		period=GetKeyPeriod(KeyPressed);				// used value of key pressed to get period for note
		//Sound_Tone(period);
		//Sound_Off();
		//delay(1000);
		
		//testing switch
		//SwitchState = GPIO_PORTE_DATA_R&0x0F;
		//if (SwitchState){
  	//		GPIO_PORTE_DATA_R |= 0x08;
		//}
		//Tester(SwitchState);// delay(1000);
  }
            
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){

		Sound_Tone(period);
		Index = (Index+1)&0x1F;	 // index values
		//DAC_Out(SoundWave[Index]);
		if (period > 1){DAC_Out(SoundWave[Index]);} // output values every interrupt
		if (period < 1){DAC_Out(0);} 
		
}

   



unsigned long GetPianoKeyIndexFromInput(unsigned long Input){

	unsigned long keyIndex=0;
	unsigned long x;
	for (x=0; x < 4; x++){
			if (PianoKeys[x].KeyPosNo == Input){
				keyIndex = x;			
			} 
	}
	
	return keyIndex;
	
}


unsigned long GetKeyPeriod(unsigned long KeyPressed){
	
	unsigned long KeyPressedIndex;
	unsigned long period;
	
	if (KeyPressed > 0){
		// get index of the pressed key from piano key array
		KeyPressedIndex = GetPianoKeyIndexFromInput(KeyPressed);
		
		period = PianoKeys[KeyPressedIndex].KeyFreq;		// get freq period
	
	}else{
		period = 0;
	}
	return period;
}

