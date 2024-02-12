#include <HCSR04.h>

int stepper_pins[] = {4,5,6,7};
int trig = 12;
int echo = 13;

HCSR04 hc(12,13); //initialisation class HCSR04 (trig pin , echo pin)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i=0;i<4;i++) pinMode(stepper_pins[i], OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

int seq[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

double measure_distance()
{
  digitalWrite(trig, LOW);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  return pulseIn(echo, HIGH)*0.034/2;
}


void stepper(int steps)
{
  int dir = steps>0 ? 1 : -1;
  steps = abs(steps);
  for (int i=0;i<steps;i++)
  {
    int index = i % 8;
    if (dir==-1) index = 7-index;
    for (int k=0;k<4;k++) digitalWrite(stepper_pins[k], seq[index][k]);
    delay(1);
  }
  for (int k=0;k<4;k++) digitalWrite(stepper_pins[k], LOW);
}

void loop() {
  int d = hc.dist();
  stepper((d-10)*2);
  //Serial.println(d);
  //delay(100);
}
