#include <LiquidCrystal.h>

// Configuración de la pantalla LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Configuración de sensores y actuadores
const int pirPin = 4;    // PIR en D4
const int trigPin = 5;   // TRIG del HC-SR04 en D5
const int echoPin = 6;   // ECHO del HC-SR04 en D6
const int buzzer = 3;    // Buzzer en D3

void setup() {
  lcd.begin(16, 2);
  pinMode(pirPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Alarma Activada");
}

void loop() {
  int estadoPIR = digitalRead(pirPin);
  int distancia = medirDistancia();  // Obtener distancia con función personalizada

  lcd.setCursor(0, 1);
  lcd.print("                ");  // Borra el mensaje anterior
  lcd.setCursor(0, 1);

  if (estadoPIR == HIGH || (distancia > 0 && distancia < 10)) {
    lcd.print("¡Intruso detectado!");
    tone(buzzer, 1000);  // Sonido de alarma
    delay(500);
    noTone(buzzer);
  } else {
    lcd.print("Zona Segura");
  }

  delay(500);
}

// Función para medir distancia con el HC-SR04
int medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH, 30000);  // Máximo 30ms de espera
  if (duracion == 0) return 999;  // Si no detecta nada, devuelve 999 cm

  int distancia = duracion * 0.034 / 2;  // Convertir tiempo a distancia en cm
  return distancia;
}

