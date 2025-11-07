/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-websocket-server-sensor/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <Preferences.h>
#include <SPI.h>
#include "Adafruit_MAX31855.h"

// #include <Adafruit_BME280.h>
// #include <Adafruit_Sensor.h>

// thermocouple pins
#define MAXDO     19
#define MAXCLK    18
#define MAXCS1    5
#define MAXCS2    17

// fan motor PWM pin
#define FANPWM 15

// Replace with your network credentials
Preferences preferences;

const int CRED_LEN=32;
char ssid[CRED_LEN];
char password[CRED_LEN];
// char* ssid = "Frogsaredope";
// char* password = "sally102";
char* ap_ssid = "SmokeWizardWiFiSetup";
char* ap_password = "wizardsrule";
bool wificonnected = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
unsigned long wifiLastTime = 0;
unsigned long maxWifiDelay = 3000; 
bool wifiTimeOut = false;  

// Create a sensor object
// Adafruit_BME280 bme;

// Create sensor dummy variables
int mySmokerTemp = 25;
int myMeatTemp = 18;
int myJuice = 100;
int myFuel = 100;

// initialize the thermocouples with hardware SPI (VSPI)
Adafruit_MAX31855 thermocouple1(MAXCLK, MAXCS1, MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, MAXCS2, MAXDO);



