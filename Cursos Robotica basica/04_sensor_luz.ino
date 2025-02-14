int ldr = A0;  // Sensor de luz en el pin A0
int led = 13;  // LED en el pin 13
int umbral = 500;  // Nivel de luz donde se enciende el LED

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);  // Habilitamos el monitor serial
}

void loop() {
  int luz = analogRead(ldr);  // Leer el sensor LDR
  Serial.println(luz);  // Mostrar el valor en pantalla

  if (luz < umbral) {  // Si la luz es menor al umbral...
    digitalWrite(led, HIGH);  // Enciende el LED
  } else {
    digitalWrite(led, LOW);  // Apaga el LED
  }

  delay(500);  // Esperar medio segundo
}
