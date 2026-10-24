#include "../control_math.h"

#include <cstdint>
#include <cstdio>

static int failures = 0;

static void expectInt(const char *name, int expected, int actual) {
  if (expected != actual) {
    std::printf("FAIL %s expected=%d got=%d\n", name, expected, actual);
    failures++;
  }
}

int main() {
  expectInt("strength midpoint", 333, normalizedStrength(100, 50, 200));
  expectInt("strength at min", 0, normalizedStrength(50, 50, 200));
  expectInt("strength at max", 1000, normalizedStrength(200, 50, 200));
  expectInt("strength below min", 0, normalizedStrength(0, 50, 200));
  expectInt("strength above max", 1000, normalizedStrength(250, 50, 200));
  expectInt("strength low contrast", 0, normalizedStrength(15, 10, 20));

  uint16_t strength[SENSOR_COUNT] = {0};
  long total = 0;

  strength[0] = 1000;
  expectInt("centroid sensor 0", 30, centroidPosition(strength, &total));
  expectInt("centroid sensor 0 total", 1000, (int)total);
  strength[0] = 0;

  strength[7] = 1000;
  expectInt("centroid sensor 7", -30, centroidPosition(strength, &total));
  strength[7] = 0;

  strength[0] = 1000;
  strength[7] = 1000;
  expectInt("centroid symmetric edges", 0, centroidPosition(strength, &total));
  expectInt("centroid symmetric total", 2000, (int)total);
  strength[0] = 0;
  strength[7] = 0;

  strength[3] = 500;
  strength[4] = 500;
  expectInt("centroid middle pair", 0, centroidPosition(strength, &total));
  strength[3] = 0;
  strength[4] = 0;

  strength[3] = 1000;
  strength[4] = 500;
  expectInt("centroid asymmetric pair", 1, centroidPosition(strength, &total));
  strength[3] = 0;
  strength[4] = 0;

  expectInt("centroid empty position", 0, centroidPosition(strength, &total));
  expectInt("centroid empty total", 0, (int)total);

  expectInt("deadband zero", 0, applyDeadband(0, 40));
  expectInt("deadband small positive", 40, applyDeadband(10, 40));
  expectInt("deadband small negative", -40, applyDeadband(-10, 40));
  expectInt("deadband above", 100, applyDeadband(100, 40));
  expectInt("deadband below", -100, applyDeadband(-100, 40));

  expectInt("lowpass rise", 30, lowPassStep(0, 100, 30));
  expectInt("lowpass fall", 70, lowPassStep(100, 0, 30));
  expectInt("lowpass negative", -30, lowPassStep(0, -100, 30));

  expectInt("line seen at enter", 1, lineSeen(400, false, 400, 300));
  expectInt("line hidden below enter", 0, lineSeen(399, false, 400, 300));
  expectInt("line held by hysteresis", 1, lineSeen(300, true, 400, 300));
  expectInt("line released below hysteresis", 0, lineSeen(299, true, 400, 300));

  expectInt("gap coast start", 0, gapPhase(0, 250, 1500));
  expectInt("gap coast before limit", 0, gapPhase(249, 250, 1500));
  expectInt("gap search at limit", 1, gapPhase(250, 250, 1500));
  expectInt("gap search before lost", 1, gapPhase(1749, 250, 1500));
  expectInt("gap lost at limit", 2, gapPhase(1750, 250, 1500));

  expectInt("sweep starts right", 1, sweepDirection(250, 250, 180));
  expectInt("sweep flips left", -1, sweepDirection(430, 250, 180));
  expectInt("sweep flips back right", 1, sweepDirection(610, 250, 180));
  expectInt("sweep before coast", 1, sweepDirection(100, 250, 180));

  if (failures == 0) {
    std::printf("all control math cases pass\n");
  }
  return failures ? 1 : 0;
}
