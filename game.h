#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <vector>
#include "node.h"
#include "sheep.h"
#include "tiger.h"
#include "game_object.h"

enum GameState {
    GAME_PVP,
    GAME_AI,
    GAME_MENU,
    GAME_OVER,
    GAME_RULES
};

enum Turn {
    TIGER,
    SHEEP
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State, PrevState;
    Turn                    turn;
    bool                    Keys[1024];
    bool                    Mouse[3];
    unsigned int            Width, Height;
    unsigned int            Lives;
    glm::vec2 endPos = glm::vec2(0, 0);
    glm::vec2 mousePosition = glm::vec2(0, 0);
    bool keysProcessed[1024];
    bool mouseProcessed[3];
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt, GLFWwindow* window);
    void Update(float dt, GLFWwindow* window);
    void Render();
    void DoCollisions();
    // reset
    void ResetLevel();
    void ResetPlayer();

private:
    Node* GetActiveNode();
    Sheep* GetActiveSheep();
    Tiger* GetActiveTiger();
    void SetOccupiedNodes();
    void InitSheep(glm::vec2 startPos);
    void InitTiger();
    bool CanTigerEatSheep(Node* nodeUnderTiger, Node* nodeClicked);
    bool IsInSameDirection(Node* nodeUnderTiger, Node* nodeOccupiedBySheep, Node* nodeClicked);
    void MoveGameObject(GameObject* gb, Node* dest);
    void DeactivateAllGameObjects();
    bool CheckIfAllSheepPlaced();
    bool ChangeScene(const glm::vec2& mouseClick, GLFWwindow* window);
    void SetStuckGameObjects();
    Node* GetNodeUnderGameObject(GameObject* gb);
    bool isNodeClickedConnected(Node* startNode, Node* nodeClicked);
    Sheep* GetSheepOverNode(Node* node);
    bool CheckButtonClick(glm::vec2 clickPos, GameObject* button);
    //check click
    Sheep* CheckSheepClick(glm::vec2 clickPos);
    Tiger* CheckTigerClick(glm::vec2 clickPos);
    Node* CheckNodeClick(glm::vec2 clickPos);
    void CheckMouseHover();
};