#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

MPU6050 mpu;


int const INTERRUPT_PIN = 2;  // Define the interruption #0 pin
bool blinkState;

/*---MPU6050 Control/Status Variables---*/
bool DMPReady = false;  // Set true if DMP init was successful
uint8_t MPUIntStatus;   // Holds actual interrupt status byte from MPU
uint8_t devStatus;      // Return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // Expected DMP packet size (default is 42 bytes)
uint8_t FIFOBuffer[64]; // FIFO storage buffer

/*---Orientation/Motion Variables---*/ 
Quaternion q;           // [w, x, y, z]         Quaternion container
VectorInt16 aa;         // [x, y, z]            Accel sensor measurements
VectorInt16 gy;         // [x, y, z]            Gyro sensor measurements
VectorInt16 aaReal;     // [x, y, z]            Gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            World-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            Gravity vector
static float euler[3];         // [psi, theta, phi]    Euler angle container
static float ypr[3];           // [yaw, pitch, roll]   Yaw/Pitch/Roll container and gravity vector


/*------Interrupt detection routine------*/
volatile bool MPUInterrupt = false;     // Indicates whether MPU6050 interrupt pin has gone high
void DMPDataReady() {
  MPUInterrupt = true;
}

void imu6050_setup() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment on this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  /*Initialize device*/
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  /*Verify connection*/
  Serial.println(F("Testing MPU6050 connection..."));
  if(mpu.testConnection() == false){
    Serial.println("MPU6050 connection failed");
    while(true);
  }
  else {
    Serial.println("MPU6050 connection successful");
  }

  /* Initializate and configure the DMP*/
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  /* Supply your gyro offsets here, scaled for min sensitivity */
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setXAccelOffset(0);
  mpu.setYAccelOffset(0);
  mpu.setZAccelOffset(0);

  /* Making sure it worked (returns 0 if so) */ 
  if (devStatus == 0) {
    mpu.CalibrateAccel(6);  // Calibration Time: generate offsets and calibrate our MPU6050
    mpu.CalibrateGyro(6);
    Serial.println("These are the Active offsets: ");
    mpu.PrintActiveOffsets();
    Serial.println(F("Enabling DMP..."));   //Turning ON DMP
    mpu.setDMPEnabled(true);

    /*Enable Arduino interrupt detection*/
    Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
    Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
    Serial.println(F(")..."));
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), DMPDataReady, RISING);
    MPUIntStatus = mpu.getIntStatus();

    /* Set the DMP Ready flag so the main loop() function knows it is okay to use it */
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    DMPReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize(); //Get expected DMP packet size for later comparison
  } 
  else {
    Serial.print(F("DMP Initialization failed (code ")); //Print the error code
    Serial.print(devStatus);
    Serial.println(F(")"));
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
  }
  pinMode(LED_BUILTIN, OUTPUT);
}

void update_imu6050() {
  if (!DMPReady) {
    Serial.println("Error IMU failed. calling IMU setup again.");
    imu6050_setup();
    return; // Stop the program if DMP programming fails.
  }
  /* Read a packet from FIFO */
  if (mpu.dmpGetCurrentFIFOPacket(FIFOBuffer)) {
      mpu.dmpGetQuaternion(&q, FIFOBuffer);
      mpu.dmpGetEuler(euler, &q);

  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState);
  }
}


void floattostring(float f, char* buffer)
{
  int sign = f>=0 ? 1 : -1;
  f = fabsf(f);
  unsigned int d = (unsigned int) f;
  f-=d;
  f*=100.0f;
  unsigned int dd = (unsigned int) f;
  if (sign > 0) sprintf(buffer, "%u.%u", d, dd);
  else sprintf(buffer, "-%u.%u", d, dd);
  return buffer;
}

float get_pitch()
{
  return  euler[1] * 180/M_PI;
}

float get_yaw()
{
  return  euler[0] * 180/M_PI;
}

float get_roll()
{
  return  euler[2] * 180/M_PI;
}

String get_imu_data()
{
  char buffer[50];
    char roll_s[10];
    char pitch_s[10];
    char yaw_s[10];
    floattostring(euler[2]* 180/M_PI, roll_s);
    floattostring(euler[1]* 180/M_PI, pitch_s);
    floattostring(euler[0]* 180/M_PI, yaw_s);
    sprintf(buffer, "%s %s %s", roll_s, pitch_s, yaw_s);
    return String(buffer);
}