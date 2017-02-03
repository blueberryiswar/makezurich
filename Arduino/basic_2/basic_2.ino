#include <rn2xx3.h>
#include <SoftwareSerial.h>
// include the library code:
#include <LiquidCrystal.h>

SoftwareSerial mySerial(7, 8); // RX, TX
#define RST  2

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 6);

// Copy the following lines from TTN Console -> Devices -> Overview tab -> "EXAMPLE CODE"
const char *devAddr = "26011FB4";
const char *nwkSKey = "7F17AB5D97EB48CCDB2D0EEDEF964FBA";
const char *appSKey = "88D1E8B5A931400CD10B344E104DEF20";

rn2xx3 myLora(mySerial);

// Setup routine runs once when you press reset
void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Trashmob rocks");

  
  pinMode(13, OUTPUT);
  led_on();

  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  mySerial.begin(9600);
  Serial.println("Startup");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 1);
  // Print a message to the LCD.
  lcd.print("Trashmob boots up:");

  lcd.begin(16, 2);
  lcd.print("Starting Lora");

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

  lcd.begin(16, 2);
  lcd.print("Lora started");

  led_off();
  delay(2000);
}

// the loop routine runs over and over again forever:
void loop() {
  send_data_to_lora();

  delay(20000);
}

void send_data_to_lora() {
  led_on();
  lcd.begin(16, 2);
  lcd.print("Sending to Lora...");
  Serial.println("TXing");
  myLora.txUncnf("trashmob.ch was here");
  led_off();
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
