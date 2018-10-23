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

//Rotate the dial Counter Clockwise
//-----------------------------------------------------
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
//-----------------------------------------------------

//Looks at current dial position and determines number of steps between where
//you are and where you want to go
//-----------------------------------------------------
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
//-----------------------------------------------------

void enableMotor(){
  digitalWrite(motorReset, HIGH);
}

void disableMotor(){
  digitalWrite(motorReset, LOW);
}

// Sets rotation speed of dial motor
// Expects value between 0-100%
//-----------------------------------------------------
int setDialSpeed(int _rotationSpeed) {
  int rotSpeed = map(_rotationSpeed, 0, 100, 0, 255);
  EEPROM.put(rotSpeed, DIAL_ROTATION_SPEED);
  analogWrite(motorControl, rotSpeed);
  Serial.print(F("Dial speed set to: "));
  Serial.println(rotSpeed);

  return _rotationSpeed;
}
//-----------------------------------------------------

//Simple boolean to see if flag is detected
//-----------------------------------------------------
bool detectFlag(){

  if(digitalRead(photo) == LOW) {
    Serial.println("FLAG DETECTED");
    return (true);
  }
  return (false);

}
//-----------------------------------------------------

bool phoneHome(){

  // return atHome;
}

//Given a dial number, goto that value
//Returns the dial value thats ended on
//-----------------------------------------------------
int setDial(int dialValue, boolean extraSpin)
{
  //Serial.print("Want dialValue: ");
  //Serial.println(dialValue);

  int encoderValue = convertDialToEncoder(dialValue); //Get encoder value
  Serial.print("Want encoderValue: ");
  Serial.println(encoderValue);

  gotoStep(encoderValue, extraSpin); //Goto that encoder value
  Serial.print("After movement, steps: ");
  Serial.println(steps);

  int actualDialValue = convertEncoderToDial(steps); //Convert back to dial values
  Serial.print("After movement, dialvalue: ");
  Serial.println(actualDialValue);

  return (actualDialValue);
}

//-----------------------------------------------------

//Given an encoder value, tell me where on the dial that equates
//Returns 0 to 99
//If there are 100 numbers on the dial, each number is 84 ticks wide
//-----------------------------------------------------
int convertEncoderToDial(int encoderValue)
{
  int dialValue = encoderValue / 84; //2388/84 = 28.43
  int partial = encoderValue % 84; //2388%84 = 36

  if (partial >= (84 / 2)) dialValue++; //36 < 42, so 28 is our final answer

  if (dialValue > 99) dialValue -= 100;

  return (dialValue);
}

//-----------------------------------------------------


//Given a dial value, covert to an encoder value (0 to 8400)
//If there are 100 numbers on the dial, each number is 84 ticks wide
//-----------------------------------------------------
int convertDialToEncoder(int dialValue)
{
  int encoderValue = dialValue * 84;

  if (encoderValue > 8400)
  {
    Serial.print("Whoa! Your trying to go to a dial value that is illegal. encoderValue: ");
    Serial.println(encoderValue);
    while (1);
  }

  return (84 * dialValue);
}

//-----------------------------------------------------

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
  Serial.println(F("2. Detech Flag"));
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
      Serial.println("Case 1!");
      Serial.print("Rotating...");
      rotateCCW(32);
    }
    else if(incoming == '2')
    {
      detectFlag();
      Serial.println("Case 2!");
    }
    else if(incoming == '3')
    {
      Serial.println("Case 3!");
      //Go to starting conditions
      detectFlag(); //Detect the flag and center the dial based on it

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

      EEPROM.write(LOCATION_HOME_OFFSET, homeOffset);

      //Adjust steps with the real-world offset
      steps = (84 * homeOffset); //84 * the number the dial sits on when 'home'

      setDial(0, false); //Turn to zero

      Serial.println(F("Dial should be at: 0"));

    }
    else if(incoming == '4')
    {
      Serial.println("Case 4!");
    }
    else
    {
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
