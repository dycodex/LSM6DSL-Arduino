#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LSM6DSL.h>

// Uncomment this line for SPI mode. Assign the correct chip select pin to the constructor.
// LSM6DSLCore imu(LSM6DSL_MODE_SPI, 33);

// Using I2C mode by default.
LSM6DSLCore imu(LSM6DSL_MODE_I2C, 0x6B);

#define INT1_PIN 6
uint8_t int1Status;

void int1Handler() {
    int1Status++;
}

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
    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_CTRL1_XL_REG, dataToWrite);

    // Enable tap detection on X, Y, and Z axis and disable latch interrupt.
    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_TAP_CFG, 0x0E);

    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_TAP_THS_6D, 0x03);

    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_INT_DUR2, 0x7F);

    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_WAKE_UP_THS, 0x80);

    errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_MD1_CFG, 0x48);

    pinMode(INT1_PIN, INPUT);
    attachInterrupt(INT1_PIN, int1Handler, RISING);
}

void loop() {
    if (int1Status > 0) {
        delay(300);

        if (int1Status == 1) {
            Serial.println("Single-tap event");
        }

        if (int1Status > 1) {
            Serial.println("Double-tap event");
        }

        int1Status = 0;
    }
}
