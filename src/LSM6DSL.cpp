#include "LSM6DSL.h"


LSM6DSLCore::LSM6DSLCore(uint8_t addr):
i2cAddress(addr) {
}

lsm6dsl_status_t LSM6DSLCore::beginCore() {
    lsm6dsl_status_t returnStatus = IMU_SUCCESS;
    Wire.begin();

    volatile uint8_t temp = 0;
    for (uint16_t i = 0; i < 10000; i++) {
        temp++;
    }

    uint8_t result;
    readRegister(&result, LSM6DSL_WHO_AM_I_REG);

    if (result != 0x6A) {
        returnStatus = IMU_HW_ERROR;
    }

    return returnStatus;
}

lsm6dsl_status_t LSM6DSLCore::readRegister(uint8_t* output, uint8_t offset) {
    uint8_t result = 0;
    uint8_t numBytes = 1;
    lsm6dsl_status_t returnStatus = IMU_SUCCESS;

    Wire.beginTransmission(i2cAddress);
    Wire.write(offset);

    if (Wire.endTransmission() != 0) {
        returnStatus = IMU_HW_ERROR;
    }

    Wire.requestFrom(i2cAddress, numBytes);
    while (Wire.available()) {
        result = Wire.read();
    }

    *output = result;
    return returnStatus;
}

lsm6dsl_status_t LSM6DSLCore::readRegisterRegion(uint8_t* output, uint8_t offset, uint8_t length) {
    lsm6dsl_status_t returnStatus = IMU_SUCCESS;
    uint8_t i = 0;
    uint8_t c = 0;

    Wire.beginTransmission(i2cAddress);
    Wire.write(offset);
    if (Wire.endTransmission() != 0) {
        returnStatus = IMU_HW_ERROR;
    } else {
        Wire.requestFrom(i2cAddress, length);
        while (Wire.available() && (i < length)) {
            c = Wire.read();
            *output = c;
            output++;
            i++;
        }
    }

    return returnStatus;
}

lsm6dsl_status_t LSM6DSLCore::readRegisterInt16(int16_t* output, uint8_t offset) {
    uint8_t buffer[2];
    lsm6dsl_status_t returnStatus = readRegisterRegion(buffer, offset, 2);
    *output = (int16_t)buffer[0] | (int16_t)buffer[1] << 8;

    return returnStatus;
}

lsm6dsl_status_t LSM6DSLCore::writeRegister(uint8_t offset, uint8_t data) {
    lsm6dsl_status_t returnStatus = IMU_SUCCESS;

    Wire.beginTransmission(i2cAddress);
    Wire.write(offset);
    Wire.write(data);
    if (Wire.endTransmission() != 0) {
        returnStatus = IMU_HW_ERROR;
    }

    return returnStatus;
}

lsm6dsl_status_t LSM6DSLCore::embeddedPage() {
    return writeRegister(LSM6DSL_ACC_GYRO_RAM_ACCESS_REG, 0x80);
}

lsm6dsl_status_t LSM6DSLCore::basePage() {
    return writeRegister(LSM6DSL_ACC_GYRO_RAM_ACCESS_REG, 0x00);
}


/* LSM6DSL class definitions */

LSM6DSL::LSM6DSL(uint8_t address):
LSM6DSLCore(address) {
	settings.gyroEnabled = 1;
	settings.gyroRange = 2000;
	settings.gyroSampleRate = 416;
	settings.gyroFifoEnabled = 1;
	settings.gyroFifoDecimation = 1;

	settings.accelEnabled = 1;
	settings.accelODROff = 1;
	settings.accelRange = 16;
	settings.accelSampleRate = 416;
	settings.accelBandWidth = 100;
	settings.accelFifoEnabled = 1;
	settings.accelFifoDecimation = 1;

	settings.tempEnabled = 1;

	//FIFO control data
	settings.fifoThreshold = 3000;
	settings.fifoSampleRate = 10;
	settings.fifoModeWord = 0;

	// allOnesCounter = 0;
	// nonSuccessCounter = 0;
}

lsm6dsl_status_t LSM6DSL::begin() {
    uint8_t data = 0;
    uint8_t bwData = 0;
    lsm6dsl_status_t returnStatus = beginCore();

    data = 0;
    if (settings.accelEnabled == 1) {
        if (settings.accelSampleRate > 0x02) {
            data |= 0x01;
        }

        switch (settings.accelRange) {
            case 2:
                data |= LSM6DSL_ACC_GYRO_FS_XL_2g;
                break;
            case 4:
                data |= LSM6DSL_ACC_GYRO_FS_XL_4g;
                break;
            case 8:
                data |= LSM6DSL_ACC_GYRO_FS_XL_8g;
                break;
            default:  //set default case to 16(max)
            case 16:
                data |= LSM6DSL_ACC_GYRO_FS_XL_16g;
                break;
        }

        switch (settings.accelSampleRate) {
            case 13:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_13Hz;
                break;
            case 26:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_26Hz;
                break;
            case 52:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_52Hz;
                break;
            default:  //Set default to 104
            case 104:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_104Hz;
                break;
            case 208:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_208Hz;
                break;
            case 416:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_416Hz;
                break;
            case 833:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_833Hz;
                break;
            case 1660:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_1660Hz;
                break;
            case 3330:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_3330Hz;
                break;
            case 6660:
                data |= LSM6DSL_ACC_GYRO_ODR_XL_6660Hz;
                break;;
        }

        data |= settings.accelRange;
        data |= settings.accelSampleRate;
    }

    writeRegister(LSM6DSL_ACC_GYRO_CTRL1_XL_REG, data);

    data = 0;
    if (settings.gyroEnabled == 1) {
        switch (settings.gyroRange) {
        case 125:
            data |= LSM6DSL_ACC_GYRO_FS_G_125dps;
            break;
        case 245:
            data |= LSM6DSL_ACC_GYRO_FS_G_245dps;
            break;
        case 500:
            data |= LSM6DSL_ACC_GYRO_FS_G_500dps;
            break;
        case 1000:
            data |= LSM6DSL_ACC_GYRO_FS_G_1000dps;
            break;
        default:  //Default to full 2000DPS range
        case 2000:
            data |= LSM6DSL_ACC_GYRO_FS_G_2000dps;
            break;
        }

        switch (settings.gyroSampleRate) {
        case 13:
            data |= LSM6DSL_ACC_GYRO_ODR_G_13Hz;
            break;
        case 26:
            data |= LSM6DSL_ACC_GYRO_ODR_G_26Hz;
            break;
        case 52:
            data |= LSM6DSL_ACC_GYRO_ODR_G_52Hz;
            break;
        default:  //Set default to 104
        case 104:
            data |= LSM6DSL_ACC_GYRO_ODR_G_104Hz;
            break;
        case 208:
            data |= LSM6DSL_ACC_GYRO_ODR_G_208Hz;
            break;
        case 416:
            data |= LSM6DSL_ACC_GYRO_ODR_G_416Hz;
            break;
        case 833:
            data |= LSM6DSL_ACC_GYRO_ODR_G_833Hz;
            break;
        case 1660:
            data |= LSM6DSL_ACC_GYRO_ODR_G_1660Hz;
            break;
        }
    }

    writeRegister(LSM6DSL_ACC_GYRO_CTRL2_G_REG, data);

    return returnStatus;
}