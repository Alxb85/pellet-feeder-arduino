// =======================================
// Pellet-Fass Steuerung (korrigierte Logik)
// Arduino Nano
// =======================================

const byte PIN_SENSOR_UNTEN = 2;   // unterer Sensor
const byte PIN_SENSOR_OBEN  = 3;   // oberer Sensor
const byte PIN_RELAIS       = 8;   // Relaismodul (LOW = Motor EIN)

const unsigned long START_DELAY_MS = 5000; // Einschaltverzögerung

bool motorState = false;
bool nachfuellen = false;
unsigned long leerSeit = 0;

void setup() {
  pinMode(PIN_SENSOR_UNTEN, INPUT_PULLUP);
  pinMode(PIN_SENSOR_OBEN,  INPUT_PULLUP);
  pinMode(PIN_RELAIS, OUTPUT);

  digitalWrite(PIN_RELAIS, HIGH); // Motor AUS

  Serial.begin(9600);
  Serial.println("=== Pelletsteuerung gestartet ===");
}

void loop() {
  // ===== Sensoren lesen =====
  bool untenFrei = (digitalRead(PIN_SENSOR_UNTEN) == HIGH); // leer
  bool obenAktiv = (digitalRead(PIN_SENSOR_OBEN)  == LOW);  // voll

  // ===== FEHLERFALL =====
  if (obenAktiv && untenFrei) {
    motorState = false;
    nachfuellen = false;
    leerSeit = 0;

    digitalWrite(PIN_RELAIS, HIGH); // Motor AUS

    Serial.println("!!! FEHLER: Oben aktiv, unten frei !!!");
    delay(500);
    return; // <<< WICHTIG: alles sperren
  }

  // ===== ZUSTANDSMASCHINE =====
  if (nachfuellen) {
    // Motor läuft
    if (obenAktiv) {
      motorState = false;
      nachfuellen = false;
      leerSeit = 0;
    }
  }
  else {
    // Motor AUS
    if (untenFrei && !obenAktiv) {
      if (leerSeit == 0) {
        leerSeit = millis();
      }
      else if (millis() - leerSeit >= START_DELAY_MS) {
        motorState = true;
        nachfuellen = true;
      }
    }
    else {
      motorState = false;
      leerSeit = 0;
    }
  }

  // ===== Relais schalten =====
  digitalWrite(PIN_RELAIS, motorState ? LOW : HIGH);

  // ===== Serieller Monitor =====
  Serial.print("Unten frei: ");
  Serial.print(untenFrei ? "JA" : "NEIN");
  Serial.print(" | Oben aktiv: ");
  Serial.print(obenAktiv ? "JA" : "NEIN");
  Serial.print(" | Motor: ");
  Serial.print(motorState ? "EIN" : "AUS");
  Serial.print(" | Nachfüllen: ");
  Serial.println(nachfuellen ? "JA" : "NEIN");

  delay(500);
}
