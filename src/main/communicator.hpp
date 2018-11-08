#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

class Communicator {
	public:
		void init();
		bool authenticate(const std::string id);
		void addConsumed(const std::string id, int milliliter);
}

#endif //COMMUNICATOR_HPP
