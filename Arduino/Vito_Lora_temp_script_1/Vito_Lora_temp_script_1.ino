#include <Adafruit_BME280.h>

#include <rn2xx3.h>
#include <SoftwareSerial.h>

//from bmetest
#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//from betest end

SoftwareSerial mySerial(7, 8); // RX, TX
#define RST  2

// Copy the following lines from TTN Console -> Devices -> Overview tab -> "EXAMPLE CODE"
const char *devAddr = "2601177B";
const char *nwkSKey = "C5E8BBAF3DC485A90648613F9E78EED6";
const char *appSKey = "B158487E5FF94EF4632E02422AE0C27D";

rn2xx3 myLora(mySerial);

// Setup routine runs once when you press reset
void setup() {
  pinMode(13, OUTPUT);
  led_on();

  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  mySerial.begin(9600);
  Serial.println("Startup");

  //from bmetest
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  //from bmetest end

  // Reset rn2xx3
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
  digitalWrite(RST, LOW);
  delay(500);
  digitalWrite(RST, HIGH);

  // Initialise the rn2xx3 module
  myLora.autobaud();

  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.print("Microchip RN2xx3 version number: ");
  Serial.println(myLora.sysver());

  myLora.initABP(devAddr, appSKey, nwkSKey);

  led_off();
  delay(2000);
}

// the loop routine runs over and over again forever:
void loop() {
  led_on();
  Serial.println("TXing");

  myLora.txUncnf("Temperature: "+String(float(bme.readTemperature())));
  myLora.txUncnf("Pressure: "+String(float(bme.readPressure() / 100.0F)));
  myLora.txUncnf("Approx. Altitude: "+String(float(bme.readAltitude(SEALEVELPRESSURE_HPA))));
  myLora.txUncnf("Humidity: "+String(float(bme.readHumidity())));

  led_off();

  delay(20000);
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}

