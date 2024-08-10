# devialet-ir

An ESP32 IR receiver to control the volume of Devialet speakers.

This project uses a IR Sensor to convert the IR signals of a remote to the following http post request:

```
GET http://{DEVIALET_IP}/ipcontrol/v1/systems/current/sources/current/soundControl/volume
{"volume":47}
POST http://{DEVIALET_IP}/ipcontrol/v1/systems/current/sources/current/soundControl/volume
{"volume":50}
```

Allowing to control the volume of the Devialet from an Apple TV Remote.

Finding the correct setup took some research as the ESP32-C6 is pretty recent and not completely supported by the tools I was using.

Here is the [IR Receiver](https://www.amazon.fr/dp/B07ZYZDW28) used for the build.
