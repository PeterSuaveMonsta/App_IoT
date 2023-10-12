#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

#define DHTPIN 26  // Pin donde está conectado el DHT11 (cambiar si es necesario)
#define DHTTYPE DHT11  // Tipo de sensor (DHT11, DHT22, AM2302, etc.)

const char *ssid = "Galaxy A233306";
const char *password = "SM-A235M957a";
const int ledPin = 12;  // Pin GPIO 27 para el LED
const int incrementButtonPin = 2; // GPIO 25 para el botón de incremento
const int decrementButtonPin = 4; // GPIO 26 para el botón de decremento

// URL del servidor Python
const String serverUrl = "http://bc3b-187-190-35-202.ngrok-free.app"; // Reemplaza con la IP del servidor Python

int contador = 11;  // Inicializa el contador


void setup() {
  Serial.begin(115200);

  // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conexión WiFi establecida");

  // Configura los pines del LED y botones como entradas/salidas
  pinMode(ledPin, OUTPUT);
  pinMode(incrementButtonPin, INPUT_PULLDOWN);
  pinMode(decrementButtonPin, INPUT_PULLDOWN);

  // Enciende el LED al principio
  digitalWrite(ledPin, HIGH);

  //SETUP DEL DHT22
  //dht.begin();
}


void loop() {
  // Realiza una solicitud HTTP GET para obtener el estado del LED
  getLedStatus();

  // Espera antes de volver a verificar
  delay(5000); // Espera 5 segundos

  // Verifica si hay datos disponibles en la ventana serial
  char command = Serial.read(); // Lee el carácter ingresado

  // Variable para almacenar la acción
  String action = "";

  // Procesa el comando
  if (command == '+') {
    action = "asc";
  } else if (command == '-') {
    action = "desc";
  }

  // Verifica si se presionó el botón de incremento
  if (digitalRead(incrementButtonPin) == HIGH) {
    action = "asc";
  }

  // Verifica si se presionó el botón de decremento
  if (digitalRead(decrementButtonPin) == HIGH) {
    action = "desc";
  }

  // Si se detectó una acción válida, envía la solicitud POST
  if (!action.isEmpty()) {
    sendHttpRequest("/counter", action); // Envía solicitud al servidor
    action="";
  }
} // Aquí se agregó el cierre de llave que faltaba

void sendHttpRequest(String path, String action) {
  
  HTTPClient http;
  String url = serverUrl + path;

  //Construye el JSON para la solicitud POST
   String postData = "{\"action\":\"" + action + "\",\"quantity\":1}";

  // Realiza una solicitud POST al servidor
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(postData);

  if (httpCode > 0) {
    Serial.printf("HTTP response code: %d\n", httpCode);
    String response = http.getString();
    Serial.println(response);

    // Analiza la respuesta JSON para verificar el estado
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    bool status = doc["status"];

    // Actualiza el estado del LED en función de la respuesta del servidor
    digitalWrite(ledPin, status ? HIGH : LOW);

    // Actualiza el contador si la solicitud fue para el contador
    /*if (path == "/counter") {
      int newCounter = doc["contador"];
      contador = newCounter;
      Serial.printf("Nuevo valor del contador: %d\n", contador);
    }*/
  } else {
    Serial.printf("HTTP error code: %d\n", httpCode);
  }

  http.end();
}

void getLedStatus() {
  HTTPClient http;
  String url = serverUrl + "/led";

  // Realiza una solicitud HTTP GET al servidor
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("HTTP response code: %d\n", httpCode);
    String response = http.getString();
    Serial.println(response);
  
    // Analiza la respuesta JSON para obtener el estado del LED
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    bool status = doc["status"];

    Serial.println(status);

    // Actualiza el estado del LED en función de la respuesta del servidor
    if (status==1) {
    // Si status es true, enciende el LED
    digitalWrite(ledPin, HIGH);
  } else if (status == 0) {
    // Si status es false, apaga el LED
    digitalWrite(ledPin, LOW);
  }
  } else {
    Serial.printf("HTTP error code: %d\n", httpCode);
  }

  http.end();
}
