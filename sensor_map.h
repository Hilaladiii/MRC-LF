#pragma once

#include <stdint.h>

// Returns the line position from -30 (far left) to +30 (far right).
// The result is the average of the per-sensor weights of every lit sensor,
// so it also covers patterns the old hand-written switch did not list.
inline int linePositionFromBits(const uint8_t sensorOnLine[8]) {
  static const int8_t weight[8] = {30, 25, 15, 5, -5, -15, -25, -30};

  int sum = 0;
  int count = 0;
  for (int i = 0; i < 8; i++) {
    if (sensorOnLine[i]) {
      sum += weight[i];
      count++;
    }
  }
  return count ? sum / count : 0;
}
