#include <LiquidCrystal_I2C.h>

#define BuzzerPin 16
#define SoilHumidityPin 32

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {

  ledcAttachChannel(BuzzerPin,1000,8,4);

  pinMode(SoilHumidityPin,INPUT);

  lcd.init();
  lcd.backlight();  
  lcd.clear();

}

void loop() {

  float shvalue = analogRead(SoilHumidityPin);

  lcd.setCursor(0, 0);
  lcd.print("SoilHum:");
  lcd.setCursor(9, 0);
  lcd.print(shvalue);
  
  //When the detected value is lower than the set threshold, the buzzer emits sound
  if(200 >= shvalue)
  {
    ledcWriteTone(BuzzerPin,532);
    delay(100);
    ledcWriteTone(BuzzerPin,532);
    delay(100);
    ledcWriteTone(BuzzerPin,659);
    delay(100);
    ledcWriteTone(BuzzerPin,0);  //Stop alarming
  }
  delay(500);
  lcd.clear();
}
