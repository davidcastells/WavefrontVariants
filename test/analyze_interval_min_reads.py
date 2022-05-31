import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import scipy
import os

plt.rcParams["figure.figsize"] = [4, 3]
plt.rcParams["figure.autolayout"] = True


x = [1,2,4,8,16,32,64,128,256,512,1024]
y0 = [0.64134364,0.794330469,0.897350906,0.94825493,0.977684095,0.989846754,0.993246898,0.998193143,1,0.999455786,0.999220634]
y1 = [0.863863774,0.931744343,0.96513649,0.982656877,0.991849005,0.996477835,0.998501804,0.999514227,1,0.999552188,0.998502685]
y2 = [0.949590813,0.974987761,0.988485436,0.99402443,0.997080587,0.998269149,0.999286329,0.999802112,1,0.999889496,0.999225706]


plt.plot(x, y0, 'g:', label='162k')
plt.plot(x, y1, 'g--', label='1M')
plt.plot(x, y2, 'g', label='3M')


plt.ylabel('Normalized Performance')
plt.ylim(0, 1.2)
plt.xscale('log')
plt.xlim(0, 1000)
plt.xlabel('Columns')
plt.legend()
plt.savefig('interval_min_reads_norm.svg')
plt.show()


def svg2pdf(file):
    cmd = 'inkscape {}.svg --export-area-drawing --export-pdf={}.pdf'.format(file, file)
    print(cmd)
    os.system(cmd)

def pdfcrop(file):
    cmd = '/tmp/pdfcrop.pl {}.pdf'.format(file)
    print(cmd)
    os.system(cmd)

svg2pdf('interval_min_reads_norm')
pdfcrop('interval_min_reads_norm')

