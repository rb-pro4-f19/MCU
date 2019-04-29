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
This system class is a standard PID controller. First it will be an PI.

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
- Add PID defines; e.g. `KP	= this->Kp`.
- Add saturation limits to `PID` class.
- Add clamp variable to `PID` class.
- Add `PID_CLAMP` to `enum` `PID_PARAM`.

#### Changed
- Update `.operate()`; semantic clean up.
- Update `.new()`; fixed `malloc()` bug.

#### Fixed
-

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-04-11

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[1.0.0]: #changelog
