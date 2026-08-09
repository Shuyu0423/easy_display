# CST816 target calibration test

`cst816_calibration_test.c` is a board-side calibration test function for flashing the STM32 target and checking CST816 touch coordinates.

## Keil usage

Enable the test switch in `driver_pack/user_conf.h`:

```c
#define TOUCH_CST816_CALIBRATION_TEST_ENABLE (1)
```

`demo/stm32_f4/example_pro/app/main.c` will include `cst816_calibration_test.h` and call `cst816_calibration_test()` before the normal LCD/command loop when the switch is enabled.

Required project sources:

- `tests/target/touch/cst816_calibration_test.c`
- `driver_pack/input/touch/core/src/touch_driver.c`
- `driver_pack/input/touch/controller/cst816/src/cst816.c`
- `demo/stm32_f4/example_pro/board/mcu_touch_iic.c`
- normal board/UART/tick startup sources already used by the demo project

Required include paths:

- `driver_pack/input/touch/core/inc`
- `driver_pack/input/touch/controller/cst816/inc`
- `demo/stm32_f4/example_pro/board`
- `demo/stm32_f4/example_pro/bsp/uart`
- `tests/target/touch`

## Test flow

Open SEGGER RTT Viewer or RTT Client before/after flashing. The test prints through RTT terminal `0`, channel `0`.

The test asks you to hold four points:

1. `TOP_LEFT`
2. `TOP_RIGHT`
3. `BOTTOM_RIGHT`
4. `BOTTOM_LEFT`

Each point collects `TOUCH_CAL_STABLE_SAMPLES` samples, prints the average, then waits for release.

After the four points are captured it prints:

- each corner average coordinate
- raw coordinate range
- `swap_xy`
- `mirror_x`
- `mirror_y`
- mapping min/max values

Then it enters live monitor mode and prints every touch event with both raw coordinates and calibrated screen coordinates.

## Optional overrides

The test defaults to a `240 x 280` panel. You can override these from compiler defines:

```c
TOUCH_CAL_PANEL_WIDTH
TOUCH_CAL_PANEL_HEIGHT
TOUCH_CAL_RAW_WIDTH
TOUCH_CAL_RAW_HEIGHT
TOUCH_CAL_STABLE_SAMPLES
TOUCH_CAL_POLL_DELAY_MS
```
