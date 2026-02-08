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

#define FINAL_CARRERA 7

// ---------- VARIABLES ----------
Servo puerta;

float pisos[4] = {
  1.2,   // Piso 1
  10.4,   // Piso 2
  15.4,  // Piso 3
  22   // Piso 4
};
float tolerancia = 1.0;

int pisoActual = 3; // Cambiado: empieza en piso 4 (índice 3)
int pisoDestino = -1;
unsigned long tiempoUltimaLectura = 0;

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

bool finalDeCarreraActivado() {
  return digitalRead(FINAL_CARRERA) == LOW;
}

// Actualiza pisoActual según la distancia actual
void actualizarPisoActual(float distancia) {
  for (int i = 0; i < 4; i++) {
    if (abs(distancia - pisos[i]) <= tolerancia) {
      pisoActual = i;
      return;
    }
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(9600); // Para debugging
  
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  pinMode(FINAL_CARRERA, INPUT_PULLUP);

  puerta.attach(SERVO_PIN);
  puerta.write(0);

  motorParar();
}

// ---------- LOOP ----------
void loop() {
  // Detectar botones presionados
  if (digitalRead(BTN1) == LOW) {
    pisoDestino = 0;
    delay(200); // Debounce
  }
  if (digitalRead(BTN2) == LOW) {
    pisoDestino = 1;
    delay(200);
  }
  if (digitalRead(BTN3) == LOW) {
    pisoDestino = 2;
    delay(200);
  }
  if (digitalRead(BTN4) == LOW) {
    pisoDestino = 3;
    delay(200);
  }

  // Si no hay destino o ya estamos en el piso, parar
  if (pisoDestino == -1 || pisoDestino == pisoActual) {
    motorParar();
    return;
  }

  // Leer distancia
  float distancia = leerDistancia();
  if (distancia < 0) return;

  // Actualizar posición actual del ascensor
  actualizarPisoActual(distancia);

  // Si llegó al piso destino
  if (llegoAlPiso(distancia)) {
    motorParar();
    pisoActual = pisoDestino;
    pisoDestino = -1;

    puerta.write(90);      // abrir puerta
    delay(2500);
    puerta.write(0);       // cerrar puerta
    return;
  }

  // Verificar final de carrera solo para piso 4
  if (finalDeCarreraActivado() && pisoDestino == 3) {
    motorParar();
    pisoActual = 3;
    pisoDestino = -1;

    puerta.write(90);
    delay(2500);
    puerta.write(0);
    return;
  }

  // Mover hacia el piso destino
  if (distancia > pisos[pisoDestino]) {
    motorSubir();
  } else {
    motorBajar();
  }
}