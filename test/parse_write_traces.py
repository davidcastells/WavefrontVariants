import pandas as pd
import numpy as np
import sys
import matplotlib.pyplot as plt


def parseSizes(line):
    part = line.split(' ')
    m = int(part[0].split('=')[1])
    n = int(part[1].split('=')[1])
    return m,n
    
def parseWrite(line, w):
    pos = line.find('(')
    pos2 = line.find(')', pos)
    str = line[pos+1:pos2].split(',')
    r0 = int(str[0])
    tile = int(str[1])
    pos = line.find('GWR(')
    pos2 = line.find(')', pos)
    str = line[pos+4:pos2]
    part = str.split(',')
    r = int(part[0])
    d = int(part[1])
    pos = line.find('e: ', pos2)
    if (pos > 0):
        e = int(line[pos+3:])
    else:
        e = -1
        
    return r0,tile, r,d,e

for line in sys.stdin:
    
    if (('m=' in line) and ('n=' in line)):
        m, n = parseSizes(line)
        x = max(m,n)
        y = 2*x+1
        w = np.zeros((y,x))
        t = np.zeros((y,x))
        
    if ('GWR' in line):
        r0,tile, r,d,v = parseWrite(line, w)
        print('parser', r0, tile, r, d, v, 'w', d+x, r)
        if (d >= -x and d <= x):
            w[d+x,r] = v
            t[d+x,r] = (10+(r0*21)+tile*11)%256
        
    #print(line, end='')
    
print(w)

plt.imshow(w)
plt.show()
plt.imshow(t)
plt.show()
