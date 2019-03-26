# SPI Module
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
A demonstration of the SPI class can be seen below:

```cpp
// init SPI instance @ 8MHz (
// bps = SYSCLK / (CPSDVSR * (1 + SCR))
SPI* spi_test = spi.new(2);

// send value '110' to PWM1 (SPI_ADDR)
spi.send(spi_test, PWM1, 110);

// send value '110' to PWM1 and validate
if(!spi.send(spi_test, PWM1, 110))
{
	puts("Error!");
}

// request data from ENC1
uint16_t enc1_dat = 0;
spi.request(spi_test, ENC1, enc1_dat);
```

<!-- ----------------------------------------------------------------------------------------- -->

## API
Text.

<!-- ----------------------------------------------------------------------------------------- -->

## Changelog

### [Unreleased]

<!-- #### Known Issues
- Item -->

<!-- #### Todo
- Item. -->

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-03-26

#### Added
- `.flush()` method for flushing FIFO buffer.
- `#defines` for pins (`PD0` etc.); should be moved to `driver.h` later.

#### Changed
- Reverted multiple cases in `_SPI_init()`; switched to SSI3 module.
- Minor optimizations and semantic updates.

#### Fixed
- spinlock in `_SPI_transmit()` was not blocking; changed to poll on `BSY` flag.
- FIFO was not flushed after transmission; popped element was always `0x00`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-03-15

#### Added
- `FRAME_DATA(x)` define to extract "header + payload" data.

#### Changed
- Implemented `CHECKSUM 1.2.X` module.
- Implemented `FRAME_DATA(x)` macro

#### Fixed
- Incorrect data passed into checksum generator & validator.

#### Hotfixes
- **`[1.1.1] - 2019-03-23`** Checking wrong flag when recieving data in `_SPI_recieve()`.
- **`[1.1.2] - 2019-03-25`** Optimized algorithm in `_SPI_recieve()`.
- **`[1.1.2] - 2019-03-25`** Checking wrong checksum in `_SPI_send()` and `_SPI_request()`.
- **`[1.1.3] - 2019-03-26`** Added SPI3 module `_SPI_init()`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-03-05

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
