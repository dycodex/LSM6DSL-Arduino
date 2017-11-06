#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
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
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_CTRL1_XL_REG, dataToWrite);

	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_WAKE_UP_DUR, 0x00);

	// set FF threshold (FF_THS[2:0] = 011b)
	// set six samples event duration (FF_DUR[5:0] = 00110b)
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_FREE_FALL, 0x33);

	// FF interrupt driven to INT1 pin
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_MD1_CFG, 0x10);

	// route to INT2 as well
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_MD2_CFG, 0x10);

	// Latch interrupt & enable interrupt
	errorAccumulator += imu.writeRegister(LSM6DSL_ACC_GYRO_TAP_CFG, 0x81);


	// you can attach an interrupt handler here
}

void loop() {
	uint8_t readDataByte = 0;

	//Read the wake-up source register
	imu.readRegister(&readDataByte, LSM6DSL_ACC_GYRO_WAKE_UP_SRC);
	//Mask off the FF_IA bit for free-fall detection
	readDataByte &= 0x20;
	//Check for free-fall
    if(readDataByte) {
		delay(10);
		Serial.println("Interrupt caught.  Free fall detected.");
	}
}
