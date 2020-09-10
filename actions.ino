void moveServoMotor(int r) {
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
    return;
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

void moveDial(int m) {

  int maxMag = 1000;
  int minMag = 1;
  int dialValue = 0;
  
   Serial.print("Test point m - ");
  Serial.println(m);
  dialValue = map(m, minMag, maxMag, 20, 200);
  for(int i=0; i<3; i++){
  analogWrite(richterPin, dialValue);
  delay(600);
  analogWrite(richterPin, LOW);
  dialValue = dialValue/2; 
}

  return;

}
