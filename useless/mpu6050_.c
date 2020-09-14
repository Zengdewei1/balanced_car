/*
 *
 *
 *
 * */
#include "drv_i2c_.h"
#include <stdio.h>
#include <math.h>

//定义MPU6050内部地址
#define	SMPLRT_DIV							0x19	//陀螺仪采样率 典型值 0X07 125Hz
#define	DLPF_CONFIG							0x1A	//低通滤波频率 典型值 0x00 
#define	GYRO_CONFIG							0x1B	//陀螺仪自检及测量范围                 典型值 0x18 不自检 2000deg/s
#define	ACCEL_CONFIG						0x1C	//加速度计自检及测量范围及高通滤波频率 典型值 0x01 不自检 2G 5Hz
#define	PWR_MGMT_1							0x6B	//电源管理 典型值 0x00 正常启用
#define	WHO_AM_I		  					0x75	//只读  默认读出应该是 MPU6050_ID = 0x68
#define MPU6050_ID              0x68
#define MPU6050_DEVICE_ADDRESS  0xD0
#define MPU6050_DEVICE_ADDRESS1  0xD2

#define	ACCEL_XOUT_H						0x3B
#define	ACCEL_XOUT_L						0x3C
#define	ACCEL_YOUT_H						0x3D
#define	ACCEL_YOUT_L						0x3E
#define	ACCEL_ZOUT_H						0x3F
#define	ACCEL_ZOUT_L						0x40
#define	TEMP_OUT_H							0x41
#define	TEMP_OUT_L							0x42
#define	GYRO_XOUT_H							0x43
#define	GYRO_XOUT_L							0x44	
#define	GYRO_YOUT_H							0x45
#define	GYRO_YOUT_L							0x46
#define	GYRO_ZOUT_H							0x47
#define	GYRO_ZOUT_L							0x48

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;


unsigned char dev_id = 0x00;
u8 MPU_Set_Rate(u16 rate);

int mpu6050Init() {
    i2cRead(MPU6050_DEVICE_ADDRESS, 0x75, &dev_id, 1);


    i2cWrite(MPU6050_DEVICE_ADDRESS, PWR_MGMT_1, 0X80); // ??
    delay_ms(100);
    i2cWrite(MPU6050_DEVICE_ADDRESS, PWR_MGMT_1, 0x00); // ?????
    i2cWrite(MPU6050_DEVICE_ADDRESS, GYRO_CONFIG, 0x10); //+-1000dps
    i2cWrite(MPU6050_DEVICE_ADDRESS, ACCEL_CONFIG, 0x10); //+-8g

    if (dev_id == MPU6050_ID) {
        printf("Gyro Num 0 OJBK!\n");
        i2cWrite(MPU6050_DEVICE_ADDRESS, PWR_MGMT_1, 0x01); // ??CLKSEL,PLL X????   
        MPU_Set_Rate(50); //????50HZ
    }
    return 0;
}

void mpu6050GyroCalibration(short *offset) {
    unsigned short i = 0;
    unsigned char gyro_buf[6];

    short gyroBuf[3] = {0, 0, 0};
    int gyroOffsetBuf[3] = {0, 0, 0};


    for (i = 0; i < 1000; i++) {
        delay_ms(2);
        i2cRead(MPU6050_DEVICE_ADDRESS, GYRO_XOUT_H, gyroBuf, 6);

        gyroBuf[0] = (gyro_buf[0] << 8 | gyro_buf[1]);
        gyroBuf[1] = (gyro_buf[2] << 8 | gyro_buf[3]);
        gyroBuf[2] = (gyro_buf[4] << 8 | gyro_buf[5]);

        gyroOffsetBuf[0] += gyroBuf[0];
        gyroOffsetBuf[1] += gyroBuf[1];
        gyroOffsetBuf[2] += gyroBuf[2];
    }

    offset[0] = (gyroOffsetBuf[0] / 1000);
    offset[1] = (gyroOffsetBuf[1] / 1000);
    offset[2] = (gyroOffsetBuf[2] / 1000);
}

void mpu6050AccelCalibration(short *offset) {
    unsigned short i = 0;
    unsigned char accel_buf[6] = {0};
    short accelBuf[3] = {0, 0, 0};
    int accelOffsetBuf[3] = {0, 0, 0};

    for (i = 0; i < 1000; i++) {
        delay_ms(2);
        i2cRead(MPU6050_DEVICE_ADDRESS, ACCEL_XOUT_H, accel_buf, 6);


        accelBuf[0] = (accel_buf[0] << 8 | accel_buf[1]);
        accelBuf[1] = (accel_buf[2] << 8 | accel_buf[3]);
        accelBuf[2] = (accel_buf[4] << 8 | accel_buf[5]);

        accelOffsetBuf[0] += accelBuf[0];
        accelOffsetBuf[1] += accelBuf[1];
        accelOffsetBuf[2] += accelBuf[2];

    }

    offset[0] = (short) (accelOffsetBuf[0] / 1000);
    offset[1] = (short) (accelOffsetBuf[1] / 1000);
    offset[2] = (short) (accelOffsetBuf[2] / 1000);
}


/*****************************************************************************/
static const float mpu6050GyroFactors[] = {
    1.3323124e-4f, // Range = +/- 250 dps  (131.0 LSBs/DPS)
    2.6646248e-4f, // Range = +/- 500 dps  (65.5 LSBs/DPS)
    5.3211258e-4f, // Range = +/- 1000 dps (32.8 LSBs/DPS)
    0.0010642252f // Range = +/- 2000 dps (16.4 LSBs/DPS)
};

