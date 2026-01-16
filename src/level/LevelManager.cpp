#include "LevelManager.h"
#include "../graphics/Renderer.h"
#include "../graphics/Palette.h"
#include <algorithm>

namespace BattleCity {

LevelManager::LevelManager(Random& random)
    : currentLevel_(1), random_(random), enemiesRemaining_(20), enemiesToSpawn_(20),
      spawnTimer_(48), spawnIndex_(0), enemySpawnCallback_(nullptr) {
    // First enemy spawns after 800ms = 48 frames at 60fps
    loadLevel(1);
}

void LevelManager::loadLevel(int level) {
    currentLevel_ = std::clamp(level, 1, MAX_LEVELS);
    enemiesRemaining_ = 20;
    enemiesToSpawn_ = 20;
    spawnTimer_ = 48; // First enemy spawns after 800ms = 48 frames at 60fps
    spawnIndex_ = 0;

    generateLevelData(currentLevel_);
}

void LevelManager::update(bool isPlaying) {
    // Only spawn enemies when game is in PLAYING state
    if (!isPlaying) {
        return;
    }

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
    spawnTimer_ = 48; // First enemy spawns after 800ms = 48 frames
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
    // Load terrain data from configuration (using fake data for levels 1-5)
    // For simplicity, use uncompressed format: each byte represents one tile
    // 0=GRASS, 1=BRICK, 2=STEEL, 3=WATER, 4=BASE_BRICK
    // 13x13 = 169 tiles = 169 bytes

    const uint8_t* data = nullptr;
    size_t dataSize = 0;

    // Level 1: Simple cross pattern with base protection
    static const uint8_t level1Data[169] = {
        2,2,2,2,2,2,2,2,2,2,2,2,2,  // Row 0: Steel boundary
        2,0,0,0,0,0,0,0,0,0,0,0,2,  // Row 1
        2,0,1,1,1,0,1,1,1,0,1,1,2,  // Row 2
        2,0,1,0,0,0,0,0,0,0,0,1,2,  // Row 3
        2,0,1,0,2,2,0,2,2,0,2,1,2,  // Row 4
        2,0,1,0,2,0,0,2,0,0,2,1,2,  // Row 5
        2,0,1,0,2,2,0,2,2,0,2,1,2,  // Row 6
        2,0,1,0,0,0,0,0,0,0,0,1,2,  // Row 7
        2,0,1,1,1,0,1,1,1,0,1,1,2,  // Row 8
        2,0,0,0,0,0,0,0,0,0,0,0,2,  // Row 9
        2,1,1,1,0,0,0,1,1,1,1,1,2,  // Row 10
        2,0,0,0,0,0,0,0,0,0,0,0,2,  // Row 11
        2,2,2,2,2,2,2,2,2,2,2,2,2   // Row 12
    };

    // Level 2-5: Use same pattern as level 1 for now (can be expanded later)
    static const uint8_t level2Data[169] = {
        2,2,2,2,2,2,2,2,2,2,2,2,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,0,1,1,1,1,1,1,1,1,1,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,0,3,3,3,0,0,3,3,1,2,
        2,0,1,0,3,3,3,0,0,3,3,1,2,
        2,0,1,0,3,3,3,0,0,3,3,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,1,1,1,1,1,1,1,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,1,1,1,0,0,0,1,1,1,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,2,2,2,2,2,2,2,2,2,2,2,2
    };

    static const uint8_t level3Data[169] = {
        2,2,2,2,2,2,2,2,2,2,2,2,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,0,1,1,1,1,1,1,1,1,1,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,0,1,1,1,0,1,1,1,1,2,
        2,0,1,0,1,1,1,0,1,1,1,1,2,
        2,0,1,0,1,1,1,0,1,1,1,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,1,1,1,1,1,1,1,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,1,1,1,0,0,0,1,1,1,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,2,2,2,2,2,2,2,2,2,2,2,2
    };

    // Level 4: Same as level 1 for now
    static const uint8_t level4Data[169] = {
        2,2,2,2,2,2,2,2,2,2,2,2,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,0,1,1,1,0,1,1,1,0,1,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,0,2,2,0,2,2,0,2,1,2,
        2,0,1,0,2,0,0,2,0,0,2,1,2,
        2,0,1,0,2,2,0,2,2,0,2,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,1,1,0,1,1,1,0,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,1,1,1,0,0,0,1,1,1,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,2,2,2,2,2,2,2,2,2,2,2,2
    };

    // Level 5: Same as level 1 for now
    static const uint8_t level5Data[169] = {
        2,2,2,2,2,2,2,2,2,2,2,2,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,0,1,1,1,0,1,1,1,0,1,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,0,2,2,0,2,2,0,2,1,2,
        2,0,1,0,2,0,0,2,0,0,2,1,2,
        2,0,1,0,2,2,0,2,2,0,2,1,2,
        2,0,1,0,0,0,0,0,0,0,0,1,2,
        2,0,1,1,1,0,1,1,1,0,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,1,1,1,0,0,0,1,1,1,1,1,2,
        2,0,0,0,0,0,0,0,0,0,0,0,2,
        2,2,2,2,2,2,2,2,2,2,2,2,2
    };

    // Select data based on level
    switch (level) {
        case 1:
            data = level1Data;
            dataSize = sizeof(level1Data);
            break;
        case 2:
            data = level2Data;
            dataSize = sizeof(level2Data);
            break;
        case 3:
            data = level3Data;
            dataSize = sizeof(level3Data);
            break;
        case 4:
            data = level4Data;
            dataSize = sizeof(level4Data);
            break;
        case 5:
            data = level5Data;
            dataSize = sizeof(level5Data);
            break;
        default:
            // For levels > 5, use level 1 data as fallback
            data = level1Data;
            dataSize = sizeof(level1Data);
            break;
    }

    // Decode uncompressed data (1 tile per byte)
    if (data && dataSize >= 169) {
        for (int y = 0; y < 13; ++y) {
            for (int x = 0; x < 13; ++x) {
                int index = y * 13 + x;
                
                // Check bounds
                if (index >= static_cast<int>(dataSize)) {
                    currentLevelData_.terrain[y][x] = TerrainType::GRASS;
                    continue;
                }
                
                uint8_t tileValue = data[index];

                // Convert byte value to TerrainType
                TerrainType terrainType;
                switch (tileValue) {
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
    } else {
        // Fallback: fill with grass if no data
        for (auto& row : currentLevelData_.terrain) {
            row.fill(TerrainType::GRASS);
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
    // Spawn interval: 1.5-2 seconds (90-120 frames) at 60fps
    // Base interval is 120 frames (2 seconds), with slight variation
    int baseInterval = 120;
    int levelReduction = (currentLevel_ - 1) * 2;
    int interval = std::max(90, baseInterval - levelReduction); // Minimum 1.5 seconds
    
    // Add slight random variation (90-120 frames range)
    int variation = random_.range(-15, 15);
    return std::max(90, interval + variation);
}

void LevelManager::render(Renderer& renderer) const {
    // Render terrain (13x13 grid, each tile is 16x16 pixels)
    const int TILE_SIZE = 16;
    
    for (int y = 0; y < 13; ++y) {
        for (int x = 0; x < 13; ++x) {
            TerrainType terrain = currentLevelData_.terrain[y][x];
            int pixelX = x * TILE_SIZE;
            int pixelY = y * TILE_SIZE;
            
            uint8_t colorIndex;
            switch (terrain) {
                case TerrainType::GRASS:
                    colorIndex = BattleCityPalette::COLOR_GREEN;
                    break;
                case TerrainType::BRICK:
                    colorIndex = BattleCityPalette::COLOR_YELLOW;
                    break;
                case TerrainType::STEEL:
                    colorIndex = BattleCityPalette::COLOR_GRAY;
                    break;
                case TerrainType::WATER:
                    colorIndex = BattleCityPalette::COLOR_CYAN;
                    break;
                case TerrainType::BASE_BRICK:
                    colorIndex = BattleCityPalette::COLOR_YELLOW;
                    break;
                default:
                    colorIndex = BattleCityPalette::COLOR_GREEN;
                    break;
            }
            
            // Render tile
            renderer.fillRect(pixelX, pixelY, TILE_SIZE, TILE_SIZE, colorIndex);
            
            // Add visual details for different terrain types
            if (terrain == TerrainType::BRICK || terrain == TerrainType::BASE_BRICK) {
                // Draw brick pattern (simple grid lines)
                renderer.drawRect(pixelX, pixelY, TILE_SIZE, TILE_SIZE, BattleCityPalette::COLOR_BLACK);
                renderer.drawRect(pixelX + 7, pixelY, 1, TILE_SIZE, BattleCityPalette::COLOR_BLACK);
                renderer.drawRect(pixelX, pixelY + 7, TILE_SIZE, 1, BattleCityPalette::COLOR_BLACK);
            } else if (terrain == TerrainType::WATER) {
                // Draw water animation pattern (simple alternating pattern)
                for (int wy = 0; wy < TILE_SIZE; wy += 4) {
                    for (int wx = ((wy / 4) % 2) * 4; wx < TILE_SIZE; wx += 8) {
                        renderer.fillRect(pixelX + wx, pixelY + wy, 4, 2, BattleCityPalette::COLOR_CYAN);
                    }
                }
            }
        }
    }
    
    // Render base (eagle icon would be rendered here, for now just a colored square)
    int baseX = currentLevelData_.basePosition.pixelX();
    int baseY = currentLevelData_.basePosition.pixelY();
    renderer.fillRect(baseX - 8, baseY - 8, 16, 16, BattleCityPalette::COLOR_ORANGE);
    renderer.drawRect(baseX - 8, baseY - 8, 16, 16, BattleCityPalette::COLOR_BLACK);
}

void LevelManager::spawnNextEnemy() {
    // Check if callback is set
    if (!enemySpawnCallback_) {
        return; // No callback set, cannot spawn
    }

    // Cycle through spawn points (4 fixed positions)
    Vector2 spawnPos = currentLevelData_.enemySpawnPoints[spawnIndex_ % 4];
    spawnIndex_++;

    // Get enemy type for this spawn
    EnemyType type = getNextEnemyType();

    // Call the spawn callback to actually create the enemy
    enemySpawnCallback_(type, spawnPos);
}

} // namespace BattleCity