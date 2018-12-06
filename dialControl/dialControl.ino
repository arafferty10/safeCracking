#define CCW 0
#define CW 1

// Pin definitions
const int motorControl = 6; // motor PWM
const int photoGate = 7;
const int motorReset = 8; // motor reset pin
const int motorDir = 3; // motor direction | HIGH - CW, LOW - CCW

volatile int steps = 0;
boolean dir = CW; // default direction (also direction for first digit of combo)
boolean prevDir = CW;

int switchDirAdjustment = (84 * 0) + 0;

const int timeMotorStop = 125; // ms for motor to stop spinning after stop command


int homeOffset = 0; // Found running findFlag()

// Looks to see if the photogate has been triggered by the coupler flag
boolean flagDetected() {
  if( digitalRead(photoGate) == LOW ){
    Serial.println("**PHOTOGATE TRIPPED**");
    return (true);
  }
  return (false);
}

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

  return (dialValue * 84);
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
  int fast = 255;
  int slow = 50;

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

void setup() {
  // put your setup code here, to run once:
  pinMode(photoGate, INPUT_PULLUP);
  pinMode(motorDir, OUTPUT);
  pinMode(motorControl, OUTPUT);
  pinMode(motorReset, OUTPUT);

  motorOn();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Safe Cracker");

  attachInterrupt(digitalPinToInterrupt(2), countA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), countB, CHANGE);
  
}

void loop() {

  char incoming;

  // Print text menu
  Serial.println();
  Serial.println("Main Menu");
  Serial.println(F("1. Go home | Reset Dial"));
  Serial.println(F("2. Go to specific dial number"));
  Serial.println(F("3. Set Starting Dial Value/Go Home"));
  Serial.println(F("4. placeholder option"));
  Serial.println(F("5. placeholder option"));
  Serial.println(F("6. Start Cracking!"));
  Serial.println();
  Serial.println(F("Enter your choice"));

  while(!Serial.available());


  incoming = Serial.read();

  // while(!Serial.available()){
    if(incoming == '1')
    {
      findFlag();
      Serial.print("Home offset: ");
      Serial.println(homeOffset);

      int zeroLocation = 0;

      while(1){
        Serial.print("Enter where dial is actually at: ");

        while(!Serial.available());

        Serial.setTimeout(1000);
        zeroLocation = Serial.parseInt();

        Serial.print(zeroLocation);
        if(zeroLocation >= 0 && zeroLocation <= 99) break;
        Serial.println(" out of bounds");
      }
      
      homeOffset = zeroLocation;
      Serial.print("Setting home offset to: ");
      Serial.println(homeOffset);

      steps = (84 * homeOffset);

      setDial(0,false);
      Serial.println("Dial should be at: 0");
//      Serial.println("Case 1!");
//      Serial.print("Rotating...");
//      rotateCCW(1000);
    }
    else if(incoming == '2')
    {
      Serial.println("Enter a dial number ");
      Serial.setTimeout(2000);
      int reqPos = Serial.parseInt();

      Serial.print("Going to dial position: ");
      Serial.println(reqPos);

      setDial(reqPos,0);
    }
    //-----------------------------------------------------
    else if(incoming == '3')
    {
      Serial.println("Case 3!");
      //Go to starting conditions
      findFlag(); //Detect the flag and center the dial based on it

      Serial.print(F("Offset from home is: "));
      Serial.println(homeOffset);

      int zeroLocation = 0;

      while (1) //Error checking
      {
        Serial.print(F("Enter where dial is actually at: "));

        while (!Serial.available()); //Wait for user input

        Serial.setTimeout(1000);
        zeroLocation = Serial.parseInt(); //Read user input

        Serial.print(zeroLocation);

        //Check to make sure the user input is in bounds
        if (zeroLocation >= 0 && zeroLocation <= 99) break;
        Serial.println(F(" out of bounds"));
      }

      homeOffset = zeroLocation;

      Serial.print(F("\n\rSetting home offset to: "));
      Serial.println(homeOffset);

//      EEPROM.write(LOCATION_HOME_OFFSET, homeOffset);

      //Adjust steps with the real-world offset
      steps = (84 * homeOffset); //84 * the number the dial sits on when 'home'

      setDial(0, false); //Turn to zero

      Serial.println(F("Dial should be at: 0"));

    }
    //-----------------------------------------------------
    else if(incoming == '4')
    {
      Serial.println("Case 4!");
      setDial(50, 0);
    }
    else
    {
      Serial.print("Unknown Option: ");
      Serial.println(incoming, HEX);
    }

}
