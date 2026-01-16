# Project Context

## Purpose
**Battle City Perfect Remake** - A pixel-perfect recreation of the classic NES game Battle City (Tank Battalion) for PC platforms. The project aims to faithfully reproduce the original gameplay mechanics, visual style, sound effects, and AI behavior while maintaining strict 60FPS performance and NES hardware-level accuracy.

### Core Goals
- **1:1 Faithful Recreation**: Pixel-level accuracy in graphics, sound, and gameplay mechanics
- **Cross-Platform Compatibility**: Windows 7+, macOS 10.12+, Linux support
- **Performance Excellence**: 60FPS with <50MB memory usage, <10% CPU utilization
- **Nostalgic Experience**: Complete preservation of original difficulty curves and hidden features

## Tech Stack
### Primary Technologies
- **Language**: C++17 (Modern C++ with smart pointers, lambdas, and STL)
- **Graphics**: SDL2 + OpenGL (Hardware-accelerated pixel-perfect rendering)
- **Audio**: SDL2_mixer (NES APU hardware simulation)
- **Build System**: CMake 3.16+ (Cross-platform build configuration)
- **Windowing**: SDL2 (Cross-platform window and input management)

### Development Environment
- **IDE**: Visual Studio 2022 / VS Code with C++ extensions
- **Version Control**: Git with conventional commit messages
- **Documentation**: Markdown-based specifications (OpenSpec framework)

## Project Conventions

### Code Style
#### Naming Conventions
- **Classes**: PascalCase (e.g., `PlayerTank`, `GameStateManager`)
- **Methods/Functions**: camelCase (e.g., `update()`, `renderSprite()`)
- **Variables**: camelCase (e.g., `playerPosition`, `enemyHealth`)
- **Constants**: SCREAMING_SNAKE_CASE (e.g., `MAP_WIDTH`, `FRAME_RATE`)
- **Files**: PascalCase matching class names (e.g., `PlayerTank.cpp`, `Game.h`)

#### Formatting Rules
- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 100 characters maximum
- **Braces**: K&R style (opening brace on same line)
- **Includes**: Group by type (standard library, SDL2, project headers)
- **Pointers/References**: Attach to type (e.g., `int* ptr`, not `int *ptr`)

#### Code Organization
```cpp
// Header file structure
#pragma once

#include <SDL2/SDL.h>        // External dependencies
#include "Core/Game.h"       // Project headers

namespace BattleCity {

class PlayerTank {
public:
    // Public interface first
    void update();
    void render(Renderer& renderer);

private:
    // Private implementation
    Vector2 position_;
    Direction direction_;
};

} // namespace BattleCity
```

### Architecture Patterns
#### Core Design Principles
- **Component-Based Architecture**: Modular systems with clear separation of concerns
- **State Machine Pattern**: Game states, AI behaviors, and UI states
- **Object Pool Pattern**: Efficient memory management for bullets, particles, enemies
- **Observer Pattern**: Event-driven communication between systems

#### Module Structure
```
BattleCity/
├── core/          # Foundation systems (Game loop, Timer, Random)
├── graphics/      # Rendering pipeline (Renderer, Sprites, Palette)
├── audio/         # Sound system (NES APU, Sound effects, Music)
├── input/         # Input handling (Keyboard, Gamepad)
├── gameplay/      # Game objects (Tanks, Bullets, PowerUps)
├── ai/           # AI systems (State machines, Pathfinding)
├── level/        # Level management (Maps, Terrain, Spawning)
├── ui/           # User interface (HUD, Menus, Score)
├── save/         # Persistence (High scores, Progress)
└── utils/        # Utilities (Math, File I/O, Debug)
```

#### Memory Management
- **RAII Pattern**: Resource acquisition is initialization
- **Smart Pointers**: `std::unique_ptr` for ownership, `std::shared_ptr` for shared resources
- **Object Pools**: Pre-allocated containers for frequently created/destroyed objects
- **No Raw Pointers**: Except for performance-critical inner loops

