#include "LevelManager.h"
#include <algorithm>

namespace BattleCity {

LevelManager::LevelManager(Random& random)
    : currentLevel_(1), random_(random), enemiesRemaining_(20), enemiesToSpawn_(20),
      spawnTimer_(80), spawnIndex_(0) {
    loadLevel(1);
}

void LevelManager::loadLevel(int level) {
    currentLevel_ = std::clamp(level, 1, MAX_LEVELS);
    enemiesRemaining_ = 20;
    enemiesToSpawn_ = 20;
    spawnTimer_ = 80; // First enemy spawns after 80 frames
    spawnIndex_ = 0;

    generateLevelData(currentLevel_);
}

void LevelManager::update() {
    // Handle enemy spawning
    if (enemiesToSpawn_ > 0 && spawnTimer_ <= 0) {
        spawnNextEnemy();
        enemiesToSpawn_--;
        spawnTimer_ = getSpawnInterval();
    }

    spawnTimer_--;
}

void LevelManager::reset() {
    enemiesRemaining_ = 20;
    enemiesToSpawn_ = 20;
    spawnTimer_ = 80;
    spawnIndex_ = 0;
}

TerrainType LevelManager::getTerrain(int x, int y) const {
    if (!isValidTerrainPosition(x, y)) {
        return TerrainType::STEEL; // Boundary
    }
    return currentLevelData_.terrain[y][x];
}

bool LevelManager::isBlocked(int x, int y, bool isBullet) const {
    TerrainType terrain = getTerrain(x, y);

    switch (terrain) {
        case TerrainType::GRASS:
            return false;
        case TerrainType::BRICK:
            return true;
        case TerrainType::STEEL:
            return true;
        case TerrainType::WATER:
            return !isBullet; // Bullets pass through water
        case TerrainType::BASE_BRICK:
            return true;
        default:
            return false;
    }
}

void LevelManager::destroyBrick(int x, int y) {
    if (isValidTerrainPosition(x, y) &&
        currentLevelData_.terrain[y][x] == TerrainType::BRICK) {
        currentLevelData_.terrain[y][x] = TerrainType::GRASS;
    }
}

void LevelManager::rebuildBaseBricks() {
    // Find base position and rebuild surrounding bricks
    int baseX = currentLevelData_.basePosition.pixelX() / 16;
    int baseY = currentLevelData_.basePosition.pixelY() / 16;

    // Rebuild base bricks (simplified - would need actual base brick positions)
    for (int y = baseY - 1; y <= baseY + 1; ++y) {
        for (int x = baseX - 1; x <= baseX + 1; ++x) {
            if (isValidTerrainPosition(x, y) &&
                currentLevelData_.terrain[y][x] == TerrainType::GRASS) {
                currentLevelData_.terrain[y][x] = TerrainType::BASE_BRICK;
            }
        }
    }
}

void LevelManager::generateLevelData(int level) {
    currentLevelData_.levelNumber = level;

    // Initialize terrain to grass
    for (auto& row : currentLevelData_.terrain) {
        row.fill(TerrainType::GRASS);
    }

    // Add boundary walls
    for (int i = 0; i < 13; ++i) {
        currentLevelData_.terrain[0][i] = TerrainType::STEEL;
        currentLevelData_.terrain[12][i] = TerrainType::STEEL;
        currentLevelData_.terrain[i][0] = TerrainType::STEEL;
        currentLevelData_.terrain[i][12] = TerrainType::STEEL;
    }

    // Load specific level terrain
    loadTerrainData(level);

    // Setup spawn points
    setupSpawnPoints();

    // Adjust base position for specific levels
    adjustBasePositionForLevel(level);
}

void LevelManager::loadTerrainData(int level) {
    // Generate terrain based on level patterns
    // This is a simplified version - actual implementation would load from data files

    // Load actual level data from embedded ROM data
    // Data is stored in 4-bit compressed format (2 tiles per byte)

    // Level 1: Simple cross pattern with base protection
    static const uint8_t level1Data[65] = {
        0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
        0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // For now, use level 1 data for all levels (simplified)
    // TODO: Add complete 35 level ROM data
    const uint8_t* data = level1Data;

    // Decode 4-bit compressed data
    for (int y = 0; y < 13; ++y) {
        for (int x = 0; x < 13; ++x) {
            int index = y * 13 + x;
            int byteIndex = index / 2;
            int nibbleShift = (index % 2) * 4;
            uint8_t nibble = (data[byteIndex] >> nibbleShift) & 0x0F;

            // Convert nibble to TerrainType
            TerrainType terrainType;
            switch (nibble) {
                case 0: terrainType = TerrainType::GRASS; break;
                case 1: terrainType = TerrainType::BRICK; break;
                case 2: terrainType = TerrainType::STEEL; break;
                case 3: terrainType = TerrainType::WATER; break;
                case 4: terrainType = TerrainType::BASE_BRICK; break;
                default: terrainType = TerrainType::GRASS; break;
            }

            currentLevelData_.terrain[y][x] = terrainType;
        }
    }

    // Ensure boundaries are steel
    for (int i = 0; i < 13; ++i) {
        currentLevelData_.terrain[0][i] = TerrainType::STEEL;
        currentLevelData_.terrain[12][i] = TerrainType::STEEL;
        currentLevelData_.terrain[i][0] = TerrainType::STEEL;
        currentLevelData_.terrain[i][12] = TerrainType::STEEL;
    }

    // Add base area
    int baseX = 6, baseY = 11;
    currentLevelData_.terrain[baseY][baseX] = TerrainType::STEEL; // Base itself
    // Add base brick walls around it
    for (int x = baseX - 1; x <= baseX + 1; ++x) {
        currentLevelData_.terrain[baseY][x] = TerrainType::BASE_BRICK;
    }
}

void LevelManager::setupSpawnPoints() {
    // Enemy spawn points (top and sides)
    currentLevelData_.enemySpawnPoints = {
        Vector2::fromPixels(20, 20),   // Top-left
        Vector2::fromPixels(236, 20),  // Top-right
        Vector2::fromPixels(20, 204),  // Bottom-left
        Vector2::fromPixels(236, 204)  // Bottom-right
    };

    // Player spawn points
    currentLevelData_.playerSpawnPoints = {
        Vector2::fromPixels(80, 200),  // Player 1 (left of base)
        Vector2::fromPixels(160, 200)  // Player 2 (right of base)
    };
}

void LevelManager::adjustBasePositionForLevel(int level) {
    // Adjust base position based on level (from original game data)
    int baseX = 120, baseY = 200; // Default position

    // Special level adjustments
    switch (level) {
        case 3: case 7: case 11: case 15: case 19:
        case 23: case 27: case 31:
            baseX = 112; // Shift left 8 pixels
            break;
        case 4: case 8: case 12: case 16: case 20:
        case 24: case 28: case 32:
            baseX = 128; // Shift right 8 pixels
            break;
        case 13: case 21: case 29:
            baseX = 104; // Shift left 16 pixels
            break;
        case 14: case 22: case 30:
            baseX = 136; // Shift right 16 pixels
            break;
    }

    currentLevelData_.basePosition = Vector2::fromPixels(baseX, baseY);
}

bool LevelManager::shouldSpawnPowerUp() const {
    // 15%概率生成道具（原版概率）
    return (random_.range(0, 99) < 15);
}

Vector2 LevelManager::getPowerUpSpawnPosition(const Vector2& enemyPosition) const {
    // 道具生成位置与敌方坦克损毁中心点完全一致
    return enemyPosition;
}

bool LevelManager::isValidTerrainPosition(int x, int y) const {
    return x >= 0 && x < 13 && y >= 0 && y < 13;
}

EnemyType LevelManager::getNextEnemyType() {
    // Enemy spawn pattern based on level progression
    int totalSpawned = 20 - enemiesToSpawn_;
    int patternIndex = totalSpawned % 4;

    if (currentLevel_ <= 9) {
        // Levels 1-9: Basic/Fast pattern
        return (patternIndex < 3) ? EnemyType::BASIC : EnemyType::FAST;
    } else if (currentLevel_ <= 19) {
        // Levels 10-19: Add Heavy
        switch (patternIndex) {
            case 0: case 1: return EnemyType::BASIC;
            case 2: return EnemyType::FAST;
            case 3: return EnemyType::HEAVY;
        }
    } else {
        // Levels 20-35: Add Elite
        switch (patternIndex) {
            case 0: return EnemyType::BASIC;
            case 1: return EnemyType::FAST;
            case 2: return EnemyType::HEAVY;
            case 3: return EnemyType::ELITE;
        }
    }

    return EnemyType::BASIC;
}

int LevelManager::getSpawnInterval() const {
    // Spawn interval decreases with level (more challenging)
    int baseInterval = 120; // 2 seconds at 60fps
    int levelReduction = (currentLevel_ - 1) * 2;
    return std::max(60, baseInterval - levelReduction); // Minimum 1 second
}

void LevelManager::spawnNextEnemy() {
    // Cycle through spawn points
    Vector2 spawnPos = currentLevelData_.enemySpawnPoints[spawnIndex_ % 4];
    spawnIndex_++;

    // Get enemy type for this spawn
    EnemyType type = getNextEnemyType();

    // Create enemy tank (would be handled by enemy manager)
    // enemyManager_->spawnEnemy(type, spawnPos);
}

} // namespace BattleCity