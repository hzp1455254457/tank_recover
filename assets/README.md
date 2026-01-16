# FC坦克大战素材说明

## 素材要求

### 图像素材 (PNG格式，带透明通道)

#### 1. 精灵图 (Sprites)
- **位置**: `assets/sprites/`
- **格式**: PNG, 8x8像素或16x16像素
- **要求**:
  - 玩家坦克: 红色车身、白色炮管，4个方向 (上/下/左/右)
  - 敌方坦克: 灰色/绿色/深灰色/红色 (普通/快速/重型/精英)，4个方向
  - 子弹: 4x4像素，4个方向
  - 地形: 草地(绿色)、砖墙(黄色)、钢墙(白色+灰色纹理)、水域(蓝色)
  - 道具: 星星(黄色)、生命(绿色坦克图标)、时钟(蓝色)、护盾(红色)、炸弹(白色)
  - 基地: 16x16像素红色鹰标
  - 爆炸: 3帧动画 (红色→橙色→黑色)

#### 2. 音频素材
- **位置**: `assets/audio/`
- **格式**: WAV或OGG
- **要求**:
  - 背景音乐: 8位芯片音乐风格，循环播放
  - 音效:
    - 移动: "咔嗒-咔嗒"循环音效
    - 射击: 短促"咻"声 (0.1秒)
    - 爆炸: "嘭"声 (0.3秒)
    - 道具拾取: 清脆"叮"声 (0.2秒)

## 推荐资源网站

### 免费素材网站
1. **OpenGameArt** (https://opengameart.org/)
   - 搜索: "tank", "battle city", "nes style"
   - 许可证: 通常为CC0或CC-BY

2. **Kenney.nl** (https://kenney.nl/)
   - 免费游戏素材包
   - 许可证: CC0 (公共领域)

3. **itch.io** (https://itch.io/game-assets)
   - 搜索: "pixel art", "tank", "retro"
   - 注意查看许可证

4. **Spriters Resource** (https://www.spriters-resource.com/)
   - NES游戏精灵图提取
   - 仅供学习参考，注意版权

### 自制素材工具
- **Aseprite**: 像素艺术编辑器
- **Piskel**: 在线像素艺术编辑器
- **GIMP/Photoshop**: 通用图像编辑

## 下载方式

### 方式1: 手动下载
1. 访问上述资源网站
2. 下载符合要求的素材
3. 按照目录结构放置文件

### 方式2: 使用脚本下载 (需要提供URL)
```bash
python download_assets.py --url <资源URL>
```

### 方式3: 从NES ROM提取 (仅供学习)
可以使用工具如:
- **YY-CHR**: NES图形编辑器
- **Tile Molester**: 通用图形查看器
- **FCEUX**: NES模拟器 (调试模式可查看图形)

## 文件命名规范

### 精灵图命名
- 玩家坦克: `player_tank_up.png`, `player_tank_down.png`, `player_tank_left.png`, `player_tank_right.png`
- 敌方坦克: `enemy_basic_up.png`, `enemy_fast_up.png`, `enemy_heavy_up.png`, `enemy_elite_up.png`
- 子弹: `bullet_up.png`, `bullet_down.png`, `bullet_left.png`, `bullet_right.png`
- 地形: `terrain_grass.png`, `terrain_brick.png`, `terrain_steel.png`, `terrain_water.png`
- 道具: `powerup_star.png`, `powerup_life.png`, `powerup_clock.png`, `powerup_shield.png`, `powerup_bomb.png`
- 基地: `base.png`, `base_destroyed.png`
- 爆炸: `explosion_01.png`, `explosion_02.png`, `explosion_03.png`

### 音频命名
- 背景音乐: `bgm_main_theme.ogg`
- 音效: `sfx_move.wav`, `sfx_shoot.wav`, `sfx_explosion.wav`, `sfx_powerup.wav`

## 注意事项

1. **版权**: 确保使用的素材有合法授权
2. **格式**: 图像使用PNG (带透明通道)，音频使用WAV或OGG
3. **尺寸**: 严格按照8x8像素 (坦克) 或16x16像素 (基地) 要求
4. **调色板**: 使用NES调色板 (54种颜色)
5. **测试**: 下载后测试素材是否能正确加载和显示

## 当前状态

请检查各目录下的文件，确保所有必需的素材都已下载并正确放置。
