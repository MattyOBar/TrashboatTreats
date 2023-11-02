#include <Arduino.h>
#include <Stepper.h>
#include <IRremote.h>
#include "PinDefinitionsAndMore.h"


/*-----( Declare Constants and Pin Numbers )-----*/
/*-----( Declare objects )-----*/
/*-----( Declare Variables )-----*/

// Stepper variables & instances
const int stepsPerRev = 509;
const int stepperSpeed = 50;
const int stepperDwell = 300;
Stepper stepper = Stepper(stepsPerRev, 49, 47, 48, 46);

// main dispenser button
const int buttonMain = 22;

// IR receiver
#define DECODE_NEC
// const int IR_RECEIVE_PIN = 2;

// Setup function
void setup() {
	Serial.begin(115200);
	
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

	pinMode(buttonMain, INPUT_PULLUP);
}



// This function activates the stepper motor to dispense treats
// stepperSpeed adjusts the speed of the motor
// stepsPerRev adjusts the length of the rotation
// stepperDwell adjusts the time dwell inbetween rotations
void dispenseTreats() {
	stepper.setSpeed(stepperSpeed);
	stepper.step(stepsPerRev);
	delay(stepperDwell);

	stepper.setSpeed(stepperSpeed);
	stepper.step(-stepsPerRev);
	delay(stepperDwell);
}

// This function checks is the treat dispense button has been pressed.
// If the button has been pressed, dispenseTreats() is called to give Trashboat his tasty treats.
void buttonDispense() {
  if (digitalRead(buttonMain) == 0) {
		dispenseTreats();
	}
}

// Main loop function
void loop() {
  buttonDispense();
	 /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     *
     * E.g. command is in IrReceiver.decodedIRData.command
     * address is in command is in IrReceiver.decodedIRData.address
     * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
     */
    if (IrReceiver.decode()) {

        /*
         * Print a short summary of received data
         */
        IrReceiver.printIRResultShort(&Serial);
        IrReceiver.printIRSendUsage(&Serial);
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
            // We have an unknown protocol here, print more info
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        }
        Serial.println();

        /*
         * !!!Important!!! Enable receiving of the next value,
         * since receiving has stopped after the end of the current received data packet.
         */
        IrReceiver.resume(); // Enable receiving of the next value

        /*
         * Finally, check the received data and perform actions according to the received command
         */
        if (IrReceiver.decodedIRData.command == 0x40) {
            dispenseTreats();
        } 
    }
}