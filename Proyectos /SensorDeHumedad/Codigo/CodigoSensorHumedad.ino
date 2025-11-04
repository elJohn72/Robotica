#include <LiquidCrystal.h>

// Pines RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int sensorPin = A0;
int valorSensor = 0;
int porcentaje = 0;

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("Sensor de humedad");
  delay(2000);
  lcd.clear();
}

void loop() {
  valorSensor = analogRead(sensorPin);
  porcentaje = map(valorSensor, 1023, 0, 0, 100); // invierte si es necesario

  Serial.print("Humedad: ");
  Serial.print(porcentaje);
  Serial.println("%");

  lcd.setCursor(0, 0);
  lcd.print("Humedad: ");
  lcd.print(porcentaje);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  if (porcentaje < 40) {
    lcd.print("Mi planta: sed ");
  } else if (porcentaje >= 40 && porcentaje < 70) {
    lcd.print("Mi planta: bien ");
  } else {
    lcd.print("Mi planta: feliz ");
  }

  delay(2000);
}
