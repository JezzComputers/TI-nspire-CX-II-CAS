# TI-Nspire CX II CAS Testing & Development

Hardware and timing tests for the TI-Nspire CX II CAS calculator.

## Hardware Information

| Property | Value |
|----------|-------|
| **Model** | N-0820AK |
| **Full Name** | TI-Nspire CX II CAS |
| **Display** | 320×240 RGB565 |
| **Architecture** | ARM |

## Project Structure

```
NspireTests/
├── Makefile              # Build configuration
├── README.md            # This file
├── build/               # Build output directory
│   ├── bin/            # Final .tns executable
│   ├── elf/            # ELF intermediate files
│   └── obj/            # Object files
└── src/                # Source code
    ├── main.cpp        # Main test program
    └── tools/          # Helper utilities
        ├── rtcHelpers.hpp      # RTC (Real-Time Clock) helpers
        ├── timerHelpers.cpp    # Timer configuration utilities
        └── timerHelpers.hpp    # Timer configuration headers
```

## Building

Requires the [Ndless SDK](https://ndless.me/) to be installed.

```bash
make clean && make
```

The compiled program will be output to `build/bin/displayTests.tns`.

## PMU (Power Management Unit) Analysis

The TI-Nspire CX II uses an "Aladdin PMU" unit at base address `0x90140000`.

### PMU Registers

| Address | Access | Description |
|---------|--------|-------------|
| `0x90140000` | R/? | Wakeup reason - indicates why the device woke from low-power mode |
| `0x90140050` | R/W | Peripheral disable register - controls bus access to peripherals |

### Peripheral Disable Bits (0x90140050)

| Bit | Peripheral | Address |
|-----|------------|---------|
| 9 | Triple DES encryption | 0xC8010000 |
| 10 | SHA-256 hash generator | 0xCC000000 |
| 13 | Watchdog timer | 0x90060000 |
| 26 | I2C controller | 0x90050000 |

**Note:** A bit value of `1` means the peripheral is **enabled**, `0` means **disabled**.

## Peripheral Clock Speed Observations

The peripheral clock speed varies based on PMU configuration. These observations were made on model N-0820AK:

### Clock Speed Table

| Wakeup Reason (0x90140000) | PMU Status (0x90140050) | Observed Clock Speed | Notes |
|----------------------------|------------------------|---------------------|-------|
| `0b00000000 00000000 00000000 00000000` | `0b11111111 11111111 11111111 11111111` | **12 MHz** | Full speed, no throttling |
| `0b10000000 00000100 00000100 00000000` | `0b11111111 11111111 11111111 11111111` | **12 MHz** | Still full speed despite wakeup bits |
| `0b00000000 00000100 00000100 00000000` | `0b11111111 11111111 11111111 11111111` | **12 MHz** | Still full speed despite wakeup bits |
| Unknown | Some bits = 0 | **46.6 kHz** | Moderate throttling |
| Unknown | Unknown | **32.79 kHz** | Heavy throttling (~12MHz/366) |

### Timer Calculations

Effective timer tick rate after applying the prescaler.

**Formula:** `timer_tick_rate = peripheral_clock / prescaler`

Converts tick rate into the number of timer ticks that occur during one 60 Hz frame interval.

**Formula:** `ticks = timer_tick_rate / 60`

With a prescaler of 256 (`PRESCALE_DIV256`), the timer tick rates are:

| Peripheral Clock | Timer Tick Rate | Ticks for 60 Hz | Expected Frame Time |
|------------------|-----------------|-----------------|---------------------|
| **12 MHz** | 46,875 Hz | 781 | 5.33s (320px @ 60fps) |
| **46.6 kHz** | 182 Hz | 3 | 5.33s (320px @ 60fps) |
| **32.79 kHz** | 128 Hz | 2 | 5.33s (320px @ 60fps) |

With a prescaler of 16 (`PRESCALE_DIV16`), the timer tick rates are:

| Peripheral Clock | Timer Tick Rate | Ticks for 60 Hz | Expected Frame Time |
|------------------|-----------------|-----------------|---------------------|
| **12 MHz** | 750 kHz | 12500 | 5.33s (320px @ 60fps) |
| **46.6 kHz** | 2,913 Hz | 49 | 5.33s (320px @ 60fps) |
| **32.79 kHz** | 2,049 Hz | 34 | 5.33s (320px @ 60fps) |

With a prescaler of 1 (`PRESCALE_DIV1`), the timer tick rates are:

| Peripheral Clock | Timer Tick Rate | Ticks for 60 Hz | Expected Frame Time |
|------------------|-----------------|-----------------|---------------------|
| **12 MHz** | 12 MHz | 200,000 | 5.33s (320px @ 60fps) |
| **46.6 kHz** | 46.6 kHz | 777 | 5.33s (320px @ 60fps) |
| **32.79 kHz** | 32.79 kHz | 547 | 5.33s (320px @ 60fps) |

## Test Programs

### Gradient Animation Test

The main program creates a tri-color gradient (red, green, blue) and scrolls it horizontally at 60 Hz:

- **Red gradient**: Rows 0-79
- **Green gradient**: Rows 80-159
- **Blue gradient**: Rows 160-239

Each timer interrupt shifts the entire framebuffer left by 1 pixel, wrapping around. At 60 Hz, a complete 320-pixel cycle should take **5.33 seconds**.

### Timer & RTC Test

After the animation, the program displays:
- PMU wakeup reason (binary)
- PMU peripheral status (binary)
- Current time with delta between updates

This helps verify timer accuracy and clock configuration.

## Known Issues & Observations

1. **msleep() Bottleneck**: The `msleep(1)` call in the animation loop may cap the effective frame rate at ~91 Hz due to scheduler granularity.

2. **PMU Throttling**: The peripheral clock speed is not consistently documented. The PMU status register provides the most reliable indicator of current clock speed.

3. **Timer Accuracy**: With `TIMER_A_LOAD = 781` at 12 MHz, the actual rate is 60.09 Hz (46,875 ÷ 781), close enough for visual applications.

## Development Notes

### Timer Setup Example

```cpp
// For 60 Hz animation at 12 MHz peripheral clock
uint32_t timerCycleTime = 781;
TIMER_SETUP(TIMER_A_SEL, true, true, true, PRESCALE_DIV256, true, false);
TIMER_A_LOAD = timerCycleTime;

// Check for timer interrupt
if ((TIMER_A_INT_STATUS & 0x1) == 0x1) {
    // Do work
    TIMER_A_INT_CLEAR = 1;  // Clear interrupt
}
```

### Reading PMU Registers

```cpp
#define PMU_WAKEUP_REASON (*(volatile uint32_t*) 0x90140000)
#define PMU_PERIPHERAL_DISABLE (*(volatile uint32_t*) 0x90140050)

uint32_t wakeup = PMU_WAKEUP_REASON;
uint32_t status = PMU_PERIPHERAL_DISABLE;
```

## Resources

- [Ndless Documentation](https://ndless.me/)
- [Hackspire Wiki](https://hackspire.org/)
- [TI-Nspire Hardware Information](https://hackspire.org/index.php/Hardware)

## Development Conventions

This project follows [Conventional Commits](https://www.conventionalcommits.org/) for commit messages to maintain clear project history.

## License

This is a testing/research project. Use at your own risk.

---

*Last updated: November 18, 2025*
