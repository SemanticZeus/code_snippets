#define dir 2
#define step 3
#define rst 4
#define sleep 5

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

void setup() {
  // put your setup code here, to run once:
  for (int i=2;i<=5;i++) pinMode(i, OUTPUT);
  Serial.begin(115200);
  digitalWrite(sleep, HIGH);
  digitalWrite(rst, HIGH);
  digitalWrite(6,HIGH);
}

void loop() {
  char comand[10];
  //read_serial(command, 10);
  //int speed = atoi(command);
  //if (speed<0) digitialWrite(dir, HIGH); else ditialWrite(dir, LOW);
  //speed = abs(speed);
    digitalWrite(step, HIGH);
    delay(1);
    digitalWrite(step, LOW);
    delay(1);
}
