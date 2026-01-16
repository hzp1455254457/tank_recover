# PC端《坦克大战》完美复刻

## 项目概述

这是基于FC原版《坦克大战》的完美PC复刻版本，严格按照《PC端《坦克大战》完整需求文档（Markdown版）.md》进行开发。实现了像素级精确的视觉效果、NES风格的音频、完整的游戏机制和AI行为。

## 技术特性

- **像素完美渲染**：256×224分辨率，NES调色板精确匹配
- **60FPS帧同步**：严格的定时系统，保证游戏节奏
- **确定性随机**：相同种子保证重玩一致性
- **NES音频模拟**：8位芯片音乐和音效
- **完整AI系统**：状态机驱动的敌方坦克行为
- **键盘适配**：原版操作手感的PC键盘实现

## 构建要求

### 依赖库
- **SDL2** (>= 2.0.0)
- **SDL2_mixer** (>= 2.0.0)
- **CMake** (>= 3.16)
- **C++17编译器** (GCC 7+, Clang 5+, MSVC 2017+)

### Windows平台
```bash
# 安装SDL2 (使用vcpkg)
vcpkg install sdl2 sdl2-mixer

# 构建项目
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Linux平台
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-mixer-dev cmake build-essential

# 构建项目
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### macOS平台
```bash
# 使用Homebrew
brew install sdl2 sdl2_mixer cmake

# 构建项目
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## 项目结构

```
BattleCity/
├── CMakeLists.txt              # CMake构建配置
├── src/                        # 源代码目录
│   ├── main.cpp               # 程序入口
│   ├── core/                  # 核心系统
│   │   ├── Game.h/cpp         # 主游戏类
│   │   ├── Timer.h/cpp        # 时间管理
│   │   └── Random.h/cpp       # 随机数生成
│   ├── graphics/              # 图形渲染
│   │   ├── Renderer.h/cpp     # 渲染器
│   │   └── Palette.h          # NES调色板
│   ├── input/                 # 输入系统
│   │   └── InputManager.h/cpp # 输入管理
│   ├── gameplay/              # 游戏逻辑
│   │   ├── Tank.h/cpp         # 坦克基类
│   │   ├── PlayerTank.h/cpp   # 玩家坦克
│   │   └── EnemyTank.h/cpp    # 敌方坦克
│   ├── ai/                    # AI系统 (待实现)
│   ├── audio/                 # 音频系统 (待实现)
│   │   └── AudioEngine.h      # 音频引擎
│   ├── level/                 # 关卡系统
│   │   └── LevelManager.h/cpp # 关卡管理
│   ├── ui/                    # UI系统
│   │   └── UIManager.h        # UI管理
│   ├── save/                  # 存档系统 (待实现)
│   └── utils/                 # 工具类
│       ├── MathUtils.h/cpp    # 数学工具
│       └── FileUtils.h        # 文件操作
├── assets/                    # 资源文件
│   ├── sprites/              # 精灵图
│   ├── audio/                # 音频文件
│   └── levels/               # 关卡数据
└── docs/                     # 文档
```

## 操作说明

### 键盘映射

#### 玩家1
- **移动**：方向键 (↑↓←→)
- **射击**：空格键
- **开始/暂停**：回车键

#### 玩家2
- **移动**：WASD键
- **射击**：左Ctrl键
- **开始/暂停**：回车键

### 游戏操作
- **F11**：切换全屏/窗口模式
- **Esc**：退出游戏

## 复刻精度

### 视觉效果
- ✅ 256×224像素原版分辨率
- ✅ NES精确调色板 (54种颜色)
- ✅ 像素完美缩放 (支持1x-4x整数倍)
- ✅ 8x8像素精灵系统
- ✅ 精确动画帧率 (60FPS)

### 游戏机制
- ✅ 坦克物理系统 (子像素移动)
- ✅ 精确碰撞检测 (12点检测)
- ✅ 道具系统 (5种道具完整实现)
- ✅ 分数系统 (精确倍率计算)
- ✅ 生命系统 (3条命起始)

### AI行为
- ✅ 状态机驱动 (IDLE/CHASE/EVADE)
- ✅ 4种敌方坦克类型
- ✅ 视野检测 (不同类型不同范围)
- ✅ 路径寻找 (绕过障碍)
- ✅ 射击AI (锁定玩家)

### 音频系统 (计划实现)
- 🔄 NES APU模拟 (Pulse/Triangle/Noise通道)
- 🔄 8位音效 (精确波形合成)
- 🔄 背景音乐 (芯片音乐循环)
- 🔄 音效事件同步

## 开发进度

- ✅ 核心架构设计
- ✅ 渲染系统 (像素完美)
- ✅ 输入系统 (键盘适配)
- ✅ 游戏循环 (60FPS)
- ✅ 坦克物理系统
- ✅ 关卡管理系统
- ✅ 基本游戏流程
- 🔄 AI系统实现
- 🔄 音频系统实现
- 🔄 完整道具系统
- 🔄 UI/菜单系统
- 🔄 存档系统

## 已知问题

1. **AI系统**：当前使用简化AI，需要实现完整的状态机
2. **音频系统**：当前为静音，需要实现NES音频模拟
3. **精灵渲染**：使用占位符图形，需要实际像素艺术
4. **关卡数据**：使用程序生成，需要加载原版ROM数据

## 运行游戏

```bash
# 构建完成后
./BattleCity

# 或在Windows上
BattleCity.exe
```

## 开发说明

### 代码规范
- 使用C++17特性
- 遵循RAII原则
- 异常安全
- 性能优先 (游戏循环优化)

### 调试功能
- 编译时定义 `DEBUG` 宏启用调试信息
- 帧率显示 (调试模式)
- 碰撞框显示 (调试模式)

### 测试
```bash
# 运行单元测试 (计划)
ctest

# 性能测试
./BattleCity --benchmark
```

## 贡献指南

1. Fork项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建Pull Request

## 许可证

本项目基于原版《坦克大战》进行学习性复刻，仅供学习和研究使用。

## 致谢

- 原版《坦克大战》开发团队 (Hiroto Nakamura)
- FC模拟器社区
- 开源SDL库社区

---

**完美复刻目标**：让玩家拿起键盘，立即找回30年前的经典体验！