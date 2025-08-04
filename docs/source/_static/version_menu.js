// Read the Docs 风格的版本菜单功能
(function() {
    'use strict';
    
    // 版本配置 - 将从 .github/versions.list 文件获取
    let VERSION_CONFIG = {
        current: 'master',
        versions: {
            'master': '最新版本',
            'v1.0': 'v1.0'
        }
    };
    
    // 从 .github/versions.list 文件获取版本信息
    async function fetchVersionInfo() {
        try {
            // 尝试读取版本配置文件（仅在本地开发时可用）
            const response = await fetch('/.github/versions.list');
            if (!response.ok) {
                throw new Error('版本文件不可用');
            }
            
            const versionsText = await response.text();
            
            // 解析版本配置
            const versions = {};
            const lines = versionsText.split('\n').filter(line => line.trim() && !line.startsWith('#'));
            
            lines.forEach(line => {
                const [version, comment] = line.split('#').map(s => s.trim());
                if (version) {
                    // 根据版本名称设置显示名称
                    let displayName;
                    if (version === 'master') {
                        displayName = '最新版本';
                    } else if (version.startsWith('v')) {
                        displayName = version;
                    } else {
                        displayName = version;
                    }
                    versions[version] = displayName;
                }
            });
            
            // 确定当前版本
            const currentPath = window.location.pathname;
            let currentVersion = 'master'; // 默认版本
            
            // 从URL路径判断当前版本
            if (currentPath.includes('/latest/')) {
                currentVersion = 'master';
            } else if (currentPath.includes('/v1.0/')) {
                currentVersion = 'v1.0';
            } else {
                // 尝试从路径中提取版本号
                const versionMatch = currentPath.match(/\/(v\d+\.\d+)\//);
                if (versionMatch) {
                    currentVersion = versionMatch[1];
                }
            }
            
            VERSION_CONFIG = {
                current: currentVersion,
                versions: versions
            };
            
            console.log('版本信息已更新:', VERSION_CONFIG);
            
            // 重新创建版本菜单
            createVersionMenu();
            
        } catch (error) {
            console.warn('无法获取版本信息，使用默认配置:', error);
            // 使用默认配置，基于当前URL路径确定版本
            const currentPath = window.location.pathname;
            let currentVersion = 'master';
            
            if (currentPath.includes('/latest/')) {
                currentVersion = 'master';
            } else if (currentPath.includes('/v1.0/')) {
                currentVersion = 'v1.0';
            }
            
            VERSION_CONFIG = {
                current: currentVersion,
                versions: {
                    'master': '最新版本',
                    'v1.0': 'v1.0'
                }
            };
            
            console.log('使用默认版本配置:', VERSION_CONFIG);
            createVersionMenu();
        }
    }
    
    // 创建版本菜单
    function createVersionMenu() {
        // 查找侧边栏
        const sidebar = document.querySelector('.wy-nav-side');
        if (!sidebar) {
            console.warn('找不到侧边栏元素');
            return;
        }
        
        // 查找项目标题链接
        const projectTitle = sidebar.querySelector('a.icon.icon-home');
        if (!projectTitle) {
            console.warn('找不到项目标题元素');
            return;
        }
        
        // 检查是否已经存在版本菜单
        if (sidebar.querySelector('.rtd-version-menu')) {
            return;
        }
        
        // 计算最长版本名称的宽度
        const versionNames = Object.values(VERSION_CONFIG.versions);
        const maxLength = Math.max(...versionNames.map(name => name.length));
        const minWidth = Math.max(80, maxLength * 5 + 20);
        
        // 创建版本菜单容器
        const versionMenu = document.createElement('div');
        versionMenu.className = 'rtd-version-menu';
        versionMenu.style.minWidth = minWidth + 'px';
        versionMenu.innerHTML = `
            <button class="rtd-version-menu__button" type="button" aria-haspopup="true" aria-expanded="false">
                <span class="rtd-version-menu__current">${VERSION_CONFIG.versions[VERSION_CONFIG.current]}</span>
            </button>
            <div class="rtd-version-menu__dropdown" role="menu">
                ${Object.entries(VERSION_CONFIG.versions).map(([version, name]) => `
                    <a class="rtd-version-menu__item ${version === VERSION_CONFIG.current ? 'active' : ''}" 
                       href="#" data-version="${version}" role="menuitem">
                        ${name}
                    </a>
                `).join('')}
            </div>
        `;
        
        // 插入到项目标题下方
        projectTitle.parentNode.insertBefore(versionMenu, projectTitle.nextSibling);
        
        // 添加事件监听器
        setupVersionMenuEvents(versionMenu);
        
        console.log('版本菜单已创建，位置：项目标题下方');
    }
    
    // 设置版本菜单事件
    function setupVersionMenuEvents(versionMenu) {
        const button = versionMenu.querySelector('.rtd-version-menu__button');
        const dropdown = versionMenu.querySelector('.rtd-version-menu__dropdown');
        
        if (!button || !dropdown) {
            console.warn('找不到版本菜单按钮或下拉菜单');
            return;
        }
        
        // 按钮点击事件
        button.addEventListener('click', function(e) {
            e.preventDefault();
            e.stopPropagation();
            
            const isExpanded = this.getAttribute('aria-expanded') === 'true';
            this.setAttribute('aria-expanded', !isExpanded);
            dropdown.classList.toggle('show');
            
            console.log('版本菜单切换:', !isExpanded);
        });
        
        // 版本选择事件
        dropdown.addEventListener('click', function(e) {
            if (e.target.classList.contains('rtd-version-menu__item')) {
                e.preventDefault();
                e.stopPropagation();
                
                const version = e.target.getAttribute('data-version');
                const versionName = e.target.textContent.trim();
                
                // 更新当前版本显示
                const currentSpan = button.querySelector('.rtd-version-menu__current');
                if (currentSpan) {
                    currentSpan.textContent = versionName;
                }
                
                // 更新活动状态
                dropdown.querySelectorAll('.rtd-version-menu__item').forEach(item => {
                    item.classList.remove('active');
                });
                e.target.classList.add('active');
                
                // 关闭下拉菜单
                button.setAttribute('aria-expanded', 'false');
                dropdown.classList.remove('show');
                
                // 版本切换逻辑
                handleVersionChange(version, versionName);
            }
        });
        
        // 点击外部关闭下拉菜单
        document.addEventListener('click', function(e) {
            if (!versionMenu.contains(e.target)) {
                button.setAttribute('aria-expanded', 'false');
                dropdown.classList.remove('show');
            }
        });
        
        // 键盘导航支持
        button.addEventListener('keydown', function(e) {
            if (e.key === 'Enter' || e.key === ' ') {
                e.preventDefault();
                this.click();
            }
        });
        
        dropdown.addEventListener('keydown', function(e) {
            const items = Array.from(this.querySelectorAll('.rtd-version-menu__item'));
            const currentIndex = items.indexOf(document.activeElement);
            
            switch (e.key) {
                case 'ArrowDown':
                    e.preventDefault();
                    const nextIndex = (currentIndex + 1) % items.length;
                    items[nextIndex].focus();
                    break;
                case 'ArrowUp':
                    e.preventDefault();
                    const prevIndex = (currentIndex - 1 + items.length) % items.length;
                    items[prevIndex].focus();
                    break;
                case 'Escape':
                    e.preventDefault();
                    button.setAttribute('aria-expanded', 'false');
                    dropdown.classList.remove('show');
                    button.focus();
                    break;
            }
        });
    }
    
    // 处理版本切换
    function handleVersionChange(version, versionName) {
        console.log('切换到版本:', version, versionName);
        
        // 获取当前页面的相对路径
        const currentPath = window.location.pathname;
        const baseUrl = window.location.origin;
        
        // 检查是否为本地开发环境
        const isLocalDev = window.location.hostname === 'localhost' || 
                          window.location.hostname === '127.0.0.1' || 
                          window.location.protocol === 'file:';
        
        // 构建版本切换URL
        let newUrl;
        
        if (isLocalDev) {
            // 本地开发环境：构建正确的本地文件路径
            // 获取当前文件的完整路径
            const fullPath = window.location.href;
            const currentPath = window.location.pathname;
            
            // 检查当前是否在某个版本目录中
            if (currentPath.includes('/latest/') || currentPath.includes('/v1.0/')) {
                // 替换版本目录
                let newPath;
                if (currentPath.includes('/latest/')) {
                    newPath = currentPath.replace('/latest/', `/${version === 'master' ? 'latest' : version}/`);
                } else if (currentPath.includes('/v1.0/')) {
                    newPath = currentPath.replace('/v1.0/', `/${version === 'master' ? 'latest' : version}/`);
                }
                newUrl = `file://${newPath}`;
            } else {
                // 如果不在版本目录中，构建到版本目录的路径
                // 获取当前文件的完整路径
                const filePath = fullPath.replace('file://', '');
                
                // 提取目录路径和文件名
                const lastSlashIndex = filePath.lastIndexOf('/');
                const directory = filePath.substring(0, lastSlashIndex);
                const fileName = filePath.substring(lastSlashIndex + 1);
                
                // 构建新版本的路径
                const versionDir = version === 'master' ? 'latest' : version;
                newUrl = `file://${directory}/${versionDir}/${fileName}`;
            }
        } else {
            // 生产环境：从当前路径中提取相对路径部分
            let relativePath = '';
            
            // 检查当前是否在某个版本目录中
            if (currentPath.includes('/latest/')) {
                // 提取 /latest/ 后面的路径部分
                const parts = currentPath.split('/latest/');
                if (parts.length > 1) {
                    relativePath = parts[1];
                }
            } else if (currentPath.includes('/v1.0/')) {
                // 提取 /v1.0/ 后面的路径部分
                const parts = currentPath.split('/v1.0/');
                if (parts.length > 1) {
                    relativePath = parts[1];
                }
            } else {
                // 尝试匹配任何版本路径
                const versionMatch = currentPath.match(/\/([^\/]+)\/(.*)/);
                if (versionMatch) {
                    relativePath = versionMatch[2];
                }
            }
            
            // 构建新版本的URL - 使用正确的GitHub Pages路径
            // 从当前URL中提取仓库名称
            const pathParts = currentPath.split('/');
            let repoName = '';
            
            // 查找仓库名称（通常在 /username/repo-name/ 结构中）
            for (let i = 0; i < pathParts.length - 1; i++) {
                if (pathParts[i] && pathParts[i + 1] && 
                    (pathParts[i + 1] === 'latest' || pathParts[i + 1] === 'v1.0')) {
                    repoName = pathParts[i];
                    break;
                }
            }
            
            // 如果找不到仓库名称，使用默认值
            if (!repoName) {
                repoName = 'sdk-bsp-rzn2l-etherkit';
            }
            
            if (version === 'master') {
                newUrl = `${baseUrl}/${repoName}/latest/${relativePath}`;
            } else {
                newUrl = `${baseUrl}/${repoName}/${version}/${relativePath}`;
            }
            
            // 确保URL以 / 结尾（如果是根路径）
            if (relativePath === '' || relativePath === '/') {
                newUrl = newUrl.replace(/\/$/, '') + '/';
            }
        }
        
        console.log('当前路径:', currentPath);
        console.log('是否为本地开发:', isLocalDev);
        console.log('跳转到:', newUrl);
        
        // 跳转到新版本
        window.location.href = newUrl;
        
        // 发送自定义事件
        const event = new CustomEvent('version-changed', {
            detail: {
                version: version,
                versionName: versionName,
                newUrl: newUrl
            }
        });
        document.dispatchEvent(event);
    }
    
    // 初始化版本菜单
    function init() {
        console.log('初始化版本菜单...');
        
        // 等待 DOM 加载完成
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', fetchVersionInfo);
        } else {
            fetchVersionInfo();
        }
        
        // 如果侧边栏是动态加载的，等待一下再尝试创建
        setTimeout(() => {
            if (!document.querySelector('.rtd-version-menu')) {
                createVersionMenu();
            }
        }, 1000);
        
        setTimeout(() => {
            if (!document.querySelector('.rtd-version-menu')) {
                createVersionMenu();
            }
        }, 2000);
        
        setTimeout(() => {
            if (!document.querySelector('.rtd-version-menu')) {
                createVersionMenu();
            }
        }, 3000);
    }
    
    // 启动
    init();
})(); 