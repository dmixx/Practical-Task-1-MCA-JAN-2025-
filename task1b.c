#include "stm32f4xx.h"
#include <string.h>

// OLED Configuration
#define I2C_ADDR    (0x3C << 1)  // SSD1306 I2C address
#define I2C_TIMEOUT 1000         // I2C Timeout
#define I2C_INSTANCE I2C1        // I2C peripheral instance

// Function Prototypes
void I2C1_Init(void);
void oled_command(uint8_t cmd);
void oled_init(void);
void clear_buffer(void);
void draw_char(uint8_t x, uint8_t y, char c);
void draw_text(uint8_t x, uint8_t y, const char *text);
void update_display(void);
void delay_ms(uint32_t ms);

// Buffer for 128x32 OLED
uint8_t buffer[512];

// Full 5x7 ASCII Font Table
static const uint8_t font5x7[96][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // 32: ' '
    {0x00,0x00,0x5F,0x00,0x00}, // 33: '!'
    {0x00,0x07,0x00,0x07,0x00}, // 34: '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // 35: '#'
};

// Initialize I2C Peripheral
void I2C1_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;   // Enable I2C1 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // Enable GPIOB clock

    // Configure PB8 (SCL) and PB9 (SDA) as alternate function (AF4)
    GPIOB->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));  
    GPIOB->MODER |= ((2 << (8 * 2)) | (2 << (9 * 2)));  

    // Set alternate function AF4 for PB8/PB9 (I2C1)
    GPIOB->AFR[1] |= (4 << ((8 - 8) * 4)) | (4 << ((9 - 8) * 4));

    // Configure I2C1
    I2C1->CR1 &= ~I2C_CR1_PE;  // Disable I2C1
    I2C1->CR2 = (I2C_CR2_FREQ & 16);  // Set peripheral clock to 16 MHz
    I2C1->CCR = 80;  // Set clock control for 100kHz
    I2C1->TRISE = 17;  // Set maximum rise time
    I2C1->CR1 |= I2C_CR1_PE;  // Enable I2C1
}

// Send command to OLED
void oled_command(uint8_t cmd) {
    I2C1->CR1 |= I2C_CR1_START;  // Generate start condition
    while (!(I2C1->SR1 & I2C_SR1_SB));  // Wait for start condition

    I2C1->DR = I2C_ADDR;  // Send device address
    while (!(I2C1->SR1 & I2C_SR1_ADDR));  
    (void)I2C1->SR2;  // Clear ADDR flag

    I2C1->DR = 0x00;  // Control byte (Command mode)
    while (!(I2C1->SR1 & I2C_SR1_TXE));  

    I2C1->DR = cmd;  // Send command
    while (!(I2C1->SR1 & I2C_SR1_TXE));  

    I2C1->CR1 |= I2C_CR1_STOP;  // Generate stop condition
}

// Initialize OLED Display
void oled_init(void) {
    static const uint8_t init_cmds[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x1F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x02,
        0x81, 0x8F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
    };

    int i;  // ? Declare `i` before loop to avoid Keil error
    for (i = 0; i < sizeof(init_cmds); i++) {
        oled_command(init_cmds[i]);
    }
}

// Clear OLED buffer
void clear_buffer(void) {
    memset(buffer, 0, sizeof(buffer));
}

// Draw a text string at (x,y)
void draw_text(uint8_t x, uint8_t y, const char *text) {
    while (*text) {
        draw_char(x, y, *text);
        x += 6;
        text++;
    }
}

// Update OLED display from buffer
void update_display(void) {
    for (uint8_t page = 0; page < 4; page++) {
        oled_command(0xB0 + page);  
        oled_command(0x00);  
        oled_command(0x10);  

        I2C1->CR1 |= I2C_CR1_START;  
        while (!(I2C1->SR1 & I2C_SR1_SB));

        I2C1->DR = I2C_ADDR;
        while (!(I2C1->SR1 & I2C_SR1_ADDR));
        (void)I2C1->SR2;

        I2C1->DR = 0x40;  
        while (!(I2C1->SR1 & I2C_SR1_TXE));

        for (uint8_t i = 0; i < 128; i++) {
            I2C1->DR = buffer[page * 128 + i];
            while (!(I2C1->SR1 & I2C_SR1_TXE));
        }

        I2C1->CR1 |= I2C_CR1_STOP;
    }
}

// Delay function for Keil
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

int main(void) {
    I2C1_Init();
    oled_init();
    clear_buffer();

    // ? Corrected function usage
    draw_text(0, 0, "<Programming My>");
    draw_text(0, 10, "<<Nucleo Board>>");

    update_display();

    while (1);
}
