#include <Arduino.h>
#include <Wire.h>
#include <LSM6DSL.h>

// Uncomment this line for SPI mode. Assign the correct chip select pin to the constructor.
// LSM6DSLCore imu(LSM6DSL_MODE_SPI, 33);

// Using I2C mode by default.
LSM6DSLCore imu(LSM6DSL_MODE_I2C, 0x6B);

void setup() {
    Serial.begin(9600);
    delay(2000);

    Serial.println("Start!");

    if (imu.beginCore() != IMU_SUCCESS) {
        Serial.println("Failed initializing IMU sensor");
    }

    uint8_t errorAccumulator = 0;
    uint8_t dataToWrite = 0;

    // Setup accelerometer
    dataToWrite = 0;
    dataToWrite |= LSM6DSL_ACC_GYRO_FS_XL_2g;
    dataToWrite |= LSM6DSL_ACC_GYRO_ODR_G_416Hz;

    // write the configuration byte
    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_CTRL1_XL_REG, dataToWrite);

    // 0x16 means:
    // PEDO_EN bit set to 1
    // PEDO_RST bit set to 1
    // FUNC_EN bit set to 1
    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_CTRL10_C, 0x16);

    // Enable interrupt on pin INT1
    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_INT1_CTRL, 0x80);

    if (errorAccumulator) {
        Serial.println("Error occured during device configuration.");
    } else {
        Serial.println("Successfully configured device.");
    }

    // you can attach an interrupt handler here
}

void loop() {
    uint8_t read = 0;
    uint16_t stepTaken = 0;

    // read the step count from the register
    imu.readRegister(&read, LSM6DSL_ACC_GYRO_STEP_COUNTER_H);
    stepTaken = ((uint16_t)read << 8);

    imu.readRegister(&read, LSM6DSL_ACC_GYRO_STEP_COUNTER_L);
    stepTaken |= read;

    Serial.print("Step taken: ");
    Serial.println(stepTaken);

    delay(1000);
}
