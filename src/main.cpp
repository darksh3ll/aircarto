//
// ─── IMPORT LIBRAIRIES ──────────────────────────────────────────────────────────
//

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SDS011.h>
#include <SimpleDHT.h>
#include "MHZ19.h"
#include <SoftwareSerial.h>

//
// ─── DEFINE PIN ─────────────────────────────────────────────────────────────────
//

#define BUZZER_PIN 4 //buzzer
#define BAUDRATE 9600

//configure capteur DHT11
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);

LiquidCrystal_I2C lcd(0x27, 20, 4);

float p10, p25;
int error;

SDS011 my_sds;

//
// ─── CONFIGURE LED ──────────────────────────────────────────────────────────────
//

//led pm2.5
int LED_GREEN_P25 = 22;
int LED_YELLOW_P25 = 24;
int LED_RED_P25 = 26;

//led pm10
int LED_GREEN_P10 = 28;
int LED_YELLOW_P10 = 30;
int LED_RED_P10 = 32;

//led temperature
int LED_GREEN_TEMPERATURE = 34;
int LED_YELLOW_TEMPERATURE = 36;
int LED_RED_TEMPERATURE = 38;

//led HUMLIDITY
int LED_GREEN_HUMIDITY = 40;
int LED_YELLOW_HUMIDITY = 42;
int LED_RED_HUMIDITY = 44;

//
// ─── ENUM ───────────────────────────────────────────────────────────────────────
//

enum NIVEAU
{
  GOOD,
  FAIR,
  DANGER
};

//
// ─── FUNCTION ───────────────────────────────────────────────────────────────────
//

int sensorLevel(int valueGood, int valueFair, int valueReel)
{
  if (valueReel <= valueGood)
  {
    return GOOD;
  }
  else if (valueReel > valueGood && valueReel <= valueFair)
  {
    return FAIR;
  }
  else
  {
    return DANGER;
  }
};

void lightLed(int led)
{
  if (led == GOOD)
  {
    digitalWrite(led, LOW);
    digitalWrite(led, LOW);
    digitalWrite(led, HIGH);
  };
  if (led == FAIR)
  {
    digitalWrite(led, LOW);
    digitalWrite(led, LOW);
    digitalWrite(led, HIGH);
  };
  if (led == DANGER)
  {
    digitalWrite(led, LOW);
    digitalWrite(led, LOW);
    digitalWrite(led, HIGH);
  }
};
// check température
int checkTemperature(int temp)
{
  if (temp >= 18 && temp <= 22)
  {
    digitalWrite(LED_RED_TEMPERATURE, LOW);
    digitalWrite(LED_YELLOW_TEMPERATURE, LOW);
    digitalWrite(LED_GREEN_TEMPERATURE, HIGH);
  }
  else if (temp >= 16 && temp <= 29)
  {
    digitalWrite(LED_RED_TEMPERATURE, LOW);
    digitalWrite(LED_GREEN_TEMPERATURE, LOW);
    digitalWrite(LED_YELLOW_TEMPERATURE, HIGH);
  }
  else
  {
    digitalWrite(LED_GREEN_TEMPERATURE, LOW);
    digitalWrite(LED_YELLOW_TEMPERATURE, LOW);
    digitalWrite(LED_RED_TEMPERATURE, HIGH);
  }
}

// check humidity
int checkHumidity(int hum)
{
  if (hum >= 45 && hum <= 55)
  {
    digitalWrite(LED_RED_HUMIDITY, LOW);
    digitalWrite(LED_YELLOW_HUMIDITY, LOW);
    digitalWrite(LED_GREEN_HUMIDITY, HIGH);
  }
  else if (hum >= 40 && hum <= 44)
  {
    digitalWrite(LED_RED_HUMIDITY, LOW);
    digitalWrite(LED_GREEN_HUMIDITY, LOW);
    digitalWrite(LED_YELLOW_HUMIDITY, HIGH);
  }
  else
  {
    digitalWrite(LED_GREEN_HUMIDITY, LOW);
    digitalWrite(LED_YELLOW_HUMIDITY, LOW);
    digitalWrite(LED_RED_HUMIDITY, HIGH);
  }
}

