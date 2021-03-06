// Sets speed of motor, 0 - 255
void setMotorSpeed(int speed){
  analogWrite(motorControl, speed);
}

// Tells motor to turn clockwise
void turnCW() {
  dir = CW;
  digitalWrite(motorDir, HIGH);
}

// Tells motor to turn counter clockwise
void turnCCW(){
  dir = CCW;
  digitalWrite(motorDir, LOW);
}

// Turn motor controller on
void motorOn(){
  digitalWrite(motorReset, HIGH);
}

// Turn motor controller off
void motorOff(){
  digitalWrite(motorReset, LOW);
}

// Looks to see if the photogate has been triggered by the coupler flag
boolean flagDetected() {
  if( digitalRead(photoGate) == LOW ){
    Serial.println("**PHOTOGATE TRIPPED**");
    return (true);
  }
  return (false);
}

// Encoder counting interrupts
void countA(){
  if(dir == CW) steps--;
  else steps++;
  if(steps < 0) steps = 8399; // limit to zeero
  if(steps > 8399) steps = 0; //limit to 8399
}

void countB(){
  if(dir == CW) steps--;
  else steps++;
  if(steps < 0) steps = 8399; // limit to zeero
  if(steps > 8399) steps = 0; //limit to 8399
}

// For a given dial value, convert to encoder value (0 - 8400)
// Since there are 100 numbers on the dial, each num is 84 ticks wide
int dialToEncoder(int dialValue){
  int encoderValue = dialValue * 84; // 8400 / 100 = 84 ticks per dial num

  if(encoderValue > 8400){ // Results when invalid value is chosen
    Serial.print("Invalid dial value. encoderValue: ");
    Serial.println(encoderValue);
    while(1);
  }

  return (84 * dialValue);
}

// For a given encoder value, convert to dial number (0 - 99)
int encoderToDial(int encoderValue){

  int dialValue = encoderValue / 84;
  int partial = encoderValue % 84;

  if(partial >= (84 / 2)) {
    dialValue++;
  }

  if(dialValue > 99){
    dialValue -= 100;
  }

  return (dialValue);
}

// Spins until photogate is triggered
void findFlag(){

  // search speeds
  int fast = 146;
  int slow = 50;

  turnCW();
  
  // check to see if PG is already triggered, if so, move away
  if(flagDetected() == true){
    Serial.println("Already close to photogate, must move away");
    int current = encoderToDial(steps);
    current += 50; // since we're close, move away 50 ticks

    if(current > 100) { // if we're out of bounds of dial
      current -= 100;
    }
    setDial(current, false); // finally move away from PG
  }

  // Begin search
  setMotorSpeed(fast);


  while(flagDetected() == false){ // spin away!
    delayMicroseconds(1);
  }

  // Just skipped past the PG, stop and turn around slowly
  setMotorSpeed(0); // motor stop
  delay(timeMotorStop); // give the motor a chance to brake
  turnCCW(); // and change direction

  setMotorSpeed(slow);
  while(flagDetected() == false){
    delayMicroseconds(1);
  }
  setMotorSpeed(0);
  delay(timeMotorStop);

  // adjust for offset
  steps = (84 * homeOffset);

  prevDir = CCW;
}

// Find number of steps between current position and goal
int stepsRequired(int current, int goal){
  if(dir == CW){
    // if current pos is above our goal, req steps is difference
    // between current and our goal
    if(current >= goal) return (current - goal);
    else if(current < goal) return (8400 - goal + current); // wee need to wrap around
  }
  else if(dir == CCW){
    if(current >= goal) return (8400 - goal + current);
    else if(current < goal) return (goal - current);
  }

  Serial.println(F("stepRequired failed")); //We shouldn't get this far
  Serial.print(F("Goal: "));
  Serial.println(goal);
  Serial.print(F("currentSteps: "));
  Serial.println(current);

  return (0);
}

// Go to given step value, adds full 360 rotation if desired
// Assumes direction previously set
int gotoStep(int stepGoal, boolean addFullRotation){

  // search speeds and window to account for motor/encoder slack
  int coarse = 200;
  int coarseWindow = 1250;
  int fine = 50;
  int fineWindow = 32;

  // Account for slack in encoder
  if(dir == CW && prevDir == CCW){
    steps += switchDirAdjustment;
    if(steps > 8400) steps -= 8400;
    prevDir = CW;
  }
  else if(dir == CCW && prevDir == CW){
    steps -= switchDirAdjustment;
    if(steps < 0) steps += 8400;
    prevDir = CCW;
  }

  setMotorSpeed(coarse); // Lets start looking!
  while(stepsRequired(steps, stepGoal) > coarseWindow); // Spin until we get within coarse search window

  // after getting close, move past goal, then go to goal (add a full rotation)
  if(addFullRotation == true){
    int tempStepGoal = steps + 8400/2; // moves away 50 ticks from current position
    if(tempStepGoal > 8400) tempStepGoal -= 8400; // account for full roto

    // run to temp pos
    while(stepsRequired(steps, tempStepGoal) > coarseWindow);

    // go to stepGoal
    while(stepsRequired(steps, stepGoal) > coarseWindow);
  }

  setMotorSpeed(fine); // slow down for fine search

  while(stepsRequired(steps, stepGoal) > fineWindow); // run until within fine window

  setMotorSpeed(0); // STOP
  delay(timeMotorStop);

  int delta = steps - stepGoal; // distance we've gone

  return (delta);
}


// Go to given dial
int setDial(int dialValue, boolean extraRoto){

  // convert given dial to encoder steps
  int encoderValue = dialToEncoder(dialValue);

  gotoStep(encoderValue, extraRoto); // go to encoder value

  int actualDialValue = encoderToDial(steps); // convert landed encoder value to a dial value

  return (actualDialValue);
}


// Resets dial
// Rotate CCW past zero, then continue until zero again
void resetDial(){
  motorOff();

  turnCCW();

  setMotorSpeed(255); // Speed racer
  motorOn();

  // run for 'two' complete rotations
  int deltaSteps = 0;
  while(deltaSteps < (8400 * 2)){
    int startingSteps = steps; // where we started
    delay(100); // spin for a bit

    if(steps >= startingSteps) deltaSteps += steps - startingSteps;
    else deltaSteps += (8400 - startingSteps + steps);
  }

  setMotorSpeed(0);
  delay(timeMotorStop);

  prevDir = CCW;
}

void positionTesting()
{
  int randomDial;

  for (int x = 0 ; x < 2 ; x++)
  {
    randomDial = random(0, 100);
//    randomDial = 25;
    turnCCW();
    setDial(randomDial, true);

    Serial.print(F("Dial should be at: "));
    Serial.println(encoderToDial(steps));
//    messagePause("Verify then press key to continue");

    randomDial = random(0, 100);
//    randomDial = 75;
    turnCW();
    setDial(randomDial, false);

    Serial.print(F("Dial should be at: "));
    Serial.println(encoderToDial(steps));
//    messagePause("Verify then press key to exit");
  }
}
