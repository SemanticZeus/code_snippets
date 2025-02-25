#include "DShot.h"
#include "pid.h"
#include "euler_angles.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>


DShot *escs[4];
int num_escs = 4;
int esc_pins[] = {2,3,4,5};

RF24 radio(8,9);
uint8_t radioNumber = 0;
uint8_t address[][6] = { "1Node", "2Node" };
const uint8_t payload_size = 20;
int throttle[4] = {100, 100, 100, 100};
int speeds[4] = {100, 100, 100, 100};

int base_speed = 100;
int base_throttle = 100;

void init_nrf() {
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  }
  Serial.print(F("radioNumber = "));
  Serial.println(radioNumber);
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(payload_size);
  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

  // set the RX address of the TX node into a RX pipe
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1
  radio.startListening();
  delay(5);  // Allow time for pipes to initialize

  
    // For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  radio.printPrettyDetails(); // (larger) function that prints human readable data
}

void send_euler_angle()
{
  radio.stopListening();
  delay(5);
  String s = get_imu_data();
  Serial.print("imu data = ");
  Serial.println(s);
  radio.write(s.c_str(), s.length());
  radio.startListening();
}

int read_command() {
  const int size = 100;
  char buffer[size];
  uint8_t pipe;
  if (!radio.available(&pipe)) return -1;
  radio.read(buffer, size);
  //Serial.println(buffer);
  if (String(buffer).substring(0, 4)=="stop") {
    Serial.println("stop");
    int n;
    int matched = sscanf(buffer, "stop %d", &n);
    if (matched==1 && n>=0 && n < 4) pinMode(esc_pins[n], INPUT);
    else 
      for (int i=0;i<4;i++) {
        pinMode(esc_pins[i], INPUT);
      }
    return 0;
  } else if (String(buffer).substring(0, 5)=="start") {
    Serial.println("start");
    for (int i=0;i<4;i++) {
      pinMode(esc_pins[i], OUTPUT);
    }
    return 0;
  }
  int n,speed;
  int matched = sscanf(buffer, "%d %d", &n, &speed);
  if (matched != 2) return -1;
  if (n<0 || n>8 || speed < -2 || speed > 100) return -1;
  //Serial.println("Valid input received:");
  if (n==5) {
    send_euler_angle();
  } else if (n==4) {
    for (int i=0;i<4;i++) speeds[i] = 100+5*speed;
  } else if (n==6) {
    
    for (int i=0;i<4;i++) { 
      int s = speeds[i]+5*speed;
      if (s<100) s = 100; else if (s>600) s = 600; 
      speeds[i] += s;
    }

  } else if (n==7) {
    
    int s[4];
    int matched = sscanf(buffer, "%d %d %d %d %d", &n, s, s+1, s+2, s+3);
    if (matched!=5) return -1;
    Serial.println("case 7");
    for (int i=0;i<4;i++) {
      int ss = speeds[i]+5*s[i];
      if (ss<100) ss = 100; else if (ss>600) ss = 600; 
      speeds[i] = ss;
      char buf[10];
      sprintf(buf, "s[%d] = %d", i, speeds[i]);
      Serial.println(buf);
    }
  } else if (n==8) {
    
    int s[4];
    int matched = sscanf(buffer, "%d %d %d %d %d", &n, s, s+1, s+2, s+3);
    if (matched!=5) return -1;
    Serial.println("case 7");
    for (int i=0;i<4;i++) {
      int ss = speeds[i]-5*s[i];
      if (ss<100) ss = 100; else if (ss>600) ss = 600; 
      speeds[i] = ss;
      char buf[10];
      sprintf(buf, "s[%d] = %d", i, speeds[i]);
      Serial.println(buf);
    }
  } else {
    if (speed == -1) {
      speeds[n] = 0;
    } else speeds[n] = 100+5*speed;
  }
  return 0;
}

/*

redefine DSHOT_PORT if you want to change the default PORT

Defaults
UNO: PORTD, available pins 0-7 (D0-D7)
Leonardo: PORTB, available pins 4-7 (D8-D11)

e.g.
#define DSHOT_PORT PORTD
*/


void setup() {
  Serial.begin(115200);
  while(!Serial);
  imu6050_setup();
  init_nrf();
  for (int i=0;i<num_escs;i++) {
    escs[i] = new DShot(DShot::Mode::DSHOT150);
    escs[i]->attach(esc_pins[i]);
  }  
}

static unsigned long time = 0;

void loop2() {
  if (millis()-time>10) {
    for (int i=0;i<num_escs;i++) {
      if (throttle[i]<speeds[i]) {
        ++throttle[i];
        //Serial.println(throttle[i]);
      }
      else if (throttle[i]>speeds[i]) --throttle[i];
      escs[i]->setThrottle(throttle[i]);
    }
    time = millis();
    
  }

  update_imu6050();
  read_command();
  if (Serial.available()) {
    char buf[100];
    for (int i=0;i<4;i++) {
      sprintf(buf, "speed[%d] = %d, throttle[%d]=%d", i, speeds[i], i, throttle[i]);
      Serial.println(buf);
    }
    Serial.readBytesUntil('\n', buf, 100);
  }
  
}

int read_commands2() {
  const int size = 100;
  char buffer[size];
  uint8_t pipe;
  if (!radio.available(&pipe)) return -1;
  radio.read(buffer, size);
  String command(buffer);
  Serial.println(buffer);
  if (command.substring(0, 4)=="stop") {
    Serial.println("stop");
    int n;
    int matched = sscanf(buffer, "stop %d", &n);
    if (matched==1 && n>=0 && n < 4) pinMode(esc_pins[n], INPUT);
    else 
      for (int i=0;i<4;i++) {
        pinMode(esc_pins[i], INPUT);
      }
    return 0;
  } else if (command.substring(0, 5)=="start") {
    Serial.println("start");
    for (int i=0;i<4;i++) {
      pinMode(esc_pins[i], OUTPUT);
    }
    return 0;
  } else if (command.substring(0,5)=="speed") {
    int speed;
    int matched = sscanf(buffer, "speed %d", &speed);
    if (matched != 1 || speed<0 || speed>100) return -1;
    Serial.println(speed);
    base_speed = 100 + 5*speed;
  } else if (command == "angles") {
    Serial.println("angles: ");
    send_euler_angle();
  }
  return 0;
}


void loop() {
  if (millis()-time>10) {
    if (base_throttle<base_speed) base_throttle++;
    else if (base_throttle>base_speed) base_throttle--;
    update_pid(base_throttle, get_roll(), get_pitch());
    for (int i=0;i<num_escs;i++) {
      escs[i]->setThrottle(get_speed(i));
    }
    char buffer[50];
    sprintf(buffer, "%d %d %d %d", get_speed(0), get_speed(1), get_speed(2), get_speed(3));
    //Serial.println(buffer);
    time = millis();
    
  }
  
  update_imu6050();
  read_commands2();
  if (Serial.available()) {
    char buf[100];
    for (int i=0;i<4;i++) {
      sprintf(buf, "speed[%d] = %d, throttle[%d]=%d", i, speeds[i], i, throttle[i]);
      Serial.println(buf);
    }
    Serial.readBytesUntil('\n', buf, 100);
  }
  
}

