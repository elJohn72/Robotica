#include <Servo.h>

// ---------- PINES ----------
#define TRIG 2
#define ECHO 4

#define IN1 5
#define IN2 3
#define ENA 6

#define SERVO_PIN 13

#define BTN1 8
#define BTN2 9
#define BTN3 10
#define BTN4 11

// ---------- VARIABLES ----------
Servo puerta;

float pisos[4] = {
  2.4,   // Piso 1
  6.4,   // Piso 2
  13.4,  // Piso 3
  19   // Piso 4
}; // CAMBIAR SEGÚN TU MAQUETA
float tolerancia = 1.0;

int pisoActual = 0;
int pisoDestino = -1;

// ---------- FUNCIONES ----------
float leerDistancia() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duracion = pulseIn(ECHO, HIGH, 30000);
  if (duracion == 0) return -1;

  return duracion / 58.0;
}

void motorParar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0);
}

void motorSubir() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);
}

void motorBajar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);
}

bool llegoAlPiso(float distancia) {
  return abs(distancia - pisos[pisoDestino]) <= tolerancia;
}

// ---------- SETUP ----------
void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  puerta.attach(SERVO_PIN);
  puerta.write(0); // puerta cerrada

  motorParar();
}

// ---------- LOOP ----------
void loop() {

  if (digitalRead(BTN1) == LOW) pisoDestino = 0;
  if (digitalRead(BTN2) == LOW) pisoDestino = 1;
  if (digitalRead(BTN3) == LOW) pisoDestino = 2;
  if (digitalRead(BTN4) == LOW) pisoDestino = 3;

  if (pisoDestino == -1 || pisoDestino == pisoActual) {
    motorParar();
    return;
  }

  float distancia = leerDistancia();
  if (distancia < 0) return;

  if (llegoAlPiso(distancia)) {
    motorParar();
    pisoActual = pisoDestino;
    pisoDestino = -1;

    puerta.write(90);      // abrir puerta
    delay(2500);
    puerta.write(0);       // cerrar puerta
    return;
  }

  if (distancia > pisos[pisoDestino]) {
    motorSubir();
  } else {
    motorBajar();
  }
}