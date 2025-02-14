void setup() {
  pinMode(4, OUTPUT);  // Configura el pin 4 como salida
}

void loop() {
  digitalWrite(4, HIGH); // Enciende el LED
  delay(1000);  // Espera 1 segundo
  digitalWrite(4, LOW);  // Apaga el LED
  delay(1000);  // Espera 1 segundo
}
