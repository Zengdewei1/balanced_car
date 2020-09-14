#ifndef _MPU6050_H_
#define _MPU6050_H_

extern int mpu6050Init(void);
extern void mpu6050AccelCalibration(short *offset);
extern void mpu6050GyroCalibration(short *offset);
extern void mpu6050AccelDataRead(float* accelData, short *offset);
extern void mpu6050GyroDataRead(float* gyroData, short *offset);
extern void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az, float *Yaw, float *Pitch);
extern void rateCalculate(float* accelData, float* rate, float t);
extern void posCalculate(float* accelData, float* rate, float t, float* pos);
#endif
