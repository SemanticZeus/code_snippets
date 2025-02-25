#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

float kp = 1.5, ki = 0.1, kd = 0.5;  // Tune these values
float roll, pitch;                   // Assume these are updated from IMU
float rollError, pitchError;
float rollIntegral = 0, pitchIntegral = 0;
float rollPrevError = 0, pitchPrevError = 0;

int minThrottle = 0, maxThrottle = 100;
static unsigned int time;


#define FR 1
#define BL 2
#define FL 3
#define BR 0

int motorFL;
int motorFR;
int motorBL;
int motorBR;


static float euler_angles[3];


int speed_upper_left() {
  return motorFL;
}

int speed_upper_right() {
  return motorFR;
}

int speed_lower_left() {
  return motorBL;
}

int speed_lower_right() {
  return motorBR;
}

void update_pid(int baseThrottle, float roll, float pitch) {
  rollError = 0 - roll;
  pitchError = 0 - pitch;

  float dt = millis() - time;
  dt = dt / 1000;

  // PID Computation
  rollIntegral += rollError * dt;
  pitchIntegral += pitchError * dt;

  float rollDerivative = (rollError - rollPrevError) / dt;
  float pitchDerivative = (pitchError - pitchPrevError) / dt;

  float rollCorrection = kp * rollError + ki * rollIntegral + kd * rollDerivative;
  float pitchCorrection = kp * pitchError + ki * pitchIntegral + kd * pitchDerivative;

  rollPrevError = rollError;
  pitchPrevError = pitchError;

  motorFL = constrain(baseThrottle - rollCorrection + pitchCorrection, minThrottle, maxThrottle);
  motorFR = constrain(baseThrottle + rollCorrection + pitchCorrection, minThrottle, maxThrottle);
  motorBL = constrain(baseThrottle - rollCorrection - pitchCorrection, minThrottle, maxThrottle);
  motorBR = constrain(baseThrottle + rollCorrection - pitchCorrection, minThrottle, maxThrottle);
}


const uint8_t payload_size = 20;
RF24 radio(8, 9);  // CE, CSN

uint8_t address[][6] = { "1Node", "2Node" };
uint8_t radioNumber = 1;


void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1)
      ;
  }
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(payload_size);
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1, address[!radioNumber]);
  //radio.startListening();
  radio.stopListening();
  // For debugging info
  printf_begin();              // needed only once for printing details
  radio.printDetails();        // (smaller) function that prints raw register values
  radio.printPrettyDetails();  // (larger) function that prints human readable data
}

void read_euler() {
  char *command = "angles";
  radio.write(command, strlen(command));
  radio.startListiening();
  delay(10);
  const int size = 100;
  char buffer[size];
  uint8_t pipe;
  if (!radio.available(&pipe)) {
    sprintf(buffer, "error occured while reading angles.");
    goto end;
  }
  radio.read(buffer, size);
  Serial.println(buffer);
end:
  radio.stopListening();
}

int read_command(char *buffer, int size) {
  if (!Serial.available()) return -1;
  String input = Serial.readStringUntil('\n');
  input.trim();
  Serial.println(input);
  if (input=="angles") {
    Serial.println("reading euler angles");
    read_euler();
    return 0;
  } else if (input.substring(0,4)=="stop") {
    input.toCharArray(buffer, size);
    return 0;
  } else if (input=="start") {
    input.toCharArray(buffer, size);
    return 0;
  }
  
  input.toCharArray(buffer, size);
  return 0;
  int n, speed;
  char prefix[20];
  int matched = sscanf(input.c_str(), "%d %d", &n, &speed);
  char text[20];
  sprintf(text, "matched = %d, %s", matched, prefix);
  Serial.println(text);
  if (matched != 2) return -1;
  Serial.println("Valid input received:");
  Serial.print("n: ");
  Serial.println(n);
  Serial.print("speed: ");
  Serial.println(speed);
  input.toCharArray(buffer, size);
  return 0;
}


unsigned long read_float(char *buffer, unsigned long index, float *f)
{
  for (;index<strlen(buffer);++index) {
    char c = buffer[index];
    if (c==' ' || c=='\t' || c=='\n' || c=='\0') continue;
    else break;
  }
  String s;
  for (;index<strlen(buffer);++index) {
    char c = buffer[index];
    if (c==' ' || c=='\t' || c=='\n' || c=='\0') break;
    s+=buffer[index];
  }
  //Serial.println(s.c_str());
  if (s.length()==0) return -1;
  *f = s.toFloat();
  return index;
}

unsigned int speed = 0;

void loop() {
  char command[100] = {};
  if (read_command(command, 100) == 0) {
    radio.write(command, strlen(command));
  }
}