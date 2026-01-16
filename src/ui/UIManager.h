#pragma once

#include "../utils/MathUtils.h"
#include "../graphics/Renderer.h"

namespace BattleCity {

class UIManager {
public:
    static void showPowerUpIcon(PowerUpType type) {}
    static void renderHUD(Renderer& renderer, int score, int lives, int level) {
        // Format score
        char scoreText[16];
        sprintf(scoreText, "%06d", score);

        // Format level
        char levelText[16];
        sprintf(levelText, "STAGE %02d", level);

        // Render HUD elements
        renderer.drawText(8, 8, "SCORE:", BattleCityPalette::COLOR_WHITE);
        renderer.drawText(8, 20, scoreText, BattleCityPalette::COLOR_WHITE);
        renderer.drawText(200, 8, levelText, BattleCityPalette::COLOR_WHITE);

        // Render lives as tank icons
        for (int i = 0; i < lives; ++i) {
            // Simple rectangle representation for now
            renderer.fillRect(8 + i * 12, 8, 8, 8, BattleCityPalette::COLOR_RED);
        }
    }
};

} // namespace BattleCity