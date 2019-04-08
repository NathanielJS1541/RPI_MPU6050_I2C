/* ============================================================================================
 * MPU6050 Interface Code for Raspberry Pi
 * ============================================================================================
 * Written by Nathaniel Struselis.
 * Last Update: 05/04/2019
 * --------------------------------------------------------------------------------------------
 * This source code declares the functions and defines the functions used to access data on the
 * MPU6050 IMU. The registers are based off the addresses found at invensense:
 * https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf. This is
 * based on the Pi2c library from https://github.com/JohnnySheppard/Pi2c.
 * Additional I2C documentation was found at
 * https://www.kernel.org/doc/Documentation/i2c/dev-interface.
 * --------------------------------------------------------------------------------------------
 */

#include "MPU6050.h" // Include definitions and declarations within the header file

#include <iostream>  // Used for data output

// Used for the I2C interface
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>     // For ioctl()
#include <fcntl.h>         // For O_RDWR
#include <unistd.h>        // For open()

// ----------------------------- Special class member definitions -----------------------------
// Default constructor
MPU6050::MPU6050(){
    // If the address for the device is not specified, use the default address.
    address = MPU_DEFAULT_I2C_ADDR;

    snprintf(fileName, 10, "/dev/i2c-%d", 1); // The I2C interface is 1 unless it is a rev0 Pi

    // Initialise the I2C interface
    i2cHandle = open(fileName, O_RDWR);
    if (i2cHandle < 0) {
        std::cout << std::endl << "Couldn't open the I2C Bus. Please ensure the I2C interface is enabled." << std::endl;
        exit(I2C_BUS_INIT_ERROR);
    }

    // Set the slave address for the device
	if (ioctl(i2cHandle, I2C_SLAVE, address) < 0) {
		std::cout << std::endl << "The I2C Device couldn't be assigned a slave address." << std::endl;
		exit(I2C_SET_SLAVE_ADDR_ERR);
	}

    gyroX = 0;
    gyroY = 0;
    gyroZ = 0;
    accelX = 0;
    accelY = 0;
    accelZ = 0;
    temperature = 0;

    // Set the registers for the MPU
    initialise();
}

MPU6050::MPU6050(const MPU6050& M){
    *this = M; // Make use of the assignment operator
}

MPU6050::~MPU6050(){
    if(i2cHandle){
        close(i2cHandle);
    }
}

// --------------------------------------------------------------------------------------------

// ----------------------------------- Operator Overloading -----------------------------------
// Assignment Operator
MPU6050& MPU6050::operator=(const MPU6050& M){
    if(this == &M)return *this; // Do nothing if assigned to itself
    address = M.address;
    i2cHandle = M.i2cHandle;

    // Set gyro data
    gyroX = M.gyroX;
    gyroY = M.gyroY;
    gyroZ = M.gyroZ;

    // Set accel data
    accelX = M.accelX;
    accelY = M.accelY;
    accelZ = M.accelZ;

    // Set the temperature
    temperature = M.temperature;

    return *this;
}
// --------------------------------------------------------------------------------------------

