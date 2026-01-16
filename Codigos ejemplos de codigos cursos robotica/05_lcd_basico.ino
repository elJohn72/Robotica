#include <LiquidCrystal.h>  // Librería para controlar la pantalla LCD

// Configurar los pines de conexión
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  lcd.begin(16, 2);  // Inicializar la pantalla (16 columnas, 2 filas)
  lcd.setCursor(0, 0);  // Mover el cursor a la primera fila
  lcd.print("Hola, Mundo!");  // Mostrar mensaje en la LCD
}

void loop() {
  // No es necesario escribir en loop(), el mensaje se queda en pantalla
}

