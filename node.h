#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

class Node {
private:
	glm::vec2 position;
	bool isOccupiedBySheep;
	bool isOccupiedByTiger;

public:
	std::vector<Node*> connected;
	bool isActive;

	Node() : isOccupiedBySheep(false), isOccupiedByTiger(false), isActive(false) {

	}
	
	glm::vec2 GetPosition() {
		return position;
	}

	void SetPosition(glm::vec2 position)
	{
		this->position = position;
	}

	void SetOccupiedBySheep(bool isOccupied) {
		this->isOccupiedBySheep = isOccupied;
	}

	bool GetOccupiedBySheep() {
		return isOccupiedBySheep;
	}

	void SetOccupiedByTiger(bool isOccupied) {
		this->isOccupiedByTiger = isOccupied;
	}

	bool GetOccupiedByTiger() {
		return isOccupiedByTiger;
	}

	void ShuffleConnected(){
		for (int i = 0; i < connected.size() - 1; i++) {
			int j = (i + rand()) % (connected.size() - i);
			std::swap(connected[i], connected[j]);
		}
	}
};