#include <LiquidCrystal.h>

// Configuración de la pantalla LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Configuración de sensores y actuadores
const int pirPin = 4;    // PIR en D4
const int trigPin = 5;   // TRIG del HC-SR04 en D5
const int echoPin = 6;   // ECHO del HC-SR04 en D6
const int buzzer = 3;    // Buzzer en D3

void setup() {
  lcd.begin(16, 2);  // Inicializa la LCD 16x2
  pinMode(pirPin, INPUT);   // PIR como entrada
  pinMode(trigPin, OUTPUT); // TRIG como salida
  pinMode(echoPin, INPUT);  // ECHO como entrada
  pinMode(buzzer, OUTPUT);  // Buzzer como salida
  Serial.begin(9600);       // Monitor Serial para depuración

  lcd.setCursor(0, 0);
  lcd.print("Sistema Activo");
}

void loop() {
  int estadoPIR = digitalRead(pirPin);  // Leer estado del PIR
  int distancia = medirDistancia();     // Obtener distancia en cm

  // Mostrar distancia en LCD
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distancia);
  lcd.print(" cm   ");  // Espacios extras para limpiar texto previo

  // Mostrar estado del PIR en LCD
  lcd.setCursor(10, 1);
  if (estadoPIR == HIGH) {
    lcd.print("Mov");
    tone(buzzer, 1000);  // Activar buzzer si hay movimiento
  } else if (distancia > 0 && distancia < 10) {
    lcd.print("Obj");
    tone(buzzer, 2000);  // Activar buzzer si hay un objeto cerca
  } else {
    lcd.print("   ");  // Borra el texto si no hay detección
    noTone(buzzer);  // Apagar el buzzer si no hay detección
  }

  // Mostrar valores en el Monitor Serial
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.print(" cm | PIR: ");
  Serial.println(estadoPIR);

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