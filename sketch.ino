/*
*       Geschwindigkeitsmessung
*       Elias Lang
*       v1.0 Final
*/

#include <Arduino.h>


long distance = 0;

int pulseCount = 0;

const int magnets = 20;
const int wheelCircumference = 100; // Radumfang in mm
const int encoderPin = 2; 
const int rotationPin = 3;

// ISR-Funktion
void encoderISR() {
    if (digitalRead(rotationPin) == HIGH) {
      pulseCount++;  // Zähle die Impulse in der ISR
    } else {
      pulseCount--;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(encoderPin, INPUT_PULLUP);
    pinMode(rotationPin, INPUT);
    // Interrupt registrieren
    attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, FALLING);  // Interrupt auf Pin 2 setzen
}

void loop() {
    int count = pulseCount;
    // Berechnung der Geschwindigkeit (nur alle 500ms)
    // Geschwindigkeit in mm/s berechnen
    float speed = (float)((wheelCircumference/magnets) * count) / 0.5;  // Geschwindigkeit in mm/s
    Serial.print("Geschwindigkeit: ");
    Serial.print(speed);
    Serial.println(" mm/s");
    Serial.println(count);
    pulseCount = 0;

    // Strecke berechnen, basierend auf der Anzahl der Impulse
    distance += (count * wheelCircumference) / magnets;

    Serial.print("Zurückgelegte Strecke: ");
    Serial.print(distance);
    Serial.println(" mm");
    delay(500);  // Die Ausgabe alle 500ms aktualisieren
}
