#include "communicator.hpp"

void Communicator::init() {
  Serial.begin(9600);
  Serial.setTimeout(1000);
}

bool Communicator::authenticate(String id) {
  Serial.print("check_UUID," + id + "\n");
  if (Serial.read() == 'y') {
    // Authorized
    return true;
  }
  return false;
}

void Communicator::sendRawUUID(String id) {
  Serial.print(id + "\n");
}

void Communicator::addConsumed(String id, int milliliter) {
    Serial.print("add_ml," + id + "," + String(milliliter) + "\n");
}

String Communicator::getNameByUUID(String id) {
    Serial.print("get_name," + id + "\n");
	String name = "";
	char c = '\0';
	while ((c = Serial.read()) != '\n') {
		name += c;
	}
	return name;
}

String Communicator::getMlByUUID(String id) {
    Serial.print("get_ml," + id + "\n");
	String ml = "";
	char c = '\0';
	while ((c = Serial.read()) != '\n') {
		ml += c;
	}
	if (ml.length() > 0) {
		return ml.toInt();
	} else {
		return 0;
	}
}

bool Communicator::inMantenance() {
  Serial.print("check_maintenance\n");
  if (Serial.read() == 'y') {
    return true;
  }
  return false;
}

bool Communicator::addingUser() {
  Serial.print("check_adding_user\n");
  if (Serial.read() == 'y') {
    return true;
  }
  return false;
}
