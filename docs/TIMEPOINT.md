# TIMEPOINT Module
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque congue feugiat ipsum vitae bibendum. Etiam ornare ligula sit amet dolor dignissim porttitor. Aliquam mattis, lorem et pharetra aliquam, sapien tellus tempor orci, malesuada aliquet ipsum felis et lacus.

---

#### Table of Contents

- [Usage](#usage)
- [API](#api)
- [Changelog](#changelog)

---

## Usage
A demonstration of the TIMEPOINT class can be seen below (slightly outdated):

```cpp

// initialize systick to 200us per systick
// maximum 65000us = 65ms
tp.init_systick(200, us);

// call this only from the SYSTICK ISR
tp.systick();

// init test TIMEPOINT instances
TIMEPOINT* tp_test1 = tp.new();
TIMEPOINT* tp_test2 = tp.new();
TIMEPOINT* tp_test3 = tp.new();

// manually read value of a TIMEPOINT
// DONT'T WRITE TO IT!
uint16_t tp_test1_minutes = tp_test1->time_array[m];

// set tp_test1 to current (global) time (value of SYSTICK)
tp.set(tp_test1, tp.now());

// set tp_test2 to 0us:10ms:2s:0m:0h
tp.set(tp_test2, (uint16_t[]){ 0, 10, 2, 0, 0 });

// copy value of tp_test2 into tp_test3
tp.copy(tp_test3, tp_test2);

// get absolute difference between tp_test1 and tp_test3 in ms
int64_t diff1 = tp.delta(tp_test1, tp_test3, ms);

// get absolute difference between current time and tp_test2 in ms
int64_t diff2 = tp.delta_now(tp_test2, ms);

```

## API
Text.

## Changelog

### [Unreleased]

#### Known Issues
- Maximum `systick_dur_us` is ~65 μs, with no error checking on init.

<!-- #### Todo
- Item. -->

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.2.0] - 2019-03-20

#### Added
- `.convert_us()` method.
- `.reset()` method (memset).
- Support for minutes and hours; reset after 24 hours.
- Proper documentation

#### Changed
- Internal operations now based on `μs` with all functions updated accordingly.
- `time_array` is now `uint16_t[]` instead of `uint64_t[]`.
- Made `.inc()` inline, with support for minutes and hours.
- `.new()` now uses `memset`.
- Expand `enum TP_UNIT`

#### Fixed
- No indexing error after 17 minutes; added automatic reset.

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.1.0] - 2019-03-14

#### Added
- `.delta_now()` method.
- `.now()` method.
- `.init_systick()` method for SysTick duration initialization.
- `.systick()` method for external ISR.

#### Changed
- Replaced types with `<stdint.h>`
- `tp_system` is now static global in class.
- Integrated interrupt checks (removed asserts).
- `.set()` now takes pointer to `time_array`

#### Removed
- `.assert_irq()` not needed anymore, automatically done by `.now()`.
- Removed callback.
- `TP_TYPE` (all timepoint instances are now NORMAL).

---

<!-- ----------------------------------------------------------------------------------------- -->

### [1.0.0] - 2017-03-05

#### Added
- Created module.
- Barebone functionality.

<!-- ----------------------------------------------------------------------------------------- -->

[Unreleased]: #changelog
[1.2.0]: #changelog
[1.1.0]: #changelog
[1.0.0]: #changelog
