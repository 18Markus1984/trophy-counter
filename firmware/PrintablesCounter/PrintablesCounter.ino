/*
 * Printables Counter for ESP8266 (Wemos D1 Mini)
 *
 * Shows follower and download count of a Printables profile on a
 * MAX7219 8-digit 7-segment display. The two numbers alternate every
 * 5 minutes. Stats are pulled from the Printables GraphQL API once
 * every 2 hours.
 *
 * Set your own profile handle in USER_HANDLE below.
 *
 * Libraries: ESP8266WiFi, WiFiManager, ESP8266HTTPClient,
 *            WiFiClientSecure, ArduinoJson, LedControl
 *
 * Wiring: DIN=D4, CLK=D2, CS=D3, VCC=5V, GND=G
 */

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <LedControl.h>

// Display pins
#define DIN_PIN D4
#define CLK_PIN D2
#define CS_LED  D3
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_LED, 1);

// Your Printables profile handle (without the @)
const char* USER_HANDLE = "YourHandle";

const char* API_URL = "https://api.printables.com/graphql/";

// Query the follower and download count for a given profile handle.
// The handle is inserted into the query string below.
String buildPayload() {
  return String("{\"query\":\"{ user(id:\\\"@") + USER_HANDLE +
         "\\\"){ followersCount downloadCount } }\"}";
}

const unsigned long FETCH_INTERVAL = 2UL * 60UL * 60UL * 1000UL; // 2 hours
const unsigned long DISPLAY_TOGGLE = 5UL * 60UL * 1000UL;        // 5 minutes

unsigned long lastFetch  = 0;
unsigned long lastToggle = 0;
bool showFollowers = true;

long followers = -1;
long downloads = -1;

// Fetch the current stats from the Printables API.
// Returns true only if a valid follower count was received.
bool getStats() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi connection");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();               // skip certificate check
  client.setBufferSizes(1024, 512);   // small TLS buffer, needed on ESP8266
  client.setTimeout(15000);

  HTTPClient http;
  http.setReuse(false);
  if (!http.begin(client, API_URL)) {
    Serial.println("http.begin() failed");
    return false;
  }
  http.addHeader("Content-Type", "application/json");
  http.addHeader("User-Agent", "ESP8266-PrintablesCounter/1.0");

  String payload = buildPayload();
  int code = http.POST((uint8_t*)payload.c_str(), payload.length());
  String body = http.getString();
  http.end();

  Serial.printf("HTTP %d, response: %s\n", code, body.c_str());
  if (code != HTTP_CODE_OK) return false;

  // Only keep the two fields we actually need to save memory
  StaticJsonDocument<128> filter;
  filter["data"]["user"]["followersCount"] = true;
  filter["data"]["user"]["downloadCount"] = true;

  DynamicJsonDocument doc(256);
  if (deserializeJson(doc, body, DeserializationOption::Filter(filter))) {
    Serial.println("JSON parse error");
    return false;
  }

  JsonVariant user = doc["data"]["user"];
  if (user.isNull()) return false;

  long f = user["followersCount"] | -1L;
  long d = user["downloadCount"]  | -1L;
  if (f < 0) return false;

  followers = f;
  downloads = d;
  Serial.printf("Followers: %ld, Downloads: %ld\n", followers, downloads);
  return true;
}

// Show a number right-aligned. On error the display shows "----".
void showNumber(long value) {
  lc.clearDisplay(0);
  if (value < 0) {
    for (int i = 0; i < 4; i++) lc.setChar(0, i, '-', false);
    return;
  }
  int idx = 0;
  if (value == 0) { lc.setDigit(0, 0, 0, false); idx = 1; }
  while (value > 0 && idx < 8) {
    lc.setDigit(0, idx, value % 10, false);
    value /= 10;
    idx++;
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // WiFiManager opens a config portal on first use so there is no need
  // to hard-code the WiFi credentials.
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);
  if (!wm.autoConnect("Printables-Counter")) {
    ESP.restart();
  }
  Serial.printf("Connected, IP %s\n", WiFi.localIP().toString().c_str());

  getStats();
  showNumber(followers);   // start with the follower count
  lastFetch = millis();
  lastToggle = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastFetch >= FETCH_INTERVAL) {
    if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
    getStats();
    lastFetch = now;
  }

  if (now - lastToggle >= DISPLAY_TOGGLE) {
    showNumber(showFollowers ? followers : downloads);
    showFollowers = !showFollowers;
    lastToggle = now;
  }
}
