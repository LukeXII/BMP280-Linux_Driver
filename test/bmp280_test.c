#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define BMP280_CHIPID (0x58) /**< Default chip ID. */
#define ARG_WRAPPER(len, reg, val) ( len | (reg << 8) | (val << 16) )

/* Registers available on the sensor */
enum {
  BMP280_REGISTER_DIG_T1 = 0x88,
  BMP280_REGISTER_DIG_T2 = 0x8A,
  BMP280_REGISTER_DIG_T3 = 0x8C,
  BMP280_REGISTER_DIG_P1 = 0x8E,
  BMP280_REGISTER_DIG_P2 = 0x90,
  BMP280_REGISTER_DIG_P3 = 0x92,
  BMP280_REGISTER_DIG_P4 = 0x94,
  BMP280_REGISTER_DIG_P5 = 0x96,
  BMP280_REGISTER_DIG_P6 = 0x98,
  BMP280_REGISTER_DIG_P7 = 0x9A,
  BMP280_REGISTER_DIG_P8 = 0x9C,
  BMP280_REGISTER_DIG_P9 = 0x9E,
  BMP280_REGISTER_CHIPID = 0xD0,
  BMP280_REGISTER_VERSION = 0xD1,
  BMP280_REGISTER_SOFTRESET = 0xE0,
  BMP280_REGISTER_CAL26 = 0xE1,         /* R calibration = 0xE1-0xF0 */
  BMP280_REGISTER_STATUS = 0xF3,
  BMP280_REGISTER_CONTROL = 0xF4,
  BMP280_REGISTER_CONFIG = 0xF5,
  BMP280_REGISTER_PRESSUREDATA = 0xF7,
  BMP280_REGISTER_TEMPDATA = 0xFA,
};

/* Struct to hold calibration data */
struct bmp280_calib_data {
  uint16_t dig_T1; /**< dig_T1 cal register. */
  int16_t dig_T2;  /**< dig_T2 cal register. */
  int16_t dig_T3;  /**< dig_T3 cal register. */

  uint16_t dig_P1; /**< dig_P1 cal register. */
  int16_t dig_P2;  /**< dig_P2 cal register. */
  int16_t dig_P3;  /**< dig_P3 cal register. */
  int16_t dig_P4;  /**< dig_P4 cal register. */
  int16_t dig_P5;  /**< dig_P5 cal register. */
  int16_t dig_P6;  /**< dig_P6 cal register. */
  int16_t dig_P7;  /**< dig_P7 cal register. */
  int16_t dig_P8;  /**< dig_P8 cal register. */
  int16_t dig_P9;  /**< dig_P9 cal register. */
};

int32_t t_fine;
static struct bmp280_calib_data _bmp280_calib;

uint8_t bmp280_get_id(void);
void bmp280_get_calib(void);
float bmp280_get_temp(void);
float bmp280_get_pressure(void);

int my_dev;

int main(void)
{
    my_dev = open("/dev/mse00", 0);

    if (my_dev < 0)
    {
        perror("Fail to open device file: /dev/mse00.");
    }
    else
    {
        printf("Device ID: %X \n", bmp280_get_id());


        // while(true)
        // {
             //printf("Temperatura: %f \n", bmp280_get_temp());
             //printf("Presion: %f \n", bmp280_get_pressure());
             
             bmp280_get_calib();
             
             printf("Calib: T1: %d, T2: %d, T3: %d \n", _bmp280_calib.dig_T1, _bmp280_calib.dig_T2, _bmp280_calib.dig_T3);
             
             sleep(1);
        // }

        //ioctl(my_dev, 100, 110); /* cmd = 100, arg = 110. */
        close(my_dev);
    }

    return 0;
}

uint8_t bmp280_get_id(void)
{

    return ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_CHIPID, 0));
}

void bmp280_get_calib(void)
{
    uint8_t temp;
    
    temp = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_DIG_T1, 0));
    _bmp280_calib.dig_T1 = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_DIG_T1 + 1, 0)) << 8 | temp;
    temp = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_DIG_T2, 0));
    _bmp280_calib.dig_T2 = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_DIG_T2 + 1, 0)) << 8 | temp;
    temp = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_DIG_T3, 0));
    _bmp280_calib.dig_T3 = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_DIG_T3 + 1, 0)) << 8 | temp;
}

float bmp280_get_temp(void)
{
    int32_t var1, var2;
    
    int32_t temp = ioctl(my_dev, 0, ARG_WRAPPER(3, BMP280_REGISTER_CHIPID, 0));
    
    
    t_fine = var1 + var2;
    
    return 0;
}

float bmp280_get_pressure(void)
{

}
