#include "Game.h"
#include "../ui/HUD.h"
#include "../ui/UIManager.h"
#include <iostream>
#include <fstream>

namespace BattleCity {

Game::Game()
    : currentState_(GameState::MENU), currentLevel_(1), highScore_(0),
      isTwoPlayerMode_(false), gameStartTime_(0), isPaused_(false),
      selectedMenuItem_(MenuItem::ONE_PLAYER_GAME) {

    // Initialize core systems
    renderer_ = std::make_unique<Renderer>(3, true); // 3x scale, VSync
    inputManager_ = std::make_unique<InputManager>();
    timer_ = std::make_unique<Timer>();
    random_ = std::make_unique<Random>();

    // Initialize level manager
    levelManager_ = std::make_unique<LevelManager>(*random_);
}

bool Game::init() {
    // Initialize renderer
    if (!renderer_->init()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }

    // Load high score
    loadHighScore();
    // Ensure we start at the menu (do not auto-start players/levels)
    currentState_ = GameState::MENU;
    isPaused_ = false;

    std::cout << "Battle City initialized successfully!" << std::endl;
    // Write log and show a modal message box so user can focus the window and we can debug rendering
    {
        std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
        if (log) log << "Game::init() completed, showing debug messagebox\n";
    }
    // Show modal message box (SDL) to pause and allow manual focus/inspection
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Battle City - Debug", "Initialization complete.\nPress OK to continue.", renderer_->getWindow());
    // Ensure window is raised/focused so keyboard input works
    SDL_RaiseWindow(renderer_->getWindow());
    return true;
}

bool Game::run() {
    bool running = true;
    bool shouldExit = false;
    std::cout << "Game::run() start" << std::endl;
    {
        std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
        if (log) log << "Game::run() start\n";
    }

    while (running && !shouldExit) {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                std::cout << "SDL_QUIT event received" << std::endl;
                running = false;
            }
            inputManager_->handleEvent(event);
        }

        // Update input
        inputManager_->update();

        // If window doesn't have input focus, log once
        Uint32 winFlags = SDL_GetWindowFlags(renderer_->getWindow());
        if ((winFlags & SDL_WINDOW_INPUT_FOCUS) == 0) {
            static bool warnedNoFocus = false;
            if (!warnedNoFocus) {
                std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
                if (log) log << "Warning: Window has no input focus\n";
                warnedNoFocus = true;
            }
        }

        // Update game logic (60 FPS)
        timer_->update([this]() { this->update(); },
                      [this](double alpha) { this->render(); });

        // Check for quit condition
        if (inputManager_->isJustPressed(GameAction::QUIT, 0)) {
            std::cout << "Input QUIT pressed" << std::endl;
            running = false;
        }

        // Check if menu requested exit
        if (currentState_ == GameState::MENU && selectedMenuItem_ == MenuItem::EXIT) {
            // Wait for confirmation
            static bool exitConfirmed = false;
            if (!exitConfirmed && (inputManager_->isJustPressed(GameAction::SHOOT, 0) ||
                                  inputManager_->isJustPressed(GameAction::START, 0))) {
                std::cout << "Exit confirmed by user" << std::endl;
                exitConfirmed = true;
                shouldExit = true;
            }
        }
    }
    std::cout << "Game::run() exiting" << std::endl;
    return shouldExit;
}

void Game::shutdown() {
    saveHighScore();
    std::cout << "Battle City shutdown complete." << std::endl;
}

void Game::changeState(GameState newState) {
    currentState_ = newState;
    std::cout << "Game::changeState -> " << static_cast<int>(newState) << std::endl;

    switch (newState) {
        case GameState::MENU:
            break;
        case GameState::PLAYING:
            if (!player1_) {
                initPlayers();
            }
            startLevel(currentLevel_);
            gameStartTime_ = timer_->getFrameCount();
            break;
        case GameState::PAUSED:
            isPaused_ = true;
            break;
        case GameState::GAME_OVER:
            saveHighScore();
            break;
        case GameState::LEVEL_COMPLETE:
            break;
    }
}

void Game::startLevel(int level) {
    currentLevel_ = level;
    levelManager_->loadLevel(level);
    cleanupLevel();

    // Reset player positions
    if (player1_) {
        player1_->setPosition(Vector2::fromPixels(80, 200));
    }
    if (player2_ && isTwoPlayerMode_) {
        player2_->setPosition(Vector2::fromPixels(160, 200));
    }
}

