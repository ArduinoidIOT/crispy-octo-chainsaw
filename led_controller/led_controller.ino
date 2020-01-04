#include "LedController.h"

void loop() {
    switch (mode) {
      case 1: // Use animation "Train"
        transferState(train[counter]);         // Update 74HC595
        counter = (counter + 1) % 24; // Prevent overflow
        break;
       case 2: // Use alternator animation
        transferState(oscillating[counter]);         // Update 74HC595
        counter = (counter + 1) % 2; // Prevent overflow
        break;
       case 3: // Use flasher animation
        transferState(flasher[counter]);         // Update 74HC595
        counter = (counter + 1) % 2; // Prevent overflow
        break;
       case 4:
        transferState(bouncer[counter]);        // Update 74HC595
        counter = (counter + 1) % 30;           // Prevent overflow
        break;
       case 5:
        transferState(Entropy.randomWord());         // Update 74HC595
        break;
        
    }
    delay250();
}


void serialEvent() {
  serread = Serial.read();                      //Action character
  data = Serial.readStringUntil('\n').toInt();  // Mode/State
  switch (serread) {
    case 's':
      mode = 0;
      transferState(data);    // Update 74HC595
      break;
    case 'a':
      if (data <= 5) {
          mode = data;                    // Set animation
          counter = 0;                    // Reset counter
      }
      break;
  }
}
void beginSerial() {
  Serial.begin(9600);
}
void beginEntropy() {
  Entropy.initialize();
}
