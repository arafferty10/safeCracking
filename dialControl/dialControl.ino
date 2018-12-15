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

int discA, discB, discC;

void setup() {
  // put your setup code here, to run once:
  pinMode(photoGate, INPUT);
  pinMode(motorDir, OUTPUT);
  pinMode(motorControl, OUTPUT);
  pinMode(motorReset, OUTPUT);
//  handle.attach(servoPin);
//  handle.write(0);
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
//  findFlag();
//  steps = (84 * homeOffset);
//  setDial(0, false);
  
}

void loop() {

  char incoming;

  // Print text menu
  Serial.println();
  Serial.println("Main Menu");
  Serial.println(F("1. Go home | Reset Dial"));
  Serial.println(F("2. Go to specific dial number"));
  Serial.println(F("3. Dial position testing"));
  Serial.println(F("4. Measure indents"));
  Serial.println(F("5. Position Testing"));
  Serial.println(F("6. Display Indent Center Values"));
  Serial.println(F("7. Test servo"));
  Serial.println(F("8. Enter a combo"));
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

      EEPROM.write(LOCATION_HOME_OFFSET, homeOffset);
      
      steps = (84 * homeOffset);

      setDial(10,false);
      
      Serial.println("Dial should be at: 10");

    }
    else if(incoming == '2')
    {
      Serial.println("Enter a dial number ");
      Serial.setTimeout(2000);
      int reqPos = Serial.parseInt();

      Serial.print("Going to dial position: ");
      Serial.println(reqPos);

      setDial(reqPos,false);

      
    }
    //-----------------------------------------------------
    else if(incoming == '3')
    {
      Serial.println("Dial positioning test");
      positionTesting(); // will pick random locations on the dial and see if we can go to them

    }
    //-----------------------------------------------------
    else if(incoming == '4')
    {
      Serial.println("Measure Indents");
      int measurements = 0;

      while(1){ // waiting for good input
        Serial.print("How many measurements would you like to take? ");
        while(!Serial.available());
        Serial.setTimeout(1000);
        measurements = Serial.parseInt();

        if(measurements >= 1 && measurements <= 20) break;

        Serial.print(measurements);
        Serial.println(" out of bounds");
      }
      Serial.println(measurements);
      measureDiscC(measurements);
    }
    else if(incoming == '5')
    {
      Serial.println("Case 5!");
      
    } else if(incoming == '6'){
      
    } else if(incoming == '7'){
      testServo();
    } else if(incoming == '8'){
      for (byte x = 0 ; x < 3 ; x++)
      {
        int combo = 0;
        while (1) //Loop until we have good input
        {
          Serial.print(F("Enter Combination "));
          if (x == 0) Serial.print("A");
          else if (x == 1) Serial.print("B");
          else if (x == 2) Serial.print("C");
          Serial.print(F(" to start at: "));
          while (!Serial.available()); //Wait for user input
  
          Serial.setTimeout(1000); //Must be long enough for user to enter second character
          combo = Serial.parseInt(); //Read user input
  
          if (combo >= 0 && combo <= 99) break;
  
          Serial.print(combo);
          Serial.println(F(" out of bounds"));
        }
        Serial.println(combo);
        if (x == 0) discA = combo;
        else if (x == 1) discB = combo;
        else if (x == 2) discC = combo;
      }
    } else if(incoming =='9'){
      turnCW();
      setDial(discA, false);
      turnCCW();
      setDial(discB, true);
      turnCW();
      setDial(discC, false);  
    } else {
      Serial.print("Unknown Option: ");
      Serial.println(incoming, HEX);
    }

}