void setup()
{
  Serial.begin(BAUDRATE);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_GREEN_P25, OUTPUT);
  pinMode(LED_YELLOW_P25, OUTPUT);
  pinMode(LED_RED_P25, OUTPUT);

  pinMode(LED_GREEN_P10, OUTPUT);
  pinMode(LED_YELLOW_P10, OUTPUT);
  pinMode(LED_RED_P10, OUTPUT);

  pinMode(LED_GREEN_TEMPERATURE, OUTPUT);
  pinMode(LED_YELLOW_TEMPERATURE, OUTPUT);
  pinMode(LED_RED_TEMPERATURE, OUTPUT);

  pinMode(LED_GREEN_HUMIDITY, OUTPUT);
  pinMode(LED_YELLOW_HUMIDITY, OUTPUT);
  pinMode(LED_RED_HUMIDITY, OUTPUT);
  my_sds.begin(10, 11);
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("AirCarto.fr");
  lcd.setCursor(0, 1);
  lcd.print("Atelier capteur");
  delay(1000);
}

void loop()
{
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;

  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess)
  {
    Serial.print("Read DHT11 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    delay(2000);
    return;
  }

  error = my_sds.read(&p25, &p10);
  if (!error)
  {

    Serial.println("P2.5: " + String(p25));
    Serial.println("P10:  " + String(p10));
    lcd.setCursor(0, 0);
    lcd.print("PM2.5: " + String(p25, 1) + "(ug/m3)");
    lcd.setCursor(0, 1);
    lcd.print("PM10:  " + String(p10, 1) + "(ug/m3)");
    lcd.setCursor(0, 2);
    lcd.print("T/H:   ");
    lcd.print(int(temperature));
    lcd.print(" C  ");
    lcd.print(int(humidity));
    lcd.print(" %");
    lcd.setCursor(0, 3);
    lcd.print("CO2:   " + String(100) + " ppm");

    int sensorPm25Status = sensorLevel(10, 20, p25);
    int sensorPm10Status = sensorLevel(50, 80, p10);
    // lightLed(sensorPm25Status);
    // lightLed(sensorPm10Status);
    checkTemperature(int(temperature));
    checkHumidity(int(humidity));

    //
    // ─── P25 ─────────────────────────────────────────────────────────
    //

    if (sensorPm25Status == GOOD)
    {
      digitalWrite(LED_RED_P25, LOW);
      digitalWrite(LED_YELLOW_P25, LOW);
      digitalWrite(LED_GREEN_P25, HIGH);
    };
    if (sensorPm25Status == FAIR)
    {
      digitalWrite(LED_RED_P25, LOW);
      digitalWrite(LED_GREEN_P25, LOW);
      digitalWrite(LED_YELLOW_P25, HIGH);
    };
    if (sensorPm25Status == DANGER)
    {
      digitalWrite(LED_GREEN_P25, LOW);
      digitalWrite(LED_YELLOW_P25, LOW);
      digitalWrite(LED_RED_P25, HIGH);
    }

    //
    // ─── P10 ────────────────────────────────────────────────────────────────────────
    //

    if (sensorPm10Status == GOOD)
    {
      digitalWrite(LED_RED_P10, LOW);
      digitalWrite(LED_YELLOW_P10, LOW);
      digitalWrite(LED_GREEN_P10, HIGH);
    };
    if (sensorPm10Status == FAIR)
    {
      digitalWrite(LED_RED_P10, LOW);
      digitalWrite(LED_GREEN_P10, LOW);
      digitalWrite(LED_YELLOW_P10, HIGH);
    };
    if (sensorPm10Status == DANGER)
    {
      digitalWrite(LED_GREEN_P10, LOW);
      digitalWrite(LED_YELLOW_P10, LOW);
      digitalWrite(LED_RED_P10, HIGH);
    }
    if (sensorPm25Status == DANGER && sensorPm10Status == DANGER)
    {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1000);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.clear();
      lcd.print("DANGER POLLUTION");
      delay(2000);
    }
  }
  delay(200);
}
