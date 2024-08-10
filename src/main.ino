#define DECODE_LG 1
#define DECODE_NEC 1         // Includes Apple and Onkyo

#include <WiFi.h>
#include <ESPmDNS.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#include <secrets.hpp>
#include <devialet_api.hpp>


// Define the IR receiver pin
// https://github.com/espressif/arduino-esp32/blob/master/variants/XIAO_ESP32C6/pins_arduino.h
// Make sure we use the correct pin mapping for the ESP32-C6

const uint16_t kRecvPin = D10; // D10
const char* ssid = SECRET_WIFI_NAME;          // Replace with your network SSID
const char* password = SECRET_WIFI_PASSWORD;  // Replace with your network password

// IR Receiver setup
IRrecv irrecv(kRecvPin);
decode_results results;
String devialetIP = "";
wl_status_t wifiStatus;

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

  for (int i = 0; i < 30 && wifiStatus != WL_CONNECTED; i++) {
    wifiStatus = WiFi.status(); 
    delay(500);
    Serial.print("@");
  }

  if (wifiStatus != WL_CONNECTED) {
    Serial.println("");
    Serial.println("[setup] Could not connect to WiFi");
    return;
  }

  Serial.println("");
  Serial.print("[setup] WiFi connected: ");
  Serial.println(WiFi.localIP());
  // turn off the LED. commands seems to be inverted
  digitalWrite(LED_BUILTIN, HIGH);

  // Initialize mDNS with the device name "Devialet IR"
  MDNS.begin("Devialet IR");

  // Tries 10 times to get the IP of the devialet
  for (u16_t i = 0; i < 10 && devialetIP == ""; i++)
  {
    Serial.println("[devialet_api] Try " + String(i) + "/" + String(10));
    digitalWrite(LED_BUILTIN, LOW);
    devialetIP = getDevialetIP();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }

  if (devialetIP == "") {
    Serial.println("[devialet_api] Could not find the Devialet IP");
  } else {
    Serial.println("[devialet_api] Found Devialet at " + devialetIP);
  }
}

void loop() {
  if (wifiStatus != WL_CONNECTED) {
    // Flash if WiFi is not connected (100ms OFF / 100ms ON)
    Serial.println("[loop] WiFi not connected " + String(wifiStatus));
    digitalWrite(LED_BUILTIN, millis() % 200 > 100);
    // wait for a delay short enough to not be visible in th blinking pattern
    // but long enough to not overload the CPU
    delay(10);
    return;
  }
  if (devialetIP == "") {
    // Flash if the Devialet IP is not found (400ms OFF / 100ms ON)
    digitalWrite(LED_BUILTIN, millis() % 500 > 100);
    // wait for a delay short enough to not be visible in th blinking pattern
    // but long enough to not overload the CPU
    delay(10);
    return;
  }

  // Check if an IR signal is received
  if (!irrecv.decode(&results)) {
    // Zone out for a bit if no signal has been received
    Serial.print(".");
    delay(100);
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

  controlDevialetVolume(devialetIP, command);
  last_command = command;
}