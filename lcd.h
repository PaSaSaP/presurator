#ifndef LCD_H_
#define LCD_H_

void LCD_Setup(uint16_t min_calib_value, uint16_t max_calib_value);
void LCD_Loop(uint16_t raw_value, 
    int8_t pressure_major, int8_t pressure_minor,
    int8_t temp_major, int8_t temp_minor);

#endif /* LCD_H_ */
