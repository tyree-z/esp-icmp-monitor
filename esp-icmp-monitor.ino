#include <ESP8266WiFi.h>
#include <ESPping.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>

// Configurable Parameters
const char* ssid = "<SSID>";
const char* password = "<Password>";
const char* serverName = "<Ping Report API Endpoint>";
const char* serviceFetchURL = "<API Endpoint To Fetch Services>"; // fetch services
const int pingRounds = 1; // 5 Pings Per Round
const int retryLimit = 3; // Ping Retry Limit
const int delayTime = 30000; // Wait 30 Seconds to Ping Services
const unsigned long serviceFetchInterval = 5000; // Fetch Services Every 5 Seconds

std::vector<String> services;
unsigned long lastServiceFetch = 0;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  if (millis() - lastServiceFetch > serviceFetchInterval) {
    fetchServices();
    lastServiceFetch = millis();
  }

  if (WiFi.status() == WL_CONNECTED) {
    sendPingData();
  } else {
    Serial.println("Reconnecting to WiFi...");
    connectToWiFi();
  }
  delay(delayTime);
}

// Init Wireless Connection
void connectToWiFi() {
  WiFi.begin(ssid, password);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < retryLimit) {
    delay(1000);
    Serial.print(".");
    retryCount++;
  }
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Check your credentials");
  }
}

// Fetch Services
void fetchServices() {
  WiFiClientSecure wifiClient;
  wifiClient.setInsecure();
  HTTPClient http;

  if (http.begin(wifiClient, serviceFetchURL)) {
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      updateServiceList(doc);
    } else {
      Serial.print("Error on fetching services: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Unable to make HTTP connection");    
  }
}

// Update JSON with new services
void updateServiceList(DynamicJsonDocument &doc) {
  services.clear();
  for (JsonPair kv : doc.as<JsonObject>()) {
    services.push_back(kv.value().as<String>());
  }
}

// JSON Object to send to API
JsonArray collectPingResults(String service, DynamicJsonDocument &doc) {
  JsonArray pingResults = doc.createNestedArray(service);

  for (int i = 0; i < pingRounds; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    Ping.ping(service.c_str(), 5);
    int minTime = Ping.minTime();
    int avgTime = Ping.averageTime();
    int maxTime = Ping.maxTime();

    Serial.print("Ping stats for "); Serial.print(service);
    Serial.print(" - Min: "); Serial.print(minTime);
    Serial.print(" ms, Avg: "); Serial.print(avgTime);
    Serial.print(" ms, Max: "); Serial.println(maxTime);
    digitalWrite(LED_BUILTIN, HIGH);

    JsonObject pingResult = pingResults.createNestedObject();
    if (minTime >= 0 && avgTime >= 0 && maxTime >= 0) {
      pingResult["min"] = minTime;
      pingResult["avg"] = avgTime;
      pingResult["max"] = maxTime;
    } else {
      pingResult["result"] = "Failed";
    }
  }
  return pingResults;
}

// Send the pings!
void sendPingData() {
  DynamicJsonDocument doc(1024);
  String deviceID = WiFi.macAddress();
  doc["device_id"] = deviceID;

  for (String &service : services) {
    collectPingResults(service, doc);
  }
  String jsonData;
  serializeJson(doc, jsonData);
  postToServer(jsonData);
}

// Send the report to API
void postToServer(String jsonData) {
  WiFiClientSecure wifiClient;
  wifiClient.setInsecure();

  HTTPClient http;
  if (http.begin(wifiClient, serverName)) {
    http.addHeader("Content-Type", "application/json");  
    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Sent Ping Report");
      Serial.print(httpResponseCode);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println("Sent Ping Report");
    }
    http.end();
  } else {
    Serial.println("Error in HTTP connection");    
  }
}
