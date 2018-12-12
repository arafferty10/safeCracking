
void testServo(){
  
  Serial.println("x to exit");
  Serial.println("a to adjust servo towards gear");
  Serial.println("z to adjust servo away from gear");

  int servo = servoRestingPosition;
  handle.write(servo);

  long timeSinceLastMovement = millis();
  int lastStep = steps;
  motorOn(); //Turn on motor controller
  turnCW();
  setMotorSpeed(50);

  

  while (1)
  {
    if (lastStep != steps)
    {
      lastStep = steps;
      timeSinceLastMovement = millis();
    }
    if (millis() - timeSinceLastMovement > 50)
    {
//      setMotorSpeed(0); //Stop!
//      Serial.println("Dial stuck");
//      while (1);
    }

    if (Serial.available())
    {
      byte incoming = Serial.read();

      if (incoming == 'a') servo++;
      if (incoming == 'z') servo--;
      if (incoming == 'x') //Exit
      {
        setMotorSpeed(0); //Stop!
        handle.write(servoRestingPosition); //Goto the resting position (handle horizontal, door closed)
        delay(timeServoRelease); //Allow servo to move
        break;
      }

      if (servo < 0) servo = 0;
      if (servo > 255) servo = 255;

      handle.write(servo); //Goto the resting position (handle horizontal, door closed)
    }
    //int handlePosition = averageAnalogRead(servoPosition); //Old way
//    int handlePosition = digitalRead(servoPositionButton); //Look for button being pressed

    Serial.print(F("servo: "));
    Serial.print(servo);
    Serial.print(F(" / handleButton: "));
//    Serial.print(servoPositionButton);
    Serial.println();

    delay(100);
  }

  //Record settings to EEPROM
  servoRestingPosition = servo + 10; //We want some clearance 
  servoTryPosition = servoRestingPosition - 60;
  servoHighPressurePosition = servoRestingPosition - 60;

  if (servoHighPressurePosition < 0 || servoTryPosition < 0)
  {
    Serial.println(F("servoPressurePosition or servoTryPosition is negative. Adjust servo higher."));
    Serial.println(F("Freezing"));
    while (1); //Freeze
  }

  EEPROM.put(LOCATION_SERVO_REST, servoRestingPosition);
  EEPROM.put(LOCATION_SERVO_TEST_PRESSURE, servoTryPosition);
  EEPROM.put(LOCATION_SERVO_HIGH_PRESSURE, servoHighPressurePosition);

  Serial.print(F("servo: resting["));
  Serial.print(servoRestingPosition);
  Serial.print(F("] try["));
  Serial.print(servoTryPosition);
  Serial.print(F("] Highpressure["));
  Serial.print(servoHighPressurePosition);
  Serial.print(F("]"));
  Serial.println();

  Serial.println(F("Servo positions stored."));

}
