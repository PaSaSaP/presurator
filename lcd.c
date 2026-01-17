#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "lcd.h"

#define LCD_ADDR 0x27

LiquidCrystal_I2C(lcd, LCD_ADDR, 16, 2);

void LCD_Setup() {
	// initialize the LCD
	lcd_begin();

	lcd_backlight();
	lcd_print_s("Presurator 0.1");
	delay(1000);
}

void LCD_Loop(uint16_t raw_value, int8_t pressure_major, int8_t pressure_minor) {
	lcd_clear();
	lcd_setCursor(0, 0);
	lcd_print_s("R:");
	lcd_setCursor(3, 0);
	lcd_print_u(raw_value);
	lcd_setCursor(0, 1);
	lcd_print_s("B:");
	lcd_setCursor(pressure_major > 10? 3: 4, 1);
	lcd_print_u(pressure_major);
	lcd_setCursor(5, 1);
	lcd_print_c('.');
	lcd_setCursor(6, 1);
	if (pressure_minor < 10) {
		lcd_print_c('0');
	}
	lcd_print_u(pressure_minor);
}
