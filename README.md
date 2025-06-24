# ESP32 + Waveshare Round Capacitive Fingerprint Sensor

This project demonstrates how to interface the **Waveshare Round Capacitive Fingerprint Sensor** with an ESP32. You can enroll, verify, and clear fingerprints, as well as manage the module's power states and sensitivity.

## Hardware Connections

| Fingerprint Sensor Pin | ESP32 Pin |
|-----------------------|-----------|
| VIN                   | 3V3       |
| GND                   | GND       |
| TX                    | D16       |
| RX                    | D17       |
| EN                    | D33       |
| IQR                   | D14       |

> **Note:**  
> - "D16", "D17", etc. refer to GPIO16, GPIO17, etc. on the ESP32.

## Features

- Enroll new fingerprints (up to 500 users).
- Verify fingerprints and identify stored users.
- Clear all enrolled fingerprints.
- Enter and wake from sleep mode.
- Adjustable matching sensitivity (compare level).
- UART-based command interface.

## Usage Instructions

1. **Wiring:**  
   Connect the sensor to the ESP32 as per the table above.

2. **Upload the Code:**  
   Use the Arduino IDE or PlatformIO to upload the provided code to your ESP32.

3. **Serial Commands:**  
   After starting, open the Serial Monitor at 115200 baud.  
   The module listens for specific serial commands:
   - `CMD1`: Query the number of registered fingerprints
   - `CMD2`: Add a new fingerprint (requires two scans)
   - `CMD3`: Verify a fingerprint
   - `CMD4`: Clear all fingerprints
   - `CMD5`: Enter sleep mode (only CMD6 is valid in this state)
   - `CMD6`: Wake up the module

   *To send a command, type "CMD" followed by the number (e.g., `CMD2`) in the serial monitor.*

4. **Sleep & Wake:**  
   - Enter sleep mode with `CMD5`.
   - Wake up with `CMD6` or by touching the sensor (if configured for auto-wake).

## Example Code

```cpp
// See main.cpp or the code below for the full implementation.
#include <stdio.h>

#define Finger_RST_Pin 14
#define Finger_WAKE_Pin 33
// ... rest of the code as provided ...
```

## Notes

- The sensor uses UART at 19200 baud (`Serial1.begin(19200, SERIAL_8N1, 16, 17)`).
- Power the sensor with 3.3V from the ESP32's 3V3 pin.
- The code can be extended or integrated with other systems (e.g., relays, access control, etc.).
- Carefully check the wiring and voltage before powering up.

## References

- [Waveshare Round Capacitive Fingerprint Sensor Wiki](https://www.waveshare.com/wiki/Round_Capacitive_Fingerprint_Sensor)
- [ESP32 Arduino Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [Waveshare Product Page](https://www.waveshare.com/round-capacitive-fingerprint-sensor.htm)

---

**Author:** Aeonix
