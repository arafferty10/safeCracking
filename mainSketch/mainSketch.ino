#include <EEPROM.h>

// EEPROM Storage Locations
const int DIAL_ROTATION_SPEED = 0;

// Pin layout
const int motorControl = 6;
const int photo = 7;





void rotateCCW(int steps){


}

// Sets rotation speed of dial motor
// Expects value between 0-100%
int setDialSpeed(int _rotationSpeed) {

  int rotSpeed = map(_rotationSpeed, 0, 100, 0, 255);
  analogWrite(motorControl, rotSpeed);
  EEPROM.put(rotSpeed, DIAL_ROTATION_SPEED);
  Serial.print(F("Dial speed set to: "));
  Serial.println(rotSpeed);

  return _rotationSpeed;
}

bool detectFlag(){


  return detectedFlag;
}

bool phoneHome(){

  return atHome;
}

void setup() {

  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.println("go");

}

void loop() {

  int incoming;

  Serial.println();
  Serial.println();
  Serial.println("Main Menu");
  Serial.println(F("1. "));
  Serial.println(F("2. "));
  Serial.println(F("3. "));
  Serial.println(F("4. "));
  Serial.println(F("5. "));
  Serial.println(F("6. "));
  Serial.println();
  Serial.print(F("Enter your choice"));



  while(!Serial.available()){

    incoming = Serial.parseInt();

    switch(incoming){
      case 1:

        break;
      case 2:

        break;
      case 3:

        break;
      case 4:

        break;
      case 5:

        break;
      case 6:

        break;
      default:
        break;
    }
  }

}
