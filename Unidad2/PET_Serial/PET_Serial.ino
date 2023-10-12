#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char *ssid = "Galaxy A233306";
const char *password = "SM-A235M957a";

const int incrementButtonPin = 25; // GPIO 25 for increment button
const int decrementButtonPin = 26; // GPIO 26 for decrement button
const int LEDButtonPin = 27; //Pin del led

// Your Domain name with URL path or IP address with path
String serverName = "https://a84f-201-175-240-164.ngrok-free.app";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
// unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void post_data(String action, int value)
{
  // Objeto JSON
  DynamicJsonDocument json_chido(1024);
  json_chido["action"] = action;
  json_chido["value"] = value;

  // Cadena JSON para enviar
  String json_str;
  serializeJson(json_chido, json_str);

  // Enviar POST
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(json_str);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void post_asc()
{
  post_data("asc", 1);
}

void post_desc()
{
  post_data("desc", 1);
}

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");

  pinMode(incrementButtonPin, INPUT);
  pinMode(decrementButtonPin, INPUT);
  pinMode(LEDButtonPin, OUTPUT);
}

void loop()
{
  // Check if the increment button is pressed
  if (digitalRead(incrementButtonPin) == HIGH) {
    post_asc();
    delay(500); // Add a delay to avoid rapid button presses
  }

  // Check if the decrement button is pressed
  if (digitalRead(decrementButtonPin) == HIGH) {
    post_desc();
    delay(500); // Add a delay to avoid rapid button presses
  }

  if (Serial.available())
  {
    String data = Serial.readStringUntil('\n');
    if (data == "+")
    {
      post_asc();
    }
    else if (data == "-")
    {
      post_desc();
    }
    else
    {
      Serial.println("Invalid command");
    }
  }
  // Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay)
  {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      String serverPath = serverName;

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());

      // If you need Node-RED/server authentication, insert user and password below
      // http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}