#ifndef solenoid_h
#define solenoid_h

#include <Arduino.h>

class Solenoid {
	int pin;
public:
	Solenoid(int _pin): pin(_pin) { pinMode(_pin, OUTPUT);};

	void open() {
		digitalWrite(pin, HIGH);
	}

	void close() {
		digitalWrite(pin, LOW);
	}
};


#endif
