# GSM SMS Sender with ESP32 and SIM7670

This project demonstrates how to send SMS messages using an ESP32 microcontroller and a SIM7670 GSM module. The Arduino sketch initializes the GSM module, configures it for SMS in text mode, and sends a predefined SMS message to a specified phone number.

## Hardware Used

- ESP32 Development Board
- SIM7670 GSM Module

## Circuit Diagram

Below is the wiring diagram for connecting the ESP32 to the SIM7670 module:

![image1](image1)

### Connections

| ESP32 Pin | SIM7670 Pin | Description           |
|-----------|-------------|-----------------------|
| 3V3       | VCC         | Power Supply          |
| GND       | GND         | Ground                |
| GPIO 16   | TXD         | ESP32 RX (GSM TX)     |
| GPIO 17   | RXD         | ESP32 TX (GSM RX)     |

> **Note:** Double-check your module's pinout, as labeling can vary by board version.

## Getting Started

### Prerequisites

- Arduino IDE installed
- ESP32 board package added to Arduino IDE
- SIM card with SMS capability (inserted into the SIM7670)
- Required libraries: none extra (uses built-in `HardwareSerial`)

### Code

Upload the provided `GSM_ESP32.ino` sketch to your ESP32.

#### Key Code Snippet

```cpp
#include <HardwareSerial.h>

#define MODEM_RX 16 // ESP32 RX pin (connect to SIM7670 TX)
#define MODEM_TX 17 // ESP32 TX pin (connect to SIM7670 RX)
HardwareSerial GSM(2);

const char* PHONE_NUMBER = "+1234567890"; // <-- PUT RECIPIENT'S PHONE NUMBER HERE

void sendCommand(const char* cmd, unsigned long waitMs = 500, bool showOutput = true) {
  GSM.println(cmd);
  delay(waitMs);
  if (showOutput) {
    while (GSM.available()) {
      Serial.write(GSM.read());
    }
  }
}

void setup() {
  Serial.begin(115200);
  GSM.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  sendCommand("AT");
  sendCommand("ATE0");
  sendCommand("AT+CMGF=1");

  sendCommand("AT+CPIN?");
  sendCommand("AT+CSQ");
  sendCommand("AT+CREG?");

  GSM.print("AT+CMGS=\"");
  GSM.print(PHONE_NUMBER);
  GSM.println("\"");
  delay(1000);

  GSM.print("hi, how are you ?");
  GSM.write(26);

  delay(5000);
  while (GSM.available()) {
    Serial.write(GSM.read());
  }
}

void loop() { }
```

### Usage

1. Wire the ESP32 and SIM7670 as shown in the diagram above.
2. Insert a SIM card with SMS capability into the SIM7670.
3. Open the Arduino IDE, load `GSM_ESP32.ino`, and update `PHONE_NUMBER` with your recipient's phone number (including country code).
4. Upload the sketch to your ESP32.
5. Open the Serial Monitor (115200 baud) to view debug output.
6. The ESP32 will initialize the SIM7670 and send an SMS to the specified number.

## Troubleshooting

- Ensure SIM card is active and unlocked (no PIN required or set PIN in code).
- Check wiring, especially TX/RX crossover.
- Use external power for SIM7670 if you observe instability.

## License

This project is open source and free to use for educational and hobbyist purposes.

---
**Author:** [Your Name]  
**Repository:** [aeoank/Resources](https://github.com/aeoank/Resources)