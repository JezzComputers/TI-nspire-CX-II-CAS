#include <nspireio/nspireio.h>
#include <libndls.h>
#include "tools/debugTools.hpp"
#include "rayTracer.hpp"

int main(void)
{
    nio_console csl;
    nio_init(&csl, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
    
    nio_fprintf(&csl, "=== Main Menu ===\n\nESC. Exit without running\n1. Run Debug Tools\n2. Run Raytracer\n\nPress relevent key to select...\n");
    
    // Wait for user input
    bool selected = false;
    while (!selected)
    {
        wait_key_pressed();
        
        nio_fprintf(&csl, "Key pressed: ");
        if (isKeyPressed(KEY_NSPIRE_1))
        {
            nio_fprintf(&csl, "1\n");
            wait_no_key_pressed();
            nio_free(&csl);
            runDebugTools();
            selected = true;
        }
        else if (isKeyPressed(KEY_NSPIRE_2))
        {
            nio_fprintf(&csl, "2\n");
            wait_no_key_pressed();
            nio_free(&csl);
            runRaytracer();
            selected = true;
        }
        else if (isKeyPressed(KEY_NSPIRE_ESC))
        {
            nio_fprintf(&csl, "ESC pressed... exiting\n");
            wait_no_key_pressed();
            nio_free(&csl);
            selected = true;
        }
        else
        {
            nio_fprintf(&csl, "(unknown)\n");
            // Display keyboard registers for debugging
            volatile uint16_t* kbd_regs = (volatile uint16_t*)0x900E0010;
            for (int i = 0; i < 8; i++)
            {
                nio_fprintf(&csl, "0x900E00%X: 0x%04X\n", 0x10 + i*2, kbd_regs[i]);
            }
            wait_no_key_pressed();
        }
    }
    
    return 0;
}