#include <LiquidCrystal.h>  // Librería para LCD

// Configuración de pines
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
const int botonAvanzar = 2;
const int botonRetroceder = 3;
int mensajeActual = 0;  // Índice del mensaje actual

// Lista de mensajes
String mensajes[] = {"Hola!", "Curso Arduino", "LCD 16x2", "Botones activos"};

void setup() {
  lcd.begin(16, 2);  // Iniciar la pantalla
  pinMode(botonAvanzar, INPUT_PULLUP);
  pinMode(botonRetroceder, INPUT_PULLUP);
  lcd.setCursor(0, 0);
  lcd.print(mensajes[mensajeActual]);  // Mostrar el primer mensaje
}

void loop() {
  if (digitalRead(botonAvanzar) == LOW) {
    mensajeActual++;
    if (mensajeActual > 3) mensajeActual = 0;
    actualizarLCD();
    delay(300);  // Pequeña pausa para evitar rebotes
  }

  if (digitalRead(botonRetroceder) == LOW) {
    mensajeActual--;
    if (mensajeActual < 0) mensajeActual = 3;
    actualizarLCD();
    delay(300);
  }
}

void actualizarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensajes[mensajeActual]);
}
