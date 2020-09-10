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
#define ARDUINOJSON_USE_LONG_LONG 1
//---------------------------------------------------------------------------------------------------------------------------------------
// Libraries
#include <WiFi101.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <TM1637Display.h>
//---------------------------------------------------------------------------------------------------------------------------------------
DeserializationError errorCheck;
const unsigned int CHARACTER_LIMIT = 10000; // Limit of characters in HTTP response
//---------------------------------------------------------------------------------------------------------------------------------------
// WiFi Connection
const char* ssid = "VM6980419";          // replace these with the name and password for your local wi-fi network
const char* password = "fz7ScsrpXxyd";
//---------------------------------------------------------------------------------------------------------------------------------------
// API
String host = "earthquake.usgs.gov";
String url = "/fdsnws/event/1/query?format=geojson&limit=1";
//---------------------------------------------------------------------------------------------------------------------------------------
// WiFi Client
WiFiSSLClient sslClient;  // HTTPS
WiFiClient client;        // HTTP
char httpResponse[CHARACTER_LIMIT]; // If you are getting responses larger that 10kB, use another request.
JsonObjectConst json;
//---------------------------------------------------------------------------------------------------------------------------------------
// Timer
unsigned long updateTimer = 30000;
//---------------------------------------------------------------------------------------------------------------------------------------
// Input/output pins
const int buttonPin = 11;    // the number of the pushbutton pin
const int ledPin = 13;       // the pin that the LED is attached to
const int richterPin = 9;    // analogue dial connected to digital pin 9

//---------------------------------------------------------------------------------------------------------------------------------------
// Variables which will change
int buttonPushCounter = 1;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

int magNewInt = 0;
int magOldInt = 0;
int diff;
int ledState = LOW;


unsigned long previousMillis;
unsigned long previousMillisClock;
unsigned long currentMillisClock;

const long interval = 1000;           // interval at which to blink (milliseconds)


unsigned long timeLimit = 60000;
unsigned long difference;
unsigned long pressTime1;
unsigned long wait1;

Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position

float magOld;
float magNew;

//---------------------------------------------------------------------------------------------------------------------------------------
// Definitions
#define CLK 6
#define DIO 5

#define numberOfSeconds(_time_)  ((_time_/1000)%60)
#define numberOfMinutes(_time_)  (((_time_/1000)/60)%60)

const uint8_t OFF[] = {0, 0, 0, 0};
const uint8_t PLAY[] = {B01110011, B00111000, B01011111, B01101110};

//---------------------------------------------------------------------------------------------------------------------------------------
// Create display object of type TM1637Display:
TM1637Display display = TM1637Display(CLK, DIO);

void setup()
{
  Serial.begin(9600);
  while (!Serial) {} // This line is blocking, remove it when you are finished

  connectToWifi(ssid, password);

  previousMillis = 0;
  previousMillisClock = 0;
  Serial.print("Start time = ");
  Serial.println(previousMillis);

  // initialize the button pin as a input:
  //  pinMode(buttonPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);


  // initialize the LED as an output:
  pinMode(ledPin, OUTPUT);

  // initialize the LED as an output:
  pinMode(richterPin, OUTPUT);

  display.clear();
  delay(1000);

  // Set the brightness:
  display.setBrightness(7);
  delay(1000);

  makeAPIcall(host, url, sslClient, 443);
  float magOld = json["features"][0]["properties"]["mag"]; // This will return the magnitude of the last earthquake to happen
  magOldInt = magOld * 100;
  magNewInt = magOldInt;

  //  uint64_t quakeTimeOld = json["features"][0]["properties"]["time"].as<uint64_t>(); // This will return the unix time of the last earthquake to happen

  Serial.print("mag old:  ");
  Serial.println(magOldInt);
  Serial.print("mag new:  ");
  Serial.println(magNewInt);

  Serial.println("End of setup loop");
}
//---------------------------------------------------------------------------------------------------------------------------------------

void loop()
{
  //  Serial.println("new API call:");
  Serial.print("Test point 9  wait1 =");
  Serial.println(wait1);

  buttonState = digitalRead(buttonPin);
  previousMillis = millis();
  previousMillisClock = previousMillis;

  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      if (buttonPushCounter == 1) {
                pressTime1 = millis();
                currentMillisClock = pressTime1;
        Serial.println("Contract set");
        makeAPIcall(host, url, sslClient, 443);
        float magOld = json["features"][0]["properties"]["mag"]; // This will return the magnitude of the last earthquake to happen
        magOldInt = magOld * 100;
      } else {
        buttonPushCounter = 0;
        Serial.println("Contract reset");
      }
    }
  }

  delay(50);

  lastButtonState = buttonState;

  if (buttonPushCounter == 0) {

    wait1 = 0;
    Serial.println("Test point 8  ");

  }
  else if (buttonPushCounter == 1) {
//    pressTime1 = millis();
//    currentMillisClock = pressTime1;
    wait1 = millis() - pressTime1;

    Serial.println("Test point 7  ");

  }

  if (wait1 != 0 && timeLimit > wait1) {

    if (currentMillisClock - previousMillisClock >= interval) {
      int timeRemaining = timeLimit - wait1;

      int seconds = numberOfSeconds(timeRemaining);
      int minutes = numberOfMinutes(timeRemaining);
      display.clear();
      display.showNumberDecEx(seconds, 0b11100000, true, 2, 2); // Expect: _-5_
      //      display.showNumberDecEx(minutes, 0b11100000, false, 1, 1); // Expect: _-5_


      Serial.print("Test point : time remaining - ");
      Serial.println(timeRemaining);
      previousMillisClock = currentMillisClock;

      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
    }

    makeAPIcall(host, url, sslClient, 443);
    float magNew = json["features"][0]["properties"]["mag"]; // This will return the magnitude of the last earthquake to happen
    magNewInt = magNew * 100;
    diff = magNewInt - magOldInt;
    diff = abs(diff);
    Serial.print("Test point 2: diff - ");
    Serial.println(diff);



    if (diff > 1) {
      //Donate the money
      //      int turn;
      moveDial(magNewInt);

      moveServoMotor(120);
      Serial.println("Result --- Contract reset - trigger");
      magOldInt = magNewInt;
      pressTime1 = millis();
      buttonPushCounter = 0;

      display.showNumberDecEx(0, 0b11100000, true, 2, 2); // Expect: _-5_
      display.showNumberDecEx(1, 0b11100000, false, 1, 1); // Expect: _-5_



    }

    delay(500);

  }

  else if (timeLimit < wait1) {
    Serial.println("Result --- Contract reset - No trigger");
    magOldInt = magNewInt;

    digitalWrite(ledPin, LOW);
    //    int turn;
    moveServoMotor(60);
    buttonPushCounter = 0;
    pressTime1 = millis();
    Serial.print("Test point 4: wait1 - ");
    Serial.print(wait1);

    display.showNumberDecEx(0, 0b11100000, true, 2, 2); // Expect: _-5_
    display.showNumberDecEx(1, 0b11100000, false, 1, 1); // Expect: _-5_

  }
  //  waitForSerialInput();
  //  Serialprint64(quakeTime);
  //  Serial.println("done");

}
