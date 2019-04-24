# RPI_MPU6050_I2C
Library for interfacing an MPU6050 with a Raspberry Pi via I2C using C++. This library is customizable, and contains many of the definitions
from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf already.

## Dependencies
* ***libi2c-dev***. This can be downloaded as follows: ```sudo apt-get install libi2c-dev```
  For more information see https://git.kernel.org/pub/scm/utils/i2c-tools/i2c-tools.git/.
  Note this is different to ```sudo apt-get install i2c-tools```. Installing libi2c-dev adds a diversion for the kernel i2c-dev header and adds one with more
  functions. This was found at https://stackoverflow.com/questions/25159064/why-are-i2c-smbus-function-not-available-i2c-embedded-linux.
* ***i2c-tools***. This is preinstalled on Raspberry Pis to the best of my knowledge.

## Usage
For example usage see main.cpp. The full list of commands can be found in MPU6050.h, where comments explain all of the functions. Note, however, that you
should only be calling the friend functions and public functions from your code. The private functions are private for a reason so keep them that way!

### Wiring
To find the pinout of your pi, go to a terminal and enter ```pinout``` or go to https://pinout.xyz/ and look at the wiring diagram there.  

| MPU6050 |   Raspberry Pi  |  
|:-------:|:---------------:|  
|   VCC   |       3.3V      |  
|   GND   |       GND       |  
|   SCL   |   SCL (GPIO3)   |  
|   SDA   |   SDA (GPIO2)   |  
|   AD0   | [OPTIONAL] 3.3V |  

Connecting AD0 to a high signal changes the I2C address from the default ***0x68*** to ***0x69***. It is pulled to ground internally so this connection is
optional as it is pulled to ground by default. Only connect this pin if you ***need*** to change the I2C address - for example if you are using two of the
MPU6050 modules.

***IMPORTANT:*** If you connect 5V from the Pi to the VCC of the MPU6050 it may output 5V logic instead of 3.3V logic. This ***WILL*** destroy the I2C interface
of your Pi. Either connect VCC of the MPU6050 to 3.3V, or use a Bi-directional logic level converter such as the one from https://www.sparkfun.com/products/12009.

### Basic Function List
* ```MPU6050 IMU;``` Creates an MPU6050 object called IMU. Defaults isRev0 to false. Defaults the I2C address to be accessed to ***0x68***.
* ```MPU6050 IMU(bool isPiRev0);``` Creates an MPU6050 object called IMU using the interface for a revision 0 Pi if isPiRev0 is set to true. This is important as
  the user-mode I2C interface was renamed in later revisions. To find if you need to do this, enter ```ls /dev/*i2c*``` into the command line. If the Pi returns
  ```/dev/i2c-1``` your Pi is not revision 0. If your Pi returns ```/dev/i2c-0```, your Pi is revision 0 and the isRev0 option ***must be set to true!***
* ```MPU6050 IMU(int deviceAddress, bool isPiRev0);``` Changes the I2C address that is to be accessed. Default value is ***0x68***, but this can be changed to
  ***0x69*** if AD0 is pulled high. The ```isPiRev0``` part is, as above, only required to be true if your Pi is revision 0. It will default to false so you can
  simply enter ```MPU6050 IMU(0x69);``` if you are on a Pi that is above revision 0. Any address can be specified here, but as far as I am aware the MPU6050 module
  defaults to ***0x68*** unless you connect the AD0 pin on the MPU6050 to a ***high*** signal, at which point it changes the I2C address to ***0x69***. If you are
  having issues, open the terminal and enter ```i2cdetect -y 1``` (or ```i2cdetect -y 0``` on a revision 0 Pi) and it should allow you to find the I2C address of
  the sensor. If the output table is blank, there is a communication issue between the two devices. All codes on the output table are in HEX, so to specify this
  in C++ simply add ***0x*** to the front of whichever number you see. More detail about i2cdetect can be found under the troubleshooting section.
* ```MPU6050 IMU(int pwrMgmtMode, int gyroConfig, int accelConfig, int deviceAddress, bool isPiRev0);``` Changes the MPU_PWR_MGMT_1 register to the values specified
  by the pwrMgmtMode, gyroConfig and accelConfig parameters. These should not be directly entered as integers, but the definitions from MPU6050.h should instead
  be used to make error checking easier and to reduce the possibility of errors. A lot of effort was put into transferring the confoguration definitions from the
  datasheet so use them! The deviceAddress and isPiRev0 parameters can also be set but are optional. The deviceAddress defaults to ***0x68*** and isPiRev0 defaults
  to ***false***.
* ```IMU.updateData();``` Fetches data from the I2C device and stores it within the IMU object.
* ```IMU.getPARAMETER();``` Replace the ***PARAMETER*** in with the parameter you want to return. Returns the float value of that parameter stored within the IMU
  object. Available parameters are: ***GyroX***, ***GyroY***, ***GyroZ***, ***AccelX***, ***AccelY***, ***AccelZ***, ***Temp***.
* ```std::cout << IMU;``` Displays data about the IMU object in a block of text.

