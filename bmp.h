#ifndef BMP_H_
#define BMP_H_

struct BMP {
    int8_t temp_major; // [Celsius]
    int8_t temp_minor; // [Celsius / 100]
};

void BMP_setup(void);
void BMP_loop(void);

struct BMP* bmp_get_data();

#endif // BMP_H_
