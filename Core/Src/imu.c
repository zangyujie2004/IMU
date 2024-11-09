//
// Created by Administrator on 24-11-2.
//

#include "gpio.h"
#include "spi.h"
#include "main.h"
#include "imu.h"

#define pi 3.1416
extern int16_t accelData[3];
extern float accelValue[3];
extern int16_t gyroData[3];
extern float gyroValue[3];

// configure
void BMI088_ACCEL_NS_L() {
    HAL_GPIO_WritePin(Acc_GPIO_Port, Acc_Pin, GPIO_PIN_RESET);
}
void BMI088_ACCEL_NS_H() {
    HAL_GPIO_WritePin(Acc_GPIO_Port, Acc_Pin, GPIO_PIN_SET);
}
void BMI088_GYRO_NS_L() {
    HAL_GPIO_WritePin(Gyro_GPIO_Port, Gyro_Pin, GPIO_PIN_RESET);
}
void BMI088_GYRO_NS_H() {
    HAL_GPIO_WritePin(Gyro_GPIO_Port, Gyro_Pin, GPIO_PIN_SET);
}

// read and write reg
void BMI088_ReadReg_ACCEL(uint8_t reg, uint8_t *return_data, uint8_t length) {
    BMI088_ACCEL_NS_L();
    reg = reg | 0x80;
    HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    HAL_SPI_Receive(&hspi1, return_data, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX);
    HAL_SPI_Receive(&hspi1, return_data, length, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX);
    BMI088_ACCEL_NS_H();
}
void BMI088_ReadReg_GYRO(uint8_t reg, uint8_t *return_data, uint8_t length) {
    BMI088_GYRO_NS_L();
    reg = reg | 0x80;
    HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    HAL_SPI_Receive(&hspi1, return_data, length, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX);
    BMI088_GYRO_NS_H();
}
void BMI088_WriteReg(uint8_t reg, uint8_t write_data) {
    BMI088_ACCEL_NS_L();
    reg = reg | 0x00;
    HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    HAL_SPI_Transmit(&hspi1, &write_data, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    BMI088_ACCEL_NS_H();
}

// initiate BMI088
void BMI088_Init() {
    // Soft Reset ACCEL
    BMI088_ACCEL_NS_L();
    BMI088_WriteReg(0x7E, 0xB6); // Write 0xB6 to ACC_SOFTRESET(0x7E)
    HAL_Delay(1);
    BMI088_ACCEL_NS_H();

    // Soft Reset GYRO
    BMI088_GYRO_NS_L();
    BMI088_WriteReg(0x14, 0xB6); // Write 0xB6 to GYRO_SOFTRESET(0x14)
    HAL_Delay(30);
    BMI088_GYRO_NS_H();

    // Switch ACCEL to Normal Mode
    BMI088_ACCEL_NS_L();
    HAL_Delay(1);
    BMI088_WriteReg(0x7D, 0x04); // Write 0x04 to ACC_PWR_CTRL(0x7D)
    HAL_Delay(1);
    BMI088_ACCEL_NS_H();
}

//Task: read reg and return
void BMI088_UserInit(uint8_t *return_data, uint8_t *range)
{
    BMI088_ReadReg_GYRO(0x00, return_data, 1);
    BMI088_WriteReg(0x41, 0x02);
    HAL_Delay(10);
    BMI088_ReadReg_ACCEL(0x41, range, 1);
}

//read and combine data
void BMI088_ReadAccelData(int16_t *accelData, float *accelValue) {
    uint8_t rawData[6];
    BMI088_ReadReg_ACCEL(0x12, rawData, 6);

    accelData[0] = (rawData[1] << 8) | rawData[0];
    accelData[1] = (rawData[3] << 8) | rawData[2];
    accelData[2] = (rawData[5] << 8) | rawData[4];

    uint8_t rangeReg;
    BMI088_ReadReg_ACCEL(0x41, &rangeReg, 1);
    float scaleFactor = 0.0f;
    switch (rangeReg) {
    case 0x00: scaleFactor = 3.0f / 32768.0f; break;
    case 0x01: scaleFactor = 6.0f / 32768.0f; break;
    case 0x02: scaleFactor = 12.0f / 32768.0f; break;
    case 0x03: scaleFactor = 24.0f / 32768.0f; break;
    }

    accelValue[0] = accelData[0] * scaleFactor;
    accelValue[1] = accelData[1] * scaleFactor;
    accelValue[2] = accelData[2] * scaleFactor;
}
void BMI088_ReadGyroData(int16_t *gyroData, float *gyroValue) {
    uint8_t rawData[6];
    BMI088_ReadReg_GYRO(0x02, rawData, 6);

    gyroData[0] = (rawData[1] << 8) | rawData[0];
    gyroData[1] = (rawData[3] << 8) | rawData[2];
    gyroData[2] = (rawData[5] << 8) | rawData[4];

    float gyroScaleFactor = 2000.0f / 32768.0f * pi / 180; //Convert to radian system
    gyroValue[0] = gyroData[0] * gyroScaleFactor;
    gyroValue[1] = gyroData[1] * gyroScaleFactor;
    gyroValue[2] = gyroData[2] * gyroScaleFactor;
}
