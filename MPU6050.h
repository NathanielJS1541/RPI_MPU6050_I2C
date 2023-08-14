/* ============================================================================================
 * MPU6050 Interface Header for Raspberry Pi
 * ============================================================================================
 * Written by Nathaniel Struselis & James Clarke.
 * Last Update: 23/04/2019
 * --------------------------------------------------------------------------------------------
 * This header declares the functions and defines the functions used to access data on the
 * MPU6050 IMU. The registers are based off the addresses found at invensense:
 * https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf. This is
 * based on the Pi2c library from https://github.com/JohnnySheppard/Pi2c.
 * Additional I2C documentation was found at
 * https://www.kernel.org/doc/Documentation/i2c/dev-interface.
 * --------------------------------------------------------------------------------------------
 */

#include <iostream> // Used for the display function

// Used for the I2C interface
#include <linux/i2c-dev.h> // For SMBus commands
#include <sys/ioctl.h>     // For ioctl()
#include <fcntl.h>         // For O_RDWR
#include <unistd.h>        // For open()

#ifndef MPU6050_H
#define MPU6050_H

// Exit codes
#define CLEAN_EXIT             0
#define I2C_BUS_INIT_ERROR     1
#define I2C_SET_SLAVE_ADDR_ERR 2
#define I2C_SET_SLAVE_PWR_MODE 3
#define I2C_SET_GYRO_RES       4
#define I2C_SET_ACCEL_RES      5
#define I2C_SETUP_INTERRUPTS   6
#define MPU_INIT_PARAM_ERROR   7

// ---------- Basic Config Parameters ----------
// Address used to access data
#define MPU_DEFAULT_I2C_ADDR 0x68

// Interrupt enable register
#define MPU_INT_ENABLE 0x38 // Register as follows: {-, -, -, FIFO_OFLOW_EN, I2C_MST_INT_EN, -, -, DATA_RDY_EN}

// Interrupt status register
#define MPU_INT_STATUS 0x3A // Register as follows: {-, -, -, FIFO_OFLOW_INT, I2C_MST_INT, -, -, DATA_RDY_INT}

// MPU configuration register
#define MPU_CONFIG 0x1A // Register as follows: {-, -, EXT_SYNC_SET[3 bits], DLPF_CONFIG[3 bits]}

// EXT_SYCN_SET parameters to set the FSYNC bit location
#define MPU_CONFIG_FSYNC_DISABLE   0
#define MPU_CONFIG_FSYNC_TEMP_OUT  1
#define MPU_CONFIG_FSYNC_GYRO_XOUT 2
#define MPU_CONFIG_FSYNC_GYRO_YOUT 3
#define MPU_CONFIG_FSYNC_GYRO_ZOUT 4
#define MPU_CONFIG_FSYNC_ACC_XOUT  5
#define MPU_CONFIG_FSYNC_ACC_YOUT  6
#define MPU_CONFIG_FSYNC_ACC_ZOUT  7

// DLPF_CONFIG parameters - for more information see the datasheet
                            // |        Accelerometer        |                Gyroscope               |
                            // |  Bandwidth/Hz  |  Delay/ms  |  Bandwidth/Hz  |  Delay/ms  |  Fs/kHz  |
#define MPU_CONFIG_DLPF_0 0 // |      260       |     0.0    |      256       |     0.98   |     8    |
#define MPU_CONFIG_DLPF_1 1 // |      184       |     2.0    |      188       |     1.9    |     1    |
#define MPU_CONFIG_DLPF_2 2 // |       94       |     3.0    |       98       |     2.8    |     1    |
#define MPU_CONFIG_DLPF_3 3 // |       44       |     4.9    |       42       |     4.8    |     1    |
#define MPU_CONFIG_DLPF_4 4 // |       21       |     8.5    |       20       |     8.3    |     1    |
#define MPU_CONFIG_DLPF_5 5 // |       10       |    13.8    |       10       |    13.4    |     1    |
#define MPU_CONFIG_DLPF_6 6 // |        5       |    19.0    |        5       |    18.6    |     1    |
#define MPU_CONFIG_DLPF_7 7 // |           RESERVED          |           RESERVED          |     8    |
// ---------------------------------------------

// ------------ Gyroscope Parameters -----------
// Gyro configuration register
#define MPU_GYRO_CONFIG 0x1B // Register as follows: {XG_ST, YG_ST, ZG_ST, FS_SEL[2 bits], -, -, -}

// FS_SEL parameters - Gyro sensitivity parameters
#define MPU_GYRO_SENS_250  0 // ± 250 °/s
#define MPU_GYRO_SENS_500  1 // ± 500 °/s
#define MPU_GYRO_SENS_1000 2 // ± 1000 °/s
#define MPU_GYRO_SENS_2000 3 // ± 2000 °/s

// Gyro scaling parameters - based on sensitivity above
#define MPU_GYRO_SCALE_250  131  // for ± 250 °/s
#define MPU_GYRO_SCALE_500  65.5 // for ± 500 °/s
#define MPU_GYRO_SCALE_1000 32.8 // for ± 1000 °/s
#define MPU_GYRO_SCALE_2000 16.4 // for ± 2000 °/s

// Gyro reading registers - Each register is 8 bits
#define MPU_GYRO_X1 0x43 // Most significant byte
#define MPU_GYRO_X2 0x44 // Least significant byte
#define MPU_GYRO_Y1 0x45
#define MPU_GYRO_Y2 0x46
#define MPU_GYRO_Z1 0x47
#define MPU_GYRO_Z2 0x48
// ---------------------------------------------

