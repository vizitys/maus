#include <BleMouse.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

BleMouse bleMouse("Maus");

Adafruit_MPU6050 mpu;

float *bufferX;
float *bufferY;
float *bufferZ;
int bufferSize = 10; // Initial buffer size

float movingAverageFilter(float input, float *buffer, int bufferSizeME)
{
  // Shift buffer values to the left
  for (int i = 0; i < bufferSizeME - 1; i++)
  {
    buffer[i] = buffer[i + 1];
  }

  // Add new input value to buffer
  buffer[bufferSizeME - 1] = input;

  // Calculate moving average
  float sum = 0.0;
  for (int i = 0; i < bufferSizeME; i++)
  {
    sum += buffer[i];
  }
  float output = sum / bufferSizeME;

  return output;
}

void setup()
{
  // Allocate memory for buffer arrays
  bufferX = new float[bufferSize];
  bufferY = new float[bufferSize];
  bufferZ = new float[bufferSize];

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

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
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

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
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
  float filteredX = 0.0;
  float filteredY = 0.0;
  float filteredZ = 0.0;

  unsigned long startTime;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float stdDev = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));

  if (stdDev > 1.0)
  {
    bufferSize = 20;
  }
  else
  {
    bufferSize = 10;
  }

  filteredX = movingAverageFilter(a.acceleration.x, bufferX, bufferSize);
  filteredY = movingAverageFilter(a.acceleration.y, bufferY, bufferSize);
  filteredZ = movingAverageFilter(a.acceleration.z, bufferZ, bufferSize);

  // Output normalized values
  Serial.print("Normalized Acceleration X: ");
  Serial.print(filteredX);
  Serial.print(", Y: ");
  Serial.print(filteredY);
  Serial.print(", Z: ");
  Serial.println(filteredZ);

  if (bleMouse.isConnected())
    bleMouse.move(filteredY, filteredZ);
}
