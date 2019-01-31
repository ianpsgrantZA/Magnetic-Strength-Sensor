#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdint.h>

/*      Interact with Sensor Registers      */

//get individual axis values
unsigned short MMC5883MA_getX(int fd)
{
    return wiringPiI2CReadReg16(fd, 0x00);
}
unsigned short MMC5883MA_getY(int fd)
{
    return wiringPiI2CReadReg16(fd, 0x02);
}
unsigned short MMC5883MA_getZ(int fd)
{
    return wiringPiI2CReadReg16(fd, 0x04);
}
//get temperature
int MMC5883MA_getTemp(int fd)
{
    return wiringPiI2CReadReg8(fd, 0x06);
}
//get and set status
int MMC5883MA_getStatus(int fd)
{
    return wiringPiI2CReadReg8(fd, 0x07);
}
int MMC5883MA_setStatus(int fd, char status)
{
    wiringPiI2CWriteReg8(fd, 0x07, status);
}
//get and set control registers
void MMC5883MA_setControl0(int fd, char control)
{
    wiringPiI2CWriteReg8(fd, 0x08, control);
}
void MMC5883MA_setControl1(int fd, char control)
{
    wiringPiI2CWriteReg8(fd, 0x09, control);
}
void MMC5883MA_setControl2(int fd, char control)
{
    wiringPiI2CWriteReg8(fd, 0x0A, control);
}
int MMC5883MA_getControl0(int fd)
{
    return wiringPiI2CReadReg8(fd, 0x08);
}
int MMC5883MA_getControl1(int fd)
{
    return wiringPiI2CReadReg8(fd, 0x09);
}
int MMC5883MA_getControl2(int fd)
{
    return wiringPiI2CReadReg8(fd, 0x0A);
}
//get and set thresholds
void MMC5883MA_setXThreshold(int fd, char X)
{
    wiringPiI2CWriteReg8(fd, 0x0B, X);
}
void MMC5883MA_setYThreshold(int fd, char Y)
{
    wiringPiI2CWriteReg8(fd, 0x0C, Y);
}
void MMC5883MA_setZThreshold(int fd, char Z)
{
    wiringPiI2CWriteReg8(fd, 0x0D, Z);
}
//get product ID
int MMC5883MA_getID(int fd)
{
    return wiringPiI2CReadReg8(fd, 0x2F);
}
