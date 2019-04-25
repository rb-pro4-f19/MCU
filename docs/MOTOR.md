# MOTOR Module
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
A demonstration of the MOTOR class can be seen below:

```cpp
// WIP
```

<!-- ----------------------------------------------------------------------------------------- -->

## API
Text.

<!-- ----------------------------------------------------------------------------------------- -->

## Changelog

### [Unreleased]

<!-- #### Known Issues -->

<!-- #### Todo
- Item. -->

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.3.0] - 2019-04-25

#### Added
- Add multiple encoder boundary variables to `MOTOR` `struct`.
- Add encoder boundary check in `.operate()`.
- Add encoder rollover in `.get_enc()`.
- Add `speed` variable + `tp_speed` to `MOTOR` `struct`.
- Add `_MOTOR_calc_speed()` method.

#### Fixed
- Encoded was not adding to stored value but saving the delta value in `.get_enc()`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.2] - 2019-03-23

#### Added
- Slewrate protection in `.operate()` + `.set_pwm()`; controlled by `bool slew`.

#### Changed
- `pwm_data` added to `.set_pwm()` for compatability with FPGA.
- Removed logging from `.set_pwm()`, `.set_freq()` and `.get_enc()`.
- Made `.set_pwm()` save actual (signed) pwm value, instead of the FPGA formatted one.
- Made `.set_pwm()` and `.set_freq()` return type `bool` for external use.
-  **`[1.2.1] - 2019-04-20`** Made `.get_enc()` store the enc value in struct.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-03-23

#### Added
- Added `tp_watchdog` to object struct + timeout constant for FPGA watchdog feeding.
- `.set_freq()` for adjusting FPGA PWM frequency.
- `.operate()` for continuous operation from main loop.
- `.feed()` for feeding the FPGA watchdog.

#### Changed
- Made `spi_module` part of class struct + `assert()` in object constructor.
- Payload pointer is padded before passed to callback in `cli.parse_frame()`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-04-02

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
