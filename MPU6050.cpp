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
 * --------------------------------------------------------------------------------------------
 */

#include "MPU6050.h" // Include definitions and declarations within the header file

// Used for the I2C interface
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>    // For O_RDWR
#include <unistd.h>   // For open()

// ----------------------------- Special class member definitions -----------------------------
// Default constructor
MPU6050::MPU6050(){
    address = MPU_DEFAULT_I2C_ADDR;

    i2cHandle = open(fileName, O_RDWR);
    if (i2cHandle < 0) {
		std::cout << std::endl << "Can't open I2C BUS" << std::endl; //If there's an error opening this, then display it.
	}
	if (ioctl(i2cHandle, I2C_SLAVE, address) < 0) { //Using ioctl set the i2c device to talk to address in the "addr" variable.
		std::cout << std::endl << "Can't set the I2C address for the slave device" << std::endl; //Display error setting the address for the slave.
	}

    gyroX = 0;
    gyroY = 0;
    gyroZ = 0;
    accelX = 0;
    accelY = 0;
    accelZ = 0;
    temperature = 0;
    initialise(); // Set the registers for the MPU
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
    char requestRegister[1];

    // ------------ Get Gyro Data ------------
    // X Axis
    requestRegister[0] = MPU_GYRO_X1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    gyroX = float(rawData[0] << 8 | rawData[1]);
    // Y Axis
    requestRegister[0] = MPU_GYRO_Y1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    gyroY = float(rawData[0] << 8 | rawData[1]);
    // Z Axis
    requestRegister[0] = MPU_GYRO_Z1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    gyroZ = float(rawData[0] << 8 | rawData[1]);
    // ---------------------------------------

    // ------- Get Accelerometer Data --------
    // X Axis
    requestRegister[0] = MPU_ACC_X1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    accelX = float(rawData[0] << 8 | rawData[1]);
    // Y Axis
    requestRegister[0] = MPU_ACC_Y1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    accelY = float(rawData[0] << 8 | rawData[1]);
    // Z Axis
    requestRegister[0] = MPU_ACC_Z1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    accelZ = float(rawData[0] << 8 | rawData[1]);
    // ---------------------------------------

    // -------- Get Temperature Data ---------
    requestRegister[0] = MPU_TEMP1;
    write(i2cHandle, requestRegister, 1);
    read(i2cHandle, rawData, 2);
    temperature = float(rawData[0] << 8 | rawData[1]);
    temperature = (temperature/340)+36.53; // Convert temperature to Celcius
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
    char dataPacket[2]; // Data to be transmitted (2 bytes at a time)

    // Configure MPU Power mode
    dataPacket[0] = MPU_PWR_MGMT_1;
    dataPacket[1] = MPU_PWR_MGMT_CLK_INTERNAL_8MHZ; // Use the 8 MHz internal clock
    write(i2cHandle, dataPacket, 1);

    // Configure the Gyroscope
    dataPacket[0] = MPU_GYRO_CONFIG;
    dataPacket[1] = MPU_GYRO_SENS_500 << 3; // Set the sensitivity. Shifted to the right to correctly position it in the register
    write(i2cHandle, dataPacket, 1);

    // Configure the Accelerometer
    dataPacket[0] = MPU_ACC_CONFIG;
    dataPacket[1] = MPU_ACC_SENS_2 << 3; // Set the sensitivity. Used for angle sensing when stationary so doesn't need a large range
    write(i2cHandle, dataPacket, 1);
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
