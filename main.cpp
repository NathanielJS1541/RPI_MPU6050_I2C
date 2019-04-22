/* ============================================================================================
 * MPU6050 Interface for Raspberry Pi - Example Code
 * ============================================================================================
 * Written by Nathaniel Struselis & James Clarke.
 * Last Update: 21/04/2019
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

    return CLEAN_EXIT;
}
