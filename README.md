# devialet-ir

An ESP32 IR receiver to control the volume of Devialet speakers.

This project uses a IR Sensor to convert a Volume+ signal to the following http post request:

POST http://192.168.1.32/ipcontrol/v1/systems/current/sources/current/soundControl/volume
{"volume":50}

Allowing to control the volume of the Devialet from an Apple TV Remote.

For now I am always receiving the same IR codes despite using different buttons on the remote.
I thought it migh be faulty hardware so I bought another set of IR receiver, but no luck,
I am alwas receiving the same `0x9C897A65 (7 Bits)` IR code.

Here is the Serial Output:

```
---- Opened the serial port /dev/tty.usbmodem2101 ----
............................E (1485108) gptimer: gptimer_start(348): timer is not enabled yet
E (1485112) gptimer: gptimer_start(348): timer is not enabled yet
E (1485113) gptimer: gptimer_start(348): timer is not enabled yet
E (1485114) gptimer: gptimer_start(348): timer is not enabled yet
E (1485120) gptimer: gptimer_start(348): timer is not enabled yet
E (1485126) gptimer: gptimer_start(348): timer is not enabled yet
E (1485131) gptimer: gptimer_start(348): timer is not enabled yet
E (1485137) gptimer: gptimer_start(348): timer is not enabled yet
E (1485143) gptimer: gptimer_start(348): timer is not enabled yet
E (1485149) gptimer: gptimer_start(348): timer is not enabled yet
E (1485155) gptimer: gptimer_start(348): timer is not enabled yet
E (1485160) gptimer: gptimer_start(348): timer is not enabled yet
E (1485166) gptimer: gptimer_start(348): timer is not enabled yet
E (1485172) gptimer: gptimer_start(348): timer is not enabled yet

Protocol  : UNKNOWN
Code      : 0x9C897A65 (7 Bits)

Will apply volume command: 0
GET response: {"volume":0}
POST response: {}
...E (1486284) gptimer: gptimer_start(348): timer is not enabled yet
E (1486288) gptimer: gptimer_start(348): timer is not enabled yet
E (1486289) gptimer: gptimer_start(348): timer is not enabled yet
E (1486291) gptimer: gptimer_start(348): timer is not enabled yet
E (1486296) gptimer: gptimer_start(348): timer is not enabled yet
E (1486302) gptimer: gptimer_start(348): timer is not enabled yet
E (1486308) gptimer: gptimer_start(348): timer is not enabled yet
E (1486313) gptimer: gptimer_start(348): timer is not enabled yet
E (1486319) gptimer: gptimer_start(348): timer is not enabled yet
E (1486325) gptimer: gptimer_start(348): timer is not enabled yet
E (1486331) gptimer: gptimer_start(348): timer is not enabled yet
E (1486337) gptimer: gptimer_start(348): timer is not enabled yet
E (1486342) gptimer: gptimer_start(348): timer is not enabled yet
E (1486348) gptimer: gptimer_start(348): timer is not enabled yet

Protocol  : UNKNOWN
Code      : 0x9C897A65 (7 Bits)

Will apply volume command: 0
GET response: {"volume":0}
POST response: {}
.....E (1488472) gptimer: gptimer_start(348): timer is not enabled yet
E (1488477) gptimer: gptimer_start(348): timer is not enabled yet
.E (1488477) gptimer: gptimer_start(348): timer is not enabled yet
E (1488479) gptimer: gptimer_start(348): timer is not enabled yet
E (1488484) gptimer: gptimer_start(348): timer is not enabled yet
E (1488490) gptimer: gptimer_start(348): timer is not enabled yet
E (1488496) gptimer: gptimer_start(348): timer is not enabled yet
E (1488502) gptimer: gptimer_start(348): timer is not enabled yet
E (1488508) gptimer: gptimer_start(348): timer is not enabled yet
E (1488513) gptimer: gptimer_start(348): timer is not enabled yet
E (1488519) gptimer: gptimer_start(348): timer is not enabled yet
E (1488525) gptimer: gptimer_start(348): timer is not enabled yet
E (1488531) gptimer: gptimer_start(348): timer is not enabled yet
E (1488537) gptimer: gptimer_start(348): timer is not enabled yet

Protocol  : UNKNOWN
Code      : 0x9C897A65 (7 Bits)

Will apply volume command: 0
GET response: {"volume":0}
POST response: {}
.E (1489264) gptimer: gptimer_start(348): timer is not enabled yet
E (1489269) gptimer: gptimer_start(348): timer is not enabled yet
E (1489269) gptimer: gptimer_start(348): timer is not enabled yet
E (1489271) gptimer: gptimer_start(348): timer is not enabled yet
E (1489276) gptimer: gptimer_start(348): timer is not enabled yet
E (1489282) gptimer: gptimer_start(348): timer is not enabled yet
E (1489288) gptimer: gptimer_start(348): timer is not enabled yet
E (1489294) gptimer: gptimer_start(348): timer is not enabled yet
E (1489300) gptimer: gptimer_start(348): timer is not enabled yet
E (1489305) gptimer: gptimer_start(348): timer is not enabled yet
E (1489311) gptimer: gptimer_start(348): timer is not enabled yet
E (1489317) gptimer: gptimer_start(348): timer is not enabled yet
E (1489323) gptimer: gptimer_start(348): timer is not enabled yet
E (1489329) gptimer: gptimer_start(348): timer is not enabled yet

Protocol  : UNKNOWN
Code      : 0x9C897A65 (7 Bits)

Will apply volume command: 0
GET response: {"volume":0}
POST response: {}
..................
---- Closed the serial port /dev/tty.usbmodem2101 ----
```
