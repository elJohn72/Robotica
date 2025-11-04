#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C del LCD (puede ser 0x27 o 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int sensorPin = A0;
int valorSensor = 0;
int humedad = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  lcd.setCursor(0, 0);
  lcd.print("Sensor de Humedad");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  valorSensor = analogRead(sensorPin);

  // Convierte el valor a porcentaje (ajústalo si se invierte)
  humedad = map(valorSensor, 1023, 0, 0, 100);

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println("%");

  lcd.setCursor(0, 0);
  lcd.print("Humedad: ");
  lcd.print(humedad);
  lcd.print("%   "); // limpia texto sobrante

  lcd.setCursor(0, 1);
  if (humedad < 40) {
    lcd.print("Mi planta: tengo sed");
  } else if (humedad >= 40 && humedad < 70) {
    lcd.print("Mi planta: estoy bien");
  } else {
    lcd.print("Mi planta: feliz :) ");
  }

  delay(2000);
}
