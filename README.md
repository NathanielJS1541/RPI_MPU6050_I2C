# RPI_MPU6050_I2C
Library for interfacing an MPU6050 with a Raspberry Pi via I2C using C++. This library is customizable, and contains many of the definitions from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf already.

## Dependencies
* ***mibi2c-dev***. This can be downloaded as follows: ```sudo apt-get install libi2c-dev```
  For more information see https://git.kernel.org/pub/scm/utils/i2c-tools/i2c-tools.git/.
  Note this is different to ```sudo apt-get install i2c-tools```. Installing libi2c-dev adds a diversion for the kernel i2c-dev header and adds one with more functions.
  This was found at https://stackoverflow.com/questions/25159064/why-are-i2c-smbus-function-not-available-i2c-embedded-linux.
* ***i2c-tools***. This is preinstalled on Raspberry Pis to the best of my knowledge.

## Example Usage
For example usage see main.cpp. The full list of commands can be found in MPU6050.h, where comments explain all of the functions.

### Basic Function List
* ```MPU6050 IMU;``` Creates an MPU6050 object called IMU.
* ```IMU.updateData();``` Fetches data from the I2C device and stores it within the IMU object.
* ```IMU.get[GyroX]();``` Replace the part in brackets with the parameter you want to return. Returns the float value of that parameter stored within the IMU object.
  Available parameters are: ***GyroX***, ***GyroY***, ***GyroZ***, ***AccelX***, ***AccelY***, ***AccelZ***, ***Temp***.
* ```std::cout << IMU;``` Displays data about the IMU object in a block of text.

### Compilation
To compile with g++ simply enter the following command: ```g++ -Wall -I. MPU6050.cpp -o MPU6050 main.cpp``` from the directory the project is in.
You can then run the compiled program with ```./MPU6050```.

## References
* The I2C interfacing was *loosely* based on the code by Johnny Sheppard at https://github.com/JohnnySheppard/Pi2c/blob/master/pi2c.cpp. I initially tried to use his code but I had problems with including the Pi2c class with in the MPU6050 class.
* Most information for the I2C interface was found at https://www.kernel.org/doc/Documentation/i2c/dev-interface.
* Address and register definitions were taken from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf.

## Contributors
* Nathaniel Struselis - NathanielJS1541 on GitHub
* James Clarke        - revivedbear on GitHub