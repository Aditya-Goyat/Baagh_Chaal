#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "text_renderer.h"
#include "tiger.h"
#include "sheep.h"
#include "board.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include "scenemanager.h"
#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;

ISoundEngine* SoundEngine = createIrrKlangDevice();
SpriteRenderer* Renderer;
GameObject* Player;
TextRenderer* Text;
Tiger* tiger[3];
Sheep* sheep[15];
GameObject* background, *sheep_won, *tiger_won, *transluscent_bg;
Board* board;
Node* nodeUnderSheep = nullptr, *nodeUnderTiger = nullptr;
int fps;
int eatenSheep = 0;
int random = 0, preRandom = random;

//Button Game Object
GameObject* button_Play, *button_PVP, *button_Exit, *button_home, *button_home2, *button_replay, *button_Rules, * button_Rules2, *rules_BG, *button_back;
GameObject* your_turn, *sheep_turn, *tiger_turn;

void ShuffleTiger();
bool AllGameObjectsAtRest();
float movementTime = 0.5f;

bool sheepWon = false, tigerWon = false;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_MENU), PrevState(GAME_MENU), Keys(), Width(width), Height(height), turn(SHEEP)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Text;
}

void Game::Init()
{
	SoundEngine->play2D("Audio/background.mp3", true);
	SoundEngine->setSoundVolume(0.12);

	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");

	//load textures
	ResourceManager::LoadTexture("Textures/MainMenuBackground.png", true, "mm_background");
	ResourceManager::LoadTexture("Textures/sheep_won.png", true, "sheep_won");
	ResourceManager::LoadTexture("Textures/tiger_won.png", true, "tiger_won");
	ResourceManager::LoadTexture("Textures/Node.png", true, "Node");
	ResourceManager::LoadTexture("Textures/Sheep.png", true, "sheep");
	ResourceManager::LoadTexture("Textures/PlayBackground.png", true, "PlayBg");
	ResourceManager::LoadTexture("Textures/button_Play.png", true, "button_Play");
	ResourceManager::LoadTexture("Textures/button_PVP.png", true, "button_PVP");
	ResourceManager::LoadTexture("Textures/button_Exit.png", true, "button_Exit");
	ResourceManager::LoadTexture("Textures/button_Rules.png", true, "button_Rules");
	ResourceManager::LoadTexture("Textures/button_home.png", true, "button_Home");
	ResourceManager::LoadTexture("Textures/button_Replay.png", true, "button_Replay");
	ResourceManager::LoadTexture("Textures/button_Home2.png", true, "button_Home2");
	ResourceManager::LoadTexture("Textures/button_Home2_Hover.png", true, "button_Home2_Hover");
	ResourceManager::LoadTexture("Textures/button_Replay_Hover.png", true, "button_Replay_Hover");
	ResourceManager::LoadTexture("Textures/button_Play_Hover.png", true, "button_Play_Hover");
	ResourceManager::LoadTexture("Textures/button_PVP_Hover.png", true, "button_PVP_Hover");
	ResourceManager::LoadTexture("Textures/button_Exit_Hover.png", true, "button_Exit_Hover");
	ResourceManager::LoadTexture("Textures/button_Rules_Hover.png", true, "button_Rules_Hover");
	ResourceManager::LoadTexture("Textures/button_home_Hover.png", true, "button_Home_Hover");
	ResourceManager::LoadTexture("Textures/Tiger.png", true, "tiger");
	ResourceManager::LoadTexture("Textures/transluscent.png", true, "transluscent");
	ResourceManager::LoadTexture("Textures/rules.png", true, "rules");
	ResourceManager::LoadTexture("Textures/button_back.png", true, "button_back");
	ResourceManager::LoadTexture("Textures/button_back_Hover.png", true, "button_back_Hover");
	ResourceManager::LoadTexture("Textures/your_turn.png", true, "your_turn");
	ResourceManager::LoadTexture("Textures/tiger_turn.png", true, "tiger_turn");
	ResourceManager::LoadTexture("Textures/sheep_turn.png", true, "sheep_turn");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// set render-specific controls
	Shader myShader = ResourceManager::GetShader("sprite");
	Renderer = new SpriteRenderer(myShader);

	Text = new TextRenderer(this->Width, this->Height);
	Text->Load("fonts/Montserrat-Bold.TTF", 24);

	background = new GameObject(glm::vec2(0, 0), glm::vec2(Width, Height), ResourceManager::GetTexture("mm_background"));
	rules_BG = new GameObject(glm::vec2(0, 0), glm::vec2(Width, Height), ResourceManager::GetTexture("rules"));
	sheep_won = new GameObject(glm::vec2(194, 117), glm::vec2(1532, 847), ResourceManager::GetTexture("sheep_won"));
	tiger_won = new GameObject(glm::vec2(194, 117), glm::vec2(1532, 847), ResourceManager::GetTexture("tiger_won"));
	transluscent_bg = new GameObject(glm::vec2(0, 0), glm::vec2(1920, 1080), ResourceManager::GetTexture("transluscent"));

	board = new Board();

	InitTiger();

	InitSheep(glm::vec2(87, 252));

	//Button Game Objets
	button_Play = new GameObject(glm::vec2(735, 330), glm::vec2(405, 118), ResourceManager::GetTexture("button_Play"));
	button_PVP = new GameObject(glm::vec2(735, 498), glm::vec2(405, 118), ResourceManager::GetTexture("button_PVP"));
	button_Exit = new GameObject(glm::vec2(735, 834), glm::vec2(405, 118), ResourceManager::GetTexture("button_Exit"));
	button_Rules = new GameObject(glm::vec2(735, 666), glm::vec2(405, 118), ResourceManager::GetTexture("button_Rules"));
	button_Rules2 = new GameObject(glm::vec2(1475, 34), glm::vec2(405, 118), ResourceManager::GetTexture("button_Rules"));
	button_back = new GameObject(glm::vec2(1475, 31), glm::vec2(405, 118), ResourceManager::GetTexture("button_back"));
	button_home = new GameObject(glm::vec2(1241, 33), glm::vec2(209, 118), ResourceManager::GetTexture("button_Home"));
	button_replay = new GameObject(glm::vec2(954, 778), glm::vec2(405, 118), ResourceManager::GetTexture("button_Replay"));
	button_home2 = new GameObject(glm::vec2(524, 778), glm::vec2(405, 118), ResourceManager::GetTexture("button_Home2"));
	your_turn = new GameObject(glm::vec2(1668, 382), glm::vec2(184, 244), ResourceManager::GetTexture("your_turn"));
	tiger_turn = new GameObject(glm::vec2(1621, 371), glm::vec2(274, 274), ResourceManager::GetTexture("tiger_turn"));
	sheep_turn = new GameObject(glm::vec2(1621, 371), glm::vec2(274, 274), ResourceManager::GetTexture("sheep_turn"));
	SceneManager::LoadScene(0, &State);

}

