#include <LiquidCrystal.h>

// Configuración de la pantalla LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Configuración del sensor PIR
const int pirPin = 4;
int estadoPIR = LOW;

void setup() {
  lcd.begin(16, 2);
  pinMode(pirPin, INPUT);
  lcd.setCursor(0, 0);
  lcd.print("Escaneando...");
}

void loop() {
  estadoPIR = digitalRead(pirPin);

  lcd.setCursor(0, 1);
  if (estadoPIR == HIGH) {
    lcd.print("¡Movimiento!  ");
  } else {
    lcd.print("Nada detectado");
  }

  delay(500);
}
