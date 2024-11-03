//
// Created by Administrator on 24-11-2.
//

// imu.h
#ifndef IMU_H
#define IMU_H


void BMI088_Init();
void BMI088_ReadAccelData(int16_t *accelData, float *accelValue);
void BMI088_ReadGyroData(int16_t *gyroData, float *gyroValue);
void BMI088_WriteReg(uint8_t reg, uint8_t write_data);
void BMI088_ReadReg_GYRO(uint8_t reg, uint8_t *return_data, uint8_t length);
void BMI088_ReadReg_ACCEL(uint8_t reg, uint8_t *return_data, uint8_t length);
void BMI088_ACCEL_NS_L();
void BMI088_GYRO_NS_L();
void BMI088_ACCEL_NS_H();
void BMI088_GYRO_NS_H();

#endif // IMU_H