void Game::shootBullet(const Vector2& position, Direction direction, BulletOwner owner, int power) {
    // Limit maximum bullets on screen
    int maxBullets = (owner == BulletOwner::ENEMY) ? 1 : 1; // Enemy can have 1, players can have 1 each
    int currentOwnerBullets = 0;

    for (auto& bullet : bullets_) {
        if (bullet->isActive() && bullet->getOwner() == owner) {
            currentOwnerBullets++;
        }
    }

    if (currentOwnerBullets >= maxBullets) {
        return; // Can't shoot more bullets
    }

    // Create new bullet
    auto bullet = std::make_unique<Bullet>();
    bullet->init(position, direction, owner, power);
    bullets_.push_back(std::move(bullet));
}

void Game::spawnEnemy(EnemyType type, const Vector2& position) {
    // Limit maximum enemies on screen (original game: 4 max)
    int maxEnemies = 4;
    int activeEnemies = 0;

    for (auto& enemy : enemies_) {
        if (enemy && enemy->isActive()) {
            activeEnemies++;
        }
    }

    if (activeEnemies >= maxEnemies) {
        return; // Can't spawn more enemies
    }

    // Create new enemy
    auto enemy = std::make_unique<EnemyTank>(type, this);
    enemy->setPosition(position);
    enemies_.push_back(std::move(enemy));
}

void Game::spawnPowerUp(const Vector2& position) {
    // Limit maximum power-ups on screen (original game: 1 max)
    int maxPowerUps = 1;
    int activePowerUps = 0;

    for (auto& powerUp : powerUps_) {
        if (powerUp && powerUp->isActive()) {
            activePowerUps++;
        }
    }

    if (activePowerUps >= maxPowerUps) {
        return; // Can't spawn more power-ups
    }

    // Create random power-up
    auto powerUp = PowerUp::createRandomPowerUp();
    if (powerUp) {
        powerUp->setPosition(position);
        powerUps_.push_back(std::move(powerUp));
    }
}

void Game::checkPowerUpCollisions() {
    for (auto& powerUp : powerUps_) {
        if (!powerUp || !powerUp->isActive()) continue;

        // Check collision with players
        if (player1_ && player1_->isActive() && powerUp->collidesWithTank(*player1_)) {
            powerUp->activate(*this);
            continue;
        }
        if (player2_ && player2_->isActive() && isTwoPlayerMode_ && powerUp->collidesWithTank(*player2_)) {
            powerUp->activate(*this);
            continue;
        }
    }

    // Remove inactive power-ups
    powerUps_.erase(
        std::remove_if(powerUps_.begin(), powerUps_.end(),
            [](const std::unique_ptr<PowerUp>& powerUp) {
                return !powerUp->isActive();
            }),
        powerUps_.end());
}

void Game::nextLevel() {
    currentLevel_++;
    if (currentLevel_ > 35) {
        // Game completed!
        changeState(GameState::MENU);
        return;
    }
    startLevel(currentLevel_);
}

void Game::restartLevel() {
    startLevel(currentLevel_);
}

bool Game::isLevelComplete() const {
    return levelManager_->isLevelComplete();
}

bool Game::isGameOver() const {
    if (player1_ && player1_->isGameOver()) return true;
    if (player2_ && isTwoPlayerMode_ && player2_->isGameOver()) return true;
    return false;
}

void Game::addPlayerScore(int player, int score) {
    if (player == 0 && player1_) {
        player1_->addScore(score);
    } else if (player == 1 && player2_) {
        player2_->addScore(score);
    }
}

void Game::playerDied(int player) {
    if (player == 0 && player1_) {
        player1_->loseLife();
    } else if (player == 1 && player2_) {
        player2_->loseLife();
    }

    if (isGameOver()) {
        changeState(GameState::GAME_OVER);
    }
}

void Game::pause() {
    if (currentState_ == GameState::PLAYING) {
        changeState(GameState::PAUSED);
    }
}

void Game::resume() {
    if (currentState_ == GameState::PAUSED) {
        changeState(GameState::PLAYING);
        isPaused_ = false;
    }
}

