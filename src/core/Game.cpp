#include "Game.h"
#include "../ui/HUD.h"
#include "../ui/UIManager.h"
#include <iostream>
#include <fstream>

namespace BattleCity {

Game::Game()
    : currentState_(GameState::MENU), currentLevel_(1), highScore_(0),
      isTwoPlayerMode_(false), gameStartTime_(0), isPaused_(false),
      selectedMenuItem_(MenuItem::ONE_PLAYER_GAME), menuBlinkFrame_(0),
      confirmAnimationFrame_(0), isConfirmAnimating_(false),
      menuFadeInFrame_(0), menuSlideInFrame_(0),
      isShowingStageTransition_(false), stageTransitionFrame_(0),
      isShowingLoading_(false), loadingFrame_(0) {

    // Initialize core systems
    renderer_ = std::make_unique<Renderer>(3, true); // 3x scale, VSync
    inputManager_ = std::make_unique<InputManager>();
    timer_ = std::make_unique<Timer>();
    random_ = std::make_unique<Random>();

    // Initialize level manager
    levelManager_ = std::make_unique<LevelManager>(*random_);
    
    // Set enemy spawn callback
    levelManager_->setEnemySpawnCallback([this](EnemyType type, const Vector2& position) {
        this->spawnEnemy(type, position);
    });
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
    // Ensure window is raised/focused so keyboard input works
    SDL_RaiseWindow(renderer_->getWindow());
    SDL_SetWindowInputFocus(renderer_->getWindow());
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

        // Check for quit (ESC key or window close)
        // Exit is handled via SDL_QUIT event or ESC key
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
            // Reset menu animations when returning to menu
            menuFadeInFrame_ = 0;
            menuSlideInFrame_ = 0;
            isShowingStageTransition_ = false;
            isShowingLoading_ = false;
            break;
        case GameState::PLAYING:
            // Show loading prompt first
            isShowingLoading_ = true;
            loadingFrame_ = 0;
            
            // Initialize players and level
            if (!player1_) {
                initPlayers();
            }
            startLevel(currentLevel_);
            
            // Show stage transition after loading
            isShowingLoading_ = false;
            isShowingStageTransition_ = true;
            stageTransitionFrame_ = 0;
            
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
            // Update stage transition animation
            if (isShowingStageTransition_) {
                stageTransitionFrame_++;
                // Stage transition lasts 90 frames (1.5 seconds)
                if (stageTransitionFrame_ >= 90) {
                    isShowingStageTransition_ = false;
                }
            }
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
    renderer_->clear();

    switch (currentState_) {
        case GameState::MENU:
            renderMenu();
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
    // Update menu fade-in animation (0.5s = 30 frames)
    if (menuFadeInFrame_ < 30) {
        menuFadeInFrame_++;
    }
    
    // Update menu slide-in animation (0.3s = 18 frames, starts after 10 frames)
    if (menuFadeInFrame_ >= 10 && menuSlideInFrame_ < 18) {
        menuSlideInFrame_++;
    }
    
    // Update menu blink animation (5Hz = every 12 frames at 60FPS)
    menuBlinkFrame_++;
    if (menuBlinkFrame_ >= 12) {
        menuBlinkFrame_ = 0;
    }

    // Handle menu navigation with debouncing
    static int lastNavFrame = -1;
    static bool lastUpState = false;
    static bool lastDownState = false;
    
    int currentFrame = timer_->getFrameCount();
    bool upPressed = inputManager_->isPressed(GameAction::UP, 0);
    bool downPressed = inputManager_->isPressed(GameAction::DOWN, 0);
    
    // Check for navigation input (with debouncing - only allow once per 15 frames for held keys)
    bool navAllowed = (currentFrame - lastNavFrame) >= 15;
    bool upJustPressed = upPressed && !lastUpState;
    bool downJustPressed = downPressed && !lastDownState;
    
    if (upJustPressed || (upPressed && navAllowed)) {
        // Move to previous menu item
        int prevItem = static_cast<int>(selectedMenuItem_) - 1;
        if (prevItem < 0) {
            prevItem = 1; // Only 2 menu items now (0 or 1)
        }
        selectedMenuItem_ = static_cast<MenuItem>(prevItem);
        lastNavFrame = currentFrame;
        std::cout << "Menu: Selected item " << static_cast<int>(selectedMenuItem_) << std::endl;
    }
    else if (downJustPressed || (downPressed && navAllowed)) {
        // Move to next menu item (only 2 items now: 1P and 2P)
        int nextItem = static_cast<int>(selectedMenuItem_) + 1;
        if (nextItem >= 2) { // Only 2 menu items now
            nextItem = 0;
        }
        selectedMenuItem_ = static_cast<MenuItem>(nextItem);
        lastNavFrame = currentFrame;
        std::cout << "Menu: Selected item " << static_cast<int>(selectedMenuItem_) << std::endl;
    }
    
    lastUpState = upPressed;
    lastDownState = downPressed;
    
    // Handle menu selection confirmation
    if (inputManager_->isJustPressed(GameAction::SHOOT, 0) ||
        inputManager_->isJustPressed(GameAction::START, 0)) {
        // Start confirmation animation (3 quick flashes)
        isConfirmAnimating_ = true;
        confirmAnimationFrame_ = 0;
        std::cout << "Menu: Started confirmation animation for selection " << static_cast<int>(selectedMenuItem_) << std::endl;
    }

    // Update confirmation animation
    if (isConfirmAnimating_) {
        confirmAnimationFrame_++;
        // Animation lasts 18 frames (3 flashes × 6 frames each)
        if (confirmAnimationFrame_ >= 18) {
            // Animation complete, execute selection
            isConfirmAnimating_ = false;
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
            }
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

    // Update level (only spawn enemies when playing)
    levelManager_->update(currentState_ == GameState::PLAYING);

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
    // Note: clear() is already called in render(), don't clear again here

    // Render title with fade-in animation (0.5s = 30 frames)
    // Use English title since font only supports ASCII
    const char* titleText = "BATTLE CITY";
    int titleWidth = strlen(titleText) * 8; // 11 characters * 8 pixels = 88 pixels
    int titleX = (renderer_->getWidth() - titleWidth) / 2; // Center horizontally
    int titleY = 100; // Y position between 80-120
    
    // Calculate fade-in alpha (0.0 to 1.0)
    float titleAlpha = (menuFadeInFrame_ < 30) ? (menuFadeInFrame_ / 30.0f) : 1.0f;
    
    // For now, render title (full implementation would use alpha blending)
    if (menuFadeInFrame_ > 0) {
        renderer_->drawText(titleX, titleY, titleText, BattleCityPalette::COLOR_WHITE);
    }

    // Render menu options with slide-in animation (0.3s = 18 frames, starts after 10 frames)
    const char* menuTexts[] = {
        "1P START",
        "2P START"
    };

    // Menu options start at Y=140, with 20px spacing
    int menuStartY = 140;
    int menuX = 96; // Left-aligned at pixel 96
    
    // Calculate slide-in offset (starts from -64 pixels, slides to 0)
    int slideOffset = 0;
    if (menuSlideInFrame_ < 18) {
        slideOffset = -64 + (menuSlideInFrame_ * 64 / 18);
    }

    for (int i = 0; i < 2; ++i) { // Only show 1P and 2P options
        // Only render if slide-in animation has started
        if (menuFadeInFrame_ >= 10) {
            uint8_t color = BattleCityPalette::COLOR_WHITE; // Default to white

            if (i == static_cast<int>(selectedMenuItem_)) {
                if (isConfirmAnimating_) {
                    // Confirmation animation: 3 quick flashes (18 frames total)
                    // Each flash: 3 frames yellow, 3 frames white
                    int flashPhase = confirmAnimationFrame_ / 6; // 0, 1, 2 for 3 flashes
                    int flashFrame = confirmAnimationFrame_ % 6;  // 0-5 within each flash
                    color = (flashFrame < 3) ? BattleCityPalette::COLOR_YELLOW_SELECTED : BattleCityPalette::COLOR_WHITE;
                } else {
                    // Normal selection blinking: 5Hz (12 frames total, 6 on, 6 off)
                    color = (menuBlinkFrame_ < 6) ? BattleCityPalette::COLOR_YELLOW_SELECTED : BattleCityPalette::COLOR_WHITE;
                }
            }

            renderer_->drawText(menuX + slideOffset, menuStartY + i * 20, menuTexts[i], color);
        }
    }

    // Render high score in top-right corner with digit-by-digit display
    char highScoreText[32];
    sprintf(highScoreText, "HI %06d", highScore_);
    // Calculate right-aligned position: screen width (256) - text width (9 chars * 8 pixels) - margin (8)
    int textWidth = strlen(highScoreText) * 8;
    int highScoreX = renderer_->getWidth() - textWidth - 8; // Right-aligned with 8px margin
    
    // Show high score digits progressively (0.2s per digit = 12 frames)
    int digitsToShow = (menuFadeInFrame_ > 20) ? strlen(highScoreText) : (menuFadeInFrame_ / 2);
    if (digitsToShow > 0) {
        char partialText[32];
        strncpy(partialText, highScoreText, digitsToShow);
        partialText[digitsToShow] = '\0';
        // Recalculate position for partial text
        int partialTextWidth = digitsToShow * 8;
        int partialX = renderer_->getWidth() - partialTextWidth - 8;
        renderer_->drawText(partialX, 8, partialText, BattleCityPalette::COLOR_WHITE);
    }
}

void Game::renderPlaying() {
    // Render stage transition overlay if showing
    if (isShowingStageTransition_) {
        // Draw semi-transparent black overlay
        renderer_->fillRect(0, 0, renderer_->getWidth(), renderer_->getHeight(), BattleCityPalette::COLOR_BLACK);
        
        // Render "STAGE 01" text in center (golden color, pixel font)
        char stageText[32];
        sprintf(stageText, "STAGE %02d", currentLevel_);
        int stageTextX = (renderer_->getWidth() - strlen(stageText) * 8) / 2;
        int stageTextY = renderer_->getHeight() / 2 - 8;
        
        // Use yellow/gold color for stage text
        renderer_->drawText(stageTextX, stageTextY, stageText, BattleCityPalette::COLOR_YELLOW_SELECTED);
        
        // Transition animation: fade in for first 30 frames, hold for 60 frames, fade out for last 30 frames
        // For now, just show the text (full alpha blending would be implemented with SDL_SetRenderDrawBlendMode)
    } else {
        // Render level terrain
        levelManager_->render(*renderer_);

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
    // Initialize player 1 (always created)
    player1_ = std::make_unique<PlayerTank>(0, this);
    
    // Set player 1 initial position (left side for single player, left side for two player)
    player1_->setPosition(Vector2::fromPixels(80, 200));
    
    // Set player 1 initial lives: 3 for single player, 2 for two player
    int player1Lives = isTwoPlayerMode_ ? 2 : 3;
    player1_->setLives(player1Lives);
    
    // Reset player 1 score and upgrade state
    player1_->setScore(0);
    // player1_->resetUpgrade(); // Would reset tank upgrade level
    
    // Initialize player 2 if in two player mode
    if (isTwoPlayerMode_) {
        player2_ = std::make_unique<PlayerTank>(1, this);
        
        // Set player 2 initial position (right side)
        player2_->setPosition(Vector2::fromPixels(160, 200));
        
        // Set player 2 initial lives: 2 for two player mode
        player2_->setLives(2);
        
        // Reset player 2 score and upgrade state
        player2_->setScore(0);
        // player2_->resetUpgrade(); // Would reset tank upgrade level
    } else {
        // Clear player 2 if switching from two player to single player
        player2_.reset();
    }
}

void Game::cleanupLevel() {
    // Reset level-specific objects
    // enemyManager_->clear();
    // powerUpManager_->clear();
    // bulletManager_->clear();
}

} // namespace BattleCity