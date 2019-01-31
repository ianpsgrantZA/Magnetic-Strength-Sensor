//Code by Ian Grant
//2019/01/19
//compile with "gcc -o connect i2cConnect.c -lwiringPi"
// use "./connect to run"

#include <stdio.h>
#include <wiringPi.h>
#include <time.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdint.h>
#include "i2cReadlib.h"
#include <sys/timeb.h>
#include <math.h>

#define SEN_DEV_ID 0x62
#define MAG_DEV_ID 0x30
#define BIT_SET(val, num) (((val) >> (num)) & 1)

int SenFd = 0;
int MagFd = 0;
FILE *fData;
int timeCount = 0;
struct timeb start, end;


static inline void Tstart()
{
    ftime(&start);
}

static inline int Tstop()
{
    ftime(&end);
    int diff = (short)(int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
    return diff;
}
//Get Epoch time in 10ths of seconds
long long getTime(){
    struct timeval tv;
    int i =100;
    gettimeofday(&tv, NULL);

    long long millisecondsSinceEpoch =
    (unsigned long long)(tv.tv_sec) * 1000 +
    (unsigned long long)(tv.tv_usec) / 1000; 
    //printf("%llu\n", millisecondsSinceEpoch);

    return millisecondsSinceEpoch/i;
    
}
//Wiring Methods
int setRegister8(int fd, int reg, int value)
{
    return wiringPiI2CWriteReg8(fd, reg, value);
}
int setRegister16(int fd, int reg, int value)
{
    return wiringPiI2CWriteReg16(SenFd, reg, value);
}
int getRegister8(int fd, int reg)
{
    return wiringPiI2CReadReg8(fd, reg);
}
int getRegister16(int fd, int reg)
{
    return wiringPiI2CReadReg16(fd, reg);
}
int orRegister8(int fd, int reg, int value)
{
    return setRegister8(fd, reg, getRegister8(fd, reg) | value);
}
int orRegister16(int fd, int reg, int value)
{
    return setRegister16(fd, reg, getRegister16(fd, reg) | value);
}
//Connect to sensor
int senConnect()
{
    SenFd = wiringPiI2CSetup(SEN_DEV_ID);
    int status = getRegister8(SenFd, 0x01);
    if (status == -1)
    {
        printf("Error: no device on Address: 0x%x\n", SEN_DEV_ID);
        return 0;
    }
    printf("Connected successfully to Sensor with status: %x\n", status);
    setRegister8(SenFd, 0x00, 0x00); //reset
    return 1;
}
//Conect to magnetometer
int magConnect()
{
    MagFd = wiringPiI2CSetup(MAG_DEV_ID);
    int ID = getRegister8(MagFd, 0x2f);
    if (ID == -1)
    {
        printf("Error: no device on Address: 0x%x\n", MAG_DEV_ID);
        return 0;
    }
    else
    {
        printf("Connected successfully to Magnetometer with ID: %i\n", ID);
    }
    setRegister8(MagFd, 0x09, 0x80); //reset
    return 1;
}
//Write sensor and magnetometer values to data.txt (apppend)
void readSenMag()
{
    fData = fopen("data.txt", "a");
    setRegister8(SenFd, 0x00, 0x04); //take a Sen reading
    setRegister8(MagFd, 0x08, 1);    //take a Mag reading
    while ((BIT_SET(getRegister8(SenFd, 0x01), 0)) && !(getRegister8(MagFd, 0x07) & 1))
    {
        delay(1);
    }
    while (BIT_SET(getRegister8(SenFd, 0x01), 0))
    {
        delay(1);
    }
    while (!(getRegister8(MagFd, 0x07) & 1))
    {
    }
    
    //printf("%llx,%x,%x,%x,%x\n", getTime(), MMC5883MA_getX(MagFd), MMC5883MA_getY(MagFd), MMC5883MA_getZ(MagFd), getRegister8(SenFd, 0x10)|(getRegister8(SenFd, 0x0f)<<8));
    fprintf(fData, "%llx,%x,%x,%x,%x\n", getTime(), MMC5883MA_getX(MagFd), MMC5883MA_getY(MagFd), MMC5883MA_getZ(MagFd), getRegister8(SenFd, 0x10)|(getRegister8(SenFd, 0x0f)<<8));

    fclose(fData);
}
//Write sensor values to data.txt
void readSen()
{
    fData = fopen("data.txt", "a");
    setRegister8(SenFd, 0x00, 0x04); //take a Sen reading
    while (BIT_SET(getRegister8(SenFd, 0x01), 0))
    {
        delay(1);
    }
    //printf("%llx,0,0,0,%x\n", getTime(), getRegister8(SenFd, 0x10)|(getRegister8(SenFd, 0x0f)<<8));
    fprintf(fData, "%llx,0,0,0,%x\n", getTime(), getRegister8(SenFd, 0x10)|(getRegister8(SenFd, 0x0f)<<8));

    fclose(fData);
}
//Write magnetometer values to data.txt
void readMag()
{
    fData = fopen("data.txt", "a");
    setRegister8(MagFd, 0x08, 1); //take a Mag reading
    while (!(getRegister8(MagFd, 0x07) & 1))
    {
    }
    fprintf(fData, "%llx,%x,%x,%x,0\n", getTime(), MMC5883MA_getX(MagFd), MMC5883MA_getY(MagFd), MMC5883MA_getZ(MagFd));
    //printf("%llx,%x,%x,%x,0\n", getTime(), MMC5883MA_getX(MagFd), MMC5883MA_getY(MagFd), MMC5883MA_getZ(MagFd));

    fclose(fData);
}

int main()
{
    
    int mag = magConnect(); //connect to magnetometer
    int sen = senConnect(); //connect to sensor

    fData = fopen("data.txt", "a");
    fprintf(fData,"\n");
    fclose(fData);

    int diff = 2000;
    delay(3000);
    printf("Recording will start in 3 seconds\n");
    delay(3000);
    printf("Readings being recorded to data.txt ...\n");

    
    
    for (int i = 0; i < 100000000; i++)
    {
        if (sen && mag)     //if sensor and magnetometer are fuctional
        {
            while(diff<=100){diff = Tstop();}   //wait 100ms
            Tstart();

            readSenMag();

            diff = Tstop();
            //printf("%i\n", Tstop());
        }
        else if (sen)       //if only sensor is fuctional
        {
            while(diff<=100){diff = Tstop();}
            Tstart();

            readSen();

            diff = Tstop();
            //printf("%i\n", Tstop());
        }
        else if (mag)
        {
            while(diff<=100){diff = Tstop();}
            Tstart();

            readMag();

            diff = Tstop();
            //printf("%i\n", Tstop());
            
        }
        else
        {
            exit(0);
        }
    }
}
