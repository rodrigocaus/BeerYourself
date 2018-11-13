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

void Communicator::addConsumed(String id, int milliliter) {
    Serial.print("add_ml," + id + "," + String(milliliter) + "\n");
}
