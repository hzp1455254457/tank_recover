#pragma once

#include "Timer.h"
#include "Random.h"
#include "../graphics/Renderer.h"
#include "../input/InputManager.h"
#include "../gameplay/PlayerTank.h"
#include "../gameplay/EnemyTank.h"
#include "../gameplay/Bullet.h"
#include "../gameplay/PowerUp.h"
#include "../level/LevelManager.h"
#include "../ui/HUD.h"
#include <memory>
#include <vector>

namespace BattleCity {

// Main game class - central controller
class Game {
private:
    // Core systems
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<InputManager> inputManager_;
    std::unique_ptr<Timer> timer_;
    std::unique_ptr<Random> random_;

    // Game state
    GameState currentState_;
    int currentLevel_;
    int highScore_;

    // Players
    std::unique_ptr<PlayerTank> player1_;
    std::unique_ptr<PlayerTank> player2_;
    bool isTwoPlayerMode_;

    // Enemies
    std::vector<std::unique_ptr<EnemyTank>> enemies_;
    std::vector<std::unique_ptr<Bullet>> bullets_;

    // Power-ups
    std::vector<std::unique_ptr<PowerUp>> powerUps_;

    // UI
    HUD hud_;

    // Menu system
    enum class MenuItem {
        ONE_PLAYER_GAME,
        TWO_PLAYER_GAME,
        COUNT
    };
    MenuItem selectedMenuItem_;

    // Menu animation state
    int menuBlinkFrame_;  // For selection blinking effect (5Hz)
    int confirmAnimationFrame_;  // For confirmation animation (3 quick flashes)
    bool isConfirmAnimating_;    // Whether confirmation animation is active
    int menuFadeInFrame_;        // For title fade-in animation (0-30 frames)
    int menuSlideInFrame_;       // For menu options slide-in animation (0-18 frames)
    
    // Game start transition state
    bool isShowingStageTransition_;  // Whether showing "STAGE 01" transition
    int stageTransitionFrame_;       // Frame counter for stage transition (0-90 frames = 1.5s)
    bool isShowingLoading_;         // Whether showing loading prompt
    int loadingFrame_;               // Frame counter for loading animation

    // Level management
    std::unique_ptr<LevelManager> levelManager_;

    // Game timing
    uint64_t gameStartTime_;
    bool isPaused_;

public:
    Game();
    ~Game() = default;

    // Main game loop
    bool init();
    bool run(); // Returns true if game should exit
    void shutdown();

    // Game state management
    void changeState(GameState newState);
    GameState getCurrentState() const { return currentState_; }

    // Level management
    void startLevel(int level);
    void nextLevel();
    void restartLevel();
    bool isLevelComplete() const;
    bool isGameOver() const;

    // Player management
    void addPlayerScore(int player, int score);
    void playerDied(int player);

    // Pause/Resume
    void pause();
    void resume();
    bool isPaused() const { return isPaused_; }

    // Bullet management
    void shootBullet(const Vector2& position, Direction direction, BulletOwner owner, int power = 1);

    // Enemy management
    void spawnEnemy(EnemyType type, const Vector2& position);

    // Power-up management
    void spawnPowerUp(const Vector2& position);
    void checkPowerUpCollisions();

    // Getters for external access
    PlayerTank* getPlayer1() const { return player1_.get(); }
    PlayerTank* getPlayer2() const { return player2_.get(); }
    int getCurrentLevel() const { return levelManager_->getCurrentLevel(); }
    bool isTwoPlayerMode() const { return player2_ != nullptr; }

private:
    // Update methods
    void update();
    void updateMenu();
    void updatePlaying();
    void updatePaused();
    void updateGameOver();
    void updateLevelComplete();

    // Render methods
    void render();
    void renderMenu();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    void renderLevelComplete();
    void renderUI();
    void renderPlayingUI();

    // Input handling
    void handleInput();

    // Helper methods
    void loadHighScore();
    void saveHighScore();
    void resetGame();
    void initPlayers();
    void cleanupLevel();
};

} // namespace BattleCity