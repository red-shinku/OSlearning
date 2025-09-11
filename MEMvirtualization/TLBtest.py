#!/usr/bin/env python3
import os
import ctypes
import matplotlib.pyplot as plt
import peekcpu

PAGENUMS = 4
MAX_PAGENUMS = 8192

_file = 'libTLBtest.so'
_path = os.path.join(*(os.path.split(__file__)[:-1] + (_file,)))
_mod = ctypes.cdll.LoadLibrary(_path)

tlb = _mod.tlb
tlb.argtypes = (ctypes.c_long, ctypes.c_int, ctypes.c_long, ctypes.c_long, ctypes.c_float)
tlb.restype = ctypes.c_double

get_page_size = _mod.get_page_size
get_page_size.argtypes = ()
get_page_size.restype = ctypes.c_long

PAGESIZE = get_page_size()

TRYTIMES = int(input('输入测试次数:'))
USE_MEM = int(input('输入内存用量(MB):')) * 1024 * 1024

x_usePagenums = []
y_ustime = []

cpu_mhz = peekcpu.get_cpu_mhz();

while PAGENUMS <= MAX_PAGENUMS:
    x_usePagenums.append(PAGENUMS)
    y_ustime.append( tlb(USE_MEM, PAGENUMS, PAGESIZE, TRYTIMES, cpu_mhz) )
    PAGENUMS *= 2

plt.plot(x_usePagenums, y_ustime, marker = 'o', linestyle = '-', color = 'b', label = 'tlbtest')
plt.xticks(x_usePagenums)
plt.xscale("log", base=2)
plt.xlabel('nums of using pages')
plt.ylabel('average time of each visit/ns')

plt.grid(True)
plt.show()



