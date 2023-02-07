# Arduino Watering Interface

This Arduino sketch is used to water a vegetable patch. It is designed to interface with another program over a serial connection. It will not water the garden on its own.

I use it to water a vegetable patch with a [Goland application](https://github.com/mewejo/go-watering).

## Features

* Send moisture readings on demand
* Send water outlet states on demand
* Set water outlet states on demand
* Automatically shut off water outlets after a period of time, a failsafe for broken communications
* Heartbeat signal
* Per sensor configurable moisture maps

## Commands

```
a # All water outlets off
b # All water outlets on
c # Outlet #1 on
d # Outlet #1 off
e # Outlet #2 on
f # Outlet #2 off
g # Outlet #3 on
h # Outlet #3 off
i # Outlet #4 on
j # Outlet #4 off
k # Request sensor readings
l # Request water outlet states
```

## Responses

### Moisture sensors

_Example/fake data_

```bash
READINGS_START
MS:1:700:44 # MS:ID:RAW:PERCENTAGE
MS:2:333:22
MS:3:333:22
MS:4:333:22
MS:5:333:22
MS:6:333:22
READINGS_END
```
### Water outlet states

Note: `REAL_STATE` and `SET_STATE` should always match. `REAL_STATE` is the current `digitalRead()` of the pin, and `SET_STATE` is what the Arduino should have set the pin to.

```bash
OUTLETS_START
WO:1:0:0 # WO:ID:REAL_STATE:SET_STATE (1 = on, 0 = off)
WO:2:1:1
WO:3:0:0
WO:4:0:0
OUTLETS_END
```

### Heartbeat

Heartbeats  are sent on a timer, if you do not receive these after a period of time, assume the connection has died.

Be sure to read from the serial buffer, else this will fill up and stop new data being received by your program.

```
HEARTBEAT
HEARTBEAT
HEARTBEAT
HEARTBEAT
```

## Dependencies

* https://github.com/contrem/arduino-timer

