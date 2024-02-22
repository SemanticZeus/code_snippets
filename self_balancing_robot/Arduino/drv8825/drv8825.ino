#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int dirPin = 2;
const int stepPin = 3;
const int stepsPerRevolution = 200;

RF24 radio(7, 8, 10000); // CE, CSN

const byte address[5] = "1SNSR";

void init_radio()
{
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  //radio.setPayloadSize(32);
  radio.setChannel(0x76);
  radio.startListening();
  Serial.println("Radio initialized");
}

int steps = 0;

void read_radio()
{
    if (!radio.available()) return;
    char text[32] = "";
    radio.read(text, sizeof(text));
    steps = atoi(text);
    if (steps<0) steps = 0;
    if (steps) Serial.println(steps);
}

void setup() {
  Serial.begin(9600);
  init_radio();

	// Declare pins as Outputs
	pinMode(stepPin, OUTPUT);
	pinMode(dirPin, OUTPUT);
}

void loop()
{
	// Set motor direction clockwise
	digitalWrite(dirPin, HIGH);

  read_radio();

  for(;steps;steps--)
	{
		digitalWrite(stepPin, HIGH);
		delayMicroseconds(1000);
		digitalWrite(stepPin, LOW);
		delayMicroseconds(1000);
  }
}