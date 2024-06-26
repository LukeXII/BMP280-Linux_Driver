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
uint8_t bmp280_get_status(void);
int16_t bmp280_set_calib_reg(int16_t cal_reg);

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

        bmp280_get_calib();
     
  	printf("Calib: T1: %d, T2: %d, T3: %d \n", _bmp280_calib.dig_T1, _bmp280_calib.dig_T2, _bmp280_calib.dig_T3);
     	printf("Calib: P1: %d, P2: %d, P3: %d \n", _bmp280_calib.dig_P1, _bmp280_calib.dig_P2, _bmp280_calib.dig_P3);
     	printf("Calib: P4: %d, P5: %d, P6: %d \n", _bmp280_calib.dig_P4, _bmp280_calib.dig_P5, _bmp280_calib.dig_P6);
     	printf("Calib: P7: %d, P8: %d, P9: %d \n", _bmp280_calib.dig_P7, _bmp280_calib.dig_P8, _bmp280_calib.dig_P9);

         while(1)
         {

             printf("Temperatura: %.1f °C", bmp280_get_temp());
             printf("\tPresion: %.1f Pa\n", bmp280_get_pressure());
             
             //printf("Status: %X \n", bmp280_get_status());
             
             sleep(2);
         }

        close(my_dev);
    }

    return 0;
}

uint8_t bmp280_get_status(void)
{
    return ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_STATUS, 0));
}

uint8_t bmp280_get_id(void)
{
    uint8_t userbuf = BMP280_REGISTER_CHIPID;

    read(my_dev, &userbuf, 1);

    return userbuf;
}

void bmp280_get_calib(void)
{
    int16_t aux, userbuf;

    _bmp280_calib.dig_T1 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_T1);
    _bmp280_calib.dig_T2 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_T2);
    _bmp280_calib.dig_T3 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_T3);
    
    _bmp280_calib.dig_P1 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P1);
    _bmp280_calib.dig_P2 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P2);
    _bmp280_calib.dig_P3 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P3);
    _bmp280_calib.dig_P4 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P4);
    _bmp280_calib.dig_P5 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P5);
    _bmp280_calib.dig_P6 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P6);
    _bmp280_calib.dig_P7 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P7);
    _bmp280_calib.dig_P8 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P8);
    _bmp280_calib.dig_P9 = bmp280_set_calib_reg(BMP280_REGISTER_DIG_P9);
    
}

int16_t bmp280_set_calib_reg(int16_t cal_reg)
{
    int16_t aux, userbuf;
    
    userbuf = cal_reg;
    read(my_dev, &userbuf, 1);
    aux = userbuf;
    userbuf = cal_reg + 1;
    read(my_dev, &userbuf, 1);
    
    return (userbuf << 8) | aux;
}

float bmp280_get_temp(void)
{
    uint32_t userbuf[2];
    uint32_t aux1, aux2, aux3;
    int32_t var1, var2, temp;

    //ioctl(my_dev, 1, ARG_WRAPPER(1, BMP280_REGISTER_SOFTRESET, 0xB6));    
    //ioctl(my_dev, 1, ARG_WRAPPER(1, BMP280_REGISTER_CONFIG, 0b00101000));
    //ioctl(my_dev, 1, ARG_WRAPPER(1, BMP280_REGISTER_CONTROL, 0b01101111));
    
    userbuf[0] = BMP280_REGISTER_CONTROL;
    userbuf[1] = 0b01101111;
    write(my_dev, &userbuf, 1);
    
    //printf("ctrl reg: %X \n", ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_CONTROL, 0)));		// debug
    
    userbuf[0] = BMP280_REGISTER_TEMPDATA;
    read(my_dev, &userbuf, 1);
    aux1 = userbuf[0];
    
    userbuf[0] = BMP280_REGISTER_TEMPDATA + 1;
    read(my_dev, &userbuf, 1);
    aux2 = userbuf[0];
    
    userbuf[0] = BMP280_REGISTER_TEMPDATA + 2;
    read(my_dev, &userbuf, 1);
    aux3 = userbuf[0];

    temp = (aux1 << 12) | (aux2 << 4) | (aux3 >> 4);
    
    //aux1  = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_TEMPDATA, 0));
    //aux2  = ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_TEMPDATA + 1, 0));
    //temp = (aux1 << 12) | (aux2 << 4) | (ioctl(my_dev, 0, ARG_WRAPPER(1, BMP280_REGISTER_TEMPDATA + 2, 0)) >> 4);
    
    //printf("aux1: %d \n", aux1);	// debug
    //printf("aux2: %d \n", aux2);	// debug
    
    var1 = (((temp >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1)) * ((int32_t)_bmp280_calib.dig_T2)) >> 11;
    var2 = (((((temp >> 4) - ((int32_t)_bmp280_calib.dig_T1)) * ((temp >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) * ((int32_t)_bmp280_calib.dig_T3)) >> 14;
    
    t_fine = var1 + var2;  
    
    float T = (t_fine * 5 + 128) >> 8;
    
    return T / 100;
}

float bmp280_get_pressure(void)
{
    uint32_t userbuf[2];
    uint32_t aux1, aux2, aux3;
    int64_t var1, var2, pressure;
    
    userbuf[0] = BMP280_REGISTER_PRESSUREDATA;
    read(my_dev, &userbuf, 1);
    aux1 = userbuf[0];
    
    userbuf[0] = BMP280_REGISTER_PRESSUREDATA + 1;
    read(my_dev, &userbuf, 1);
    aux2 = userbuf[0];
    
    userbuf[0] = BMP280_REGISTER_PRESSUREDATA + 2;
    read(my_dev, &userbuf, 1);
    aux3 = userbuf[0];

    pressure = (aux1 << 12) | (aux2 << 4) | (aux3 >> 4);
    
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) + ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;
    
    pressure = 1048576 - pressure;
    pressure = (((pressure << 31) - var2) * 3125) / var1;
    
    var1 = (((int64_t)_bmp280_calib.dig_P9) * (pressure >> 13) * (pressure >> 13)) >> 25;
    var2 = (((int64_t)_bmp280_calib.dig_P8) * pressure) >> 19;

    pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
    
    return (float)pressure / 256;
    
}
