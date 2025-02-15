#include <LiquidCrystal.h>

// Configuración de la pantalla LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Configuración del sensor ultrasónico
const int trigPin = 3;
const int echoPin = 4;
long duracion;
int distancia;

void setup() {
  lcd.begin(16, 2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Enviar pulso ultrasónico
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Leer el eco del sensor
  duracion = pulseIn(echoPin, HIGH);
  distancia = duracion * 0.034 / 2; // Convertir tiempo a distancia en cm

  // Mostrar distancia en la LCD
  lcd.setCursor(0, 0);
  lcd.print("Distancia: ");
  lcd.print(distancia);
  lcd.print(" cm   ");

  // Mostrar alerta si está muy cerca
  lcd.setCursor(0, 1);
  if (distancia < 10) {
    lcd.print("Alerta: Objeto! ");
  } else {
    lcd.print("Zona Segura     ");
  }

  delay(500);
}
