#include <Arduino.h>
#include "steptimer.h"
#include "statespace.h"
#include <driver/adc.h>
//#include <dac.h>


StepTimer timer;
sss::StateSpace<2,1,1> sys;
maths::matrix<1,1> u;

void setup() {
	Serial.begin(115200);
	
	maths::matrix<2,2> A = {{0,1},{-1 -2}};
	maths::matrix<2,1> B = {{0},{1}};
	maths::matrix<1,2> C = {{1,0}};

	sys.setA(A);
	sys.setB(B);
	sys.setC(C);
	u.set<0,0>(0);

	adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);

	timer.fixedStepTime(1000);
	timer.reset();
}

#define FACTOR 0.0000805860805860
#define CONVERT(val) ((float)((FACTOR)*(val))) 

float value;
void loop()
{
	//
	timer.tick([&]()mutable
	{
		value = CONVERT(adc1_get_raw(ADC1_CHANNEL_0));
		u.set<0,0>(value);
		sys.updateLeapfrog(timer.elapsedTime()/1000000.0f, u);
		
		float output = sys.getOutput().get<0,0>() / 3.3f;
		output = output * 255 + 128;
		output = min(max(output, 0.0f), 255.0f);
		dacWrite(DAC1, (uint8_t)output);
		Serial.print(output);
	});
}
