// Functions used to measure the indents on Disc C

void measureDiscC(int numOfTests){

  // Zero out indent arrays
  for(int x = 0; x < 12; x++){
    indentLocations[x] = 0;
    indentWidths[x] = 0;  
  }  

  for(int testNumber = 0; testNumber < numOfTests; testNumber++){
    for(int indentNumber = 0; indentNumber < 12; indentNumber++){

      // checkForUserPause();

      findFlag();

      int dialValue;
      if(indentNumber == 0){
        dialValue = 0;  
      } else {
        dialValue = encoderToDial(indentLocations[indentNumber - 1]) + 10; // move to middle of high part past last indent
      }
      if(dialValue >= 100) dialValue -= 100;

      setDial(dialValue, false);

      measureIndent(indentLocations[indentNumber], indentWidths[indentNumber]);

      Serial.print(F("Test "));
      Serial.print(testNumber + 1);
      Serial.print(F(" of "));
      Serial.print(numOfTests);
      Serial.print(F(". IndentNum["));
      Serial.print(indentNumber);
      Serial.print(F("] Encoder["));
      Serial.print(indentLocations[indentNumber]);
      Serial.print(F("] / Dial["));
      Serial.print(encoderToDial(indentLocations[indentNumber]));
      Serial.print(F("] / Width["));
      Serial.print(indentWidths[indentNumber]);
      Serial.print(F("] / Depth["));
//      Serial.print(indentDepths[indentNumber]);
      Serial.print("]");
      Serial.println();
    
    }  
  }
  
  // find largest indent
  int biggestWidth = 0;
  int biggestIndent = 0;

  for(int x = 0; x < 12; x++){
    if(indentWidths[x] > biggestWidth){
      biggestWidth = indentWidths[x];
      biggestIndent = x;  
    }  
  }
  Serial.print("Largest indent number: ");
  Serial.println(biggestIndent);

  // turn off all indents
  for(int x = 0; x < 12; x++){
    indentsToTry[x] = false;
    EEPROM.put(LOCATION_TEST_INDENT_0 + x, false);
  }  

  // turn on largest indent
  indentsToTry[biggestIndent] = true;
  EEPROM.put(LOCATION_TEST_INDENT_0 + biggestIndent, true); 
  Serial.println("Largest indent now set to test");

  for(byte x = 0; x < 12; x++){
    EEPROM.put(LOCATION_INDENT_DIAL_0 + (x*2), indentLocations[x]);  
  }
  Serial.println("Indent locations stored to EEPROM");
}

// takes measurements of indents on disc C from any positon on dial
void measureIndent(int &indentLocation, int &indentWidth){
    
  int slowSearch = 50;

  int edgeFar = 0;
  int edgeNear = 0;

  handle.write(servoHighPressurePosition);
  delay(timeServoApply);

  // turn until we hit an edge
  turnCW();
  setMotorSpeed(slowSearch);
  
  long timeSinceLastMovement = millis();
  int lastStep = steps;
  
  while(1){
    if(lastStep != steps){
      lastStep = steps;
      timeSinceLastMovement = millis();  
    }
    if(millis() - timeSinceLastMovement > 10) break;    
  }
  setMotorSpeed(0);
  delay(timeMotorStop);
  
  steps += switchDirAdjustment;
  if(steps > 8400) steps -= 8400;

  edgeFar = steps; // record measurement

  // we've hit an edge, time to look for the other side
  turnCCW();
  setMotorSpeed(slowSearch);

  timeSinceLastMovement = millis();
  lastStep = steps;
  while(1){
    if(lastStep != steps){
      lastStep = steps;
      timeSinceLastMovement = millis();  
    }
    if(millis() - timeSinceLastMovement > 10) break;    
  }
  setMotorSpeed(0);
  delay(timeMotorStop);

  steps -= switchDirAdjustment;
  if(steps < 0) steps += 8400;
  
  edgeNear = steps; // record positon

  // time for a second reading
  turnCW();
  prevDir = CW;

  setMotorSpeed(slowSearch);

  timeSinceLastMovement = millis();
  lastStep = steps;
  
  while(1){
    if(lastStep != steps){
      lastStep = steps;
      timeSinceLastMovement = millis();  
    }
    if(millis() - timeSinceLastMovement > 10) break;    
  }
  setMotorSpeed(0);
  delay(timeMotorStop);

  steps -= switchDirAdjustment;
  if(steps < 0) steps += 8400;

  int edgeFar2 = steps; // record easurement

  int sizeOfIndent;
  if(edgeFar2 > edgeNear) sizeOfIndent = 8400 - edgeFar2 + edgeNear;
  else sizeOfIndent = edgeNear - edgeFar2;

  indentWidth += sizeOfIndent;

  indentLocation = edgeFar2 + (sizeOfIndent / 2); // find center of indent
  if(indentLocation >= 8400) indentLocation -= 8400;

  turnCCW();
  prevDir = CCW;

  gotoStep(indentLocation, false);
  handle.write(servoRestingPosition);
  delay(timeServoRelease * 2);
  
}
