## ADDED Requirements

### Requirement: Enemy Tank Spawning System
系统SHALL提供完整的敌人坦克生成系统，按照关卡配置在指定位置和时机生成敌方坦克。

#### Scenario: Enemy Spawning on Level Start
- **WHEN** 游戏进入PLAYING状态并加载关卡
- **THEN** 首辆敌人坦克在关卡开始后800ms（48帧）生成
- **AND** 后续敌人坦克按1.5-2秒间隔（90-120帧）生成
- **AND** 每关固定生成20辆敌人坦克
- **AND** 单屏同时存在的敌人坦克数量不超过4辆

#### Scenario: Spawn Point Rotation
- **WHEN** 敌人坦克需要生成
- **THEN** 从4个固定出生点按顺序循环选择：(20,20)、(236,20)、(20,204)、(236,204)
- **AND** 每个出生点按固定顺序使用，避免重叠生成
- **AND** 生成前检查出生点周围8像素内是否有其他坦克，如有则延迟生成

#### Scenario: Enemy Type Sequence
- **WHEN** 敌人坦克按序列生成
- **THEN** 关卡1-9：按"普通→普通→普通→快速"的4辆循环模式生成
- **AND** 关卡10-19：按"普通→普通→快速→重型"的4辆循环模式生成
- **AND** 关卡20-35：按"普通→快速→重型→精英"的4辆循环模式生成
- **AND** 每关20辆敌人严格按上述序列生成，无随机偏差

#### Scenario: Spawning State Management
- **WHEN** 游戏处于非PLAYING状态（菜单、暂停、游戏结束等）
- **THEN** 敌人生成系统暂停，不生成新敌人
- **AND** 当游戏返回PLAYING状态时，生成系统恢复工作
- **AND** 关卡切换时重置生成计数器和计时器

### Requirement: Terrain Data Configuration System
系统SHALL提供地形数据的配置化加载机制，支持按关卡号加载对应的地形布局。

#### Scenario: Terrain Data Loading
- **WHEN** 关卡加载时（`LevelManager::loadLevel()`）
- **THEN** 从关卡配置数据加载13×13网格的地形布局
- **AND** 支持5种地形类型：草地(GRASS)、砖墙(BRICK)、钢墙(STEEL)、水域(WATER)、基地砖(BASE_BRICK)
- **AND** 地形数据可以来自配置文件或假数据（初始实现可使用假数据）
- **AND** 每个关卡有唯一的地形布局配置

#### Scenario: Terrain Rendering
- **WHEN** 地形数据加载完成
- **THEN** 地形正确渲染到游戏画面
- **AND** 地形类型与碰撞检测系统正确对应
- **AND** 地形边界（钢墙）正确包围地图四周
- **AND** 基地位置和周围砖墙正确显示

#### Scenario: Multi-Level Terrain Support
- **WHEN** 切换到不同关卡
- **THEN** 加载对应关卡的地形配置数据
- **AND** 地形布局正确切换，无残留或错误显示
- **AND** 支持至少5个关卡的地形数据（关卡1-5，使用假数据）
- **AND** 地形数据加载失败时使用默认地形（全草地+边界钢墙）

### Requirement: Level Configuration Integration
系统SHALL提供关卡配置数据的统一管理，整合地形、出生点、敌人序列等配置信息。

#### Scenario: Level Configuration Structure
- **WHEN** 关卡配置系统初始化
- **THEN** 每个关卡配置包含：地形数据、敌人出生点、敌人类型序列、基地位置
- **AND** 配置数据与现有 `LevelData` 结构兼容
- **AND** 配置数据支持从假数据或未来配置文件加载

#### Scenario: Configuration Validation
- **WHEN** 加载关卡配置数据
- **THEN** 验证配置数据的完整性和有效性
- **AND** 配置数据缺失或无效时使用默认配置
- **AND** 记录配置加载错误（用于调试）

## MODIFIED Requirements

### Requirement: Level Management System
系统SHALL完善关卡管理系统，确保敌人生成和地形加载的正确集成。

#### Scenario: Level Initialization with Spawning
- **WHEN** 关卡初始化时（`LevelManager::loadLevel()`）
- **THEN** 加载地形配置数据
- **AND** 初始化敌人生成系统（重置计数器、计时器）
- **AND** 设置敌人生成回调或引用，使 `spawnNextEnemy()` 能实际创建敌人
- **AND** 配置敌人类型序列和生成间隔

#### Scenario: Level Update with Enemy Spawning
- **WHEN** 关卡更新时（`LevelManager::update()`）
- **THEN** 检查敌人生成条件（计时器、数量限制、游戏状态）
- **AND** 满足条件时调用 `spawnNextEnemy()` 生成敌人
- **AND** 更新生成计数器和计时器
- **AND** 跟踪已生成和剩余的敌人数量
