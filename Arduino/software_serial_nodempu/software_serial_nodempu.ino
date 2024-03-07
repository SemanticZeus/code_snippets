#include <SoftwareSerial.h>

SoftwareSerial sserial(7,8);

void setup() {
  // put your setup code here, to run once:
  sserial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  sserial.println("AT");
  delay(100);
}
