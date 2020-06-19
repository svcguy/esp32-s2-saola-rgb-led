/**
 * @file        esp32-s2-saola-rgb-led.c
 * @author      svcguy
 * @brief       A simple set of methods to drive the ESP32-S2-SAOLA-1's
 *              onboard RGB led
 * @version     0.1
 * @date        2020-06-18
 * 
 * @copyright   Copyright (c) 2020
 * 
 */

#include "esp32-s2-saola-rgb-led.h"

// Timing parameters from the datasheet
// https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf
#define WS2812_T0H_NS   400
#define WS2812_T1H_NS   800
#define WS2812_T0L_NS   850
#define WS2812_T1L_NS   450

static const char *TAG = "esp32-s2-saola-led";

esp_err_t esp_saola_led_set_color_rgb( uint32_t r, uint32_t g, uint32_t b )
{
   esp_err_t ret = ESP_OK;

   ESP_LOGI(TAG, "Call to set color to R=%d, G=%d, B=%d", r, g, b);
   ESP_LOGD(TAG, "Configuring RMT");

   // Parameter check
   if( r > 255 || g > 255 || b > 255)
   {
      return ESP_ERR_INVALID_ARG;
   }

   // Use default RMT TX Config
   rmt_config_t led_config = RMT_DEFAULT_CONFIG_TX(GPIO_NUM_18, RMT_CHANNEL_0);
   // Set clock to 20MHz
   led_config.clk_div = 4;
   // No loop
   led_config.tx_config.loop_en = false;
   // Setup the RMT peripherial
   ESP_ERROR_CHECK( rmt_config(&led_config) );
   ESP_ERROR_CHECK( rmt_driver_install(led_config.channel, 0, 0) );
   ESP_LOGD(TAG, "RMT Configured for channel %d", led_config.channel);

   ESP_LOGD(TAG, "Setting color data");
   // One clock period, or 'tick' is 50ns at 20MHz, so lets create a conversion factor
   uint32_t rmt_clock_hz;
   rmt_get_counter_clock( led_config.channel, &rmt_clock_hz);
   uint32_t rmt_tick_ns = 1e9 / rmt_clock_hz;
   ESP_LOGD(TAG, "RMT timing parameters: speed=%dHz, tick period = %dns", rmt_clock_hz, rmt_tick_ns);
   // Now, lets create the patterns for 0s and 1s
   const rmt_item32_t bit0 = {{{ (WS2812_T0H_NS / rmt_tick_ns), 1, (WS2812_T0L_NS / rmt_tick_ns), 0}}};
   const rmt_item32_t bit1 = {{{ (WS2812_T1H_NS / rmt_tick_ns), 1, (WS2812_T1L_NS / rmt_tick_ns), 0}}};

   // To write a color to a single WS2812 takes 3 bytes, or 24 bits
   rmt_item32_t color_to_write[24];
   // Cycle through the desired color, remembering that the order is GRB on the WS2812B, MSB first
   for( uint8_t i = 0; i < sizeof(color_to_write) / sizeof(rmt_item32_t); i++ )
   {
      if(i < 8)
      // Green
      {
         if(g >> ( 7 - i%8 ))
         {
            color_to_write[i] = bit1;
            ESP_LOGD(TAG, "bit[%d] = 1", i);
         }
         else
         {
            color_to_write[i] = bit0;
            ESP_LOGD(TAG, "bit[%d] = 0", i);
         }         
      }
      if(i >= 8 && i < 16)
      // Red
      {
         if(r >> ( 7 - i%8 ))
         {
            color_to_write[i] = bit1;
            ESP_LOGD(TAG, "bit[%d] = 1", i);
         }
         else
         {
            color_to_write[i] = bit0;
            ESP_LOGD(TAG, "bit[%d] = 0", i);
         }         
      }
      if(i >= 16 && i < 24)
      // Blue
      {
         if(b >> ( 7 - i%8))
         {
            color_to_write[i] = bit1;
            ESP_LOGD(TAG, "bit[%d] = 1", i);
         }
         else
         {
            color_to_write[i] = bit0;
            ESP_LOGD(TAG, "bit[%d] = 0", i);
         }
         
      }
   }

   ESP_LOGD(TAG, "Sending color data to led");
   // Send it off the the led
   ESP_ERROR_CHECK( rmt_write_items(led_config.channel, 
                                    color_to_write, 
                                    sizeof(color_to_write) / sizeof(rmt_item32_t), 
                                    0) );

   // Done so now uninstall the driver
   ESP_ERROR_CHECK( rmt_driver_uninstall(led_config.channel) );

   return ret;
}

esp_err_t esp_saola_led_set_color_hsv( uint32_t h, double s, double v )
{
    // Thanks to github user kuathadianto for the algorithm
    //    https://gist.github.com/kuathadianto/200148f53616cbd226d993b400214a7f
    uint32_t r, g, b;
    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Call to set led to H=%d, S=%f, V=%f", h, s, v);
    // Parameter check
    if(h > 360 || s > 1.0 || v > 1.0)
    {
        return ESP_ERR_INVALID_ARG;
    }
    // Convert HSV to RGB
    double C = s * v;
    double X = C * (1 - abs(fmod(h / 60.0, 2) - 1));
    double m = v - C;
    double Rs, Gs, Bs;

	if(h < 60) 
    {
		Rs = C;
		Gs = X;
		Bs = 0;	
	}
	else if(h >= 60 && h < 120) 
    {	
		Rs = X;
		Gs = C;
		Bs = 0;	
	}
	else if(h >= 120 && h < 180) 
    {
		Rs = 0;
		Gs = C;
		Bs = X;	
	}
	else if(h >= 180 && h < 240) 
    {
		Rs = 0;
		Gs = X;
		Bs = C;	
	}
	else if(h >= 240 && h < 300) 
    {
		Rs = X;
		Gs = 0;
		Bs = C;	
	}
	else 
    {
		Rs = C;
		Gs = 0;
		Bs = X;	
	}
	
	r = (Rs + m) * 255;
	g = (Gs + m) * 255;
	b = (Bs + m) * 255;

   ESP_LOGD(TAG, "Converted to R=%d, G=%d, B=%d", r, g, b);
   // Send to led
   ESP_ERROR_CHECK( esp_saola_led_set_color_rgb(r, g, b) );

   return ret;
}