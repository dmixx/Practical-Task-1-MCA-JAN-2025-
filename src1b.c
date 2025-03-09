#include <stm32f4xx.h>

extern void init_i2c_asm(void);
extern void oled_init_asm(void);
extern void oled_print_asm(void);

void delay(void) {
    volatile int i; 
    for (i = 0; i < 1000000; i++);
}

int main(void) {
    RCC->AHB1ENR |= (1 << 0);  // Enable GPIOA clock
    GPIOA->MODER |= (1 << (5 * 2));  // Set PA5 as output

    init_i2c_asm();  // Initialize I2C1
    oled_init_asm(); // Initialize OLED

    while (1) {
        GPIOA->ODR ^= (1 << 5);  // Toggle LED before printing
        delay();

        oled_print_asm(); // Attempt to display text

        GPIOA->ODR ^= (1 << 5);  // Toggle LED after printing
        delay();
    }
}