void Game::Update(float dt, GLFWwindow* window)
{
	fps = 60.0 / dt;

	if (State == GAME_PVP || State == GAME_AI) {
		int numberOfSheepEaten = 0;
		for (Sheep*& currSheep : sheep) {
			if (currSheep->isEaten) {
				currSheep->Color.w -= dt * 10;
				if (currSheep->Color.w <= 0)
					currSheep->Color.w = 0;
				numberOfSheepEaten++;
			}
			if (currSheep->Update(dt))
				turn = TIGER;
		}

		for (Tiger* currTiger : tiger) {
			if (currTiger->Update(dt) && State == GAME_PVP)
				turn = SHEEP;
		}

		if (numberOfSheepEaten >= 5 && AllGameObjectsAtRest()) {
			tigerWon = true;
			//SceneManager::LoadScene(2, &State);
		}

		SetStuckGameObjects();
	}

	if (State == GAME_AI && turn == TIGER) {
		ShuffleTiger();

		//CASE 1: If a tiger can immediately kill a goat
		for (Tiger* currTiger : tiger) {

			nodeUnderTiger = GetNodeUnderGameObject(currTiger);
			if (nodeUnderTiger != NULL)
			{
				for (auto itr = nodeUnderTiger->connected.begin(); itr!=nodeUnderTiger->connected.end(); itr++)
				{
					//Getting the sheep adjacent to tiger
					Node* nodeOccupiedBySheep = nullptr;
					if((*itr)->GetOccupiedBySheep() == true)
					{
						nodeOccupiedBySheep = (*itr);
					}

					//Getting nodes that are connected to the adjacent sheep
					if (nodeOccupiedBySheep != nullptr)
					{
						for (auto itr = nodeOccupiedBySheep->connected.begin(); itr != nodeOccupiedBySheep->connected.end(); itr++)
						{
							(*itr)->ShuffleConnected();
							if (!(*itr)->GetOccupiedBySheep() && !(*itr)->GetOccupiedByTiger() && IsInSameDirection(nodeUnderTiger, nodeOccupiedBySheep, *itr))
							{
								MoveGameObject(currTiger, *itr);
								SoundEngine->play2D("Audio/tiger.mp3", false);
								Sheep* sheep = GetSheepOverNode(nodeOccupiedBySheep);
								sheep->isEaten = true;
								eatenSheep++;
								turn = SHEEP;
								nodeOccupiedBySheep = nullptr;
								return;
							}
						}
					}
				}

			}
		}

		//CASE 2: If a tiger can kill a goat in next turn
		for (Tiger* currTiger : tiger)
		{
			random = (preRandom + rand()) % (10);
			preRandom = random;
			if (random == 1 || random == 2)
				break;

			nodeUnderTiger = GetNodeUnderGameObject(currTiger);
			
			if (nodeUnderTiger != NULL)
			{
				for (auto itr1 = nodeUnderTiger->connected.begin(); itr1 != nodeUnderTiger->connected.end(); itr1++)
				{
					if (!(*itr1)->GetOccupiedByTiger() && !(*itr1)->GetOccupiedBySheep())
					{
						for (auto itr2 = (*itr1)->connected.begin(); itr2 != (*itr1)->connected.end(); itr2++)
						{
							if ((*itr2)->GetOccupiedBySheep())
							{
								(*itr2)->ShuffleConnected();
								for (auto itr3 = (*itr2)->connected.begin(); itr3 != (*itr2)->connected.end(); itr3++)
								{
									if (!(*itr3)->GetOccupiedBySheep() && !(*itr3)->GetOccupiedByTiger() && IsInSameDirection(*itr1, *itr2, *itr3))
									{
										SoundEngine->play2D("Audio/tiger.mp3", false);
										MoveGameObject(currTiger, *itr1);
										turn = SHEEP;
										return;
									}
								}
							}
						}
					}
				}
			}
		}

		//CASE 3: Randomly move a tiger
		for (Tiger* currTiger : tiger)
		{
			nodeUnderTiger = GetNodeUnderGameObject(currTiger);
			if (nodeUnderTiger != NULL)
			{
				for (auto itr = nodeUnderTiger->connected.begin(); itr != nodeUnderTiger->connected.end(); itr++)
				{
					(*itr)->ShuffleConnected();
					if (!(*itr)->GetOccupiedBySheep() && !(*itr)->GetOccupiedByTiger())
					{
						SoundEngine->play2D("Audio/tiger.mp3", false);
						MoveGameObject(currTiger, *itr);
						turn = SHEEP;
						return;
					}
				}
			}
		}


	}
}

