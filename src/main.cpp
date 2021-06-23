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

bool runningBuzzer = false;

//
// ─── DEFINE PIN ─────────────────────────────────────────────────────────────────
//
#define BUTTON_PIN 7
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

void turnOnLedGreen(int sensorsLedGreen, int sensorsLedYellow, int sensorsLedRed)
{
  digitalWrite(sensorsLedGreen, HIGH);
  digitalWrite(sensorsLedYellow, LOW);
  digitalWrite(sensorsLedRed, LOW);
};

void turnOnLedYellow(int sensorsLedGreen, int sensorsLedYellow, int sensorsLedRed)
{
  digitalWrite(sensorsLedGreen, LOW);
  digitalWrite(sensorsLedYellow, HIGH);
  digitalWrite(sensorsLedRed, LOW);
};

void turnOnLedRed(int sensorsLedGreen, int sensorsLedYellow, int sensorsLedRed)
{
  digitalWrite(sensorsLedGreen, LOW);
  digitalWrite(sensorsLedYellow, LOW);
  digitalWrite(sensorsLedRed, HIGH);
};

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

//
// ─── CHECK TEMPERATURE ──────────────────────────────────────────────────────────
//

int checkTemperature(int temp)
{
  if (temp >= 18 && temp <= 22)
  {
    turnOnLedGreen(
        LED_GREEN_TEMPERATURE,
        LED_YELLOW_TEMPERATURE,
        LED_RED_TEMPERATURE);
  }
  else if (temp >= 16 && temp <= 29)
  {
    turnOnLedYellow(
        LED_GREEN_TEMPERATURE,
        LED_YELLOW_TEMPERATURE,
        LED_RED_TEMPERATURE);
  }
  else
  {
    turnOnLedRed(
        LED_GREEN_TEMPERATURE,
        LED_YELLOW_TEMPERATURE,
        LED_RED_TEMPERATURE);
  }
}

//
// ─── CHECK HUMIDITY ─────────────────────────────────────────────────────────────
//

int checkHumidity(int hum)
{
  if (hum >= 45 && hum <= 55)
  {
    turnOnLedGreen(
        LED_GREEN_HUMIDITY,
        LED_YELLOW_HUMIDITY,
        LED_RED_HUMIDITY);
  }
  else if (hum >= 40 && hum <= 44)
  {
    turnOnLedYellow(
        LED_GREEN_HUMIDITY,
        LED_YELLOW_HUMIDITY,
        LED_RED_HUMIDITY);
  }
  else
  {
    turnOnLedRed(
        LED_GREEN_HUMIDITY,
        LED_YELLOW_HUMIDITY,
        LED_RED_HUMIDITY);
  }
}

void setup()
{
  Serial.begin(BAUDRATE);

  pinMode(BUTTON_PIN, INPUT);
  //
  // ─── PIN BUZZER ─────────────────────────────────────────────────────────────────────
  //

  pinMode(BUZZER_PIN, OUTPUT);

  //
  // ─── PIN LED P25 ────────────────────────────────────────────────────────────────
  //

  pinMode(LED_GREEN_P25, OUTPUT);
  pinMode(LED_YELLOW_P25, OUTPUT);
  pinMode(LED_RED_P25, OUTPUT);
  //
  // ─── PIN LED P10 ────────────────────────────────────────────────────────────────
  //

  pinMode(LED_GREEN_P10, OUTPUT);
  pinMode(LED_YELLOW_P10, OUTPUT);
  pinMode(LED_RED_P10, OUTPUT);

  //
  // ─── PIN LED TEMPERATURE ────────────────────────────────────────────────────────────────
  //
  pinMode(LED_GREEN_TEMPERATURE, OUTPUT);
  pinMode(LED_YELLOW_TEMPERATURE, OUTPUT);
  pinMode(LED_RED_TEMPERATURE, OUTPUT);

  //
  // ─── PIN LED HUMIDITY ────────────────────────────────────────────────────────────────
  //
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
  };

  if (digitalRead(BUTTON_PIN) == HIGH)
  {
    runningBuzzer = !runningBuzzer;
  }

  error = my_sds.read(&p25, &p10);
  if (!error)
  {

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

    //
    // ─── CHECK TEMPERATURE ──────────────────────────────────────────────────────────
    //

    checkTemperature(int(temperature));
    //
    // ─── CHECK HUMIDITY ──────────────────────────────────────────────────────────
    //
    checkHumidity(int(humidity));

    //
    // ─── SENSORS P25 ─────────────────────────────────────────────────────────
    //

    if (sensorPm25Status == GOOD)
    {
      turnOnLedGreen(
          LED_GREEN_P25,
          LED_YELLOW_P25,
          LED_RED_P25);
    };
    if (sensorPm25Status == FAIR)
    {
      turnOnLedYellow(
          LED_GREEN_P25,
          LED_YELLOW_P25,
          LED_RED_P25);
    };
    if (sensorPm25Status == DANGER)
    {
      turnOnLedRed(
          LED_GREEN_P25,
          LED_YELLOW_P25,
          LED_RED_P25);
    }

    //
    // ───SENSORS P10 ────────────────────────────────────────────────────────────────────────
    //

    if (sensorPm10Status == GOOD)
    {
      turnOnLedGreen(
          LED_GREEN_P10,
          LED_YELLOW_P10,
          LED_RED_P10);
    };
    if (sensorPm10Status == FAIR)
    {
      turnOnLedYellow(
          LED_GREEN_P10,
          LED_YELLOW_P10,
          LED_RED_P10);
    };
    if (sensorPm10Status == DANGER)
    {
      turnOnLedRed(
          LED_GREEN_P10,
          LED_YELLOW_P10,
          LED_RED_P10);
    }
    delay(2000);

    if (sensorPm25Status == DANGER && sensorPm10Status == DANGER)
    {
      if (runningBuzzer)
      {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(1000);
        digitalWrite(BUZZER_PIN, LOW);
      }

      lcd.clear();
      lcd.print("DANGER POLLUTION");
      delay(2000);
    }
  }
}
