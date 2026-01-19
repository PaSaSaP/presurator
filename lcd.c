#include <Arduino.h>
#include <LiquidCrystal_I2C_debug.h>
#include "lcd.h"

#define LCD_ADDR 0x27

LiquidCrystal_I2C(lcd, LCD_ADDR, 16, 2);

void LCD_Setup(uint16_t min_calib_value, uint16_t max_calib_value) {
	// initialize the LCD
	lcd_begin();

	// lcd_backlight();
	lcd_print_s("Presurator 0.1");
	// Show calibration values
	lcd_setCursor(0, 1);
	lcd_print_i(min_calib_value);
	lcd_print_c('-');
	lcd_print_i(max_calib_value);
	delay(1000);
}

static void LCD_print_char_with_colon(char c) {
	lcd_print_c(c);
	lcd_print_c(':');
}

static void LCD_print_major_minor(int8_t major, int8_t minor) {
	lcd_print_i(major);
	lcd_print_c('.');
	if (minor < 10) {
		lcd_print_c('0');
	}
	lcd_print_i(minor);
}

void LCD_Loop(uint16_t raw_value, int8_t pressure_major, int8_t pressure_minor,
		int8_t temp_major, int8_t temp_minor) {
	lcd_clear();
	lcd_setCursor(0, 0);
	LCD_print_char_with_colon('R');
	lcd_print_i(raw_value);
	lcd_setCursor(8, 0);
	LCD_print_char_with_colon('T');
	LCD_print_major_minor(temp_major, temp_minor); // minor part can be out of screen

	lcd_setCursor(0, 1);
	LCD_print_char_with_colon('B');
	lcd_setCursor(pressure_major > 10? 3: 4, 1);
	LCD_print_major_minor(pressure_major, pressure_minor);
}
