#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Configuración de sensores y actuadores
const int pirPin = 4;
const int trigPin = 5;
const int echoPin = 6;
const int buzzer = 3;
String codigoIngresado = "";
const String codigoCorrecto = "1234";  // Código correcto para desactivar la alarma

bool alarmaActiva = false;

void setup() {
  lcd.begin(16, 2);
  pinMode(pirPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);

  lcd.setCursor(0, 0);
  lcd.print("Alarma Activada");
  Serial.println("Ingrese el código para desactivar la alarma:");
}

void loop() {
  int estadoPIR = digitalRead(pirPin);  // Leer estado del PIR
  int distancia = medirDistancia();     // Medir distancia con HC-SR04

  // Activar la alarma si detecta movimiento o un objeto cerca
  if (!alarmaActiva && (estadoPIR == HIGH || (distancia > 0 && distancia < 10))) {
    alarmaActiva = true;
    lcd.setCursor(0, 1);
    lcd.print("¡Intruso detectado!");
    tone(buzzer, 1000);  // Activar buzzer
    Serial.println("¡Intruso detectado! Ingrese código para desactivar.");
  }

  // Leer código ingresado por el usuario
  if (alarmaActiva && Serial.available() > 0) {
    char input = Serial.read();  // Leer un carácter

    if (input == '\n' || input == '\r') {  // Si se presiona Enter
      if (codigoIngresado == codigoCorrecto) {
        alarmaActiva = false;
        noTone(buzzer);  // Apagar buzzer
        lcd.setCursor(0, 1);
        lcd.print("Alarma Desactivada ");
        Serial.println("Código correcto. Alarma desactivada.");
      } else {
        Serial.println("Código incorrecto. Inténtelo de nuevo.");
      }
      codigoIngresado = "";  // Reiniciar código ingresado
    } else {
      codigoIngresado += input;  // Agregar carácter a la cadena
    }
  }

  delay(500);
}

// Función para medir la distancia con el HC-SR04
int medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH, 30000);
  if (duracion == 0) return 999;
  int distancia = duracion * 0.034 / 2;
  return distancia;
}

