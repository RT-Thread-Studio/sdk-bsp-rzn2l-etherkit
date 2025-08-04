#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SDK 文档生成器
模块化的文档生成系统，支持配置化的文档构建
"""

import sys
import os
from pathlib import Path

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from utils import ConfigLoader, ProjectScanner, FileProcessor, IndexGenerator


class DocGenerator:
    def __init__(self, config_path: str = "config.yaml"):
        """初始化文档生成器"""
        self.config_loader = ConfigLoader(config_path)
        self.config_loader.validate_config()
        
        # 获取配置信息
        self.project_info = self.config_loader.get_project_info()
        self.repository_info = self.config_loader.get_repository_info()
        self.categories = self.config_loader.get_categories()
        self.generation_config = self.config_loader.get_generation_config()
        
        # 初始化组件
        self.project_scanner = ProjectScanner(
            self.repository_info.get('projects_dir', '../projects'),
            self.categories
        )
        
        self.file_processor = FileProcessor(
            self.repository_info.get('projects_dir', '../projects'),
            self.repository_info.get('docs_dir', '.'),
            self.generation_config
        )
        
        self.index_generator = IndexGenerator(
            self.repository_info.get('docs_dir', '.'),
            self.file_processor
        )

    def run(self):
        """运行文档生成流程"""
        print("开始生成文档...")
        
        try:
            # 1. 清理目标目录
            print("清理目标目录...")
            self.file_processor.cleanup_dest_dir()
            
            # 2. 扫描项目
            print("扫描项目...")
            category_mapping = self.project_scanner.scan_projects()
            
            # 3. 验证项目
            if not self.project_scanner.validate_projects():
                print("警告: 部分项目目录不存在")
            
            # 4. 处理每个分类
            for category, config in self.categories.items():
                projects = category_mapping.get(category, [])
                if projects:
                    category_name = config.get('name', category)
                    print(f"\n处理 {category_name} 分类...")
                    
                    for project in projects:
                        self.file_processor.copy_project_files(project, category)
            
            # 5. 生成索引文件
            print("\n生成索引文件...")
            self.index_generator.generate_all_indexes(
                self.categories, 
                category_mapping, 
                self.project_info
            )
            
            print("\n文档生成完成!")
            return True
            
        except Exception as e:
            print(f"文档生成失败: {e}")
            return False

    def get_statistics(self):
        """获取生成统计信息"""
        stats = {
            'total_projects': len(self.project_scanner.get_all_projects()),
            'categories': {}
        }
        
        for category, projects in self.project_scanner.category_mapping.items():
            stats['categories'][category] = len(projects)
        
        return stats


def main():
    """主函数"""
    import argparse
    
    parser = argparse.ArgumentParser(description='SDK 文档生成器')
    parser.add_argument('--config', '-c', default='config.yaml', 
                       help='配置文件路径 (默认: config.yaml)')
    parser.add_argument('--stats', '-s', action='store_true',
                       help='显示统计信息')
    
    args = parser.parse_args()
    
    try:
        generator = DocGenerator(args.config)
        
        if args.stats:
            # 只扫描项目，不生成文档
            generator.project_scanner.scan_projects()
            stats = generator.get_statistics()
            print("项目统计信息:")
            print(f"总项目数: {stats['total_projects']}")
            for category, count in stats['categories'].items():
                print(f"{category}: {count} 个项目")
        else:
            # 生成文档
            success = generator.run()
            if success:
                stats = generator.get_statistics()
                print(f"\n生成完成! 共处理 {stats['total_projects']} 个项目")
            else:
                sys.exit(1)
                
    except Exception as e:
        print(f"错误: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main() 