void Game::update() {
    switch (currentState_) {
        case GameState::MENU:
            updateMenu();
            break;
        case GameState::PLAYING:
            updatePlaying();
            break;
        case GameState::PAUSED:
            updatePaused();
            break;
        case GameState::GAME_OVER:
            updateGameOver();
            break;
        case GameState::LEVEL_COMPLETE:
            updateLevelComplete();
            break;
        case GameState::DEMO:
            // Demo mode not implemented yet
            break;
    }
}

void Game::render() {
    std::cout << "Game::render state=" << static_cast<int>(currentState_) << std::endl;
    renderer_->clear();

    switch (currentState_) {
        case GameState::MENU:
            renderMenu();
            // Debug: log menu state and key raw states each render
            {
                std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
                if (log) {
                    log << "Render: MENU selected=" << static_cast<int>(selectedMenuItem_)
                        << " up=" << inputManager_->isPressed(GameAction::UP, 0)
                        << " down=" << inputManager_->isPressed(GameAction::DOWN, 0)
                        << " shoot=" << inputManager_->isPressed(GameAction::SHOOT, 0)
                        << " start=" << inputManager_->isPressed(GameAction::START, 0)
                        << "\n";
                }
            }
            break;
        case GameState::PLAYING:
        case GameState::PAUSED:
            renderPlaying();
            if (currentState_ == GameState::PAUSED) {
                renderPaused();
            }
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::LEVEL_COMPLETE:
            renderLevelComplete();
            break;
        case GameState::DEMO:
            // Demo mode not implemented yet
            break;
    }

    renderUI();
    renderer_->present();
}

void Game::updateMenu() {
    // Handle menu navigation
    // Debug: log input states when in menu
    bool upPressed = inputManager_->isPressed(GameAction::UP, 0);
    bool downPressed = inputManager_->isPressed(GameAction::DOWN, 0);
    bool shootJust = inputManager_->isJustPressed(GameAction::SHOOT, 0);
    bool startJust = inputManager_->isJustPressed(GameAction::START, 0);
    if (upPressed || downPressed || shootJust || startJust) {
        std::ofstream log("D:/tankRecover/build/Release/debug_log.txt", std::ios::app);
        if (log) log << "Menu input state: up=" << upPressed << " down=" << downPressed
                     << " shootJust=" << shootJust << " startJust=" << startJust
                     << " selected=" << static_cast<int>(selectedMenuItem_) << "\n";
    }

    if (inputManager_->isJustPressed(GameAction::DOWN, 0)) {
        // Move to next menu item
        int nextItem = static_cast<int>(selectedMenuItem_) + 1;
        if (nextItem >= static_cast<int>(MenuItem::COUNT)) {
            nextItem = 0;
        }
        selectedMenuItem_ = static_cast<MenuItem>(nextItem);
        std::cout << "Menu: Selected item " << static_cast<int>(selectedMenuItem_) << std::endl;
    }
    else if (inputManager_->isJustPressed(GameAction::UP, 0)) {
        // Move to previous menu item
        int prevItem = static_cast<int>(selectedMenuItem_) - 1;
        if (prevItem < 0) {
            prevItem = static_cast<int>(MenuItem::COUNT) - 1;
        }
        selectedMenuItem_ = static_cast<MenuItem>(prevItem);
        std::cout << "Menu: Selected item " << static_cast<int>(selectedMenuItem_) << std::endl;
    }
    else if (inputManager_->isJustPressed(GameAction::SHOOT, 0) ||
             inputManager_->isJustPressed(GameAction::START, 0)) {
        // Confirm selection
        std::cout << "Menu: Confirmed selection " << static_cast<int>(selectedMenuItem_) << std::endl;
        switch (selectedMenuItem_) {
            case MenuItem::ONE_PLAYER_GAME:
                std::cout << "Starting 1 player game" << std::endl;
                isTwoPlayerMode_ = false;
                changeState(GameState::PLAYING);
                break;
            case MenuItem::TWO_PLAYER_GAME:
                std::cout << "Starting 2 player game" << std::endl;
                isTwoPlayerMode_ = true;
                changeState(GameState::PLAYING);
                break;
            case MenuItem::HIGH_SCORES:
                // TODO: Show high scores screen
                std::cout << "High scores not implemented yet" << std::endl;
                break;
            case MenuItem::EXIT:
                std::cout << "Exiting game" << std::endl;
                // Note: Exit will be handled in the main game loop
                break;
        }
    }
}

