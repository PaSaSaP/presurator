#include <Arduino.h>
#include <I2C_tiny.h>

#include "main.h"
#include "atm.h"
#include "bmp.h"
#include "lcd.h"

inline void watchdogInit(void) {
  IWDG->KR = 0xCC;   // start
  IWDG->KR = 0x55;   // unlock
  IWDG->PR = 0x06;   // prescaler 256
  IWDG->RLR = 255;  // ~500 ms
  IWDG->KR = 0xAA;  // reload
}

inline void watchdogKick(void) {
  IWDG->KR = 0xAA;
}

void main_setup(void) {
  watchdogInit();
  I2C_begin();
  atm_setup();
  BMP_setup();
  LCD_Setup(atm_min_calib_value(), atm_max_calib_value());
}

static unsigned long last_atm_update = 0;
static unsigned long last_bmp_update = 0;
static unsigned long last_lcd_update = 0;
unsigned long current_time = 0;

void main_loop(void) {
  current_time = millis();
  if (current_time - last_atm_update > 50) {
    last_atm_update = current_time;
    atm_update_reading();
  }
  if (current_time - last_bmp_update > 400) {
    last_bmp_update = current_time;
    BMP_loop();
  }
  if (current_time - last_lcd_update > 200) {
    last_lcd_update = current_time;
    struct ATM *atm_reading = atm_get_reading();
    struct BMP * bmp_reading = bmp_get_data();
    LCD_Loop(atm_reading->raw, atm_reading->pressure_major,
             atm_reading->pressure_minor, 
             bmp_reading->temp_major, bmp_reading->temp_minor,
            bmp_reading->hum_major, bmp_reading->hum_minor);
  }
  watchdogKick();
}
