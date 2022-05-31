import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import scipy
import os

df = pd.read_csv('min_reads.txt', skiprows=1)

x = np.array(df['wgs'])
y = np.array(df['time'])
z = np.array(df['gcups'])
    
    #fmt = map_format[lbl]
    #plt.plot(x, z, fmt, label=lbl )
    
    
plt.rcParams["figure.figsize"] = [4, 3]
plt.rcParams["figure.autolayout"] = True

print('Max GCUPS:', np.max(z), 'for', x[np.argmax(z)], 'workitems', 'execution time:', y[np.argmax(z)])
plt.plot(x, z, 'g')
    

    
#plt.title('Global')
plt.ylabel('GCUPS')
plt.ylim(0, 300)
plt.xlim(0, 1024)
plt.xlabel('Workgroup Size')
#plt.legend()
plt.savefig('min_reads_gcups.svg')
plt.show()


def svg2pdf(file):
    cmd = 'inkscape {}.svg --export-area-drawing --export-pdf={}.pdf'.format(file, file)
    print(cmd)
    os.system(cmd)

def pdfcrop(file):
    cmd = '/tmp/pdfcrop.pl {}.pdf'.format(file)
    print(cmd)
    os.system(cmd)

svg2pdf('min_reads_gcups')
pdfcrop('min_reads_gcups')

