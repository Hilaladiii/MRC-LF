#include "track.h"

void setup() {
  initHardware();
}

void runMenu() {
START:
  beep(1, 10);
  displayText(2, 10, 25, "1 > Ready", true, true);
  delay(250);

MENU:
  if (digitalRead(BUTTON_CALIBRATE) == 1) {
    displayText(2, 15, 20, "Calibrate", true, true);
    delay(250);
    stopMotors();
    calibrate(100);
    beep(2, 10);
    displayText(2, 35, 25, "Finish", true, true);
    delay(250);
    display.clearDisplay();
    goto START;
  }

  if (digitalRead(BUTTON_COURSE) == 1) {
    delay(150);
    runCourse();
    stopMotors();
    goto MENU;
  }

  if (digitalRead(BUTTON_TEST) == 1) {
    delay(150);
TEST:
    followLine(250);
    if (digitalRead(BUTTON_TEST) == 1) {
      delay(250);
      goto MENU;
    }
    goto TEST;
  }

  if (digitalRead(BUTTON_COPY) == 1) {
    delay(150);
    loadDefaultCalibration();
    saveCalibration();
    displayText(2, 43, 20, "Copy", true, true);
    delay(250);
    displayText(2, 35, 25, "Finish", true, true);
    delay(250);
    display.clearDisplay();
    beep(1, 50);
    goto START;
  }

  stopMotors();
  goto MENU;
}

void loop() {
  runMenu();
}
