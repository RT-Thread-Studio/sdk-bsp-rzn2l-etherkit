#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
项目扫描器模块
负责扫描和分类项目目录
"""

import os
import fnmatch
from pathlib import Path
from typing import Dict, List, Tuple


class ProjectScanner:
    def __init__(self, projects_dir: str, categories: Dict):
        self.projects_dir = Path(projects_dir)
        self.categories = categories
        self.category_mapping = {cat: [] for cat in categories.keys()}

    def scan_projects(self) -> Dict[str, List[str]]:
        """扫描项目目录并分类"""
        if not self.projects_dir.exists():
            raise FileNotFoundError(f"项目目录不存在: {self.projects_dir}")

        print("开始扫描项目...")
        
        for project_dir in self.projects_dir.iterdir():
            if project_dir.is_dir():
                project_name = project_dir.name
                category = self._classify_project(project_name)
                if category:
                    self.category_mapping[category].append(project_name)
                    print(f"分类项目: {project_name} -> {category}")

        return self.category_mapping

    def _classify_project(self, project_name: str) -> str:
        """根据项目名称分类"""
        for category, config in self.categories.items():
            patterns = config.get('patterns', [])
            for pattern in patterns:
                if fnmatch.fnmatch(project_name, pattern):
                    return category
        return None

    def get_projects_by_category(self, category: str) -> List[str]:
        """获取指定分类的项目列表"""
        return self.category_mapping.get(category, [])

    def get_all_projects(self) -> List[str]:
        """获取所有项目"""
        all_projects = []
        for projects in self.category_mapping.values():
            all_projects.extend(projects)
        return all_projects

    def validate_projects(self) -> bool:
        """验证项目目录结构"""
        for project_name in self.get_all_projects():
            project_path = self.projects_dir / project_name
            if not project_path.exists():
                print(f"警告: 项目目录不存在: {project_path}")
                return False
        return True 