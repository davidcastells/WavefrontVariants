import os

def split_multiple(s, seps):
    res = [s]
    for sep in seps:
        s, res = res, []
        for seq in s:
            res += seq.split(sep)
    return res
    
def testED(f1, f2):
    cmd = '../wavefront -ED -fP ../data/{} -fT ../data/{} -wgs 32 -v -k -1'.format(f1,f2)
    print(cmd)
    os.system(cmd)
    
def test(f1, f2):
    cmd = '../wavefront -WFDD2OCL -fP ../data/{} -fT ../data/{} -wgs 32 -v -k -1'.format(f1,f2)
    print(cmd)
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
    
    print('{},{},{},{},{},{},{}'.format(f1,m,f2,n,distance,time, gcups))
    
def testLong(f1, f2):
    cmd = '../wavefront -WFDD2OCL -fP ../data/{} -fT ../data/{} -wgs 32 -v -k -1'.format(f1,f2)
    print(cmd)
    os.system(cmd)
    
# CUDAlign 1.0 tests

#test('NC_000898.1.fasta', 'NC_007605.1.fasta')
#test('NC_003064.2.fasta', 'NC_000914.1.fasta')
#test('CP000051.1.fasta', 'AE002160.2.fasta')
testED('BA000035.2.fasta', 'BX927147.1.fasta')
testED('AE016879.1.fasta', 'AE017225.1.fasta')
testED('NC_005027.1.fasta', 'NC_003997.3.fasta')
testED('NT_033779.4.fasta', 'NT_037436.3.fasta') 
testED('BA000046.3.fasta', 'NC_000021.7.fasta') 





