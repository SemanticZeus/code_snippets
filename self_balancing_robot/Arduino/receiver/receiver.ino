/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8, 10000); // CE, CSN

const byte address[5] = "1SNSR";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  //radio.setPayloadSize(32);
  radio.setChannel(0x76);
  radio.startListening();
  Serial.println("receiver");
}
int i=0;
void loop() {
    if (radio.available()) Serial.println("Radio is Available"); 
    else { Serial.println("Radio is not available"); delay(1000); return; }
    char text[32] = "";
    radio.read(text, sizeof(text));
    Serial.println(text);
    delay(1000);
}