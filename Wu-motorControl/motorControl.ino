
const int switchPin = 2; // switch input
const int motor1Pin = 3; // H-Bridge leg 1 (pin 2, 1A)
const int motor2Pin = 4; // H-Bridge leg 2 (pin 7, 2A)
const int enablePin = 9; // H-Bridge enable pin

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(switchPin, INPUT); // set switchPin as an INPUT
  pinMode(motor1Pin, OUTPUT); 
  pinMode(motor2Pin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, HIGH); // set enablePin to HIGH
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(switchPin) == HIGH)
  {
    digitalWrite(motor1Pin, LOW); // set leg 1 to LOW
    digitalWrite(motor2Pin, HIGH); // set leg 2 to HIGH
  }
  else
  {
    digitalWrite(motor1Pin, HIGH); // set leg 1 to HIGH
    digitalWrite(motor2Pin, LOW); // set leg 2 to LOW
  }
}
