#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8, 1000000); // CE, CSN

const byte address[6] = "1SNSR";

void setup() {
  Serial.begin(115200);
  while (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    delay(1000);
  }

  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  //radio.setPayloadSize(32);
  radio.stopListening();
  Serial.println("Transmitter");
}

int read_serial(char *buffer, int size)
{
  int valid = 0;
  int n = 0;
  unsigned long long start = millis();
  while (n<size-1) {
    while (!Serial.available()) if (millis() - start > 500) return n;
    char ch = Serial.read();
    if ((ch=='\r' || ch =='\n') && !valid) continue;
    else if ((ch=='\r' || ch=='\n') && valid) break;
    valid = 1;
    buffer[n++]=ch;
  }
  buffer[n]='\0';
  return n;
}

int read_serial(char *buffer, int size)
{
  int valid = 0;
  int n = 0;
  unsigned long long start = millis();
  while (n<size-1) {
    while (!Serial.available()) if (millis() - start > 500) return n;
    char ch = Serial.read();
    if ((ch=='\r' || ch =='\n') && !valid) continue;
    else if ((ch=='\r' || ch=='\n') && valid) break;
    valid = 1;
    buffer[n++]=ch;
  }
  buffer[n]='\0';
  return n;
}

const int buffer_size = 2*16+1;
char buffer[buffer_size];
unsigned int counter = 0;
void loop() {
  int n = read_serial(buffer,buffer_size);
  if (n) {
    radio.write(buffer, n+1);
  } else {
    char message[32];
    snprintf(message, 32,  "%d Heather Mason\0", counter++ % 256);
    radio.write(message, strlen(message)+1);
  }
}
