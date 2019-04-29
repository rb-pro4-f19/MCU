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
// create new UART instance with BAUDRATE 9600
UART* uart1 = uart.new(BAUD_9600);

// construct UART frame
UART_FRAME frm =
{
	 0,
	 0,
	 (uint8_t[14]){0},
	 0
};

// construct transmission data array
uint8_t tx_data[4] = {120, 0x22, 0, 'f'};

// send some data of type 'MSG'
uart.send(uart1, UART_MSG, tx_data, 4);

// infinite loop of reading from UART
while(true)
{
	uart.read(uart1, &frm, true);
	for (int i = 0; i < 1000; ++i);
}
```

<!-- ----------------------------------------------------------------------------------------- -->

## API
Text.

<!-- ----------------------------------------------------------------------------------------- -->

## Changelog

### [Unreleased]

#### Known Issues
- Protocol only supports `31 bytes` of payload; should be `255 bytes`.

#### Added
- Add `.send_obj()` method.

#### Changed
- Update `enum``UART_FRAME_TYPE`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-04-20

#### Added
- Add `.stream()` method.
- Add TX and RX size defines, e.g. `MAX_TX_PAYLOAD_SIZE`.
- Add boundary restriction in `.send()`.
- Add `case` `BAUD_921600` to `_UART_set_baudrate()`; removed numbering in `enum`.

#### Changed
- Changed `payload_size` to `size_t`.
- Added `const` specifier to payload array pointer.
- Implemented size defines in various methods.
- Updated `API` + add link in `uart.h`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-03-15

#### Added
- Add `UART_BAUDRATE` `enum`.
- Add `_UART_set_baudrate()` method; used in `_UART_init`.

#### Changed
- Made `.new()` take `UART_BAUDRATE` as argument.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-03-05

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.4.0]: #changelog
[1.3.0]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
