/*
*    Crazy Cart Main Code
*    Petrovic Luka, Emmerlahu Leonit, Lang Elias
*    v 0.5
*/

#define ADC_Kanaele 4  // Anzahl der ADC-Kanäle
#include <Arduino.h>

volatile uint16_t adcWerte[ADC_Kanaele]; // Array zum Speichern der Werte
volatile uint8_t aktuellerKanal = 0;  // Aktueller MUX-Kanal

long distance = 0;
int pulseCount = 0;

const int magnets = 20;
const int wheelCircumference = 100; // Radumfang in mm
const int encoderPin = 2; 
const int rotationPin = 3;

// ISR für ADC
ISR(TIMER1_OVF_vect) {
    ADCSRA |= (1 << ADSC); // ADC Wandlung starten
}

ISR(ADC_vect) {
    adcWerte[aktuellerKanal] = ADC;  // Wert speichern

    aktuellerKanal = (aktuellerKanal + 1) % ADC_Kanaele; // Kanal wechseln
    ADMUX = (ADMUX & 0xF0) | aktuellerKanal; // Nur den Kanal setzen

    // Auto Trigger aktivieren mit Timer1 
    ADCSRB = (1 << ADTS2) | (1 << ADTS1);
    ADCSRA |= (1 << ADATE); // Auto-Trigger aktivieren
}

// ISR für den Encoder
void encoderISR() {
    if (digitalRead(rotationPin) == HIGH) {
        pulseCount++;  // Zähle die Impulse in der ISR
    } else {
        pulseCount--;
    }
}

void setup() {
    Serial.begin(115200);

    // TIMER1 SETUP
    TCCR1A = 0;  // Normaler Modus
    TCCR1B = (1 << CS12) | (1 << CS10);  // Prescaler
    TIMSK1 = (1 << TOIE1);  // Interrupt aktivieren

    // ADC SETUP (Single Conversion Mode) 
    ADCSRA = (1 << ADEN)  // ADC aktivieren
           | (1 << ADIE)  // Interrupt aktivieren
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128 kHz

    sei(); // Interrupts aktivieren

    // Pins für den Encoder
    pinMode(encoderPin, INPUT_PULLUP);
    pinMode(rotationPin, INPUT);

    // Interrupt für den Encoder registrieren
    attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, FALLING);  
}

void loop() {
    // ADC-Werte ausgeben
    Serial.print("ADC Channels: ");
    for (int i = 0; i < ADC_Kanaele; i++) {
        Serial.print("CH"); 
        Serial.print(i);
        Serial.print(": ");
        Serial.print(adcWerte[i]);
        Serial.print(" | ");
    }
    Serial.println();

    // Geschwindigkeit berechnen
    int count = pulseCount;
    float speed = (float)((wheelCircumference / magnets) * count) / 0.5;  // Geschwindigkeit in mm/s
    Serial.print("Geschwindigkeit: ");
    Serial.print(speed);
    Serial.println(" mm/s");

    // Strecke berechnen
    distance += (count * wheelCircumference) / magnets;
    Serial.print("Zurückgelegte Strecke: ");
    Serial.print(distance);
    Serial.println(" mm");

    pulseCount = 0; // Reset nach der Berechnung
    delay(500);  // Ausgabe-Intervall
}
