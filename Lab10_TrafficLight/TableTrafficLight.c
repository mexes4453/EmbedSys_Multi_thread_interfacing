// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016
// UDOH ALBERT

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

#define SENSOR 						   (*((volatile unsigned long *)0x4002401C))						// switches PORTE
#define TRAFFIC_LIGHT  		   (*((volatile unsigned long *)0x400053FC))						// PORTB
#define PEDESTRIAN_LIGHT     (*((volatile unsigned long *)0x40025038))						// PORTF
#define SYSCTL_RCGC2_R       (*((volatile unsigned long *)0x400FE108))

#define GPIO_PORTF_AMSEL_R   (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTB_AMSEL_R   (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTE_AMSEL_R   (*((volatile unsigned long *)0x40024528))
	
#define GPIO_PORTF_AFSEL_R   (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTB_AFSEL_R   (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTE_AFSEL_R   (*((volatile unsigned long *)0x40024420))
	
#define GPIO_PORTF_PUR_R     (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTB_PUR_R     (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTE_PUR_R     (*((volatile unsigned long *)0x40024510))
	
#define GPIO_PORTF_PCTL_R    (*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTB_PCTL_R    (*((volatile unsigned long *)0x4000552C))
#define GPIO_PORTE_PCTL_R    (*((volatile unsigned long *)0x4002452C))
	
#define GPIO_PORTF_LOCK_R    (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTB_LOCK_R    (*((volatile unsigned long *)0x40005520))
#define GPIO_PORTE_LOCK_R    (*((volatile unsigned long *)0x40024520))
	
#define GPIO_PORTF_CR_R      (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTB_CR_R      (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTE_CR_R      (*((volatile unsigned long *)0x40024524))

#define GPIO_PORTF_DEN_R     (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTB_DEN_R     (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTE_DEN_R     (*((volatile unsigned long *)0x4002451C))
	
#define GPIO_PORTF_DIR_R     (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTB_DIR_R     (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTE_DIR_R     (*((volatile unsigned long *)0x40024400))

#define NVIC_ST_CTRL_R       (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R     (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R    (*((volatile unsigned long *)0xE000E018))
	
#define goW     0
#define waitW   1
#define goS     2
#define waitS   3
#define walkP   4
#define hurryP1 5
#define hurryP2 6
#define hurryP3 7
#define hurryP4 8
#define hurryP5 9
#define hurryP6 10

struct state{
	unsigned long PBOut;				// 6 bit pattern out for traffic light PORTB
	unsigned long PFOut;				// 2 bit pattern out for Pedestrian light PORTF
	unsigned long delayTime;		// delay time in 10ms
	unsigned long next[8];			// next state for the inputs (8 inputs from 3 switches as sensors)

};

typedef const struct state sTyp;

sTyp FSM[11] = {
	{0x0C, 0x02, 600, {goW,         goW,   waitW,   waitW,     waitW,   waitW,   waitW,   waitW}},
	{0x14, 0x02, 200, {goS,         goW,     goS,     goS,     walkP,   walkP,   walkP,     goS}},
	{0x21, 0x02, 600, {goS,       waitS,     goS,   waitS,     waitS,   waitS,   waitS,   waitS}},
	{0x22, 0x02, 200, {waitS,       goW,     goS,     goW,     walkP,   walkP,   walkP,   walkP}},
	{0x24, 0x08, 300, {hurryP1, hurryP1, hurryP1, hurryP1,   hurryP1, hurryP1, hurryP1, hurryP1}},
	{0x24, 0x02, 100, {hurryP2, hurryP2, hurryP2, hurryP2,   hurryP2, hurryP2, hurryP2, hurryP2}},
	{0x24, 0x00, 100, {hurryP3, hurryP3, hurryP3, hurryP3,   hurryP3, hurryP3, hurryP3, hurryP3}},
	{0x24, 0x02, 100, {hurryP4, hurryP4, hurryP4, hurryP4,   hurryP4, hurryP4, hurryP4, hurryP4}},
	{0x24, 0x00, 100, {hurryP5, hurryP5, hurryP5, hurryP5,   hurryP5, hurryP5, hurryP5, hurryP5}},
	{0x24, 0x02, 100, {hurryP6, hurryP6, hurryP6, hurryP6,   hurryP6, hurryP6, hurryP6, hurryP6}},
	{0x24, 0x00, 100, {goW,         goW,     goS,     goW,   walkP,     goW,     goS,     goW}}		

};
// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Port_Init(void);
void SysTick_Init(void);
void SysTick_Wait10ms(unsigned long);
void SysTick_Wait(unsigned long);

// ***** 3. Subroutines Section *****
unsigned long s;  				// index to the current state
unsigned long input; 

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
	Port_Init();
	SysTick_Init();
	
	s = goW;			// Initialise default state
  while(1){
		// GPIO_PORTF_DATA_R = 0x02; SysTick_Wait10ms(100);
		TRAFFIC_LIGHT = FSM[s].PBOut;					//	set traffic lights for cars on both WEST & SOUTH
		GPIO_PORTF_DATA_R = FSM[s].PFOut;			//	set light for pedestrian
		SysTick_Wait10ms(FSM[s].delayTime);		// 	wait time defined for current state
		input = SENSOR;												//	read inputs from sensors/switches
		s = FSM[s].next[input];								//	move to next state based on current state and input
     
  }
}


void Port_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R = 0x32;
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;					// unlock port for changes
	GPIO_PORTB_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x0A;									// specific port bits for changes
	GPIO_PORTB_CR_R = 0xFF;
	GPIO_PORTE_CR_R = 0x07;
	GPIO_PORTF_AMSEL_R = 0x00;							// disable analog function
	GPIO_PORTB_AMSEL_R = 0x00;
	GPIO_PORTE_AMSEL_R = 0x00;
	GPIO_PORTF_AFSEL_R = 0x00;							// disable alternate function
	GPIO_PORTB_AFSEL_R = 0x00;
	GPIO_PORTE_AFSEL_R = 0x00;
	GPIO_PORTF_PUR_R = 0x00;								// disable pull-up resistors
	GPIO_PORTB_PUR_R = 0x00;
	GPIO_PORTE_PUR_R = 0x00;
	GPIO_PORTF_PCTL_R = 0x00;								// disable peripheral signal for port bits
	GPIO_PORTB_PCTL_R = 0x00;
	GPIO_PORTE_PCTL_R = 0x00;
	GPIO_PORTF_DEN_R = 0x0A;								// enable digital signal function
	GPIO_PORTB_DEN_R = 0xFF;
	GPIO_PORTE_DEN_R = 0x07;
	GPIO_PORTF_DIR_R |= 0x0A;								// set port bits as output || pedestrian light
	GPIO_PORTB_DIR_R |= 0xFF;								// set port bits as output || cars (South - North / East - West) light
	GPIO_PORTE_DIR_R &= ~0x07;							// set port bits as input  || Switch as input sensors 

}



void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
