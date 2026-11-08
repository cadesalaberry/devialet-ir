# devialet-ir

An ESP32 IR receiver to control the volume of Devialet speakers.

<p align="center">
  <img src="./docs/devialet-phantom-ii.gif"></img>
</p>

This project uses an IR sensor to turn the volume keys of a remote into [Devialet IP Control](./Devialet%20IP%20Control%20-%20R1.pdf) requests, and exposes every key press to Home Assistant:

```
GET http://{DEVIALET_IP}/ipcontrol/v1/systems/current/sources/current/soundControl/volume
{"volume":47}
POST http://{DEVIALET_IP}/ipcontrol/v1/systems/current/sources/current/soundControl/volume
{"volume":50}
```

Allowing to control the volume of a Phantom Speaker from an Apple TV Remote.

Finding the correct setup took some research as the ESP32-C6 is pretty recent and not completely supported by the tools I was using.

## Prerequisites

Here is what devices were used for the project:

- An Apple TV 4K (model=AppleTV11,1;iOS=17.6)
- An Apple Siri Remote (Charging via lightning)
- A LG TV (Model 42LD420)
- A Devialet Phantom II 98dB Speaker ([Link to the website](https://www.devialet.com/fr-fr/phantom-speaker/phantom-ii/phantom-ii-98db-white-stereo/))
- A Seeed Studio XIAO ESP32C6 Board ([Link to the official product](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/), [The website on which I bought it](https://www.gotronic.fr/art-carte-xiao-esp32c6-38917.htm))
- [IR Receiver](https://www.amazon.fr/dp/B07ZYZDW28) used for the build.

<p align="center">
  <img src="./docs/KY-022.jpg" width="50%"></img>
</p>

## Firmware

The firmware is an [ESPHome](https://esphome.io) configuration: [`esphome/devialet-ir.yaml`](./esphome/devialet-ir.yaml).
The original Arduino/PlatformIO firmware (`src/`, `platformio.ini`) is kept for reference but is no longer maintained.

### Wiring

Connect the KY-022 IR receiver to the board: the **S** (signal) pin to **D10**, the middle pin to **3V3**, and the **-** pin to **GND**.

![image of the esp32-c6 board](./docs/xiao-esp32c6.png)

### Flashing

```sh
pip install esphome
cd esphome
cp secrets.example.yaml secrets.yaml   # fill in Wi-Fi and keys (openssl rand -base64 32)
esphome run devialet-ir.yaml           # first time over USB-C, then over Wi-Fi (OTA)
```

Set `speaker_address` in the `substitutions:` block to the IP of your Phantom (reserve its DHCP lease), or change it later from Home Assistant.

> If the board refuses to be flashed: unplug it, hold the tiny **B** button next to the USB-C connector, and plug it back in while holding.

If the device can't join your Wi-Fi it opens a `Devialet-IR Fallback` hotspot where you can enter new credentials.

## Home Assistant

The device is discovered automatically (Settings → Devices & services → ESPHome). It exposes:

| Entity | Purpose |
| --- | --- |
| `event.devialet_ir_ir_command` | Fires `volume_up`, `volume_down` or `mute` on each key press; use it to trigger any automation |
| `switch.devialet_ir_direct_speaker_control` | ON: the ESP drives the speaker itself (still works when HA is down). OFF: only the event fires |
| `text.devialet_ir_speaker_address` | IP/host of the Phantom used for direct control |
| `sensor.devialet_ir_last_ir_code` | Last decoded NEC code, handy to map another remote |

Also add the built-in **Devialet** integration in Home Assistant to get the Phantom as a media player.
To let Home Assistant handle the keys instead of the ESP, turn the direct control switch off and use
[`esphome/home-assistant-automation.example.yaml`](./esphome/home-assistant-automation.example.yaml).

### Using another remote

Set `dump: all` under `remote_receiver:`, press the keys, read the codes in `esphome logs devialet-ir.yaml`
(or the *Last IR code* sensor), and update the `switch (x.command)` block in `on_nec`.

## LED

The yellow LED is ESPHome's status LED: it blinks slowly on warnings (e.g. Wi-Fi or Home Assistant not connected) and fast on errors, and stays off when everything is fine.
