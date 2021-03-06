# CLI Module
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
A demonstration of the CLI class can be seen below:

```cpp
// WIP
```

<!-- ----------------------------------------------------------------------------------------- -->

## API
Text.

<!-- ----------------------------------------------------------------------------------------- -->

## Changelog

### [Unreleased]

#### Known Issues
- All `.log()` methods fail when used with payload > 32 chars; UART protocol wrongly defined.

#### Added
- Add `UART_DO` commands to `CMD_ID` `enum`.

#### Changed
- Changed `GET_ECHO` to `DO_ECHO`.

<!-- #### Todo
- Item. -->

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-04-20

#### Added
- Add `CLI_MSG_MAXLEN` define.
- Add `allow_msg` control variable to `cli`.
- **`[1.2.1] - 2019-04-25`** Add `CMD_ID` `enum` for `CLI_ACTION` `id`.

#### Changed
- Changed `.log()` & `.logf()` to `msg()` & `.msgf()`.
- Removed size checks; now handled by UART.
- **`[1.2.1] - 2019-04-25`** Changed `CLI_MAX_TYPES` to 4 and `CLI_MAX_ACTIONS` to 12.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-03-23

#### Added
- `CLI_LAMBDA` define for use in `CMD_TABLE`.
- `.logf()` with support of custom formatting; like `printf()`.

#### Changed
- Callback type changed to `void (*)(const char* args)`.
- Payload pointer is padded before passed to callback in `cli.parse_frame()`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-03-17

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
