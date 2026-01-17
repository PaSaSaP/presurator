#include <Arduino.h>
#include <Wire.h>

#include "main.h"
#include "atm.h"
#include "lcd.h"

void main_setup(void) {
  Wire_setClock(400000);
  atm_setup();
  LCD_Setup(atm_min_calib_value(), atm_max_calib_value());
}

static unsigned long last_atm_update = 0;
static unsigned long last_lcd_update = 0;
unsigned long current_time = 0;

void main_loop(void) {
  current_time = millis();
  if (current_time - last_atm_update > 50) {
    last_atm_update = current_time;
    atm_update_reading();
  }
  if (current_time - last_lcd_update > 200) {
    last_lcd_update = current_time;
    struct ATM *atm_reading = atm_get_reading();
    LCD_Loop(atm_reading->raw, atm_reading->pressure_major,
             atm_reading->pressure_minor);
  }
}
