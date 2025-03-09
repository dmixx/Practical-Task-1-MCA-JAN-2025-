#include "stm32f4xx.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "font5x7.h"

// -----------------------------------------------------------------------------
// User-configurable parameters
// -----------------------------------------------------------------------------
#define SYSCLK          168000000UL // 168 MHz system clock
#define I2C_FREQ        100000UL    // 100 kHz I2C
#define OLED_ADDR       0x3C        // SSD1306 I2C address (7-bit)

// Known resistor in the voltage divider, in ohms
#define R_KNOWN         1000.0f     // e.g. 1 kO

// Reference voltage (approx. 3.3 V on Nucleo)
#define VREF            3.3f

// ADC resolution
#define ADC_MAX         4095.0f     // 12-bit => 0..4095

// OLED geometry for 128x32
#define OLED_WIDTH      128
#define OLED_HEIGHT     32
#define OLED_PAGES      (OLED_HEIGHT / 8)

// -----------------------------------------------------------------------------
// Display buffer
// -----------------------------------------------------------------------------
static uint8_t buffer[OLED_WIDTH * OLED_PAGES];

// -----------------------------------------------------------------------------
// Simple busy-wait delay
// -----------------------------------------------------------------------------
static void delay_ms(uint32_t ms)
{
    volatile uint32_t count;
    while (ms--)
    {
        count = SYSCLK / 8000;
        while (count--) {
            // do nothing
        }
    }
}

// -----------------------------------------------------------------------------
// I2C1 initialization on PB8=SCL, PB9=SDA
// -----------------------------------------------------------------------------
static void I2C1_Init(void)
{
    // Enable clock for I2C1 and GPIOB
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // PB8, PB9 => AF4, open-drain, pull-up
    GPIOB->MODER &= ~((3UL << (8*2)) | (3UL << (9*2)));
    GPIOB->MODER |=  ((2UL << (8*2)) | (2UL << (9*2))); // AF mode
    GPIOB->OTYPER |= ( (1UL << 8) | (1UL << 9) );       // open-drain
    GPIOB->PUPDR &= ~((3UL << (8*2)) | (3UL << (9*2)));
    GPIOB->PUPDR |=  ((1UL << (8*2)) | (1UL << (9*2))); // pull-up
    GPIOB->AFR[1] &= ~((0xF << ((8-8)*4)) | (0xF << ((9-8)*4)));
    GPIOB->AFR[1] |=  ((4UL << ((8-8)*4)) | (4UL << ((9-8)*4)));

    // Reset I2C1
    RCC->APB1RSTR |=  RCC_APB1RSTR_I2C1RST;
    delay_ms(1);
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    // Disable I2C before config
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Configure for 100 kHz
    I2C1->CR2 = 42;     // APB1 = 42 MHz
    I2C1->CCR = 210;    // 42e6 / (100e3*2) = 210
    I2C1->TRISE = 43;   // 42 + 1

    // Enable I2C
    I2C1->CR1 |= I2C_CR1_PE;
}

// -----------------------------------------------------------------------------
// I2C1 write (blocking)
// -----------------------------------------------------------------------------
static void I2C1_Write(uint8_t addr, const uint8_t *data, uint16_t len)
{
    volatile uint32_t timeout;
    uint16_t i;

    // START
    I2C1->CR1 |= I2C_CR1_START;
    timeout = 100000;
    while (!(I2C1->SR1 & I2C_SR1_SB)) {
        if (--timeout == 0) return;
    }

    // Send address (write)
    I2C1->DR = (addr << 1);
    timeout = 100000;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
        if (--timeout == 0) return;
    }
    (void)I2C1->SR1;
    (void)I2C1->SR2; // clear ADDR

    // Send data
    for (i = 0; i < len; i++) {
        timeout = 100000;
        while (!(I2C1->SR1 & I2C_SR1_TXE)) {
            if (--timeout == 0) return;
        }
        I2C1->DR = data[i];
    }

    // Wait BTF
    timeout = 100000;
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {
        if (--timeout == 0) return;
    }

    // STOP
    I2C1->CR1 |= I2C_CR1_STOP;
}

// -----------------------------------------------------------------------------
// Send a single SSD1306 command
// -----------------------------------------------------------------------------
static void oled_command(uint8_t cmd)
{
    uint8_t packet[2];
    packet[0] = 0x00; // control byte => command
    packet[1] = cmd;
    I2C1_Write(OLED_ADDR, packet, 2);
}

// -----------------------------------------------------------------------------
// Update display from 'buffer'
// -----------------------------------------------------------------------------
static void update_display(void)
{
    uint8_t page;
    for (page = 0; page < OLED_PAGES; page++)
    {
        uint8_t packet[1 + OLED_WIDTH];
				oled_command(0xB0 + page); // set page
        oled_command(0x00);        // lower col = 0
        oled_command(0x10);        // upper col = 0

        // data packet
        packet[0] = 0x40; // control byte => data
        memcpy(&packet[1], &buffer[page * OLED_WIDTH], OLED_WIDTH);

        I2C1_Write(OLED_ADDR, packet, 1 + OLED_WIDTH);
    }
}

