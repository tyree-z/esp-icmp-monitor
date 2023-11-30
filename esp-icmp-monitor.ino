#include <ESP8266WiFi.h>
#include <ESPping.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Configurable Parameters
const char* ssid = "Sog";              // Replace with your SSID
const char* password = "simonpeach";   // Replace with your Password
const char* serverName = "https://eok255xjsca4nb2.m.pipedream.net/pingreport"; // Replace with your API endpoint
const char* services[] = {"10.0.0.1", "8.8.8.8", "174.4.216.120"}; // Replace with IPs or domain names
const int numServices = sizeof(services) / sizeof(services[0]);
const int maxPings = 1;
const int retryLimit = 3;
const int delayTime = 30000; // 30 seconds

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    sendPingData();
  } else {
    Serial.println("Reconnecting to WiFi...");
    connectToWiFi();
  }
  delay(delayTime);
}

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

JsonArray collectPingResults(String service, DynamicJsonDocument &doc) {
  JsonArray pingResults = doc.createNestedArray(service);

  for (int i = 0; i < maxPings; i++) {
    Ping.ping(service.c_str(), 5);
    int minTime = Ping.minTime();
    int avgTime = Ping.averageTime();
    int maxTime = Ping.maxTime();

    Serial.print("Ping stats for "); Serial.print(service);
    Serial.print(" - Min: "); Serial.print(minTime);
    Serial.print(" ms, Avg: "); Serial.print(avgTime);
    Serial.print(" ms, Max: "); Serial.println(maxTime); // Debugging line

    JsonObject pingResult = pingResults.createNestedObject();
    if (minTime >= 0 && avgTime >= 0 && maxTime >= 0) {
      pingResult["min"] = minTime;
      pingResult["avg"] = avgTime;
      pingResult["max"] = maxTime;
    } else {
      pingResult["result"] = "Failed"; // Indicate a failed ping
    }
  }
  return pingResults;
}



void sendPingData() {
  DynamicJsonDocument doc(1024);
  for (int i = 0; i < numServices; i++) {
    collectPingResults(services[i], doc);
  }
  String jsonData;
  serializeJson(doc, jsonData);
  postToServer(jsonData);
}


void postToServer(String jsonData) {
  WiFiClientSecure wifiClient;
  wifiClient.setInsecure(); // Use this for an insecure connection, which is okay for basic projects

  HTTPClient http;
  if (http.begin(wifiClient, serverName)) {  // Initialize, start and connect to the server
    http.addHeader("Content-Type", "application/json");  
    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end(); // Close connection
  } else {
    Serial.println("Error in HTTP connection");    
  }
}