void Game::ProcessInput(float dt, GLFWwindow* window)
{
	CheckMouseHover();

	if (Keys[GLFW_KEY_SPACE] && !keysProcessed[GLFW_KEY_SPACE]) {
		keysProcessed[GLFW_KEY_SPACE] = true;
		SceneManager::LoadScene(1, &State);
	}

	if (Mouse[0] && !mouseProcessed[0]) {
		mouseProcessed[0] = true;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec2 mouseClick = glm::vec2(xpos, ypos);
		Sheep* activeSheep;
		Tiger* activeTiger;
		Node* nodeClicked;

		//Change scene depending on the button clicked in the main menu.
		if (State == GAME_MENU) {
			if (ChangeScene(mouseClick, window))
				return;
		}

		if (State == GAME_PVP || State == GAME_AI) {
			if (CheckButtonClick(mouseClick, button_home)) {
				PrevState = State;
				SceneManager::LoadScene(0, &State);
				SoundEngine->setSoundVolume(0.6);
				SoundEngine->play2D("Audio/button.mp3", false);
				SoundEngine->setSoundVolume(0.2);
				return;
			}

			if (CheckButtonClick(mouseClick, button_Rules2)) {
				PrevState = State;
				SceneManager::LoadScene(3, &State);
				SoundEngine->setSoundVolume(0.6);
				SoundEngine->play2D("Audio/button.mp3", false);
				SoundEngine->setSoundVolume(0.2);
				return;
			}
		}

		if (State == GAME_PVP) {
			std::cout << "In game PVP" << std::endl;
			if (tigerWon || sheepWon) {

				if (CheckButtonClick(mouseClick, button_home2)) {
					SceneManager::LoadScene(0, &State);
					SoundEngine->setSoundVolume(0.6);
					SoundEngine->play2D("Audio/button.mp3", false);
					SoundEngine->setSoundVolume(0.2);
				}
				
				if (CheckButtonClick(mouseClick, button_replay)) {
					turn = SHEEP;
					eatenSheep = 0;
					sheepWon = false;
					tigerWon = false;
					InitSheep(glm::vec2(87, 252));
					InitTiger();
					SoundEngine->setSoundVolume(0.6);
					SoundEngine->play2D("Audio/button.mp3", false);
					SoundEngine->setSoundVolume(0.12);
				}
				return;
			}

			//Check if a sheep has been clicked.
			//---------------------------------
			Sheep* sheepClicked = CheckSheepClick(mouseClick);
			if (sheepClicked != nullptr) {
				//If it is the tiger's turn, return from the function.
				if (turn == TIGER)
					return;

				//if the sheep is stuck, return from the function.
				if (sheepClicked->GetStuck())
					return;

				if (sheepClicked->sheepState == SHEEP_PLACED) {

					if (!CheckIfAllSheepPlaced())	//If any sheep is in the placing mode, return from the function.
						return;

					//Get the node under the sheep that has been clicked.
					nodeUnderSheep = GetNodeUnderGameObject(sheepClicked);
				}
				if (sheepClicked->Velocity == glm::vec2(0, 0)) {

					DeactivateAllGameObjects();		//Set all other game object's isActive to false to avoid multiple active game objects.

					//Set the sheep clicked's isActive to true and change it's color to green.
					sheepClicked->IsActive = true;
					sheepClicked->Color = glm::vec4(0, 1, 0, 1);
				}

				return;
			}

			//Check if a tiger has been clicked.
			//---------------------------------
			Tiger* tigerClicked = CheckTigerClick(mouseClick);
			if (tigerClicked != nullptr) {
				//If it is the sheep's turn, return from the function.
				if (turn == SHEEP)
					return;

				//If the tiger is stuck, return from the function.
				if (tigerClicked->GetStuck())
					return;

				//Get the node under the tiger that has been clicked.
				nodeUnderTiger = GetNodeUnderGameObject(tigerClicked);

				if (tigerClicked->Velocity == glm::vec2(0, 0)) {

					DeactivateAllGameObjects();		//Set all other game object's isActive to false to avoid multiple active game objects.

					//Set the tiger clicked's isActive to true and change it's color to green.
					tigerClicked->IsActive = true;
					tigerClicked->Color = glm::vec4(0, 1, 0, 1);
				}

				return;
			}

			//Get the active sheep.
			activeSheep = GetActiveSheep();
			//Get the active tiger.
			activeTiger = GetActiveTiger();		//At a time, both the activeSheep and activeTiger will not have values. Either one will be null.

			//Check the node that has been clicked as the destination node for moving.
			nodeClicked = CheckNodeClick(mouseClick);

			if (nodeClicked != nullptr) {

				if (activeSheep != nullptr) {
					if (nodeUnderSheep != nullptr) {
						//Check if the node clicked on is connected to the node that the sheep is placed on.
						//---------------------------------------------------------------------------------
						if (!isNodeClickedConnected(nodeUnderSheep, nodeClicked))
							return;
					}

					//Move the sheep to the node's position.
					MoveGameObject(activeSheep, nodeClicked);
					SoundEngine->play2D("Audio/sheep.mp3", false);

					activeSheep->sheepState = SHEEP_PLACED;
					//turn = TIGER;

				}

				if (activeTiger != nullptr) {
					if (nodeUnderTiger != nullptr) {

						//Check if the node clicked is connected to the node under the tiger.
						if (!isNodeClickedConnected(nodeUnderTiger, nodeClicked)) {

							//Check if the node is connected to a sheep that is connected to the tiger so that the tiger can eat the sheep.
							if (!CanTigerEatSheep(nodeUnderTiger, nodeClicked))
								return;
						}
					}

					//Move the tiger to the node's position.
					MoveGameObject(activeTiger, nodeClicked);
					SoundEngine->play2D("Audio/tiger.mp3", false);
					//turn = SHEEP;
				}
			}
		}

		if (State == GAME_AI) {
			if (tigerWon || sheepWon) {

				if (CheckButtonClick(mouseClick, button_home2)) {
					SceneManager::LoadScene(0, &State);
					SoundEngine->setSoundVolume(0.6);
					SoundEngine->play2D("Audio/button.mp3", false);
					SoundEngine->setSoundVolume(0.2);
				}
				
				if (CheckButtonClick(mouseClick, button_replay)) {
					turn = SHEEP;
					eatenSheep = 0;
					sheepWon = false;
					tigerWon = false;
					InitSheep(glm::vec2(87, 252));
					InitTiger();
					SoundEngine->setSoundVolume(0.6);
					SoundEngine->play2D("Audio/button.mp3", false);
					SoundEngine->setSoundVolume(0.12);
				}
				std::cout << "In tiger or sheep won" << std::endl;
				return;
			}


			//Check if a sheep has been clicked.
			//---------------------------------
			Sheep* sheepClicked = CheckSheepClick(mouseClick);
			if (sheepClicked != nullptr) {
				//If it is the tiger's turn, return from the function.
				if (turn == TIGER)
					return;

				//if the sheep is stuck, return from the function.
				if (sheepClicked->GetStuck())
					return;

				if (sheepClicked->sheepState == SHEEP_PLACED) {

					if (!CheckIfAllSheepPlaced())	//If any sheep is in the placing mode, return from the function.
						return;

					//Get the node under the sheep that has been clicked.
					nodeUnderSheep = GetNodeUnderGameObject(sheepClicked);
				}
				if (sheepClicked->Velocity == glm::vec2(0, 0)) {

					DeactivateAllGameObjects();		//Set all other game object's isActive to false to avoid multiple active game objects.

					//Set the sheep clicked's isActive to true and change it's color to green.
					sheepClicked->IsActive = true;
					sheepClicked->Color = glm::vec4(0, 1, 0, 1);
				}

				return;
			}

			std::cout << "Getting active sheep" << std::endl;
			//Get the active sheep.
			activeSheep = GetActiveSheep();

			std::cout << "Checking node clicked" << std::endl;
 			//Check the node that has been clicked as the destination node for moving.
			nodeClicked = CheckNodeClick(mouseClick);

			if (nodeClicked != nullptr) {

				if (activeSheep != nullptr) {
					if (nodeUnderSheep != nullptr) {
						//Check if the node clicked on is connected to the node that the sheep is placed on.
						//---------------------------------------------------------------------------------
						if (!isNodeClickedConnected(nodeUnderSheep, nodeClicked))
							return;
					}

					//Move the sheep to the node's position.
					MoveGameObject(activeSheep, nodeClicked);
					SoundEngine->play2D("Audio/sheep.mp3", false);

					activeSheep->sheepState = SHEEP_PLACED;
					//turn = TIGER;
				}
			}

		}

		if (State == GAME_RULES) {
			if (CheckButtonClick(mouseClick, button_back))
				State = PrevState;
		}
	}
}

