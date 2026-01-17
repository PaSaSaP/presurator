#ifndef ATM_H_ 
#define ATM_H_

struct ATM {
    uint16_t raw;
    uint8_t pressure_major;
    uint8_t pressure_minor;
};

void atm_setup(void);
void atm_update_reading(void);
struct ATM* atm_get_reading(void);

uint16_t atm_min_calib_value();
uint16_t atm_max_calib_value();

#endif /* ATM_H_ */
