
def normd(d):
    s = '{}'.format(d)
    return s.replace('-', 'm')
    
def createStruct(tl):
    print('typedef struct')
    print('{')
    
    for r in range(tl):
        for d in range(-r, r+1):
            print('   long r{}_d{};'.format(r, normd(d)))
            
    for r in range(tl):
        rr = tl - r -1
        pr = tl + r
        for d in range(-rr, rr+1):
            print('   long r{}_d{};'.format(pr, normd(d)))

    print('} LOCAL_TILE;')
    print('#define LOCAL_TILE_PTR      LOCAL_TILE*')
    print('')
    
    
def createWriteIf(tl):
    print('void inline __attribute__((always_inline))')
    print('writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)')
    print('{')
    
    slink1 = ''
    
    for r in range(tl):
        rn = tl-r-1
        rp = tl+r
        print('   {} if (r == {})'.format(slink1, rn))
        print('   {')
        slink2 = ''
        for d in range(-rn, rn+1):
            print('      {} if (d == {}) localW->r{}_d{} = v; '.format(slink2, d, rn, normd(d)));
            slink2 = 'else'
        print('   }')
        
        print('   else if (r == {})'.format(rp))
        print('   {')
        slink2 = ''
        for d in range(-rn, rn+1):
            print('      {} if (d == {}) localW->r{}_d{} = v; '.format(slink2, d, rp, normd(d)));
            slink2 = 'else'
        print('   }')
        
        slink1 = 'else'
    print('}')
    print('')
    
    
def createReadIf(tl):
        
    print('long inline __attribute__((always_inline))')
    print('readLocalW(LOCAL_TILE_PTR localW, int d, int r)')
    print('{')
    print('   long v; ')
    
    slink1 = ''
    for r in range(tl):
        rn = tl-r-1
        rp = tl+r
        print('   {} if (r == {})'.format(slink1, rn))
        print('   {')
        slink2 = ''
        for d in range(-rn, rn+1):
            print('      {} if (d == {}) v = localW->r{}_d{}; '.format(slink2, d, rn, normd(d)));
            slink2 = 'else'
        print('   }')
        
        print('   else if (r == {})'.format(rp))
        print('   {')
        slink2 = ''
        for d in range(-rn, rn+1):
            print('      {} if (d == {}) v = localW->r{}_d{}; '.format(slink2, d, rp, normd(d)));
            slink2 = 'else'
        print('   }')
        
        slink1 = 'else'
        
    print('   return v;')
    print('}')
    print('')
    
def createWriteSwitch(tl):
    print('void inline __attribute__((always_inline))')
    print('writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)')
    print('{')
    

    print('   switch(r)')        
    print('   {')
    for r in range(tl):
        rn = tl-r-1
        rp = tl+r
        print('      case {}:'.format(rn))
        print('         switch(d)')
        print('         {')
        for d in range(-rn, rn+1):
            print('             case {}: localW->r{}_d{} = v; break; '.format(d, rn, normd(d)));
        print('         }')
        print('         break;')
        
        print('      case  {}:'.format(rp))
        print('         switch(d)')
        print('         {')
        for d in range(-rn, rn+1):
            print('            case {}: localW->r{}_d{} = v; break;  '.format(d, rp, normd(d)));
        print('         }')
        print('         break;')
        
    print('   }')
    print('}')
    print('')
    
    
def createReadSwitch(tl):
        
    print('long inline __attribute__((always_inline))')
    print('readLocalW(LOCAL_TILE_PTR localW, int d, int r)')
    print('{')
    print('   long v; ')
    
    print('   switch(r)')        
    print('   {')
    for r in range(tl):
        rn = tl-r-1
        rp = tl+r
        print('      case {}:'.format(rn))
        print('         switch(d)')
        print('         {')
        for d in range(-rn, rn+1):
            print('             case {}: v = localW->r{}_d{}; break; '.format(d, rn, normd(d)));
        print('         }')
        print('         break;')
        
        print('      case  {}:'.format(rp))
        print('         switch(d)')
        print('         {')
        for d in range(-rn, rn+1):
            print('            case {}: v = localW->r{}_d{}; break;  '.format(d, rp, normd(d)));
        print('         }')
        print('         break;')
        
    print('   }')
        
    print('   return v;')
    print('}')
    print('')
    
for tl in range(10):
    print('#if TILE_LEN == {}'.format(tl))
    createStruct(tl)
    createWriteIf(tl)
    createReadIf(tl)         
    #createWriteSwitch(tl)
    #createReadSwitch(tl)         
    print('#endif')
