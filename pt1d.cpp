#include "mbed.h"
#include "Adafruit_SSD1306.h"

// Define I2C pins for OLED (PB_9 = SDA, PB_8 = SCL)
I2C i2c(PB_9, PB_8);
Adafruit_SSD1306_I2c oled(i2c, D13, 0x78, 32, 128);  // OLED Display (D13 is reset pin)

// Define Analog Input pin for battery voltage
AnalogIn voltageSensor(A0);  // Changed from A0 to A1

int main() {
    // Initialize OLED display
    oled.begin();
    oled.clearDisplay();
    oled.setTextCursor(0, 0);
    oled.printf("VOLTMETER");
    oled.display();

    while (1) {
        // Read voltage from A1 and convert to actual voltage
        float voltage = voltageSensor.read() * 3.3;

        // Print to serial monitor for debugging
        int voltageInt = voltage * 100;  // Convert to integer representation (e.g., 3.25V → 325)
        printf("Voltage: %d.%02d V\n", voltageInt / 100, voltageInt % 100);
        fflush(stdout);  // Ensure output is printed immediately

        // Display voltage on OLED
        oled.clearDisplay();
        oled.setTextCursor(0, 0);
        oled.printf("VOLTMETER\n%d.%02d V", voltageInt / 100, voltageInt % 100);
        oled.display();

        // Delay for 500ms
        ThisThread::sleep_for(500ms);
    }
}
