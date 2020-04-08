void WebServerInit(void) {
  if (PortalStarted == true)
  {
    InitWebServerPortalMode();
  }
  else
  {
    InitWebServerClientMode();
  }
}

void InitWebServerPortalMode(void) {
  server.begin();
  server.onNotFound(handleNotFound); // Not Found Page Error 404
  server.on("/", handlePageStream); //index.htm
  server.on("/param", handleSetParam); //Send param to ESP
}

//ToDO
void InitWebServerClientMode() {}


void handlePageStream() {
  File  file = SPIFFS.open("/index.htm", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void handleNotFound() {
  String message = "Page Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

//Сохранение конфигурации в config.json
void saveConfiguration(const char *filename, const Config &config) {
  SPIFFS.remove(filename);
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    // Serial.println(F("Failed to create file"));
    return;
  }
  StaticJsonDocument<256> doc;
  doc["ssidAP"] = ssidAP;
  doc["passwordAP"] = passwordAP;
  doc["blynkAuthToken"] = blynkAuthToken;
  doc["tempCorrection"] = tempCorrection;
  doc["tempStarted"] = tempStarted;
  doc["tempMaximum"] = tempMaximum;

  if (serializeJson(doc, file) == 0) {
    //Serial.println("Файл не записан");
  }
  file.close();
}

void handleSetParam() {
  ssidAP = server.arg("ssidAP");
  passwordAP = server.arg("passwordAP");
  blynkAuthToken = server.arg("BlynkAuthToken");
  tempCorrection = server.arg("tempCorrection");
  tempStarted = server.arg("tempStarted");
  tempMaximum = server.arg("tempMaximum");
  saveConfiguration(filename, config);
  server.send(200, "text/plain", "OK");
}
