#include <nspireio/nspireio.h>
#include <libndls.h>
#include <time.h>
#include "tools/timerHelpers.hpp"
#include "tools/rtcHelpers.hpp"

#define PMU_WAKEUP_REASON (*(volatile u_int32_t*) 0x90140000)
#define PMU_PERIPHERAL_DISABLE (*(volatile u_int32_t*) 0x90140050)

int main(void)
{
    scr_type_t lcdType = lcd_type();        // Should return SCR_320x240_565 for TI-nspire cx2 CAS (Screen 320x240 RGB565)
    static uint16_t framebuffer[320 * 240]; // Buffer the size of the screen in RAM
    lcd_init(lcdType);                      // Init with correct mode (given by lcd_type)
    
    uint16_t row[320]; // Create rows before use
    
    for (int x = 0; x < 320; x++) // Fill screen with gradient black to red
    {
        int t = (x * 255) / (319);
        row[x] = rgb565(t, 0, 0);
    }
    for (int y = 0; y < 80; y++)
    {
        memcpy(&framebuffer[y * 320], row, sizeof(row));
    }
    for (int x = 0; x < 320; x++) // Fill screen with gradient black to green
    {
        int t = (x * 255) / (319);
        row[x] = rgb565(0, t, 0);
    }
    for (int y = 80; y < 160; y++)
    {
        memcpy(&framebuffer[y * 320], row, sizeof(row));
    }
    for (int x = 0; x < 320; x++) // Fill screen with gradient black to blue
    {
        int t = (x * 255) / (319);
        row[x] = rgb565(0, 0, t);
    }
    for (int y = 160; y < 240; y++)
    {
        memcpy(&framebuffer[y * 320], row, sizeof(row));
    }
    lcd_blit(framebuffer, lcdType); // Send buffer to screen
    
    wait_key_pressed();
    wait_no_key_pressed();
    
    // PMU shows all 1s = all peripherals enabled, so peripheral clock is 12MHz (not throttled)
    // Peripheral clock speed (12MHz) / prescale (256) = 46,875 Hz tick rate
    // For 60 Hz: 46,875 / 60 ≈ 781 ticks
    uint32_t timerCycleTime = 781; // 46,875 Hz / 781 ≈ 60 Hz animation
    TIMER_SETUP(TIMER_A_SEL, true, true, true, PRESCALE_DIV256, true, false);
    TIMER_A_LOAD = timerCycleTime;
    while (!any_key_pressed())
    {
        if ((TIMER_A_INT_STATUS & 0x1) == 0x1)
        {
            for (int y = 0; y < 240; y++)
            {
                uint16_t *line = &framebuffer[y * 320];
                memmove(line, line + 1, (320 - 1) * sizeof(uint16_t));
                line[319] = line[0];
            }
            TIMER_A_INT_CLEAR = 1; // Write anything to clear
            lcd_blit(framebuffer, lcdType);
        }
        msleep(1);
    }
    wait_no_key_pressed();
    
    // Set up for 5 second intervals
    timerCycleTime = 234375; // (32.79kHz/256) * 5 seconds = 640 ticks
    TIMER_SETUP(TIMER_A_SEL, true, true, true, PRESCALE_DIV256, true, false);
    TIMER_A_LOAD = timerCycleTime;
    nio_console csl;
    nio_init(&csl, NIO_MAX_COLS, NIO_MAX_ROWS, 2, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
    
    // Print wakeup reason
    uint32_t wakeup_reason = PMU_WAKEUP_REASON;
    char binary_str1[36];
    for (int i = 0; i < 32; i++) {
        if (i > 0 && i % 8 == 0) {
            binary_str1[i + (i/8 - 1)] = ' ';
            binary_str1[i + i/8] = ((wakeup_reason >> (31 - i)) & 1) ? '1' : '0';
        } else {
            binary_str1[i + i/8] = ((wakeup_reason >> (31 - i)) & 1) ? '1' : '0';
        }
    }
    binary_str1[35] = '\0';
    nio_fprintf(&csl, "Wakeup reason: 0b%s\n", binary_str1);
    
    // Print peripheral disable status
    uint32_t pmu_status = PMU_PERIPHERAL_DISABLE;
    char binary_str[36]; // 32 bits + 3 spaces + null terminator
    for (int i = 0; i < 32; i++) {
        if (i > 0 && i % 8 == 0) {
            binary_str[i + (i/8 - 1)] = ' ';
            binary_str[i + i/8] = ((pmu_status >> (31 - i)) & 1) ? '1' : '0';
        } else {
            binary_str[i + i/8] = ((pmu_status >> (31 - i)) & 1) ? '1' : '0';
        }
    }
    binary_str[35] = '\0';
    nio_fprintf(&csl, "PMU status: 0b%s\n", binary_str);
    uint32_t oldEpoch = RTC_SECONDS;
    while (!any_key_pressed())
    {
        if ((TIMER_A_INT_STATUS & 0x1) == 0x1)
        {
            time_t epoch = RTC_SECONDS;
            time_t delta = (epoch - oldEpoch);
            oldEpoch = epoch;
            TIMER_A_INT_CLEAR = 1; // Write anything to clear
            struct tm *local_time = localtime(&epoch);
            char buf[64];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local_time);
            nio_fprintf(&csl, "Local time: %s, Delta: %lld\n", buf, delta);
        }
        msleep(1);
    }
    wait_no_key_pressed();
    
    TIMER_A_CONTROL = 0;
    lcd_init(SCR_TYPE_INVALID); // Allow os control of the screen (for popups ect. and program end)
    return 0;
}