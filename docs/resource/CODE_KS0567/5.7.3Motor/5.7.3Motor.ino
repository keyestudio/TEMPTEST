#define MotorPin1 19  //(IN+)
#define MotorPin2 18  //(IN-)

void setup() {
  pinMode(MotorPin1, OUTPUT);
  pinMode(MotorPin2, OUTPUT);
}

void loop() {
  //corotation 
  analogWrite(MotorPin1, 255); //Adjust the motor speed by modifying the analog value output range from 0-255
  analogWrite(MotorPin2, 0);
  delay(2000);
  //Stop Transition
  delay(200);
  analogWrite(MotorPin1, 0);
  analogWrite(MotorPin2, 0);
  delay(200);
  //reversal
  analogWrite(MotorPin1, 0);
  analogWrite(MotorPin2, 255);
  delay(2000);
  //Stop
  analogWrite(MotorPin1, 0);
  analogWrite(MotorPin2, 0);
  delay(2000);
}