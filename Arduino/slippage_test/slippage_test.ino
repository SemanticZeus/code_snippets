#include <HCSR04.h>

int stepper_pins[] = {4,5,6,7};
int trig = 12;
int echo = 13;

HCSR04 hc(12,13); //initialisation class HCSR04 (trig pin , echo pin)


enum STATE {INITIAL, FINAL, PULLUP, PULLDOWN, UNDETERMINED};

unsigned long time;

int state = INITIAL;
int pot_value = -1;
int distance = -1;
int pot_zero_value = -1;
int chrono = -1;
char buf[200];
int step_count = 0;
int stepper_step_len = 10;

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

int read_distance()
{
  int dist = 0;
  int n = 10;
  for (int i=0;i<n;i++) {
    dist += hc.dist();
    delay(60);
  }
  dist /= n;
  return dist;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int i=0;i<4;i++) pinMode(stepper_pins[i], OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
//
//    // put your setup code here, to run once:
//  digitalWrite(LED_BUILTIN, 0);
//  pinMode(LED_BUILTIN, OUTPUT);
   //analogReference(EXTERNAL);
   time = millis();
  
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
  int current_dist = read_distance();
  switch (state) {
    case UNDETERMINED:

    break;
    case INITIAL:
      if (read_key(8)) state = PULLUP;
      pot_zero_value = analogRead(A0);
      distance = read_distance();
      step_count = 0;
    break;
    case PULLUP:
      stepper(stepper_step_len);
      step_count += stepper_step_len;
      current_dist = read_distance();
      if (abs(current_dist-distance)>100) state = FINAL;
    break;
    case FINAL:
      pot_value = analogRead(A0);
      delay(1000);
      int diff = abs(pot_value-pot_zero_value);
      sprintf(buf, "diff = %d, distance dif = %d", diff, abs(current_dist-distance));
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
  if (millis()-time<500) return;
  time = millis();
  sprintf(buf, "step_count = %d, state = %d, analog = %d, key = %d, current_dist = %d", 
            (int)step_count, (int)state, (int)analogRead(A0), (int)read_key(8), (int) current_dist);
  Serial.println(buf);
}