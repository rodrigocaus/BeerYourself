#include "flow.hpp"


bool Flow::get_state() {
	return (bool) digitalRead(this->pin);
}

void Flow::wait_pulse() {
	while (this->get_state());
}

void Flow::count_pulses(int n) {
	while (this->get_state());
	for (int i = 0; i < n; i++) {
		while (this->get_state());
	}
}

void Flow::count_volume(float volume, float volume_pulse_ratio) {
	this->count_pulses((int)(volume/volume_pulse_ratio));
}
