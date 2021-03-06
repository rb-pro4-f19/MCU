# MCU
The microcontroller unit is the brain of the [Pan-Tilt system][1] and facilitates a CLI interface to the system as-well as controls the attached hardware using a joystick, through the means of a closed-loop feedback mechanism. The system is implemented using a super-loop structure with class-like [modules](#modules) and a central [system process](#system-process).

This project uses the [`RB-C99 1.0`][2] coding conventions.

[1]: https://rb-pro4-f19.github.io/
[2]: https://github.com/rb-pro4-f19/rb-pro4-f19.github.io/blob/master/docs/RB-C99/rb_c99_1.0.md

---

#### Table of Contents

- [System Process](#system-process)
- [Modules](#modules)
- [Modes](#modes)

---

## System Process
The main super-loop process  consists of two primary stages; CLI communication followed by a FSM which in accordance with the selected [system mode](#modes), determines how to regulate the attached Pan-Tilt hardware.

![System Process](https://raw.githubusercontent.com/rb-pro4-f19/Overleaf/master/assets/img/mcu_architecture.jpg)

## Modules
The MCU has is comprised of various modules, each with a specific purpose.

| Name       | File         | Description                                                      |
|------------|--------------|------------------------------------------------------------------|
| DRIVER     | [`driver.h`] | MCU port access, interrupts, systick etc.                        |
| SYSTEM     | [`sys.h`]    | System control, modes, get/set methods.                          |
| ~~EXCHDLR~~| `exchdlr.h`  | Exception handling with `assert()` & `require()` methods.        |
| TIMEPOINT  | [`tp.h`]     | Time tracking, duration calculation etc.                         |
| CHECKSUM   | [`chksum.h`] | Static checksum method collection.                               |
| UART       | [`uart.h`]   | UART communication with telegram protocol.                       |
| SPI        | [`spi.h`]    | SPI communication with telegram protocol.                        |
| CLI        | [`cli.h`]    | Command interpretation, console logging.                         |
| MOTOR      | [`mot.h`]    | Motor interface with encoder, PWM etc.                           |
| PID        | [`pid.h`]    | PID computation class.                                           |

<!-- Links to the header files -->

[`sys.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/sys.h
[`driver.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/driver.h
[`tp.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/tp.h
[`chksum.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/chksum.h
[`uart.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/uart.h
[`spi.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/spi.h
[`cli.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/cli.h
[`mot.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/mot.h
[`pid.h`]: https://github.com/rb-pro4-f19/MCU/blob/master/modules/pid.h

## Modes
Text.
