void onTimerA() {
  if (dcA) // ON
  {
    dcA = false;
    timerAlarmWrite(timerA, durationON, true);
  }
  else // OFF
  {
    if (digitalRead(POWER_OPT) == true)  {
      digitalWrite(POWER_OPT, false);
    }
    if (digitalRead(TEMPUP_OPT) == true) {
      digitalWrite(TEMPUP_OPT, false);
    }
    if (digitalRead(TEMPDOWN_OPT) == true) {
      digitalWrite(TEMPDOWN_OPT, false);
    }
    if (digitalRead(CHANGEMODEHEAT_OPT) == true) {
      digitalWrite(CHANGEMODEHEAT_OPT, false);
    }
    if (digitalRead(CHANGEMODEFAN_OPT) == true) {
      digitalWrite(CHANGEMODEFAN_OPT, false);
    }
    dcA = true;
    timerAlarmWrite(timerA, durationOFF, true);
    timerAlarmDisable(timerA);
  }
}

//Получить температуру с градусника
static void getTempFromDHT()
{
  float t = dht.readTemperature();
  if (isnan(t)) {
    //Температура не прочитана
    tempNow = 0;
  }
 //  Serial.println(tempNow);
  tempNow = numTempCorrection + t;
}


//При включении питания если температура до которой греть равно температуре с градусника
//то значение нагревать до (numTempStarted) изменится

//Для значений
//от 1 до 22 - будет установлено 23
//от 23 до 25 - будет установлено 25
//от 26 до 27 - будет установлено 28
static void setTempStarted()
{
  if (globalPower == 1)
  {
    isTempChangedNeedUpdate = true;

    if (tempNow > 1 && tempNow < 23)
    {
      if (tempNow == numTempStarted)
      {
        numTempStarted = 23;
      }
    }
    if (tempNow > 22 && tempNow < 25)
    {
      if (tempNow == numTempStarted)
      {
        numTempStarted = 26;
      }
    }
    if (tempNow > 25 && tempNow < 28)
    {
      if (tempNow == numTempStarted)
      {
        numTempStarted = 28;
      }
    }
  }
}
