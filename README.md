# MCU
What the hell is this?

#### Table of Contents
- [System Architecture](#system-architecture)
  * [Modules](#modules)
  * [System Process](#system-process)
- [Coding Conventions](#coding-conventions)

---

# System Architecture
Description of architecture text here please.

[img_placeholder]

## Modules

```
MODULES
|
├── SYS/DRIVER  (driver.h)
├── TIMEPOINT   (tp.h)
├── EXCHDLR     (exchdlr.h)
├── UART        (uart.h)
├── SPI         (spi.h)
├── LOG         (log.h)
|
├── JOYSTICK    (joystk.h)
├── MOTOR       (dcmot.h)
└── PID         (pid.h)
```

## System Process

```
SYSTEM PROCESS
|
├── Check CLI commands
|   ├── Read UART
|   ├── Interpret command
|   └── Change STATE if necessary
|
├── Controller (STATE)
|   ├── MODE_CAL
|   |   └── ...
|   ├── MODE_RUN
|   |   ├── Read JOYSTICK
|   |   ├── Update desired position [dcmot.set_pos(x, y);]
|   |   ├── Read E1 & E2 [dcmot.read_enc(E1);]
|   |   ├── Compute feedback control (error)
|   |   ├── Compute control variable
|   |   ├── Write M1 & M2 [dcmot.set_pwm(M1, val);]
|   |   ├── Write CLI (optional)
|   |   └── Save data for next iteration
|   ├── MODE_RST
|   └── ...
|
├── System Timepoint (ISR)*
└── Exception Handler*
```

# Coding Conventions
Uff, this is going to be a tough one.
