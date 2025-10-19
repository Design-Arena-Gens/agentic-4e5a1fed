#include <SoftwareSerial.h>

// Pin definitions as const uint8_t
const uint8_t PIN_BT_RX = 10; // HC-05 TX -> Arduino RX (SoftwareSerial)
const uint8_t PIN_BT_TX = 11; // HC-05 RX <- Arduino TX (SoftwareSerial)

// L293D M1 (Steering)
const uint8_t PIN_IN1 = 2;   // IN1
const uint8_t PIN_IN2 = 3;   // IN2
const uint8_t PIN_EN1 = 5;   // EN1 (PWM)

// L293D M3 (Propulsion)
const uint8_t PIN_IN3 = 4;   // IN3
const uint8_t PIN_IN4 = 6;   // IN4
const uint8_t PIN_EN3 = 7;   // EN3 (PWM)

const uint8_t PIN_LED = 13;  // Built-in LED

SoftwareSerial btSerial(PIN_BT_RX, PIN_BT_TX); // RX, TX

void neutralSteering() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  analogWrite(PIN_EN1, 0);
}

void stopPropulsion() {
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
  analogWrite(PIN_EN3, 0);
}

void applyCommand(char c) {
  switch (c) {
    case 'F':
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
      analogWrite(PIN_EN3, 255);
      break;
    case 'B':
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, HIGH);
      analogWrite(PIN_EN3, 255);
      break;
    case 'L':
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
      analogWrite(PIN_EN1, 255);
      break;
    case 'R':
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);
      analogWrite(PIN_EN1, 255);
      break;
    case 'S':
    default:
      neutralSteering();
      stopPropulsion();
      break;
  }
}

void ack() {
  // Toggle LED briefly to acknowledge
  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
  delay(50);
  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

void selfDiagnostic() {
  for (uint8_t i = 0; i < 3; i++) {
    digitalWrite(PIN_LED, HIGH);
    delay(300);
    digitalWrite(PIN_LED, LOW);
    delay(300);
  }
}

void setup() {
  pinMode(PIN_LED, OUTPUT);

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_EN1, OUTPUT);

  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_EN3, OUTPUT);

  neutralSteering();
  stopPropulsion();

  selfDiagnostic();

  btSerial.begin(9600);
}

void loop() {
  if (btSerial.available() > 0) {
    int incoming = btSerial.read();
    if (incoming >= 0) {
      char c = (char)incoming;
      if (c == 'F' || c == 'B' || c == 'L' || c == 'R' || c == 'S') {
        applyCommand(c);
        ack();
      } else {
        applyCommand('S');
      }
    }
  }
}
