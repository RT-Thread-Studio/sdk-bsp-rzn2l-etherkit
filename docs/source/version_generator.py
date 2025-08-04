#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
版本生成器
根据 .github/versions.json 文件生成不同版本的文档
支持多分支文档生成
"""

import os
import sys
import shutil
import subprocess
import yaml
import json
from pathlib import Path

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
        with open(versions_file, 'r', encoding='utf-8') as f:
            config = json.load(f)
        
        versions = config.get('versions', [])
        print(f"加载版本配置: {[v['name'] for v in versions]}")
        return versions
    except Exception as e:
        print(f"错误: 无法解析版本配置文件: {e}")
        return []

def get_branch_name():
    """获取当前分支名称"""
    try:
        result = subprocess.run(
            ['git', 'rev-parse', '--abbrev-ref', 'HEAD'],
            capture_output=True, text=True, check=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError:
        print("警告: 无法获取当前分支名称")
        return None

def checkout_branch(branch_name):
    """切换到指定分支"""
    try:
        print(f"切换到分支: {branch_name}")
        subprocess.run(['git', 'checkout', branch_name], check=True)
        return True
    except subprocess.CalledProcessError as e:
        print(f"错误: 无法切换到分支 {branch_name}: {e}")
        return False

def get_branch_versions():
    """获取当前分支对应的版本列表"""
    current_branch = get_branch_name()
    if not current_branch:
        return []
    
    versions = load_versions()
    branch_versions = []
    
    for version in versions:
        # 检查版本是否对应当前分支
        if version['branch'] == current_branch:
            branch_versions.append(version)
        elif version['branch'] == 'master' and current_branch == 'main':
            # 处理主分支名称差异
            branch_versions.append(version)
    
    print(f"当前分支 {current_branch} 对应的版本: {[v['name'] for v in branch_versions]}")
    return branch_versions

def build_version_docs(version_config, branch_name=None):
    """为指定版本构建文档"""
    version_name = version_config['name']
    print(f"\n开始构建版本 {version_name} 的文档...")
    
    # 确定对应的分支名称
    if branch_name is None:
        branch_name = version_config['branch']
    
    # 创建版本输出目录
    output_dir = Path(f"_build/html/{version_config['url_path']}")
    
    # 清理输出目录
    if output_dir.exists():
        shutil.rmtree(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    
    try:
        # 检查是否在GitHub Actions环境中
        is_github_actions = os.environ.get('GITHUB_ACTIONS') == 'true'
        
        # 保存当前分支名称
        current_branch = get_branch_name()
        print(f"当前分支: {current_branch}")
        
        # 如果需要切换到不同分支
        if branch_name != current_branch:
            print(f"切换到分支: {branch_name}")
            subprocess.run(['git', 'checkout', branch_name], check=True)
            
            # 拉取最新代码
            subprocess.run(['git', 'pull', 'origin', branch_name], check=True)
        else:
            print(f"已在目标分支: {branch_name}")
        
        # 运行文档生成脚本
        subprocess.run([
            sys.executable, 'doc_generator.py'
        ], cwd=".", check=True)
        
        # 构建HTML文档
        subprocess.run([
            sys.executable, '-m', 'sphinx.cmd.build',
            '-b', 'html',
            '.',
            str(output_dir)
        ], cwd=".", check=True)
        
        # 生成版本切换配置文件
        generate_version_config(output_dir, version_config)
        
        # 生成版本切换配置文件
        generate_version_config(output_dir, version_config)
        
        # 如果切换了分支，恢复到原始分支
        if branch_name != current_branch:
            print(f"恢复到原始分支: {current_branch}")
            subprocess.run(['git', 'checkout', current_branch], check=True)
        
        print(f"✓ 版本 {version_name} 文档构建完成: {output_dir}")
        return True
        
    except subprocess.CalledProcessError as e:
        print(f"✗ 版本 {version_name} 文档构建失败: {e}")
        return False

def generate_version_config(output_dir, current_version_config):
    """生成版本切换配置文件"""
    print(f"生成版本切换配置...")
    
    # 加载所有版本信息
    all_versions = load_versions()
    
    # 创建版本配置对象
    version_config = {
        "current_version": current_version_config['name'],
        "versions": {}
    }
    
    # 为每个版本创建配置
    for version in all_versions:
        version_config["versions"][version['name']] = {
            "display_name": version['display_name'],
            "url_path": version['url_path'],
            "description": version.get('description', '')
        }
    
    # 写入配置文件
    config_file = output_dir / "_static" / "version_config.json"
    config_file.parent.mkdir(parents=True, exist_ok=True)
    
    with open(config_file, 'w', encoding='utf-8') as f:
        json.dump(version_config, f, ensure_ascii=False, indent=2)
    
    print(f"✓ 版本配置已生成: {config_file}")

def create_root_redirect():
    """创建根目录重定向页面"""
    print("\n创建根目录重定向页面...")
    
    # 加载版本配置
    versions = load_versions()
    default_version = None
    
    # 查找默认版本
    for version in versions:
        if version['name'] == 'master':
            default_version = version
            break
    
    if not default_version:
        print("警告: 未找到默认版本，使用第一个版本")
        default_version = versions[0] if versions else None
    
    if not default_version:
        print("错误: 没有可用的版本配置")
        return False
    
    # 创建根目录的 index.html，重定向到默认版本
    root_index = Path("_build/html/index.html")
    root_index.parent.mkdir(parents=True, exist_ok=True)
    
    redirect_html = f"""<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SDK 文档</title>
    <meta http-equiv="refresh" content="0; url=./{default_version['url_path']}/">
    <style>
        body {{
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }}
        .container {{
            text-align: center;
            background: rgba(255, 255, 255, 0.1);
            padding: 40px;
            border-radius: 12px;
            backdrop-filter: blur(10px);
        }}
        .spinner {{
            border: 3px solid rgba(255, 255, 255, 0.3);
            border-top: 3px solid white;
            border-radius: 50%;
            width: 40px;
            height: 40px;
            animation: spin 1s linear infinite;
            margin: 0 auto 20px;
        }}
        @keyframes spin {{
            0% {{ transform: rotate(0deg); }}
            100% {{ transform: rotate(360deg); }}
        }}
        h1 {{
            margin: 0 0 10px 0;
            font-size: 24px;
        }}
        p {{
            margin: 0;
            opacity: 0.9;
        }}
        a {{
            color: white;
            text-decoration: underline;
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="spinner"></div>
        <h1>SDK 文档</h1>
        <p>正在跳转到{default_version['display_name']}...</p>
        <p><a href="./{default_version['url_path']}/">如果页面没有自动跳转，请点击这里</a></p>
    </div>
</body>
</html>"""
    
    with open(root_index, 'w', encoding='utf-8') as f:
        f.write(redirect_html)
    
    print(f"✓ 根目录重定向页面创建完成: {root_index}")
    return True

def main():
    """主函数"""
    print("开始生成多版本文档...")
    
    # 检查是否在GitHub Actions环境中
    is_github_actions = os.environ.get('GITHUB_ACTIONS') == 'true'
    
    if is_github_actions:
        print("检测到GitHub Actions环境")
        # 在GitHub Actions中，为所有版本构建文档
        versions = load_versions()
        if not versions:
            print("错误: 没有找到有效的版本配置")
            return 1
        
        # 获取当前分支名称
        current_branch = get_branch_name()
        print(f"当前触发分支: {current_branch}")
        
        # 为每个版本构建文档
        results = {}
        for version_config in versions:
            # 检查分支是否存在
            try:
                subprocess.run(['git', 'ls-remote', '--heads', 'origin', version_config['branch']], 
                             capture_output=True, check=True)
                print(f"✓ 分支 {version_config['branch']} 存在，开始构建")
                success = build_version_docs(version_config, version_config['branch'])
            except subprocess.CalledProcessError:
                print(f"⚠️  分支 {version_config['branch']} 不存在，跳过构建")
                success = False
            results[version_config['name']] = success
    else:
        print("本地构建环境")
        # 在本地环境中，可以选择构建所有版本或只构建当前分支对应的版本
        import sys
        build_all = len(sys.argv) > 1 and sys.argv[1] == '--all'
        
        if build_all:
            print("构建所有版本...")
            versions = load_versions()
            results = {}
            for version_config in versions:
                # 检查分支是否存在
                try:
                    subprocess.run(['git', 'ls-remote', '--heads', 'origin', version_config['branch']], 
                                 capture_output=True, check=True)
                    print(f"✓ 分支 {version_config['branch']} 存在，开始构建")
                    success = build_version_docs(version_config, version_config['branch'])
                except subprocess.CalledProcessError:
                    print(f"⚠️  分支 {version_config['branch']} 不存在，跳过构建")
                    success = False
                results[version_config['name']] = success
        else:
            print("只构建当前分支对应的版本...")
            # 只构建当前分支对应的版本
            branch_versions = get_branch_versions()
            if not branch_versions:
                print("警告: 当前分支没有对应的版本配置")
                # 尝试构建默认版本
                versions = load_versions()
                branch_versions = [v for v in versions if v['name'] == 'master']
                if not branch_versions:
                    branch_versions = versions[:1] if versions else []
            
            results = {}
            for version_config in branch_versions:
                success = build_version_docs(version_config)
                results[version_config['name']] = success
    
    # 输出结果
    print("\n" + "="*50)
    print("版本生成结果:")
    for version_name, success in results.items():
        status = "✓ 成功" if success else "✗ 失败"
        print(f"  {version_name}: {status}")
    
    success_count = sum(1 for success in results.values() if success)
    total_count = len(results)
    
    print(f"\n总计: {success_count}/{total_count} 个版本生成成功")
    
    # 在所有版本构建完成后创建根目录重定向页面
    if success_count > 0:
        create_root_redirect()
    
    if success_count == total_count:
        print("🎉 所有版本生成完成！")
        return 0
    else:
        print("⚠️  部分版本生成失败，请检查错误信息")
        return 1

if __name__ == "__main__":
    sys.exit(main())