void Game::Render()
{

	if (State == GAME_PVP || State == GAME_AI) {
		Texture2D PlayBg = ResourceManager::GetTexture("PlayBg");
		Renderer->DrawSprite(PlayBg, glm::vec2(0, 0), glm::vec2(Width, Height));

		for (int i = 0; i < 15; i++) {
			sheep[i]->Draw(*Renderer);
		}

		for (int i = 0; i < 3; i++) {
			tiger[i]->Draw(*Renderer);
		}

		Text->RenderText(std::to_string(eatenSheep), 262, 75, 3, glm::vec3(0, 0, 0));
		Text->RenderText(std::to_string((15 - eatenSheep)), 615, 75, 3, glm::vec3(0, 0, 0));

		button_home->Draw(*Renderer);
		button_Rules2->Draw(*Renderer);

		if (State == GAME_PVP) {
			if (turn == SHEEP)
				sheep_turn->Draw(*Renderer);
			else if (turn == TIGER)
				tiger_turn->Draw(*Renderer);
		}
		//else {
		//	if (turn == SHEEP)
		//		your_turn->Draw(*Renderer);
		//}

		if (sheepWon || tigerWon) {
			
			transluscent_bg->Draw(*Renderer);

			if (sheepWon) {
				sheep_won->Draw(*Renderer);
			}
			if (tigerWon) {
				tiger_won->Draw(*Renderer);
			}

			button_home2->Draw(*Renderer);
			button_replay->Draw(*Renderer);
		}
		
	}

	else if (State == GAME_MENU) {
		background->Draw(*Renderer);
		button_Play->Draw(*Renderer);
		button_Exit->Draw(*Renderer);
		button_PVP->Draw(*Renderer);
		button_Rules->Draw(*Renderer);
		Texture2D sheepTex = ResourceManager::GetTexture("sheep");
		Renderer->DrawSprite(sheepTex, glm::vec2(670, 904), glm::vec2(143, 172));
	}

	else if (State == GAME_RULES) {
		rules_BG->Draw(*Renderer);
		button_back->Draw(*Renderer);
	}

	//Text->RenderText(std::to_string(fps), 10, 10, 1);
}

