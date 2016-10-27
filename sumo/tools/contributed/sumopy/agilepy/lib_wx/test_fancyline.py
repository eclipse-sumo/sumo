 
import sys, os, types
APPDIR = os.path.join(os.path.dirname(__file__),"..")
sys.path.append(os.path.join(APPDIR,"lib_base"))
IMAGEDIR =  os.path.join(os.path.dirname(__file__),"images")

import classman as cm
import arrayman as am

from geometry import *


def get_ending(y1,x1,y2,x2,width):
    n_vert = 2
    alpha0=np.arctan2(y2-y1,x2-x1)
    
    print '  alpha0',alpha0/np.pi*180
    
    dphi = np.pi/(n_vert-1)
    phi = np.arange(-np.pi/2,np.pi/2+dphi,dphi).reshape(-1,1)
    alphas = alpha0+phi
    print '  alphas=alpha0+phi\n',alphas/np.pi*180
    y = np.sin(alphas) * width
    x = np.cos(alphas) * width
    print '  y=\n',y
    print '  x=\n',x

def get_lineindices(vertices, colours):
    linevertices = np.zeros((0,2,3),float)
    for polyline in vertices:
        print '======='
        print 'polyline\n',polyline
        v = np.zeros(  ( 2*len(polyline)-2,3),float)
        v[0]=polyline[0]
        v[-1]=polyline[-1]
        if len(v)>2:
            
            #print 'v[1:-1]',v[1:-1]
            #print 'v=\n',v
            #m = np.repeat(polyline[1:-1],2,0) 
            #print 'm\n',m,m.shape,m.dtype
            #v[1:-1] = m
            v[1:-1] = np.repeat(polyline[1:-1],2,0) 
        #vadd = v.reshape((-1,2,3))
        #print 'vadd\n',vadd
        
        print 'linevertex\n',linevertices
        linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))
        #linevertices = np.concatenate((linevertices, vadd))
        #linevertices += list(v.reshape((-1,2,3)))
    print 'FINAL linevertex\n',linevertices
    return linevertices

def get_polyvertices(vertices):
    linevertices = np.zeros((0,2,3),float)
    for polyline in vertices:
        print '======='
        print '  polyline\n',polyline
        v = np.zeros(  ( 2*len(polyline),3),float)
        v[0]=polyline[0]
        v[-2]=polyline[-1]
        v[-1]=polyline[0]
        #print '  v\n',v
        if len(v)>3:
            
            #print 'v[1:-1]',v[1:-1]
            #print 'v=\n',v
            #m = np.repeat(polyline[1:-1],2,0) 
            #print 'm\n',m,m.shape,m.dtype
            #v[1:-1] = m
            v[1:-1] = np.repeat(polyline[1:],2,0) 
            #print '  v rep\n',v,v.reshape((-1,2,3)), v.reshape((-1,2,3)).shape
            
        #vadd = v.reshape((-1,2,3))
        #print 'vadd\n',vadd
        #print '  linevertex\n',linevertices,linevertices.shape
        
        linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))
        #linevertices = np.concatenate((linevertices, vadd))
        #linevertices += list(v.reshape((-1,2,3)))
        print '  linevertex\n',linevertices
        
    print 'FINAL linevertex\n',linevertices
    return linevertices
  
def get_linevertices(vertices):
    linevertices = np.zeros((0,2,3),float)
    for polyline in vertices:
        print '======='
        print 'polyline\n',polyline
        v = np.zeros(  ( 2*len(polyline)-2,3),float)
        v[0]=polyline[0]
        v[-1]=polyline[-1]
        if len(v)>2:
            
            #print 'v[1:-1]',v[1:-1]
            #print 'v=\n',v
            #m = np.repeat(polyline[1:-1],2,0) 
            #print 'm\n',m,m.shape,m.dtype
            #v[1:-1] = m
            v[1:-1] = np.repeat(polyline[1:-1],2,0) 
        #vadd = v.reshape((-1,2,3))
        #print 'vadd\n',vadd
        
        print 'linevertex\n',linevertices
        linevertices = np.concatenate((linevertices, v.reshape((-1,2,3))))
        #linevertices = np.concatenate((linevertices, vadd))
        #linevertices += list(v.reshape((-1,2,3)))
    print 'FINAL linevertex\n',linevertices
    return linevertices

if 1:
    colors =np.array( [
                    [0.0,0.9,0.0,0.9],    # 0
                    [0.9,0.0,0.0,0.9],    # 1
                    ]) 
    vertices = np.array([
                        [[0.0,2.0,0.0],[5.0,2.0,0.0],[5.0,7.0,0.0],[0.0,7.0,0.0]],# 0 green
                        [[0.0,-2.0,0.0],[-2.0,-2.0,0.0],[-2.0,0.0,0.0]],# 1 red
                        ], np.object) 
    #get_linevertices(vertices)
    get_polyvertices(vertices)
    #print 'colors',colors[[0,1,1,0]]#<<cool
    #get_lineindices(vertices, colors)
    
    
if 0: 
    vertices = np.array([
                        [[0.0,2.0,0.0],[5.0,2.0,0.0],[5.0,7.0,0.0],[0.0,7.0,0.0]],# 0 green
                        [[0.0,-2.0,0.0],[-2.0,-2.0,0.0]],# 1 red
                        ], np.object)
    linevertices = []
    for polyline in vertices:
        print '======='
        print 'polyline\n',polyline
        v = np.zeros(  ( 2*len(polyline)-2,3),float)
        v[0]=polyline[0]
        v[-1]=polyline[-1]
        if len(v)>2:
            
            print 'v[1:-1]',v[1:-1]
            print 'v=\n',v
            m = np.repeat(polyline[1:-1],2,0) 
            
            #v[1:-1] = np.repeat(polyline[1:-1],2,0) 
            print 'm\n',m,m.shape,m.dtype
            
            v[1:-1] = m
            
        print 'linevertex',v.reshape((-1,2,3))
        #print 'polyline[1:-1]\n',polyline[1:-1]
        #print 'r\n',np.concatenate( ( np.repeat(polyline[1:-1],2,0) ) )
                        
if 0:
    vertices = np.array([
                        [[0.0,0.0,0.0],[1.0,0.0,0.0]],# 0 green
                        [[0.0,0.0,0.0],[0.0,1.0,0.0]],# 1 red
                        [[0.0,0.0,0.0],[1.0,1.0,0.0]],# 1 red
                        ])
    n= len(vertices)
    width = np.array([1.0,2.0,3.0])#.reshape((1,-1))                    
    x1 = vertices[:,0,0]
    y1 = vertices[:,0,1]
    
    x2 = vertices[:,1,0]
    y2 = vertices[:,1,1]
    get_ending(y1,x1,y2,x2,width)
                           