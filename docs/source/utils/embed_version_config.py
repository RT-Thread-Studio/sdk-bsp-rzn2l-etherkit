#!/usr/bin/env python3
"""
版本配置嵌入脚本
在构建过程中将 .github/versions.json 的内容嵌入到 version_menu.js 中
完全基于 .github/versions.json，无硬编码版本信息
支持自动备份和还原 version_menu.js
"""

import json
import os
import sys
from pathlib import Path
import shutil

def load_versions_config():
    """加载 .github/versions.json 文件"""
    versions_file = Path(__file__).parent.parent.parent.parent / '.github' / 'versions.json'
    if not versions_file.exists():
        print(f"警告: 找不到版本配置文件 {versions_file}")
        return None
    try:
        with open(versions_file, 'r', encoding='utf-8') as f:
            config = json.load(f)
            if not isinstance(config, dict) or 'versions' not in config:
                print(f"错误: 版本配置文件格式不正确 {versions_file}")
                return None
            return config
    except Exception as e:
        print(f"错误: 无法读取版本配置文件: {e}")
        return None

def embed_config_to_js(versions_config):
    if not versions_config:
        return """
    // 获取嵌入的版本配置（在构建时由脚本生成）
    function getEmbeddedVersionConfig() {
        return null; // 无可用版本配置
    }
    """
    config_js = json.dumps(versions_config, ensure_ascii=False, indent=2)
    embedded_config_js = f"""
    // 获取嵌入的版本配置（在构建时由脚本生成）
    function getEmbeddedVersionConfig() {{
        return {config_js};
    }}
    """
    return embedded_config_js

def backup_js(js_file):
    bak_file = js_file.with_suffix('.js.bak')
    if js_file.exists():
        shutil.copy2(js_file, bak_file)
        print(f"已备份 {js_file} -> {bak_file}")
    return bak_file

def restore_js(js_file):
    bak_file = js_file.with_suffix('.js.bak')
    if bak_file.exists():
        shutil.copy2(bak_file, js_file)
        print(f"已还原 {bak_file} -> {js_file}")
        return True
    else:
        print(f"未找到备份文件 {bak_file}")
        return False

def update_version_menu_js(restore_after=False):
    versions_config = load_versions_config()
    embedded_config_js = embed_config_to_js(versions_config)
    js_file = Path(__file__).parent.parent / '_static' / 'version_menu.js'
    # 备份
    bak_file = backup_js(js_file)
    try:
        with open(js_file, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"错误: 无法读取 {js_file}: {e}")
        return False
    import re
    pattern = r'// 获取嵌入的版本配置（在构建时由脚本生成）\s+function getEmbeddedVersionConfig\(\)\s*\{[^}]*\}'
    if re.search(pattern, content, re.DOTALL):
        content = re.sub(pattern, embedded_config_js.strip(), content, flags=re.DOTALL)
        print("替换现有函数")
    else:
        fetch_pattern = r'(async function fetchVersionInfo\(\)\s*\{)'
        if re.search(fetch_pattern, content):
            content = re.sub(fetch_pattern, embedded_config_js.strip() + '\n\n    ' + r'\1', content)
            print("插入新函数")
        else:
            print("警告: 在文件中找不到合适的插入位置")
            return False
    try:
        with open(js_file, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"成功更新 {js_file}")
        if versions_config:
            print(f"嵌入版本配置: {len(versions_config.get('versions', []))} 个版本")
        else:
            print("嵌入兜底配置: 无可用版本")
    except Exception as e:
        print(f"错误: 无法写入 {js_file}: {e}")
        return False
    # 可选还原
    if restore_after:
        restore_js(js_file)
    return True

def main():
    restore_after = False
    if len(sys.argv) > 1 and sys.argv[1] == '--restore-after':
        restore_after = True
    print("开始嵌入版本配置...")
    if update_version_menu_js(restore_after=restore_after):
        print("版本配置嵌入完成")
        return 0
    else:
        print("版本配置嵌入失败")
        return 1

if __name__ == '__main__':
    sys.exit(main()) 