### Compilation
To compile with g++ simply enter the following command: ```g++ -Wall -I. MPU6050.cpp -o MPU6050 main.cpp``` from the directory the project is in.
You can then run the compiled program with ```./MPU6050```. If you wish for the program to be called something else, for instance motionTracker, just change
the line to ```g++ -Wall -I. MPU6050.cpp -o motionTracker main.cpp``` and then you can execute the compiled program with ```./motionTracker```.

## Troubleshooting
This section details steps you can take to try and solve errors when using this library 

### Error Codes
* ***Compile Errors:*** If you are getting compile errors, try compiling the example code. If you are using the example code, and you are using the line in
  the compilation section, you may be having issues with the ***libi2c-dev*** library. Please see the dependencies section and ensure it is installed.
* ***Exit Code 1*** - _Couldn't open the I2C Bus. Please ensure the I2C interface is enabled and that the correct Pi rev version is selected_. This error
  means that the I2C bus cannot be accessed. Ensure I2C is enabled (using ```sudo raspi-config```, go to ***Interfacing Options***, then ***I2C*** and enable
  it). You may need to reboot after this step, and if asked to do so reboot before continuing. Try to run the program again. If you have no luck, check that
  you are using the correct revision (See ```ls /dev/*i2c*``` Troubleshooting Commands). The constructors do not use the revision 0 I2C bus by default. If you
  need it, specify isRev0 as tru as detailed in the Basic Function List section. Currently, we are unable to get access to a revision 0 Pi, so if you are still
  experiencing issues please contact us.
* ***Exit Code 2*** - _The I2C Device couldn't be assigned a slave address. This suggests that there is a communication problem between the devices_. Try using
  ```i2cdetect``` (Detailed in the Troubleshooting Commands section) to identify the address of your module. If it is not ***0x68*** you will need to specify
  the correct address as detailed in the Basic Function List section. If no addresses show up, you may have a wiring fault, or the MPU6050 module may not work. 
  The modules usually have a light on them so you can identify if the module is being powered.
* ***Exit Code 3*** - _Error when setting the power register. Potential connectivity problem?_ This is very similar to Exit Code 2. It suggests there is
  a communication error between the two devices, or that the register is not able to be modified. Try using i2cdetect as for Exit Code 2.
* ***Exit Code 4*** - _Error when setting up the Gyro. Potential connectivity problem?_ This is very similar to Exit Code 2. It suggests there is
  a communication error between the two devices, or that the register is not able to be modified. Try using i2cdetect as for Exit Code 2.
* ***Exit Code 5*** - _Error when setting up the Accelerometer. Potential connectivity problem?_ This is very similar to Exit Code 2. It suggests there is
  a communication error between the two devices, or that the register is not able to be modified. Try using i2cdetect as for Exit Code 2.
* ***Exit Code 6*** - _Error when setting up the interrupts. Potential connectivity problem?_ This is very similar to Exit Code 2, however the interrupts are no
  longer set. If you are getting this error you may be using an older version of the library. It suggests there is a communication error between the two devices,
  or that the register is not able to be modified. Try using i2cdetect as for Exit Code 2.
* ***Exit Code 7*** - _Generic error when parsing inizialization parameters to the constructor._ The parameters that may cause this error are: ***pwrMgmtMode***,
  ***gyroConfig***, and ***accelConfig***. Please check that the parameter you are parsing the constructor is one available within the MPU6050 datasheet or within
  the MPU6050.h definitions. To prevent errors, it is recommended to use these definitions rather than entering a plain number as it will prevent these errors.
* ***Last Resort:*** As a last resort please open an issue on the GitHub page (at https://github.com/NathanielJS1541/RPI_MPU6050_I2C/issues). Note that this is
  the ***preferred*** way to contact us, but requires a GutHub account. If yo do not have a GitHub account, please send an Email to one of us (Emails can be found
  on GitHub Profiles). If you are sending an Email, please include the Repsoitory name in the subject. And in both cases be as specific as possible about your
  issue: include error codes, specific snippits of code that cause problems etc.
  
### Troubleshooting Commands
* ```ls /dev/*i2c*``` Will tell you the revision number of your Pi. If it returns ```/dev/i2c-1``` your Pi is not revision 0 but if it returns ```/dev/i2c-0```,
  your Pi is revision 0.
* ```i2cdetect -y 1``` (Or ```i2cdetect -y 0``` for revision 0 Pis - see above). This will probe I2C addresses on the I2C bus and output a table of which
  addresses were found on the bus. These addresses are in HEX so must be identified as such with ***0x*** at the start when using them in C++, e.g. ***0x68***. 
  More information can be found at https://linux.die.net/man/8/i2cdetect.

## References
* The I2C interfacing was *loosely* based on the code by Johnny Sheppard at https://github.com/JohnnySheppard/Pi2c/blob/master/pi2c.cpp. I initially tried to use
  his code but I had problems with including the Pi2c class with in the MPU6050 class.
* Most information for the I2C interface was found at https://www.kernel.org/doc/Documentation/i2c/dev-interface.
* Address and register definitions were taken from https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf.

## Contributors
* Nathaniel Struselis - NathanielJS1541 on GitHub
* James Clarke        - revivedbear on GitHub
