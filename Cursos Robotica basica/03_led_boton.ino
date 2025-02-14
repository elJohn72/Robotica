const int boton = 2;   // Pin del botón
const int led = 4;     // Pin del LED
int estadoBoton = 0;   // Variable para almacenar el estado del botón

void setup() {
  pinMode(boton, INPUT);   // Configura el botón como entrada
  pinMode(led, OUTPUT);    // Configura el LED como salida
}

void loop() {
  estadoBoton = digitalRead(boton);  // Lee el estado del botón

  if (estadoBoton == HIGH) {
    digitalWrite(led, HIGH);  // Enciende el LED si el botón está presionado
  } else {
    digitalWrite(led, LOW);   // Apaga el LED si el botón no está presionado
  }
}
