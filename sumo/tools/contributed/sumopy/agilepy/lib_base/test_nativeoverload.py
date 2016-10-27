"""
 Attempt to override natives with attrconfig.
 Problem: with each modifixation (i.e. x+=1)
a native is assigned to the attribute and configuration is lost.
"""
class NewBool(int):
    def __new__(cls, value,unit):
        
        self = int.__new__(cls, bool(value))
        self.unit = unit
        return self

class NewInt(int):
    def __new__(cls, value,unit):
        
        self = int.__new__(cls, int(value))
        self.unit = unit
        return self
 
class NewFloat(float):
    def __new__(cls, value,unit):
        
        self = float.__new__(cls, float(value))
        self.unit = unit
        return self


class NewString(str):
    def __new__(cls, value,unit):
        
        self = str.__new__(cls, str(value))
        self.unit = unit
        return self
               
###############################################################################
if __name__ == '__main__':
    """
    Test
    """
    b=NewBool(1,'m')
    c=NewBool(0,'n')
    
    print 'b&c',b&c
    print 'b|c',b|c
    print 'b.unit,c.unit', b.unit,c.unit
    
    
    n =NewInt(16,'m')
    m=NewInt(-30,'n')
    print 'm-n',m-n
    print 'm%d%s'%(m,m.unit)
    
    x = NewFloat(16.5,'m/s')
    y = NewFloat(1.0/3,'m/s')
    print 'x-y',x-y
    x = NewFloat(x+y,'m/s') # <<<<<<<<<<<this is the problem
    print 'x=%.2f%s'%(x,x.unit)
    
    s =NewString('autobus','xx')
    #s += '_13'
    print 's=%s%s'%(s,s.unit)
    
    