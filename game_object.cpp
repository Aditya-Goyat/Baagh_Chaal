#include "game_object.h"
#include <iostream>

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), prevVelocity(Velocity), Color(1.0f), Rotation(0.0f), Sprite(), IsActive(false), Destroyed(false), endPos(0.0f, 0.0f), isStuck(false) {
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec4 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), prevVelocity(Velocity), Color(color), Rotation(0.0f), Sprite(sprite), IsActive(false), Destroyed(false), endPos(pos), isStuck(false) { }

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

bool GameObject::Move(float dt) {
    glm::vec2 diff = endPos - Position;
    float dist = glm::length(diff);
    Velocity = glm::normalize(diff);
    
    if (dist <= 0.1f) {
        Position = endPos;
        if (prevVelocity != glm::vec2(0, 0)) {
            Color = glm::vec4(1, 1, 1, 1);
            Velocity = glm::vec2(0, 0);
            prevVelocity = Velocity;
            return true;
        }
        Velocity = glm::vec2(0, 0);
    }

    prevVelocity = Velocity;
    Position += Velocity * dt * speed;
    return false;
}

bool GameObject::Update(float dt) {
    return Move(dt);
}

float GameObject::Approach(float startPos, float endPos, float dt) {
    float diff = endPos - startPos;

    if (diff > dt)
        return startPos + (dt * speed);

    if (diff < -dt)
        return startPos - (dt * speed);

    return endPos;
}

void GameObject::SetStuck(bool isStuck) {
    this->isStuck = isStuck;
}

bool GameObject::GetStuck() {
    return isStuck;
}