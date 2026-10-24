#pragma once

#include "Arduino.h"
#include <Servo.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "control_math.h"

// Pins
constexpr uint8_t BUZZER_PIN = 12;

constexpr uint8_t MOTOR_LEFT_FWD = 5;
constexpr uint8_t MOTOR_LEFT_REV = 3;
constexpr uint8_t MOTOR_RIGHT_FWD = 6;
constexpr uint8_t MOTOR_RIGHT_REV = 11;

constexpr uint8_t BUTTON_CALIBRATE = 8;
constexpr uint8_t BUTTON_COURSE = 13;
constexpr uint8_t BUTTON_TEST = 2;
constexpr uint8_t BUTTON_COPY = 4;

constexpr uint8_t MUX_SELECT_A = A0;
constexpr uint8_t MUX_SELECT_B = A1;
constexpr uint8_t MUX_SELECT_C = A2;
constexpr uint8_t MUX_INPUT = A3;

constexpr uint8_t GRIPPER_PIN = 10;
constexpr uint8_t LIFT_PIN = 9;

// Servo angles
constexpr int GRIPPER_OPEN = 5;
constexpr int GRIPPER_CLOSED = 20;
constexpr int LIFT_UP = 107;
constexpr int LIFT_DOWN = 147;
constexpr int LIFT_PLACE = 115;

// Line sensor calibration
constexpr uint8_t DEFAULT_THRESHOLDS[SENSOR_COUNT] = {113, 134, 127, 129,
                                                      124, 132, 121, 103};
constexpr int CALIBRATION_MARKER = 0x5A5B;
constexpr unsigned long SEARCH_TIMEOUT_MS = 10000UL;
constexpr int MIN_LINE_STRENGTH = 400;
constexpr int MOTOR_DEADBAND = 40;

constexpr uint8_t LEFT_EDGE_MASK = 0b10000000;
constexpr uint8_t CENTER_EDGE_MASK = 0b01000010;

// Line following
constexpr int SET_POINT = 0;
constexpr int KP = 5;
constexpr int KD = 10;
constexpr int DERIVATIVE_ALPHA = 30;
constexpr int SAMPLE_MS = 4;
constexpr int MIN_PWM = 0;
constexpr int EDGE_POSITION = 28;
constexpr int PIVOT_SPEED = 140;
constexpr int SEARCH_SPEED = 100;
constexpr uint8_t DEBOUNCE_SAMPLES = 3;

// Dashed-line following (terpisah dari followLine)
constexpr unsigned long DASHED_COAST_MS = 250;
constexpr unsigned long DASHED_SEARCH_MS = 1500;
constexpr unsigned long DASHED_SWEEP_MS = 180;
constexpr uint8_t DASHED_REACQUIRE_SAMPLES = 3;
constexpr int DASHED_RELEASE_STRENGTH = 300;
constexpr int DASHED_SEARCH_SPEED = 100;
constexpr int DASHED_SWEEP_BIAS = 55;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo gripper;
Servo lift;

uint8_t rawSensor[SENSOR_COUNT];
uint8_t sensorOnLine[SENSOR_COUNT];
int threshold[SENSOR_COUNT];
int sensorMin[SENSOR_COUNT];
int sensorMax[SENSOR_COUNT];
uint8_t sensorMask = 0;
long lineStrength = 0;
int linePosition = 0;
int lastError = 0;
int filteredDerivative = 0;
int lastSeenSide = 1;

bool dashedLineWasVisible = false;
bool dashedInGap = false;
unsigned long dashedHiddenSince = 0;
int dashedLastCorrection = 0;
int dashedLastError = 0;
int dashedFilteredDerivative = 0;
uint8_t dashedReacquireStreak = 0;

void selectMuxChannel(uint8_t channelA, uint8_t channelB, uint8_t channelC) {
  digitalWrite(MUX_SELECT_A, channelA);
  digitalWrite(MUX_SELECT_B, channelB);
  digitalWrite(MUX_SELECT_C, channelC);
}

void readSensors() {
  selectMuxChannel(1, 1, 1);
  rawSensor[7] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(0, 1, 1);
  rawSensor[6] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(1, 0, 1);
  rawSensor[5] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(0, 0, 1);
  rawSensor[4] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(1, 1, 0);
  rawSensor[3] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(0, 1, 0);
  rawSensor[2] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(1, 0, 0);
  rawSensor[1] = analogRead(MUX_INPUT) / 4;
  selectMuxChannel(0, 0, 0);
  rawSensor[0] = analogRead(MUX_INPUT) / 4;
}

