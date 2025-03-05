#define NUM_CHANNELS 4  // Anzahl der ADC-Kanäle
volatile uint16_t adcValues[NUM_CHANNELS]; // Array zum speichern der Werte
volatile uint8_t currentChannel = 0;  // Aktueller MUX-Kanal

void setup() {

    // TIMER1 SETUP
    TCCR1A = 0;  // Normaler Modus
    TCCR1B = (1 << CS12) | (1 << CS10);  // Prescaler
    TIMSK1 = (1 << TOIE1);  //Interrupt aktivieren

    // --- ADC SETUP (Single Conversion Mode) ---
    ADCSRA = (1 << ADEN)  // ADC aktivieren
           | (1 << ADIE)  // Interrupt aktivieren
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128 kHz

    sei(); // Interrupts aktivieren
}

// --- TIMER1
ISR(TIMER1_OVF_vect) {
    ADCSRA |= (1 << ADSC); // ADC wandlung startet nachdem timer1 abläuft
}

// --- ADC ISR ---
ISR(ADC_vect) {
    adcValues[currentChannel] = ADC;  // Ergebnis speichern

    currentChannel = (currentChannel + 1) % NUM_CHANNELS; // Kanal wechseln
    ADMUX = ADMUX  | currentChannel; // Neuen Kanal setzen

    // Jetzt Auto Trigger aktivieren mit Timer1
    ADCSRB = (1 << ADTS2) | (1 << ADTS1);
    ADCSRA |= (1 << ADATE); // Auto-Trigger aktivieren
}

void loop() {

}