bool Game::CanTigerEatSheep(Node* nodeUnderTiger, Node* nodeClicked) {
	Node* nodeOccupiedBySheep = nullptr;
	for (auto itr = nodeUnderTiger->connected.begin(); itr != nodeUnderTiger->connected.end(); itr++) {
		if ((*itr)->GetOccupiedBySheep() == true) {
			nodeOccupiedBySheep = (*itr);
		}

		if (nodeOccupiedBySheep != nullptr) {
			for (auto itr = nodeOccupiedBySheep->connected.begin(); itr != nodeOccupiedBySheep->connected.end(); itr++) {
				if ((*itr)->GetPosition() == nodeClicked->GetPosition()) {

					if (!IsInSameDirection(nodeUnderTiger, nodeOccupiedBySheep, nodeClicked))
						return false;

					nodeUnderTiger->SetOccupiedByTiger(false);
					Sheep* sheepOverNode = GetSheepOverNode(nodeOccupiedBySheep);
					sheepOverNode->isEaten = true;
					eatenSheep++;
					nodeOccupiedBySheep->SetOccupiedBySheep(false);
					return true;
				}
			}
		}
	}

	return false;
}

bool Game::IsInSameDirection(Node* nodeUnderTiger, Node* nodeOccupiedBySheep, Node* nodeClicked) {

	glm::vec2 dirFromTigerToSheep = glm::normalize(nodeOccupiedBySheep->GetPosition() - nodeUnderTiger->GetPosition());
	glm::vec2 dirFromSheepToNodeClicked = glm::normalize(nodeClicked->GetPosition() - nodeOccupiedBySheep->GetPosition());

	glm::vec2 errorVec = dirFromSheepToNodeClicked - dirFromTigerToSheep;

	if (glm::length(errorVec) >= 1.0f)
		return false;

	return true;
}

