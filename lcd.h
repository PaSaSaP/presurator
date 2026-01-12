#ifndef LCD_H_
#define LCD_H_

void LCD_Setup();
void LCD_Loop(uint16_t raw_atm, int8_t pressure_major, int8_t pressure_minor);

#endif /* LCD_H_ */
