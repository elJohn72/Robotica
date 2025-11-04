// -------------------------------------------------------------
// Robot Evasor de Obstáculos
// Componentes: Arduino UNO, HC-SR04, L298N (con jumpers ENA/ENB)
// Autor: AJTecnology
// -------------------------------------------------------------

// Pines L298N
const int IN1 = 4;
const int IN2 = 5;
const int IN3 = 6;
const int IN4 = 7;

// Pines Ultrasonido
const int TRIG = 9;
const int ECHO = 8;

// Variables
int distancia;
int umbral = 18;   // cm - distancia mínima para girar

// -------------------------------------------------------------
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  detener(); // inicia detenido
  delay(1000);
}

// -------------------------------------------------------------
// Medir distancia con el sensor HC-SR04
int medirDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracion = pulseIn(ECHO, HIGH, 25000); // máx 25 ms (~4 m)
  int cm = duracion * 0.034 / 2;              // convierte a cm
  return cm;
}

// -------------------------------------------------------------
// Control de motores (sin PWM)
void adelante() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void atras() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void izquierda() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void derecha() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void detener() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// -------------------------------------------------------------
void loop() {
  distancia = medirDistancia();

  if (distancia > umbral) {
    adelante();
  } else {
    detener();
    delay(200);
    atras();
    delay(300);

    if (random(0, 2) == 0) {
      izquierda();
    } else {
      derecha();
    }
    delay(400);
  }

  delay(50);
}