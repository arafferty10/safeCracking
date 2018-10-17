const byte encA = 1;
const byte encB = 3;

#define CCW 0
#define CW 1

volatile int steps = 0;
boolean dir = CW;
boolean prevDir = CW;

void countEncoder(){
  if(dir == CW){
    steps--;
  } else {
    steps++;
  }
  if(steps < 0){
    steps = 8399;
  }
  if(steps > 8399){
    steps = 0;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Encoder Test");

  pinMode(encA, INPUT);
  pinMode(encB, INPUT);

  // Attach encoder interrupts
  attachInterrupt(digitalPinToInterrupt(encA), countEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encB), countEncoder, CHANGE);

}

void loop() {

  Serial.print("Steps: ");
  Serial.println(steps);

}
