# RPI_MPU6050_I2C
Library for interfacing an MPU6050 with a Raspberry Pi via I2C using C++. This library is customizable, and contains many of the definitions from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf already.

## Dependencies
* i2c-tools. This can be downloaded as follows:
```sudo apt-get install i2c-tools```

## Example Usage
For example usage see main.cpp. The full list of commands can be found in MPU6050.h, where comments explain all of the functions.

## References

* The I2C interfacing was based on the code by Johnny Sheppard at https://github.com/JohnnySheppard/Pi2c/blob/master/pi2c.cpp. I initially tried to use his code but I had problems with including the Pi2c class with in the MPU6050 class.
* Additional information for the I2C interface was found at https://www.kernel.org/doc/Documentation/i2c/dev-interface.
* Address and register definitions were taken from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf.
* Uses code from the following library for the I2C interface: https://git.kernel.org/pub/scm/utils/i2c-tools/i2c-tools.git/.

## Contributors
* Nathaniel Struselis - NathanielJS1541 on GitHub
* James Clarke        - revivedbear on GitHub