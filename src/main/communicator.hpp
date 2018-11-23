#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include "Arduino.h"

class Communicator {
	public:
		void init();
		bool authenticate(String id);
		void addConsumed(String id, int milliliter);
		String getNameByUUID(String id);
		String getMlByUUID(String id);
		bool inMantenance();
		bool addingUser();
		void sendRawUUID(String id);
};

#endif //COMMUNICATOR_HPP
