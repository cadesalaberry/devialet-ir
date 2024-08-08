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

bool getVolumeCommand(decode_results* results) {
  // Add your logic to check if the received signal is the "volume up" signal
  // This will depend on your specific remote and IR signal pattern
  switch (results->value) {
    case (0x9C897A65):
      return 0; // MUTE
    case (0x94AFAA8C):
    case (0xE49E3262):
      return 1; // VOLUME UP
    case (0xC1BFD92):
      return -1; // VOLUME DOWN
    default:
      return NULL;
  }
}

void setup() {
  Serial.begin(115200);

  Serial.print("Starting IR receiver... ");
  // Initialize IR Receiver
  irrecv.enableIRIn();
  Serial.print("OK");
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  // Check if an IR signal is received
  if (irrecv.decode(&results)) {
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)
    Serial.println("");
    Serial.println(resultToHumanReadableBasic(&results));
    irrecv.resume();  // Receive the next value
    int command = getVolumeCommand(&results);
    Serial.print("Will apply volume command: ");
    Serial.println(command);
    controlDevialetVolume(command);
    delay(300);       // keep the LED on for some time
  }
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED off by making the voltage LOW
  Serial.print(".");
  delay(300);
}