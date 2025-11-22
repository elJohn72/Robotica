// =========================
// SISTEMA HIDROPÓNICO AJTECH
// Modo MANUAL Toggle y Modo AUTOMÁTICO
// =========================

// Pines
const int pinManual = 12;     // Pulsador Manual toggle
const int pinAuto   = 11;     // Pulsador para activar modo automático
const int pinRele   = 10;     // Relé que activa la bomba

// Estados
bool modoAutomatico = false;
bool bombaManualEncendida = false;

unsigned long tiempoAnterior = 0;
unsigned long debounceDelay = 200;

// AUTOMÁTICO
unsigned long tiempoEncendido = 20000;      // 20 seg
unsigned long tiempoApagado   = 180000;     // 3 min
bool bombaEncendida = false;

// Variables para anti-rebote
unsigned long ultimoManual = 0;
unsigned long ultimoAuto   = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pinManual, INPUT_PULLUP);
  pinMode(pinAuto,   INPUT_PULLUP);
  pinMode(pinRele,   OUTPUT);

  digitalWrite(pinRele, HIGH);  // Relé apagado

  Serial.println("SISTEMA HIDROPONICO INICIADO");
}

void loop() {

  // ================================
  // BOTÓN MANUAL (TOGGLE)
  // ================================
  if (!digitalRead(pinManual) && (millis() - ultimoManual > debounceDelay) && !modoAutomatico) {
    ultimoManual = millis(); // anti-rebote

    bombaManualEncendida = !bombaManualEncendida; // invierte estado

    if (bombaManualEncendida) {
      digitalWrite(pinRele, LOW);
      Serial.println("MANUAL: BOMBA ENCENDIDA");
    } else {
      digitalWrite(pinRele, HIGH);
      Serial.println("MANUAL: BOMBA APAGADA");
    }
  }

  // ================================
  // BOTÓN MODO AUTOMÁTICO (TOGGLE)
  // ================================
  if (!digitalRead(pinAuto) && (millis() - ultimoAuto > debounceDelay)) {
    ultimoAuto = millis();
    modoAutomatico = !modoAutomatico;

    if (modoAutomatico) {
      Serial.println("MODO AUTOMATICO ACTIVADO");
      // Apagamos manual si estaba encendido
      bombaManualEncendida = false;
      digitalWrite(pinRele, LOW);
      bombaEncendida = true;
      tiempoAnterior = millis();
    } else {
      Serial.println("MODO AUTOMATICO DESACTIVADO");
      digitalWrite(pinRele, HIGH);
    }
  }

  // ================================
  // AUTOMÁTICO
  // ================================
  if (modoAutomatico) {
    unsigned long tiempoActual = millis();

    // ENCENDIDA → apagar cuando cumpla tiempoEncendido
    if (bombaEncendida && (tiempoActual - tiempoAnterior >= tiempoEncendido)) {
      digitalWrite(pinRele, HIGH);
      bombaEncendida = false;
      tiempoAnterior = tiempoActual;
      Serial.println("AUTO: BOMBA APAGADA");
    }

    // APAGADA → encender cuando cumpla tiempoApagado
    if (!bombaEncendida && (tiempoActual - tiempoAnterior >= tiempoApagado)) {
      digitalWrite(pinRele, LOW);
      bombaEncendida = true;
      tiempoAnterior = tiempoActual;
      Serial.println("AUTO: BOMBA ENCENDIDA");
    }
  }
}