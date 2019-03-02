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
The MCU has is comprised of various modules, each with a specific purpose.

| Name       	| File        	| Description                                                      	|
|------------	|-------------	|------------------------------------------------------------------	|
| SYS/DRIVER 	| `driver.h`  	| MCU port access, interrupts, systick etc.                        	|
| FPGA       	| `fpga.h`    	| 32-bit FPGA control line.                                        	|
| EXCHDLR    	| `exchdlr.h` 	| Exception handling with `assert()` & `require()` methods.        	|
| TIMEPOINT  	| `tp.h`      	| Time tracking, duration calculation etc.                         	|
| UART       	| `uart.h`    	| UART communication with telegram protocol.                       	|
| SPI        	| `spi.h`     	| SPI communcation with telegram protocol.                         	|
| CLI        	| `cli.h`     	| Command interpretation, console logging.                         	|
| CONTROLLER 	| `ctrl.h`    	| Controller data struct with previous data, desired position etc. 	|
| JOYSTICK   	| `joystk.h`  	| Joystick driver with kinematic mapping.                          	|
| MOTOR      	| `dcmotor.h` 	| DC Motor interface with encoder, PWM etc.                        	|
| PID        	| `pid.h`     	| PID computation class.                                           	|

## System Process
A little information about the process would be nice.

```
SYSTEM PROCESS
|
├── Check CLI commands
|   ├── Read UART
|   ├── Interpret command
|   └── Update STATE & FPGA_CTRL
|
├── Controller (STATE)
|   ├── MODE_CAL
|   |   └── ...
|   ├── MODE_RUN
|   |   ├── Read from joystick
|   |   ├── Update desired position (ref)
|   |   ├── Read from encoders
|   |   ├── Compute feedback (error)
|   |   ├── Compute control variable
|   |   ├── Compute output (PWM)
|   |   ├── Write to motors [dcmot.set_pwm(M1, val);]
|   |   ├── Save data for next iteration
|   |   └── Log to CLI (optional)
|   ├── MODE_RST
|   └── ...
|
├── System Timepoint (ISR)*
└── Exception Handler*
```

# Coding Conventions
Uff, this is going to be a tough one.
