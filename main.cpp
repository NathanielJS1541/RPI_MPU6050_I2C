/* ============================================================================================
 * MPU6050 Interface for Raspberry Pi - Example Code
 * ============================================================================================
 * Written by Nathaniel Struselis & James Clarke.
 * Last Update: 24/04/2019
 * --------------------------------------------------------------------------------------------
 * This main demonstrates how the program operates and how to create/use the MPU6050 class.
 * --------------------------------------------------------------------------------------------
 */

#include <iostream>
#include "MPU6050.h"

using namespace std;

int main()
{
    MPU6050 IMU;      // Create the MPU6050 object - this line must be changed on revision 0 Pis, or for MPU6050 modules not on address 0x68
    IMU.updateData(); // Get data from the IMU
    cout << IMU;      // Display all of the data about this object
    IMU.updateData(); // Get data from the IMU to refresh it
    cout << IMU;      // Display the refreshed data

    // Example using constructor with more configurable parameters
    MPU6050 customIMU(MPU_PWR_MGMT_CLK_INTERNAL_8MHZ, MPU_GYRO_SENS_250, MPU_ACC_SENS_2, MPU_DEFAULT_I2C_ADDR, false);
    customIMU.updateData(); // Update the new IMU
    cout << customIMU;      // Print the data
    customIMU.updateData(); // Update the new IMU
    cout << customIMU;      // Print the data

    return CLEAN_EXIT;
}
