## Changelog

<!-- ----------------------------------------------------------------------------------------- -->

### [Unreleased]

#### Known Issues
- UART protocol max payload size wrongly defined; maximum of `256 bytes` instead of `32 bytes`.
- All `.log()` methods fail when used with payload > 32 chars.

#### Added
- Add `.get_hal()` method + `CMD_TABLE` entry.
- Add `.set_mode()` method + `CMD_TABLE` entry.
-
#### Changed
- Implemented new methods from SYS module into `CMD_TABLE`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.4.0] - 2019-04-11

#### Added
- Implemented `sys.h` module; all changed will be logged here.

#### Changed
- Moved initialization code to `sys.init()`.
- Moved system operation code to `sys.operate()`.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.3.0] - 2019-03-23

#### Added
- `mot.h` module + initialization + updated `CMD_TABLE` accordingly

#### Changed
- ~~Expanded `set_pwm()`; now logs success or failure based on `spi.send()`.~~
- Replaced `set_pwm()` with `mot.set_pwm(MOTOR*)`

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-03-23

#### Added
- Updated main structure; added comments and divisors.
- Callbacks for CLI & FPGA testing.
	- `echo();`
	- `set_pwm();`
	- `get_enc();`

#### Changed
- `CMD_TABLE` updated to use new macros and callbacks.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2017-03-05

#### Added
- CLI command table.
- SysTick ISR.
- Barebone functionality.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2019-03-05

#### Added
- Created module.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
