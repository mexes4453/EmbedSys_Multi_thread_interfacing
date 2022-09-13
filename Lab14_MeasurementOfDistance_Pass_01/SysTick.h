#define NVIC_ST_CTRL_R 		    (*((volatile unsigned long *)0xE000E010)) // System clock reg
#define NVIC_ST_RELOAD_R 		  (*((volatile unsigned long *)0xE000E014))	// System clock reload reg
#define NVIC_ST_CURRENT_R 		(*((volatile unsigned long *)0xE000E018)) // System clock current reg
#define NVIC_SYS_PRI3_R 		  (*((volatile unsigned long *)0xE000ED20))	// System clock interrupt priority reg SysTick	


void SysTick_Init(unsigned long);
