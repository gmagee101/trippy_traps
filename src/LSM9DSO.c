#include "LSM9DSO.h"
#include "i2c.h"
#include "usb_debug_only.h"

#include <alloca.h>
#include <util/delay.h>

#define MULTIPLE_READ (1<<8)

//Angular Rate
#define GYRO_SAD_W 0xD6
#define GYRO_SAD_R 0xD7
#define CTRL_REG1_G 0x20
#define OUT_X_L_G 0x28

//Linear Acceleration & Magnetic
#define ACCMAG_SAD_W 0x3A
#define ACCMAG_SAD_R 0x3B
#define OUT_X_L_M 0x08
#define CTRL_REG1_XM 0x20
#define CTRL_REG5_XM 0x24 //This has temp sensor enable
#define OUT_X_L_A 0x28

typedef enum
{
    LSM_STATE_ACC,
    LSM_STATE_GYRO,
    LSM_STATE_MAG,
    LSM_STATE_UNINIT,
} LSM_STATE;

typedef enum
{
    ACC_IDX = 0,
    MAG_IDX,
    GYR_IDX,
    NUM_IDX
} LSM_IDX;

static i2c_txn_t * txn;
static uint8_t xyz[NUM_IDX][6] = {0}; //mag, accel, gyro in 2d array (2s complement, 16b)
static LSM_STATE CurrentState = LSM_STATE_UNINIT;

void LSM_Init(void)
{
    uint8_t ctrl[2] = {0x20, 0x1F};
    i2c_init();
    _delay_ms(1000);
    txn = alloca(sizeof(*txn) + 1 * sizeof(txn->ops[0]));
    i2c_txn_init(txn, 1);
    //Initialize Linear Accelerometer
    i2c_op_init(&txn->ops[0], 0x3a, ctrl, 2);
    i2c_post(txn);
    while (!(txn->flags & I2C_TXN_DONE)) {
    }

    if (txn->flags & I2C_TXN_ERR)
    {
        usb_debug_putchar('E');
        usb_debug_putchar('1');
        return;
    }

#if 0
    ctrl[0] = CTRL_REG1_G;
    ctrl[1] = 0x0F;
    i2c_op_init(&txn->ops[0], GYRO_SAD_W, ctrl, sizeof(ctrl));
    i2c_post(txn);
    while (!(txn->flags & I2C_TXN_DONE)) {
    }

    if (txn->flags & I2C_TXN_ERR)
    {
        usb_debug_putchar('E');
        usb_debug_putchar('2');
        return;
    }
#endif

    CurrentState = LSM_STATE_ACC;
}

void LSM_Tick(void)
{
    txn = alloca(sizeof(*txn) + 2 * sizeof(txn->ops[0]));
    uint8_t sub; //sub address
    switch(CurrentState)
    {
        case LSM_STATE_ACC:
            sub = 0xA8;
            i2c_txn_init(txn, 2);
            i2c_op_init(&txn->ops[0], 0x3A, &sub, 1);
            i2c_op_init(&txn->ops[1], 0x3B, xyz[ACC_IDX], 6);
            i2c_post(txn);
            while (!(txn->flags & I2C_TXN_DONE)) {
            }

            if (txn->flags & I2C_TXN_ERR)
            {
                usb_debug_putchar('E');
                usb_debug_putchar('A');
            }
            CurrentState = LSM_STATE_GYRO;
            break;
        case LSM_STATE_GYRO:
            CurrentState = LSM_STATE_MAG;
            break;
        case LSM_STATE_MAG:
            CurrentState = LSM_STATE_ACC;
            break;
        case LSM_STATE_UNINIT:
            usb_debug_putchar('W');
            break;
    }
}

uint8_t LSM_GetMagnetData(LSM_MagnetData * mag)
{

    return 1;
}

uint8_t LSM_GetAccelerationData(LSM_AccelerationData * acc)
{
    if (acc && CurrentState != LSM_STATE_UNINIT)
    {
        (acc)->ax = ((uint16_t)(xyz[ACC_IDX][1])<<8) | xyz[ACC_IDX][0];
        (acc)->ay = ((uint16_t)(xyz[ACC_IDX][3])<<8) | xyz[ACC_IDX][2];
        (acc)->az = ((uint16_t)(xyz[ACC_IDX][5])<<8) | xyz[ACC_IDX][4];
        return 0;
    }
    return 1;
}

uint8_t LSM_GetGyroData(LSM_GyroData *gyr)
{

    return 1;
}
