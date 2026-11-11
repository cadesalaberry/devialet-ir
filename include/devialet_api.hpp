// DEPRECATED: the Arduino/PlatformIO firmware is no longer maintained.
// The project now uses ESPHome (esphome/devialet-ir.yaml). See README.md and README.arduino.md.

#include <ESPmDNS.h>

#define VOLUME_UP 1
#define VOLUME_DOWN 2
#define VOLUME_MUTE 3
#define VOLUME_UNKNOWN 4

void controlDevialetVolume(String devialetIP, int command);
String getDevialetIP();
