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

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
