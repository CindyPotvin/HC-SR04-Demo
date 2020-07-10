
/*
Shows the distance from a ultrasonic sensor on a 3 digit 7-segment module.

Hardware:
* Arduino Uno
* HR-SR04 ultrasonic sensor
* MCP23017 I/O Expander http://ww1.microchip.com/downloads/en/devicedoc/21952b.pdf
* Kingbright BC56-12SRWA 7-segment module (common cathode)

Pinout from: https://github.com/mizraith/mizraith-MCP23017.git
This shows the locations of the 16 pins as they relate to the pin number you'd use with 
the Arduino MCP23017 library.
These aren't the actual pin numbers given in the data sheet!
     ----u----
     8       7
     9       6
     10      5
     11      4
     12      3
     13      2
     14      1
     15      0
     |       |
     |       |
     |       |
     |       |
     |       |
     |       |
     ---------
*/

#include <HCSR04.h>
#include <Arduino.h>
#include <Adafruit_MCP23017.h>

const int NUMBERS[] = {
	 0b111111001011, // 0
	 0b100111001000, // 1
	 0b110111010011, // 2
	 0b110111011010, // 3
	 0b101111011000, // 4
	 0b111110011010, // 5
	 0b101110011011, // 6
	 0b110111001000, // 7
	 0b111111011011, // 8
	 0b111111011000, // 9
    0b100110000100, // DP Only
};
// The pin for the digit must be to ground. 
// Those output are at 1 in both NUMBERS and DIGITS. 
// We will use XOR with both array so the pin for the digit to show will end up
// as a 0 for the common cathode.
const int DIGITS[] = {
   0b100000000000, // First
	0b000100000000, // Third
   0b000010000000, // Second
};

int const ULTRASONIC_TRIGGER_PIN = 13;
int const ULTRASONIC_ECHO_PIN = 12;
// Initialize the pins to use for the ultrasonic sensor
UltraSonicDistanceSensor distanceSensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);

Adafruit_MCP23017 IOExpander;

void setup() {
   // Starts the connexion to the I/) expander with i2c
   IOExpander.begin();
   // Put all the pins in output mode, since all the pins used will drive 
   // the leds.
   for (int i=0; i < 16; i++) {
      IOExpander.pinMode(i, OUTPUT);
      }
   // Initialize serial connection to print values from sensor.
   Serial.begin(9600);  
}

void loop () {
   // Do a measurement using the sensor and print the distance in 
   // centimeters.
   int distance = (int)distanceSensor.measureDistanceCm();
   Serial.println("Distance");
   Serial.println(distance);
   char distanceArr[4];
   sprintf(distanceArr, "%03i", distance);
   for (int j=0; j< 100; j++) {
       // If distance greater than 0 print the number, otherwise light up 
       // the dots.
      if (distance > 0) {
         // Printing each digit in quick succession to display them all 
         // (multiplexing).
         for (int i=2; i >= 0; i--) {
            int numberIndex = distanceArr[i] - 48;
            int number = NUMBERS[numberIndex];
            IOExpander.writeGPIOAB(number ^ DIGITS[i]);    
            delay(5);
         }
      } else {
         for (int i=2; i >= 0; i--) {
            IOExpander.writeGPIOAB(NUMBERS[10] ^ DIGITS[i]); 
            delay(5);
         }
      }
   }
}