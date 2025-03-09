#include <stm32f4xx.h>
#include  <system_stm32f4xx.h>

// Declare external assembly functions (from .s files)
extern void int_gpioa_asm(void);
extern void gpioa_bsrr_high_asm(void);
extern void gpioa_bsrr_low_asm(void);

// Delay function
void delay(void) {
    int i; //  Declare outside the loop
    for (i = 0; i < 1000000; i++); 

}

int main(void) {
    int_gpioa_asm(); // Initialize GPIOA

    while (1) {
        gpioa_bsrr_high_asm(); // Turn ON LED (PA5 HIGH)
        delay();

        gpioa_bsrr_low_asm(); // Turn OFF LED (PA5 LOW)
        delay();
    }
}