// ---------- Accelerometer Parameters ---------
// Accelerometer configuration register
#define MPU_ACC_CONFIG 0x1C // Register as follows: {XA_ST, YA_ST, ZA_ST, AFS_SEL[2 bits], -, -, -}

// AFS_SEL parameters - Accelerometer sensitivity parameters
#define MPU_ACC_SENS_2  0 // ±2g
#define MPU_ACC_SENS_4  1 // ±4g
#define MPU_ACC_SENS_8  2 // ±8g
#define MPU_ACC_SENS_16 3 // ±16g

// Accelerometer scaling parameters - based on sensitivity above
#define MPU_ACC_SCALE_2  16384 // for ±2g
#define MPU_ACC_SCALE_4  8192  // for ±4g
#define MPU_ACC_SCALE_8  4096  // for ±8g
#define MPU_ACC_SCALE_16 2048  // for ±16g

// Accelerometer reading registers - Each register is 8 bits
#define MPU_ACC_X1 0x3B // Most significant byte
#define MPU_ACC_X2 0x3C // Least significant byte
#define MPU_ACC_Y1 0x3D
#define MPU_ACC_Y2 0x3E
#define MPU_ACC_Z1 0x3F
#define MPU_ACC_Z2 0x40
// ---------------------------------------------

// ------- Temperature Sensor Parameters -------
// Temperature reading registers
#define MPU_TEMP1 0x41 // Most significant byte
#define MPU_TEMP2 0x42 // Least significant byte

// Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
// ---------------------------------------------

// -------------- Power Registers --------------
// Power management registers
#define MPU_PWR_MGMT_1 0x6B // Register is as follows: {DEVICE_REST, SLEEP, CYCLE, -, TEMP_DISABLE, CLK_SEL[3 bits]}
#define MPU_PWR_MGMT_2 0x6C // Register is as follows: {LP_WAKE_CTRL[2 bits], STBY_XA, STBY_YA, STBY_ZA, STBY_XG, STBY_YG, STBY_ZG}

// Clock configuration values
#define MPU_PWR_MGMT_CLK_INTERNAL_8MHZ 0
#define MPU_PWR_MGMT_CLK_PLL_X_GYRO    1
#define MPU_PWR_MGMT_CLK_PLL_Y_GYRO    2
#define MPU_PWR_MGMT_CLK_PLL_Z_GYRO    3
#define MPU_PWR_MGMT_CLK_EXT_32_768KHZ 4
#define MPU_PWR_MGMT_CLK_EXT_19_2MHZ   5
#define MPU_PWR_MGMT_CLK_STOP          7

// Wakeup data frequency configuration values
#define MPU_PWR_MGMT_WAKE_1_25HZ 0
#define MPU_PWR_MGMT_WAKE_5HZ    1
#define MPU_PWR_MGMT_WAKE_20HZ   2
#define MPU_PWR_MGMT_WAKE_40HZ   3

// ---------------------------------------------

// Declare a class to process and store the data
class MPU6050{
public:
	// ---------- Special class members -----------
    MPU6050();                                         // Default constructor - used for making arrays of the object
	MPU6050(bool isPiRev0);                            // Default constructor with compatibility for rev0 Pis
	MPU6050(int deviceAddress, bool isPiRev0 = false); // Constructor with additional parameters to set the address and if the Pi is rev0
	// Constructor to allow customization of basic configuration parameters
	MPU6050(int pwrMgmtMode, int gyroConfig, int accelConfig, int deviceAddress = MPU_DEFAULT_I2C_ADDR, bool isPiRev0 = false);
	MPU6050(const MPU6050& M);                         // Copy constructor
	~MPU6050();                                        // Destructor
	// --------------------------------------------

	// ----------- Operator overloading -----------
	MPU6050& operator=(const MPU6050& M); // Assignment operator
	// --------------------------------------------

	// ------ MPU Configuration Functions ---------
	void reconfigure(int pwrMgmtMode, int gyroConfig = MPU_GYRO_SENS_500, int accelConfig = MPU_ACC_SENS_2);
	void sleep();
	void wake();
	void disableTemp();
	void enableTemp();
	void disableGyro();
	void enableGyro();
	void disableAccel();
	void enableAccel();
    // --------------------------------------------

	// ---------- Data Access Functions -----------
	void updateData();
	float getGyroX();
	float getGyroY();
	float getGyroZ();
	float getAccelX();
	float getAccelY();
	float getAccelZ();
	float getTemp();
	// --------------------------------------------

	// ----------- Data Output Function -----------
	friend std::ostream& operator<<(std::ostream& out, MPU6050& M);
	// --------------------------------------------

private:
	// I2C address of the device
	int address;

	// I2C Variables
	char fileName[11];
	int i2cHandle;

	// Functions to initialise the MPU6050 - should only be called once
	void defaultInitialise();
	void initialise(int pwrMgmtMode, int gyroConfig, int accelConfig);

	// Function to read an entire 16-bit register from the MPU6050
	int16_t read16BitRegister(__u8 MSBRegister, __u8 LSBRegister, bool &readError);

	// Gyroscope values
	float gyroScale;
	float gyroX;
	float gyroY;
	float gyroZ;

	// Accelerometer values
	float accelScale;
	float accelX;
	float accelY;
	float accelZ;

	// Temperature value
	float temperature;
};

#endif