bool Game::isNodeClickedConnected(Node* startNode, Node* nodeClicked)
{
	for (auto itr = startNode->connected.begin(); itr != startNode->connected.end(); itr++) {
		if ((*itr)->GetPosition() == nodeClicked->GetPosition()) {
			return true;
		}
	}

	return false;
}

void Game::MoveGameObject(GameObject* gb, Node* dest) {
	gb->endPos = (dest->GetPosition() + glm::vec2(23, 23)) - (gb->Size) * 0.5f;
	float dist = glm::length(gb->endPos - gb->Position);
	gb->speed = dist / movementTime;
	gb->IsActive = false;
}

void Game::DeactivateAllGameObjects()
{
	for (Tiger* currTiger : tiger) {
		currTiger->IsActive = false;
		if (currTiger->GetStuck())
			continue;
		currTiger->Color = glm::vec4(1, 1, 1, 1);
	}
	for (Sheep* currSheep : sheep) {
		if (currSheep->isEaten)
			continue;
		currSheep->IsActive = false;
		if (currSheep->GetStuck())
			continue;
		currSheep->Color = glm::vec4(1, 1, 1, 1);
	}
}

bool Game::CheckIfAllSheepPlaced()
{
	for (Sheep* currSheep : sheep) {
		if (currSheep->sheepState == SHEEP_PLACING)
			return false;
	}
	return true;
}

bool Game::ChangeScene(const glm::vec2& mouseClick, GLFWwindow* window)
{
	if (CheckButtonClick(mouseClick, button_Play)) {
		turn = SHEEP;
		eatenSheep = 0;
		sheepWon = false;
		tigerWon = false;
		InitSheep(glm::vec2(87, 252));
		InitTiger();
		SoundEngine->setSoundVolume(0.6);
		SoundEngine->play2D("Audio/button.mp3", false);
		SoundEngine->setSoundVolume(0.12);
		PrevState = State;
		board = new Board();
		SceneManager::LoadScene(4, &State);
		return true;
	}
	else if (CheckButtonClick(mouseClick, button_Exit)) {
		glfwSetWindowShouldClose(window, true);
		return true;
	}
	else if (CheckButtonClick(mouseClick, button_PVP)) {
		turn = SHEEP;
		eatenSheep = 0;
		sheepWon = false;
		tigerWon = false;
		InitSheep(glm::vec2(87, 252));
		InitTiger();
		SoundEngine->setSoundVolume(0.6);
		SoundEngine->play2D("Audio/button.mp3", false);
		SoundEngine->setSoundVolume(0.12);
		PrevState = State;
		board = new Board();
		SceneManager::LoadScene(1, &State);
		return true;
	}
	else if (CheckButtonClick(mouseClick, button_Rules)) {
		PrevState = State;
		SceneManager::LoadScene(3, &State);
		return true;
	}
}

void Game::ResetLevel()
{

}

void Game::ResetPlayer()
{

}

// collision detection
bool CheckCollision(GameObject& one, GameObject& two);

void Game::DoCollisions()
{

}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// collision only if on both axes
	return collisionX && collisionY;
}

void Game::InitSheep(glm::vec2 startPos) {
	int counter = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 5; j++) {
			glm::vec2 position = glm::vec2(startPos.x + (i * 107), startPos.y + (j * 114));
			glm::vec2 size = glm::vec2(89, 106);

			sheep[counter] = new Sheep(position, size, ResourceManager::GetTexture("sheep"));
			sheep[counter]->sheepState = SHEEP_PLACING;
			sheep[counter]->IsActive = false;
			counter++;
		}
	}
}

void Game::InitTiger() {
	glm::vec2 size(108, 85);
	tiger[0] = new Tiger((board->nodes[0].GetPosition() + glm::vec2(23, 23) - (size) * 0.5f), size, ResourceManager::GetTexture("tiger"));
	tiger[1] = new Tiger((board->nodes[3].GetPosition() + glm::vec2(23, 23) - (size) * 0.5f), size, ResourceManager::GetTexture("tiger"));
	tiger[2] = new Tiger((board->nodes[4].GetPosition() + glm::vec2(23, 23) - (size) * 0.5f), size, ResourceManager::GetTexture("tiger"));

}

