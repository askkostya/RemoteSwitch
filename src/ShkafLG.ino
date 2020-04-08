#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

hw_timer_t * timerA = NULL;
volatile bool dcA = true;

int durationON = 800000;
int durationOFF = 120000;

//Переменные для WIFI подключения
String ssidAP;
String passwordAP;
String blynkAuthToken;
String SoftAPName;
bool PortalStarted;

//Переменные blynk
bool isFirstBlynkConnect = true;
bool isPowerSwitchNeedUpdate = false;
bool isTempChangedNeedUpdate = false;
bool isChangeModeSwitchNeedUpdate = false;

//Текущие значения режимов
int globalPower = 0; // Начальное состояния кнопки POWER_SW
int globalChangeMode = 1; //Начальное значение режима (1- тепло/2-вентилятор)
bool globalBlocked = false; //Начальное значение режима блокировки кнопок (не блокировано)
//
int power_state = 0;
int power_reading = 0;
int power_previous = 1;
//
int tempup_state = 0;
int tempup_reading = 0;
int tempup_previous = 1;
//
int tempdown_state = 0;
int tempdown_reading = 0;
int tempdown_previous = 1;
//
int changemode_heat_state = 0;
int changemode_heat_reading = 0;
int changemode_heat_previous = 1;
//
int changemode_fan_state = 0;
int changemode_fan_reading = 0;
int changemode_fan_previous = 1;

//Переменные для градусника/температуры
String tempCorrection; //Значение корректировки температуры (из файла настроек)
int numTempCorrection; //Значение корректировки температуры (число)
String tempStarted;  //Значение температуры на табло при включении (из файла настроек)
int numTempStarted; //Значение температуры на табло при включении (число)
String tempMaximum; //Значение максимально допустимой температуры (из файла настроек)
int numTempMaximum; //Значение максимально допустимой температуры (число)

//Переменные для кнопок
#define POWER_SW 34
#define TEMPUP_SW 35
#define TEMPDOWN_SW 36
#define CHANGEMODEHEAT_SW 39
#define CHANGEMODEFAN_SW 26

//Переменные для оптронов
#define POWER_OPT 16
#define TEMPUP_OPT 17
#define TEMPDOWN_OPT 18
#define CHANGEMODEHEAT_OPT 19
#define CHANGEMODEFAN_OPT 25

#define PORTAL_PIN 33
DHT dht(21, DHT22);

struct Config {
  String ssidAP;
  String passwordAP;
  String blynkAuthToken;
  String tempCorrection;
  String tempStarted;
  String tempMaxumum;
};

Config config;
const char *filename = "/config.json";

BlynkTimer timer;
WebServer server(80);

void setup() {
  pinMode (POWER_SW, INPUT);
  pinMode (TEMPUP_SW, INPUT);
  pinMode (TEMPDOWN_SW, INPUT);
  pinMode (CHANGEMODEHEAT_SW, INPUT);
  pinMode (CHANGEMODEFAN_SW, INPUT);

  pinMode (POWER_OPT, OUTPUT);
  pinMode (TEMPUP_OPT, OUTPUT);
  pinMode (TEMPDOWN_OPT, OUTPUT);
  pinMode (CHANGEMODEHEAT_OPT, OUTPUT);
  pinMode (CHANGEMODEFAN_OPT, OUTPUT);

  pinMode (PORTAL_PIN, INPUT_PULLUP);

  digitalWrite(POWER_SW, false);
  digitalWrite(TEMPUP_SW, false);
  digitalWrite(TEMPDOWN_SW, false);
  digitalWrite(CHANGEMODEHEAT_SW, false);
  digitalWrite(CHANGEMODEFAN_SW, false);

  digitalWrite(POWER_OPT, false);
  digitalWrite(TEMPUP_OPT, false);
  digitalWrite(TEMPDOWN_OPT, false);
  digitalWrite(CHANGEMODEHEAT_OPT, false);
  digitalWrite(CHANGEMODEFAN_OPT, false);

  //Если включен PORTAL_PIN то запускаем портал для настройки
  if (digitalRead(PORTAL_PIN) == LOW)
  {
    PortalStarted = true;
  }

  SoftAPName = getMacAddress();
  WiFi.disconnect();
  SPIFFS.begin(true);
  Serial.begin(9600);
  WifiConnectionInit();
  delay(100); //единственный delay
  WebServerInit();
  //Запуск таймера для повторного подключения к Wifi
  //и Blynk в случае отключения/обрыва связи
  timer.setInterval(60000L, WifiTimerReconnect);

  //Инициализация и запуск Blynk
  Blynk.config(blynkAuthToken.c_str());
  if (WiFi.status() == WL_CONNECTED)
  {
    Blynk.connect();
  }
  dht.begin();

  //Инициализация таймера сброса оптронов
  timerA = timerBegin(2, 80, true);
  timerAttachInterrupt(timerA, &onTimerA, true);
  timerAlarmWrite(timerA, durationON, true);
  timerAlarmDisable(timerA);

  numTempCorrection = tempCorrection.toInt(); //Значения корректировки температуры (число)
  numTempStarted = tempStarted.toInt();       //Значение стартовой температуры (число)
  numTempMaximum = tempMaximum.toInt();       //Значение максимальной температуры (число)

  //Инициализация прерываний (включается по нажатию аппаратных кнопок)
  attachInterrupt(digitalPinToInterrupt(POWER_SW), press_power, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TEMPUP_SW), press_tempup, CHANGE);
  attachInterrupt(digitalPinToInterrupt(TEMPDOWN_SW), press_tempdown, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHANGEMODEHEAT_SW), press_changemode_heat, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CHANGEMODEFAN_SW), press_changemode_fan, CHANGE);
}

