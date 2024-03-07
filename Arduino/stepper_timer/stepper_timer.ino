#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int dirPin = 12;
const int stepPin = 11;
const int enablePin = 13;
int trig = 10;
int echo = 9;

HCSR04 hc(trig, echo);

typedef void (*callback_function)(void);

#define NUM_CALLBACKFUNCTIONS 5
callback_function callback_functions[NUM_CALLBACKFUNCTIONS];

int interruptInterval = 1; // 1 millisecond interval


int stepper_speed = 100;
int step_intervals = 1;
unsigned long long stepper_prev_timer = 0;
void set_stepper_speed(int speed)
{
  if (speed>100 || speed < -100) return;
  if (speed < 0) digitalWrite(dirPin, LOW); else digitalWrite(dirPin, HIGH); 
  stepper_speed = abs(speed);
  step_intervals = -stepper_speed/100.0 * 10.0 + 10 + 1;
}

int step_value = HIGH;
void stepper_callback(void)
{
  if (stepper_speed == 0) {
    digitalWrite(enablePin, LOW);
    return;
  }
  digitalWrite(enablePin, HIGH);
  unsigned long long cur = millis();
  if (stepper_prev_timer+step_intervals > cur) return;
  stepper_prev_timer = cur;
  step_value = step_value == HIGH ? LOW : HIGH;
  digitalWrite(stepPin, step_value);
}

double analog_voltage = 0;
void analog_read_callback()
{
  int value = 0;
  for (int i=0;i<10;i++) {
    value += analogRead(A0);
  }
  value /= 10;
  analog_voltage = (double)value /1024.0 * 3.3;
}

void setup_timer() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10); 
  set_stepper_speed(100);
  TIMSK1 |= (1 << OCIE1A);
  OCR1A = 250; // (16 MHz / (64 * 1000 Hz)) - 1
  sei();
}

ISR(TIMER1_COMPA_vect) {
  for (int i=0;i<NUM_CALLBACKFUNCTIONS;i++) 
    if (callback_functions[i]) callback_functions[i]();
}

void setup()
{
  lcd.init();
  lcd.backlight();
  pinMode(trig, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  pinMode(echo, INPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  callback_functions[0] = stepper_callback;
  setup_timer();
  Serial.begin(115200);
  analogReference(EXTERNAL);
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

void loop()
{
  char command[10];
  int n = read_serial(command, 10);
  if (n) {
    Serial.println(command);
    set_stepper_speed(atoi(command));
  }
  analog_read_callback();
  lcd.setCursor(0,0);
  lcd.clear();
  char buffer[16];
  snprintf(buffer, 16, "s=%d, A0=%d.%d", stepper_speed, (int)analog_voltage, (int)((analog_voltage-(int)analog_voltage)*100));
  lcd.print(buffer);
  snprintf(buffer, 16, "d=%d", (int)hc.dist());
  lcd.setCursor(0,1);
  lcd.print(buffer);
}