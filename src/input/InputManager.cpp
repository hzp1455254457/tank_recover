#include "InputManager.h"
#include <cstring>

namespace BattleCity {

InputManager::InputManager() {
    initDefaultMappings();
}

void InputManager::initDefaultMappings() {
    // Player 1 default mappings
    player1Mappings_[GameAction::UP] = SDLK_UP;
    player1Mappings_[GameAction::DOWN] = SDLK_DOWN;
    player1Mappings_[GameAction::LEFT] = SDLK_LEFT;
    player1Mappings_[GameAction::RIGHT] = SDLK_RIGHT;
    player1Mappings_[GameAction::SHOOT] = SDLK_SPACE;
    player1Mappings_[GameAction::START] = SDLK_RETURN;
    player1Mappings_[GameAction::PAUSE] = SDLK_p;

    // Player 2 default mappings
    player2Mappings_[GameAction::UP] = SDLK_w;
    player2Mappings_[GameAction::DOWN] = SDLK_s;
    player2Mappings_[GameAction::LEFT] = SDLK_a;
    player2Mappings_[GameAction::RIGHT] = SDLK_d;
    player2Mappings_[GameAction::SHOOT] = SDLK_LCTRL;
    player2Mappings_[GameAction::START] = SDLK_RETURN;
    player2Mappings_[GameAction::PAUSE] = SDLK_p;
}

void InputManager::update() {
    // Copy current states to previous
    previousKeyStates_ = currentKeyStates_;

    // Update current key states
    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    // Update all mapped keys
    for (const auto& mapping : player1Mappings_) {
        currentKeyStates_[mapping.second] = keyboardState[SDL_GetScancodeFromKey(mapping.second)];
    }
    for (const auto& mapping : player2Mappings_) {
        currentKeyStates_[mapping.second] = keyboardState[SDL_GetScancodeFromKey(mapping.second)];
    }
}

bool InputManager::isPressed(GameAction action, int player) const {
    SDL_Keycode key = getKeyForAction(action, player);
    auto it = currentKeyStates_.find(key);
    return it != currentKeyStates_.end() && it->second;
}

bool InputManager::isJustPressed(GameAction action, int player) const {
    SDL_Keycode key = getKeyForAction(action, player);
    auto currentIt = currentKeyStates_.find(key);
    auto previousIt = previousKeyStates_.find(key);

    bool currentPressed = currentIt != currentKeyStates_.end() && currentIt->second;
    bool previousPressed = previousIt != previousKeyStates_.end() && previousIt->second;

    return currentPressed && !previousPressed;
}

bool InputManager::isJustReleased(GameAction action, int player) const {
    SDL_Keycode key = getKeyForAction(action, player);
    auto currentIt = currentKeyStates_.find(key);
    auto previousIt = previousKeyStates_.find(key);

    bool currentPressed = currentIt != currentKeyStates_.end() && currentIt->second;
    bool previousPressed = previousIt != previousKeyStates_.end() && previousIt->second;

    return !currentPressed && previousPressed;
}

void InputManager::handleEvent(const SDL_Event& event) {
    // Handle special events like quit, etc.
    // Most input is handled via polling in update()
}

bool InputManager::getKeyState(SDL_Keycode key) const {
    auto it = currentKeyStates_.find(key);
    return it != currentKeyStates_.end() && it->second;
}

void InputManager::setPlayer1Mapping(GameAction action, SDL_Keycode key) {
    player1Mappings_[action] = key;
}

void InputManager::setPlayer2Mapping(GameAction action, SDL_Keycode key) {
    player2Mappings_[action] = key;
}

SDL_Keycode InputManager::getKeyForAction(GameAction action, int player) const {
    if (player == 0) {
        auto it = player1Mappings_.find(action);
        return it != player1Mappings_.end() ? it->second : SDLK_UNKNOWN;
    } else {
        auto it = player2Mappings_.find(action);
        return it != player2Mappings_.end() ? it->second : SDLK_UNKNOWN;
    }
}

} // namespace BattleCity