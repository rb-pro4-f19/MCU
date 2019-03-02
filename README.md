# MCU
The microcontroller unit is the brain of the Pan-Tilt system and facilitates a CLI interface to the system as-well as controls the attached hardware using a Joystick, through the means of a closed-loop feedback mechanism.

#### Table of Contents
- [System Architecture](#system-architecture)
  * [Modules](#modules)
  * [Communication Protocols](#communication-protocols)
    + [UART](#uart)
    + [SPI](#spi)
  * [System Process](#system-process)
- [Coding Conventions](#coding-conventions)

---

# System Architecture
The MCU connects to a CLI using serial connection, enabling users to interact with the system. Control of the Pan-Tilt system is achieved using a Joystick connected to the MCU, where data-transfer including motor PWM and encoder data is facilitated by parallel connections to an FPGA using multiple SPI modules.

![System Arhictecture](https://i.imgur.com/tqUFwMv.jpg)

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

## Communication Protocols

### UART
Serial communication is performed at `9600 baud` using a total of 10-bits, with no parity check. A telegram is comprised of `3 bytes`, following the frame format as shown below.

### SPI
Text.

## System Process
The main process consists of two primary stages; CLI communication followed by a FSM which determines who to control the attached hardware.

![System Process](https://i.imgur.com/iiY7sYH.jpg)

---

# Coding Conventions
This project use the following coding conventions for `C99`:

- [Coding Conventions](#coding-conventions)
  * [GitHub](#github)
  * [Modules](#modules-1)
      - [Module Header](#module-header)
      - [Sections](#sections)
      - [Function Header](#function-header)
      - [Includes](#includes)
      - [Module Interface](#module-interface)
  * [Formatting](#formatting)
      - [Language](#language)
      - [Indentation](#indentation)
      - [Syntax](#syntax)
      - [Cases](#cases)

## GitHub
Commits should be staged by relevant grouping of files, where commits should as shortly as possible explain the changes; otherwise a more detailed description is encouraged.

![GitHub Commit Example](https://i.imgur.com/IAO0n1E.gif)

## Modules
Each module, defined as a set of `.h` and `.c` files, must follow the predefined structure, as demonstrated in [`template.h`](https://github.com/martinandrovich/emp-blinker/blob/master/src/template_h.txt) and [`template.c`](https://github.com/martinandrovich/emp-blinker/blob/master/src/template_c.txt).

#### Module Header
Both files of a module must be described using a module header at the beginning of the document, following the format as defined in the template files.

#### Sections
Each module must be comrpised of the predefined sections, even if unused. If necessary, sections may be added.

#### Function Header
All function definitions must be desrbibed using a header (after function name and before curly braces), following the format as defined in the template file. This only affects `.c` files, since header files should never include any function declarations.

#### Includes
Each header-file of a module should include the `#pragma once` include guard. It is illegal to include `.c` files directly.

#### Module Interface
A module (or class) must bear the same name as the `class struct` of that module, although the `class TYPE` should be more descriptive. Non-static modules must have a contructor named `new()` and a destructor named `del()`.

Struct modules should be comprised of:
- Struct Declarations (forward declaration)
- Struct Definition (`struct TYPE`)
- Struct Class (`extern const struct TYPE_CLASS`) with function pointers to public methods.

An example of a `DEMO` module:

**Forward Declarations:**
```C
typedef struct  DEMO DEMO;
typedef enum    DEMO_TYPE DEMO_TYPE;
```

**Struct Definition in `demo.h`:**
```C
struct DEMO
{
	int_u8  	some_variable;
	DEMO_TYPE	type : 1;
};
```

**Struct Class in `demo.h`:**
```C
extern const struct DEMO_CLASS
{
	// Constructor & Destructor
	DEMO*		(*new)(DEMO_TYPE type);
	void 		(*del)(DEMO* this);

	// Methods
	void		(*some_method)(DEMO* this);
} demo;
```

**Struct Class in `demo.c`:**
```C
const struct DEMO_CLASS demo =
{
	// Constructor & Destructor
	.new		= &DEMO_new,
	.del		= &DEMO_del,

	// Methods
	.some_method= &DEMO_some_method
} demo;
```

## Formatting

#### Language
All code, including comments, should be written in _American-English_. Inline code comments are always lowercase, where descriptive text in e.g. headers is sentence case.

#### Indentation
For identation, tabs of size 4 should be used, indenting each block as defined per a set of curly braces; these must always start on a new line. In some cases, indentation may be used with paranthetes, as demonstrated below:

Combination of declarations should _preferably_ be aligned using tabs.

#### Syntax
Per default, code should be written using the following conventions:

| Case                      | Convention                                      |
|---------------------------|-------------------------------------------------|
| Keywords                  | `struct`, `enum`, `while`  ...                  |
| Types*                    | `int_u8` ...                                    |
| Constants                 | `SOME_CONSTANT`                                 |
| Methods                   | `void some_method(int var);`                    |
| Assembly Methods          | `__some_ASM_method(int var);`                   |
| Variables                 | `int some_variable;`                            |
| Pointers (Variables)      | `int* some_variable`                            |
| Struct Type               | `struct EXAMPLE_TYPE{...};`                     |
| Struct Class              | `const struct EXAMPLE_TYPE_CLASS {...} extype;` |
| Struct Variable (private) | `static int _variable`                          |
| Struct Method (public)    | `static void EXAMPLE_TYPE_method(int var)`      |
| Struct Method (private)   | `static void _EXAMPLE_TYPE_method(int var)`     |

\* the predefined types from `typedefs.h` should be used.

**Furthermore**:

- Single prefixed underscores are reserved for private methods and variables of structs.
- Double prefixed underscores are reserved for ASSEMBLY methods.
- Numberical values should be defined as constants with meaningful names.
- Variables that are desired to be left unchange must be marked `const`.
- Variables that are susceptable unexpected changed must be marked `volatile`.
- Methods that take no arguments must be explicitly specified, e.g. `void some_method(void);`.
- The keyword `this` is reserved for method taking a pointer to the object as an argument.
- Bitfields are encouraged in structs if applicable.
- Arithmetic operations are always to be separated by spaces, e.g. `c = a * (b / 4);`.

#### Cases
In all cases, the use of (early) `return`, `continue` and `break` **is allowed**; altbeit `GOTO` is disallowed.

##### While

```C
while(condition)
{
	...
}
```

##### Do While

```C
do
{
	some_method();
} while (condition);
```

##### For
For loops should be indexed using `i`, `j`, `k` etc., unless otherwise specified or required. Execution conditions should preferably be specified as constants.

```C
for(int i = 0; i < SOME_VALUE_MAX; i++)
{
	some_method(i);
}
```

##### If
If statements are to be used when less than 3 expressions are to be tested. Nesting of if statements is highly discouraged.

```C
if(condition)
{
	some_method();
}
else if (condition)
{
	return;
}
else
{
	some_other_method();
}
```

##### Switch
If more than 3 conditions are to be sequentially tested, a switch statement is required; these should be written with plentiful of spacing. A `default` case is always required and no break scenarios must be highlighted using comments.

```C
switch (select_expression)
{
	case CASE_0:

		statement_0_0();
		...
		statement_0_n();
		break;

	case CASE_1:

		statement_1_0();
		...
		statement_1_n();

		/* no break */

	case CASE_M:

		statement_m_0();
		...
		statement_m_n();
		break;

	default:

		statement_default();
};
```
