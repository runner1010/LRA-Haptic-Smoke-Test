LRA Haptic Smoke Test: Method to test the feel of a variety of haptic motors with a specified alarm waveform

There are multiple firmware versions contained in the src directory.
Use build_src_filter in platformio.ini file to select which firmware is built.

//////////////////////////////////////////////////////////////////////////////////////////

Haptic driver: DRV2605L
Datasheet: https://www.ti.com/lit/ds/symlink/drv2605l.pdf

//////////////////////////////////////////////////////////////////////////////////////////

Alarm Waveform (**** Specifications to try and match ****):
Pattern 47 (Buzz 1 - 100%) --> 1x
Pattern 24 (Sharp Tick 1 - 100%) --> 1x
Delay

Timing:
Pattern 47 & Pattern 24 take 500ms
Delay is 250ms
Total: 750ms

Loop: Infinity
Pattern type: normal
Duration: 30s

//////////////////////////////////////////////////////////////////////////////////////////

Setting DRV2605L Waveforms:
The slot number determines the order in which the DRV2605L will play patterns. It starts at slot 0 and plays forward until it hits a slot with effectID = 0 (which means end).

| ID     | Function | Delay Duration | drv.setWaveform(<slot>, <effectID>) |
| ------ | -------- | -------------- | ----------------------------------- |
| `0xFC` | Delay    | 5 ms           | drv.setWaveform(<slot>, 0xFC);      |
| `0xFD` | Delay    | 10 ms          | drv.setWaveform(<slot>, 0xFD);      |
| `0xFE` | Delay    | 20 ms          | drv.setWaveform(<slot>, 0xFE);      |
| `0xFF` | Delay    | 40 ms          | drv.setWaveform(<slot>, 0xFF);      |

You can also just add a delay between triggers of the drv.go() in the main loop.
The function drv.go() is non blocking and  imediately returns so total loop delay can work.

//////////////////////////////////////////////////////////////////////////////////////////

DRV2605L Internal Libraries
| Library # | Motor Type | Description / Use Case                                                             |
| --------- | ---------- | ---------------------------------------------------------------------------------- |
| 1         | ERM        | Default ERM library — general-purpose effects like clicks, buzzes, pulses          |
| 2         | ERM        | Stronger effects (higher intensity); useful for larger ERMs or stronger response   |
| 3         | ERM        | Effects optimized for longer durations (ramp-up/ramp-down)                         |
| 4         | ERM        | Shorter effects; very crisp, good for short alerts or ticks                        |
| 5         | ERM        | Continuation of 4, with enhanced multi-pulse effects                               |
| 6         | LRA        | Only library specifically optimized for LRA resonance, phase, and drive timing     |

//////////////////////////////////////////////////////////////////////////////////////////