Haptic driver: DRV2605L
Datasheet: https://www.ti.com/lit/ds/symlink/drv2605l.pdf

//////////////////////////////////////////////////////////////////////////////////////////

**** Specifications to try and match ****
Alarm Waveform:
Pattern 47 (Buzz 1 - 100%) --> 1x
Pattern 24 (Sharp Tick 1 - 100%) --> 1x
Delay

Loop: Infinity
Pattern type: normal
Duration: 30s

**** Notes ****
- Chat GPT Pattern 47 is not in the LRA library and may be weak ad its not optimized for to match their resonant behavior...
- ChatGPT recomends "Match the feel of 47 on LRA --> Try effect 14, 25–28 from Library 6 — they approximate buzz patterns in the ERM libraries"
- This inference is because TI only lists IDs 1-30 for library 6, even though they state that all 123 waveforms are supported.

//////////////////////////////////////////////////////////////////////////////////////////

Setting DRV2605L Wavfeorms:
The slot number determines the order in which the DRV2605L will play patterns. It starts at slot 0 and plays forward until it hits a slot with effectID = 0 (which means end).

| ID     | Function | Delay Duration | drv.setWaveform(<slot>, <effectID>) |
| ------ | -------- | -------------- | ----------------------------------- |
| `0xFC` | Delay    | 5 ms           | drv.setWaveform(<slot>, 0xFC);      |
| `0xFD` | Delay    | 10 ms          | drv.setWaveform(<slot>, 0xFD);      |
| `0xFE` | Delay    | 20 ms          | drv.setWaveform(<slot>, 0xFE);      |
| `0xFF` | Delay    | 40 ms          | drv.setWaveform(<slot>, 0xFF);      |

You can also just add a delay between triggers of the drv.go() in the main loop

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

- All waveform IDs 1–123 are used in every library
- ChatGPT has made some inference that olny waveforms 1-30 are valid in Library 6 (does not have evidence of this)
- Waveform libraries are tuned for certain motor types, but you can still use any motor with them.
- "The effects in each library were created to achieve the same feel, but the output will appear slightly from library to library
  to account for differences in motor characteristics like startup time, acceleration, and brake time"
  (Page 14, https://www.ti.com/lit/an/sloa189/sloa189.pdf)

//////////////////////////////////////////////////////////////////////////////////////////