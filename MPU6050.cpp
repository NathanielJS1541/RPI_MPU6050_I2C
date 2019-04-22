/* ============================================================================================
 * MPU6050 Interface Code for Raspberry Pi
 * ============================================================================================
 * Written by Nathaniel Struselis & James Clarke.
 * Last Update: 22/04/2019
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
#include <linux/i2c-dev.h> // For SMBus commands
#include <sys/ioctl.h>     // For ioctl()
#include <fcntl.h>         // For O_RDWR
#include <unistd.h>        // For open()

// ----------------------------- Special class member definitions -----------------------------
// Default constructor
MPU6050::MPU6050(){
    // If the address for the device is not specified, use the default address.
    address = MPU_DEFAULT_I2C_ADDR;

    snprintf(fileName, 11, "/dev/i2c-%d", 1); // The I2C interface is 1 unless it is a rev0 Pi

    // Initialise the I2C interface
    i2cHandle = open(fileName, O_RDWR);
    if (i2cHandle < 0) {
        std::cout << std::endl << "Couldn't open the I2C Bus. Please ensure the I2C interface is enabled and that the correct Pi rev version is selected." << std::endl;
        exit(I2C_BUS_INIT_ERROR);
    }

    // Set the slave address for the device
	if (ioctl(i2cHandle, I2C_SLAVE, address) < 0) {
		std::cout << std::endl << "The I2C Device couldn't be assigned a slave address." << std::endl;
		exit(I2C_SET_SLAVE_ADDR_ERR);
	}

    // Set the registers for the MPU
    initialise();

	// Get an initial set of readings
	updateData();
}

// Constructor which can be used if the Pi is rev0
MPU6050::MPU6050(bool isPiRev0){
    // If the address for the device is not specified, use the default address.
    address = MPU_DEFAULT_I2C_ADDR;

    if(isPiRev0){
        snprintf(fileName, 11, "/dev/i2c-%d", 0); // The I2C interface is 0 on a rev0 Pi
    }
    else{
        snprintf(fileName, 11, "/dev/i2c-%d", 1); // The I2C device is 1 otherwise
    }

    // Initialise the I2C interface
    i2cHandle = open(fileName, O_RDWR);
    if (i2cHandle < 0) {
        std::cout << std::endl << "Couldn't open the I2C Bus. Please ensure the I2C interface is enabled and that the correct Pi rev version is selected." << std::endl;
        exit(I2C_BUS_INIT_ERROR);
    }

    // Set the slave address for the device
    if (ioctl(i2cHandle, I2C_SLAVE, address) < 0) {
        std::cout << std::endl << "The I2C Device couldn't be assigned a slave address." << std::endl;
        exit(I2C_SET_SLAVE_ADDR_ERR);
    }

    // Set the registers for the MPU
    initialise();

	// Get an initial set of readings
	updateData();
}

// Constructor with ability to select a custom address and select if the Pi is rev0
MPU6050::MPU6050(int deviceAddress, bool isPiRev0){
    // For this constructor the device address MUST be specified, so use that address.
    address = deviceAddress;

    if(isPiRev0){
        snprintf(fileName, 10, "/dev/i2c-%d", 0); // The I2C interface is 0 on a rev0 Pi
    }
    else{
        snprintf(fileName, 10, "/dev/i2c-%d", 1); // The I2C device is 1 otherwise
    }

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

    // Set the registers for the MPU
    initialise();

	// Get an initial set of readings
	updateData();
}

// Copy constructor
MPU6050::MPU6050(const MPU6050& M){
    *this = M; // Make use of the assignment operator
}

// Destructor - close the I2C handle to end transmissions
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
    bool readError = false;
    int16_t rawData;

    // ------------ Get Gyro Data ------------
    // X Axis
	rawData = read16BitRegister(MPU_GYRO_X1, MPU_GYRO_X2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Gyro X axis data." << std::endl;
        gyroX = 0;
    }
    else{
        gyroX = float(rawData)/gyroScale;
    }

    // Y Axis
    rawData = read16BitRegister(MPU_GYRO_Y1, MPU_GYRO_Y2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Gyro Y axis data." << std::endl;
        gyroY = 0;
    }
    else{
        gyroY = float(rawData)/gyroScale;
    }

    // Z Axis
    rawData = read16BitRegister(MPU_GYRO_Z1, MPU_GYRO_Z2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Gyro Z axis data." << std::endl;
        gyroZ = 0;
    }
    else{
        gyroZ = float(rawData)/gyroScale;
    }
    // ---------------------------------------

    // ------- Get Accelerometer Data --------
    // X Axis
    rawData = read16BitRegister(MPU_ACC_X1, MPU_ACC_X2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Accel X axis data." << std::endl;
        accelX = 0;
    }
    else{
        accelX = float(rawData)/accelScale;
    }
    // Y Axis
    rawData = read16BitRegister(MPU_ACC_Y1, MPU_ACC_Y2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Accel Y axis data." << std::endl;
        accelY = 0;
    }
    else{
        accelY = float(rawData)/accelScale;
    }
    // Z Axis
    rawData = read16BitRegister(MPU_ACC_Z1, MPU_ACC_Z2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Accel Z axis data." << std::endl;
        accelZ = 0;
    }
    else{
        accelZ = float(rawData)/accelScale;
    }
    // ---------------------------------------

    // -------- Get Temperature Data ---------
    rawData = read16BitRegister(MPU_TEMP1, MPU_TEMP2, readError);
    if(readError){
        std::cout << std::endl << "Error accessing Temperature data." << std::endl;
        temperature = 0;
    }
    else{
        temperature = float(rawData)/320 + 36.53; // Convert temperature to Celcius
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
// Function to initialize the MPU6050
void MPU6050::initialise(){
    __u8 deviceRegister; // The device register to access
    __s32 returnedData;  // The data returned by the device

    // Configure the MPU Power Mode
    deviceRegister = MPU_PWR_MGMT_1;
    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, MPU_PWR_MGMT_CLK_INTERNAL_8MHZ);
    if (returnedData < 0){
        std::cout << std::endl << "Error when setting the power register. Potential connectivity problem?" << std::endl;
        exit(I2C_SET_SLAVE_PWR_MODE);
    }

    // Configure Interrupts
//    deviceRegister = MPU_INT_ENABLE;
//    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, 1);
//    if (returnedData < 0){
//        std::cout << std::endl << "Error when setting up the interrupts. Potential connectivity problem?" << std::endl;
//        exit(I2C_SETUP_INTERRUPTS);
//    }

    // Configure the Gyroscope
    deviceRegister = MPU_GYRO_CONFIG;
    // Set the sensitivity. Value shifted to the right to correctly position it in the register
    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, MPU_GYRO_SENS_500 << 3);
    if (returnedData < 0){
        std::cout << std::endl << "Error when setting up the Gyro. Potential connectivity problem?" << std::endl;
        exit(I2C_SET_GYRO_RES);
    }
    gyroScale = MPU_GYRO_SCALE_500;


    // Configure the Accelerometer
    deviceRegister = MPU_ACC_CONFIG;
    // Set the sensitivity. Value shifted to the right to correctly position it in the register
    returnedData = i2c_smbus_write_byte_data(i2cHandle, deviceRegister, MPU_ACC_SENS_2 << 3);
    if (returnedData < 0){
        std::cout << std::endl << "Error when setting up the Accelerometer. Potential connectivity problem?" << std::endl;
        exit(I2C_SET_ACCEL_RES);
    }
    accelScale = MPU_ACC_SCALE_2;
}

// Function to read to read an entire 16-bit register from the MPU6050
int16_t MPU6050::read16BitRegister(__u8 MSBRegister, __u8 LSBRegister, bool &readError){
    __u32 MSB, LSB; // Variables to store the returned Most Significant Byte and Least Significant Byte
    MSB = i2c_smbus_read_word_data(i2cHandle, MSBRegister); // Read the Most Significant Byte from the register
    LSB = i2c_smbus_read_word_data(i2cHandle, LSBRegister); // Read the Least Significant Byte from the register
	if(MSB < 0 || LSB < 0){
		// If either byte is less than 0, there was a read error
		readError = true; // Set the error variable to display the error message relevant to which register is being accessed
		return 0;         // Return 0 - avoid pointless operations when the data is probably junk anyway
	}
	else{
		readError = false; // Make sure the error flag is set to false
    }
	return int16_t(MSB << 8 | LSB); // Combine the bytes into a 16-bit signed integer
}
// --------------------------------------------------------------------------------------------

// ---------------------------------- Data Display Function -----------------------------------
std::ostream& operator<<(std::ostream& out, MPU6050& M){
    out << std::endl;
    out << "-------------------------------------" << std::endl;
    out << "----- Basic Info -----" << std::endl;
    out << "I2C Address: 0x" << std::hex << M.address << std::dec << std::endl; // This now outputs the address in hex to make it more clear
    out << "I2C Interface: " << M.fileName << std::endl;
    out << std::endl;
    out << "---- Gyro Values -----" << std::endl;
    out << "GyroX: " << M.gyroX << std::endl;
    out << "GyroY: " << M.gyroY << std::endl;
    out << "GyroZ: " << M.gyroZ << std::endl;
    out << std::endl;
    out << "---- Accel Values ----" << std::endl;
    out << "AccelX: " << M.accelX << std::endl;
    out << "AccelY: " << M.accelY << std::endl;
    out << "AccelZ: " << M.accelZ << std::endl;
    out << std::endl;
    out << "Temp: " << M.temperature << std::endl;
    out << "-------------------------------------" << std::endl;
    return out;
}
// --------------------------------------------------------------------------------------------
