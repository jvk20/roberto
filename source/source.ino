#include <SoftwareSerial.h>

SoftwareSerial BTSerial(10, 11); // RX, TX
#define BUZZER_PIN 8

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  // Bluetooth -> USB (ver recibido por BT)
  if (BTSerial.available()) {
    char c = BTSerial.read();

    Serial.print("[BT→USB]: ");
    Serial.print(c);
    Serial.print(" (ASCII: ");
    Serial.print((int)c);
    Serial.println(")");

    // Procesar comando
    if (c == 'Y' || c == 'y') {
      activarBuzzer();
    }
    else if (c == 'N' || c == 'n') {
      desactivarBuzzer();
    }
  }

  // USB -> Bluetooth (enviar desde Monitor Serial al BT)
  if (Serial.available()) {
    char c = Serial.read();
    BTSerial.write(c);

    Serial.print("[USB→BT]: ");
    Serial.println(c);

    // Tambien procesar localmente
    if (c == 'Y' || c == 'y') {
      activarBuzzer();
    }
    else if (c == 'N' || c == 'n') {
      desactivarBuzzer();
    }
  }
}

void activarBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  Serial.println(">>> BUZZER ON");
  BTSerial.println(">>> BUZZER ON");
  delay(1000);
  digitalWrite(BUZZER_PIN, LOW);
}

void desactivarBuzzer() {
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println(">>> BUZZER OFF");
  BTSerial.println(">>> BUZZER OFF");
}