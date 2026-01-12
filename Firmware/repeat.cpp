/**
 * M808 Repeat Marker - Based on Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Adapted for Prusa MK3 firmware
 */

#include "repeat.h"
#include "cardreader.h"

#ifdef ENABLE_GCODE_REPEAT_MARKERS

// Static member initialization
repeat_marker_t Repeat::marker[MAX_REPEAT_NESTING];
uint8_t Repeat::index = 0;

/**
 * Check if any repeat markers are currently active
 */
bool Repeat::is_active() {
  for (uint8_t i = 0; i < index; i++) {
    if (marker[i].counter != 0) return true;
  }
  return false;
}

/**
 * Check if a command string is M808 (but not M8080, M8081, etc.)
 */
bool Repeat::is_command_M808(const char* cmd) {
  return cmd[0] == 'M'
      && cmd[1] == '8'
      && cmd[2] == '0'
      && cmd[3] == '8'
      && !(cmd[4] >= '0' && cmd[4] <= '9');
}

/**
 * Add a new repeat marker at the given SD position
 * @param sdpos  SD card byte position (start of next line after M808 L)
 * @param count  Number of total passes (0 = infinite, N = N passes)
 */
void Repeat::add_marker(uint32_t sdpos, uint32_t count) {
  if (index >= MAX_REPEAT_NESTING) {
    // Too many nested markers - silently ignore
    return;
  }

  marker[index].sdpos = sdpos;
  // count=0 means infinite loop (-1)
  // count=N means N-1 more loops after first pass
  marker[index].counter = count ? (int32_t)(count - 1) : -1;
  index++;
}

/**
 * Process a loop point (M808 without L parameter)
 * Decrements counter and seeks back to marker if loops remain
 */
void Repeat::loop() {
  if (!index) {
    // No marker set - silently ignore
    return;
  }

  const uint8_t ind = index - 1;  // Active marker's index

  if (marker[ind].counter == 0) {
    // Counter exhausted - pop marker and continue
    index--;
  }
  else {
    // Loop back to the marker position
    card.setIndex(marker[ind].sdpos);

    // Decrement counter (ignore if negative/infinite)
    if (marker[ind].counter > 0) {
      marker[ind].counter--;
    }
  }
}

/**
 * Cancel all active repeat markers (M808 K from host)
 */
void Repeat::cancel() {
  for (uint8_t i = 0; i < index; i++) {
    marker[i].counter = 0;
  }
}

/**
 * Early parse M808 commands during SD command buffering
 * Called from get_command() in cmdqueue.cpp
 * @param cmd    Command string (null-terminated)
 * @param sdpos  Current SD position (after this command)
 */
void Repeat::early_parse_M808(const char* cmd, uint32_t sdpos) {
  if (!is_command_M808(cmd)) {
    return;
  }

  // Look for 'L' parameter
  const char* p = cmd + 4;  // Skip "M808"

  // Skip spaces
  while (*p == ' ') p++;

  if (*p == 'L' || *p == 'l') {
    // M808 L[count] - Set marker
    p++;  // Skip 'L'
    uint32_t count = (*p) ? strtoul(p, nullptr, 10) : 0;
    add_marker(sdpos, count);
  }
  else if (*p == 'K' || *p == 'k') {
    // M808 K - Cancel (handled in process_commands, but catch here too)
    cancel();
  }
  else if (*p == '\0' || *p == '\n' || *p == '\r' || *p == ';') {
    // M808 without parameters - Loop back
    loop();
  }
  // Ignore any other characters after M808
}

#endif // ENABLE_GCODE_REPEAT_MARKERS
