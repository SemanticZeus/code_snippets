/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8, 20000); // CE, CSN

const byte address[5] = "1SNSR";

void setup() {
  radio.begin();
  Serial.begin(9600);
    radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  //radio.setPayloadSize(32);
  radio.stopListening();
  Serial.println("Transmitter");
}

void loop() {  
  const char text[] = "Goodbye Cruel World!";
  radio.write(text, sizeof(text));
}