// -----------------------------------------------------------------------------
// Basic SSD1306 init for 128x32
// -----------------------------------------------------------------------------
static void oled_init(void)
{
    // Minimal reset if you have an OLED reset pin (PD4, etc.). Skipped here.

    static const uint8_t init_cmds[] = {
        0xAE,       // display off
        0xD5, 0x80, // clock div
        0xA8, 0x1F, // multiplex = 32
        0xD3, 0x00, // display offset
        0x40,       // start line
        0x8D, 0x14, // charge pump
        0x20, 0x00, // memory mode = horizontal
        0xA1,       // seg remap
        0xC8,       // COM scan dec
        0xDA, 0x02, // COM pins
        0x81, 0x8F, // contrast
        0xD9, 0xF1, // pre-charge
        0xDB, 0x40, // vcom detect
        0xA4,       // resume
        0xA6,       // normal
        0xAF        // display on
    };
    uint32_t i;
    for (i = 0; i < sizeof(init_cmds); i++) {
        oled_command(init_cmds[i]);
    }
    delay_ms(100);
}


// -----------------------------------------------------------------------------
// Clear the display buffer
// -----------------------------------------------------------------------------
static void clear_buffer(void)
{
    memset(buffer, 0, sizeof(buffer));
}

// -----------------------------------------------------------------------------
// Draw a single character at (x, y)
// -----------------------------------------------------------------------------
static void draw_char(uint8_t x, uint8_t y, char c) {
    uint8_t index;
    int col, row;
    int px, py;
    int page, bit;
    uint8_t colData;

    if (c < 32 || c > 127) {
        return;
    }

    index = (uint8_t)(c - 32);

    for (col = 0; col < 5; col++) {
        colData = font5x7[index][col];
        for (row = 0; row < 7; row++) {
            if (colData & (1 << row)) {
                px = x + col;
                py = y + row;
                if (px >= OLED_WIDTH || py >= OLED_HEIGHT) {
                    continue;
                }
                page = py / 8;
                bit = py % 8;
                buffer[page * OLED_WIDTH + px] |= (1 << bit);
            }
        }
    }
}


// -----------------------------------------------------------------------------
// Draw a text string at (x, y)
// -----------------------------------------------------------------------------
static void draw_text(uint8_t x, uint8_t y, const char *text)
{
    while (*text) {
        draw_char(x, y, *text);
        x += 6; // 5 wide + 1 spacing
        text++;
    }
}

// -----------------------------------------------------------------------------
// ADC Setup for PA0
// -----------------------------------------------------------------------------
static void ADC1_Init(void)
{
    // Enable clock for ADC1 and GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA0 => analog mode
    GPIOA->MODER |= (3UL << (0 * 2)); // 11 => analog

    // Single conversion, 12-bit, SW trigger
    // ADC Common config (if needed) => default single mode
    ADC->CCR = 0;
    // ADC1 => channel 0
    ADC1->SQR3 = 0; // channel 0 in first conversion
    // continuous or single? Let's do single for demonstration
    ADC1->CR2 = 0;
    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;
    delay_ms(1);
}

// -----------------------------------------------------------------------------
// ADC1_Read: do one conversion, return 12-bit result
// -----------------------------------------------------------------------------
static uint16_t ADC1_Read(void)
{
    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
    // Wait EOC
    while (!(ADC1->SR & ADC_SR_EOC)) {
        // spin
    }
    return (uint16_t)(ADC1->DR & 0xFFF);
}

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(void)
{
    char displayStr[32];
    SystemInit();

    // 1) Initialize I2C, OLED
    I2C1_Init();
    oled_init();
    clear_buffer();
    update_display();

    // 2) Initialize ADC for measuring the midpoint of the voltage divider
    ADC1_Init();

    while (1)
    {
        // 3) Read ADC => convert to voltage
        uint16_t adcVal = ADC1_Read();
        float v_meas = (adcVal / ADC_MAX) * VREF;

        // 4) Compute R_x from the voltage divider formula
        // R_x = R_known * (v_meas / (VREF - v_meas))
        // If v_meas is close to 3.3, that means R_x >> R_known, watch for division by zero.
        // If v_meas is 0, that means R_x=0. In practice, ensure resistor range is correct.
        float r_x = 0.0f;
        if (v_meas < (VREF - 0.01f)) { // avoid near-division by zero
            r_x = R_KNOWN * (v_meas / (VREF - v_meas));
        } else {
            // If it's extremely close to 3.3, the resistor is very large (or open circuit)
            r_x = 999999.0f; // dummy large
        }

        // 5) Display result on the OLED
        // Clear screen
        clear_buffer();

        // For example: "R = 995.12 ohms"
        sprintf(displayStr, "R = %.2f ohms", r_x);
        draw_text(0, 0, "Resistor Value:");
        draw_text(0, 16, displayStr);

        update_display();

        delay_ms(1000);
    }
}