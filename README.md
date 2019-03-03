# MCU
The microcontroller unit is the brain of the [Pan-Tilt system][2] and facilitates a CLI interface to the system as-well as controls the attached hardware using a joystick, through the means of a closed-loop feedback mechanism.

This project uses the [`RB-C99-1`][1] coding conventions.

[1]: https://github.com/rb-pro4-f19/rb-pro4-f19.github.io/blob/master/docs/STANDARDS.md
[2]: https://google.com

---

#### Table of Contents
- [System Architecture](#system-architecture)
- [Modules](#modules)
- [System Process](#system-process)
- [Communication Protocols](#communication-protocols)
	- [UART](#uart)
	- [SPI](#spi)
- [Coding Conventions][1]

---

## System Architecture
The MCU connects to a CLI using serial connection, enabling users to interact with the system. Control of the Pan-Tilt system is achieved using a joystick connected to the MCU, where communication (motor PWM and encoder data) is facilitated by parallel connections to an FPGA with multiple SPI modules.

![System Arhictecture](https://i.imgur.com/tqUFwMv.jpg)

The system is implemented using a super-loop structure, using various [modules](#modules) and a central [system process](#system-process).

## Modules
The MCU has is comprised of various modules, each with a specific purpose.

| Name       | File        | Description                                                      |
|------------|-------------|------------------------------------------------------------------|
| SYS/DRIVER | `driver.h`  | MCU port access, interrupts, systick etc.                        |
| FPGA       | `fpga.h`    | 32-bit FPGA control line.                                        |
| EXCHDLR    | `exchdlr.h` | Exception handling with `assert()` & `require()` methods.        |
| TIMEPOINT  | `tp.h`      | Time tracking, duration calculation etc.                         |
| UART       | `uart.h`    | UART communication with telegram protocol.                       |
| SPI        | `spi.h`     | SPI communication with telegram protocol.                        |
| CLI        | `cli.h`     | Command interpretation, console logging.                         |
| CONTROLLER | `ctrl.h`    | Controller data struct with previous data, desired position etc. |
| JOYSTICK   | `joystk.h`  | Joystick driver with kinematic mapping.                          |
| MOTOR      | `dcmotor.h` | DC Motor interface with encoder, PWM etc.                        |
| PID        | `pid.h`     | PID computation class.                                           |

## System Process
The main super-loop process  consists of two primary stages; CLI communication followed by a FSM which in accordance with the selected [system mode](#modes), determines how to regulate the attached Pan-Tilt hardware.

![System Process](https://i.imgur.com/iiY7sYH.jpg)

## Modes
Text.

## Communication Protocols

### UART
Serial communication is performed at `9600 baud` with `10-bit` (total) frame-size, with no parity check. A telegram is comprised of `3 bytes`, following the frame format as shown below.

[Image of UART Frame]

The system is interfaced using [these commands](https://google.com).

### SPI
Text.