void displayText(int size, int x, int y, const char *text, bool refresh,
                 bool clear) {
  if (clear) {
    display.clearDisplay();
  }
  display.setTextSize(size);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(text);
  if (refresh) {
    display.display();
  }
}

void drawProgressBar(int x, int y, int w, int h, int percent) {
  float width = ((float)(w - 1) / 100) * percent;
  display.drawRect(x, y, w, h, WHITE);
  display.fillRect(x + 2, y + 2, width, h - 4, WHITE);
}

void saveCalibration() {
  int address = 0;
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    EEPROM.put(address, threshold[i]);
    address += sizeof(int);
  }
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    EEPROM.put(address, sensorMin[i]);
    address += sizeof(int);
  }
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    EEPROM.put(address, sensorMax[i]);
    address += sizeof(int);
  }
  EEPROM.put(address, CALIBRATION_MARKER);
}

bool loadCalibration() {
  int storedThreshold[SENSOR_COUNT];
  int storedMin[SENSOR_COUNT];
  int storedMax[SENSOR_COUNT];
  int address = 0;

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    EEPROM.get(address, storedThreshold[i]);
    address += sizeof(int);
  }
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    EEPROM.get(address, storedMin[i]);
    address += sizeof(int);
  }
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    EEPROM.get(address, storedMax[i]);
    address += sizeof(int);
  }

  int marker = 0;
  EEPROM.get(address, marker);
  if (marker != CALIBRATION_MARKER) {
    return false;
  }

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    threshold[i] = storedThreshold[i];
    sensorMin[i] = storedMin[i];
    sensorMax[i] = storedMax[i];
  }
  return true;
}

void loadDefaultCalibration() {
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    threshold[i] = DEFAULT_THRESHOLDS[i];
    sensorMin[i] = 0;
    sensorMax[i] = 255;
  }
}

void calibrate(int samples) {
  int minReading[SENSOR_COUNT];
  int maxReading[SENSOR_COUNT];

  display.clearDisplay();
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    minReading[i] = 255;
    maxReading[i] = 0;
  }

  for (int sample = 0; sample < samples; sample++) {
    drawProgressBar(0, 25, 120, 10, sample);
    display.display();
    readSensors();
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
      if (rawSensor[i] < minReading[i]) {
        minReading[i] = rawSensor[i];
      }
      if (rawSensor[i] > maxReading[i]) {
        maxReading[i] = rawSensor[i];
      }
      delay(1);
    }
  }

  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    if (maxReading[i] - minReading[i] >= MIN_CONTRAST) {
      sensorMin[i] = minReading[i];
      sensorMax[i] = maxReading[i];
    }
    threshold[i] = (sensorMin[i] + sensorMax[i]) / 2;
  }
  saveCalibration();
}

void driveMotors(int leftPwm, int rightPwm) {
  leftPwm = applyDeadband(leftPwm, MOTOR_DEADBAND);
  rightPwm = applyDeadband(rightPwm, MOTOR_DEADBAND);

  if (leftPwm > 0) {
    analogWrite(MOTOR_LEFT_FWD, leftPwm);
    analogWrite(MOTOR_LEFT_REV, 0);
  } else if (leftPwm < 0) {
    analogWrite(MOTOR_LEFT_FWD, 0);
    analogWrite(MOTOR_LEFT_REV, -leftPwm);
  } else {
    analogWrite(MOTOR_LEFT_FWD, 0);
    analogWrite(MOTOR_LEFT_REV, 0);
  }

  if (rightPwm > 0) {
    analogWrite(MOTOR_RIGHT_FWD, rightPwm);
    analogWrite(MOTOR_RIGHT_REV, 0);
  } else if (rightPwm < 0) {
    analogWrite(MOTOR_RIGHT_FWD, 0);
    analogWrite(MOTOR_RIGHT_REV, -rightPwm);
  } else {
    analogWrite(MOTOR_RIGHT_FWD, 0);
    analogWrite(MOTOR_RIGHT_REV, 0);
  }
}

void brake(int durationMs) {
  driveMotors(0, 0);
  delay(durationMs);
}

void stopMotors() {
  driveMotors(0, 0);
  gripper.write(GRIPPER_CLOSED);
}

