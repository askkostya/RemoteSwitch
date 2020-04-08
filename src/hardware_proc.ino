void onTimerA() {
  if (dcA) // ON
  {
    dcA = false;
    timerAlarmWrite(timerA, durationON, true);
  }
  else // OFF
  {
    if (digitalRead(POWER_OPT) == true)
    {
      digitalWrite(POWER_OPT, false);
    }
    if (digitalRead(TEMPUP_OPT) == true)
    {
      digitalWrite(TEMPUP_OPT, false);
    }
    if (digitalRead(TEMPDOWN_OPT) == true)
    {
      digitalWrite(TEMPDOWN_OPT, false);
    }
    if (digitalRead(CHANGEMODEHEAT_OPT) == true)
    {
      digitalWrite(CHANGEMODEHEAT_OPT, false);
    }
    if (digitalRead(CHANGEMODEFAN_OPT) == true)
    {
      digitalWrite(CHANGEMODEFAN_OPT, false);
    }
    dcA = true;
    timerAlarmWrite(timerA, durationOFF, true);
    timerAlarmDisable(timerA);
  }
}

//Получить температуры с градусника
int getTempFromDHT()
{
  float t = dht.readTemperature();
  if (isnan(t)) {
    //Температура не прочитана
    return 0;
  }
  return numTempCorrection + t;
}
