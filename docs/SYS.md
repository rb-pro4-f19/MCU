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

#### Added
- Add `.set_pwm()` method.
- Add `.set_freq()` method.
- Add `.get_enc()` method.
- Add `.get_hal()` method.
- Add `_SYSTEM_MODE_calibration()` method + defines.

#### Changed
- Overhaul to calibration method; mostly semantic changes.
- Prefixed `SYS_MODE` enum members with `SYS_`.
- Changed SYSTICK duration constant to microseconds (us) unit + implemented new `sys_tick_init()` method.
- Changed default SYSTICK duration to `500 us`.

<!-- #### Known Issues -->


<!-- #### Todo
- Item. -->

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-04-11

#### Added
- Added Calibration to SYS class.
- Added calibration enum.
- Added `abs()` ternary macro.

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-04-11

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
