#include <Arduino.h>
#include <Stepper.h>

// Stepper variables & instances
const int stepsPerRev = 509;
const int stepperSpeed = 50;
const int stepperDwell = 300;
Stepper stepper = Stepper(stepsPerRev, 2, 4, 3, 5);

// main dispenser button
const int buttonMain = 22;


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

void setup() {
	Serial.begin(9600);
	pinMode(buttonMain, INPUT_PULLUP);
}

void loop() {
	if (digitalRead(buttonMain) == 0) {
		dispenseTreats();
	}
	
	
	
}