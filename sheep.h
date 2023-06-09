#pragma once
#include "game_object.h"

enum SheepState {
	SHEEP_PLACING,
	SHEEP_PLACED
};

class Sheep : public GameObject {
public:
	bool isEaten;
	SheepState sheepState;
	Sheep();
	Sheep(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
};