void setup() {
  Serial.begin(115200);
  Serial.println("hello world");
}

void loop() {
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    if (data == "+") {
      // TODO: enviar POST ASC
      Serial.println("Enviado POST ASC...");
    }
    else if (data == "-") {
      // TODO: enviar POST DESC
      Serial.println("Enviado POST DESC...");
    }
    else {
      Serial.println("Comando inválido");
    }
  }
}