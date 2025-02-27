/*
*       Geschwindigkeitsmessung
*       Elias Lang
*       v1.0 Final
*/

#include <Arduino.h>


unsigned long distance = 0;
unsigned long pulseInterval = 0;

volatile unsigned long lastPulseTime = 0;
volatile unsigned long pulseCount = 0;

const int magnets = 20;
const int wheelCircumference = 100; // Radumfang in mm
const int encoderPin = 2; 

// ISR-Funktion
void encoderISR() {
    unsigned long currentTime = micros();
    pulseInterval = currentTime - lastPulseTime;
    pulseCount++;  // Zähle die Impulse in der ISR
    lastPulseTime = currentTime;
}

void setup() {
    Serial.begin(115200);
    pinMode(encoderPin, INPUT_PULLUP);
    // Interrupt registrieren
    attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, RISING);  // Interrupt auf Pin 2 setzen
}

void loop() {
    unsigned long count = pulseCount;
    unsigned long interval = pulseInterval;
    // Berechnung der Geschwindigkeit (nur alle 500ms)
    // Geschwindigkeit in mm/s berechnen
    float speed = (float)((wheelCircumference/magnets) * count) / (interval / 100000);  // Geschwindigkeit in mm/s
    pulseCount = 0;  // Zähler zurücksetzen
    pulseInterval = 0;
    Serial.print("Geschwindigkeit: ");
    Serial.print(speed);
    Serial.println(" mm/s");
    Serial.println(interval);
    Serial.println(count);

    // Strecke berechnen, basierend auf der Anzahl der Impulse
    distance += (count * wheelCircumference) / magnets;

    Serial.print("Zurückgelegte Strecke: ");
    Serial.print(distance);
    Serial.println(" mm");
    delay(500);  // Die Ausgabe alle 500ms aktualisieren
}
