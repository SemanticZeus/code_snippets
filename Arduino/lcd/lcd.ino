#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  

  lcd.init();
  lcd.backlight();
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  lcd.print("Heather Mason");
  lcd.setCursor(0,1);
  lcd.print("Silent Hill 3");
}