void beep(int times, int durationMs) {
  for (int i = 1; i <= times; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(durationMs);
    digitalWrite(BUZZER_PIN, LOW);
    delay(durationMs);
  }
}

void updateLineState() {
  readSensors();

  uint16_t strength[SENSOR_COUNT];
  sensorMask = 0;
  for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
    strength[i] = normalizedStrength(rawSensor[i], sensorMin[i], sensorMax[i]);
    sensorOnLine[i] = rawSensor[i] > threshold[i] ? 1 : 0;
    sensorMask = (sensorMask << 1) | sensorOnLine[i];
  }
  linePosition = centroidPosition(strength, &lineStrength);
}

void followLine(int baseSpeed) {
  updateLineState();

  if (lineStrength < MIN_LINE_STRENGTH) {
    int direction = lastSeenSide >= 0 ? 1 : -1;
    lastError = 0;
    filteredDerivative = 0;
    driveMotors(direction * SEARCH_SPEED, -direction * SEARCH_SPEED);
    delay(SAMPLE_MS);
    return;
  }
  lastSeenSide = linePosition >= 0 ? 1 : -1;

  int error = SET_POINT - linePosition;
  int derivative = error - lastError;
  lastError = error;
  filteredDerivative = lowPassStep(filteredDerivative, derivative, DERIVATIVE_ALPHA);

  int correction = KP * error + KD * filteredDerivative;
  correction = constrain(correction, -baseSpeed, baseSpeed);

  int leftPwm = constrain(baseSpeed - correction, MIN_PWM, baseSpeed);
  int rightPwm = constrain(baseSpeed + correction, MIN_PWM, baseSpeed);
  driveMotors(leftPwm, rightPwm);

  if (linePosition <= -EDGE_POSITION) {
    driveMotors(-PIVOT_SPEED, PIVOT_SPEED);
  }
  if (linePosition >= EDGE_POSITION) {
    driveMotors(PIVOT_SPEED, -PIVOT_SPEED);
  }

  delay(SAMPLE_MS);
}

// Line follower untuk garis putus-putus: menjembatani celah pendek dengan
// koreksi terakhir, lalu menyapu jika garis tidak kembali.
void followLineDashed(int baseSpeed) {
  updateLineState();

  bool visible = lineSeen((int)lineStrength, dashedLineWasVisible,
                          MIN_LINE_STRENGTH, DASHED_RELEASE_STRENGTH);

  if (visible) {
    dashedLineWasVisible = true;
    if (dashedReacquireStreak < DASHED_REACQUIRE_SAMPLES) {
      dashedReacquireStreak++;
    }
  } else {
    dashedLineWasVisible = false;
    dashedReacquireStreak = 0;
  }

  bool reacquired =
      visible &&
      (!dashedInGap || dashedReacquireStreak >= DASHED_REACQUIRE_SAMPLES);

  if (reacquired) {
    if (dashedInGap) {
      // Bumpless reacquire: cegah lonjakan suku D saat posisi garis melompat.
      dashedLastError = SET_POINT - linePosition;
      dashedFilteredDerivative = 0;
      dashedInGap = false;
    }

    int error = SET_POINT - linePosition;
    int derivative = error - dashedLastError;
    dashedLastError = error;
    dashedFilteredDerivative =
        lowPassStep(dashedFilteredDerivative, derivative, DERIVATIVE_ALPHA);

    int correction = KP * error + KD * dashedFilteredDerivative;
    correction = constrain(correction, -baseSpeed, baseSpeed);
    dashedLastCorrection = correction;

    int leftPwm = constrain(baseSpeed - correction, MIN_PWM, baseSpeed);
    int rightPwm = constrain(baseSpeed + correction, MIN_PWM, baseSpeed);
    driveMotors(leftPwm, rightPwm);
    delay(SAMPLE_MS);
    return;
  }

  if (!dashedInGap) {
    dashedInGap = true;
    dashedHiddenSince = millis();
  }

  unsigned long hiddenMs = millis() - dashedHiddenSince;
  GapPhase phase = gapPhase(hiddenMs, DASHED_COAST_MS, DASHED_SEARCH_MS);

  if (phase == GAP_COAST) {
    int coastCorrection =
        constrain(dashedLastCorrection, -baseSpeed, baseSpeed);
    int leftPwm = constrain(baseSpeed - coastCorrection, MIN_PWM, baseSpeed);
    int rightPwm = constrain(baseSpeed + coastCorrection, MIN_PWM, baseSpeed);
    driveMotors(leftPwm, rightPwm);
    delay(SAMPLE_MS);
    return;
  }

  if (phase == GAP_LOST) {
    // Berhenti tanpa stopMotors(): jangan menulis servo gripper tiap sampel.
    driveMotors(0, 0);
    delay(SAMPLE_MS);
    return;
  }

  // GAP_SEARCH: maju pelan sambil menyapu kiri/kanan.
  int direction = sweepDirection(hiddenMs, DASHED_COAST_MS, DASHED_SWEEP_MS);
  int leftPwm = DASHED_SEARCH_SPEED + direction * DASHED_SWEEP_BIAS;
  int rightPwm = DASHED_SEARCH_SPEED - direction * DASHED_SWEEP_BIAS;
  driveMotors(constrain(leftPwm, 0, 255), constrain(rightPwm, 0, 255));
  delay(SAMPLE_MS);
}

