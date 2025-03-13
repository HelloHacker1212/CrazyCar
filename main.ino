/*
*    Crazy Cart Main Code
*    Petrovic Luka, Emmerlahu Leonit, Lang Elias
*    v 0.6
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
}

void encoderISR() {
    if (digitalRead(rotationPin) == HIGH) {
        pulseCount++;  
    } else {
        pulseCount--;
    }
}

void setup() {
    Serial.begin(115200);

    // TIMER1 SETUP
    TCCR1A = 0;  
    TCCR1B = (1 << CS11) | (1 << CS10);  // Prescaler auf 64, Timer schneller
    TIMSK1 = (1 << TOIE1);  

    // ADC SETUP (Schnellerer Prescaler)
    ADCSRA = (1 << ADEN)  
           | (1 << ADIE)  
           | (1 << ADPS2); // Prescaler auf 16 für höhere ADC Geschwindigkeit

    sei(); // Interrupts aktivieren

    pinMode(encoderPin, INPUT_PULLUP);
    pinMode(rotationPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, FALLING);  
}

void loop() {
    Serial.print("ADC Channels: ");
    for (int i = 0; i < ADC_Kanaele; i++) {
        Serial.print("CH"); 
        Serial.print(i);
        Serial.print(": ");
        Serial.print(adcWerte[i]);
        Serial.print(" | ");
    }
    Serial.println();

    int count = pulseCount;
    float speed = (float)((wheelCircumference / magnets) * count) / 0.5;  
    Serial.print("Geschwindigkeit: ");
    Serial.print(speed);
    Serial.println(" mm/s");

    distance += (count * wheelCircumference) / magnets;
    Serial.print("Zurückgelegte Strecke: ");
    Serial.print(distance);
    Serial.println(" mm");

    pulseCount = 0; 
    delay(500);  // 500ms Ausgabe
}