Node* Game::CheckNodeClick(glm::vec2 clickPos)
{
	std::cout << "In node Click" << std::endl;
	for (int i = 0; i < 23; i++)
	{
		if (clickPos.x >= board->nodes[i].GetPosition().x && clickPos.x <= board->nodes[i].GetPosition().x + 46)
			if (clickPos.y >= board->nodes[i].GetPosition().y && clickPos.y <= board->nodes[i].GetPosition().y + 46) {
				return &board->nodes[i];
			}
	}

	return nullptr;
}

Sheep* Game::CheckSheepClick(glm::vec2 clickPos)
{
	for (int i = 0; i < 15; i++)
	{
		if (sheep[i]->isEaten)
			continue;
		if (clickPos.x >= sheep[i]->Position.x && clickPos.x <= sheep[i]->Position.x + sheep[i]->Size.x)
			if (clickPos.y >= sheep[i]->Position.y && clickPos.y <= sheep[i]->Position.y + sheep[i]->Size.y)
				return sheep[i];
	}

	return nullptr;
}

Tiger*  Game::CheckTigerClick(glm::vec2 clickPos)
{
	for (int i = 0; i < 3; i++)
	{
		if (clickPos.x >= tiger[i]->Position.x && clickPos.x <= tiger[i]->Position.x + tiger[i]->Size.x)
			if (clickPos.y >= tiger[i]->Position.y && clickPos.y <= tiger[i]->Position.y + tiger[i]->Size.y)
				return tiger[i];
	}

	return nullptr;
}

bool Game::CheckButtonClick(glm::vec2 clickPos, GameObject* button)
{
	if (clickPos.x >= button->Position.x && clickPos.x <= button->Position.x + button->Size.x)
		if (clickPos.y >= button->Position.y && clickPos.y <= button->Position.y + button->Size.y)
			return true;

	return false;
}

Node* Game::GetActiveNode() {
	for (int i = 0; i < 23; i++) {
		if (board->nodes[i].isActive)
			return &board->nodes[i];
	}

	return nullptr;
}

Sheep* Game::GetActiveSheep() {
	for (Sheep*& currSheep : sheep) {
		if (currSheep->IsActive)
			return currSheep;
	}

	return nullptr;
}

Node* Game::GetNodeUnderGameObject(GameObject* gb) {
	for (int i = 0; i < 23; i++) {
		glm::vec2 pos = (board->nodes[i].GetPosition() + glm::vec2(23, 23) - (gb->Size) * 0.5f);
		if (pos == gb->Position)
			return &(board->nodes[i]);
	}

	return nullptr;
}

Sheep* Game::GetSheepOverNode(Node* node) {
	for (Sheep* currSheep : sheep) {
		if (currSheep->isEaten)
			continue;
		glm::vec2 pos = (node->GetPosition() + glm::vec2(23, 23) - (currSheep->Size * 0.5f));

		if (pos == currSheep->Position)
			return currSheep;
	}

	return nullptr;
}

void Game::SetOccupiedNodes() {

	for (int i = 0; i < 23; i++) {
		board->nodes[i].SetOccupiedBySheep(false);
		board->nodes[i].SetOccupiedByTiger(false);
	}

	for (Sheep* currSheep : sheep) {
		if (currSheep->isEaten)
			continue;
		Node* nodeUnderSheep = GetNodeUnderGameObject(currSheep);
		if (nodeUnderSheep != nullptr) {
			nodeUnderSheep->SetOccupiedBySheep(true);
		}
	}

	for (Tiger* currTiger : tiger) {
		Node* nodeUnderTiger = GetNodeUnderGameObject(currTiger);
		if (nodeUnderTiger != nullptr) {
			nodeUnderTiger->SetOccupiedByTiger(true);
		}
	}
}

