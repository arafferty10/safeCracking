#include <EEPROM.h>
#include <Servo.h>
#include "eeprom_loc.h"

Servo handle;

#define CCW 0
#define CW 1

// Pin definitions
const int motorControl = 6; // motor PWM
const int photoGate = 5;
const int motorReset = 8; // motor reset pin
const int motorDir = 10; // motor direction | HIGH - CW, LOW - CCW
const int encA = 2;
const int encB = 3;
const int servoPin = 9;

int testSpots[] = {99, 8, 16, 24, 33, 41, 50, 58, 66, 74, 83, 91}; 
boolean indentsToTry[12];
int indentWidths[12];
int indentLocations[12];


volatile int steps = 0;

boolean dir = CW; // default direction (also direction for first digit of combo)
boolean prevDir = CW;
int homeOffset = 0; // Found running findFlag()

int switchDirAdjustment = (84 * 0) + 0;

const int timeMotorStop = 125; // ms for motor to stop spinning after stop command

int servoRestingPosition = 0; // handle, at rest position
int servoTryPosition = 50; // position when measuring
int servoHighPressurePosition = 60; // position when opening safe

const int timeServoApply = 350;
const int timeServoRelease = 250;

void setup() {
  // put your setup code here, to run once:
  pinMode(photoGate, INPUT);
  pinMode(motorDir, OUTPUT);
  pinMode(motorControl, OUTPUT);
  pinMode(motorReset, OUTPUT);
//  handle.attach(servoPin);
  motorOn();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Safe Cracker");

  attachInterrupt(digitalPinToInterrupt(encA), countA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encB), countB, CHANGE);

  // Load settings from EEPROM
  EEPROM.get(LOCATION_HOME_OFFSET, homeOffset); // read in homeOffset
  Serial.print("Home offset: ");
  Serial.println(homeOffset);

  // Init dial -> find home, set offset, go to zero
  findFlag();
  steps = (84 * homeOffset);
  setDial(0, false);
  
}

void loop() {

  char incoming;

  // Print text menu
  Serial.println();
  Serial.println("Main Menu");
  Serial.println(F("1. Go home | Reset Dial"));
  Serial.println(F("2. Go to specific dial number"));
  Serial.println(F("3. Set Dial to 54"));
  Serial.println(F("4. Display current step counter value"));
  Serial.println(F("5. Position Testing"));
  Serial.println(F("6. Servo Testing"));
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

        Serial.setTimeout(1500);
        zeroLocation = Serial.parseInt();

        Serial.print(zeroLocation);
        if(zeroLocation >= 0 && zeroLocation <= 99) break;
        Serial.println(" out of bounds");
      }
      
      homeOffset = zeroLocation;
      Serial.print("\nSetting home offset to: ");
      Serial.println(homeOffset);

      steps = (84 * homeOffset);

      setDial(10,false);
      
      Serial.println("Dial should be at: 0");

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
      Serial.print("Steps required for movement: ");
      Serial.println(stepsRequired(0, 54));
      setDial(54,false);

    }
    //-----------------------------------------------------
    else if(incoming == '4')
    {
      Serial.println("Case 4!");
//      setDial(50, 0);
      Serial.print("Step number: ");
      Serial.println(steps);
    }
    else if(incoming == '5')
    {
      Serial.println("Case 5!");
      positionTesting();
    } else if(incoming == '6'){
      
    }
    {
      Serial.print("Unknown Option: ");
      Serial.println(incoming, HEX);
    }

}