void Game::updatePlaying() {
    if (isPaused_) return;

    // Update players
    if (player1_) {
        player1_->handleInput(*inputManager_);
        player1_->update();
    }
    if (player2_ && isTwoPlayerMode_) {
        player2_->handleInput(*inputManager_);
        player2_->update();
    }

    // Update enemies
    for (auto& enemy : enemies_) {
        if (enemy && enemy->isActive()) {
            enemy->update();
        }
    }

    // Update bullets
    for (auto& bullet : bullets_) {
        if (bullet && bullet->isActive()) {
            bullet->update();

            // Check bullet collisions
            // With terrain
            if (bullet->checkCollisionWithTerrain(*levelManager_)) {
                continue;
            }

            // With base
            Vector2 basePos = levelManager_->getCurrentLevelData().basePosition;
            if (bullet->checkCollisionWithBase(basePos)) {
                changeState(GameState::GAME_OVER);
                continue;
            }

            // With players
            if (player1_ && bullet->getOwner() == BulletOwner::ENEMY) {
                if (bullet->checkCollisionWithTank(*player1_)) {
                    if (player1_->isGameOver()) {
                        changeState(GameState::GAME_OVER);
                    }
                    continue;
                }
            }
            if (player2_ && isTwoPlayerMode_ && bullet->getOwner() == BulletOwner::ENEMY) {
                if (bullet->checkCollisionWithTank(*player2_)) {
                    if (player2_->isGameOver()) {
                        changeState(GameState::GAME_OVER);
                    }
                    continue;
                }
            }

            // With enemies
            if (bullet->getOwner() != BulletOwner::ENEMY) {
                for (auto& enemy : enemies_) {
                    if (enemy && enemy->isActive()) {
                        if (bullet->checkCollisionWithTank(*enemy)) {
                            Vector2 enemyPos = enemy->getPosition();

                            // Calculate and add score for destroying enemy
                            if (player1_ && player1_->isActive()) {
                                int score = player1_->calculateEnemyScore(enemy->getType());
                                player1_->addScore(score);
                            }

                            enemy->destroy();
                            levelManager_->enemyDestroyed();

                            // Check if should spawn power-up
                            if (levelManager_->shouldSpawnPowerUp()) {
                                Vector2 powerUpPos = levelManager_->getPowerUpSpawnPosition(enemyPos);
                                spawnPowerUp(powerUpPos);
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    // Remove inactive bullets
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const std::unique_ptr<Bullet>& bullet) {
                return !bullet->isActive();
            }),
        bullets_.end());

    // Remove inactive enemies
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<EnemyTank>& enemy) {
                return !enemy->isActive();
            }),
        enemies_.end());

    // Update power-ups
    for (auto& powerUp : powerUps_) {
        if (powerUp && powerUp->isActive()) {
            powerUp->update();
        }
    }

    // Check power-up collisions
    checkPowerUpCollisions();

    // Update level
    levelManager_->update();

    // Check level completion
    if (isLevelComplete()) {
        changeState(GameState::LEVEL_COMPLETE);
    }

    // Check game over
    if (isGameOver()) {
        changeState(GameState::GAME_OVER);
    }
}

void Game::updatePaused() {
    // Handle pause input
    if (inputManager_->isJustPressed(GameAction::START, 0) ||
        inputManager_->isJustPressed(GameAction::PAUSE, 0)) {
        resume();
    }
}

void Game::updateGameOver() {
    // Handle game over input
    if (inputManager_->isJustPressed(GameAction::START, 0)) {
        resetGame();
        changeState(GameState::MENU);
    }
}

void Game::updateLevelComplete() {
    // Auto-advance to next level after a delay
    static int delay = 0;
    delay++;
    if (delay >= 180) { // 3 seconds at 60fps
        delay = 0;
        nextLevel();
    }
}

void Game::renderMenu() {
    // Render title
    renderer_->drawText(80, 60, "BATTLE CITY", BattleCityPalette::COLOR_WHITE);

    // Render menu options with selection highlighting
    const char* menuTexts[] = {
        "1 PLAYER GAME",
        "2 PLAYER GAME",
        "HIGH SCORES",
        "EXIT"
    };

    int menuY = 120;
    for (int i = 0; i < static_cast<int>(MenuItem::COUNT); ++i) {
        uint8_t color = (i == static_cast<int>(selectedMenuItem_)) ?
                       BattleCityPalette::COLOR_YELLOW : BattleCityPalette::COLOR_WHITE;
        renderer_->drawText(100, menuY + i * 20, menuTexts[i], color);
    }

    // Render high score
    char highScoreText[32];
    sprintf(highScoreText, "HI SCORE %06d", highScore_);
    renderer_->drawText(80, 200, highScoreText, BattleCityPalette::COLOR_WHITE);

    // Render instruction
    renderer_->drawText(60, 220, "PRESS UP/DOWN TO SELECT, ENTER TO CONFIRM", BattleCityPalette::COLOR_WHITE);
}

void Game::renderPlaying() {
    // Render level terrain (would be implemented)
    // levelManager_->render(*renderer_);

    // Render players
    if (player1_) {
        player1_->render(*renderer_);
    }
    if (player2_ && isTwoPlayerMode_) {
        player2_->render(*renderer_);
    }

    // Render enemies
    for (auto& enemy : enemies_) {
        if (enemy && enemy->isActive()) {
            enemy->render(*renderer_);
        }
    }

    // Render bullets
    for (auto& bullet : bullets_) {
        if (bullet && bullet->isActive()) {
            bullet->render(*renderer_);
        }
    }

    // Render power-ups
    for (auto& powerUp : powerUps_) {
        if (powerUp && powerUp->isActive()) {
            powerUp->render(*renderer_);
        }
    }

    // Render HUD
    int score = player1_ ? player1_->getScore() : 0;
    int lives = player1_ ? player1_->getLives() : 0;
    int level = getCurrentLevel();
    bool twoPlayerMode = isTwoPlayerMode();
    hud_.render(*renderer_, score, lives, level, twoPlayerMode);
}

void Game::renderPaused() {
    // Render pause overlay
    renderer_->drawText(120, 100, "PAUSED", BattleCityPalette::COLOR_WHITE);
    renderer_->drawText(80, 120, "PRESS START TO RESUME", BattleCityPalette::COLOR_WHITE);
}

void Game::renderGameOver() {
    // Render game over screen
    renderer_->drawText(110, 100, "GAME OVER", BattleCityPalette::COLOR_WHITE);

    // Show final scores
    if (player1_) {
        char scoreText[32];
        sprintf(scoreText, "SCORE: %06d", player1_->getScore());
        renderer_->drawText(100, 130, scoreText, BattleCityPalette::COLOR_WHITE);
    }
}

void Game::renderLevelComplete() {
    // Render level complete message
    renderer_->drawText(100, 100, "NEXT STAGE", BattleCityPalette::COLOR_WHITE);
}

void Game::renderUI() {
    // Render common UI elements
    switch (currentState_) {
        case GameState::PLAYING:
        case GameState::PAUSED:
            renderPlayingUI();
            break;
        default:
            break;
    }
}

void Game::renderPlayingUI() {
    // Use HUD class for rendering UI elements
    if (player1_) {
        int score = player1_->getScore();
        int lives = player1_->getLives();
        int level = currentLevel_;
        bool isTwoPlayerMode = isTwoPlayerMode_;

        std::cout << "Game::renderPlayingUI calling HUD::render with score=" << score
                  << " lives=" << lives << " level=" << level << " twoPlayer=" << isTwoPlayerMode << std::endl;

        hud_.render(*renderer_, score, lives, level, isTwoPlayerMode);
    } else {
        // Fallback to basic HUD if no player
        UIManager::renderHUD(*renderer_, 0, 0, currentLevel_);
    }
}

void Game::loadHighScore() {
    // Load from file (simplified)
    highScore_ = 0; // Would load from config file
}

void Game::saveHighScore() {
    // Save to file (simplified)
    if (player1_ && player1_->getScore() > highScore_) {
        highScore_ = player1_->getScore();
    }
    if (player2_ && player2_->getScore() > highScore_) {
        highScore_ = player2_->getScore();
    }
    // Would save to config file
}

void Game::resetGame() {
    currentLevel_ = 1;
    player1_.reset();
    player2_.reset();
    levelManager_->loadLevel(1);
}

void Game::initPlayers() {
    player1_ = std::make_unique<PlayerTank>(0, this);
    if (isTwoPlayerMode_) {
        player2_ = std::make_unique<PlayerTank>(1, this);
    }
}

void Game::cleanupLevel() {
    // Reset level-specific objects
    // enemyManager_->clear();
    // powerUpManager_->clear();
    // bulletManager_->clear();
}

} // namespace BattleCity