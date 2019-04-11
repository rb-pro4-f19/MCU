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

#### Changed
- `pwm_data` added to `.set_pwm()` for compatability with FPGA.
- Removed logging from `.set_pwm()`.
- Made `.set_pwm()` save actual (signed) pwm value, instead of the FPGA formatted one.
- Made `.set_pwm()` and `.set_freq()` return type `bool` for external use.

<!-- #### Known Issues -->


<!-- #### Todo
- Item. -->

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
