#ifndef RTC_HPP
#define RTC_HPP

#include <stdint.h>

#define RTC_SECONDS (*(volatile uint32_t*)0x90090000)

/**
 * Converts from R255, G255, B255 to R31, G63, B31 (RGB to RGB565).
 *
 * @param r Red channel (0–255)
 * @param g Green channel (0–255)
 * @param b Blue channel (0–255)
 * @return Packed 16-bit RGB565 value
 */
static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r * 31) / 255 << 11) | ((g * 63) / 255 << 5) | ((b * 31) / 255); // 31 shades of red | 63 shades of green | 31 shades of blue
}

#endif