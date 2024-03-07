const int outputPin = 9; // Pin 9 is OC1A

void setup() {
  Serial.begin(115200);
  // Set pin 9 as an output
  pinMode(outputPin, OUTPUT);
  // Configure Timer 1 for Fast PWM mode, 8-bit
  // Set WGM bits to Fast PWM, 8-bit (WGM13 = 0, WGM12 = 1, WGM11 = 0, WGM10 = 1)
  TCCR1A |= (1 << WGM10) | (1 << WGM12);
  // Set COM1A1 bit to 1 (Clear OC1A on Compare Match, set OC1A at BOTTOM)
  TCCR1A |= (1 << COM1A1);
  // Set prescaler to 1 (CS12 = 0, CS11 = 0, CS10 = 1)
  TCCR1B |= (1 << CS10);
  //TCCR1B |= (1 << CS11) | (1 << CS10);
  // Set initial duty cycle to 50%
  OCR1A = 127; // 50% of 255
  // Optional: Disable Timer 1 interrupts
  TIMSK1 = 0;
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(4, OUTPUT);
  TCCR2A =  _BV(COM2B1) | _BV(WGM20) | _BV(WGM21);
  TCCR2B =  _BV(CS20) | _BV(WGM22);
  OCR2A = 6;
  OCR2B = 3;
}

void loop() {
}
