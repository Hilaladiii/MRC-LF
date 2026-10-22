#include "../sensor_map.h"

#include <cstdio>
#include <cstdint>

static int positionFromMask(uint8_t mask) {
  uint8_t sensorOnLine[8];
  for (int i = 0; i < 8; i++) {
    sensorOnLine[i] = (mask >> (7 - i)) & 1;
  }
  return linePositionFromBits(sensorOnLine);
}

int main() {
  struct Case {
    uint8_t mask;
    int expected;
  };

  // The 13 patterns the old switch handled, plus line loss and a cross line.
  const Case cases[] = {
      {0b10000000, 30},  {0b01000000, 25},  {0b01100000, 20},
      {0b01110000, 15},  {0b00110000, 10},  {0b00010000, 5},
      {0b00011000, 0},   {0b00001000, -5},  {0b00001100, -10},
      {0b00001110, -15}, {0b00000110, -20}, {0b00000010, -25},
      {0b00000001, -30}, {0b00000000, 0},   {0b11111111, 0},
  };

  int failures = 0;
  for (const Case &testCase : cases) {
    int got = positionFromMask(testCase.mask);
    if (got != testCase.expected) {
      std::printf("FAIL mask=%d expected=%d got=%d\n", testCase.mask,
                  testCase.expected, got);
      failures++;
    }
  }

  if (failures == 0) {
    std::printf("all %d cases pass\n",
                static_cast<int>(sizeof(cases) / sizeof(cases[0])));
  }
  return failures ? 1 : 0;
}
