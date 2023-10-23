#include <BleMouse.h>

BleMouse bleMouse;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

void loop()
{
  if (bleMouse.isConnected())
  {

    unsigned long startTime;

    Serial.println("Scroll up");
    startTime = millis();
    while (millis() < startTime + 2000)
    {
      bleMouse.move(0, 0, 1);
      delay(100);
    }
    delay(500);

    Serial.println("Scroll down");
    startTime = millis();
    while (millis() < startTime + 2000)
    {
      bleMouse.move(0, 0, -1);
      delay(100);
    }
    delay(500);

    Serial.println("Left click");
    bleMouse.click(MOUSE_LEFT);
    delay(500);

    Serial.println("Right click");
    bleMouse.click(MOUSE_RIGHT);
    delay(500);

    Serial.println("Move mouse pointer up");
    startTime = millis();
    while (millis() < startTime + 2000)
    {
      bleMouse.move(0, -1);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer down");
    startTime = millis();
    while (millis() < startTime + 2000)
    {
      bleMouse.move(0, 1);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer left");
    startTime = millis();
    while (millis() < startTime + 2000)
    {
      bleMouse.move(-1, 0);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer right");
    startTime = millis();
    while (millis() < startTime + 2000)
    {
      bleMouse.move(1, 0);
      delay(100);
    }
    delay(500);
  }
}
