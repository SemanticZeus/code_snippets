enum KeyState { ON_KEY_PRESSED, ON_KEY_RELEASED, KEY_DOWN, KEY_UP};

typedef void (*callback_function)(void);

class Key {
public:
  Key(int k, int t = 200) {
    key = k;
    time = millis();
    state = KEY_UP;
    update_interval = t;
    for(int i=0;i<2;i++) key_callback_functions[0] = NULL;
  }

  void update() {
    unsigned long now = millis();
    KeyState s = read_key() ? KEY_UP : KEY_DOWN;
    if (now - time < update_interval) return;
    time = now;
    switch(s) {
      case KEY_UP:
        if (state == KEY_DOWN) key_callback_functions[ON_KEY_RELEASED]();
        state = KEY_UP;
      break;
      case KEY_DOWN:
      if (state == KEY_UP) key_callback_functions[ON_KEY_PRESSED]();
        state = KEY_DOWN;
      break;
    }
  }

  void register_callback_function(callback_function f, KeyState s)
  {
    if (s != ON_KEY_PRESSED && s !=ON_KEY_RELEASED) return;
    key_callback_functions[s] = f;
  }

  KeyState get_state() {
    return state;
  } 

  int read_key() {
    pinMode(key, INPUT_PULLUP);
    int count = 0;
    int n = 50;
    for (int i = 0; i < n; i++) {
      count += digitalRead(key);
      delay(1);
    }
    return count > n / 3;
  }

private:
  int key;
  unsigned long time;
  int update_interval;
  KeyState state;
  callback_function key_callback_functions[2];
};

int state = 0;
void on_reset_key_pressed(void)
{
  Serial.println("reset key pressed");
  state = 1;
}

void on_reset_key_released(void)
{
  Serial.println("reset key released");
  state = 0;
}

Key key(8);
unsigned long time;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  key.register_callback_function(on_reset_key_pressed, ON_KEY_PRESSED);
  key.register_callback_function(on_reset_key_released, ON_KEY_RELEASED);
  time = millis();
  pinMode(LED_BUILTIN, OUTPUT);
}

int led = 0;
void loop() {
  key.update();
  if (millis()-time<50 || state == 0) return;
  time = millis();
  led=1-led;
  digitalWrite(LED_BUILTIN, led);
}
