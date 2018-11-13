#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include "Arduino.h"

class Communicator {
	public:
		void init();
		bool authenticate(String id);
		void addConsumed(String id, int milliliter);
};

#endif //COMMUNICATOR_HPP
