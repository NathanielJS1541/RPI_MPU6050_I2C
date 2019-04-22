/* ============================================================================================
 * MPU6050 Interface for Raspberry Pi - Example Code
 * ============================================================================================
 * Written by Nathaniel Struselis & James Clarke.
 * Last Update: 05/04/2019
 * --------------------------------------------------------------------------------------------
 * This main demonstrates how the program operates and how to create/use the MPU6050 class.
 * --------------------------------------------------------------------------------------------
 */

#include <iostream>
#include "MPU6050.h"

using namespace std;

int main()
{
    MPU6050 IMU;      // Create the MPU6050 object
    IMU.updateData(); // Get data from the IMU
    cout << IMU;      // Display all of the data about this object
    for(int i=0; i<100; i++) {
        IMU.updateData(); // Get data from the IMU to refresh it
        cout << IMU;
    }
    return CLEAN_EXIT;
}