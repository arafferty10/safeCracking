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

// Looks to see if the photogate has been triggered by the coupler flag
boolean flagFound() { 
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


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
