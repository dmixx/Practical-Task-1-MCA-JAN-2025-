// This program blinks the green LD2 LED on the NUCLEO-F411RE board
// The green LD2 LED is connected to GPIOA PA5 and is active high

#include "stm32f4xx.h" // Include CMSIS

// Reset and Clock Control for AHB1
#define RCC_AHB1ENR (*(volatile unsigned int *)(0x40023830))
#define GPIOA_MODER (*(volatile unsigned int *)(0x40020000))
#define GPIOA_OTYPER (*(volatile unsigned int *)(0x40020004))
#define GPIOA_OSPEEDR (*(volatile unsigned int *)(0x40020008))
#define GPIOA_BSRR (*(volatile unsigned int *)(0x40020018))

#define RCC_A_ENABLE 0x00000001
#define PA5_OUT (1 << (5 * 2))  // Set PA5 as output
#define LED_ILLUM (1 << 5)      // Set PA5 high
#define LED_DEILLUM (1 << (5 + 16)) // Reset PA5

#define DELAY_TIME 40000

void delay(int n) {
    volatile int i; // Declare separately
    for (i = 0; i < n; i++); // Simple loop delay
}

int main(void) {
    // Enable clocks to GPIOA
    RCC_AHB1ENR |= RCC_A_ENABLE; 

    // Configure PA5 as output without affecting other pins
    GPIOA_MODER &= ~(3 << (5 * 2)); // Clear existing mode bits
    GPIOA_MODER |= PA5_OUT;         // Set PA5 as output mode

    while (1) {
        GPIOA_BSRR = LED_ILLUM;  // Turn LED on
        delay(DELAY_TIME);
        GPIOA_BSRR = LED_DEILLUM; // Turn LED off
        delay(DELAY_TIME);
    }
}
