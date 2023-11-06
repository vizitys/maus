#include <BleMouse.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

BleMouse bleMouse("Maus", "Maus", 69);

Adafruit_MPU6050 mpu;

const int scanFrequency = 1000; // Hz
const int sensitivity = 100;
const int num_samples = 10;

const int leftBtn = 18;
const int rightBtn = 19;

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
  Serial.print("Non-normalized Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.println(a.acceleration.z);

  static float x_acc_avg = 0;
  static float y_acc_avg = 0;
  static int count = 0;

  if (bleMouse.isConnected())
  {
    // Calculate moving average of acceleration values
    x_acc_avg = (x_acc_avg * count + a.acceleration.y) / (count + 1);
    y_acc_avg = (y_acc_avg * count + a.acceleration.y) / (count + 1);
    count++;

    if (count > num_samples)
    {
      // Adjust mouse movement based on rotation and moving average
      int x = (x_acc_avg - (g.gyro.x * 0.1)) * -sensitivity;
      int y = (y_acc_avg + (g.gyro.y * 0.1)) * sensitivity;

      if (abs(x) < 5)
        x = 0;
      if (abs(y) < 5)
        y = 0;

      Serial.print("X: ");
      Serial.print(x);
      Serial.print(", Y: ");
      Serial.println(y);

      bleMouse.move(x, y);

      // Reset moving average and count
      x_acc_avg = 0;
      y_acc_avg = 0;
      count = 0;
    }

    if (digitalRead(leftBtn) == HIGH)
    {
      Serial.println("MOUSE LEFT CLICKED");
      bleMouse.click(MOUSE_LEFT);
    }

    if (digitalRead(rightBtn) == HIGH)
    {
      Serial.println("MOUSE RIGHT CLICKED");
      bleMouse.click(MOUSE_RIGHT);
    }
  }

  delay(1000 / scanFrequency);
}