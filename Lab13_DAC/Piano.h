// Piano.h
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano, Jonathan Valvano
// December 29, 2014


#define GPIO_PORTE_DATA_R 	(*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R 		(*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R 	(*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R 		(*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R 		(*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R 	(*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R 		(*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R 	(*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R 	(*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R		 	(*((volatile unsigned long *)0x400FE108))

#define PE4 	(*((volatile unsigned long *)0x40024040))
#define PE3 	(*((volatile unsigned long *)0x40024020))
#define PE2   (*((volatile unsigned long *)0x40024010))
#define PE1		(*((volatile unsigned long *)0x40024008))
#define PE0 	(*((volatile unsigned long *)0x40024004))

// no of element = 32
// Period = 80000000/(Freq * no of element)
#define C 4590  // Freq = 523 Hz 4777.82
#define D 4076  // Freq = 587 Hz 4256.55
#define E 3622  // Freq = 659 Hz 3792.159
#define G 3010  // Freq = 784 Hz 3188.81





// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void); 
  

// **************Piano_In*********************
// Input from piano key inputs
// Input: none 

unsigned long Piano_In(void);
void Tester(unsigned long);
