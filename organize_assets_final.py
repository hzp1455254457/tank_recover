#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
最终整理assets/src中的资源到项目要求的目录结构
"""

import os
import shutil
from pathlib import Path

# 项目根目录
PROJECT_ROOT = Path(__file__).parent
SRC_DIR = PROJECT_ROOT / "assets" / "src"
ASSETS_DIR = PROJECT_ROOT / "assets"
SPRITES_DIR = ASSETS_DIR / "sprites"
TERRAIN_DIR = SPRITES_DIR / "terrain"
UI_DIR = SPRITES_DIR / "ui"

def move_terrain_files():
    """将sprites根目录下的terrain文件移动到terrain目录"""
    print("整理terrain文件...")
    
    terrain_files = [
        "terrain_grass.png",
        "terrain_brick.png", 
        "terrain_steel.png",
        "terrain_water.png",
        "base.png",
        "base_destroyed.png"
    ]
    
    for filename in terrain_files:
        src_file = SPRITES_DIR / filename
        if src_file.exists():
            dst_file = TERRAIN_DIR / filename
            if not dst_file.exists():
                shutil.move(str(src_file), str(dst_file))
                print(f"  [MOVE] {filename} -> terrain/")
            else:
                print(f"  [SKIP] {filename} 已存在")
                src_file.unlink()  # 删除重复文件

def process_remaining_map_files():
    """处理map目录的剩余文件"""
    map_src = SRC_DIR / "map"
    
    if not map_src.exists():
        return
    
    print("处理map目录剩余文件...")
    
    # 处理剩余文件
    remaining_files = {
        "stone.png": "terrain_steel_alt.png",  # 石头作为钢墙的变体
        "wall0.png": "terrain_brick_alt0.png",  # 砖墙变体
        "wall1.png": "terrain_brick_alt1.png",  # 砖墙变体
        "river1.png": "terrain_water_alt.png"   # 水域变体
    }
    
    for file in map_src.iterdir():
        if file.is_file() and file.suffix == ".png":
            new_name = remaining_files.get(file.name, file.name)
            dst_file = TERRAIN_DIR / new_name
            
            if not dst_file.exists():
                shutil.move(str(file), str(dst_file))
                print(f"  [MOVE] {file.name} -> terrain/{new_name}")
            else:
                print(f"  [SKIP] {file.name} (已存在)")
                file.unlink()
    
    # 删除空的map目录
    try:
        map_src.rmdir()
        print("  [OK] 已删除空map目录")
    except:
        print(f"  [INFO] map目录不为空，保留")

def cleanup_empty_dirs():
    """清理空的src子目录"""
    print("清理空目录...")
    
    subdirs = ["bullet", "effect", "enemy", "player", "prop", "ui"]
    
    for subdir_name in subdirs:
        subdir = SRC_DIR / subdir_name
        if subdir.exists():
            try:
                # 检查是否为空
                if not any(subdir.iterdir()):
                    subdir.rmdir()
                    print(f"  [OK] 删除空目录: {subdir_name}")
                else:
                    print(f"  [INFO] 目录不为空: {subdir_name}")
                    # 列出剩余文件
                    for item in subdir.iterdir():
                        print(f"    - {item.name}")
            except Exception as e:
                print(f"  [WARN] 无法删除 {subdir_name}: {e}")

def organize_ui_files():
    """整理UI文件，将base相关文件移到terrain"""
    print("整理UI和terrain文件...")
    
    # base相关文件应该在terrain目录
    ui_base_files = ["base.png", "base_destroyed.png"]
    
    for filename in ui_base_files:
        src_file = UI_DIR / filename
        if src_file.exists():
            dst_file = TERRAIN_DIR / filename
            if not dst_file.exists():
                shutil.move(str(src_file), str(dst_file))
                print(f"  [MOVE] {filename} -> terrain/")
            else:
                print(f"  [SKIP] {filename} 已存在")
                src_file.unlink()

def main():
    """主函数"""
    print("=" * 60)
    print("最终整理资源文件...")
    print("=" * 60)
    print()
    
    # 1. 移动terrain文件到terrain目录
    move_terrain_files()
    print()
    
    # 2. 处理map目录剩余文件
    process_remaining_map_files()
    print()
    
    # 3. 整理UI文件
    organize_ui_files()
    print()
    
    # 4. 清理空目录
    cleanup_empty_dirs()
    print()
    
    # 5. 尝试删除src目录
    try:
        if SRC_DIR.exists() and not any(SRC_DIR.iterdir()):
            SRC_DIR.rmdir()
            print("[OK] 已删除空的src目录")
        else:
            print("[INFO] src目录不为空，保留")
            print("剩余内容:")
            for item in SRC_DIR.iterdir():
                print(f"  - {item.name}")
    except Exception as e:
        print(f"[WARN] 无法删除src目录: {e}")
    
    print()
    print("=" * 60)
    print("整理完成！")
    print("=" * 60)
    print()
    print("最终目录结构:")
    print("  assets/sprites/player/    - 玩家坦克")
    print("  assets/sprites/enemy/     - 敌方坦克")
    print("  assets/sprites/bullets/   - 子弹")
    print("  assets/sprites/terrain/   - 地形图块和基地")
    print("  assets/sprites/powerups/  - 道具")
    print("  assets/sprites/effects/   - 特效")
    print("  assets/sprites/ui/        - UI元素")

if __name__ == "__main__":
    main()
