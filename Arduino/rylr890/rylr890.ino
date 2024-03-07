void setup()
{
  // put your setup code here, to run once:
  Serial.begin (115200);
  Serial1.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void flush_serial1()
{
  char ch;
  int i = 0;
  int valid = 0;
  unsigned long long t = millis();
  while(1) {
    while(!Serial1.available()) if (millis() - t > 10) return;
    ch = Serial1.read();
  }
}

static char answer[50];
int send_command(const char *command)
{
  const int buffer_size = 100;
  char ch;
  int i = 0;
  int valid = 0;
  unsigned long long t = millis();

  flush_serial1();
  Serial1.print(command);
  Serial1.print("\r\n");

  for (;i<buffer_size-1;) {
    while(!Serial1.available()) if (millis() - t > 10) return -1;
    ch = Serial1.read();
    if ((ch == '\n' || ch == '\r') && ! valid) continue;
    if ((ch == '\n' || ch == '\r') && valid) break;
    answer[i++] = ch;
    valid = 1;
  }
  answer[i] = '\0';
  return 0;
}

int serial_read_line(char *buffer, int size)
{
  char ch;
  int i = 0;
  int valid = 0;
  unsigned long long t = millis();
  for (;i<size-1;) {
    while(!Serial.available()) if (millis() - t > 10) return 0;
    ch = Serial.read();
    if ((ch == '\n' || ch == '\r') && ! valid) continue;
    if ((ch == '\n' || ch == '\r') && valid) break;
    buffer[i++] = ch;
    valid = 1;
  }
  buffer[i]= '\0';
  return i;
}

int counter = 0;
char counter_str[4];
char *get_counter()
{
  int c = counter++;
  for (int i=0;i<3;i++) {
    counter_str[2-i] = c % 10 + '0';
    c /=10;
  }
  counter_str[3] = 0;
  return counter_str;
}

void loop()
{
  char buffer[150];
  int receiver_address = 3;
  int len;

  if (len = serial_read_line(buffer, 150)) {
    if (send_command(buffer) == -1) Serial.println("an Error occured");
    Serial.println(answer);
  } else {
    char *message = "Heather Mason, SILENT HILL 3";
    sprintf(buffer, "AT+SEND=2,%d,%s_%s",strlen(message)+4, get_counter(), message);
    send_command(buffer);
    Serial.println(buffer);
    delay(200);
  }
  if (Serial1.available()) Serial.println(Serial1.readString());
}