#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "your-wifi-name";
const char* password = "your-wifi-password";
const String slackToken = "your-slack-token";
const String bearerSlackToken = "Bearer " + slackToken;

// Buttons
const int green_button = 25;
const int yellow_button = 26;
const int white_button = 32;
const int black_button = 35;
const int blue_button = 33;
const int red_button = 34;


void setup() {
  Serial.begin(115200);

  // Buttons pinMode
  pinMode(green_button, INPUT);
  pinMode(yellow_button, INPUT);
  pinMode(white_button, INPUT);
  pinMode(black_button, INPUT);
  pinMode(blue_button, INPUT);
  pinMode(red_button, INPUT);

  // Start WIFI connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int green_button_state = digitalRead(green_button);
  int yellow_button_state = digitalRead(yellow_button);
  int white_button_state = digitalRead(white_button);
  int black_button_state = digitalRead(black_button);
  int blue_button_state = digitalRead(blue_button);
  int red_button_state = digitalRead(red_button);

  if (WiFi.status() == WL_CONNECTED) {
    if ( red_button_state == HIGH ) {
      changeSlackStatus(":sandwich:", "Lunch time!");
    } else if ( white_button_state == HIGH ) {
      setDndOn();
    } else if ( yellow_button_state == HIGH ) {
      setDndOff();
    } else if ( blue_button_state == HIGH ) {
      changeSlackStatus(":calendar:", "In a meeting");
    } else if ( black_button_state == HIGH ) {
      changeSlackStatus("", "");
      setPresence("away");
    } else if ( green_button_state == HIGH ) {
      changeSlackStatus("", "");
      setPresence("auto");
    }
  } else {
    Serial.println("WiFi Disconnected");
  }
}

// Chnages the slack status
void changeSlackStatus(String emoji, String status_text) {
  HTTPClient http;

  http.begin("https://slack.com/api/users.profile.set");
  http.addHeader("Host", "slack.com");
  http.addHeader("Content-Type", "application/json; charset=utf-8");
  http.addHeader("Authorization", bearerSlackToken);

  DynamicJsonDocument doc(1024);

  JsonObject profile  = doc.createNestedObject("profile");
  profile["status_emoji"] = emoji;
  profile["status_text"] = status_text;

  String requestBody;
  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  String printStatus = "Slack status updated to ";
  Serial.println(printStatus + status_text);

  http.end();
}

// Presence can be either auto or away
void setPresence(String presence) {
  HTTPClient http;

  http.begin("https://slack.com/api/users.setPresence");
  http.addHeader("Host", "slack.com");
  http.addHeader("Content-Type", "application/json; charset=utf-8");
  http.addHeader("Authorization", bearerSlackToken);

  DynamicJsonDocument doc(1024);
  doc["presence"] = presence;

  String requestBody;
  serializeJson(doc, requestBody);

  int httpResponseCode = http.POST(requestBody);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.println(requestBody);

  String printStatus = "Slack presence set to ";
  Serial.println(printStatus + presence);

  http.end();
}

// Set Do Not Disturb for 1 hour
void setDndOn() {
  HTTPClient http;

  int minutes = 60;
  String url = "https://slack.com/api/dnd.setSnooze?token=" + slackToken + "&num_minutes=" + minutes;

  http.begin(url);
  http.addHeader("Host", "slack.com");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpResponseCode = http.GET();

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  String printStatus = "Slack DND ON for 60 minutes";
  Serial.println(printStatus);

  http.end();
}

// Stop current Do Not Disturb
void setDndOff() {
  HTTPClient http;

  http.begin("https://slack.com/api/dnd.endSnooze");
  http.addHeader("Host", "slack.com");
  http.addHeader("Content-Type", "application/json; charset=utf-8");
  http.addHeader("Authorization", bearerSlackToken);

  int httpResponseCode = http.POST("");

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  String printStatus = "Slack DND disabled";
  Serial.println(printStatus);

  http.end();
}