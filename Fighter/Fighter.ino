#include "Motor.h"
#include "Pitches.h"

/************************/
/** PIN CONFIGURATIONS **/
/************************/
// LED pin
#define led 13

// Hardware pins
#define button 2
#define buzzer 3

// IR Sensor pins
#define leftSensor  A1
#define rightSensor A2
#define IREmitter   4

// Motor pins
#define leftMotorSpeed 5
#define rightMotorSpeed 6
#define leftMotorDirection 7
#define rightMotorDirection 8

// Ultrasonic sensor pins
#define echoPin A0
#define pingPin 10


/*************************************************/
/** Global Variables and Configuration Settings **/
/*************************************************/
// Competition configuration settings
#define abortThreshold 200
#define searchTime     2000
#define attackDistance 22

// competition configuration variables
int turnDirection      = 1; // 1 = left, 2 = right
unsigned long turnTime = 0; // holds the return from millis()

// Create our motor object
Motor motor;

// State variables
int buttonState = 1; // 1 = up, 0 = pressed
int state       = 1; // 1 = idle, 2 = competing
// variables to know which sensor touched border first
boolean isLeftSensorTouchingBorder = 0;
boolean isRightSensorTouchingBorder = 0;

void setup() {
  Serial.begin(9600);

  // pin setup
  pinMode(button, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  // IR sensors
  pinMode(leftSensor,  INPUT);
  pinMode(rightSensor, INPUT);
  pinMode(IREmitter,   OUTPUT);

  // Turn on our IR Emitter
  digitalWrite(IREmitter, HIGH);

  // ultrasonic sensors
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Motor setup
  motor.setupRight(rightMotorSpeed, rightMotorDirection);
  motor.setupLeft(leftMotorSpeed, leftMotorDirection);

  Serial.println("Robot powered up. Waiting for button press.");
}

void loop() {
  if ( buttonState == 1 && digitalRead(button) == 0 ) {
    buttonState = 0;
  }
  if ( buttonState == 0 && digitalRead(button) == 1 ) {
    buttonState = 1;

    if ( state == 1 ) {
      // we were in idle mode, so let's begin our
      // competition countdown
      Serial.println("Competition countdown starting...");
      Serial.println("Good luck!");
      begin_countdown();
    } else {
      state = 1;
      motor.left(0);
      motor.right(0);
      Serial.println("Robot entering idle mode.");
    }
  }

  // if the countdown has ended, we're now in competition mode
  if ( state == 2 ) {
    compete();
  }
}

/***************************************************/
/** Competition State Functions                   **/
/** These functions control the operational state **/
/** of our robot while it's in state 2.           **/
/***************************************************/

// This function is the primary competition control
// function and controls the operational flow and state
// of the robot in competition mode (state 2).
void compete() {
  // read our sensor values to determine what we
  // need our robot to do.
  int distance = msToCm( ping() );
  Serial.println(distance);
  int leftIR   = analogRead(leftSensor);
  int rightIR  = analogRead(rightSensor);
  Serial.println(leftIR);
  Serial.println(rightIR);
  if (leftIR < abortThreshold)
    isLeftSensorTouchingBorder = 1;
  else
    isLeftSensorTouchingBorder = 0;

  if (rightIR < abortThreshold)
    isRightSensorTouchingBorder = 1;
  else
    isRightSensorTouchingBorder = 0;

  if ( leftIR < abortThreshold || rightIR < abortThreshold ) {
    // if we have detected a ring border, abort!
    abortBackup(isLeftSensorTouchingBorder,isRightSensorTouchingBorder);
  } else if ( distance < attackDistance ) {
    // We have detected an enemy. Attack!
    attackNow();
  } else {
    // We don't see an enemy, search until we find one.
    search();
  }
}

// This function is a simple search algorithm that
// attempts to locate the opponent before moving
// into attack mode.
void search() {
  // our time has exceeded our configured searchTime, reset.
  if ( turnTime != 0 && ((millis() - turnTime) > searchTime) ) {
    turnTime = 0;
  }

  // get random direction and store the start time
  if ( turnTime == 0 ) {
    turnDirection = random(1, 3);
    turnTime  = millis();
  }

  // start our turn, 1 = left, 2 = right
  if ( turnDirection == 1 ) {
    motor.left(255);
    motor.right(-255);
  } else {
    motor.left(-255);
    motor.right(255);
  }
}

// The attackNow() function is used to control the robot
// when an opponent has been detected closer than the
// defined attackDistance. It will move ahead at full
// speed in an attempt to push the opponent outside of
// the ring.
void attackNow() {
  motor.left(255);
  motor.right(255);
}

// The abortBackup() function is an interrupting function that
// is called whenever the infrared sensors detect the
// ring border. This function will immediately halt
// other control function operations and move in reverse
// at full speed.
void abortBackup(boolean isLeft, boolean isRight) {
  if (isLeft && isRight) {
    // turn anticlockwise
    motor.left(-255);
    motor.right(-255);
    delay(300);
  } else if (isLeft) {
    // turn anticlockwise
    motor.left(-255);
    motor.right(255);
    delay(300);
  } else if (isRight) {
    // turn clockwise
    motor.left(255);
    motor.right(-255);
    delay(300);
  }
  motor.left(-255);
  motor.right(-255);
  delay(1000);
}


// This function will begin our competition countdown sequence.
// When the button is depressed when in idle mode, this function
// will be called and begin a 5 second countdown with audible
// beeps at every second.
//
// PRACTICAL EXERCISE SOLUTION:
// This function also turns on the LED each time a note is
// played on the buzzer.
void begin_countdown() {
  for ( int i = 0; i < 3; i++ ) {
    delay(1000); // wait 1 full second

    // blink angrily at our opponent
    digitalWrite(led, HIGH);

    // play note G3 for 200ms with a 15ms rest
    playNote(NOTE_G3, 200, 15);
    digitalWrite(led, LOW);
  }

  // wait one more full second before playing our next note
  delay(1000);

  digitalWrite(led, HIGH);
  // Play a higher note for a 500ms
  playNote(NOTE_G4, 500, 15);
  digitalWrite(led, LOW);

  // Our countdown is nearly complete. Wait 1
  // more second and then set state to 2 so that
  // we can begin our competition!
  delay(1000);
  state = 2;
  Serial.println("War starting");
}

/**********************/
/** Helper Functions **/
/**********************/
// Note-playing buzzer helper function.
void playNote(int note, int duration, int rest) {
  tone(buzzer, note, duration);
  delay(rest);
}

// Helper function to manage our ultrasonic sensor.
long ping() {
  long duration;
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(echoPin, HIGH, 5500);  //setting the timeout to 5500 is helpful but will return 0 whenever pulseIn() times out
  if (duration == 0) {
    duration = 5500;  //here we change the 0 value to the actual timeout limit
  }
  return duration;
}

// Helper function to return the distance to an object
// detected by the ultrasonic sensor in centimeters.
long msToCm(long microseconds) {
  return microseconds / 29 / 2;
}

// Simple blink function called in loop() whenever a state
// change is made by a user button press.
void blink(int blinks) {
  for ( int i = 0; i <= blinks; i++ ) {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
}
