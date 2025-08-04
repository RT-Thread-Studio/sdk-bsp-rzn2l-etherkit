# SDK 文档系统迁移指南

本指南将帮助您快速将文档构建系统迁移到新的SDK项目中。

## 🚀 快速迁移 (推荐)

### 方法一：使用自动设置工具

1. **复制文档系统**：
   ```bash
   cp -r source/ your-new-sdk-project/
   cd your-new-sdk-project/
   ```

2. **运行自动设置**：
   ```bash
   python setup_new_project.py
   ```
   按照提示输入项目信息，工具会自动生成所有配置文件。

3. **验证构建**：
   ```bash
   python build_local.py --check
   python build_local.py
   ```

### 方法二：手动配置

1. **复制必要文件**：
   ```bash
   # 复制核心文件
   cp -r source/ your-new-sdk-project/
   cd your-new-sdk-project/
   
   # 复制配置文件模板
   cp template_config.yaml config.yaml
   ```

2. **修改配置文件**：
   编辑 `config.yaml`，修改以下内容：

   ```yaml
   project:
     name: "Your_SDK_Docs"           # 修改为你的SDK名称
     title: "Your SDK 文档"           # 修改为你的SDK标题
     description: "你的SDK描述"        # 修改描述
     version: "0.0.1"                # 修改版本号
     author: "your_name"             # 修改作者
     copyright: "2025, your_company" # 修改版权信息

   repository:
     name: "your-sdk-repo"           # 修改为你的GitHub仓库名

   categories:
     basic:
       patterns:
         - "your_basic_*"            # 修改为你的项目命名模式
         - "your_blink_led"
     driver:
       patterns:
         - "your_driver_*"           # 修改为你的驱动项目命名模式
     # ... 其他分类
   ```

3. **创建GitHub配置文件**：
   ```bash
   # 创建GitHub Actions
   mkdir -p ../.github/workflows/
   cp ../.github/workflows/gh-pages.yml ../.github/workflows/gh-pages.yml
   
   # 创建版本列表
   mkdir -p ../.github/
   echo "master" > ../.github/versions.list
   echo "v1.0" >> ../.github/versions.list
   
   # 创建Read the Docs配置
   cp ../.readthedocs.yaml ../.readthedocs.yaml
   ```

## 📋 详细配置说明

### 1. 项目基本信息配置

```yaml
project:
  name: "Your_SDK_Docs"           # 文档系统名称
  title: "Your SDK 文档"           # 文档标题
  description: "SDK描述"           # 文档描述
  version: "0.0.1"                # 版本号
  author: "your_name"             # 作者
  copyright: "2025, company"      # 版权信息
  language: "zh_CN"               # 语言 (zh_CN/en)
```

### 2. 仓库配置

```yaml
repository:
  name: "your-sdk-repo"           # GitHub仓库名称
  projects_dir: "../projects"      # 项目目录路径
  docs_dir: "."                   # 文档输出目录
```

### 3. 项目分类配置

根据你的项目命名规则配置分类：

```yaml
categories:
  basic:
    name: "基础篇"
    description: "基础功能示例"
    patterns:
      - "your_basic_*"            # 基础项目命名模式
      - "your_blink_led"          # 具体项目名
  
  driver:
    name: "驱动篇"
    description: "外设驱动示例"
    patterns:
      - "your_driver_*"           # 驱动项目命名模式
      - "your_usb_*"              # USB相关项目
  
  component:
    name: "组件篇"
    description: "网络组件示例"
    patterns:
      - "your_component_*"        # 组件项目命名模式
  
  protocol:
    name: "工业协议篇"
    description: "工业协议示例"
    patterns:
      - "your_ethercat_*"         # EtherCAT项目
      - "your_modbus_*"           # Modbus项目
      - "your_profinet_*"         # PROFINET项目
```

### 4. 文件复制配置

```yaml
generation:
  copy_files:
    - "README.md"                 # 英文README
    - "README_zh.md"              # 中文README
    # 可以根据需要添加其他文件
    # - "LICENSE"
    # - "CHANGELOG.md"
  
  copy_dirs:
    - "figures"                   # 图片目录
    # 可以根据需要添加其他目录
    # - "docs"
    # - "examples"
```

