/*
   What is this called?
   What does it do?
   Who wrote it? and when?
   What equipment do I need?
   What pins are connected to what?
  | Arduino Pin | Device pin |
  | ----------- | ---------- |
  | 0           |            |
  | 1           |            |
  | 2           |            |
  | 3           |            |
  | 5V          |            |
  | GND         |            |
   Do you need to add any certificates?
   Do you need to add any Libraries?
*/

//---------------------------------------------------------------------------------------------------------------------------------------
// Libraries
#include <WiFi101.h>
#include <ArduinoJson.h>
//#include <SPI.h>
//#include <Wire.h>
#include <Servo.h>
#include <TM1637Display.h>
//---------------------------------------------------------------------------------------------------------------------------------------
const unsigned int CHARACTER_LIMIT = 10000; // Limit of characters in HTTP response
//---------------------------------------------------------------------------------------------------------------------------------------
// WiFi Connection
const char* ssid = "-------";          // replace these with the name and password for your local wi-fi network
const char* password = "-------";
//---------------------------------------------------------------------------------------------------------------------------------------
// API
String host = "earthquake.usgs.gov";
String url = "/fdsnws/event/1/query?format=geojson&limit=1";
//---------------------------------------------------------------------------------------------------------------------------------------
// WiFi Client
WiFiSSLClient sslClient;  // HTTPS
WiFiClient client;        // HTTP
char httpResponse[CHARACTER_LIMIT]; // If you are getting responses larger that 10kB, use another request.
//---------------------------------------------------------------------------------------------------------------------------------------
// Timer
unsigned long updateTimer = 300000;
//---------------------------------------------------------------------------------------------------------------------------------------
// Input/output pins
const int buttonPin = 11;    // the number of the pushbutton pin
const int ledPin = 13;       // the pin that the LED is attached to
const int richterPin = 9;    // analogue dial connected to digital pin 9

//---------------------------------------------------------------------------------------------------------------------------------------
// Variables which will change
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int happened1 = 0;
int serialValue = 0;         // the sensor value
int serialMin = 15;         // minimum magnitude of earthquake
int serialMax = 100;           // maximum magnitude of earthquake

unsigned long previousMillis;
unsigned long timeLimit = 180000;
unsigned long difference;
unsigned long pressTime1;
unsigned long wait1;

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

//---------------------------------------------------------------------------------------------------------------------------------------
// Definitions
#define CLK 6
#define DIO 5

#define numberOfSeconds(_time_) ((_time_/1000)%60)
#define numberOfMinutes(_time_) (((_time_/1000)%60))%60)
const uint8_t OFF[] = {0, 0, 0, 0};
const uint8_t PLAY[] = {B01110011, B00111000, B01011111, B01101110};

//---------------------------------------------------------------------------------------------------------------------------------------
// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {} // This line is blocking, remove it when you are finished

  connectToWifi(ssid, password);

  previousMillis = 0;
  Serial.print("Start time = ");
  Serial.println(previousMillis);

  JsonObject json = makeAPIcall(host, url, sslClient, 443);

  float mag = json["features"][0]["properties"]["mag"]; // This will return the magnitude of the last earthquake to happen
  unsigned long quaketime = json["features"][0]["properties"]["time"]; // This will return the unix time of the last earthquake to happen



  Serial.print("\n\nMagnitude: ");
  Serial.println(mag);


  // initialize the button pin as a input:
  pinMode(buttonPin, INPUT);
  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the LED as an output:
  pinMode(richterPin, OUTPUT);

  display.clear();
  delay(1000);

  // Set the brightness:
  display.setBrightness(7);
  delay(1000);

  Serial.println("End of setup loop");

  //  haltFirmware();


}
//---------------------------------------------------------------------------------------------------------------------------------------

void loop()
{

  JsonObject json = makeAPIcall(host, url, sslClient, 443);

  float mag = json["features"][0]["properties"]["mag"]; // This will return the magnitude of the last earthquake to happen
  unsigned long quaketime = json["features"][0]["properties"]["time"]; // This will return the unix time of the last earthquake to happen

  Serial.print("\n\nMagnitude: ");
  Serial.println(mag);
  Serial.print("Unix time: ");
  Serial.println(quaketime);

  delay(10000);

  // haltFirmware();


}
