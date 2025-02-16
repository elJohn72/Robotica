// Definición de pines para cada LED
const int pinVerde   = 2;  // LED verde conectado al pin digital D1
const int pinAmarillo = 3;  // LED amarillo conectado al pin digital D2
const int pinRojo     = 4;  // LED rojo conectado al pin digital D3

void setup() {
  // Configuramos cada pin como salida
  pinMode(pinVerde, OUTPUT);
  pinMode(pinAmarillo, OUTPUT);
  pinMode(pinRojo, OUTPUT);
}

void loop() {
  // Encendemos el LED verde y apagamos los demás
  digitalWrite(pinVerde, HIGH);
  digitalWrite(pinAmarillo, LOW);
  digitalWrite(pinRojo, LOW);
  delay(5000);  // Verde encendido por 5 segundos

  // Encendemos el LED amarillo y apagamos los demás
  digitalWrite(pinVerde, LOW);
  digitalWrite(pinAmarillo, HIGH);
  digitalWrite(pinRojo, LOW);
  delay(2000);  // Amarillo encendido por 2 segundos

  // Encendemos el LED rojo y apagamos los demás
  digitalWrite(pinVerde, LOW);
  digitalWrite(pinAmarillo, LOW);
  digitalWrite(pinRojo, HIGH);
  delay(5000);  // Rojo encendido por 5 segundos
}
