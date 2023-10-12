#include <DHT.h>

#define DHTPIN 26  // Pin donde está conectado el DHT11 (cambiar si es necesario)
#define DHTTYPE DHT11  // Tipo de sensor (DHT11, DHT22, AM2302, etc.)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  // Leer la humedad relativa y la temperatura en grados Celsius
  float humedad = dht.readHumidity();
  float temperaturaCelsius = dht.readTemperature();

  // Verificar si la lectura fue exitosa
  if (isnan(humedad) || isnan(temperaturaCelsius)) {
    Serial.println("Error al leer el sensor DHT11");
    return;
  }

  // Mostrar los datos en el monitor serial
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.print(" %\t");

  Serial.print("Temperatura: ");
  Serial.print(temperaturaCelsius);
  Serial.println(" °C");

  delay(2000);  // Esperar 2 segundos antes de realizar la próxima lectura
}