## 🔧 项目命名模式说明

### 模式匹配规则

- `*` : 通配符，匹配任意字符
- `?` : 匹配单个字符
- `[abc]` : 匹配字符集中的任意一个字符
- `[!abc]` : 匹配不在字符集中的任意一个字符

### 示例

```yaml
patterns:
  - "my_sdk_basic_*"             # 匹配: my_sdk_basic_led, my_sdk_basic_gpio
  - "my_sdk_driver_*"            # 匹配: my_sdk_driver_spi, my_sdk_driver_i2c
  - "my_sdk_usb_*"               # 匹配: my_sdk_usb_cdc, my_sdk_usb_msc
  - "my_sdk_ethercat_*"          # 匹配: my_sdk_ethercat_coe, my_sdk_ethercat_eoe
```

## 📁 目录结构要求

确保你的项目目录结构如下：

```
your-sdk-project/
├── projects/                     # 项目目录
│   ├── your_basic_led/          # 基础项目
│   │   ├── README.md
│   │   ├── README_zh.md
│   │   └── figures/
│   ├── your_driver_spi/         # 驱动项目
│   │   ├── README.md
│   │   ├── README_zh.md
│   │   └── figures/
│   └── your_component_mqtt/     # 组件项目
│       ├── README.md
│       ├── README_zh.md
│       └── figures/
├── source/                       # 文档系统
│   ├── config.yaml
│   ├── doc_generator.py
│   └── ...
└── ...
```

## 🧪 验证步骤

### 1. 检查依赖

```bash
python build_local.py --check
```

### 2. 测试构建

```bash
python build_local.py
```

### 3. 本地预览

```bash
python build_local.py --serve
```

### 4. 检查生成的文件

```bash
# 检查生成的目录结构
ls -la _build/html/

# 检查分类目录
ls -la basic/ driver/ component/ protocol/
```

## 🌐 部署配置

### GitHub Pages

1. **启用GitHub Pages**：
   - 进入仓库设置
   - 找到Pages设置
   - 选择"GitHub Actions"作为源

2. **推送代码**：
   ```bash
   git add .
   git commit -m "Add documentation system"
   git push origin master
   ```

3. **检查部署**：
   - 查看Actions标签页
   - 确认构建成功
   - 访问GitHub Pages URL

### Read the Docs

1. **连接仓库**：
   - 登录Read the Docs
   - 导入你的GitHub仓库

2. **配置构建**：
   - 构建配置已预置在`.readthedocs.yaml`
   - 系统会自动使用`source/conf.py`

## 🔍 常见问题

### Q: 项目没有被正确分类怎么办？

A: 检查以下几点：
1. 项目名称是否匹配patterns中的模式
2. 项目目录是否存在于`projects_dir`中
3. 运行`python doc_generator.py --stats`查看统计信息

### Q: 构建失败怎么办？

A: 检查以下几点：
1. 运行`python build_local.py --check`检查依赖
2. 确保`config.yaml`格式正确
3. 检查项目目录结构是否正确

### Q: 如何添加新的分类？

A: 在`config.yaml`中添加新的分类：

```yaml
categories:
  # 现有分类...
  new_category:
    name: "新分类"
    description: "新分类描述"
    patterns:
      - "your_new_*"
```

然后在`generation.output_structure`中添加：

```yaml
generation:
  output_structure:
    - "basic"
    - "driver"
    - "new_category"  # 添加新分类
```

### Q: 如何自定义文档主题？

A: 修改`conf.py`中的主题配置：

```python
html_theme = 'sphinx_rtd_theme'  # 或其他主题
html_theme_options = {
    'navigation_depth': 4,
    'titles_only': False,
}
```

## 📞 获取帮助

如果遇到问题，请：

1. 检查本文档的故障排除部分
2. 查看`README.md`中的详细说明
3. 运行`python build_local.py --check`检查环境
4. 查看生成的错误日志

## 🎯 最佳实践

1. **命名规范**：使用一致的命名模式，便于自动分类
2. **文档结构**：每个项目都包含README.md和README_zh.md
3. **图片管理**：将图片放在figures目录中
4. **版本控制**：定期更新版本列表
5. **测试验证**：在推送前进行本地测试 