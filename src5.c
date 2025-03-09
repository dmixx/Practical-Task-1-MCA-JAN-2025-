#include <stm32f4xx.h>

// Declare external assembly functions
extern void init_gpio_led2_odr_asm(void);
extern void gpio_led2_on_odr_asm(void); // This will turn LED ON

// C Delay Function
void delay(void) {
    volatile int i; // Declare `i` before the loop
    for (i = 0; i < 500000; i++); 
}

int main(void) {
    init_gpio_led2_odr_asm(); // Initialize GPIOB for LED2

    while (1) {
        gpio_led2_on_odr_asm(); // Turn LED ON (PB13 HIGH)
        delay();                // Wait
        
        // Toggle LED OFF using C (since no OFF assembly function)
        GPIOB->ODR &= ~(1 << 13); // Turn OFF LED (PB13 LOW)
        delay();                  // Wait
    }
}
