#include <AccelStepper.h>

// Pins
#define STEPPER1_DIR_PIN 3
#define STEPPER1_STEP_PIN 2

// Configuration
#define stepsPerDegree 400
#define DEFAULT_ACCELLERATION 1000 
#define DEFAULT_SPEED 3000

enum states { STOP, MOVING, LEFT_JOG, RIGHT_JOG, SET_ZERO, GOTO_ZERO, RAPID_ZERO, TRIAL };
states state = STOP;

AccelStepper stepper(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);

// Trial Phase Structure
struct Phase {
  float speed;            // steps/sec
  unsigned long duration; // milliseconds
  int direction;          // 1 for CW, -1 for CCW
};

// sequence (30 phases)
Phase trialPhases[30] = {
  {400, 5000, 1}, {2400, 5000, -1}, {1200, 5000, -1}, {1600, 5000, 1}, {800, 5000, -1},
  {1200, 5000, 1}, {800, 5000, 1}, {400, 5000, 1}, {2400, 5000, -1}, {1600, 5000, -1},
  {1600, 5000, -1}, {2400, 5000, 1}, {800, 5000, 1}, {400, 5000, -1}, {1200, 5000, -1},
  {400, 5000, 1}, {1200, 5000, -1}, {1600, 5000, 1}, {800, 5000, -1}, {2400, 5000, -1},
  {800, 5000, 1}, {1600, 5000, -1}, {2400, 5000, 1}, {1200, 5000, 1}, {400, 5000, -1},
  {2400, 5000, -1}, {800, 5000, 1}, {400, 5000, -1}, {1200, 5000, 1}, {1600, 5000, -1}
};



// Variables for Trial Management
int currentPhase = 0;
unsigned long phaseStartTime = 0;
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void setup() {
  Serial.begin(9600);
  stepper.setMaxSpeed(DEFAULT_SPEED);
  stepper.setAcceleration(DEFAULT_ACCELLERATION);
  Serial.println("System Ready. Type 'START' to begin trial or a number to move.");
}

void loop() {
  // Always call runSpeed() or run() to keep the motor turning
  if (state == TRIAL) {
    stepper.runSpeed();
  } else {
    stepper.run();
  }

  recvWithEndMarker();
  handleSerialCommands();

  switch (state) {
    case STOP:
      if (stepper.isRunning()) {
        stepper.stop();
        stepper.runToPosition();
      }
      break;

    case TRIAL:
      runTrialSequence();
      break;

    case MOVING:
      if (!stepper.isRunning()) {
        Serial.println("Target Reached.");
        state = STOP;
      }
      break;

    case GOTO_ZERO:
      stepper.moveTo(0);
      state = MOVING;
      break;

    case SET_ZERO:
      stepper.setCurrentPosition(0);
      Serial.println("Zero set.");
      state = STOP;
      break;
  }
}

// Logic to handle the sequence of 30 phases
void runTrialSequence() {
  unsigned long currentTime = millis();

  // Check if current phase duration has expired
  if (currentTime - phaseStartTime >= trialPhases[currentPhase].duration) {
    currentPhase++; // Move to next phase

    // Check if we finished all 30 phases
    if (currentPhase >= 30) {
      Serial.println("Trial Complete.");
      state = STOP;
      stepper.setSpeed(0);
    } else {
      startPhase(currentPhase);
    }
  }
}

// Helper to set up the motor for a specific phase
void startPhase(int phaseIndex) {
  phaseStartTime = millis();
  float targetSpeed = trialPhases[phaseIndex].speed * trialPhases[phaseIndex].direction;
  
  stepper.setSpeed(targetSpeed);
  
  Serial.print("Starting Phase: ");
  Serial.print(phaseIndex + 1);
  Serial.print(" | Speed: ");
  Serial.println(targetSpeed);
}

void handleSerialCommands() {
  if (newData) {
    // 1. Check for START command
    if (strcmp(receivedChars, "START") == 0) {
      currentPhase = 0;
      state = TRIAL;
      startPhase(currentPhase);
    } 
    // 2. Check for STOP command
    else if (strcmp(receivedChars, "STOP") == 0) {
      state = STOP;
      stepper.setSpeed(0);
      Serial.println("Emergency Stop.");
    } 
    // 3. If it's a number, move to that position (Degrees)
    else {
      float val = atof(receivedChars);
      if (val != 0 || receivedChars[0] == '0') {
        stepper.setMaxSpeed(DEFAULT_SPEED);
        stepper.moveTo(val * stepsPerDegree);
        state = MOVING;
        Serial.print("Moving to degree: ");
        Serial.println(val);
      }
    }
    newData = false;
  }
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker && rc != '\r') {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) ndx = numChars - 1;
    } else if (rc == endMarker) {
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
    }
  }
}