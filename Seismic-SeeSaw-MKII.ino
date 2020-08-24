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
  while (!Serial) {} // This line is blocking, remove it when you are finished

  connectToWifi(ssid, password);

  previousMillis = 0;
  Serial.print("Start time = ");
  Serial.println(previousMillis);

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

  makeAPIcall(host, url, sslClient, 443);

  float magOld = json["features"][0]["properties"]["mag"].as<float>(); // This will return the magnitude of the last earthquake to happen
  float magNew = json["features"][0]["properties"]["mag"].as<float>();;

  //  uint64_t quakeTimeOld = json["features"][0]["properties"]["time"].as<uint64_t>(); // This will return the unix time of the last earthquake to happen

  Serial.print("mag old:  ");
  Serial.println(magOld);
  Serial.print("mag new:  ");
  Serial.println(magNew);

  Serial.println("End of setup loop");
}
//---------------------------------------------------------------------------------------------------------------------------------------

void loop()
{
  //  Serial.println("new API call:");

  buttonState = !digitalRead(buttonPin);
  previousMillis = millis();

  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;

      if (buttonPushCounter == 1) {
        pressTime1 = millis();
        Serial.println("Contract set");
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
  }
  else if (buttonPushCounter == 1) {
    wait1 = millis() - pressTime1;
    //    display.showNumberDecEx(300, 0b01000000, false, 4, 0);

  }

  if (wait1 != 0 && timeLimit > wait1) {

    makeAPIcall(host, url, sslClient, 443);
    float magNew = json["features"][0]["properties"]["mag"].as<float>(); // This will return the magnitude of the last earthquake to happen
    float magOld;
    
    float diff = magNew - magOld;
    
        if (diff > 0.1) {
          //Donate the money
//          int turn;
Serial.print("Test point 1 ");

//          turn = ServoTurn(120);
        }

        delay(5000);

  }

  else if (timeLimit < wait1) {
    Serial.println("Result --- Contract reset - No trigger");
    digitalWrite(ledPin, LOW);
//    int turn;
//    turn = ServoTurn(60);
    buttonPushCounter = 0;
    pressTime1 = millis();
  }
  //  waitForSerialInput();
  //  Serialprint64(quakeTime);
  //  Serial.println("done");

}

int ServoTurn(int r) {

  int cw = 1111;
  int acw = 1380; //dont touch
  if (r > 90) {
    myservo.attach(12);  // attaches the servo on pin 9 to the servo object
    myservo.write(r);  // set servo to mid-point
    delay(acw);
    myservo.detach();
    delay(1000);
    r = r - 60;
    myservo.attach(12);  // attaches the servo on pin 9 to the servo object
    myservo.write(r);
    delay(cw);
    myservo.detach();
  }
  else if (r <= 90) {
    myservo.attach(12);  // attaches the servo on pin 9 to the servo object
    myservo.write(r);  // set servo to mid-point
    delay(acw);
    myservo.detach();
    delay(1000);
    r = r + 60;
    myservo.attach(12);  // attaches the servo on pin 9 to the servo object
    myservo.write(r);
    delay(cw);
    myservo.detach();
  }

}
