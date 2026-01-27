#ifndef BMP_H_
#define BMP_H_

struct AHT {
    int8_t temp_major; // [Celsius]
    int8_t temp_minor; // [Celsius / 100]
    int8_t hum_major; // [humidity in %]
    int16_t hum_minor; // [humidity / 1024]
};

void AHT_setup(void);
void AHT_loop(void);

struct AHT* AHT_getData();

#endif // BMP_H_
