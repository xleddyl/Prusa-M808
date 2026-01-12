/**
 * M808 Repeat Marker - Based on Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Adapted for Prusa MK3 firmware
 */

#ifndef REPEAT_H
#define REPEAT_H

#include <stdint.h>

#define MAX_REPEAT_NESTING 10

typedef struct {
  uint32_t sdpos;   // SD card byte position
  int32_t counter;  // -1 = infinite, 0 = done, >0 = remaining loops
} repeat_marker_t;

class Repeat {
private:
  static repeat_marker_t marker[MAX_REPEAT_NESTING];
  static uint8_t index;  // Current nesting depth

public:
  static void reset() { index = 0; }
  static bool is_active();
  static bool is_command_M808(const char* cmd);
  static void early_parse_M808(const char* cmd, uint32_t sdpos);
  static void add_marker(uint32_t sdpos, uint32_t count);
  static void loop();
  static void cancel();
};

extern Repeat repeat;

#endif // REPEAT_H
