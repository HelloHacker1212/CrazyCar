/*
*    Crazy Cart Main Code
*    Petrovic Luka, Emmerlahu Leonit, Lang Elias
*    v 1,0
*/

#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

#define ADC_Kanaele 4  // Anzahl der ADC-Kanäle

volatile uint16_t adcWerte[ADC_Kanaele]; // Array zum Speichern der Werte
volatile uint8_t aktuellerKanal = 0;  // Aktueller MUX-Kanal

long distance = 0;
int pulseCount = 0;

const int magnets = 20;
const int wheelCircumference = 100; // Radumfang in mm
const int encoderPin = 2; 
const int rotationPin = 3;

void init_timer1() {
    // Timer1 für ADC
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12); // CTC Modus, Prescaler 256
    OCR1A = 62500; // 16 MHz / 256 / 1 Hz = 62500
    TIMSK1 = (1 << OCIE1A); // Timer1 Compare Match Interrupt aktivieren
}

void init_timer4() {
    // Timer4 für Servo-Steuerung
    TCCR4A = 0;
    TCCR4B = 0;

    // Fast PWM Mode mit ICR4 als TOP-Wert (50 Hz / 20 ms Periode)
    TCCR4A = (1 << COM4A1) | (1 << WGM41);  // Clear OC4A on Compare Match, Fast PWM Mode
    TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41); // Prescaler 8

    ICR4 = 20000; // 50 Hz PWM (20 ms Periode)
}

void set_servo_position(uint16_t grad) {
    OCR4A = map(grad, 0, 180, 1000, 2000); // PWM-Wert für Servo einstellen
}

ISR(TIMER1_COMPA_vect) {
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

void init_adc() {
    // ADC Setup
    ADMUX = (1 << REFS0); // AVcc als Referenzspannung
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2); // ADC aktivieren, Interrupt aktivieren, Prescaler auf 16
    ADCSRA |= (1 << ADSC); // Erste Wandlung starten
}

void setup() {
    Serial.begin(115200);

    // Timer initialisieren
    init_timer1();
    init_timer4();

    // ADC initialisieren
    init_adc();

    sei(); // Interrupts aktivieren

    pinMode(encoderPin, INPUT_PULLUP);
    pinMode(rotationPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(encoderPin), encoderISR, FALLING);  

    // Servo auf 90 Grad stellen
    set_servo_position(90);
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

    // Servo basierend auf ADC-Wert steuern
    set_servo_position(map(adcWerte[0], 0, 1023, 0, 180));

    pulseCount = 0; 
    delay(1000);  // 1000ms Ausgabe
}
