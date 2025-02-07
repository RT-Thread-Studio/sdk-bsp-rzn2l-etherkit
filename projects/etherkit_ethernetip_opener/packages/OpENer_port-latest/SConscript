from building import *
import os

cwd     = GetCurrentDir()
CPPPATH = [cwd + '/../OpENer-latest/source/src']
CPPPATH += [cwd + '/../OpENer-latest/source/src/cip']
CPPPATH += [cwd + '/../OpENer-latest/source/src/enet_encap']
CPPPATH += [cwd + '/../OpENer-latest/source/src/utils']
CPPPATH += [cwd + '/../OpENer-latest/source/src/ports']

src = Glob('../OpENer-latest/source/src/cip/*.c')
src += Glob('../OpENer-latest/source/src/enet_encap/*.c')
src += Glob('../OpENer-latest/source/src/utils/*.c')
src += Glob('../OpENer-latest/source/src/ports/*.c')

CPPDEFINES = ['OPENER_CONSUMED_DATA_HAS_RUN_IDLE_HEADER']
CPPDEFINES += ['RESTRICT=__restrict']

if GetDepend('PKG_OPENER_WITH_TRACES'):
    CPPDEFINES += ['OPENER_WITH_TRACES']
    CPPDEFINES += ['OPENER_TRACE_LEVEL=' + str(GetConfigValue('PKG_OPENER_TRACE_LEVEL'))]

CPPDEFINES += ['PC_OPENER_ETHERNET_BUFFER_SIZE=' + str(GetConfigValue('PKG_OPENER_ETHERNET_BUFFER_SIZE'))]

group_src = DefineGroup('OpENer/src', src, depend = ['PKG_USING_OPENER'], CPPPATH = CPPPATH, CPPDEFINES = CPPDEFINES)


CPPPATH = [cwd]
CPPPATH += [cwd + '/sample_application']

src = Glob('*.c')
src += Glob('sample_application/*.c')

group_port = DefineGroup('OpENer/port', src, depend = ['PKG_USING_OPENER_PORT'], CPPPATH = CPPPATH)

group = group_src + group_port

Return('group')
