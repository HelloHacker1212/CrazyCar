#define ADC_Kanaele 4  // Anzahl der ADC-Kanäle
volatile uint16_t adcWerte[ADC_Kanaele]; // Array zum Speichern der Werte
volatile uint8_t aktuellerKanal = 0;  // Aktueller MUX-Kanal

void setup() {
    Serial.begin(9600);

    // TIMER1 SETUP
    TCCR1A = 0;  // Normaler Modus
    TCCR1B = (1 << CS12) | (1 << CS10);  // Prescaler
    TIMSK1 = (1 << TOIE1);  // Interrupt aktivieren

    // ADC SETUP (Single Conversion Mode) 
    ADCSRA = (1 << ADEN)  // ADC aktivieren
           | (1 << ADIE)  // Interrupt aktivieren
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128 kHz

    sei(); // Interrupts aktivieren
}

// --- TIMER1 ISR ---
ISR(TIMER1_OVF_vect) {
    ADCSRA |= (1 << ADSC); // ADC Wandlung starten
}

// --- ADC ISR ---
ISR(ADC_vect) {
    adcWerte[aktuellerKanal] = ADC;  // Wert speichern

    aktuellerKanal = (aktuellerKanal + 1) % ADC_Kanaele; // Kanal wechseln
    ADMUX = ADMUX  | aktuellerKanal; // Nur den Kanal setzen

    // Auto Trigger aktivieren mit Timer1 
    ADCSRB = (1 << ADTS2) | (1 << ADTS1);
    ADCSRA |= (1 << ADATE); // Auto-Trigger aktivieren
}

void loop() {
    Serial.print("ADC Channels Working: ");
    for (int i = 0; i < ADC_Kanaele; i++) {
        Serial.print("CH"); 
        Serial.print(i);
        Serial.print(": ");
        Serial.print(adcWerte[i]);
        Serial.print(" | ");
    }
    Serial.println();
}
