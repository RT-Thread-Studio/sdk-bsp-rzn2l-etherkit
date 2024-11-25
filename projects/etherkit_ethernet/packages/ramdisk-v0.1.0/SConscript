from building import *
Import('rtconfig')

src   = []
cwd   = GetCurrentDir()

if GetDepend('PKG_USING_RAMDISK'):
    src += Glob('src/*.c')

# add include path.
path  = [cwd + '/inc']

# add src and include to group.
group = DefineGroup('ramdisk', src, depend = ['PKG_USING_RAMDISK'], CPPPATH = path)

Return('group')
