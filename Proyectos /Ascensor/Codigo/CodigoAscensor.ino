/*
  ASCENSOR DIDÁCTICO - Arduino Nano + L298N + HC-SR04 + Servo + 4 Botones
  - Sin Bluetooth
  - Botones con INPUT_PULLUP (un lado a GND)
  - Controla posición por distancia medida con HC-SR04 (cm)
  - L298N: IN1/IN2 para sentido, ENA opcional para PWM (velocidad)
*/

#include <Servo.h>

// -------------------- PINES (AJUSTA A TU ESQUEMÁTICO) --------------------
// HC-SR04
const byte PIN_TRIG = 2;
const byte PIN_ECHO = 4;

// L298N (Motor canal A)
const byte PIN_IN1  = 5;
const byte PIN_IN2  = 3;
const byte PIN_ENA  = 6;   // Si no usas ENA (jumper puesto), igual puedes dejarlo conectado o poner 255 fijo

// Servo (puerta/freno)
const byte PIN_SERVO = 9;  // Recomendado. Si en tu esquema usas D13, cambia a 13.

// LED (indicador)
const byte PIN_LED = 8;    // Si no tienes LED en D8, cambia o comenta uso

// Botones (4 pisos)
const byte BTN1 = 10;
const byte BTN2 = 11;
const byte BTN3 = 12;
const byte BTN4 = A0;      // Si no quieres usar A0, pon D7/D8/etc.

// -------------------- CONFIGURACIÓN DEL ASCENSOR --------------------
const int   NUM_FLOORS = 4;

// ✅ CALIBRACIÓN: coloca aquí la distancia (cm) que lee el sensor en cada piso.
// Ejemplo: si el sensor está arriba mirando hacia abajo, al subir la cabina la distancia DISMINUYE.
// Debes medir y reemplazar valores reales.
float floorCm[NUM_FLOORS] = { 25.0, 18.0, 12.0, 6.0 }; // <-- CAMBIAR con tus medidas reales

const float TOL_CM = 1.0;            // Tolerancia para considerar “llegó al piso”
const int   MOTOR_PWM = 200;         // 0-255 (si ENA por PWM). Si jumper ENA fijo, no afecta mucho.
const unsigned long MOVE_TIMEOUT = 20000; // ms (seguridad: si no llega en 20s, se detiene)

const int SERVO_CERRADO = 0;
const int SERVO_ABIERTO = 90;

const unsigned long DOOR_OPEN_TIME = 2500; // ms puerta abierta
const unsigned long DEBOUNCE_MS    = 35;   // antirrebote botones

// Si al SUBIR la cabina la distancia del sensor AUMENTA, pon true.
// Si al SUBIR la cabina la distancia DISMINUYE, pon false.
const bool distanceIncreasesWhenGoingUp = false;

// -------------------- VARIABLES INTERNAS --------------------
Servo puerta;

enum State { IDLE, MOVING, DOOR_OPEN, DOOR_CLOSE, ERROR_STOP };
State state = IDLE;

int currentFloor = 0;     // 0..3
int targetFloor  = -1;

unsigned long stateTs = 0;
unsigned long moveStartTs = 0;

struct DebBtn {
  byte pin;
  bool lastRead;
  bool stable;
  unsigned long ts;
};
DebBtn buttons[NUM_FLOORS];


// -------------------- UTILIDADES --------------------
float readDistanceCm() {
  // Disparo
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Lectura
  unsigned long duration = pulseIn(PIN_ECHO, HIGH, 30000UL); // 30ms ~ 5m
  if (duration == 0) return -1; // sin lectura

  // cm: (us/58) aprox
  float cm = duration / 58.0;
  return cm;
}

int nearestFloorFromCm(float cm) {
  int best = 0;
  float bestErr = 9999;
  for (int i = 0; i < NUM_FLOORS; i++) {
    float err = fabs(cm - floorCm[i]);
    if (err < bestErr) {
      bestErr = err;
      best = i;
    }
  }
  return best;
}

bool arrivedToTarget(float cm) {
  if (cm < 0) return false;
  return fabs(cm - floorCm[targetFloor]) <= TOL_CM;
}

void motorStop() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  analogWrite(PIN_ENA, 0);
}

void motorUp() {
  // Definimos “SUBIR” según la relación distancia/altura
  // Si al subir la distancia DISMINUYE (sensor arriba mirando abajo), entonces subir debe mover hacia “target menor”.
  // El sentido eléctrico del motor lo ajustas invirtiendo IN1/IN2 si lo ves al revés.
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  analogWrite(PIN_ENA, MOTOR_PWM);
}

void motorDown() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, HIGH);
  analogWrite(PIN_ENA, MOTOR_PWM);
}

