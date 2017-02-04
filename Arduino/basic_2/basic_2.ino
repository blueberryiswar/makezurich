#include <rn2xx3.h>
#include <SoftwareSerial.h>
// include the library code:
#include <LiquidCrystal.h>
#include <TinyGPS.h>
TinyGPS gps;
SoftwareSerial nss(14, 15); // RX, TX
SoftwareSerial mySerial(7, 8); // RX, TX
#define RST  2

#define  c     3830    // 261 Hz
#define  d     3400    // 294 Hz
#define  e     3038    // 329 Hz
#define  f     2864    // 349 Hz
#define  g     2550    // 392 Hz
#define  a     2272    // 440 Hz
#define  b     2028    // 493 Hz
#define  C     1912    // 523 Hz
// Define a special note, 'R', to represent a rest
#define  R     0

// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;
int myCount = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 6);

// Copy the following lines from TTN Console -> Devices -> Overview tab -> "EXAMPLE CODE"
const char *devAddr = "26011FB4";
const char *nwkSKey = "7F17AB5D97EB48CCDB2D0EEDEF964FBA";
const char *appSKey = "88D1E8B5A931400CD10B344E104DEF20";

rn2xx3 myLora(mySerial);

// constants won't change. They're used here to
// set pin numbers:
const int buttonPin = 10;     // the number of the pushbutton pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

// Setup routine runs once when you press reset
void setup() {
  pinMode(speakerOut, OUTPUT);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Trashmob rocks");
  delay(1000);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  
  pinMode(13, OUTPUT);
  led_on();

  unsigned long fix_age;
  long lat, lon;

  if (nss.available()) {
    int cgps = nss.read();
    if (gps.encode(cgps)) {
      // process new gps info here
      // retrieves +/- lat/long in 100000ths of a degree
      gps.get_position(&lat, &lon, &fix_age);
    }
  }

  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  mySerial.begin(9600);
  Serial.println("Startup");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 1);
  // Print a message to the LCD.
  lcd.print("Trashmob boots up:");
  delay(500);
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

  lcd.begin(16, 2);
  lcd.print("Latitude:");
  delay(600);
  lcd.begin(16, 2);
  lcd.print(lat);
  delay(1500);
  lcd.begin(16, 2);
  lcd.print("Longitude:");
  delay(600);
  lcd.begin(16, 2);
  lcd.print(lon);
  delay(1500);
  String slat = String(lat);
  String slon = String(lon);
  myLora.txUncnf(slat);
  myLora.txUncnf(slon);
  lcd.begin(16, 2);
  lcd.print("Press Button");
}

// the loop routine runs over and over again forever:
void loop() {

   // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // send Lora Data
     playMelody();
     send_data_to_lora();
     myCount = myCount + 1;
     printMyCount();
     delay(1200);
     printPressButton();
  }
}

void send_data_to_lora() {
  led_on();
  lcd.begin(16, 2);
  lcd.print("Sending to Lora...");
  Serial.println("TXing");
  String sMyCount = String(myCount);
  myLora.txUncnf(sMyCount);
  led_off();
}

void printPressButton() {
  lcd.begin(16, 2);
  lcd.print("Press Button");
}

void printMyCount() {
  lcd.begin(16, 1);
  lcd.print(myCount);
}


void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}


// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[],
//  which sets each note's relative length (higher #, longer note)
int melody[] = {  c,   f,   C,   R  };
int beats[]  = {  8,   16,  32,  64 };
int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    }
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                
}


void playMelody() {
  for (int i=0; i<MAX_COUNT; i++) {
    tone_ = melody[i];
    beat = beats[i];

    duration = beat * tempo; // Set up timing

    playTone();
    // A pause between notes...
    delayMicroseconds(pause);

  }
}


