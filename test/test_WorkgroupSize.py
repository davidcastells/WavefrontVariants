import os

def split_multiple(s, seps):
    res = [s]
    for sep in seps:
        s, res = res, []
        for seq in s:
            res += seq.split(sep)
    return res

def testWFO(f1, f2, i):
    cmd = '../wavefront -WFO2OCL -fP ../data/{} -fT ../data/{} -wgs {} -v -k -1'.format(f1,f2, i)
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
            
        if (('Distance=' in line) and ('Time=' in line)):
            part = split_multiple(line, '=')
            distance = int(part[1].split()[0])
            time = float(part[2].split()[0])
            #print('D=', distance, 'T=', time)
        
    gcups = m*n/time/1E9
    
    print('{},{},{},{},{},{},{},{}'.format(i, f1,m,f2,n,distance,time, gcups))
    
def testWFDD(f1, f2, i):
    cmd = '../wavefront -WFDD2OCL -fP ../data/{} -fT ../data/{} -wgs {} -v -k -1'.format(f1,f2, i)
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
            
        if (('Distance=' in line) and ('Time=' in line)):
            part = split_multiple(line, '= ')
            distance = int(part[22])
            time = float(part[24])
            #print('D=', distance, 'T=', time)
        
    gcups = m*n/time/1E9
    
    print('{},{},{},{},{},{},{},{}'.format(i, f1,m,f2,n,distance,time, gcups))
    
def testLong(f1, f2):
    cmd = '../wavefront -WFDD2OCL -fP ../data/{} -fT ../data/{} -wgs 32 -v -k -1'.format(f1,f2)
    print(cmd)
    os.system(cmd)
    
# CUDAlign 1.0 tests

for i in range(1,128):
    #testWFDD('NC_000898.1.fasta', 'NC_007605.1.fasta', i)
    testWFO('NC_000898.1.fasta', 'NC_007605.1.fasta', i)

#test('NC_003064.2.fasta', 'NC_000914.1.fasta')
#test('CP000051.1.fasta', 'AE002160.2.fasta')
#test('BA000035.2.fasta', 'BX927147.1.fasta')
#test('AE016879.1.fasta', 'AE017225.1.fasta')
#test('NC_005027.1.fasta', 'NC_003997.3.fasta')
#testLong('NT_033779.4.fasta', 'NT_037436.3.fasta') 
#testLong('BA000046.3.fasta', 'NC_000021.7.fasta') 