void doorOpen()  { puerta.write(SERVO_ABIERTO); }
void doorClose() { puerta.write(SERVO_CERRADO); }

void initButtons() {
  const byte pins[NUM_FLOORS] = { BTN1, BTN2, BTN3, BTN4 };
  for (int i = 0; i < NUM_FLOORS; i++) {
    buttons[i].pin = pins[i];
    pinMode(pins[i], INPUT_PULLUP);
    buttons[i].lastRead = digitalRead(pins[i]);
    buttons[i].stable = buttons[i].lastRead;
    buttons[i].ts = millis();
  }
}

int readButtonFloorPressed() {
  // Retorna 0..3 cuando detecta un “press” (transición estable a LOW)
  for (int i = 0; i < NUM_FLOORS; i++) {
    bool r = digitalRead(buttons[i].pin);
    if (r != buttons[i].lastRead) {
      buttons[i].lastRead = r;
      buttons[i].ts = millis();
    }
    if ((millis() - buttons[i].ts) > DEBOUNCE_MS) {
      if (buttons[i].stable != r) {
        buttons[i].stable = r;
        // Botón con pullup: presionado = LOW
        if (buttons[i].stable == LOW) return i;
      }
    }
  }
  return -1;
}

void setState(State s) {
  state = s;
  stateTs = millis();
}


// -------------------- SETUP --------------------
void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENA, OUTPUT);

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  puerta.attach(PIN_SERVO);
  doorClose();

  initButtons();
  motorStop();

  // Estimar piso inicial por sensor
  delay(300);
  float cm = readDistanceCm();
  if (cm > 0) currentFloor = nearestFloorFromCm(cm);
}


// -------------------- LOOP PRINCIPAL --------------------
void loop() {
  float cm = readDistanceCm();

  // LED simple: encendido cuando está moviéndose
  digitalWrite(PIN_LED, (state == MOVING) ? HIGH : LOW);

  // Leer botones siempre
  int pressed = readButtonFloorPressed();
  if (pressed != -1) {
    targetFloor = pressed;
    // Si está idle y piden un piso diferente, iniciar movimiento
    if (state == IDLE && targetFloor != currentFloor) {
      moveStartTs = millis();
      setState(MOVING);
    }
  }

  switch (state) {
    case IDLE: {
      // Actualiza piso actual si estás “cerca” de un piso (mejora estabilidad)
      if (cm > 0) {
        int near = nearestFloorFromCm(cm);
        if (fabs(cm - floorCm[near]) <= TOL_CM) currentFloor = near;
      }
      motorStop();
      break;
    }

    case MOVING: {
      if (targetFloor < 0 || targetFloor >= NUM_FLOORS) {
        motorStop();
        setState(ERROR_STOP);
        break;
      }

      // Seguridad por timeout
      if (millis() - moveStartTs > MOVE_TIMEOUT) {
        motorStop();
        setState(ERROR_STOP);
        break;
      }

      // Si no hay lectura válida, sigue intentando pero sin “llegar”
      if (cm < 0) {
        motorStop();
        break;
      }

      if (arrivedToTarget(cm)) {
        motorStop();
        currentFloor = targetFloor;
        doorOpen();
        setState(DOOR_OPEN);
        break;
      }

      // Decidir si subir o bajar usando distancia
      // Si al subir la distancia aumenta:
      //   - para ir a un piso con mayor distancia => subir
      // Si al subir la distancia disminuye:
      //   - para ir a un piso con menor distancia => subir
      float targetCm = floorCm[targetFloor];

      bool needUp;
      if (distanceIncreasesWhenGoingUp) {
        needUp = (cm < targetCm - TOL_CM);
      } else {
        needUp = (cm > targetCm + TOL_CM);
      }

      if (needUp) motorUp();
      else        motorDown();

      break;
    }

    case DOOR_OPEN: {
      // Mantener puerta abierta un tiempo y luego cerrar
      if (millis() - stateTs > DOOR_OPEN_TIME) {
        doorClose();
        setState(DOOR_CLOSE);
      }
      break;
    }

    case DOOR_CLOSE: {
      // Pequeña espera para que cierre
      if (millis() - stateTs > 600) {
        targetFloor = -1;
        setState(IDLE);
      }
      break;
    }

    case ERROR_STOP: {
      // Paro de seguridad (puedes reintentar presionando un botón)
      motorStop();
      // Si presionan algún botón, intenta de nuevo
      if (pressed != -1) {
        targetFloor = pressed;
        if (targetFloor != currentFloor) {
          moveStartTs = millis();
          setState(MOVING);
        } else {
          setState(IDLE);
        }
      }
      break;
    }
  }

  delay(40); // estabilidad
}