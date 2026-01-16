# Change: implement-enemy-spawning-terrain-generation

## Why
当前游戏已实现基础的关卡管理系统和敌人坦克生成逻辑框架，但缺少完整的敌人坦克生成流程和地形数据配置化加载。根据需求文档，每关需要按固定配置生成20辆敌方坦克，从4个固定出生点按顺序刷新，且地形布局需要从关卡配置数据加载。目前的实现中，`LevelManager::spawnNextEnemy()` 只是注释占位，没有实际调用 `Game::spawnEnemy()` 来创建敌人坦克；地形数据也仅硬编码了关卡1的数据，无法支持多关卡配置。需要完善敌人坦克生成系统与Game类的集成，并实现地形数据的配置化加载机制（可以使用假数据作为初始实现）。

## What Changes
- 完善敌人坦克生成系统，连接 `LevelManager` 和 `Game` 的生成流程
- 实现按关卡配置的敌人坦克生成逻辑（波次、出生点、类型序列）
- 实现地形数据的配置化加载机制（支持从配置文件或假数据加载）
- 确保进入游戏后能正确按关卡配置生成敌人和地形
- 优化生成系统的性能和可维护性

## Impact
- Affected specs: level/ (新增敌人生成和地形配置规范)
- Affected code: 
  - `src/level/LevelManager.cpp`, `src/level/LevelManager.h` (完善生成逻辑和配置加载)
  - `src/core/Game.cpp`, `src/core/Game.h` (集成生成回调机制)
  - 新增或修改地形数据加载相关代码
- 新增功能: 完整的敌人坦克生成流程、地形配置化加载
- 用户体验: 进入游戏后能看到按关卡配置正确生成的敌人坦克和地形布局
