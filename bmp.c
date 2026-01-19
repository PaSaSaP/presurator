#include <Arduino.h>
#include <I2C_tiny.h>
#include "bmp.h"

#define BMP280_ADDR 0x76
static uint8_t bmp_buf[6];
static int32_t t_fine;
static int32_t t_fine_var1, t_fine_var2;

static struct {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
} bmp_cal;

static struct BMP bmp_data;

static void BMP_write(uint8_t reg, uint8_t val) {
  I2C_write_reg(BMP280_ADDR, reg, val);
}

static void BMP_read_buf(uint8_t reg, uint8_t *buf, uint8_t len) {
  I2C_read_reg(BMP280_ADDR, reg, len);
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = I2C_receive();
  }
}

static void BMP_read_calibration(void) {
  I2C_read_reg(BMP280_ADDR, 0x88, sizeof(bmp_cal));
  uint8_t *buf = (uint8_t *)&bmp_cal;
  for (uint8_t i = 0; i < sizeof(bmp_cal); i++) {
    uint16_t lsb = I2C_receive();
    uint16_t msb = I2C_receive();
    *buf++ = msb;
    *buf++ = lsb;
  }
}

static int32_t BMP_calc_temperature(int32_t adc_T) {
  adc_T >>= 3;

  t_fine_var1 = adc_T;
  t_fine_var1 -= (int32_t)bmp_cal.dig_T1 << 1;
  t_fine_var1 *= (int32_t)bmp_cal.dig_T2;
  t_fine_var1 >>= 11;

  adc_T >>= 1;
  t_fine_var2 = adc_T;
  t_fine_var2 -= (int32_t)bmp_cal.dig_T1;
  t_fine_var2 *= t_fine_var2;
  t_fine_var2 >>= 12;
  t_fine_var2 *= (int32_t)bmp_cal.dig_T3;
  t_fine_var2 >>= 14;

  t_fine = t_fine_var1 + t_fine_var2;
  t_fine *= 5;
  t_fine += 128;
  t_fine >>= 8;

  return t_fine; // temperature [Celsius * 100]
}

void BMP_setup(void) {
  BMP_read_calibration();

  // ctrl_meas: temp x2, pressure x16, normal
  BMP_write(0xF4, 0x57);
  // config: standby 500ms, filter x16
  BMP_write(0xF5, 0xB0);
}

static int32_t BMP_adc_calc(uint8_t* buf) {
  uint32_t adc = 0;
  for (uint8_t* t = buf; t < buf + 3; t++) {
    adc <<= 8;
    adc |= *t;
  }
  adc >>= 4;
  return (int32_t)adc;
}

void BMP_loop(void) {
  BMP_read_buf(0xF7, bmp_buf, 6);

  // uint32_t adc_P = BMP_adc_calc(bmp_buf);
  int32_t adc_T = BMP_adc_calc(bmp_buf + 3);
  int32_t temp_centi = BMP_calc_temperature(adc_T);
  bmp_data.temp_major = temp_centi / 100;
  bmp_data.temp_minor = abs(temp_centi) % 100;
}

struct BMP* bmp_get_data() {
  return &bmp_data;
}
