#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

// OLED Display Configuration (I2C Pins)
#define OLED_I2C_ADDR   0x3C // SSD1306 I2C address
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   32

I2C i2c(PB_9, PB_8);  // SDA = PB9, SCL = PB8
Adafruit_SSD1306_I2c oled(i2c, NC, OLED_I2C_ADDR, SCREEN_WIDTH, SCREEN_HEIGHT); // Use NC if no reset pin

AnalogIn adc(A0); // PA0 - Reads voltage from resistor divider

// Function to measure resistor
float measure_resistor() {
    float Vcc = 3.3;  // Supply voltage
    float Vadc = adc.read() * Vcc;  // Convert ADC reading to voltage
    float Rref = 1000.0;  // Reference resistor value (1kΩ)

    // Calculate unknown resistor using voltage divider formula
    float Runknown = Rref * (Vadc / (Vcc - Vadc));
    
    return Runknown;
}

int main() {
    // Initialize OLED
    oled.begin();
    oled.clearDisplay();
    oled.display();

    while (1) {
        oled.clearDisplay();

        // Display header
        oled.setTextCursor(10, 0);
        oled.printf("Measuring...");

        // Measure resistor
        float R = measure_resistor();

        // Display result
        oled.setTextCursor(10, 20);
        oled.printf("R = %.0f Ohm", R);
        oled.display();

        ThisThread::sleep_for(2s); // Mbed OS delay
    }
}
