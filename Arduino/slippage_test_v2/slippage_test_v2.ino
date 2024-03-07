#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>

//-------------------------------------------------------------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
void print_message(char *m1, char *m2)
{
  lcd.clear();
  lcd.setCursor(0,0);
  if (m1) lcd.print(m1);
  lcd.setCursor(0,1);
  if (m2) lcd.print(m2);
}

//-------------------------------------------------------------------------------------------------------
enum KeyState {ON_KEY_PRESSED, ON_KEY_RELEASED, KEY_DOWN, KEY_UP};

class Key {
public:
  Key(int k, int t = 200) {
    key = k;
    time = millis();
    state = KEY_UP;
    update_interval = t;
    pinMode(key, INPUT_PULLUP);
  }

  void update() {
    unsigned long long now = millis();
    KeyState k = read_key() ? KEY_UP : KEY_DOWN;
    if (now - time < update_interval) return;
    time = now;
    if (k == KEY_DOWN && state == KEY_UP) state = ON_KEY_PRESSED;
    else if (k == KEY_UP && state == KEY_DOWN) state = ON_KEY_RELEASED;
    else if (k == KEY_DOWN) state = KEY_DOWN;
    else if (k == KEY_UP) state = KEY_UP;
  }

  KeyState get_state() {
    KeyState s = state;
    if (state == ON_KEY_RELEASED) state = KEY_UP;
    else if (state == ON_KEY_PRESSED) state = KEY_DOWN;
    return s;
  }

private:
  int read_key() {
    int count = 0;
    int n = 50;
    for (int i = 0; i < n; i++) {
      count += digitalRead(key);
      delay(1);
    }
    return count > n / 2;
  }

  int key;
  unsigned long long  time;
  unsigned long long update_interval;
  KeyState state;
};
Key key(6);

//-------------------------------------------------------------------------------------------------------
class stepper_motor {
public:
  stepper_motor() : enable{13}, direction{12}, step{11}, speed{0} {
    pinMode(enable, OUTPUT);
    pinMode(direction, OUTPUT);
    pinMode(step, OUTPUT);
    TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(WGM22) | _BV(CS22) | _BV(CS21) | _BV(CS20);
    OCR2A = 20;
    OCR2B = 1;
    digitalWrite(enable, LOW);
  }

  void set_speed(int s) {
    if (s > 100 || s < -100) return;
    if (s == 0) {speed = 0; digitalWrite(enable, LOW); return;}
    digitalWrite(enable, HIGH);
    speed = s;
    digitalWrite(direction, speed < 0 ? LOW : HIGH);
    OCR2A = -abs(speed)/100.0 * (255-15) + 255;
  }

private:
  int step;
  int enable;
  double speed;
  int direction;
};



//-------------------------------------------------------------------------------------------------------
enum STATE {INITIALIZE, FINAL, PULLUP, READY};
stepper_motor stepper;
unsigned long long prev_time;
int state = INITIALIZE;
int pot_value = -1;
int distance = -1;
int pot_zero_value = -1;
int chrono = -1;
char buf[200];

int trig = 10;
int echo = 9;
HCSR04 hc(trig, echo);
int read_distance()
{
  int dist = 0;
  int n = 3;
  for (int i=0;i<n;i++) {
    dist += hc.dist();
    delay(60);
  }
  dist /= n;
  return dist;
}

int analog_read()
{
  int value = 0;
  for (int i=0;i<10;i++) {
    value += analogRead(A0);
  }
  value /= 10;
  return value;
}

void init_device()
{
  snprintf(buf, 200, "initializing...");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(buf);
  lcd.setCursor(0,1);
  snprintf(buf, 200, "please wait.");
  lcd.print(buf);

  delay(500);

  double prev_value = analog_read();
  stepper.set_speed(-100);
  while (1) {
    double current_value = analog_read();
    if (current_value == prev_value) break;
    prev_value = current_value;
    delay(500);
  }
  stepper.set_speed(0);
  pot_zero_value = analog_read();
  state = READY;
  lcd.clear();
}


//-------------------------------------------------------------------------------------------------------
void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(trig, OUTPUT);
  Serial.begin(115200);
  analogReference(EXTERNAL);
  prev_time = millis();
  state = INITIALIZE;
}

int movement_thresh = 2;

void loop() {
  int current_dist = read_distance();
  key.update();
  KeyState key_state = key.get_state();
  switch (state) {
    case INITIALIZE:
      init_device();
      break;
    case READY:
      if (key_state == ON_KEY_RELEASED) {
        state = PULLUP;
        stepper.set_speed(100);
        pot_zero_value = analogRead(A0);
        distance = read_distance();
      } else {
        print_message("Press key to", "start the test.");
      }
      break;
    case PULLUP:
      if (abs(current_dist - distance) > movement_thresh) { state = FINAL; stepper.set_speed(0); }
      snprintf(buf,16, "angle = %d", analog_read()); 
      print_message("please wait ...", buf);
      break;
    case FINAL:  
      pot_value = analogRead(A0);
      int diff = abs(pot_value - pot_zero_value);
      snprintf(buf, 17, "angle=%d", analog_read());
      print_message(buf, "press key to reset.");
      if (key_state == ON_KEY_RELEASED) state = INITIALIZE;
      break;
    default:
      Serial.println("You have error in your code");
  }
}
