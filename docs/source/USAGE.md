# EtherKit SDK 文档构建系统使用说明

## 🚀 快速开始

### 基本构建命令

```bash
# 从项目根目录构建
python build_docs.py

# 从docs/source目录构建
cd docs/source
python build_local.py
```

### 多版本构建

```bash
# 构建所有版本（master和v1.0）
python build_local.py --all-versions

# 从项目根目录构建所有版本
python build_docs.py --all-versions
```

## 📋 可用参数

### build_local.py 参数

- `--check`: 检查依赖是否安装
- `--serve`: 启动本地服务器预览
- `--port <端口号>`: 指定服务器端口（默认8000）
- `--clean`: 清理构建目录
- `--check-branch`: 检查分支版本映射
- `--all-versions`: 构建所有版本

### 示例用法

```bash
# 检查依赖
python build_local.py --check

# 构建并启动服务器
python build_local.py --serve --port 8080

# 清理并重新构建
python build_local.py --clean

# 构建所有版本
python build_local.py --all-versions
```

## 📁 构建输出结构

### 单版本构建
```
docs/source/_build/html/
├── index.html          # 重定向页面
├── basic/             # 基础篇文档
├── driver/            # 驱动篇文档
├── component/         # 组件篇文档
├── protocol/          # 工业协议篇文档
└── start/             # 快速上手篇文档
```

### 多版本构建
```
docs/source/_build/html/
├── index.html          # 版本选择页面
├── latest/            # master版本文档
└── v1.0/              # v1.0版本文档
```

## ⚙️ 版本配置

版本配置在 `.github/versions.list` 文件中：

```
master    # 默认版本（生成到 latest/）
v1.0    # 对应 release/v1.0 分支
```

## 🔧 故障排除

### 常见问题

1. **路径错误**: 确保在正确的目录下运行命令
2. **依赖缺失**: 运行 `pip install -r requirements.txt`
3. **Git分支问题**: 确保所有配置的分支都存在
4. **权限问题**: 确保有足够的文件读写权限

### 调试命令

```bash
# 检查版本配置
python build_local.py --check-branch

# 查看详细输出
python build_local.py --verbose

# 仅检查项目扫描
python build_local.py --stats
```

## 📝 注意事项

1. **多版本构建**需要Git仓库支持，确保所有配置的分支都存在
2. **本地构建**会自动切换到对应分支进行构建
3. **GitHub Actions**环境下会自动检测并构建所有版本
4. 构建完成后会自动恢复到原始分支

## 🎯 最佳实践

1. 在修改文档前先运行 `--check` 确保环境正常
2. 使用 `--serve` 参数预览文档效果
3. 定期运行 `--all-versions` 确保多版本文档同步
4. 使用 `--clean` 清理旧的构建文件 