void driveForward(int leftPwm, int rightPwm, int seconds) {
  for (int i = 0; i <= seconds * 1000; i++) {
    driveMotors(leftPwm, rightPwm);
  }
}

void driveBackward(int leftPwm, int rightPwm, int seconds) {
  for (int i = 0; i <= seconds * 1000; i++) {
    driveMotors(-leftPwm, -rightPwm);
  }
}

void followLineForTicks(int speed, int ticks) {
  for (int i = 0; i <= ticks * 10; i++) {
    followLine(speed);
  }
}

void followLineForTicksDashed(int speed, int ticks) {
  for (int i = 0; i <= ticks * 10; i++) {
    followLineDashed(speed);
  }
}

void turnRight(int leftPwm, int rightPwm, int ticks) {
  for (int i = 0; i <= ticks * 500; i++) {
    driveMotors(leftPwm, -rightPwm);
  }
}

void turnLeft(int leftPwm, int rightPwm, int ticks) {
  for (int i = 0; i <= ticks * 500; i++) {
    driveMotors(-leftPwm, rightPwm);
  }
}

void liftUp(int stepDelay) {
  for (int angle = LIFT_DOWN; angle >= LIFT_UP; angle--) {
    lift.write(angle);
    delay(stepDelay);
  }
}

void liftDown(int stepDelay) {
  for (int angle = LIFT_UP; angle <= LIFT_DOWN; angle++) {
    lift.write(angle);
    delay(stepDelay);
  }
}

void waitCrossings(uint8_t mask, int crossings, int speed) {
  int count = 0;
  unsigned long startedAt = millis();

  while (count < crossings) {
    int streak = 0;
    while (streak < DEBOUNCE_SAMPLES) {
      if (millis() - startedAt > SEARCH_TIMEOUT_MS) {
        stopMotors();
        return;
      }
      streak = ((sensorMask & mask) == mask) ? streak + 1 : 0;
      followLine(speed);
    }

    streak = 0;
    while (streak < DEBOUNCE_SAMPLES) {
      if (millis() - startedAt > SEARCH_TIMEOUT_MS) {
        stopMotors();
        return;
      }
      streak = ((sensorMask & mask) != mask) ? streak + 1 : 0;
      followLine(speed);
    }

    beep(1, 5);
    count++;
  }
}

void initHardware() {
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(MOTOR_RIGHT_FWD, OUTPUT);
  pinMode(MOTOR_RIGHT_REV, OUTPUT);
  pinMode(MOTOR_LEFT_FWD, OUTPUT);
  pinMode(MOTOR_LEFT_REV, OUTPUT);

  pinMode(BUTTON_CALIBRATE, INPUT);
  pinMode(BUTTON_COURSE, INPUT);
  pinMode(BUTTON_TEST, INPUT);
  pinMode(BUTTON_COPY, INPUT);

  pinMode(MUX_SELECT_A, OUTPUT);
  pinMode(MUX_SELECT_B, OUTPUT);
  pinMode(MUX_SELECT_C, OUTPUT);
  pinMode(MUX_INPUT, INPUT);

  gripper.attach(GRIPPER_PIN);
  lift.attach(LIFT_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  gripper.write(GRIPPER_CLOSED);
  lift.write(LIFT_UP);

  if (!loadCalibration()) {
    loadDefaultCalibration();
  }
}
