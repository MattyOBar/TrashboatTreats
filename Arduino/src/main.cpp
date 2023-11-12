#include <Arduino.h>
#include <Stepper.h>
#include <IRremote.h>
#include "PinDefinitionsAndMore.h"
#include <TM1637Display.h>


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

// timer display
#define CLK 26
#define DIO 27
TM1637Display display = TM1637Display(CLK, DIO);
const uint8_t allON[] = {0xff, 0xff, 0xff, 0xff};
const uint8_t allOFF[] = {0x00, 0x00, 0x00, 0x00};

// // Treat Counter
#define CLK2 28
#define DIO2 29
TM1637Display treatDisplay = TM1637Display(CLK2, DIO2);
int treatCount = 0;


boolean startRan = false;

// Setup function
void setup() {
	Serial.begin(115200);
	
  display.setBrightness(5);
  display.setSegments(allON);
  treatDisplay.setBrightness(5);
  treatDisplay.setSegments(allON);

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);
  Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));


	pinMode(buttonMain, INPUT_PULLUP);

  delay(500);
  display.clear();
  treatDisplay.clear();
  
}

// This function only runs at the start.
void displaysOn() {
  if (startRan == false) {
    display.showNumberDecEx(100, 0b01000000, true, 4, 0);
    treatDisplay.showNumberDec(treatCount, true);
    startRan = true;
  }
}

// This function starts the 1 minute countdown on the 7 segment 4 digit display.
// The timer is purposely blocking other functions from running, so as to prevent Trashboat from getting to many treats.
void startTimer() {
  delay(1000);
  for (int i = 5; i != 0; i--) {
    display.showNumberDec(i, true);
    delay(1000);
  }
  display.showNumberDec(0, true);
  delay(1000);
  display.showNumberDecEx(100, 0b01000000, true, 4, 0);
}

// This function updates the local count for how many times treats have been dispensed.
void updateTreatCounter() {
  treatCount += 1;
  treatDisplay.showNumberDec(treatCount, true);
  Serial.println("treatCount = " + treatCount);
}

// This function activates the stepper motor to dispense treats.
// stepperSpeed adjusts the speed of the motor.
// stepsPerRev adjusts the length of the rotation.
// stepperDwell adjusts the time dwell inbetween rotations.
void dispenseTreats() {
	stepper.setSpeed(stepperSpeed);
	stepper.step(stepsPerRev);
	delay(stepperDwell);

	stepper.setSpeed(stepperSpeed);
	stepper.step(-stepsPerRev);
  updateTreatCounter();
	startTimer();

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
  displaysOn();
  checkButton();
  delay(50);
  checkIrRemote();
  // Serial.println(startRan);
}