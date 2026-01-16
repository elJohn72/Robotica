#include <Servo.h>

#define FLAME_SENSOR 8
#define SERVO_X 9
#define SERVO_Y 10
#define RELAY 11

Servo servoX;
Servo servoY;

int posX = 0;
int posY = 45;

void setup() {
  Serial.begin(9600);
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(RELAY, OUTPUT);
  
  servoX.attach(SERVO_X);
  servoY.attach(SERVO_Y);

  servoX.write(posX);
  servoY.write(posY);
  digitalWrite(RELAY, LOW);

  Serial.println("Sistema listo...");
}

void loop() {
  int flame = digitalRead(FLAME_SENSOR);

  if (flame == LOW) { // Llama detectada
    Serial.println("🔥 Fuego detectado!");
    digitalWrite(RELAY, HIGH);  // Enciende bomba

    // ServoY apunta al fuego
    for (int i = 45; i <= 90; i++) {
      servoY.write(i);
      delay(15);
    }

    // Simula movimiento del agua
    delay(3000); // tiempo de extinción

    digitalWrite(RELAY, LOW); // Apaga bomba

    // Regresa a posición inicial
    for (int i = 90; i >= 45; i--) {
      servoY.write(i);
      delay(15);
    }
  } 
  else {
    // ServoX busca la llama moviéndose lentamente
    posX += 5;
    if (posX > 180) posX = 0;
    servoX.write(posX);
    delay(100);
  }
}
