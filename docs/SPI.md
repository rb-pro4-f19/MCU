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

### [1.1.0] - 2019-03-15

#### Added
- `FRAME_DATA(x)` define to extract "header + payload" data

#### Changed
- Implemented `CHECKSUM 1.2.X` module.
- Implemented `FRAME_DATA(x)` macro

#### Fixed
- Incorrect data passed into checksum generator & validator.

#### Hotfixes
- **`[1.1.1] - 2019-03-23`** Checking wrong flag when recieving data in `_SPI_recieve()`.

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