// Get Sensor Readings and return JSON object
String getSensorReadings(){

  Serial.print("Internal Temp = ");
  Serial.println(thermocouple1.readInternal());

  double c1 = thermocouple1.readCelsius();
  if (isnan(c1)) {
    Serial.println("Thermocouple 1 - fault(s) detected!");
    uint8_t e = thermocouple1.readError();
    if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple 1 is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple 1 is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple 1 is short-circuited to VCC.");
  } else {
    Serial.print("C = ");
    Serial.println(c1);
  }

  double c2 = thermocouple2.readCelsius();
  if (isnan(c2)) {
    Serial.println("Thermocouple 2 - fault(s) detected!");
    uint8_t e = thermocouple2.readError();
    if (e & MAX31855_FAULT_OPEN) Serial.println("FAULT: Thermocouple 2 is open - no connections.");
    if (e & MAX31855_FAULT_SHORT_GND) Serial.println("FAULT: Thermocouple 2 is short-circuited to GND.");
    if (e & MAX31855_FAULT_SHORT_VCC) Serial.println("FAULT: Thermocouple 2 is short-circuited to VCC.");
  } else {
    Serial.print("C = ");
    Serial.println(c2);
  }


  readings["smoker-temperature"] = String(c1);
  readings["meat-temperature"] = String(c2);
  readings["spritz-juice-percent"] =  String(myJuice);
  readings["fuel-supply-percent"] =  String(myFuel);
  // mySmokerTemp = mySmokerTemp + 1;
  // myMeatTemp = myMeatTemp + 1;
  myJuice = myJuice + 1;
  myFuel = myFuel + 1;
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize LittleFS
void initLittleFS() {
  if (!LittleFS.begin(true)) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

bool initWiFiSTA(char *ss, char *pw) {
  // try to connect to the wifi network using existing (currently hard-coded) credentials
  WiFi.begin(ss, pw);
  Serial.print("Connecting to WiFi ..");

  wifiLastTime = millis();
  maxWifiDelay = 5000; 
  while (WiFi.status() != WL_CONNECTED && !(wifiTimeOut)) {
    Serial.print('.');
    delay(1000);
    wifiTimeOut = ((millis() - wifiLastTime) > maxWifiDelay);
  }
  if (wifiTimeOut) {
    WiFi.disconnect();
  }
  return !(wifiTimeOut);
}

void handleWiFiSetupPage(AsyncWebServerRequest *request) {
  request->send(LittleFS, "/wifi-setup.html", "text/html");
}

void handleMainPage(AsyncWebServerRequest *request) {
  request->send(LittleFS, "/index.html", "text/html");
}

void (*myFuncPointer)(AsyncWebServerRequest *request) = handleWiFiSetupPage;

void handleGET(AsyncWebServerRequest *request) {
  myFuncPointer(request);
}

void handleWiFiCredentials(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  bool delimFound = false;
  int delimIndex = 0;
  // clear any previous network credentials
  memset(ssid, 0, sizeof ssid);
  memset(password, 0, sizeof ssid);
  // get new credentials
  for (size_t i = 0; i < len; i++) {
    if (!delimFound) {
      delimFound = ((char)data[i] == ',');
      delimIndex = i;
      if (!delimFound) {
        ssid[i] = (char)data[i];
      }
    } else {
      password[i - (delimIndex + 1)] = (char)data[i];
    }
  }
  // attempt new credentials
  Serial.println(ssid);
  Serial.println(password);
  WiFi.begin(ssid, password);
  wifiLastTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - wifiLastTime) < 3000) {
    Serial.print('.');
    delay(1000);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wificonnected = true;
    initWebSocket();
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    myFuncPointer = handleMainPage;
    Serial.println("WiFi connected successfully. Redirecting to main webpage...");
    Serial.println("Local IP on connected LAN: ");
    Serial.println(WiFi.localIP());
    // request->redirect(WiFi.localIP().toString());
    request->redirect("/index.html");
  } else {
    wificonnected = false; // redundant but kept for readability
    Serial.println("WiFi connection failed. Please try again or enter new credentials. ");
    request->send(500);
  }
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      String sensorReadings = getSensorReadings();
      Serial.println(sensorReadings);
      notifyClients(sensorReadings);
    //}
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// --------------------------------------

void setup() {
  delay(3000); // wait for serial monitor to start completely
  Serial.begin(115200);

  // generate test PWM for fan motor
  // pinMode(FANPWM, OUTPUT);
  // analogWrite(FANPWM, 100);

  // initialize thermocouples
  Serial.print("Initializing sensor...");
  if (!thermocouple1.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }

  // set up file system
  initLittleFS();

  // serve GET requests to root by serving either the WiFi setup page or the main page
  server.on(
    "/", 
    HTTP_GET, 
    handleGET);

  // handle POST requests that send WiFi credentials
  server.on(
    "/credPost.html", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request) {},
    NULL,
    handleWiFiCredentials);

  // use Access-Point/Station mode to handle successful and unsuccessful connection cases
  WiFi.mode(WIFI_AP_STA);


  // TODO: Retrieve existing credentials from memory
  // strncpy(ssid, "Frogsaredope", sizeof(ssid));
  // strncpy(password, "sally102", sizeof(password));
  preferences.begin("credentials", false);
  strncpy(ssid, preferences.getString("ssid", "").c_str(), sizeof(preferences.getString("ssid", "")));
  strncpy(password, preferences.getString("password", "").c_str(), sizeof(preferences.getString("password", "")));
  Serial.println(strcmp(ssid, ""));
  Serial.println(strcmp(password, ""));
  if (strcmp(ssid, "") == 0 || strcmp(password, "") == 0) {
    Serial.println("No values saved for ssid or password.");
  } else {
    Serial.println("\nSaved SSID: ");
    Serial.print(ssid);
    // Serial.println("\nSaved password: ");
    // Serial.print(password);
  }

  // try to connect with existing credentials:
  if (!initWiFiSTA(ssid, password)) {    
    Serial.println("Failed to connect. Creating access point...");
    WiFi.disconnect(true, false); // WiFi.begin() was started earlier and never stopped; 
    // it has to be stopped before trying again or it will give the "wifi:sta cannot set config" error
    
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println(WiFi.softAPIP());
    myFuncPointer = handleWiFiSetupPage;
    
  } else {
    Serial.println("Connected successfully!.  ");
    wificonnected = true;
    initWebSocket();
    myFuncPointer = handleMainPage;
  }
  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void loop() {
  if (wificonnected && ((millis() - lastTime) > timerDelay)) {
    String sensorReadings = getSensorReadings();
    // Serial.println(sensorReadings);
    notifyClients(sensorReadings);
    lastTime = millis();
  }
  ws.cleanupClients();
}
