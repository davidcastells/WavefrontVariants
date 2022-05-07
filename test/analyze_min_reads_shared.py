import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import scipy
import os

df = pd.read_csv('min_reads_shared.txt', skiprows=1)

df_shared = df[df['mem'] == 'shared']
df_global = df[df['mem'] == 'global']


x_shared = np.array(df_shared['wgs'])
y_shared = np.array(df_shared['time'])
gcups_shared = np.array(df_shared['gcups'])

x_global = np.array(df_global['wgs'])
y_global = np.array(df_global['time'])
gcups_global = np.array(df_global['gcups'])

plt.rcParams["figure.figsize"] = [5, 4]
plt.rcParams["figure.autolayout"] = True

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



def svg2pdf(file):
    cmd = 'inkscape {}.svg --export-area-drawing --export-pdf={}.pdf'.format(file, file)
    print(cmd)
    os.system(cmd)
    

svg2pdf('min_reads_shared')

plt.plot(x_shared, gcups_shared, label='shared')
plt.plot(x_global, gcups_global, label='global')


#plt.xlim(0, d)
#plt.ylim(0, 0.2)

#plt.title('Kernel execution time as a function of number of Workitems')
plt.ylabel('GCUPS')
plt.xlabel('Workgroup Size')
plt.legend()
plt.savefig('min_reads_shared_gcups.svg')
plt.show()


svg2pdf('min_reads_shared_gcups')
