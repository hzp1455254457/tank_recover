#pragma once

#include <SDL.h>
#include <map>
#include "../utils/MathUtils.h"

namespace BattleCity {

// Input manager for keyboard handling
class InputManager {
private:
    std::map<SDL_Keycode, bool> currentKeyStates_;
    std::map<SDL_Keycode, bool> previousKeyStates_;

    // Key mappings for players
    std::map<GameAction, SDL_Keycode> player1Mappings_;
    std::map<GameAction, SDL_Keycode> player2Mappings_;

public:
    InputManager();
    ~InputManager() = default;

    // Update input states (call once per frame)
    void update();

    // Check if action is currently pressed
    bool isPressed(GameAction action, int player = 0) const;

    // Check if action was just pressed (this frame)
    bool isJustPressed(GameAction action, int player = 0) const;

    // Check if action was just released (this frame)
    bool isJustReleased(GameAction action, int player = 0) const;

    // Handle SDL events
    void handleEvent(const SDL_Event& event);

    // Get raw key state
    bool getKeyState(SDL_Keycode key) const;

    // Remap keys
    void setPlayer1Mapping(GameAction action, SDL_Keycode key);
    void setPlayer2Mapping(GameAction action, SDL_Keycode key);

private:
    SDL_Keycode getKeyForAction(GameAction action, int player) const;
    void initDefaultMappings();
};

// Input state structure for easier access
struct InputState {
    bool up, down, left, right, shoot, start, pause;

    InputState() : up(false), down(false), left(false), right(false),
                   shoot(false), start(false), pause(false) {}
};

} // namespace BattleCity