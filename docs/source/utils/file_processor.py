#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
文件处理器模块
负责复制和处理项目文件
"""

import shutil
import os
from pathlib import Path
from typing import List, Dict


class FileProcessor:
    def __init__(self, source_dir: str, dest_dir: str, config: Dict):
        self.source_dir = Path(source_dir)
        self.dest_dir = Path(dest_dir)
        self.config = config
        self.copy_files = config.get('copy_files', [])
        self.copy_dirs = config.get('copy_dirs', [])

    def copy_project_files(self, project_name: str, category: str) -> bool:
        """复制项目文件到目标目录"""
        source_project = self.source_dir / project_name
        dest_project = self.dest_dir / category / project_name

        if not source_project.exists():
            print(f"警告: 源项目目录不存在: {source_project}")
            return False

        # 创建目标目录
        dest_project.mkdir(parents=True, exist_ok=True)

        # 复制指定文件
        for file_name in self.copy_files:
            source_file = source_project / file_name
            if source_file.exists():
                shutil.copy2(source_file, dest_project)
                print(f"复制文件: {project_name}/{file_name}")
            else:
                print(f"警告: 文件不存在: {source_file}")

        # 复制指定目录
        for dir_name in self.copy_dirs:
            source_dir_path = source_project / dir_name
            if source_dir_path.exists() and source_dir_path.is_dir():
                dest_dir_path = dest_project / dir_name
                if dest_dir_path.exists():
                    shutil.rmtree(dest_dir_path)
                shutil.copytree(source_dir_path, dest_dir_path)
                print(f"复制目录: {project_name}/{dir_name}")

        return True

    def get_readme_title(self, project_name: str, category: str) -> str:
        """从README_zh.md文件中提取一级标题"""
        readme_path = self.dest_dir / category / project_name / "README_zh.md"
        if readme_path.exists():
            try:
                with open(readme_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    # 查找第一个一级标题
                    lines = content.split('\n')
                    for line in lines:
                        line = line.strip()
                        if line.startswith('# ') and len(line) > 2:
                            return line[2:].strip()  # 移除 "# " 前缀
            except Exception as e:
                print(f"读取 {project_name}/README_zh.md 标题时出错: {e}")
        
        # 如果无法读取标题，使用项目名称作为后备
        return project_name.replace("etherkit_", "").replace("_", " ").title()

    def cleanup_dest_dir(self):
        """清理目标目录"""
        if self.dest_dir.exists():
            # 保存Sphinx必需的文件
            sphinx_files = {}
            for file_name in ['conf.py', 'requirements.txt', 'config.yaml']:
                file_path = self.dest_dir / file_name
                if file_path.exists():
                    with open(file_path, 'r', encoding='utf-8') as f:
                        sphinx_files[file_name] = f.read()
            
            # 保存静态文件目录中的文件
            static_dir = self.dest_dir / '_static'
            static_files = {}
            if static_dir.exists():
                for file_path in static_dir.rglob('*'):
                    if file_path.is_file():
                        relative_path = file_path.relative_to(static_dir)
                        with open(file_path, 'rb') as f:
                            static_files[str(relative_path)] = f.read()
            
            # 保存模板文件目录中的文件
            templates_dir = self.dest_dir / '_templates'
            template_files = {}
            if templates_dir.exists():
                for file_path in templates_dir.rglob('*'):
                    if file_path.is_file():
                        relative_path = file_path.relative_to(templates_dir)
                        with open(file_path, 'r', encoding='utf-8') as f:
                            template_files[str(relative_path)] = f.read()
            
            # 只删除生成的文档目录，不删除整个目录
            for category in ['start', 'basic', 'driver', 'component', 'protocol']:
                category_dir = self.dest_dir / category
                if category_dir.exists():
                    shutil.rmtree(category_dir)
                    print(f"清理目录: {category_dir}")
            
            # 恢复Sphinx必需的文件
            for file_name, content in sphinx_files.items():
                file_path = self.dest_dir / file_name
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
            
            # 重新创建静态文件目录并恢复文件
            if static_dir.exists():
                shutil.rmtree(static_dir)
            static_dir.mkdir(exist_ok=True)
            
            # 恢复静态文件
            for file_path, content in static_files.items():
                full_path = static_dir / file_path
                full_path.parent.mkdir(parents=True, exist_ok=True)
                with open(full_path, 'wb') as f:
                    f.write(content)
                
            # 重新创建模板文件目录并恢复文件
            if templates_dir.exists():
                shutil.rmtree(templates_dir)
            templates_dir.mkdir(exist_ok=True)
            
            # 恢复模板文件
            for file_path, content in template_files.items():
                full_path = templates_dir / file_path
                full_path.parent.mkdir(parents=True, exist_ok=True)
                with open(full_path, 'w', encoding='utf-8') as f:
                    f.write(content)
        else:
            # 如果目标目录不存在，创建它
            self.dest_dir.mkdir(exist_ok=True)
            (self.dest_dir / '_static').mkdir(exist_ok=True)
            (self.dest_dir / '_templates').mkdir(exist_ok=True) 