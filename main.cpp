/* ============================================================================================
 * MPU6050 Interface for Raspberry Pi - Example Code
 * ============================================================================================
 * Written by Nathaniel Struselis
 * Last Update: 05/04/2019
 * --------------------------------------------------------------------------------------------
 * This main demonstrates how the program operates and how to create/use the MPU6050 class.
 * --------------------------------------------------------------------------------------------

#include <iostream>
#include "MPU6050.h"

using namespace std;

int main()
{
    MPU6050 IMU;
    IMU.updateData();
    cout << IMU;
    IMU.updateData();
    cout << IMU;
    IMU.updateData();
    cout << IMU;
    return 0;
}
