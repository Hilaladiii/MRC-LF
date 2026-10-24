#pragma once

#include <stdint.h>

constexpr int SENSOR_COUNT = 8;
constexpr int MIN_CONTRAST = 30;

inline int clampInt(int value, int low, int high) {
  if (value < low) {
    return low;
  }
  if (value > high) {
    return high;
  }
  return value;
}

// 0..1000 line strength. Sensors with too little contrast report 0.
inline int normalizedStrength(int raw, int minReading, int maxReading) {
  if (maxReading - minReading < MIN_CONTRAST) {
    return 0;
  }
  return clampInt((raw - minReading) * 1000 / (maxReading - minReading), 0, 1000);
}

// Sensor 0 is the +30 side, sensor 7 is the -30 side.
// Returns -30..+30 and writes the sum of all strengths through total.
inline int centroidPosition(const uint16_t strength[SENSOR_COUNT], long *total) {
  long weighted = 0;
  *total = 0;
  for (int i = 0; i < SENSOR_COUNT; i++) {
    weighted += (long)i * strength[i];
    *total += strength[i];
  }
  if (*total == 0) {
    return 0;
  }
  long center = weighted * 1000 / *total;
  return (int)((3500 - center) * 30 / 3500);
}

inline int applyDeadband(int pwm, int deadband) {
  if (pwm > 0 && pwm < deadband) {
    return deadband;
  }
  if (pwm < 0 && pwm > -deadband) {
    return -deadband;
  }
  return pwm;
}

inline int lowPassStep(int previous, int current, int alphaPercent) {
  return (alphaPercent * current + (100 - alphaPercent) * previous) / 100;
}

// Fase celah pada garis putus-putus.
enum GapPhase { GAP_COAST, GAP_SEARCH, GAP_LOST };

// Hysteresis: ambil garis di enterStrength, pertahankan sampai releaseStrength.
inline bool lineSeen(int strength, bool wasVisible, int enterStrength,
                     int releaseStrength) {
  return wasVisible ? strength >= releaseStrength : strength >= enterStrength;
}

inline GapPhase gapPhase(unsigned long hiddenMs, unsigned long coastMs,
                         unsigned long searchMs) {
  if (hiddenMs < coastMs) {
    return GAP_COAST;
  }
  if (hiddenMs < coastMs + searchMs) {
    return GAP_SEARCH;
  }
  return GAP_LOST;
}

// Arah sapuan bergantian +1/-1 setiap sweepMs setelah fase coast.
inline int sweepDirection(unsigned long hiddenMs, unsigned long coastMs,
                          unsigned long sweepMs) {
  if (hiddenMs < coastMs) {
    return 1;
  }
  unsigned long index = (hiddenMs - coastMs) / sweepMs;
  return (index % 2 == 0) ? 1 : -1;
}
