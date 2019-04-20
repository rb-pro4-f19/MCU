# HAL Module
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

<!-- ----------------------------------------------------------------------------------------- -->

## API
The `.read()` method takes a `HAL*` instance as argument returns a `HAL_VAL`; either a
read => {0 or 1} or `HAL_FAULT`, meaning that SPI transmission failed.

<!-- ----------------------------------------------------------------------------------------- -->

## Changelog

### [Unreleased]

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-04-20

#### Added
- Add `val` variable to struct; initialized to `HAL_LOW`.

#### Changed
- Updated changelog to match default format.
- `hal.read()` was flipped; now reads sensor as active low.
- Updated `HAL_VAL` `enum` to explicitly show values.
- Made `.read()` update `this->val` and return that.
- Update API link in `hal.h` + API description in `HAL.md`.

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
