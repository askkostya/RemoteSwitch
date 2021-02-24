//Запускается приложение на телефоне
BLYNK_APP_CONNECTED() {
  Blynk.virtualWrite(V10, globalPower);     //Состояние (включен/выключен)
  Blynk.virtualWrite(V3, globalChangeMode); //Режим работы (нагрев/вентилятор)
  Blynk.virtualWrite(V4, numTempStarted);   //Включен нагрев до (температура)
  Blynk.virtualWrite(V5, tempNow); //Температура с учетом корректировки (температура)
  Blynk.virtualWrite(V0, globalBlocked);    //Статус блокировки клавиатуры
}

//Устройство присоединяется к серверу Blynk
BLYNK_CONNECTED()
{
  if (isFirstBlynkConnect == true)
  {
    Blynk.virtualWrite(V10, globalPower);     //Состояние (включен/выключен)
    Blynk.virtualWrite(V3, globalChangeMode); //Режим работы (нагрев/вентилятор)
    Blynk.virtualWrite(V4, numTempStarted);   //Включен нагрев до (температура)
    Blynk.virtualWrite(V5, tempNow);          //Температура с градусника с учетом корректировки
    Blynk.virtualWrite(V0, globalBlocked);    //Статус блокировки кнопок
    isFirstBlynkConnect = false;
  }
}

//Нажатие кнопки блокировка в приложении
BLYNK_WRITE(V0) {
  globalBlocked = param.asInt();
}

//Нажатие кнопки POWER в приложении
BLYNK_WRITE(V10) {
digitalWrite(POWER_OPT, true);  //включить POWER
timerAlarmEnable(timerA);
//globalPower=!digitalRead(POWERLAMP);
globalPower=!globalPower;
Blynk.virtualWrite(V10, globalPower);
setTempStarted();
}

//Нажатие кнопки (+) в приложении
BLYNK_WRITE(V1) {
  if (numTempStarted < numTempMaximum)
  {
    digitalWrite(TEMPUP_OPT, true);  // включить TEMPUP_SW
    numTempStarted = numTempStarted + 1; // добавить 1 градус
    timerAlarmEnable(timerA);
    Blynk.virtualWrite(V4, numTempStarted);
  }
  Blynk.virtualWrite(V1, 0);
}

//Нажатие кнопки (-) в приложении
BLYNK_WRITE(V2) {
  if (numTempStarted != 0)
  {
    digitalWrite(TEMPDOWN_OPT, true);  // включить TEMPUP_OPT
    numTempStarted = numTempStarted - 1; // уменьшить на 1 градус
    timerAlarmEnable(timerA);
    Blynk.virtualWrite(V4, numTempStarted);
  }
  Blynk.virtualWrite(V2, 0);
}

//Нажатие кнопки (Нагрев/Вентилятор) в приложении
BLYNK_WRITE(V3) {
  globalChangeMode = param.asInt();
  if (globalChangeMode == 1)
  {
    digitalWrite(CHANGEMODEHEAT_OPT, true);  //Установить режим (нагрев)
  }
  if (globalChangeMode == 2)
  {
    digitalWrite(CHANGEMODEFAN_OPT, true);  //Установить режим (вентилятор)
  }
  timerAlarmEnable(timerA);
}

//Чтение температуры с градусника
BLYNK_READ(V5)
{
  Blynk.virtualWrite(V5, tempNow);
}
