/****************************************************
 * SmartVest - Etapa 2 (SOS + GPS + SIM800L)
 * Ultrasonido + Buzzer + Vibrador
 * + GPS NEO-6M (TinyGPSPlus)
 * + SIM800L (SMS SOS con ubicación)
 ****************************************************/

#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

// ==== PINES ULTRASONIDO / ALERTAS ====
const int PIN_TRIG      = 5;
const int PIN_ECHO      = 18;
const int PIN_BUZZER    = 13;
const int PIN_VIBRADOR  = 32;

// ==== BOTÓN SOS ====
const int PIN_SOS       = 33;  // Ajusta si usaste otro pin

// ==== UMBRALES ====
const unsigned long TIMEOUT_ULTRA = 30000UL;
const float DIST_PELIGRO     = 40.0;
const float DIST_ALERTA      = 100.0;
const float DIST_PRECAUCION  = 200.0;

// ===== GPS NEO-6M =====
// Según TU código final:
HardwareSerial SerialGPS(1);
const int GPS_RX = 4;      // ESP32 <- TX GPS
const int GPS_TX = 14;     // ESP32 -> RX GPS

TinyGPSPlus gps;

// ===== SIM800L =====
HardwareSerial SerialGSM(2);
const int GSM_RX = 16;     // ESP32 <- TX SIM800L
const int GSM_TX = 17;     // ESP32 -> RX SIM800L

// Destino del mensaje SOS
const char NUM_DESTINO[] = "+593963930791";

// ==== ESTRUCTURA DE PATRÓN ====
struct PatronAlerta {
  unsigned long periodo;
  unsigned long onTime;
  bool activo;
};

PatronAlerta patronActual = {0, 0, false};
unsigned long tInicioPatron = 0;
float distanciaCM = 0.0;

// ==== Estado del SOS ====
bool sosActivo = false;
bool smsEnviado = false;

//
// ================== PROTOTIPOS ==================
//
float medirDistanciaCM();
void actualizarPatronPorDistancia(float d);
void aplicarPatron();
void procesarGPS();
bool sim800_enviarComando(const char* cmd, const char* resp, unsigned long timeout);
bool sim800_enviarSMS(String msg);
void manejarSOS();

//
// ================== SETUP ==================
//
void setup() {
  Serial.begin(115200);
  delay(1500);
  Serial.println("SmartVest - Etapa 2 (SOS + GPS + SIM800L)");

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VIBRADOR, OUTPUT);
  pinMode(PIN_SOS, INPUT_PULLUP);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  digitalWrite(PIN_VIBRADOR, LOW);

  // GPS
  SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  // SIM800L
  SerialGSM.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);
  delay(2000);
  sim800_enviarComando("AT", "OK", 2000);
  sim800_enviarComando("ATE0", "OK", 2000);
  sim800_enviarComando("AT+CMGF=1", "OK", 2000);

  tInicioPatron = millis();
}

//
// ================== LOOP ==================
//
void loop() {
  procesarGPS();      // Actualiza datos GPS
  manejarSOS();       // Manejo del botón SOS

  distanciaCM = medirDistanciaCM();

  if (!sosActivo) {
    actualizarPatronPorDistancia(distanciaCM);
  } else {
    // Patrón especial SOS
    patronActual = {300, 200, true};
  }

  aplicarPatron();

  Serial.print("Distancia: ");
  Serial.print(distanciaCM);
  Serial.print(" cm | SOS: ");
  Serial.println(sosActivo ? "ACTIVO" : "INACTIVO");

  delay(20);
}

//
// ================== MEDIR DISTANCIA ==================
//
float medirDistanciaCM() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_ULTRA);

  if (duracion == 0) return 9999.0;

  return (duracion * 0.0343) / 2.0;
}

//
// ================== ACTUALIZAR PATRÓN ==================
//
void actualizarPatronPorDistancia(float d) {
  if (d > 500 || d > DIST_PRECAUCION) {
    patronActual = {0, 0, false};
  } else if (d > DIST_ALERTA) {
    patronActual = {1000, 200, true};
  } else if (d > DIST_PELIGRO) {
    patronActual = {600, 200, true};
  } else {
    patronActual = {400, 300, true};
  }
}

//
// ================== APLICAR PATRÓN ==================
//
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

//
// ================== GPS ==================
//
void procesarGPS() {
  while (SerialGPS.available()) {
    gps.encode(SerialGPS.read());
  }
}

//
// ================== SOS ==================
//
void manejarSOS() {
  bool presionado = (digitalRead(PIN_SOS) == LOW);

  if (presionado && !sosActivo) {
    sosActivo = true;
    smsEnviado = false;
    Serial.println(">>> SOS ACTIVADO");
  }

  if (sosActivo && !smsEnviado) {
    String mensaje = "⚠️ SOS ACTIVADO\n";

    if (gps.location.isValid()) {
      mensaje += "Lat: " + String(gps.location.lat(), 6) + "\n";
      mensaje += "Lon: " + String(gps.location.lng(), 6) + "\n";
      mensaje += "https://maps.google.com/?q=" +
                 String(gps.location.lat(), 6) + "," +
                 String(gps.location.lng(), 6);
    } else {
      mensaje += "Ubicación no disponible aún.";
    }

    if (sim800_enviarSMS(mensaje)) {
      Serial.println(">>> SMS ENVIADO");
      smsEnviado = true;
    } else {
      Serial.println(">>> ERROR enviando SMS");
    }
  }
}

//
// ================== SIM800L ==================
//
bool sim800_enviarComando(const char* cmd, const char* resp, unsigned long timeout) {
  SerialGSM.println(cmd);
  unsigned long t = millis();

  while (millis() - t < timeout) {
    if (SerialGSM.find(const_cast<char*>(resp))) {
      return true;
    }
  }
  return false;
}

bool sim800_enviarSMS(String msg) {
  if (!sim800_enviarComando("AT+CMGF=1", "OK", 2000)) return false;

  SerialGSM.print("AT+CMGS=\"");
  SerialGSM.print(NUM_DESTINO);
  SerialGSM.println("\"");
  delay(500);

  SerialGSM.print(msg);
  delay(500);

  SerialGSM.write(26); // CTRL+Z finaliza SMS

  return true;
}