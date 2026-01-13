#include <Arduino.h>

#include "atm.h"

#define ATM_PIN A1

// #define IDEAL_WORLD // Uncomment to use ideal world calibration
#define SENSOR_BLACK // Uncomment to use SENSOR_BLACK calibration
// #define SENSOR_GREEN // Uncomment to use SENSOR_GREEN calibration

/* 
MEAS_4_20_MA_MAX_ANALOG_READ means the maximum analog read value corresponding to 20mA
which means that ideal sensor would have 15.79 ATM / 1.6MPa / 16 Bar pressure at 20mA
MEAS_4_20_MA_MIN_ANALOG_READ means the minimum analog read value corresponding to 4mA
which means that ideal sensor would have 0 ATM / 0MPa / 0 Bar pressure at 4mA

Different sensors have different calibration values, hence the different defines.
To calibrate your own sensor, measure the analog read value
at 0 ATM and at 15.79 ATM and set the defines accordingly.
*/

#if defined(IDEAL_WORLD)
// Ideal world calibration values
#define MEAS_4_20_MA_MAX_ANALOG_READ (1023) // 10 bit ADC max value
#define MEAS_4_20_MA_MIN_ANALOG_READ                                           \
  (MEAS_4_20_MA_MAX_ANALOG_READ * 4 / 20) // Min value for 4mA

#elif defined(SENSOR_BLACK)
#define MEAS_4_20_MA_MAX_ANALOG_READ (1060)
#define MEAS_4_20_MA_MIN_ANALOG_READ (209)

#elif defined(SENSOR_GREEN)
#define MEAS_4_20_MA_MAX_ANALOG_READ (1084)
#define MEAS_4_20_MA_MIN_ANALOG_READ (216)

#endif

static struct ATM atm_reading;

#define ATM_READINGS_BUFFER_SIZE 8
static struct ATM
    atm_readings_buffer[ATM_READINGS_BUFFER_SIZE]; // zeroed by default
static int8_t atm_last_idx = 0;

#define MAX_ATM_SPIKES_COUNT 5
#define SPIKE_THRESHOLD 50
static int8_t atm_spikes_cnt = 0;

static long atm_raw_to_pressure(int raw) {
  return map(raw, MEAS_4_20_MA_MIN_ANALOG_READ, MEAS_4_20_MA_MAX_ANALOG_READ, 0,
             1579); // [ATM / 100], 1.6MPa = 15.79 ATM
}

static void assign_pressure_to_reading(struct ATM *reading, long pressure) {
  if (pressure < 0)
    pressure = 0;
  reading->pressure_major = pressure / 100;
  reading->pressure_minor = pressure % 100;
}

void atm_setup(void) {}

void atm_update_reading(void) {
  struct ATM last_atm_reading;
  last_atm_reading.raw = analogRead(ATM_PIN);
  if (atm_reading.raw != 0) {
    if (abs(atm_reading.raw - last_atm_reading.raw) > SPIKE_THRESHOLD &&
        atm_spikes_cnt < MAX_ATM_SPIKES_COUNT) {
      // Spike detected, ignore it
      atm_spikes_cnt++;
      return;
    } else {
      atm_spikes_cnt = 0;
    }
  }
  long pressure = atm_raw_to_pressure(last_atm_reading.raw);
  assign_pressure_to_reading(&last_atm_reading, pressure);
  atm_readings_buffer[atm_last_idx] = last_atm_reading;
  atm_last_idx = (atm_last_idx + 1) % ATM_READINGS_BUFFER_SIZE;
}

struct ATM *atm_get_reading(void) {
  // Return the average of the buffered readings, on the start is gives wrong
  // value but to keep it simple we ignore that
  uint32_t raw_sum = 0;
  for (int i = 0; i < ATM_READINGS_BUFFER_SIZE; i++) {
    raw_sum += atm_readings_buffer[i].raw;
  }
  atm_reading.raw = raw_sum / ATM_READINGS_BUFFER_SIZE;
  long pressure = atm_raw_to_pressure(atm_reading.raw);
  assign_pressure_to_reading(&atm_reading, pressure);
  return &atm_reading;
}
