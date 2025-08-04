#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
新项目快速设置脚本
帮助用户快速配置新的SDK文档项目
"""

import os
import sys
import shutil
import yaml
from pathlib import Path

def get_user_input(prompt, default=""):
    """获取用户输入"""
    if default:
        user_input = input(f"{prompt} (默认: {default}): ").strip()
        return user_input if user_input else default
    else:
        return input(f"{prompt}: ").strip()

def create_config_interactive():
    """交互式创建配置文件"""
    print("=" * 60)
    print("SDK 文档项目快速设置")
    print("=" * 60)
    
    # 项目基本信息
    print("\n📋 项目基本信息:")
    project_name = get_user_input("SDK文档名称", "My_SDK_Docs")
    project_title = get_user_input("SDK文档标题", "My SDK 文档")
    project_description = get_user_input("SDK描述", f"{project_title} 提供了丰富的示例项目，包括基础功能、驱动示例和组件示例。")
    project_version = get_user_input("版本号", "0.0.1")
    project_author = get_user_input("作者", "your_name")
    project_copyright = get_user_input("版权信息", "2025, your_company")
    
    # 仓库信息
    print("\n📦 仓库信息:")
    repo_name = get_user_input("GitHub仓库名称", "your-sdk-repo")
    
    # 项目命名模式
    print("\n🔧 项目命名模式:")
    print("请根据你的项目命名规则设置模式匹配")
    sdk_prefix = get_user_input("SDK项目前缀", "my_sdk")
    
    # 构建配置
    config = {
        'project': {
            'name': project_name,
            'title': project_title,
            'description': project_description,
            'version': project_version,
            'author': project_author,
            'copyright': project_copyright,
            'language': 'zh_CN'
        },
        'repository': {
            'name': repo_name,
            'projects_dir': '../projects',
            'docs_dir': '.'
        },
        'categories': {
            'basic': {
                'name': '基础篇',
                'description': '基础功能示例',
                'patterns': [
                    f'{sdk_prefix}_basic_*',
                    f'{sdk_prefix}_blink_led',
                    f'{sdk_prefix}_factory'
                ]
            },
            'driver': {
                'name': '驱动篇',
                'description': '外设驱动示例',
                'patterns': [
                    f'{sdk_prefix}_driver_*',
                    f'{sdk_prefix}_usb_*'
                ]
            },
            'component': {
                'name': '组件篇',
                'description': '网络组件示例',
                'patterns': [
                    f'{sdk_prefix}_component_*'
                ]
            },
            'protocol': {
                'name': '工业协议篇',
                'description': '工业协议示例',
                'patterns': [
                    f'{sdk_prefix}_ethercat_*',
                    f'{sdk_prefix}_modbus_*',
                    f'{sdk_prefix}_profinet_*',
                    f'{sdk_prefix}_ethernetip_*',
                    f'{sdk_prefix}_ethernet'
                ]
            }
        },
        'generation': {
            'copy_files': [
                'README.md',
                'README_zh.md'
            ],
            'copy_dirs': [
                'figures'
            ],
            'output_structure': [
                'basic',
                'driver',
                'component',
                'protocol'
            ]
        },
        'sphinx': {
            'theme': 'sphinx_rtd_theme',
            'extensions': [
                'myst_parser'
            ],
            'source_suffix': {
                '.rst': 'restructuredtext',
                '.md': 'markdown'
            },
            'myst_extensions': [
                'colon_fence',
                'deflist',
                'dollarmath',
                'html_image',
                'html_admonition',
                'replacements',
                'smartquotes',
                'strikethrough',
                'substitution',
                'tasklist'
            ]
        }
    }
    
    return config

def save_config(config, output_path="config.yaml"):
    """保存配置文件"""
    with open(output_path, 'w', encoding='utf-8') as f:
        yaml.dump(config, f, default_flow_style=False, allow_unicode=True, indent=2)
    print(f"✓ 配置文件已保存: {output_path}")

def create_github_workflow(repo_name):
    """创建GitHub Actions工作流"""
    workflow_content = f"""name: Deploy Sphinx docs to GitHub Pages

on:
  push:
    branches:
      - master
      - main