// ---------------------------------- Data Access Functions -----------------------------------
void MPU6050::updateData(){
    char rawData[2];
    __u8 requestRegister;
    __u32 returnedData;

    // ------------ Get Gyro Data ------------
    // X Axis
    requestRegister = MPU_GYRO_X1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Gyro X axis data." << std::endl;
    }
    else{
        gyroX = float(returnedData);
    }

    // Y Axis
    requestRegister = MPU_GYRO_Y1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Gyro Y axis data." << std::endl;
    }
    else{
        gyroY = float(returnedData);
    }

    // Z Axis
    requestRegister = MPU_GYRO_Z1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Gyro Z axis data." << std::endl;
    }
    else{
        gyroZ = float(returnedData);
    }
    // ---------------------------------------

    // ------- Get Accelerometer Data --------
    // X Axis
    requestRegister = MPU_ACC_X1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Accel X axis data." << std::endl;
    }
    else{
        accelX = float(returnedData);
    }
    // Y Axis
    requestRegister = MPU_ACC_Y1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Accel Y axis data." << std::endl;
    }
    else{
        accelY = float(returnedData);
    }
    // Z Axis
    requestRegister = MPU_ACC_Z1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Accel Z axis data." << std::endl;
    }
    else{
        accelZ = float(returnedData);
    }
    // ---------------------------------------

    // -------- Get Temperature Data ---------
    requestRegister = MPU_TEMP1;
    returnedData = i2c_smbus_read_word_data(i2cHandle, requestRegister);
    if(returnedData < 0){
        std::cout << std::endl << "Error accessing Temperature data." << std::endl;
    }
    else{
        temperature = float(returnedData);
        temperature = (temperature/340)+36.53; // Convert temperature to Celcius
    }
    // ---------------------------------------
}

float MPU6050::getGyroX(){return gyroX;}

float MPU6050::getGyroY(){return gyroY;}

float MPU6050::getGyroZ(){return gyroZ;}

float MPU6050::getAccelX(){return accelX;}

float MPU6050::getAccelY(){return accelY;}

float MPU6050::getAccelZ(){return accelZ;}

float MPU6050::getTemp(){return temperature;}


// --------------------------------------------------------------------------------------------

// --------------------------------- Private Class Functions ----------------------------------
void MPU6050::initialise(){
    __u8 deviceRegister; // The device register to access
    __s32 returnedData;  // The data returned by the device

    // Configure the MPU Power Mode
    deviceRegister = MPU_PWR_MGMT_1;
    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, MPU_PWR_MGMT_CLK_INTERNAL_8MHZ);
    if (returnedData < 0){
        // Not sure if any data SHOULD be returned yet...
        std::cout << std::endl << "Data was not returned by the device when setting the power register. Potential connectivity problem?" << std::endl;
        exit(I2C_SET_SLAVE_PWR_MODE);
    }

    // Configure the Gyroscope
    deviceRegister = MPU_GYRO_CONFIG;
    // Set the sensitivity. Value shifted to the right to correctly position it in the register
    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, MPU_GYRO_SENS_500 << 3);
    if (returnedData < 0){
        // Not sure if any data SHOULD be returned yet...
        std::cout << std::endl << "Data was not returned by the device when setting up the Gyro. Potential connectivity problem?" << std::endl;
        exit(I2C_SET_GYRO_RES);
    }


    // Configure the Accelerometer
    deviceRegister = MPU_ACC_CONFIG;
    // Set the sensitivity. Value shifted to the right to correctly position it in the register
    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, MPU_ACC_SENS_2 << 3);
    if (returnedData < 0){
        // Not sure if any data SHOULD be returned yet...
        std::cout << std::endl << "Data was not returned by the device when setting up the Accelerometer. Potential connectivity problem?" << std::endl;
        exit(I2C_SET_ACCEL_RES);
    }
}
// --------------------------------------------------------------------------------------------

// ---------------------------------- Data Display Function -----------------------------------
std::ostream& operator<<(std::ostream& out, MPU6050& M){
    out << std::endl;
    out << "-------------------------------------" << std::endl;
    out << "Gyro Values:" << std::endl;
    out << "GyroX: " << M.gyroX << std::endl;
    out << "GyroY: " << M.gyroY << std::endl;
    out << "GyroZ: " << M.gyroZ << std::endl;
    out << std::endl;
    out << "Accel Values:" << std::endl;
    out << "AccelX: " << M.accelX << std::endl;
    out << "AccelY: " << M.accelY << std::endl;
    out << "AccelZ: " << M.accelZ << std::endl;
    out << std::endl;
    out << "Temp: " << M.temperature << std::endl;
    out << "-------------------------------------" << std::endl;
    return out;
}
// --------------------------------------------------------------------------------------------
