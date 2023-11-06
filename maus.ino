#include <BleMouse.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

BleMouse bleMouse("Maus", "Maus", 69);

Adafruit_MPU6050 mpu;

const int scanFrequency = 70; // Hz
const int sensitivity = 100;

const int leftBtn = 18;
const int rightBtn = 19;

// variables to keep track of how long each button has been held for
int startPressedL = 0;
int startPressedR = 0;
int endPressedL = 0;
int endPressedR = 0;

const int calibrationThreshold = 5000; // ms, how long to hold both buttons for calibration

int calibrationX = 0;
int calibrationY = 0;

void setup()
{
  pinMode(leftBtn, INPUT);
  pinMode(rightBtn, INPUT);

  Serial.begin(115200);

  while (!Serial)
    delay(10);

  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(500);

  Serial.println("Starting BLE");
  bleMouse.begin();
}

void loop()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  double vx = g.gyro.y * sensitivity - calibrationX;
  double vy = -g.gyro.z * sensitivity - calibrationY;

  Serial.print("X: ");
  Serial.print(g.gyro.x);
  Serial.print(" Y: ");
  Serial.print(vx);
  Serial.print(" Z: ");
  Serial.println(vy);

  if (digitalRead(leftBtn) == HIGH)
  {
    bleMouse.press(MOUSE_LEFT);
    startPressedL = millis();
  }
  else if (bleMouse.isPressed(MOUSE_LEFT))
  {
    bleMouse.release(MOUSE_LEFT);
    Serial.println("MOUSE LEFT CLICKED");
    endPressedL = millis();
  }

  if (digitalRead(rightBtn) == HIGH)
  {
    bleMouse.press(MOUSE_RIGHT);
    startPressedR = millis();
  }
  else if (bleMouse.isPressed(MOUSE_RIGHT))
  {
    bleMouse.release(MOUSE_RIGHT);
    Serial.println("MOUSE RIGHT CLICKED");
    endPressedR = millis();
  }

  int holdTimeL = startPressedL - endPressedL;
  int holdTimeR = startPressedR - endPressedR;

  Serial.println(holdTimeL);
  Serial.println(holdTimeR);

  if (holdTimeL > calibrationThreshold && holdTimeR > calibrationThreshold)
  {
    Serial.println("CALIBRATING");
    calibrationX = vx;
    calibrationY = vy;
  }

  bleMouse.move(vx, vy);

  delay(1000 / scanFrequency);
}