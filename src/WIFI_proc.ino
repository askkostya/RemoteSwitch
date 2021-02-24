// Загрузка конфигурации из файла config.json
void loadConfiguration(const char *filename, Config &config) {
  File file = SPIFFS.open(filename, "r");
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, file);
  ssidAP = doc["ssidAP"].as<String>();
  passwordAP = doc["passwordAP"].as<String>();
  blynkAuthToken = doc["blynkAuthToken"].as<String>();
  tempCorrection = doc["tempCorrection"].as<String>();
  tempStarted = doc["tempStarted"].as<String>();
  tempMaximum = doc["tempMaximum"].as<String>();
  file.close();
}

//Получение MAC адреса WIFI ESP32 (будет использовано для имени точки доступа в режиме SoftAP)
String getMacAddress() {
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  char baseMacChr[18] = {0};
  sprintf(baseMacChr, "%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  return String(baseMacChr);
}

void WifiConnectionInit() {
  if (PortalStarted == true)
  {
    startWifi_SoftAP();
  }
  else
  {
    startWifi_Client();
  }
}

bool startWifi_SoftAP()
{
  WiFi.softAP(SoftAPName.c_str(), "12345678");
 //Serial.println("Portal started...");
}

bool startWifi_Client()
{
WiFi.begin(ssidAP.c_str(), passwordAP.c_str());
//Serial.println("Client started...");
}

//Таймер для реконнекта к WIFI и Blynk в случае потери связи
static void WifiTimerReconnect(void)
{
  if (PortalStarted == false)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
 //     Serial.println("Reconnect to WiFi Started...");
      WiFi.begin(ssidAP.c_str(), passwordAP.c_str());
    }
    if (!Blynk.connected() && WiFi.status() == WL_CONNECTED)
    {
      Blynk.connect();
    }
  }
}
