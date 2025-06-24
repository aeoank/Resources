# ESP32 + BN-220 GPS Module Project

This project demonstrates how to interface an **ESP32** with a **BN-220 GPS module** using the [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus) library and [ESPSoftwareSerial](https://github.com/plerup/espsoftwareserial). The example reads NMEA data from the GPS module and displays interpreted information such as latitude, longitude, altitude, satellites, date, time, and more.

## Hardware Connections

| BN-220 Pin | ESP32 Pin |
|------------|-----------|
| VIN        | 3V3       |
| GND        | GND       |
| RX         | GPIO 17   |
| TX         | GPIO 16   |

- **VIN**: Connect to ESP32 3.3V output.
- **GND**: Connect to ESP32 GND.
- **RX**: Connect to ESP32 GPIO 17 (TX pin for SoftwareSerial).
- **TX**: Connect to ESP32 GPIO 16 (RX pin for SoftwareSerial).

> **Note:** The BN-220 operates at 3.3V logic. Do **not** connect VIN to 5V!

## Libraries Required

- [TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)
- [ESPSoftwareSerial](https://github.com/plerup/espsoftwareserial) (install from Arduino Library Manager as "ESPSoftwareSerial" by Peter Lerup)

## Example Code

```cpp
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h> // Use ESPSoftwareSerial library for ESP32

// If using PlatformIO or Arduino IDE, install "ESPSofwareSerial" by Peter Lerup

static const int RXPin = 16, TXPin = 17; // Change these pins as per your wiring
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin); // This is ESPSoftwareSerial

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println(F("FullExample.ino (ESP32 SoftwareSerial)"));
  Serial.println(F("An extensive example of many interesting TinyGPSPlus features"));
  Serial.print(F("Testing TinyGPSPlus library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
  Serial.println(F("Sats HDOP  Latitude   Longitude   Fix  Date       Time     Date Alt    Course Speed Card  Distance Course Card  Chars Sentences Checksum"));
  Serial.println(F("           (deg)      (deg)       Age                      Age  (m)    --- from GPS ----  ---- to London  ----  RX    RX        Fail"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------"));
}

void loop()
{
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  printInt(gps.satellites.value(), gps.satellites.isValid(), 5);
  printFloat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printInt(gps.location.age(), gps.location.isValid(), 5);
  printDateTime(gps.date, gps.time);
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  printFloat(gps.course.deg(), gps.course.isValid(), 7, 2);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);
  printStr(gps.course.isValid() ? TinyGPSPlus::cardinal(gps.course.deg()) : "*** ", 6);

  unsigned long distanceKmToLondon =
    (unsigned long)TinyGPSPlus::distanceBetween(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON) / 1000;
  printInt(distanceKmToLondon, gps.location.isValid(), 9);

  double courseToLondon =
    TinyGPSPlus::courseTo(
      gps.location.lat(),
      gps.location.lng(),
      LONDON_LAT, 
      LONDON_LON);

  printFloat(courseToLondon, gps.location.isValid(), 7, 2);

  const char *cardinalToLondon = TinyGPSPlus::cardinal(courseToLondon);

  printStr(gps.location.isValid() ? cardinalToLondon : "*** ", 6);

  printInt(gps.charsProcessed(), true, 6);
  printInt(gps.sentencesWithFix(), true, 10);
  printInt(gps.failedChecksum(), true, 9);
  Serial.println();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    Serial.print(F("********** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid())
  {
    Serial.print(F("******** "));
  }
  else
  {
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
```

## Usage Notes

- Make sure the BN-220 GPS antenna has a clear view of the sky for a fast satellite fix.
- The first fix may take several minutes; subsequent fixes are much faster.
- You may see asterisks (`*`) in the output when the GPS data is not yet valid (before satellite lock).
- Adjust the RX/TX pins in the code if you use different wiring.

## Troubleshooting

- If you see only asterisks or zeros in the output, ensure the GPS module is powered and outdoors.
- Double-check the wiring and baud rate.
- Ensure you are using the **ESPSoftwareSerial** library, not the regular Arduino SoftwareSerial.

## License

This example is provided under the MIT License.
