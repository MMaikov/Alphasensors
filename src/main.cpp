#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

// Typical B4 sensors
namespace Alphasensors {
	struct Reading {
		float op1;
		float op2;
	};

	struct CO {
		// Working Electrode zero offset
		static constexpr float WE_0 = 270;
		// Auxiliary Electrode zero offset
		static constexpr float AE_0 = 340;
		// Working Electrode Sensitivity
		static constexpr float SENSITIVITY = 420;
	};
	
	struct H2S {
		// Working Electrode zero offset
		static constexpr float WE_0 = 350;
		// Auxiliary Electrode zero offset
		static constexpr float AE_0 = 350;
		// Working Electrode Sensitivity
		static constexpr float SENSITIVITY = 1400;
	};

	struct NO {
		// Working Electrode zero offset
		static constexpr float WE_0 = 545;
		// Auxiliary Electrode zero offset
		static constexpr float AE_0 = 520;
		// Working Electrode Sensitivity
		static constexpr float SENSITIVITY = 1400;
	}; 

	struct NO2 {
		// Working Electrode zero offset
		static constexpr float WE_0 = 225;
		// Auxiliary Electrode zero offset
		static constexpr float AE_0 = 245;
		// Working Electrode Sensitivity
		static constexpr float SENSITIVITY = 309;
	}; 

	struct O3 {
		// Working Electrode zero offset
		static constexpr float WE_0 = 260;
		// Auxiliary Electrode zero offset
		static constexpr float AE_0 = 300;
		// Working Electrode Sensitivity
		static constexpr float SENSITIVITY = 298;
	}; 

	struct SO2 {
		// Working Electrode zero offset
		static constexpr float WE_0 = 355;
		// Auxiliary Electrode zero offset
		static constexpr float AE_0 = 345;
		// Working Electrode Sensitivity
		static constexpr float SENSITIVITY = 280;
	}; 
}

template<typename Config, typename SensorReader>
class Alphasensor {
public:
	float read() {
		const Alphasensors::Reading reading = SensorReader::read();
		constexpr float OFFSET_DIFF = Config::WE_0 - Config::AE_0;
		return (reading.op1 - reading.op2 - OFFSET_DIFF) / Config::SENSITIVITY;
	}
};

struct CO_Reader {
	static Alphasensors::Reading read() {
		constexpr float ADS_MULTIPLIER = 0.03125f;

		int32_t acc_op1 = 0;
		int32_t acc_op2 = 0;
		uint32_t samples = 0;
		unsigned long lastTime = millis();
		while ((millis()-lastTime)<1000) {
			acc_op1 += ads.readADC_Differential_0_1();
			acc_op2 += ads.readADC_Differential_2_3();
			samples++;
		}

		const float op1 = acc_op1 / static_cast<float>(samples);
		const float op2 = acc_op2 / static_cast<float>(samples);

		return Alphasensors::Reading {
			op1 * ADS_MULTIPLIER,
			op2 * ADS_MULTIPLIER
		};
	}
};

Alphasensor<Alphasensors::CO, CO_Reader> sensor;

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
	const float co_ppb = 1000.0f * sensor.read();

	Serial.print(F("CO: "));
	Serial.print(co_ppb);
	Serial.println(F(" ppb"));
}