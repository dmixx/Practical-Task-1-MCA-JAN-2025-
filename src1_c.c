#include <stm32f4xx.h>
#include  <system_stm32f4xx.h>		// Include this as part of CMSIS

///declare external assembly language functions (in *.s file)

extern void init_gpioa_asm(void);
extern void gpioa_odr_high_asm(void);
extern void gpioa_odr_low_asm(void);


// This program blinks the green LD2 LED on the NUCLEO-F411 board
// The green LED is connected to GPIOA PA5 and is active high



void delay(void) {									// Delay procedure
   int i;
   for (i=0; i< 1000000; i++);
}

int main (void) {
	
	  init_gpioa_asm(); // initialize GPIOA	
		
	//	gpioa_odr_high_asm(); // control the ODR pin of GPIOA high
	//  gpioa_odr_low_asm(); // control the ODR pin of GPIOA	low	
	 
		while(1) {
								gpioa_odr_high_asm(); // make the PA5 pin output high -- turn ON LED
								delay();
											
								gpioa_odr_low_asm(); // make the PA5 pin output low -- turn OFF LED
								delay();
														}
}