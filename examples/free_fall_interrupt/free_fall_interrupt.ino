#include <Arduino.h>
#include <Wire.h>
#include <LSM6DSL.h>

LSM6DSLCore imu;

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

	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_WAKE_UP_DUR, 0x00);

	// set FF threshold (FF_THS[2:0] = 011b)
	// set six samples event duration (FF_DUR[5:0] = 00110b)
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_FREE_FALL, 0x33);

	// FF interrupt driven to INT1 pin
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_MD1_CFG, 0x10);

	// route to INT2 as well
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_MD2_CFG, 0x10);

	// Latch interrupt
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_TAP_CFG, 0x01);


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
