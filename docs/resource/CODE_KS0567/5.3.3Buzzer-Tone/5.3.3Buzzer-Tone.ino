const int buzzerPin = 16;   //Set buzzer pin to 16
void setup() {
  ledcAttachChannel(buzzerPin,1000,8,4);
}
void loop() {
    ledcWriteTone(buzzerPin,532);           //duo --C2
    delay(100);
    ledcWriteTone(buzzerPin,587);           //re --D3
    delay(100);
    ledcWriteTone(buzzerPin,659);           //mi --E3
    delay(100);
   //Alarm
   for(int i = 200; i<=1000; i+=10){ 
    ledcWriteTone(buzzerPin,i);
    delay(10);
   }
    //Alarm
   for(int i = 1000; i>=200; i-=10){ 
    ledcWriteTone(buzzerPin,i);
    delay(10);
   }
ledcWriteTone(buzzerPin,0);
}