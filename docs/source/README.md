# SDK 文档构建系统

这是一个模块化的 SDK 文档构建系统，支持配置化的文档生成和自动化部署。系统设计为可复用，只需修改一个配置文件即可快速迁移到其他SDK项目。

## 🚀 快速开始

### 新项目快速设置

1. **复制文档系统到新项目**：
   ```bash
   cp -r source/ your-new-sdk-project/
   cd your-new-sdk-project/
   ```

2. **运行快速设置工具**：
   ```bash
   python setup_new_project.py
   ```
   按照提示输入项目信息，工具会自动生成所有配置文件。

3. **构建文档**：
   ```bash
   python build_local.py
   ```

### 现有项目迁移

1. **复制配置文件**：
   ```bash
   cp template_config.yaml config.yaml
   ```

2. **修改配置文件**：
   编辑 `config.yaml`，修改以下关键信息：
   - `project.*`: 项目基本信息
   - `repository.name`: 仓库名称
   - `categories.*.patterns`: 项目命名模式

3. **构建文档**：
   ```bash
   python build_local.py
   ```

## 📁 目录结构

```
source/
├── config.yaml              # 主配置文件 (需要根据项目修改)
├── template_config.yaml     # 模板配置文件
├── doc_generator.py         # 主文档生成器
├── version_generator.py     # 版本生成器 (主要构建脚本)
├── build_local.py           # 本地构建脚本
├── setup_new_project.py     # 新项目快速设置工具
├── conf.py                  # Sphinx 配置文件
├── requirements.txt         # Python 依赖
├── utils/                   # 工具模块
│   ├── __init__.py
│   ├── config_loader.py     # 配置加载器
│   ├── project_scanner.py   # 项目扫描器
│   ├── file_processor.py    # 文件处理器
│   └── index_generator.py   # 索引生成器
└── _static/                 # 静态文件
    ├── version_menu.css     # 版本菜单样式
    └── version_menu.js      # 版本菜单脚本
```

## 🛠️ 使用方法

### 本地构建

```bash
# 检查依赖
python build_local.py --check

# 构建文档
python build_local.py

# 清理构建并重新构建
python build_local.py --clean

# 构建并启动本地服务器预览
python build_local.py --serve

# 指定端口预览
python build_local.py --serve --port 8080
```

### 生产构建

```bash
# 使用版本生成器 (支持多版本)
python version_generator.py
```

## ⚙️ 配置说明

### 项目配置 (project)

```yaml
project:
  name: "Your_SDK_Docs"      # SDK 文档名称
  title: "Your SDK 文档"      # 文档标题
  description: "SDK 描述"     # 文档描述
  version: "0.0.1"           # 版本号
  author: "your_name"        # 作者
  copyright: "2025, company" # 版权信息
  language: "zh_CN"          # 语言
```

### 仓库配置 (repository)

```yaml
repository:
  name: "your-sdk-repo"      # GitHub仓库名称
  projects_dir: "../projects" # 项目目录路径
  docs_dir: "."              # 文档输出目录
```

### 分类配置 (categories)

```yaml
categories:
  basic:
    name: "基础篇"
    description: "基础功能示例"
    patterns:
      - "your_basic_*"       # 项目命名模式
      - "your_blink_led"
```

### 生成配置 (generation)

```yaml
generation:
  copy_files:                # 要复制的文件
    - "README.md"
    - "README_zh.md"
  copy_dirs:                 # 要复制的目录
    - "figures"
  output_structure:          # 输出目录结构
    - "basic"
    - "driver"
```

## 🔧 模块说明

### ConfigLoader
负责加载和验证配置文件，提供配置信息的访问接口。

### ProjectScanner
扫描项目目录，根据配置的模式对项目进行分类。

### FileProcessor
处理文件复制操作，支持指定文件和目录的复制。

### IndexGenerator
生成各种索引文件，包括主索引和分类索引。

## 🌐 部署支持

### GitHub Pages
- 自动部署到 GitHub Pages
- 支持多版本文档
- 自动生成版本切换菜单

### Read the Docs
- 支持 Read the Docs 自动构建
- 配置文件已预置

## 📋 迁移检查清单

### 必需修改的文件
- [ ] `config.yaml` - 项目配置
- [ ] `.github/workflows/gh-pages.yml` - GitHub Actions
- [ ] `.readthedocs.yaml` - Read the Docs配置
- [ ] `.github/versions.list` - 版本列表

### 可选修改的文件
- [ ] `conf.py` - Sphinx配置 (高级用户)
- [ ] `requirements.txt` - Python依赖 (如需要)
- [ ] `_static/` - 静态文件 (如需要)

### 验证步骤
1. [ ] 运行 `python build_local.py --check`
2. [ ] 运行 `python build_local.py`
3. [ ] 检查生成的文档结构
4. [ ] 测试本地预览 `python build_local.py --serve`
5. [ ] 推送到GitHub测试自动部署

## 🔍 故障排除

### 常见问题

1. **配置文件不存在**: 确保 `config.yaml` 文件存在且格式正确
2. **项目目录不存在**: 检查 `projects_dir` 路径是否正确
3. **依赖安装失败**: 确保安装了所有 Python 依赖
4. **Sphinx 构建失败**: 检查生成的文档文件是否完整

### 调试命令

```bash
# 查看项目统计
python doc_generator.py --stats

# 使用自定义配置文件
python doc_generator.py --config my_config.yaml

# 检查依赖
python build_local.py --check
```

## 📚 高级配置

### 自定义Sphinx配置

在 `conf.py` 中可以添加自定义的Sphinx配置：

```python
# 添加自定义扩展
extensions.append('sphinx.ext.autodoc')

# 自定义主题选项
html_theme_options = {
    'navigation_depth': 4,
    'titles_only': False,
}
```

### 添加新的分类

1. 在 `config.yaml` 中添加新的分类配置
2. 在 `output_structure` 中添加新分类
3. 更新主索引文件中的 toctree

### 修改文件复制规则

在 `generation.copy_files` 和 `generation.copy_dirs` 中添加或删除项目。

## 🤝 贡献指南

1. Fork 项目
2. 创建功能分支
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 📄 许可证

Apache License 2.0 