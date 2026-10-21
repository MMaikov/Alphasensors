#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup() {
	Serial.begin(9600);
	Serial.println(F("Hello, world!"));

	if (!ads.begin()) {
		Serial.println(F("Failed to initialize ADS1115"));
		while(1);
	} else {
		Serial.println(F("ADS1115 initialized"));
	}

	// ADS gain of 4 means +-1.024V
	ads.setGain(GAIN_FOUR);
}

void loop() {
	// ADS1115 ADC count to mV factor
	constexpr float ADS_MULTIPLIER = 0.03125f;

	// How long to collect samples for in ms
	constexpr unsigned long SAMPLE_DURATION = 1000;

	// Working Electrode (WE) zero offset in mV
	constexpr float WE_0 = 270.0f;
	// Auxiliary Electrode (AE) zero offset in mV
	constexpr float AE_0 = 340.0f;

	// Working Electrode (WE) sensitivity
	constexpr float SENSITIVITY = 420.0f;

	int32_t acc_op1 = 0;
	int32_t acc_op2 = 0;
	uint32_t samples = 0;

	const unsigned long lastTime = millis();
	while ((millis() - lastTime) < SAMPLE_DURATION) {
		acc_op1 += ads.readADC_Differential_0_1();
		acc_op2 += ads.readADC_Differential_2_3();
		samples++;
	}

	const float op1 = (acc_op1) / (float)(samples);
	const float op2 = (acc_op2) / (float)(samples);

	const float op1_voltage = op1 * ADS_MULTIPLIER - WE_0;
	const float op2_voltage = op2 * ADS_MULTIPLIER - AE_0;
	const float op_voltage = op1_voltage - op2_voltage;
	const float co_ppb = op_voltage / SENSITIVITY * 1000.0f;

	Serial.print(F("Op1: "));
	Serial.print(op1_voltage);
	Serial.print(F(" mV Op2: "));
	Serial.print(op2_voltage);
	Serial.print(F(" mV Op: "));
	Serial.print(op_voltage);
	Serial.print(F(" mV CO: "));
	Serial.print(co_ppb);
	Serial.println(F(" ppb"));
}