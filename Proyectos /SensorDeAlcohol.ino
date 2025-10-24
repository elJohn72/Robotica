#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int MQ3_PIN   = A0;
const int LED_VERDE = 3;
const int LED_AMAR  = 4;
const int LED_ROJO  = 5;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const unsigned long WARMUP_MS = 60000;  // 1 minuto
const uint8_t N_MEDIA = 30;
const uint16_t SAMPLE_MS = 100;
const uint16_t HISTERESIS = 20;

int TH1, TH2;
int buf[N_MEDIA];
uint8_t idx = 0;
long acc = 0;
int baseRef = 0;

void setLeds(bool v, bool a, bool r){
  digitalWrite(LED_VERDE, v);
  digitalWrite(LED_AMAR,  a);
  digitalWrite(LED_ROJO,  r);
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
  lcd.setCursor(0,0); lcd.print("ALcolimetro");
  lcd.setCursor(0,1); lcd.print("Calibrando...");
  
  // Calentamiento
  for(uint8_t i=0; i<N_MEDIA; i++){
    buf[i] = analogRead(MQ3_PIN);
    acc += buf[i];
    delay(50);
  }
  unsigned long start = millis();
  while(millis() - start < WARMUP_MS){
    delay(100);
  }

  // Medición baseRef
  long sum=0;
  const uint8_t M = 40;
  for(uint8_t i=0;i<M;i++){
    sum += readSmooth();
    delay(50);
  }
  baseRef = sum / M;

  // Asignación automática umbrales
  TH1 = baseRef + 60;
  TH2 = baseRef + 200;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Base:" ); lcd.print(baseRef);
  lcd.setCursor(0,1);
  lcd.print("TH1:"); lcd.print(TH1);
  lcd.print(" TH2:"); lcd.print(TH2);
  delay(3000);
  lcd.clear();
}

enum Estado { BAJO, MEDIO, ALTO };
Estado estado = BAJO;

void loop(){
  int val = readSmooth();
  Serial.print("Val:"); Serial.print(val);
  Serial.print("  Base:"); Serial.print(baseRef);
  Serial.print("  TH1:"); Serial.print(TH1);
  Serial.print("  TH2:"); Serial.println(TH2);

  Estado nuevo = estado;
  if(val >= TH2 + HISTERESIS)        nuevo = ALTO;
  else if(val < TH1 - HISTERESIS)    nuevo = BAJO;
  else                               nuevo = MEDIO;

  if(nuevo != estado){
    estado = nuevo;
    switch(estado){
      case BAJO:  setLeds(1,0,0); break;
      case MEDIO: setLeds(0,1,0); break;
      case ALTO:  setLeds(0,0,1); break;
    }
  }

  lcd.setCursor(0,0);
  lcd.print("Val:"); lcd.print(val); lcd.print("   ");
  lcd.setCursor(0,1);
  switch(estado){
    case BAJO:  lcd.print("Estado: VERDE   "); break;
    case MEDIO: lcd.print("Estado: AMARILLO"); break;
    case ALTO:  lcd.print("Estado: ROJO    "); break;
  }

  delay(SAMPLE_MS);
}

