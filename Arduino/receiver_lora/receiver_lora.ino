#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

void print(char *message)
{
  if(Serial) Serial.println(message);
}

void lcd_print(char *message, int x, int y)
{
  lcd.setCursor(x,y);
  lcd.print(message);
}

void flush_serial1()
{
  char ch;
  int i = 0;
  int valid = 0;
  unsigned long long t = millis();
  while(1) {
    while(!Serial1.available()) if (millis() - t > 50) return;
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
  answer[0] = '\0';

  for (;i<buffer_size-1;) {
    while(!Serial1.available()) if (millis() - t > 1000) return -1;
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
    while(!Serial.available()) if (millis() - t > 50) return 0;
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

void init_lora()
{
  send_command("AT+ADDRESS=2"); print(answer);
  send_command("AT+PARAMETER=7,3,4,5"); print(answer);
  send_command("AT+NETWORKID=0"); print(answer);
  send_command("AT+PARAMETER?"); print(answer);
  send_command("AT+BAND?"); print(answer);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin (115200);
  Serial1.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  init_lora();
  lcd.init();
  lcd.backlight();
}


int lora_received_data(char *buffer, int size)
{
  char ch;
  int i = 0;
  int valid = 0;
  unsigned long long t = millis();
  for (;i<size-1;) {
    while(!Serial1.available()) if (millis() - t > 5000) return 0;
    ch = Serial1.read();
    if ((ch == '\n' || ch == '\r') && ! valid) continue;
    if ((ch == '\n' || ch == '\r') && valid) break;
    buffer[i++] = ch;
    valid = 1;
  }
  buffer[i]= '\0';
  return i;
}

void loop()
{
  char buffer[200];
  int receiver_address = 3;

  if (!Serial1.available()) return;
  int len = lora_received_data(buffer, 200);
  if (strncmp(buffer, "+RCV=",5)) return;

  int m=-1,n=-1,message_len = 0;
  for (int i=0;i<len;i++) {
    char ch = buffer[i];
    if (ch == ',' && m == -1) m = i;
    else if (ch == ',' && n == -1) n = i;
    if (m!=-1 && n!=-1) {
      for (int i=m+1;i<n;i++) message_len = message_len*10+buffer[i]-'0';
      break;
    }
  }
  buffer[n+1+message_len]='\0';
  lcd_print(buffer+n+1, 0, 0);
  Serial.println(buffer);
}