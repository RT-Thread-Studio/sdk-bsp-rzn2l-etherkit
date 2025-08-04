#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
分支版本检查脚本
检查当前分支是否在版本列表中，并验证分支版本映射关系
"""

import os
import sys
import subprocess
from pathlib import Path

def get_current_branch():
    """获取当前分支名称"""
    try:
        result = subprocess.run(
            ['git', 'rev-parse', '--abbrev-ref', 'HEAD'],
            capture_output=True, text=True, check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError:
        print("错误: 无法获取当前分支名称")
        return None

def get_all_branches():
    """获取所有本地分支"""
    try:
        result = subprocess.run(
            ['git', 'branch', '--format=%(refname:short)'],
            capture_output=True, text=True, check=True
        )
        return [branch.strip() for branch in result.stdout.splitlines() if branch.strip()]
    except subprocess.CalledProcessError:
        print("错误: 无法获取分支列表")
        return []

def load_versions():
    """从 versions.json 文件加载版本列表"""
    # 尝试多个可能的路径
    possible_paths = [
        Path("../.github/versions.json"),  # 从根目录运行
        Path("../../.github/versions.json"),  # 从docs/source运行
        Path(".github/versions.json"),  # 从项目根目录运行
    ]
    
    versions_file = None
    for path in possible_paths:
        if path.exists():
            versions_file = path
            break
    
    if not versions_file:
        print(f"错误: 版本文件不存在，尝试的路径:")
        for path in possible_paths:
            print(f"  - {path.absolute()}")
        return []
        
    try:
        import json
        with open(versions_file, 'r', encoding='utf-8') as f:
            config = json.load(f)
        
        versions = []
        for version_config in config.get('versions', []):
            versions.append(version_config['name'])
        
        return versions
    except Exception as e:
        print(f"错误: 无法解析版本配置文件: {e}")
        return []

def check_branch_version_mapping():
    """检查分支版本映射关系"""
    print("=" * 60)
    print("分支版本映射检查")
    print("=" * 60)
    
    # 获取当前分支
    current_branch = get_current_branch()
    if not current_branch:
        return False
    
    print(f"当前分支: {current_branch}")
    
    # 获取所有分支
    all_branches = get_all_branches()
    print(f"本地分支: {', '.join(all_branches)}")
    
    # 加载版本列表
    versions = load_versions()
    if not versions:
        print("错误: 没有找到有效的版本配置")
        return False
    
    print(f"版本列表: {', '.join(versions)}")
    print()
    
    # 检查映射关系
    print("分支版本映射关系:")
    print("-" * 40)
    
    mapping_found = False
    for version in versions:
        if version in all_branches:
            status = "✓" if version == current_branch else " "
            current_marker = " (当前)" if version == current_branch else ""
            print(f"{status} {version} -> {version}{current_marker}")
            mapping_found = True
        else:
            print(f"✗ {version} -> {version} (分支不存在)")
    
    print()
    
    # 检查当前分支是否在版本列表中
    if current_branch in versions:
        print(f"✓ 当前分支 '{current_branch}' 在版本列表中")
        return True
    else:
        print(f"⚠️  当前分支 '{current_branch}' 不在版本列表中")
        print("建议:")
        print(f"  1. 将分支 '{current_branch}' 添加到 .github/versions.json")
        print(f"  2. 或者切换到版本列表中的分支")
        return False

def check_github_actions_ready():
    """检查GitHub Actions是否准备就绪"""
    print("\n" + "=" * 60)
    print("GitHub Actions 准备状态检查")
    print("=" * 60)
    
    # 检查必要文件
    required_files = [
        ".github/workflows/gh-pages.yml",
        ".github/versions.json",
        "source/version_generator.py",
        "source/config.yaml"
    ]
    
    all_ready = True
    for file_path in required_files:
        if Path(file_path).exists():
            print(f"✓ {file_path}")
        else:
            print(f"✗ {file_path} (缺失)")
            all_ready = False
    
    print()
    
    if all_ready:
        print("✓ GitHub Actions 配置完整")
        return True
    else:
        print("⚠️  GitHub Actions 配置不完整")
        return False

def main():
    """主函数"""
    print("开始检查分支版本映射...")
    
    # 检查分支版本映射
    mapping_ok = check_branch_version_mapping()
    
    # 检查GitHub Actions准备状态
    actions_ready = check_github_actions_ready()
    
    print("\n" + "=" * 60)
    print("检查结果总结")
    print("=" * 60)
    
    if mapping_ok and actions_ready:
        print("🎉 所有检查通过！")
        print("✅ 分支版本映射正确")
        print("✅ GitHub Actions 配置完整")
        print("\n可以运行以下命令构建文档:")
        print("  python build_local.py")
        return 0
    else:
        print("⚠️  发现问题，请根据上述建议进行修复")
        return 1

if __name__ == "__main__":
    sys.exit(main()) 