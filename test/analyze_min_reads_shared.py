import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import scipy
import os

df = pd.read_csv('min_reads_shared.txt', skiprows=1)

df.sort_values(['wgs'], inplace=True)

df_shared = df[df['mem'] == 'shared']
df_global = df[df['mem'] == 'global']


x_shared = np.array(df_shared['wgs'])
y_shared = np.array(df_shared['time'])
gcups_shared = np.array(df_shared['gcups'])

x_global = np.array(df_global['wgs'])
y_global = np.array(df_global['time'])
gcups_global = np.array(df_global['gcups'])

print('MAX Global:', np.max(gcups_global), 'workitems:', x_global[np.argmax(gcups_global)])
print('MAX Shared:', np.max(gcups_shared), 'workitems:', x_shared[np.argmax(gcups_shared)])

if (False):
    plt.plot(x_shared, y_shared, label='shared')
    plt.plot(x_global, y_global, label='global')


    #plt.xlim(0, d)
    #plt.ylim(0, 0.2)

    #plt.title('Kernel execution time as a function of number of Workitems')
    plt.ylabel('Execution time (s)')
    plt.xlabel('Workgroup Size')
    plt.legend()
    plt.savefig('min_reads_shared.svg')
    plt.show()


    cmd = 'inkscape min_reads_shared.svg --export-area-drawing --export-pdf=min_reads_shared.pdf'
    print(cmd)
    os.system(cmd)

plt.rcParams["figure.figsize"] = [4, 3]
plt.rcParams["figure.autolayout"] = True

plt.plot(x_global, gcups_global, label='global')
plt.plot(x_shared, gcups_shared, label='shared')


#plt.xlim(0, d)
#plt.ylim(0, 0.2)

#plt.title('Kernel execution time as a function of number of Workitems')
plt.ylabel('GCUPS')
plt.xlabel('Workgroup Size')
plt.xlim(0, 1024)
plt.ylim(0, 300)
plt.legend()
plt.savefig('min_reads_shared_gcups.svg')
plt.show()


def svg2pdf(file):
    cmd = 'inkscape {}.svg --export-area-drawing --export-pdf={}.pdf'.format(file, file)
    print(cmd)
    os.system(cmd)
    
def pdfcrop(file):
    cmd = '/tmp/pdfcrop.pl {}.pdf'.format(file)    
    print(cmd)
    os.system(cmd)
    
svg2pdf('min_reads_shared_gcups')
pdfcrop('min_reads_shared_gcups')
