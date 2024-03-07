#include <LiquidCrystal_I2C.h>

#define ESP8266 Serial1
#define ESP8266_RST_PIN 3
 
LiquidCrystal_I2C lcd(0x27, 16,2);

int serial_read_line(char *buffer, int size)
{
  char ch;
  int i = 0;
  int valid = 0;
  unsigned long long t = millis();
  for (;i<size-1;) {
    while(!Serial.available()) if (millis() - t > 5000) return 0;
    ch = Serial.read();
    if ((ch == '\n' || ch == '\r') && ! valid) continue;
    if ((ch == '\n' || ch == '\r') && valid) break;
    buffer[i++] = ch;
    valid = 1;
  }
  buffer[i]= '\0';
  return i;
}

int print_esp8266_response()
{
  char buffer[1024];
  unsigned long long start = millis();
  int ret = 0;
  while (1) {
    while (!ESP8266.available()) if (millis() - start > 1000) break;
    if (!ESP8266.available()) break;
    int n = ESP8266.readBytes(buffer, 500);
    Serial.write(buffer, n);
    ret+=n;
  }
  return ret;
}
 
void setup() {
  Serial.begin(115200);
  ESP8266.begin(115200); 
	digitalWrite(ESP8266_RST_PIN, LOW);
	delay(1);
	digitalWrite(ESP8266_RST_PIN, HIGH);
}
 

char buffer[100];
void loop() {
 
  if (serial_read_line(buffer,100))
  {
    ESP8266.print(buffer);
    ESP8266.print("\r\n");
  }
  print_esp8266_response(); 
}