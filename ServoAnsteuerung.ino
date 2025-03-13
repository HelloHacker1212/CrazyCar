// Servo Motor Controller ohne Servo.h-Bibliothek (mit Timer4)
// Leonit Emmerlhau


void setup() {
  pinMode(6, OUTPUT); // Pin 6 als Ausgang (OC4A)

  // Timer4 initialisieren für Fast PWM mit ICR4 als TOP-Wert
  TCCR4A = 0;
  TCCR4B = 0;
  
  // Fast PWM Mode mit ICR4 als TOP-Wert (50 Hz / 20 ms Periode)
  TCCR4A = (1 << COM4A1) | (1 << WGM41);  // Clear OC4A on Compare Match, Fast PWM Mode
  TCCR4B = (1 << WGM43) | (1 << WGM42) | (1 << CS41); // Prescaler 8 = 16 MHz / 8 = 2 MHz, Fast PWM mit ICR4 als TOP

  ICR4 = 20000; // Setze den TOP-Wert für 50 Hz PWM (20 ms Periode)
  // Servo arbeitet bei 50 Hz, wobei die ersten 1-2 ms bestimmen, wie weit der Servo gedreht wird
}

void loop() {
  control(0);   // Setze den Servo auf 0 Grad
  delay(1000);  // Warte 1 Sekunde
  control(90);  // Setze den Servo auf 90 Grad
  delay(1000);  // Warte 1 Sekunde
  control(180); // Setze den Servo auf 180 Grad
  delay(1000);  // Warte 1 Sekunde
}

// Funktion zur Steuerung des Servos basierend auf Grad
void control(int grad) {
  // Grad von 0 bis 180 auf PWM-Werte von 1000 bis 4950 umrechnen
  OCR4A = map(grad, 0, 180, 1000, 4950); 
}
