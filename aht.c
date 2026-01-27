#include <Arduino.h>
#include <I2C_tiny.h>
#include "aht.h"

// AHT20
#define AHT_ADDR 0x38

static enum {
  AhtSensorInit,
  AhtSensorWaitingForData,
  AhtSensorReadingDone
} ahtState;

static uint8_t aht_buf[6];
static struct AHT aht_data;

static void AHT_write(uint8_t reg, uint8_t val) {
  I2C_write_reg(AHT_ADDR, reg, val);
}

static void AHT_read_buf(uint8_t reg, uint8_t *buf, uint8_t len) {
  I2C_read_reg(AHT_ADDR, reg, len);
  for (uint8_t i = 0; i < len; i++) {
    buf[i] = I2C_receive();
  }
}

void AHT_setup(void) {
  I2C_write_sn(AHT_ADDR, 0xBE, (uint8_t*) "\x08\x00", 2);
}

static void AHT_startReading(void) {
  I2C_write_sn(AHT_ADDR, 0xAC, (uint8_t*) "\x33\x00", 2);
}

static int32_t AHT_adc_calc(uint8_t *buf, uint8_t moveRight) {
  uint32_t adc = 0;
  for (uint8_t *t = buf; t < buf + 3; t++) {
    adc <<= 8;
    adc |= *t;
  }
  if (moveRight) {
    adc >>= 4;
  }
  adc &= 0x0FFFFF; // 20 bits
  return (int32_t)adc;
}

void AHT_loop(void) {
  // reading should not be too quick so sensor doesn't get heat
  // also sensor needs about 80-85 ms to make reading
  if (ahtState != AhtSensorWaitingForData) {
    AHT_startReading();
    ahtState = AhtSensorWaitingForData;
    return;
  }

  // first byte is status, should be 1, not checked here
  AHT_read_buf(0x38, aht_buf, sizeof(aht_buf));
  ahtState = AhtSensorReadingDone;

  // float humidity = (raw_hum * 100.0f) / 1048576.0f;
  int32_t adc = AHT_adc_calc(aht_buf+1, 1);
  adc = adc * 625 / 65536; // [H / 100], no overflow
  aht_data.hum_major = adc / 100;
  aht_data.hum_minor = abs(adc) % 100;

  // float temperature = (raw_temp * 200.0f) / 1048576.0f - 50.0f;
  adc = AHT_adc_calc(aht_buf + 3, 0);
  adc = adc * 625 / 32768 - 5000; // [T / 100], no overflow
  aht_data.temp_major = adc / 100;
  aht_data.temp_minor = abs(adc) % 100;
}

struct AHT *AHT_getData() { return &aht_data; }
