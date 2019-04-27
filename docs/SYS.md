# SYSTEM MODULE
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque congue feugiat ipsum vitae bibendum. Etiam ornare ligula sit amet dolor dignissim porttitor. Aliquam mattis, lorem et pharetra aliquam, sapien tellus tempor orci, malesuada aliquet ipsum felis et lacus.

<!-- ----------------------------------------------------------------------------------------- -->

---

#### Table of Contents

- [Usage](#usage)
- [API](#api)
- [Changelog](#changelog)

---

<!-- ----------------------------------------------------------------------------------------- -->

## Usage
The `SYSTEM` class is to be called via CLI commands, such that a command table is populated with the appropriate commands. System operation is achieved by calling the `.operate()` method from a super-loop. A demonstration of the SYSTEM class can be seen below:

```cpp
// WIP
```

<!-- ----------------------------------------------------------------------------------------- -->

## API
Text.

<!-- ----------------------------------------------------------------------------------------- -->

## Changelog

### [Unreleased]
- Nothing to see here, keep moving.

#### Added
- Add PID defines, e.g. `PID0_KP`.
- Initialize PID instance + variables; `pid0` and `pid1`.
- Add to PID operation to `SYS_TUNING`.

#### Changed
- Remove define of `enum` `PID_PARAM`.
- Changed `.set_pos()` method.
- Disabled bounds in calibration mode.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.4.0] - 2019-04-25

#### Added
- Add redundancy check defines.
- Add boundary defines.
- Add redundancy check in `_SYSTEM_MODE_calibration()` for `CAL_PAN_SEEK_BOUNDARY`.
- Initialize motor boundary settings in `.init()`.
- Included calibration check in `.operate()` for `SYS_OPERATION` state.
- ~~Add temporary define of `enum` `PID_PARAM`.~~
- Add `.set_pos()` method.
- Add `.set_slew()` method.
- Add `.set_bound()` method.
- Add `.set_pid()` method; still needs work.
- Add link to API document in `sys.h`.


#### Changed
- Changed calibration defines; updated values.
- Updated `CAL_TILT_FINETUNE` state.
- Renamed `is_cal` to `cal_done` for improved clarity; same with `is_init`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.3.0] - 2019-04-18

#### Added
- Add `GUI_DATA` + `MOT_DATA` constructs.
- Add `to_gui` control variable to `sys` struct.
- Add `gui_data` variable to `sys` struct.
- Add `op_time` variable to `sys` struct + `tp.lmeasure()` in `SYS_OPERATION` case.
- Add `tp_gui` + `tp_tst` timepoints to `sys` struct.
- Add `_SYSTEM_fill_gui()` method.
- Add ~~`_SYSTEM_to_gui()`~~ + `_SYSTEM_to_gui_bg()` method.
- Add `.set_gui()` method.
- Add `.set_msg()` method.
-

#### Changed
- Renamed `.state` to `.mode` in `sys` struct.
- Changed `UART` module to run at `BAUD_921600`.
- Changed all `.logf()` to `.msgf()`.
- Changed `.echo()` message to `"Hello world!"`.

#### Fixed
- Calibration method never exited last state; added transition to `CAL_FINISH`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-04-18

#### Added
- Add `.set_pwm()` method.
- Add `.set_freq()` method.
- Add `.get_enc()` method.
- Add `.get_hal()` method.
- Add `.set_mode()` method.
- Add `_SYSTEM_MODE_calibration()` method + state enum/defines.

#### Changed
- Overhaul to calibration method; mostly semantic changes.
- Prefixed `SYS_MODE` enum members with `SYS_`.
- Changed SYSTICK duration constant to microseconds (us) unit + implemented new `sys_tick_init()` method.
- Changed default SYSTICK duration to `500 us`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-04-11

#### Added
- Add calibration methods to `SYS` class.
- Add calibration state `enum`.
- ~~Add `abs()` ternary macro~~.


---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-04-11

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.5.0]: #changelog
[1.4.0]: #changelog
[1.3.0]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