jobs:
  build-deploy:
    runs-on: ubuntu-latest
    permissions:
      contents: write
      pages: write
      id-token: write
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
        with:
          fetch-depth: 0

      - name: Set up Python
        uses: actions/setup-python@v5
        with:
          python-version: '3.11'
          
      - name: Set up environment
        run: |
          export LANG=C.UTF-8
          export LC_ALL=C.UTF-8
          
      - name: Configure Git
        run: |
          git config --global user.name "github-actions[bot]"
          git config --global user.email "41898282+github-actions[bot]@users.noreply.github.com"

      - name: Cache pip dependencies
        uses: actions/cache@v3
        with:
          path: ~/.cache/pip
          key: ${{{{ runner.os }}}}-pip-${{{{ hashFiles('source/requirements.txt') }}}}
          restore-keys: |
            ${{{{ runner.os }}}}-pip-

      - name: Install dependencies
        run: |
          python -m pip install --upgrade pip
          pip install -r source/requirements.txt

      - name: Generate documentation
        run: |
          cd source
          python version_generator.py
          
      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v4
        with:
          github_token: ${{{{ secrets.GITHUB_TOKEN }}}}
          publish_dir: source/_build/html
          force_orphan: true
          user_name: 'github-actions[bot]'
          user_email: '41898282+github-actions[bot]@users.noreply.github.com'
"""
    
    workflow_dir = Path("../.github/workflows")
    workflow_dir.mkdir(parents=True, exist_ok=True)
    
    workflow_path = workflow_dir / "gh-pages.yml"
    with open(workflow_path, 'w', encoding='utf-8') as f:
        f.write(workflow_content)
    
    print(f"✓ GitHub Actions工作流已创建: {workflow_path}")

def create_readthedocs_config():
    """创建Read the Docs配置文件"""
    rtd_config = """# .readthedocs.yaml
# Read the Docs configuration file
# See https://docs.readthedocs.io/en/stable/config-file/v2.html for details
  
# Required
version: 2

# Set the OS, Python version and other tools you might need
build:
  os: ubuntu-22.04
  tools:
    python: "3.12"

# Build documentation in the "docs/" directory with Sphinx
sphinx:
   configuration: source/conf.py

# Optional but recommended, declare the Python requirements required
# to build your documentation
python:
    install:
      - requirements: source/requirements.txt
"""
    
    rtd_path = Path("../.readthedocs.yaml")
    with open(rtd_path, 'w', encoding='utf-8') as f:
        f.write(rtd_config)
    
    print(f"✓ Read the Docs配置文件已创建: {rtd_path}")

def create_versions_list():
    """创建版本列表文件"""
    versions_content = """# 版本列表
# 每行一个版本，以#开头的行为注释
# 支持以下格式:
# - master (最新版本)
# - v1.0 (具体版本)
# - v1.1
# - v2.0

master
v1.0
"""
    
    versions_dir = Path("../.github")
    versions_dir.mkdir(parents=True, exist_ok=True)
    
    versions_path = versions_dir / "versions.list"
    with open(versions_path, 'w', encoding='utf-8') as f:
        f.write(versions_content)
    
    print(f"✓ 版本列表文件已创建: {versions_path}")

def main():
    """主函数"""
    print("🚀 SDK文档项目快速设置工具")
    print("此工具将帮助您快速配置新的SDK文档项目")
    
    # 检查是否在正确的目录
    if not Path("doc_generator.py").exists():
        print("❌ 错误: 请在source目录中运行此脚本")
        sys.exit(1)
    
    # 交互式创建配置
    config = create_config_interactive()
    
    # 保存配置文件
    save_config(config)
    
    # 创建GitHub相关文件
    repo_name = config['repository']['name']
    create_github_workflow(repo_name)
    create_readthedocs_config()
    create_versions_list()
    
    print("\n" + "=" * 60)
    print("✅ 项目设置完成!")
    print("=" * 60)
    print("\n📝 下一步操作:")
    print("1. 检查并修改 config.yaml 文件")
    print("2. 确保项目目录结构正确")
    print("3. 运行测试构建:")
    print("   python build_local.py --check")
    print("4. 构建文档:")
    print("   python build_local.py")
    print("5. 本地预览:")
    print("   python build_local.py --serve")
    print("\n🔗 部署相关:")
    print("- GitHub Pages: 推送到master分支自动部署")
    print("- Read the Docs: 连接仓库后自动构建")
    print("\n📚 更多信息请查看 README.md")

if __name__ == "__main__":
    main() 