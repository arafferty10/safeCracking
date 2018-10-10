#include <EEPROM.h>

#define CCW 0
#define CW 1
// EEPROM Storage Locations
const int DIAL_ROTATION_SPEED = 0;


// Pin layout
const int motorControl = 6;
const int photo = 7;
const int motorRester = 8;
const int motorDir = 10;

volatile int steps = 0;
boolean dir = CW;
boolean prevDir = CW;


void rotateCCW(int _steps){
  if(direction != 0){
    digitalWrite(motorDir, LOW);
  }

  int stepsLeft = _steps;

  while(stepsLeft > 0){
    analogWrite(motorControl, 200);
    stepsLeft--;
  }
  analogWrite(motorControl, 0);
}

int stepsRequired(int curSteps, int dest){
  if(direction == CW){
    if( curSteps >= dest) {
      return (curSteps - dest);
    } else if(curSteps < dest){
      return (8400 - dest + curSteps);
    }
  } else if(direction == CCW){
    if(curSteps >= dest){
      return (8400 - curSteps + dest);
    } else if(curSteps < goal){
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


  return detectedFlag;
}

bool phoneHome(){

  return atHome;
}

void setup() {

  pinMode(photo, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.println("go");

}

void loop() {

  while(!Serial.available())
  {
    int incoming;

    Serial.println();
    Serial.println();
    Serial.println("Main Menu");
    Serial.println(F("1. Display settings"));
    Serial.println(F("2. "));
    Serial.println(F("3. "));
    Serial.println(F("4. "));
    Serial.println(F("5. "));
    Serial.println(F("6. Start Cracking!"));
    Serial.println();
    Serial.print(F("Enter your choice"));

    incoming = Serial.parseInt();
  }

  if(incoming == 1)
  {
    char c = EEPROM.get(DIAL_ROTATION_SPEED);
    Serial.println();
    Serial.print("Dial Rotation Speed: ");
    Serial.println(c);
    break;
  }
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