### Testing Strategy
#### Test Categories
- **Unit Tests**: Individual class/method functionality
- **Integration Tests**: System interactions and data flow
- **Performance Tests**: Frame rate, memory usage, CPU utilization benchmarks
- **Compatibility Tests**: Cross-platform verification (Windows/macOS/Linux)

#### Testing Framework
- **Unit Testing**: Google Test (gtest) for C++ unit tests
- **Integration Testing**: Custom test harness for gameplay verification
- **Performance Testing**: Custom benchmarking tools with SDL2 timer integration

#### Test Coverage Requirements
- **Core Systems**: 90%+ coverage (Game loop, Physics, Collision)
- **Gameplay Systems**: 80%+ coverage (AI, Power-ups, Scoring)
- **UI Systems**: 70%+ coverage (Menus, HUD, Transitions)

### Git Workflow
#### Branching Strategy
- **main**: Production-ready code, always stable
- **develop**: Integration branch for features
- **feature/**: Feature branches (e.g., `feature/add-audio-system`)
- **bugfix/**: Bug fix branches (e.g., `bugfix/collision-detection`)
- **hotfix/**: Critical fixes for production

#### Commit Conventions
```
type(scope): description

Types:
- feat: New features
- fix: Bug fixes
- docs: Documentation
- style: Code style changes
- refactor: Code refactoring
- perf: Performance improvements
- test: Testing
- chore: Maintenance

Examples:
- feat(collision): add pixel-perfect collision detection
- fix(ai): resolve enemy tank pathfinding bug
- perf(renderer): optimize sprite batching
```

## Domain Context
### Game Mechanics Knowledge
#### NES Hardware Constraints
- **Resolution**: 256×224 pixels (overscan-safe area)
- **Color Palette**: 54 colors from NES PPU palette
- **Frame Rate**: 60 FPS (16.67ms per frame)
- **Audio**: 5-channel APU (2 pulse, 1 triangle, 1 noise, 1 DMC)

#### Battle City Specifics
- **Tank Movement**: 8-directional with 4 visual directions
- **Collision Detection**: 12-point collision system for precise tank-terrain interaction
- **AI States**: IDLE (wandering), CHASE (player pursuit), EVADE (damage avoidance)
- **Power-up System**: 5 types with specific timing and effects
- **Level Design**: 35 levels with increasing difficulty and special layouts

#### Performance Requirements
- **Memory Budget**: <50MB total RAM usage
- **CPU Target**: <10% utilization on modern hardware
- **Frame Budget**: 16.67ms per frame (60 FPS) with 2ms variance tolerance
- **Object Limits**: Max 8 enemies on screen, 2 bullets per player

## Important Constraints
### Technical Constraints
- **Pixel Perfect**: No interpolation, integer scaling only (2x, 3x, 4x)
- **Deterministic Random**: Same seed produces identical gameplay
- **NES Accuracy**: Hardware-level simulation of original behavior
- **Cross-Platform**: Consistent behavior across Windows, macOS, Linux

### Performance Constraints
- **60 FPS Lock**: Fixed timestep updates, no frame rate drops
- **Memory Limits**: Object pools, no dynamic allocation in hot paths
- **Binary Size**: <10MB executable size target
- **Startup Time**: <2 seconds from launch to gameplay

### Compatibility Constraints
- **SDL2 Version**: 2.0.8+ for consistent API across platforms
- **C++ Standard**: C++17 minimum, no C++20 features
- **Compiler Support**: MSVC 2019+, GCC 9+, Clang 10+
- **OS Support**: Windows 7+, macOS 10.12+, Ubuntu 18.04+

## External Dependencies
### Build Dependencies
- **SDL2**: Cross-platform multimedia library (graphics, input, windowing)
- **SDL2_mixer**: Audio mixing and playback with multiple format support
- **CMake**: Build system generator and project configuration

### Runtime Dependencies
- **Visual C++ Redistributable**: Windows runtime libraries
- **System Libraries**: Platform-specific graphics/audio drivers

### Development Dependencies
- **Git**: Version control system
- **Visual Studio 2022**: Primary Windows development environment
- **Google Test**: Unit testing framework
- **Doxygen**: Documentation generation (future)
