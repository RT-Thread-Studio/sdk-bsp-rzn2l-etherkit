
from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c') + Glob('*.cpp')
path    = [cwd]

group = DefineGroup('at24cxx', src, depend = ['PKG_USING_AT24CXX'], CPPPATH = path)

Return('group')
