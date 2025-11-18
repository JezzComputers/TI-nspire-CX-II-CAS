#include <nspireio/nspireio.h>
#include <libndls.h>
#include "tools/rtcHelpers.hpp"
#include "rayTracer.hpp"

void runRaytracer()
{
    scr_type_t lcdType = lcd_type();        // Should return SCR_320x240_565 for TI-nspire cx2 CAS (Screen 320x240 RGB565)
    static uint16_t framebuffer[320 * 240]; // Buffer the size of the screen in RAM
    lcd_init(lcdType);

    // Initialize entire framebuffer to black
    memset(framebuffer, 0, sizeof(framebuffer));

    uint16_t row[320];
    for (int x = 106; x < 212; x++)
    {
        row[x] = rgb565(125, 125, 0);
    }
    for (int y = 80; y < 160; y++)
    {
        memcpy(&framebuffer[y * 320], row, sizeof(row));
    }

    lcd_blit(framebuffer, lcdType); // Send buffer to screen

    wait_key_pressed();
    wait_no_key_pressed();

    lcd_init(SCR_TYPE_INVALID); // Allow os control of the screen (for popups ect. and program end)
}
