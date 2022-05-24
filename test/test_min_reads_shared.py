import os
import pandas as pd

def split_multiple(s, seps):
    res = [s]
    for sep in seps:
        s, res = res, []
        for seq in s:
            res += seq.split(sep)
    return res
    
    
def test(f1, f2, mem, wgs):
    cmd = '../wavefront -pid 1 -WFO2OCL -fP ../data/{} -fT ../data/{} -wgs {} --local-tile-memory {} -v -k -1 -pp 10'.format(f1,f2, wgs, mem)
    #print(cmd)
    p = os.popen(cmd)
    res = p.read()
    lines = res.splitlines()
    for line in lines:
        #print(line)
        if (('m=' in line) and ('n=' in line)):
            part = split_multiple(line, '= ')
            m = int(part[1])
            n = int(part[3])
            #print('M=', m, 'N=', n)
            
        if (('ptxas info' in line) and ('Used' in line)):
            #print(line)
            part = line.split(':')[1]
            parts = part.split(',')
            
            registers = 0
            shmem = 0
            cmem = 0
            
            for part in parts:
                if ('registers' in part):
                    registers = int(part.strip().split(' ')[1])
                if ('smem' in part):
                    shmem = int(part.strip().split(' ')[0])
                if ('cmem' in part):
                    cmem += int(part.strip().split(' ')[0])

            
        if (('Distance=' in line) and ('Time=' in line)):
            part = split_multiple(line, '=')
            #print(part)
            distance = int(part[1].split()[0])
            time = float(part[2].split()[0])
            #print('D=', distance, 'T=', time)
        
    gcups = m*n/time/1E9
    
    print('{},{},{},{},{},{},{}'.format(mem, wgs,registers, shmem, cmem, time, gcups))
    
def testLong(f1, f2, i):
    cmd = '../wavefront -pid 1 -WFO2OCL -fP ../data/{} -fT ../data/{} -wgs {} -v -k -1 -pp 10'.format(f1,f2, i)
    print(cmd)
    os.system(cmd)
    
# CUDAlign 1.0 tests

os.system('export CUDA_CACHE_DISABLE=1'); 

print('Testing BA000035.2.fasta vs BX927147.1.fasta')
print('mem,wgs,register,shmem,cmem,time,gcups')

df = pd.read_csv('min_reads_shared.txt', skiprows=1)

for wgs in range(4,1025, 4):
    df_wgs = df[df['wgs']==wgs]
    for mem in ['shared','global']:
        df_mem = df_wgs[df_wgs['mem']==mem]
        
        if (len(df_mem) == 0):
            test('BA000035.2.fasta', 'BX927147.1.fasta', mem, wgs)



#test('NC_000898.1.fasta', 'NC_007605.1.fasta')
#testLong('NC_003064.2.fasta', 'NC_000914.1.fasta')
#test('CP000051.1.fasta', 'AE002160.2.fasta')
#testED('AE016879.1.fasta', 'AE017225.1.fasta')
#testED('NC_005027.1.fasta', 'NC_003997.3.fasta')
#test('NT_033779.4.fasta', 'NT_037436.3.fasta', -1) 
#testED('BA000046.3.fasta', 'NC_000021.7.fasta') 

# RUN IT WITH:
# nohup python3 -u test_min_reads_shared.py > new_min_reads_shared.txt &



