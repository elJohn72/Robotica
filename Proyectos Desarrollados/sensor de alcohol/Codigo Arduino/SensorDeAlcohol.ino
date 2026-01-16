#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int MQ3_PIN   = A0;
const int LED_VERDE = 3;
const int LED_AMAR  = 4;
const int LED_ROJO  = 5;

// *** CAMBIA ESTO SEGÚN TU CABLEADO ***
// true  -> ánodo del LED al pin y cátodo a GND (HIGH = encender)
// false -> ánodo a +5V y cátodo al pin (LOW = encender)
const bool LED_ACTIVE_HIGH = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const unsigned long WARMUP_MS = 60000;  // 1 min
const uint8_t  N_MEDIA     = 30;
const uint16_t SAMPLE_MS   = 100;
const uint16_t HISTERESIS  = 20;

int TH1, TH2;
int buf[N_MEDIA];
uint8_t idx = 0;
long acc = 0;
int baseRef = 0;

inline void writeLed(int pin, bool on){
  if (LED_ACTIVE_HIGH) {
    digitalWrite(pin, on ? HIGH : LOW);
  } else {
    digitalWrite(pin, on ? LOW : HIGH);
  }
}

void setLeds(bool v, bool a, bool r){
  writeLed(LED_VERDE, v);
  writeLed(LED_AMAR,  a);
  writeLed(LED_ROJO,  r);
}

int readSmooth(){
  acc -= buf[idx];
  buf[idx] = analogRead(MQ3_PIN);
  acc += buf[idx];
  idx = (idx + 1) % N_MEDIA;
  return acc / N_MEDIA;
}

void setup(){
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMAR,  OUTPUT);
  pinMode(LED_ROJO,  OUTPUT);
  setLeds(0,0,0);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("alcoholimetro");
  lcd.setCursor(0,1); lcd.print("Calibrando...");

  // TEST de LEDs al arranque (verás cada color 500 ms)
  setLeds(1,0,0); delay(500);
  setLeds(0,1,0); delay(500);
  setLeds(0,0,1); delay(500);
  setLeds(0,0,0);

  // Precarga buffer
  for (uint8_t i=0; i<N_MEDIA; i++){
    buf[i] = analogRead(MQ3_PIN);
    acc += buf[i];
    delay(10);
  }

  // Warm-up
  unsigned long start = millis();
  while (millis() - start < WARMUP_MS) delay(100);

  // baseRef
  long sum=0; const uint8_t M=40;
  for (uint8_t i=0; i<M; i++){ sum += readSmooth(); delay(50); }
  baseRef = sum / M;

  // Umbrales auto
  TH1 = baseRef + 60;
  TH2 = baseRef + 200;

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Base:"); lcd.print(baseRef);
  lcd.setCursor(0,1); lcd.print("TH1:"); lcd.print(TH1); lcd.print(" TH2:"); lcd.print(TH2);
  delay(1500);
  lcd.clear();
}

enum Estado { BAJO, MEDIO, ALTO };
Estado estado = BAJO;

void loop(){
  int val = readSmooth();

  // Determina estado (con histéresis)
  Estado nuevo;
  if (val >= TH2 + HISTERESIS)      nuevo = ALTO;
  else if (val < TH1 - HISTERESIS)  nuevo = BAJO;
  else                              nuevo = MEDIO;

  estado = nuevo; // actualízalo siempre

  // *** ACTUALIZA LEDs EN CADA CICLO ***
  switch(estado){
    case BAJO:  setLeds(1,0,0); break;  // Verde
    case MEDIO: setLeds(0,1,0); break;  // Amarillo
    case ALTO:  setLeds(0,0,1); break;  // Rojo
  }

  // LCD
  lcd.setCursor(0,0);
  lcd.print("Val:"); lcd.print(val); lcd.print("   ");
  lcd.setCursor(0,1);
  if (estado==BAJO)   lcd.print("Estado: VERDE   ");
  if (estado==MEDIO)  lcd.print("Estado: AMARILLO");
  if (estado==ALTO)   lcd.print("Estado: ROJO    ");

  // Debug opcional por serie
  // Serial.print("Val=");Serial.print(val);
  // Serial.print("  Base=");Serial.print(baseRef);
  // Serial.print("  TH1=");Serial.print(TH1);
  // Serial.print("  TH2=");Serial.println(TH2);

  delay(SAMPLE_MS);
}