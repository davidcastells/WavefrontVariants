import os
import pandas as pd

def split_multiple(s, seps):
    res = [s]
    for sep in seps:
        s, res = res, []
        for seq in s:
            res += seq.split(sep)
    return res
    
    
def test(f1, f2, tl, mem, wgs):
    cmd = '../wavefront -pid 1 -WFO2OCL -fP ../data/{} -fT ../data/{} -wgs {} -v -k -1 -tl {} --local-tile-memory {} -pp 10'.format(f1,f2, wgs, tl, mem)
    #print(cmd)
    
    p = os.popen(cmd)
    res = p.read()
    lines = res.splitlines()
    
    #import subprocess
    #p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    #doRun = True
    #while (doRun):
    #    line = p.stdout.readline()
        
    #    if (len(line) == 0):
            
    #        doRun = False;
    #        continue;
            
    #    line = line.decode('utf8')
    
    for line in lines:
        #print('[LINE:]', line)
        
        if (('CL_OUT_OF_RESOURCES' in line) or ('uses too much shared data' in line) or ('error:' in line)):
            print('{},{},{},{},{},{},{},{}'.format(mem,tl,wgs, 0, 0, 0, 0, 0))
            return
            
        
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
        
    #print('[INFO] Finished')
    #retval = p.wait()
    
    gcups = m*n/time/1E9
    
    print('{},{},{},{},{},{},{},{}'.format(mem,tl,wgs,registers, shmem, cmem, time, gcups))
    
def testLong(f1, f2, i, mem):
    cmd = '../wavefront -pid 1 -WFO2OCL -fP ../data/{} -fT ../data/{} -wgs 96 -v -k -1 -tl {} --local-tile-memory {} -pp 10'.format(f1,f2, i, mem)
    print(cmd)
    os.system(cmd)
    
# CUDAlign 1.0 tests

print('Testing BA000035.2.fasta vs BX927147.1.fasta')
print('mem,tl,wgs,register,shmem,cmem,time,gcups')

df = pd.read_csv('local_tiles_column.txt', skiprows=0)

df.sort_values(['wgs'], inplace=True)

for tl in range(1, 40):
    df_tl = df[df['tl'] == tl]
    
    #print('Tile len' , tl)
    for wgs in range(16,1025,16): #[16,24, 32, 40, 48, 64, 96, 128, 144, 160, 192, 208, 224, 256, 384, 448, 512, 768, 1024]: # 
        df_wgs = df_tl[df_tl['wgs']==wgs]
        
        #print('Tile len' , wgs)
            
        for mem in [ 'global', 'shared', 'register' ]:   
        
            #print('Mem:', mem)
            
            df_mem = df_wgs[df_wgs['mem'] == mem]
            
                    
            if (len(df_mem) == 0):
                test('BA000035.2.fasta', 'BX927147.1.fasta', tl, mem, wgs)

            #else:
            #    print('tl: {} wgs: {} mem: {} already computed '.format(tl, wgs, mem))


#test('NC_000898.1.fasta', 'NC_007605.1.fasta')
#testLong('NC_003064.2.fasta', 'NC_000914.1.fasta')
#test('CP000051.1.fasta', 'AE002160.2.fasta')
#testED('AE016879.1.fasta', 'AE017225.1.fasta')
#testED('NC_005027.1.fasta', 'NC_003997.3.fasta')
#test('NT_033779.4.fasta', 'NT_037436.3.fasta', -1) 
#testED('BA000046.3.fasta', 'NC_000021.7.fasta') 

# LAUNCH WITH : nohup python3 -u test_local_tiles_column.py > new_local_tiles_column.txt 2> /dev/null &

def sortData():
    import pandas as pd
    df = pd.read_csv('local_tiles_column.txt', skiprows=0)
    df2 = df.sort_values(['tl','mem','wgs'])
    df3 = df2.drop_duplicates(['tl','mem','wgs'])
    df3.to_csv('local_tiles_column.txt', index=False)
