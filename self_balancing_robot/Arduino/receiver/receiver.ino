#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte address[6] = "1SNSR";

void setup() {
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  while (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    delay(1000);
  }
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  //radio.setPayloadSize(32);
  radio.setChannel(0x76);
  radio.startListening();
  if (Serial) Serial.println("receiver");
}
int i = 0;
void loop() {
  if (!radio.available()) { lcd.clear(); return; }
  char text[32] = "";
  if (radio.available()) radio.read(text, sizeof(text));
  int n = strlen(text);
  if (n) {
    if (Serial) Serial.println(text);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text);
  } 
  delay(1000);
}