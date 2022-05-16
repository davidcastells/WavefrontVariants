
def normd(d):
    s = '{}'.format(d)
    return s.replace('-', 'm')
    
def createStruct(tl):
    k = 2*tl +1
    print('typedef struct')
    print('{')
    
    for r in range(2):
        for d in range(k):
            print('   long r{}_d{};'.format(r, normd(d)))
            
    print('} LOCAL_TILE;')
    print('#define LOCAL_TILE_PTR      LOCAL_TILE*')
    print('#define LOCAL_TILE_PARAM    LOCAL_TILE* localW')
    print('')
    
    
def createWriteIf(tl):
    k = 2*tl +1
    print('void inline __attribute__((always_inline))')
    print('writeLocalW(LOCAL_TILE_PARAM, int df, int rf, long v)')
    print('{')
    print('   int r = rf % 2;')
    print('   int d = df + {};'.format(tl))
    slink1 = ''
    
    for r in range(2):
        print('   {}if (r == {})'.format(slink1, r))
        print('   {')
        slink2 = ''
        for d in range(k):
            print('      {}if (d == {}) localW->r{}_d{} = v; '.format(slink2, d, r, d));
            slink2 = 'else '
        print('   }')
        
        
        slink1 = 'else '
    print('}')
    print('')
    
    
def createReadIf(tl):
    k = 2*tl +1
        
    print('long inline __attribute__((always_inline))')
    print('readLocalW(LOCAL_TILE_PARAM, int df, int rf)')
    print('{')
    print('   int r = rf % 2;')
    print('   int d = df + {};'.format(tl))
    print('   long v;')
    slink1 = ''
    
    for r in range(2):
        print('   {}if (r == {})'.format(slink1, r))
        print('   {')
        slink2 = ''
        for d in range(k):
            print('      {}if (d == {}) v = localW->r{}_d{} ; '.format(slink2, d, r, d));
            slink2 = 'else '
        print('   }')
        
        
        slink1 = 'else '
        
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
    print('readLocalW(LOCAL_TILE_PTR localW, int df, int rf)')
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
    createStruct(tl-1)
    createWriteIf(tl-1)
    createReadIf(tl-1)         
    #createWriteSwitch(tl)
    #createReadSwitch(tl)         
    print('#endif')
    
    
# EXECUTE IT WITH:
# python3 metaprogram_tiling.py > metaprogrammed_tiles.cl