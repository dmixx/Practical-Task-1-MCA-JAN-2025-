#include <stm32f4xx.h>

// Declare external assembly functions
extern void init_gpio_led2_asm(void);
extern void gpio_led2_on_asm(void);

int main(void) {
    init_gpio_led2_asm(); // Initialize GPIOC for LED2

    while (1) {
        gpio_led2_on_asm(); // Keep LED2 ON (PC13 HIGH)
    }
}
