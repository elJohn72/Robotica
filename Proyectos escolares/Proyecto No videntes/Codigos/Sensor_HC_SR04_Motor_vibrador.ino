// --- Pines HC-SR04 ---
#define TRIG_PIN 5
#define ECHO_PIN 18

// --- Pin motor vibrador (transistor 2N2222) ---
#define MOTOR_PIN 32

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MOTOR_PIN, OUTPUT);

  digitalWrite(MOTOR_PIN, LOW);  // Apaga el motor al inicio
  Serial.println("Prueba: Sensor ultrasónico + Motor vibrador");
}

void loop() {
  // Generar pulso de disparo
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Leer tiempo del eco
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calcular distancia en cm
  float distance = duration * 0.034 / 2;

  // Mostrar distancia
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Encender el vibrador si el objeto está a menos de 50 cm
  if (distance > 0 && distance < 50) {
    digitalWrite(MOTOR_PIN, HIGH);  // activa transistor → motor vibra
  } else {
    digitalWrite(MOTOR_PIN, LOW);
  }

  delay(500);
}
