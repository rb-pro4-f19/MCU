## Changelog

### [Unreleased]

#### Changed
- Expanded `set_pwm()`; now logs success or failure based on `spi.send()`.

<!-- #### Known Issues
- Item -->

<!-- #### Todo
- Item. -->

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
