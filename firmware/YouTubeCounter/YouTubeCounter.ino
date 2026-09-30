/*
 * YouTube Counter for ESP8266 (Wemos D1 Mini)
 *
 * Shows subscriber and view count of a YouTube channel on a MAX7219
 * 8-digit 7-segment display. The two numbers alternate every 5 minutes.
 * Stats are pulled from the YouTube Data API v3 once every 15 minutes.
 *
 * You need your own API key and channel ID:
 *   - Create an API key in the Google Cloud Console and enable the
 *     "YouTube Data API v3".
 *   - The channel ID is the string starting with UC... from your channel URL.
 * Do not share your API key publicly.
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

// Fill in your own values here
const char* API_KEY    = "YOUR_API_KEY";
const char* CHANNEL_ID = "YOUR_CHANNEL_ID";

const unsigned long FETCH_INTERVAL = 15UL * 60UL * 1000UL; // 15 minutes
const unsigned long DISPLAY_TOGGLE = 5UL * 60UL * 1000UL;  // 5 minutes

unsigned long lastFetch  = 0;
unsigned long lastToggle = 0;
bool showSubs = true;

long subscribers = -1;
long views = -1;

// Fetch subscriber and view count from the YouTube Data API.
// Returns true only if a valid subscriber count was received.
bool getStats() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi connection");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();               // skip certificate check
  client.setBufferSizes(1024, 512);   // small TLS buffer, needed on ESP8266
  client.setTimeout(15000);

  String url = String("https://www.googleapis.com/youtube/v3/channels")
             + "?part=statistics&id=" + CHANNEL_ID + "&key=" + API_KEY;

  HTTPClient http;
  http.setReuse(false);
  if (!http.begin(client, url)) {
    Serial.println("http.begin() failed");
    return false;
  }

  int code = http.GET();
  String body = http.getString();
  http.end();

  Serial.printf("HTTP %d\n", code);
  if (code != HTTP_CODE_OK) return false;

  // Only keep the statistics object to save memory
  StaticJsonDocument<192> filter;
  filter["items"][0]["statistics"]["subscriberCount"] = true;
  filter["items"][0]["statistics"]["viewCount"] = true;

  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, body, DeserializationOption::Filter(filter))) {
    Serial.println("JSON parse error");
    return false;
  }

  JsonVariant stats = doc["items"][0]["statistics"];
  if (stats.isNull()) return false;

  // The API returns these numbers as strings, so convert them
  long s = String((const char*)(stats["subscriberCount"] | "-1")).toInt();
  long v = String((const char*)(stats["viewCount"] | "-1")).toInt();
  if (s < 0) return false;

  subscribers = s;
  views = v;
  Serial.printf("Subscribers: %ld, Views: %ld\n", subscribers, views);
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
  if (!wm.autoConnect("YouTube-Counter")) {
    ESP.restart();
  }
  Serial.printf("Connected, IP %s\n", WiFi.localIP().toString().c_str());

  getStats();
  showNumber(subscribers);   // start with the subscriber count
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
    showNumber(showSubs ? subscribers : views);
    showSubs = !showSubs;
    lastToggle = now;
  }
}
