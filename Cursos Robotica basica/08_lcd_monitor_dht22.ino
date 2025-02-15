// Importamos las librerías necesarias para el sensor DHT22 y la pantalla LCD
#include <DHT.h>
#include <LiquidCrystal.h>

// Definimos el pin donde conectaremos el sensor DHT22
#define DHTPIN 4

// Especificamos que estamos usando un sensor DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);  // Creamos el objeto dht para manejar el sensor

// Configuración de la pantalla LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {
  lcd.begin(16, 2);  // Inicializa la pantalla LCD con 16 columnas y 2 filas
  dht.begin();       // Inicializa el sensor DHT22

  // Muestra los encabezados en la primera fila de la LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp:   Hum:");
}

void loop() {
  // Lee los valores de temperatura y humedad del sensor
  float temperatura = dht.readTemperature();  // Obtiene la temperatura en °C
  float humedad = dht.readHumidity();         // Obtiene la humedad en %

  // Verifica si la lectura del sensor es válida
  if (isnan(temperatura) || isnan(humedad)) {
    lcd.setCursor(0, 1);
    lcd.print("Error en sensor ");
    return;  // Sale del loop si hay un error en la lectura
  }

  // Muestra la temperatura en la LCD
  lcd.setCursor(6, 0);   // Posiciona el cursor en la fila 1, columna 6
  lcd.print(temperatura, 1);  // Muestra la temperatura con 1 decimal
  lcd.print("C ");

  // Muestra la humedad en la LCD
  lcd.setCursor(13, 0);  // Posiciona el cursor en la fila 1, columna 13
  lcd.print(humedad, 1); // Muestra la humedad con 1 decimal
  lcd.print("% ");

  // Verifica si la temperatura es mayor a 30°C y muestra una alerta
  lcd.setCursor(0, 1);  // Posiciona el cursor en la segunda fila
  if (temperatura > 30) {
    lcd.print("Alerta: Calor!  ");  // Muestra una advertencia
  } else {
    lcd.print("Ambiente normal");  // Mensaje estándar
  }

  delay(2000);  // Espera 2 segundos antes de actualizar la lectura
}
