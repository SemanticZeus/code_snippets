#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {

  lcd.init();
  lcd.backlight();
  // put your setup code here, to run once:

}

int count = 1;

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  lcd.print("Heather Mason");
  lcd.setCursor(0,1);
  char buf[512];
  snprintf(buf, 512, "Silent Hill %d", count);
  lcd.print(buf);
  count = (count+1)%100;
  delay(200);
}
