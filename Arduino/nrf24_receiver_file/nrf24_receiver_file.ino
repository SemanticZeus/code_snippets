#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

struct Package {
  int8_t number;
  uint8_t data[31];
}__attribute__((packed));

RF24 radio(7, 8, 1000000);  // CE, CSN

const byte address[6] = "1SNSR";

void setup() {
  Serial.begin(115200);
  while (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    delay(1000);
  }

  radio.setDataRate(RF24_250KBPS);

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  //radio.setPayloadSize(32);
  radio.stopListening();
  Serial.println("Transmitter");
}

#define ACK -1
#define END_OF_PACKAGES -2
#define SEND_NUMBER_OF_PACKAGES -3
#define TIMEOUT -4
#define COMMUNICATION_ERROR -5;

unsigned long package_counter = 0;

int read_package(Package *p)
{
  radio.startListening();
    unsigned long long start = millis();
    while (millis()-start<500) if (radio.available()) break;
    if (!radio.available()) return TIMEOUT;
    radio.read(p, 32);
    return 0;
}

int send_command_wait_ack_nrf24(Package *p)
{
  Package m;
  unsigned long long start = millis();
  while (1) {
    radio.stopListening();
    radio.write(p, 32);
    delay(5);
    radio.startListening();
    if (millis() - start > 500) return TIMEOUT;
    if (read_package(&m)) continue;
    if (m.number != p->number) continue;
    else break;
  }
  memcpy(p, &m, 32);
  return m.number;
}

int transmit_nrf24(Package *p, int n) {
  radio.stopListening();
  Package m;
  unsigned long long start = millis();

  m.number = SEND_NUMBER_OF_PACKAGES;
  m.data[0] = n;
  if (send_command_wait_ack_nrf24(&m)!=m.number) return TIMEOUT;

  uint8_t waiting[31];
  memset(waiting, 1, 31);
  radio.stopListening();
  for (int i=0;i<n;i++) p[i].number = package_counter++ % 31;
  
  int count = 0;
  for(int t=0;t<10;t++) {
    count = 0;
    for (int i = 0; i < n; i++) {
      if (waiting[i]==0) {count++; continue;}
      radio.write(&p[n], 32);
      delay(1);
    }
    if (count == n) break;
    m.number = END_OF_PACKAGES;
    if(send_command_wait_ack_nrf24(&m)!=m.number) return TIMEOUT;
    memcpy(waiting, m.data, 31);
    radio.stopListening();
  }
TRANSMIT_EXIT:
  if (count<n) return -1;
  return 0;
}

void send_package(Package *p)
{
  radio.stopListening();
  radio.write(p, 32);
}

const int buffer_size = 2 * 16 + 1;
char buffer[buffer_size];
unsigned int counter = 0;
const int max_num_packages = 31;
Package packages[max_num_packages];

int receive_nrf24(Package *p)
{
  Package m;
  int n;
  radio.startListening();
  unsigned long long start = millis();
  Package ack;
  memset(&ack,0, 32);
  ack.number = SEND_NUMBER_OF_PACKAGES;

  while (1) {
    if (read_package(&m)) return TIMEOUT;
    if (m.number !=SEND_NUMBER_OF_PACKAGES) continue;
    n = m.data[0];
    send_package(&m);
    break;
  }
  
  start = millis();
  while (millis()-start<500) {
    for (int i=0;i<n;i++) {
      if (read_package(&m)) return TIMEOUT;
      if (m.number < 0) break;
      if (m.number > 31) return COMMUNICATION_ERROR;
      memcpy(&p[m.number], &m, 32);
      ack.data[m.number] = 1;
    }
    //if (m.number!=END_OF_PACKAGES) return COMMUNICATION_ERROR;
    int count = 0;
    for (int i=0;i<n;i++) if (ack.data[i]) count++;
    send_package(&ack);
    if (count == n) return n;
  }
  return TIMEOUT;
}

void loop() {
  int n = receive_nrf24(packages);
  for (int i=0;i<n;i++) {
    char message[40];
    snprintf(message, 40, "%d = %s", packages[i].number, packages[i].data);
    Serial.println(message);
  }
}
