#include <Arduino.h>
#include <Stepper.h>
#include <IRremote.h>
#include "PinDefinitionsAndMore.h"


/*-----( Declare Constants and Pin Numbers )-----*/
/*-----( Declare objects )-----*/
/*-----( Declare Variables )-----*/

// Stepper variables & instances
const int stepsPerRev = 500;
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
void checkButton() {
  if (digitalRead(buttonMain) == 0) {
		dispenseTreats();
	}
}

// This function checks if the IR Receiver has received an infrared signal.
// If a signal has been received, decode the signal and make sure it comes from the correct remote and the correct button was pressed.
// If the correct button was pressed, dispense treats.
void checkIrRemote() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        Serial.println(F("Received noise or an unknown protocol"));
        IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();
    IrReceiver.resume();

    if (IrReceiver.decodedIRData.command == 0x40) {
        dispenseTreats();
    } 
  }
}

// Main loop function
void loop() {
  checkButton();
  checkIrRemote();
}