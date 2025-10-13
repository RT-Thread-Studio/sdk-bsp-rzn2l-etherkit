# for module compiling
import os
Import('RTT_ROOT')
Import('rtconfig')
from building import *
from gcc import *

cwd = GetCurrentDir()
src = []
CPPPATH = [cwd]
group = []
list = os.listdir(cwd)

CPPDEFINES = ['_RZN_ORDINAL=1']

if rtconfig.PLATFORM in ['iccarm'] + GetGCCLikePLATFORM():
    if rtconfig.PLATFORM == 'iccarm' or GetOption('target') != 'mdk5':
        CPPPATH = [cwd + '/src']
        src = Glob('./src/*.c')
        group = DefineGroup('Applications', src, depend = [''], CPPPATH = CPPPATH, CPPDEFINES=CPPDEFINES)

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        group = group + SConscript(os.path.join(d, 'SConscript'))

Return('group')
