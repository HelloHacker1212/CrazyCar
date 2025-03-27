// Arduino Mega: ADC Konfiguration 
#include <avr/io.h> 
#include <avr/interrupt.h> 
volatile uint16_t adcResults[4] = {0}; // Array, um die Ergebnisse der ADC-Kanäle zu speichern 
volatile uint8_t currentChannel = 0;  // Aktueller ADC-Kanal (0 bis 3) 
void setup() { 
  cli(); // Interrupts deaktivieren 
  // Referenzspannung auf interne 2.56V setzen 
  ADMUX = (1 << REFS1) | (1 << REFS0); // Internal 2.56V Reference with external capacitor at AREF pin 
  // ADC Prescaler auf 128 setzen (für 16 MHz Takt) 
  ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler = 128
  // Auto Trigger Source auf Timer1 Compare Match B setzen 
  ADCSRB = (1 << ADTS2) | (1 << ADTS0); // Trigger Source = Timer1 Compare Match B 
  // Digital Input deaktivieren für ADC0, ADC1, ADC2, ADC3 
  DIDR0 = (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D) | (1 << ADC3D); 
  // Auto Trigger und ADC Interrupt aktivieren 
  ADCSRA |= (1 << ADATE) | (1 << ADIE); 
  // ADC aktivieren 
  ADCSRA |= (1 << ADEN); 
  // Timer1 konfigurieren 
  TCCR1A = 0; // Normaler Modus, keine PWM 
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC-Modus, Prescaler = 64 
  OCR1A = 3124; // Compare Match A, entspricht einer Periode von 12.5ms 
  OCR1B = 3124; // Compare Match B für den ADC-Trigger 
  sei(); // Interrupts aktivieren 
} 
 
void loop() { 
  // Beispiel: Ausgabe der ADC-Werte über die serielle Schnittstelle 
  Serial.begin(115200); 
  for (uint8_t i = 0; i < 4; i++) { 
    Serial.print("ADC"); 
    Serial.print(i); 
    Serial.print(": "); 
    Serial.println(adcResults[i]); 
  } 
  delay(1000); 
} 
// ADC Interrupt Service Routine 
ISR(ADC_vect) { 
  // Speichere das aktuelle ADC-Ergebnis 
  adcResults[currentChannel] = ADC; 
    
  // Nächsten Kanal einstellen 
  currentChannel = (currentChannel + 1) % 4; // Zyklisch zwischen ADC0, ADC1, ADC2, ADC3 
  ADMUX = (ADMUX & 0xF0) | currentChannel; // Unteren 4 Bits von ADMUX setzen 
  // Compare Match Flag manuell zurücksetzen! 
  TIFR1 = (1 << OCF1B);   
}
