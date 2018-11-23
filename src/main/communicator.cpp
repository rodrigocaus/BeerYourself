#include "communicator.hpp"

void Communicator::init() {
  Serial.begin(9600);
  Serial.setTimeout(1000);
}

void Communicator::serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

bool Communicator::authenticate(String id) {
  Serial.print("check_UUID," + id + "\n");
  while (!Serial.available());
  if (Serial.read() == 'y') {
    // Authorized
    this->serialFlush();
    return true;
  }
  this->serialFlush();
  return false;
}

void Communicator::sendRawUUID(String id) {
  Serial.print("raw_UUID," + id + "\n");
  this->serialFlush();
}

void Communicator::addConsumed(String id, int milliliter) {
    Serial.print("add_ml," + id + "," + String(milliliter) + "\n");
    this->serialFlush();
}

String Communicator::getNameByUUID(String id) {
    Serial.print("get_name," + id + "\n");
	String name = "";
	char c = '\0';
  while (!Serial.available());
	while ((c = Serial.read()) != '\n') {
		name += c;
	}
  this->serialFlush();
	return name;
}

String Communicator::getMlByUUID(String id) {
    Serial.print("get_ml," + id + "\n");
	String ml = "";
	char c = '\0';
  while (!Serial.available());
	while ((c = Serial.read()) != '\n') {
		ml += c;
	}
  this->serialFlush();
	if (ml.length() > 0) {
		return ml.toInt();
	} else {
		return 0;
	}
}

bool Communicator::inMaintenance() {
  Serial.print("check_maintenance,\n");
  while (!Serial.available());
  if (Serial.read() == 'y') {
    this->serialFlush();
    return true;
  }
  this->serialFlush();
  return false;
}

bool Communicator::addingUser() {
  Serial.print("check_adding_user,\n");
  while (!Serial.available());
  if (Serial.read() == 'y') {
    this->serialFlush();
    return true;
  }
  this->serialFlush();
  return false;
}
