#ifndef LSM6DSL_LIBRARY_H
#define LSM6DSL_LIBRARY_H

#include <Wire.h>
#include <stdint.h>
#include "LSM6DSL_Constants.h"

/**
 * LSM6DSL status enum
 */
typedef enum {
	IMU_SUCCESS,
	IMU_HW_ERROR,
	IMU_NOT_SUPPORTED,
	IMU_GENERIC_ERROR,
	IMU_OUT_OF_BOUNDS,
	IMU_ALL_ONES_WARNING,
} lsm6dsl_status_t;

class LSM6DSLCore {
public:
    LSM6DSLCore(uint8_t addr);
    ~LSM6DSLCore() = default;

    lsm6dsl_status_t beginCore();
	lsm6dsl_status_t readRegister(uint8_t* output, uint8_t offset);
	lsm6dsl_status_t readRegisterRegion(uint8_t* output, uint8_t offset, uint8_t length);
	lsm6dsl_status_t readRegisterInt16(int16_t* output, uint8_t offsetL, uint8_t offsetM);
    lsm6dsl_status_t readRegisterInt16(int16_t* output, uint8_t offset);
    lsm6dsl_status_t writeRegister(uint8_t offset, uint8_t data);
    lsm6dsl_status_t embeddedPage();
	lsm6dsl_status_t basePage();

private:
    uint8_t i2cAddress;
};

struct SensorSettings {
    //Gyro settings
	uint8_t gyroEnabled;
	uint16_t gyroRange;
	uint16_t gyroSampleRate;

	uint8_t gyroFifoEnabled;
	uint8_t gyroFifoDecimation;

	//Accelerometer settings
	uint8_t accelEnabled;
	uint8_t accelODROff;
	uint16_t accelRange;
	uint16_t accelSampleRate;
	uint16_t accelBandWidth;
	
	uint8_t accelFifoEnabled;
	uint8_t accelFifoDecimation;
	
	//Temperature settings
	uint8_t tempEnabled;
	
	//Non-basic mode settings
	uint8_t commMode;
	
	//FIFO control data
	uint16_t fifoThreshold;
	int16_t fifoSampleRate;
	uint8_t fifoModeWord;
};

class LSM6DSL: public LSM6DSLCore {
public:
    SensorSettings settings;

    LSM6DSL(uint8_t address = 0x6B);
    ~LSM6DSL() = default;

	lsm6dsl_status_t begin();

	int16_t readRawTemperature();
	float readTemperatureC();
	float readTemperatureF();

	int16_t readRawAccelX();
	int16_t readRawAccelY();
	int16_t readRawAccelZ();

	float readFloatAccelX();
	float readFloatAccelY();
	float readFloatAccelZ();

	float convertAccel(int16_t axisValue);
};

#endif