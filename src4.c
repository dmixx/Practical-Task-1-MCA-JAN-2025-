#include <stm32f4xx.h>
#include  <system_stm32f4xx.h>

// Declare external assembly functions
extern void init_gpio_led2_odr_asm(void);
extern void gpio_led2_on_odr_asm(void);

int main(void) {
    init_gpio_led2_odr_asm(); // Initialize GPIOB for LED2

    while (1) {
        gpio_led2_on_odr_asm(); // Keep LED2 ON (PB13 HIGH)
    }
}
