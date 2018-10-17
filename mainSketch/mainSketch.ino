#include <EEPROM.h>

#define CCW 0
#define CW 1
// EEPROM Storage Locations
const int DIAL_ROTATION_SPEED = 0;


// Pin layout
const int motorControl = 6;
const int photo = 7;
const int motorReset = 8;
const int motorDir = 3;

volatile int steps = 0;
boolean dir = CW;
boolean prevDir = CW;


void rotateCCW(int _steps){
  Serial.println("inside rotateCCW()");

  digitalWrite(motorDir, LOW);

  int stepsLeft = _steps;

  while(stepsLeft > 0){
    Serial.println("trying to turn...");
    analogWrite(motorControl, 255);
    stepsLeft--;
  }
  analogWrite(motorControl, 0);
}

int stepsRequired(int curSteps, int dest){
  if(dir == CW){
    if( curSteps >= dest) {
      return (curSteps - dest);
    } else if(curSteps < dest){
      return (8400 - dest + curSteps);
    }
  } else if(dir == CCW){
    if(curSteps >= dest){
      return (8400 - curSteps + dest);
    } else if(curSteps < dest){
      return (dest - curSteps);
    }
  }
}

void enableMotor(){
  digitalWrite(motorReset, HIGH);
}

void disableMotor(){
  digitalWrite(motorReset, LOW);
}

// Sets rotation speed of dial motor
// Expects value between 0-100%
int setDialSpeed(int _rotationSpeed) {
  int rotSpeed = map(_rotationSpeed, 0, 100, 0, 255);
  EEPROM.put(rotSpeed, DIAL_ROTATION_SPEED);
  analogWrite(motorControl, rotSpeed);
  Serial.print(F("Dial speed set to: "));
  Serial.println(rotSpeed);

  return _rotationSpeed;
}

bool detectFlag(){


  // return detectedFlag;
}

bool phoneHome(){

  // return atHome;
}

void setup() {

  pinMode(photo, INPUT_PULLUP);
  pinMode(motorDir, OUTPUT);
  pinMode(motorControl, OUTPUT);
  pinMode(motorReset, OUTPUT);

  enableMotor();
  Serial.begin(9600);
  Serial.println();
  Serial.println("Safe Cracker");

}

void loop() {

  char incoming;

  // Print text menu
  Serial.println();
  Serial.println("Main Menu");
  Serial.println(F("1. Rotate motor"));
  Serial.println(F("2. placeholder option"));
  Serial.println(F("3. placeholder option"));
  Serial.println(F("4. placeholder option"));
  Serial.println(F("5. placeholder option"));
  Serial.println(F("6. Start Cracking!"));
  Serial.println();
  Serial.println(F("Enter your choice"));

  while(!Serial.available());


  incoming = Serial.read();

  // while(!Serial.available()){
    if(incoming == '1'){
      Serial.println("Case 1!");
      Serial.print("Rotating...");
      rotateCCW(32);
    } else if(incoming == '2'){
      Serial.println("Case 2!");
    } else if(incoming == '3'){
      Serial.println("Case 3!");
    }else if(incoming == '4'){
      Serial.println("Case 4!");
    } else {
      Serial.print("Unknown Option: ");
      Serial.println(incoming, HEX);
    }

  // }

  // while(!Serial.available()){
  //
  //   while(Serial.available());
  //   incoming = Serial.read();
  //   Serial.print("Incoming serial: ");
  //   Serial.println(incoming);
  //
  // }

}
