/****************************************************
 * SmartVest - Etapa 1.5
 * Ultrasonido + Buzzer + Vibrador
 * + Inicialización UART GPS y SIM800L (sin lógica aún)
 ****************************************************/

#include <HardwareSerial.h>

// ==== PINES ULTRASONIDO / ALERTAS (según tu esquema) ====
const int PIN_TRIG      = 5;    // TRIG HC-SR04 -> D5
const int PIN_ECHO      = 18;   // ECHO HC-SR04 -> D18 (con divisor)
const int PIN_BUZZER    = 13;   // BUZZ  -> D13
const int PIN_VIBRADOR  = 32;   // VIBRA -> D32

// ==== PARÁMETROS DEL SENSOR ULTRA ====
const unsigned long TIMEOUT_ULTRA = 30000UL; // 30 ms

// ==== UMBRALES DE DISTANCIA (cm) ====
const float DIST_PELIGRO     = 40.0;
const float DIST_ALERTA      = 100.0;
const float DIST_PRECAUCION  = 200.0;

// ==== UART GPS (NEO-6M) ====
// En el PCB: TXD del GPS -> D14 (RXD2), RXD del GPS -> D4 (TXD2)
HardwareSerial SerialGPS(1);    // UART1
const int GPS_RX = 4;          // ESP32 GPIO14 recibe desde TX del GPS
const int GPS_TX = 14;           // ESP32 GPIO4 transmite hacia RX del GPS

// ==== UART SIM800L ====
// En el PCB: TXD del SIM800L -> GPIO16 (RX2), RXD del SIM800L -> GPIO17 (TX2)
HardwareSerial SerialGSM(2);    // UART2
const int GSM_RX = 16;          // ESP32 GPIO16 recibe desde TX del SIM800L
const int GSM_TX = 17;          // ESP32 GPIO17 transmite hacia RX del SIM800L

// ==== ESTRUCTURA DE PATRÓN ====
struct PatronAlerta {
  unsigned long periodo;
  unsigned long onTime;
  bool activo;
};

PatronAlerta patronActual = {0, 0, false};
unsigned long tInicioPatron = 0;
float distanciaCM = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("SmartVest - Etapa 1.5");

  // --- Pines ultrasonido / alertas ---
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VIBRADOR, OUTPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_VIBRADOR, LOW);

  // --- UART GPS (NEO-6M) ---
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Serial.println("GPS UART1: RX=GPIO14, TX=GPIO4");

  // --- UART SIM800L ---
  SerialGSM.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);
  Serial.println("SIM800L UART2: RX=GPIO16, TX=GPIO17");

  tInicioPatron = millis();
}

// ========= LOOP PRINCIPAL =========
void loop() {
  // 1. Medir distancia y actualizar alertas
  distanciaCM = medirDistanciaCM();
  actualizarPatronPorDistancia(distanciaCM);
  aplicarPatron();

  // 2. (Opcional) monitoreo por Serial
  Serial.print("Distancia: ");
  Serial.print(distanciaCM);
  Serial.println(" cm");

  // 3. (Test simple) leer si llega algo del SIM800L
  while (SerialGSM.available()) {
    char c = SerialGSM.read();
    Serial.write(c);  // se ve en el monitor serie
  }

  // 4. (Test simple) leer si llega algo del GPS
  while (SerialGPS.available()) {
    char c = SerialGPS.read();
    Serial.write(c);  // tramas NMEA
  }

  delay(50);
}

// ========= FUNCIÓN: MEDIR DISTANCIA =========
float medirDistanciaCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_ULTRA);

  if (duracion == 0) {
    return 9999.0;
  }

  float distancia = (duracion * 0.0343) / 2.0;
  return distancia;
}

// ========= FUNCIÓN: ELEGIR PATRÓN SEGÚN DISTANCIA =========
void actualizarPatronPorDistancia(float d) {
  PatronAlerta nuevoPatron;

  if (d > 500.0 || d > DIST_PRECAUCION) {
    nuevoPatron = {0, 0, false};    // zona segura o sin lectura
  }
  else if (d > DIST_ALERTA) {
    nuevoPatron = {1000, 200, true}; // precaución
  }
  else if (d > DIST_PELIGRO) {
    nuevoPatron = {600, 200, true};  // alerta
  }
  else {
    nuevoPatron = {400, 300, true};  // peligro
  }

  if (nuevoPatron.periodo != patronActual.periodo ||
      nuevoPatron.onTime  != patronActual.onTime  ||
      nuevoPatron.activo  != patronActual.activo) {
    patronActual = nuevoPatron;
    tInicioPatron = millis();
  }
}

// ========= FUNCIÓN: APLICAR PATRÓN =========
void aplicarPatron() {
  if (!patronActual.activo || patronActual.periodo == 0) {
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_VIBRADOR, LOW);
    return;
  }

  unsigned long ahora = millis();
  unsigned long tCiclo = (ahora - tInicioPatron) % patronActual.periodo;

  if (tCiclo < patronActual.onTime) {
    digitalWrite(PIN_BUZZER, HIGH);
    digitalWrite(PIN_VIBRADOR, HIGH);
  } else {
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_VIBRADOR, LOW);
  }
}