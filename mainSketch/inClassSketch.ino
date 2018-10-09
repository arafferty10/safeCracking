void setup(){
  Serial.begin(9600);
  Serial.println("Terminal");
}

void loop
{
  if (Serial.available())
  {
    byte incoming = Serial.read();

    Serial.print("You pressed: ");
    Serial.println(incoming);

    if(incoming == 'd')
    {
      Serial.print("Enter dial position to turn to...");

      while(Serial.available() == 0);
      Serial.setTimeout(1000);
      
      int dialPos = Serial.parseInt();

      Serial.print("Going to: ");
      Serial.println(dialPos);
    }
    else if (incoming =='t')
    {
      
    }
  }
}
