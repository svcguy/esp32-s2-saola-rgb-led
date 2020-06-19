/**
 * @file        esp32-s2-saola-rgb-led.h
 * @author      svcguy
 * @brief       A simple set of methods to drive the ESP32-S2-SAOLA-1's
 *              onboard RGB led
 * @version     0.1
 * @date        2020-06-18
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <math.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/rmt.h"

/**
 * @brief       Send a color to the RGB led in RGB format
 * 
 * @param r     uint32_t - Red (0..255)
 * @param g     uint32_t - Green (0..255)
 * @param b     uint32_t - Blue (0..255)
 * @return      esp_err_t - ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t esp_saola_led_set_color_rgb( uint32_t r, uint32_t g, uint32_t b );

/**
 * @brief       Send a color to the RGB led in HSV format
 * 
 * @param h     uint32_t - Hue (0..360)
 * @param s     double - Saturation (0..1)
 * @param v     double - Lightness (0..1)
 * @return      esp_err_t - ESP_OK on success, ESP_ERR_* on failure
 */
esp_err_t esp_saola_led_set_color_hsv( uint32_t h, double s, double v );

#ifdef __cplusplus
}
#endif /* __cplusplus */
