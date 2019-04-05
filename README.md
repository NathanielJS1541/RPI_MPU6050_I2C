# RPI_MPU6050_I2C
Library for interfacing an MPU6050 with a Raspberry Pi via I2C using C++. This library is customizable, and contains many of the definitions from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf already.

## Dependencies

## Example Usage

## References

* The I2C interfacing was based on the code by Johnny Sheppard at https://github.com/JohnnySheppard/Pi2c/blob/master/pi2c.cpp. I initially tried to use his code but I had problems with including the Pi2c class with in the MPU6050 class.
* Additional information for the I2C interface was found at https://www.kernel.org/doc/Documentation/i2c/dev-interface.
* Address and register definitions were taken from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf.