void Game::SetStuckGameObjects() {
	SetOccupiedNodes();

	for (Sheep* currSheep : sheep) {
		if (currSheep->isEaten)
			continue;
		Node* nodeUnderSheep = GetNodeUnderGameObject(currSheep);
		if (nodeUnderSheep != nullptr) {
			bool flag = true;
			for (auto itr = nodeUnderSheep->connected.begin(); itr != nodeUnderSheep->connected.end(); itr++) {
				if ((*itr)->GetOccupiedBySheep() == false && (*itr)->GetOccupiedByTiger() == false) {
					flag = false;
					break;
				}
			}

			if (flag) {
				currSheep->Color = glm::vec4(1, 0, 0, 1);
				currSheep->SetStuck(true);
			}
			else {
				if(!currSheep->IsActive)
					currSheep->Color = glm::vec4(1, 1, 1, 1);
				currSheep->SetStuck(false);
			}
		}
	}

	bool allTigerStuck = true;
	for (Tiger* currTiger : tiger) {
		Node* nodeUnderTiger = GetNodeUnderGameObject(currTiger);
		if (nodeUnderTiger != nullptr) {
			bool flag = true;
			for (auto itr = nodeUnderTiger->connected.begin(); itr != nodeUnderTiger->connected.end(); itr++) {

				if ((*itr)->GetOccupiedByTiger())
					continue;

				if ((*itr)->GetOccupiedBySheep() == false && (*itr)->GetOccupiedByTiger() == false) {
					flag = false;
					break;
				}

				Node* connectedNode = (*itr);
				for (auto itr1 = connectedNode->connected.begin(); itr1 != connectedNode->connected.end(); itr1++) {

					glm::vec2 dirNodeUnderTigerToConnectedNode = glm::normalize(connectedNode->GetPosition() - nodeUnderTiger->GetPosition());
					glm::vec2 dirConnectedNodeToitr1 = glm::normalize((*itr1)->GetPosition() - connectedNode->GetPosition());

					glm::vec2 errorVec = dirNodeUnderTigerToConnectedNode - dirConnectedNodeToitr1;

					if (glm::length(errorVec) >= 1.0f)
						continue;

					if ((*itr1)->GetOccupiedBySheep() == false && (*itr1)->GetOccupiedByTiger() == false) {
						flag = false;
						break;
					}
				}

				if (!flag)
					break;
			}

			if (flag) {
				currTiger->Color = glm::vec4(1, 0, 0, 1);
				currTiger->SetStuck(true);
			}
			else {
				if (!currTiger->IsActive)
					currTiger->Color = glm::vec4(1, 1, 1, 1);

				currTiger->SetStuck(false);
			}
		}

		if (!currTiger->GetStuck())
			allTigerStuck = false;
	}

	if (allTigerStuck) {
		sheepWon = true;
		//SceneManager::LoadScene(2, &State);
	}
}

Tiger* Game::GetActiveTiger() {
	for (Tiger* currTiger : tiger) {
		if (currTiger->IsActive)
			return currTiger;
	}

	return nullptr;
}

void ShuffleTiger() {
	for (int i = 0; i < 3 - 1; i++) {
		int j = i + rand() % (3 - i);
		std::swap(tiger[i], tiger[j]);
	}
}

bool AllGameObjectsAtRest() {
	for (Tiger* currTiger : tiger) {
		if (currTiger->Velocity != glm::vec2(0))
			return false;
	}
	
	for (Sheep* currSheep : sheep) {
		if (currSheep->Velocity != glm::vec2(0))
			return false;
	}

	return true;
}

void Game::CheckMouseHover() {

	if (State == GAME_MENU) {
		button_Play->Sprite = ResourceManager::GetTexture("button_Play");
		button_PVP->Sprite = ResourceManager::GetTexture("button_PVP");
		button_Exit->Sprite = ResourceManager::GetTexture("button_Exit");
		button_Rules->Sprite = ResourceManager::GetTexture("button_Rules");

		if (CheckButtonClick(mousePosition, button_Play))
			button_Play->Sprite = ResourceManager::GetTexture("button_Play_Hover");
		else if (CheckButtonClick(mousePosition, button_PVP))
			button_PVP->Sprite = ResourceManager::GetTexture("button_PVP_Hover");
		else if (CheckButtonClick(mousePosition, button_Exit))
			button_Exit->Sprite = ResourceManager::GetTexture("button_Exit_Hover");
		else if (CheckButtonClick(mousePosition, button_Rules))
			button_Rules->Sprite = ResourceManager::GetTexture("button_Rules_Hover");
	}
	else if (State == GAME_RULES) {
		button_back->Sprite = ResourceManager::GetTexture("button_back");

		if (CheckButtonClick(mousePosition, button_back))
			button_back->Sprite = ResourceManager::GetTexture("button_back_Hover");
	}
	else if (State == GAME_AI || State == GAME_PVP) {
		button_home->Sprite = ResourceManager::GetTexture("button_Home");
		button_Rules2->Sprite = ResourceManager::GetTexture("button_Rules");

		if (CheckButtonClick(mousePosition, button_home))
			button_home->Sprite = ResourceManager::GetTexture("button_Home_Hover");
		else if (CheckButtonClick(mousePosition, button_Rules2)) {
			button_Rules2->Sprite = ResourceManager::GetTexture("button_Rules_Hover");
		}

		if (sheepWon || tigerWon) {
			button_home2->Sprite = ResourceManager::GetTexture("button_Home2");
			button_replay->Sprite = ResourceManager::GetTexture("button_Replay");

			if (CheckButtonClick(mousePosition, button_home2)) {
				button_home2->Sprite = ResourceManager::GetTexture("button_Home2_Hover");
			}
			else if (CheckButtonClick(mousePosition, button_replay)) {
				button_replay->Sprite = ResourceManager::GetTexture("button_Replay_Hover");
			}
		}
	}
}