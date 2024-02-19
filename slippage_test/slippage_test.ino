int read_key(int key)
{
  pinMode(key,INPUT_PULLUP);
  int count = 0;
  for (int i=0;i<100;i++) {
    count+=digitalRead(key);
    delay(1);
  }
  if (count>5) return 1;
  return 0;
}


#include <HCSR04.h>

int stepper_pins[] = {4,5,6,7};
int trig = 12;
int echo = 13;

HCSR04 hc(12,13); //initialisation class HCSR04 (trig pin , echo pin)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i=0;i<4;i++) pinMode(stepper_pins[i], OUTPUT);
//  pinMode(trig, OUTPUT);
//  pinMode(echo, INPUT);
//
//    // put your setup code here, to run once:
//  digitalWrite(LED_BUILTIN, 0);
//  pinMode(LED_BUILTIN, OUTPUT);
   //analogReference(EXTERNAL);
  
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


enum STATE {INITIAL, FINAL, PULLUP, PULLDOWN, UNDETERMINED};

int state = INITIAL;
int pot_value = -1;
int distance = -1;
int pot_zero_value = -1;
int chrono = -1;
char buf[50];
int step_count = 0;
int stepper_step_len = 10;
void loop() {
  switch (state) {
    case UNDETERMINED:

    break;
    case INITIAL:
      if (read_key(8)) state = PULLUP;
      pot_zero_value = analogRead(A0);
      distance = hc.dist(); 
      step_count = 0;
    break;
    case PULLUP:
      stepper(stepper_step_len);
      step_count += stepper_step_len;
      int d = hc.dist();
      if (abs(d-distance)>100) state = FINAL;
    break;
    case FINAL:
      pot_value = analogRead(A0);
      delay(1000);
      int diff = abs(pot_value-pot_zero_value);
      sprintf(buf, "diff = %d, distance dif = %d", diff, abs(d-distance));
      Serial.println(buf);
      state = PULLDOWN;
    break;
    case PULLDOWN:
      stepper(-step_count-10);
      state = INITIAL;
    break;
    default:

    Serial.println("You have error in your code");
  }
  sprintf(buf, "step_count = %d, state = %d, analog = %d, key = %d", (int)step_count, (int)state, (int)analogRead(A0), (int)read_key(8));
  Serial.println(buf);
}
