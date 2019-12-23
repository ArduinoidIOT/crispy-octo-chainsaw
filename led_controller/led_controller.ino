#include "LedController.h"

void setup() {
  Serial.begin(9600);   //Initialize serial 
  Entropy.initialize(); // Initialize random number generator
  initSPI();            // Initialize SPI
  updateShiftRegister();// Clear shift register
  lastmillis = millis();// Set time start
}

void loop() {
  k = millis();
  if(k-lastmillis >= 250) {
    lastmillis = k;
    switch (mode) {
      case 1: // Use animation "Train"
        leds = train[counter];
        counter = (counter + 1) % 24; // Prevent overflow
        updateShiftRegister();       // Update 74HC595
        break;
       case 2: // Use alternator animation
        leds = oscillating[counter];
        counter = (counter + 1) % 2; // Prevent overflow
        updateShiftRegister();       // Update 74HC595
        break;
       case 3: // Use flasher animation
        leds = flasher[counter];
        counter = (counter + 1) % 2; // Prevent overflow
        updateShiftRegister();       // Update 74HC595
        break;
       case 4:
        leds = bouncer[counter];
        counter = (counter + 1)%31; // Prevent overflow
        updateShiftRegister();       // Update 74HC595
        break;
       case 5:
        leds = Entropy.randomWord(); // Get random 16-bit word
        updateShiftRegister();       // Update 74HC595
        break;
        
    }
  }
}


void initSPI() {
  uint8_t sreg = SREG; //Save SREG
  cli();               // Disable Interrupts
  DDRB |= DDRB_MASK;   //Set Data direction
  SPCR |= SPCR_MASK;   // Set SPI mode
  SPSR |= _BV(SPI2X);  // Set SPI double speed mode;
  SREG = sreg;         // Restore SREG
}

void serialEvent() {
  serread = Serial.read();                      //Action character
  data = Serial.readStringUntil('\n').toInt();  // Mode/State
  switch (serread) {
    case 's':
      leds = data;            //Set state
      mode = 0;               // Disable animations
      updateShiftRegister();  // Update 74HC595
      break;
    case 'a':
      if (data <= 5) {
          if (mode == 0) oldleds = leds;  // Save state
          if (data == 0) {
            leds = oldleds;               //Retrieve state
            updateShiftRegister();        // Update 74HC595
          }
          mode = data;                    // Set animation
          counter = 0;                    // Reset counter
      }
      break;
  }
}
void updateShiftRegister() {
  if (!transaction_in_progress) {
    transaction_in_progress = true;  //Set transaction flag
    cbi(PORTB, PB2);                 //Drive latch/Pin 10 LOW
    SPDR = leds & 255;               // Update SPI Data register 
    while (!(SPSR & _BV(SPIF)));     // Wait for transaction complete
    SPDR = (leds >> 8) & 255;        // Update SPI Data register
    while (!(SPSR & _BV(SPIF)));     // Wait for transaction complete
    sbi(PORTB, PB2);                 //Drive latch/Pin 10 HIGH
    transaction_in_progress = false; // Clear transaction flag
  }
}
