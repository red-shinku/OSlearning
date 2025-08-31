#!/usr/bin/env python3
import os
import ctypes
import csv
import peekcpu

_file = 'libTLBtest.so'
_path = os.path.join(*(os.path.split(__file__)[:-1] + (_file,)))
_mod = ctypes.cdll.LoadLibrary(_path)

tlb = _mod.tlb
tlb.argtypes = (ctypes.c_int, ctypes.c_long, ctypes.c_float)
tlb.restype = ctypes.c_double

TRYTIMES = int(input('输入测试次数'))
PAGENUMS = 1

x_usePagenums = []
y_ustime = []

cpu_mhz = peekcpu.get_cpu_mhz()

while PAGENUMS <= 8192:
    x_usePagenums.append(PAGENUMS)
    y_ustime.append( tlb(PAGENUMS, TRYTIMES, cpu_mhz) )
    PAGENUMS *= 2

with open('result.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['使用页数量','平均每次访问用时/ns'])
    writer.writerows( zip(x_usePagenums, y_ustime) )