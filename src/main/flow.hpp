#ifndef flow_h
#define flow_h

#include <Arduino.h>

class Flow {
	int pin;

public:
	Flow(int _pin): pin(_pin) { pinMode(_pin, INPUT);};
	bool get_state();
	void wait_pulse();
	void count_pulses(int n);
	void count_volume(float vol, float volume_pulse_ratio);
};


#endif