static const float mpu6050AccelFactors[] = {
    0.00059875, // Range = +/- 2 g (16384 lsb/g)
    0.00119751, // Range = +/- 4 g (8192 lsb/g)
    0.00239502, // Range = +/- 8 g (4096 lsb/g)
    0.00479004 // Range = +/- 16 g (2048 lsb/g)
};

/*****************************************************************************/

void mpu6050GyroDataRead(float* gyroData, short *offset) {
    unsigned char gyro_buf[6];
    short tempGyro[3];

    i2cRead(MPU6050_DEVICE_ADDRESS, GYRO_XOUT_H, gyro_buf, 6);


    tempGyro[0] = (gyro_buf[0] << 8 | gyro_buf[1]);
    tempGyro[1] = (gyro_buf[2] << 8 | gyro_buf[3]);
    tempGyro[2] = (gyro_buf[4] << 8 | gyro_buf[5]);

    gyroData[0] = (float) (tempGyro[0] - offset[0]) * mpu6050GyroFactors[2]; //见datasheet 32 of 47
    gyroData[1] = (float) (tempGyro[1] - offset[1]) * mpu6050GyroFactors[2]; //见datasheet 32 of 47
    gyroData[2] = (float) (tempGyro[2] - offset[2]) * mpu6050GyroFactors[2]; //见datasheet 32 of 47

}

void mpu6050AccelDataRead(float* accelData, short *offset) {
    unsigned char accel_buf[6];

    short tempAccel[3];
    i2cRead(MPU6050_DEVICE_ADDRESS, ACCEL_XOUT_H, accel_buf, 6);
    tempAccel[0] = (accel_buf[0] << 8 | accel_buf[1]);
    tempAccel[1] = (accel_buf[2] << 8 | accel_buf[3]);
    tempAccel[2] = (accel_buf[4] << 8 | accel_buf[5]);

    //将原生数据转换为实际值，计算公式跟寄存器的配置有关
    accelData[0] = (float) (tempAccel[0] - offset[0]) * mpu6050AccelFactors[2]; //见datasheet 30 of 47
    accelData[1] = (float) (tempAccel[1] - offset[1]) * mpu6050AccelFactors[2]; //见datasheet 30 of 47
    accelData[2] = (float) (tempAccel[2] - offset[2]) * mpu6050AccelFactors[2]; //见datasheet 30 of 47
}

#define Kp 100.0f                        // ??????????????/???
#define Ki 0.002f                // ????????????????
#define halfT 0.001f                // ???????

float q0 = 1, q1 = 0, q2 = 0, q3 = 0; // ?????????????
float exInt = 0, eyInt = 0, ezInt = 0; // ?????????

void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az, float *Pitch, float *Roll) {
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    // ?????
    norm = sqrt(ax * ax + ay * ay + az * az);
    ax = ax / norm; //???
    ay = ay / norm;
    az = az / norm;
    // ???????
    vx = 2 * (q1 * q3 - q0 * q2);
    vy = 2 * (q0 * q1 + q2 * q3);
    vz = q0 * q0 - q1 * q1 - q2 * q2 + q3*q3;
    // ???????????????????????????
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);
    // ??????????
    exInt = exInt + ex*Ki;
    eyInt = eyInt + ey*Ki;
    ezInt = ezInt + ez*Ki;
    // ?????????
    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;
    // ??????????
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;
    // ?????
    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;
    *Pitch = asin(-2 * q1 * q3 + 2 * q0 * q2)* 57.3; // pitch ,?????
    *Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1)* 57.3; // rollv
    //*Yaw = atan2(2 * (q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;                //?????????
}

void rateCalculate(float* accelData, float* rate, float t) {
    int i;
    for (i = 0; i < 3; i++) {
        rate[i] += accelData[i] * t;
    }
}

void posCalculate(float* accelData, float* rate, float t, float *pos) {
    int i;
    for (i = 0; i < 3; i++) {
        pos[i] += rate[i] * t + 0.5 * accelData[i] * t*t;
    }
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 

u8 MPU_Set_Gyro_Fsr(u8 fsr) {
    i2cWrite(MPU6050_DEVICE_ADDRESS, GYRO_CONFIG, fsr << 3);
    return 0; //+-4g
}
//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 

u8 MPU_Set_Accel_Fsr(u8 fsr) {
    i2cWrite(MPU6050_DEVICE_ADDRESS, ACCEL_CONFIG, fsr << 3);
    return 0;
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 

u8 MPU_Set_LPF(u16 lpf) {
    u8 data = 0;
    if (lpf >= 188)data = 1;
    else if (lpf >= 98)data = 2;
    else if (lpf >= 42)data = 3;
    else if (lpf >= 20)data = 4;
    else if (lpf >= 10)data = 5;
    else data = 6;
    i2cWrite(MPU6050_DEVICE_ADDRESS, 0x1A, data);


    return 0; //+-4g

}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 

u8 MPU_Set_Rate(u16 rate) {
    u8 data;
    if (rate > 1000)rate = 1000;
    if (rate < 4)rate = 4;
    data = 1000 / rate - 1;
    i2cWrite(MPU6050_DEVICE_ADDRESS, SMPLRT_DIV, data);


    return MPU_Set_LPF(rate / 2); //????????????
}