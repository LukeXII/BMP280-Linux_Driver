#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define BMP280_CHIPID (0x58) /**< Default chip ID. */

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

int my_dev

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
        //     bmp280_get_temp();
        //     bmp280_get_pressure();
        // }

        //ioctl(my_dev, 100, 110); /* cmd = 100, arg = 110. */
        close(my_dev);
    }

    return 0;
}

uint8_t bmp280_get_id(void)
{
    char buffer[5];

    mse_read(my_dev, buffer, 0, 0);

    return buffer[0];
}

// void bmp280_get_temp()
// {
    
// }

// void bmp280_get_pressure()
// {

// }