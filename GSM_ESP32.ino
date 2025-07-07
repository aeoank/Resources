#include <HardwareSerial.h>

// Use UART2 on ESP32 (you can use any available UART)
#define MODEM_RX 16 // ESP32 RX pin (connect to SIM7670 TX)
#define MODEM_TX 17 // ESP32 TX pin (connect to SIM7670 RX)
HardwareSerial GSM(2); // UART2

const char* PHONE_NUMBER = "+1234567890"; // <-- PUT RECIPIENT'S PHONE NUMBER HERE, include country code

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

  Serial.println("Initializing SIM7670...");
  delay(3000); // Wait for module to boot

  sendCommand("AT");            // AT command test
  sendCommand("ATE0");          // Echo off
  sendCommand("AT+CMGF=1");     // Set SMS to text mode

  // Optional: check SIM card
  sendCommand("AT+CPIN?");      // SIM ready?
  sendCommand("AT+CSQ");        // Signal quality
  sendCommand("AT+CREG?");      // Network registration

  // Send the SMS
  Serial.print("Sending SMS to ");
  Serial.println(PHONE_NUMBER);

  GSM.print("AT+CMGS=\"");
  GSM.print(PHONE_NUMBER);
  GSM.println("\"");
  delay(1000); // Wait for > prompt

  GSM.print("hi, how are you ?");
  GSM.write(26); // Ctrl+Z to send SMS

  // Show response
  delay(5000);
  while (GSM.available()) {
    Serial.write(GSM.read());
  }
}

void loop() {
  // Nothing to do here
}