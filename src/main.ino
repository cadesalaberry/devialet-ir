#define DECODE_LG 1
#define DECODE_NEC 1         // Includes Apple and Onkyo

#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <secrets.hpp>
#include <devialet_api.hpp>


// Define the IR receiver pin
// https://github.com/espressif/arduino-esp32/blob/master/variants/XIAO_ESP32C6/pins_arduino.h
// Make sure we use the correct pin mapping for the ESP32-C6
const uint16_t kRecvPin = 18; // D10
const char* ssid = SECRET_WIFI_NAME;          // Replace with your network SSID
const char* password = SECRET_WIFI_PASSWORD;  // Replace with your network password

// IR Receiver setup
IRrecv irrecv(kRecvPin);
decode_results results;

int last_command = VOLUME_UNKNOWN;

/**
 * Uses the IRremoteESP8266 library to determine if we should volume up, down or mute.
 */
int getVolumeCommand(decode_results* results) {
  const decode_type_t type = results->decode_type;
  const int value = results->value;

  if (type == decode_type_t::LG) {
    if (value == 0x20DF40BF) {
      return VOLUME_UP;
    }
    else if (value == 0x20DFC03F) {
      return VOLUME_DOWN;
    }
    else if (value == 0x20DF906F) {
      return VOLUME_MUTE;
    }
  }

  // My TV remote seems to be using NEC protocol, but key codes are different
  if (value == 0x20DF40BF) {
    return VOLUME_UP;
  } else if (value == 0x20DFC03F) {
    return VOLUME_DOWN;
  } else if (value == 0x20DF906F) {
    return VOLUME_MUTE;
  } else if (value == 0x71B93203) {
    // Sometimes the mute button triggers a 34 bit code
    return VOLUME_MUTE;
  } else if (value == 0x8EEF4B83) {
    // Sometimes the volume down button triggers a 34 bit code
    return VOLUME_DOWN;
  } else if (value == 0x9E0A10FF) {
    // Sometimes the volume up button triggers a 34 bit code
    return VOLUME_UP;
  }

  return VOLUME_UNKNOWN;
}

void setup() {
  Serial.begin(115200);

  Serial.print("[setup] Starting IR receiver... ");
  // Initialize IR Receiver
  irrecv.enableIRIn();
  Serial.println("OK");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to WiFi
  Serial.print("[setup] Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("@");
  }

  Serial.println("");
  Serial.print("[setup] WiFi connected: ");
  Serial.println(WiFi.localIP());
  // turn off the LED. commands seems to be inverted
  digitalWrite(LED_BUILTIN, HIGH);
}


void loop() {

  // Check if an IR signal is received
  if (!irrecv.decode(&results)) {
    // Zone out for a bit if no signal has been received
    Serial.print(".");
    delay(300);
    return;
  }

  Serial.println("");
  Serial.println(resultToHumanReadableBasic(&results));

  int command = getVolumeCommand(&results);
  bool shouldRepeatLastCommand = results.repeat;

  irrecv.resume();  // Receive the next value

  if (shouldRepeatLastCommand) {
    Serial.println("[main] Repeating last command...");
    command = last_command;
  }

  if (command == VOLUME_UNKNOWN) {
    Serial.println("[main] Ignoring unknown command");
   return;
  }

  controlDevialetVolume(command);
  last_command = command;
}