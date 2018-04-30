#ifndef _LSM9DSO_H
#define _LSM9DSO_H

#include "types.h"

typedef struct _accel
{
    int16_t ax;
    int16_t ay;
    int16_t az;
} LSM_AccelerationData;

typedef struct _mag
{
    int16_t mx;
    int16_t my;
    int16_t mz;
} LSM_MagnetData;

typedef struct _gyr
{
    int16_t gx;
    int16_t gy;
    int16_t gz;
} LSM_GyroData;

typedef struct _temp
{
    int16_t temp;
} LSM_TemperatureData;


void LSM_Init(void);
void LSM_Tick(void);

uint8_t LSM_GetMagnetData(LSM_MagnetData *);
uint8_t LSM_GetAccelerationData(LSM_AccelerationData * acc);
uint8_t LSM_GetGyroData(LSM_GyroData *);
//uint8_t LSM_GetTemperatureData(LSM_MagnetData *); //TODO (maybe at some point)

#endif // _LSM9DSO_H
