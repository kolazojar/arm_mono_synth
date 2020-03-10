#!/bin/python
# @file lut.py
# @author Matt Krol

import math
import sys

def gen_lut(name, dtype, bits, func):
    lut_size = 1 << bits
    tab_size = 4
    indent = 0
    sys.stdout.write(' '*tab_size*indent + '{} {}[] = {{\n'.format(dtype, name))
    for i in range (0, lut_size):
        if i == 0:
            sys.stdout.write(' '*tab_size*(indent+1))
        elif i != 0 and i % 8 == 0:
            sys.stdout.write('\n' + ' '*tab_size*(indent+1))
        y = func(i)
        sys.stdout.write('%s, ' % (y))
    sys.stdout.write('\n' + ' '*tab_size*indent + '};\n')

def sin_func(t, bits, scale):
    y_shift = 0
    x_shift = 0.5
    delta = 0.5*math.pi/(1 << bits)
    y = scale*(math.sin((t + x_shift)*delta) + y_shift)
    return int(y + 0.5)

def main():
    bits = 8
    scale = (1 << 31) - 1;
    sys.stdout.write('/* Begin auto-generated lookup table code from lut.py */\n\n#ifndef LUT_H\n#define LUT_H\n\n#define SIN_LUT_SCALE ((double){})\n\n'.format(1/scale))
    gen_lut('SIN_LUT', 'const int32_t', bits, lambda t: sin_func(t, bits, scale))
    sys.stdout.write('\n#endif\n\n/* End auto-generated lookup table code from lut.py */')

if __name__ == '__main__':
    main()
