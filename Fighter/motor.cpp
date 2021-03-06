#include "Arduino.h"
#include "Motor.h"

// Define our minimum and maximum speeds for error checking.
#define maxSpeed 255
#define minSpeed -255

// This is our class constructor. When this method is called
// the class will create an object of this instance. We don't
// need our motor constructor to do anything, so we will leave
// it empty.
Motor::Motor() {
  // Intentionally do nothing
}

void Motor::setupRight(int rightSpeed_pin, int rightDirection_pin) {
  // Store the pin in a private variable for later use
  _rightMotorSpeedPin     = rightSpeed_pin;
  _rightMotorDirectionPin = rightDirection_pin;

  // Set up pins for output
  pinMode(_rightMotorSpeedPin,     OUTPUT);
  pinMode(_rightMotorDirectionPin, OUTPUT);
}


void Motor::setupLeft(int leftSpeed_pin, int leftDirection_pin) {
  // Store the pin in a private variable for later use
  _leftMotorSpeedPin     = leftSpeed_pin;
  _leftMotorDirectionPin = leftDirection_pin;

  // Set up pins for output
  pinMode(_leftMotorSpeedPin,     OUTPUT);
  pinMode(_leftMotorDirectionPin, OUTPUT);
}

void Motor::right(int speed) {
  // Simple check to make sure our speed does not exceed
  // the defined speed limits.
  if ( speed > maxSpeed ) {
    speed = maxSpeed;
  } else if ( speed < minSpeed ) {
    speed = minSpeed;
  }

  if ( speed < 0 ) {
    digitalWrite(_rightMotorDirectionPin, LOW);
    analogWrite(_rightMotorSpeedPin, abs(speed));
  } else {
    digitalWrite(_rightMotorDirectionPin, HIGH);
    analogWrite(_rightMotorSpeedPin, speed);
  }
}

void Motor::left(int speed) {
  if ( speed > maxSpeed ) {
    speed = maxSpeed;
  } else if ( speed < minSpeed ) {
    speed = minSpeed;
  }

  if ( speed < 0 ) {
    digitalWrite(_leftMotorDirectionPin, HIGH);
    analogWrite(_leftMotorSpeedPin, abs(speed));
  } else {
    digitalWrite(_leftMotorDirectionPin, LOW);
    analogWrite(_leftMotorSpeedPin, speed);
  }
}

// Our attack() method is a very simple function that will
// tell both motors to charge forward at full speed.
void Motor::attack() {
  right(maxSpeed);
  left(maxSpeed);
}

// Our abort() method is a very simple function that will
// tell both motors to reverse at full speed.
void Motor::abortNow() {
  right(minSpeed);
  left(minSpeed);
}
