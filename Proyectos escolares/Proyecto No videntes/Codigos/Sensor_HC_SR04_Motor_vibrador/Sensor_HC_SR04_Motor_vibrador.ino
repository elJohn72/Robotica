/****************************************************
 * SmartVest - Etapa 1
 * Ultrasonido + Buzzer + Motor vibrador
 * Lógica por distancia (no bloqueante)
 ****************************************************/

// ==== PINES (AJUSTAR SEGÚN TU PCB) ====
const int PIN_TRIG      = 5;   // TRIG del HC-SR04
const int PIN_ECHO      = 18;   // ECHO del HC-SR04 (con divisor de voltaje HW)
const int PIN_BUZZER    = 13;  // Salida hacia buzzer (net BUZZ)
const int PIN_VIBRADOR  = 32;  // Salida hacia transistor del motor (net VIBRA)

// ==== PARÁMETROS DEL SENSOR ====
const unsigned long TIMEOUT_ULTRA = 30000UL; // 30 ms máx. espera de eco (~5 m)

// ==== UMBRALES DE DISTANCIA (cm) ====
const float DIST_PELIGRO     = 40.0;   // Muy cerca
const float DIST_ALERTA      = 100.0;  // Cerca
const float DIST_PRECAUCION  = 200.0;  // Un poco lejos

// ==== ESTRUCTURA DE PATRÓN ====
struct PatronAlerta {
  unsigned long periodo;    // Tiempo total de ciclo (ms)
  unsigned long onTime;     // Tiempo encendido dentro del ciclo (ms)
  bool activo;              // Si se aplica o no el patrón
};

// Patrón actual aplicado a buzzer y vibrador
PatronAlerta patronActual = {0, 0, false};
unsigned long tInicioPatron = 0;

// ==== VARIABLES DE ESTADO ====
float distanciaCM = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("SmartVest - Etapa 1: Ultrasonido + Buzzer + Vibrador");

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VIBRADOR, OUTPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_VIBRADOR, LOW);

  tInicioPatron = millis();
}

// ========= LOOP PRINCIPAL =========
void loop() {
  // 1. Medir distancia
  distanciaCM = medirDistanciaCM();

  // 2. Actualizar el patrón según la distancia medida
  actualizarPatronPorDistancia(distanciaCM);

  // 3. Aplicar patrón al buzzer y al motor vibrador (no bloqueante)
  aplicarPatron();

  // 4. Monitoreo por Serial
  Serial.print("Distancia: ");
  Serial.print(distanciaCM);
  Serial.println(" cm");

  delay(50);  // Pequeño delay solo para no saturar el puerto serie
}

// ========= FUNCIÓN: MEDIR DISTANCIA =========
float medirDistanciaCM() {
  // Pulso de disparo
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // Mide el tiempo del pulso en ECHO
  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_ULTRA);

  if (duracion == 0) {
    // No se recibió eco: consideramos que no hay obstáculo detectado
    return 9999.0;
  }

  // Fórmula HC-SR04: distancia (cm) = tiempo(us) * 0.0343 / 2
  float distancia = (duracion * 0.0343) / 2.0;
  return distancia;
}

// ========= FUNCIÓN: ELEGIR PATRÓN SEGÚN DISTANCIA =========
void actualizarPatronPorDistancia(float d) {
  PatronAlerta nuevoPatron;

  if (d > 500.0) {
    // Lectura muy grande / sin objeto
    nuevoPatron = {0, 0, false};
  }
  else if (d > DIST_PRECAUCION) {
    // Zona segura
    nuevoPatron = {0, 0, false};
  }
  else if (d > DIST_ALERTA && d <= DIST_PRECAUCION) {
    // Zona de precaución (lejos pero presente)
    nuevoPatron.periodo = 1000;   // 1 segundo de ciclo
    nuevoPatron.onTime  = 200;    // 200 ms encendido
    nuevoPatron.activo  = true;
  }
  else if (d > DIST_PELIGRO && d <= DIST_ALERTA) {
    // Zona de alerta (medio cerca)
    nuevoPatron.periodo = 600;    // 0.6 s
    nuevoPatron.onTime  = 200;    // 200 ms encendido
    nuevoPatron.activo  = true;
  }
  else {
    // Zona de peligro (muy cerca)
    nuevoPatron.periodo = 400;    // 0.4 s
    nuevoPatron.onTime  = 300;    // 300 ms encendido
    nuevoPatron.activo  = true;
  }

  // Si el patrón cambió, reiniciamos el tiempo de referencia
  if (nuevoPatron.periodo != patronActual.periodo ||
      nuevoPatron.onTime  != patronActual.onTime ||
      nuevoPatron.activo  != patronActual.activo) {

    patronActual = nuevoPatron;
    tInicioPatron = millis();
  }
}

// ========= FUNCIÓN: APLICAR PATRÓN A SALIDAS =========
void aplicarPatron() {
  if (!patronActual.activo || patronActual.periodo == 0) {
    // Todo apagado
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_VIBRADOR, LOW);
    return;
  }

  unsigned long ahora = millis();
  unsigned long tiempoDentroDelCiclo = (ahora - tInicioPatron) % patronActual.periodo;

  if (tiempoDentroDelCiclo < patronActual.onTime) {
    // Fase ON
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_VIBRADOR, HIGH);
  } else {
    // Fase OFF
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_VIBRADOR, LOW);
  }
}