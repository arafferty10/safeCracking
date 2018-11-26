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


const int timeMotorStop = 125; // ms for motor to stop spinning after stop command


int homeOffset = 0; // Found running findFlag()

// Looks to see if the photogate has been triggered by the coupler flag
boolean flagDetected() {
  if( digitalRead(photoGate) == LOW ){
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

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
