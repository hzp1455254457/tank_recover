#pragma once

#include "../utils/MathUtils.h"
#include "../core/Random.h"
#include <vector>
#include <array>
#include <functional>

namespace BattleCity {

// Level data structure
struct LevelData {
    int levelNumber;
    std::array<std::array<TerrainType, 13>, 13> terrain;
    Vector2 basePosition;
    std::vector<Vector2> enemySpawnPoints;
    std::vector<Vector2> playerSpawnPoints;
};

// Forward declaration
class Game;

// Enemy spawn callback type
using EnemySpawnCallback = std::function<void(EnemyType, const Vector2&)>;

// Level manager - handles level loading, terrain, and enemy spawning
class LevelManager {
private:
    int currentLevel_;
    LevelData currentLevelData_;
    Random& random_;

    int enemiesRemaining_;
    int enemiesToSpawn_;
    int spawnTimer_;
    int spawnIndex_;

    // Enemy spawn callback
    EnemySpawnCallback enemySpawnCallback_;

    // Enemy spawn patterns per level
    static const int MAX_LEVELS = 35;

public:
    LevelManager(Random& random);

    // Level management
    void loadLevel(int level);
    void update(bool isPlaying = true); // Only update spawning when playing
    void reset();

    // Getters
    int getCurrentLevel() const { return currentLevel_; }
    const LevelData& getCurrentLevelData() const { return currentLevelData_; }
    TerrainType getTerrain(int x, int y) const;
    bool isBlocked(int x, int y, bool isBullet = false) const;
    bool isLevelComplete() const { return enemiesRemaining_ == 0; }
    int getEnemiesRemaining() const { return enemiesRemaining_; }

    // Terrain modification
    void destroyBrick(int x, int y);
    void rebuildBaseBricks();

    // Enemy management
    void enemyDestroyed() { enemiesRemaining_--; }

    // Power-up spawning
    bool shouldSpawnPowerUp() const;
    Vector2 getPowerUpSpawnPosition(const Vector2& enemyPosition) const;

    // Spawn points
    const std::vector<Vector2>& getEnemySpawnPoints() const { return currentLevelData_.enemySpawnPoints; }
    const std::vector<Vector2>& getPlayerSpawnPoints() const { return currentLevelData_.playerSpawnPoints; }

    // Enemy spawn callback setup
    void setEnemySpawnCallback(EnemySpawnCallback callback) { enemySpawnCallback_ = callback; }

    // Rendering
    void render(class Renderer& renderer) const;

private:
    // Level data loading
    void generateLevelData(int level);
    void loadTerrainData(int level);
    void setupSpawnPoints();

    // Enemy spawning
    EnemyType getNextEnemyType();
    int getSpawnInterval() const;
    void spawnNextEnemy();

    // Terrain helpers
    bool isValidTerrainPosition(int x, int y) const;
    void adjustBasePositionForLevel(int level);
};

} // namespace BattleCity