//Нажатие аппаратной кнопки питание (ВКЛ/ВЫКЛ)
void press_power()
{
  if (globalBlocked == false) {
    power_reading = digitalRead(POWER_SW);
    if (power_reading == 0 && power_previous == 1)
    {
      power_state = !power_state;
      globalPower = !globalPower; //Инверсия значения globalPower
      isPowerSwitchNeedUpdate = true; //Необходимо обновить состояние кнопки V10 в Blynk
    }
    digitalWrite(POWER_OPT, !power_reading);
    power_previous = power_reading;
  }
}

//Нажатие аппаратной кнопки температура вверх
void press_tempup()
{
  if (globalBlocked == false) {
    if (numTempStarted < numTempMaximum) //Проверяем не превышена ли допустимая температура
    {
      tempup_reading = digitalRead(TEMPUP_SW);
      if (tempup_reading == 0 && tempup_previous == 1)
      {
        tempup_state = !tempup_state;
        numTempStarted =  numTempStarted + 1; // добавить градус
        isTempChangedNeedUpdate = true;
        tempup_previous = tempup_reading;
      }
      digitalWrite(TEMPUP_OPT, !tempup_reading);
      tempup_previous = tempup_reading;
    }
    else
    {
      digitalWrite(TEMPUP_OPT, false);
      tempup_previous = 1;
    }
  }
}
//Нажатие аппаратной кнопки температура вниз
void press_tempdown()
{
  if (globalBlocked == false) {
    if (numTempStarted != 0)
    {
      tempdown_reading = digitalRead(TEMPDOWN_SW);
      if (tempdown_reading == 0 && tempdown_previous == 1)
      {
        tempdown_state = !tempdown_state;
        numTempStarted = numTempStarted - 1; // удалить градус
        isTempChangedNeedUpdate = true;
      }
      digitalWrite(TEMPDOWN_OPT, !tempdown_reading);
      tempdown_previous = tempdown_reading;
    }
    else
    {
      digitalWrite(TEMPDOWN_OPT, false);
      tempdown_previous = 1;
    }
  }
}

//Нажатие аппаратной кнопки изменение режима (включить тепло)
void press_changemode_heat()
{
  if (globalBlocked == false) {
    changemode_heat_reading = digitalRead(CHANGEMODEHEAT_SW);
    if (changemode_heat_reading == 0 && changemode_heat_previous == 1)
    {
      globalChangeMode = 1;
      changemode_heat_state = !changemode_heat_state;
      isChangeModeSwitchNeedUpdate = true;
    }
    digitalWrite(CHANGEMODEHEAT_OPT, !changemode_heat_reading);
    changemode_heat_previous = changemode_heat_reading;
  }
}

//Нажатие аппаратной кнопки изменение режима (включить вентилятор)
void press_changemode_fan()
{
  if (globalBlocked == false) {
    changemode_fan_reading = digitalRead(CHANGEMODEFAN_SW);
    if (changemode_fan_reading == 0 && changemode_fan_previous == 1)
    {
      globalChangeMode = 2;
      changemode_fan_state = !changemode_fan_state;
      isChangeModeSwitchNeedUpdate = true;
    }
    digitalWrite(CHANGEMODEFAN_OPT, !changemode_fan_reading);
    changemode_fan_previous = changemode_fan_reading;
  }
}

void loop()  {
  server.handleClient();
  timer.run();
  if (Blynk.connected())
  {
    Blynk.run();
    //Если нажимали аппаратную кнопку POWER обновим состояние кнопки в Blynk
    if (isPowerSwitchNeedUpdate == true)
    {
      Blynk.virtualWrite(V10, globalPower);
      isPowerSwitchNeedUpdate = false;
    }
    //Если нажимали аппаратные кнопки (TEMP_UP/TEMP_DOWN)обновим состояние кнопки в Blynk
    if (isTempChangedNeedUpdate == true)
    {
      Blynk.virtualWrite(V4, numTempStarted);
      isTempChangedNeedUpdate = false;
    }
    //Если нажимали аппаратную кнопку CHANGE_MODE (тепло/вентилятор) обновим состояние кнопки в Blynk
    if (isChangeModeSwitchNeedUpdate == true)
    {
      Blynk.virtualWrite(V3, globalChangeMode);
      isChangeModeSwitchNeedUpdate = false;
    }
  }
}
