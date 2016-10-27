"""
This plugin provides algorithms to assign an OD demand to a PRT network.
"""
 
"""
High demand crash bug:
On 06/22/2014 12:38 PM, Tiziano Parriani wrote:

> Ma adesso sono tornato a dedicare un po del mio tempo libero alla ricerca.
>
> Nell'istanza che mi mandi c'e una commodity, la 58, che invia e riceve
> dallo stesso nodo:
>
> 197    58    10
> 197    58    -10
>
> In questo caso succede che il problema generato e infeasible.
> Io non ho un segmentation fault, ma soltanto un warning: "WARNING: Model
> is infeasible:quality 3". E la successiva terminazione dell'algoritmo.
> Pero posso immaginare che il return della chiamata al mio codice torni
> qualcosa di brutto, che genera poi l'errore di memoria.
>
 
""" 

import sys,os, subprocess, glob
try:
    DIRNAME = os.path.dirname(os.path.abspath(__file__))
except:
    DIRNAME = os.path.dirname(os.path.abspath(sys.argv[0]))

APPDIR =  os.path.join(DIRNAME,"..","..")
BINDIR =  DIRNAME

import platform
global P
if platform.system()=='Windows':
    P = '"'
else:
    P=''
    
import lib_meta.classmanager as cm

try:
    import numpy as np
    from scipy.optimize import fmin
except:
    print 'Import error: In order to run the PRT assign module you need to install the modules:'
    print '  numpy, scipy, and CPlex.'
    print 'Please install these method if you want to use it.'
    print __doc__
    if platform.system() not in ['Linux','Unix']:
        print '  You also need to run it on a 64bit Linux or Unix.'
    raise
    
## functions from flussi_costi4.py...
inf_cost = 10.**4 #s
v_jam = 1.0/3.6#m/s
mu = 0.1

eta = 1.0 # step length scaler


#_____________________________________________________________________________
# ue with cost functions

#def get_c_ue(f,tau,a,L,v_line,la):
#    #return la*(1.0/v_line +a*f**4)
#    return la*(1.0/v_line +10.0*a*f**2)

#def get_dc_over_df_smooth(f,tau,a,L,v_line,la):  
#    return 1.0/4*la*a*f**3
#    #return 1.0/2*la*10*a*f
 
def get_obj_uefourth(f,f_line,a0,a1,a2,a3,a4):
  x=f-f_line
  z =  a0*f_line -0.5*a1*f_line**2  +a0*x+a1*x**2/2+a2*x**3/3+a3*x**4/4+a4*x**5/5
   
  if hasattr(f, '__iter__'):
        ind_free = f<=f_line
        if hasattr(a0, '__iter__'):
            z[ind_free] = a0[ind_free]*f[ind_free] -a1*f_line[ind_free]*f[ind_free]+0.5*a1*f[ind_free]**2
        else:
            z[ind_free] = a0*f[ind_free] -a1*f_line*f[ind_free]+0.5*a1*f[ind_free]**2
        return np.sum(z)
    
  elif f<=p:
        z = a0*f -a1*f_line*f+0.5*a1*f**2
  return z  
  

def get_obj_dir_uefourth(f,f_lin,f_line,a0,a1,a2,a3,a4):
    return get_obj_uefourth(f,f_line,a0,a1,a2,a3,a4)+np.sum(get_c_fourth(f,f_line,a0,a1,a2,a3,a4)*(f_lin-f))

def get_linobj_uefourth(params,f,f_lin,f_line,a0,a1,a2,a3,a4):
    #print 'get_linobj_uefourth',params,len(f),len(f_lin),len(f_line),len(a0),a1,len(a2),len(a3),len(a4)
    fp = f+params[0]*(f_lin-f)
    return get_obj_uefourth(fp,f_line,a0,a1,a2,a3,a4)

#_____________________________________________________________________________
# ue with smooth cost functions currentlu NOT USED

def get_c_ue(f,tau,a,L,v_line,la):
    #return la*(1.0/v_line +a*f**4)
    return la*(1.0/v_line +10.0*a*f**2)

#def get_dc_over_df_smooth(f,tau,a,L,v_line,la):  
#    return 1.0/4*la*a*f**3
#    #return 1.0/2*la*10*a*f
 
def get_obj_ue(f,tau,a,L,v_line,la):
  return np.sum(   f*la/v_line + 10.0/3*la*a*f**3)

def get_obj_dir_ue(f,f_lin,tau,a,L,v_line,la):
    return get_obj_ue(f,tau,a,L,v_line,la)+np.sum(get_c_ue(f,tau,a,L,v_line,la)*(f_lin-f))

def get_linobj_ue(params,f,f_lin,tau,a,L,v_line,la):
    fp = f+params[0]*(f_lin-f)
    return get_obj_ue(fp,tau,a,L,v_line,la )


#_____________________________________________________________________________
# smooth cost functions

def get_c_smooth(f,tau,a,L,v_line,la):
    
    #return la*(1.0/v_line +a*f**6)
    #return la*(1.0/v_line +a*f**5)
    return la*(1.0/v_line +a*f**4)
    #return la*(1.0/v_line +10.0*a*f**2)

def get_dc_over_df_smooth(f,tau,a,L,v_line,la):  
    #return 6.0*la*a*f**5
    #return 5.0*la*a*f**4
    return 4.0*la*a*f**3
    #return 2.0*la*10.0*a*f
 
def get_obj_smooth(f,tau,a,L,v_line,la):
  return np.sum( f*get_c_smooth(f,tau,a,L,v_line,la) )

def get_obj_dir_smooth(f,f_lin,tau,a,L,v_line,la):
    return get_obj_smooth(f,tau,a,L,v_line,la)+np.sum(get_dc_over_df_smooth(f,tau,a,L,v_line,la)*(f_lin-f))

def get_linobj_smooth(params,f,f_lin,tau,a,L,v_line,la):
    fp = f+params[0]*(f_lin-f)
    return np.sum( fp*get_c_smooth(fp,tau,a,L,v_line,la) )



#_____________________________________________________________________________
# linearized system optimum cost functions

def get_c_linear(f,tau,a,L,v_line,la):
    """Piecewise linear cost function"""
    c0=la/v_line# beta
    c1=la/get_v_crit(a,L)#gamma 
    p = get_f(v_line,tau,a,L)
    q = get_q(tau,a,L)
    c = c0+(c1-c0)/(q-p)*(f-p)
    
    if hasattr(f, '__iter__'):
        ind_free = f<=p
        c[ind_free] = c0[ind_free] -(p[ind_free]-f[ind_free])*mu
        c[c<0]=0.0
        
    elif f<=p:
        c = c0 -(p-f)*mu
        if c<0: c=0.0
    return c
  
def get_dc_over_df_linear(f,tau,a,L,v_line,la):  
    c0=la/v_line
    c1=la/get_v_crit(a,L)
    p = get_f(v_line,tau,a,L)
    q = get_q(tau,a,L)
    dcdf = (c1-c0)/(q-p)
        
    if hasattr(dcdf, '__iter__'):
        dcdf[f<=p] = mu
        #dcdf[np.isnan(dcdf)]=inf_cost
        
    elif f<=p:
        dcdf= mu
        
    elif np.isnan(c):
        dcdf = inf_cost
        
    return dcdf
 
def get_obj_linear(f,tau,a,L,v_line,la):
  return np.sum( f*get_c_linear(f,tau,a,L,v_line,la) )

def get_obj_dir_linear(f,f_lin,tau,a,L,v_line,la):
    return get_obj_linear(f,tau,a,L,v_line,la)+eta*np.sum(get_dc_over_df_linear(f,tau,a,L,v_line,la)*(f_lin-f))

def get_linobj_linear(params,f,f_lin,tau,a,L,v_line,la):
    fp = f+params[0]*(f_lin-f)
    return np.sum( fp*get_c_linear(fp,tau,a,L,v_line,la) ) 


#_____________________________________________________________________________
# quadratic system optimum cost functions

def get_c_quad(f,tau,a,L,v_line,la):
    """Piecewise linear cost function"""
    c0=la/v_line# beta
    c1=la/get_v_crit(a,L)#gamma 
    p = get_f(v_line,tau,a,L)
    q = get_q(tau,a,L)
    c = c0+ mu*(f-p) +(c1-c0-mu*(q-p))/(q-p)**2*(f-p)**2
    
    if hasattr(f, '__iter__'):
        ind_free = f<=p
        c[ind_free] = c0[ind_free] -(p[ind_free]-f[ind_free])*mu
        c[c<0]=0.0
    elif f<=p:
        c = c0 -(p-f)*mu
        if c<0: c=0.0
    return c
  
def get_dc_over_df_quad(f,tau,a,L,v_line,la):  
    c0=la/v_line
    c1=la/get_v_crit(a,L)
    p = get_f(v_line,tau,a,L)
    q = get_q(tau,a,L)
    dcdf = mu + 2.0*(c1-c0-mu*(q-p))/(q-p)**2*(f-p)
        
    if hasattr(dcdf, '__iter__'):
        dcdf[f<=p] = mu
        #dcdf[np.isnan(dcdf)]=inf_cost
        
    elif f<=p:
        dcdf= mu
        
    elif np.isnan(c):
        dcdf = inf_cost
        
    return dcdf
 
def get_obj_quad(f,tau,a,L,v_line,la):
  return np.sum( f*get_c_quad(f,tau,a,L,v_line,la) )

def get_obj_dir_quad(f,f_lin,tau,a,L,v_line,la):
    return get_obj_quad(f,tau,a,L,v_line,la)+eta*np.sum(get_dc_over_df_quad(f,tau,a,L,v_line,la)*(f_lin-f))

def get_linobj_quad(params,f,f_lin,tau,a,L,v_line,la):
    fp = f+params[0]*(f_lin-f)
    return np.sum( fp*get_c_quad(fp,tau,a,L,v_line,la) )   

    
#_____________________________________________________________________________
# 4th order approximation  cost functions  

def get_c_fourth(f,f_line,a0,a1,a2,a3,a4):
    """4th order approximation of cost function"""
    x=f-f_line
    c = a0+a1*x+a2*x**2+a3*x**3+a4*x**4
    
    if hasattr(f, '__iter__'):
        ind_free = f<=f_line
        if hasattr(a0, '__iter__'):
            c[ind_free] = a0[ind_free] -(f_line[ind_free]-f[ind_free])*a1
        else:
            c[ind_free] = a0 -(f_line-f[ind_free])*a1
        
        c[c<0]=0.0
        
    elif f<=p:
        c = a0 -(f_line-f)*a1
        if c<0.0: c=0.0
    
    return c
  
def get_dc_over_df_fourth(f,f_line,a0,a1,a2,a3,a4):  
    x=f-f_line
    dcdf = a1+2.0*a2*x+3.0*a3*x**2+4.0*a4*x**3
    
    if hasattr(dcdf, '__iter__'):
        dcdf[f<=f_line] = a1
  
    elif f<=f_line:
        dcdf = a1
        
  
    return dcdf

def get_obj_fourth(f,f_line,a0,a1,a2,a3,a4):
  return np.sum( f*get_c_fourth(f,f_line,a0,a1,a2,a3,a4) )

def get_obj_dir_fourth(f,f_lin,f_line,a0,a1,a2,a3,a4):
    return get_obj_fourth(f,f_line,a0,a1,a2,a3,a4)+eta*np.sum(get_dc_over_df_fourth(f,f_line,a0,a1,a2,a3,a4)*(f_lin-f))

def get_linobj_fourth(params,f,f_lin,f_line,a0,a1,a2,a3,a4):
    fp = f+params[0]*(f_lin-f)
    return np.sum( fp*get_c_fourth(fp,f_line,a0,a1,a2,a3,a4) ) 
    
def get_params_fourth(tau,a,L,v_line,la,mu):
    #mu,y0,x1,y1,x2,y2,x3,y3
    f0= get_f(v_line,tau,a,L)
    f3 = get_q(tau,a,L)
    f_delta = f3-f0
    x1 = 0.5*f_delta
    x2 = 0.9*f_delta
    x3 = f_delta
    
    y0 = la/v_line
    y1 = get_c(f0+x1,tau,a,L,v_line,la)
    y2 = get_c(f0+x2,tau,a,L,v_line,la)
    y3 = la/get_v_crit(a,L)
    
    #..................
    a0 = y0
    a1 = mu
    a2 = ((x1**3*x2**4-x1**4*x2**3)*y3+(x1**4*x3**3-x1**3*x3**4)*y2\
    +(x2**3*x3**4-x2**4*x3**3)*y1+((x1**3-x2**3)*x3**4+(x2**4-x1**4)*x3**3\
    -x1**3*x2**4+x1**4*x2**3)*y0+(mu*x1**3*x2-mu*x1*x2**3)*x3**4+\
    (mu*x1*x2**4-mu*x1**4*x2)*x3**3+(mu*x1**4*x2**3-mu*x1**3*x2**4)*x3)\
    /((x1**2*x2**3-x1**3*x2**2)*x3**4+(x1**4*x2**2-x1**2*x2**4)*x3**3\
    +(x1**3*x2**4-x1**4*x2**3)*x3**2)
        
    a3 = -((x1**2*x2**4-x1**4*x2**2)*y3+(x1**4*x3**2-x1**2*x3**4)*y2\
    +(x2**2*x3**4-x2**4*x3**2)*y1+((x1**2-x2**2)*x3**4+(x2**4-x1**4)*x3**2\
    -x1**2*x2**4+x1**4*x2**2)*y0+(mu*x1**2*x2-mu*x1*x2**2)*x3**4\
    +(mu*x1*x2**4-mu*x1**4*x2)*x3**2+(mu*x1**4*x2**2-mu*x1**2*x2**4)*x3)\
    /((x1**2*x2**3-x1**3*x2**2)*x3**4+(x1**4*x2**2-x1**2*x2**4)*x3**3\
    +(x1**3*x2**4-x1**4*x2**3)*x3**2)

    a4 = ((x1**2*x2**3-x1**3*x2**2)*y3+(x1**3*x3**2-x1**2*x3**3)*y2+\
    (x2**2*x3**3-x2**3*x3**2)*y1+((x1**2-x2**2)*x3**3+\
    (x2**3-x1**3)*x3**2-x1**2*x2**3+x1**3*x2**2)*y0+\
    (mu*x1**2*x2-mu*x1*x2**2)*x3**3+(mu*x1*x2**3-mu*x1**3*x2)*x3**2\
    +(mu*x1**3*x2**2-mu*x1**2*x2**3)*x3)/((x1**2*x2**3-x1**3*x2**2)*x3**4\
    +(x1**4*x2**2-x1**2*x2**4)*x3**3+(x1**3*x2**4-x1**4*x2**3)*x3**2)
    
    return f0,a0,a1,a2,a3,a4
    
    
#_____________________________________________________________________________
# system optimum  cost functions    
def get_c(f,tau,a,L,v_line,la):
  """Exact cost function"""
  q= get_q(tau,a,L)
  #print 'get_c',q
  #print 'f=\n',type(f),f
  
  if hasattr(f, '__iter__'):
      #print 'f=\n',f
      #print 'f>q',len(f>q), f>q
      c = la*f/(np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)-a*f*tau+a)
      #print 'get_v_crit(a,L)=',get_v_crit(a,L),type(get_v_crit(a,L))
      #print 'len(c),type(c)',len(c),type(c)
      c[f>q]= la[f>q]/get_v_crit(a,L)
      #print 'get_c=\n',c
      
      f_line = get_f(v_line,tau,a,L)
      ind_free = f<=f_line
      c_line = la[ind_free]/v_line[ind_free]
      f_free = f[ind_free]
      c[ind_free] = c_line -(f_line[ind_free]-f_free)*mu
  
      inds_nan= np.isnan(c)
      c[inds_nan]=la[inds_nan]/get_v_crit(a,L)#la[inds_nan]/v_jam# inf_cost
      c[c<0]=0.0
      
  elif f<=get_f(v_line,tau,a,L):
      c = la/v_line-(get_f(v_line,tau,a,L)-f)*mu
      if c<0:
          c=0.0
  else:
      if f>q: 
        c = la/get_v_crit(a,L)
      else:
        c = la*f/(np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)-a*f*tau+a)
      
      if np.isnan(c):
          c = la/v_jam
      if c<0:
          c=0.0
      
  return c

   
def get_dc_over_df(f,tau,a,L,v_line,la):
    
  c0=la/v_line
  c1=la/get_v_crit(a,L)
  p = get_f(v_line,tau,a,L)
  q = get_q(tau,a,L)
    

  dcdf = la/(np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)-a*f*tau+a)-f*la*((-4*a*f*L+2*a**2*f*tau**2-2*a**2*tau)/np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)/2.0E+0-a*tau)/(np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)-a*f*tau+a)**2
      
  if hasattr(dcdf, '__iter__'):
      ind_jam = f>=q
      dcdf[ind_jam] = (c1[ind_jam]-c0[ind_jam])/(q-p[ind_jam])
      dcdf[f<=get_f(v_line,tau,a,L)] = mu
      #dcdf[np.isnan(dcdf)]=inf_cost
      
  elif f<=get_f(v_line,tau,a,L):
      dcdf= mu
  elif f>=q:
      dcdf = (c1-c0)/(q-p)#inf_cost
  #else:
  #    dcdf = la/(np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)-a*f*tau+a)-f*la*((-4*a*f*L+2*a**2*f*tau**2-2*a**2*tau)/np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)/2.0E+0-a*tau)/(np.sqrt(-2*a*f**2*L+a**2*f**2*tau**2-2*a**2*f*tau+a**2)-a*f*tau+a)**2
  
  return dcdf
 
def get_obj(f,tau,a,L,v_line,la):
  return np.sum( f*get_c(f,tau,a,L,v_line,la) )

def get_obj_dir(f,f_lin,tau,a,L,v_line,la):
    return get_obj(f,tau,a,L,v_line,la)+eta*np.sum(get_dc_over_df(f,tau,a,L,v_line,la)*(f_lin-f))

def get_linobj(params,f,f_lin,tau,a,L,v_line,la):
    fp = f+params[0]*(f_lin-f)
    return np.sum( fp*get_c(fp,tau,a,L,v_line,la) )
    
#_____________________________________________________________________________
# general flow functions   

def get_f(v,tau,a,L):
  """Maximum Flow at speed v """
  return 1.0/(L/v+v/(2*a)+tau)

def get_v_crit(a,L):
  """Critical speed"""
  return np.sqrt(2*a*L)

def get_q(tau,a,L):
  """Capacity"""
  return get_f(get_v_crit(a,L),tau,a,L)


#_____________________________________________________________________________
# class definitions
     
class PRTAssignment(cm.ObjManagerMixin):
    def __init__(self,scenario, vtype = 'PRT',tau = 0.5, c_demand= 1.0, iter_max=5, err_min=0.02,gap_min=5.0,is_emptyflows=True):
        self._init_objman('assign_prt',parent=scenario, name = 'PRT Assignment')
        self.attrs = self.set_attrman(cm.AttrsManager(self,'attrs'))
        
        

        # here we ged classes not vehicle type
        # specific vehicle type within a class will be generated later 
        vtypes = self.parent.demand.getVehicles().get_keys()
        #print '  vtypes =',vtypes
        #self.vtype = vtype
        self.vtype = self.attrs.add(cm.AttrConf(  'vtype', vtype,
                                    groupnames = ['options'], 
                                    perm='rw', 
                                    choices = vtypes,
                                    name = 'Vehicle type', 
                                    info ='Select a vehicle type for assignment.',
                                    ))
                                    
        self.tau = self.attrs.add(cm.AttrConf(  'tau', tau,
                                    groupnames = ['params'], 
                                    perm='rw', 
                                    name = 'tau',
                                    unit = 's', 
                                    info ='Break actuation time.',
                                    digits_fraction= 4,
                                    ))
        
        self.c_demand = self.attrs.add(cm.AttrConf(  'c_demand', c_demand,
                                    groupnames = ['params'], 
                                    perm='rw', 
                                    name = 'demand factor',
                                    info ='Multiplier for demand.',
                                    digits_fraction= 4,
                                    ))
        
         
        self.is_emptyflows = self.attrs.add(cm.AttrConf(  'is_emptyflows', is_emptyflows,
                                    groupnames = ['params'], 
                                    perm='rw', 
                                    name = 'assign empty flows',
                                    info ='Assigne empty vehicle flows.',
                                    ))
        
        self.iter_max = self.attrs.add(cm.AttrConf(  'iter_max', iter_max,
                                    groupnames = ['params'], 
                                    perm='rw', 
                                    name = 'max iterations',
                                    info ='Maximum number of iterations.',
                                    ))
        self.err_min = self.attrs.add(cm.AttrConf(  'err_min', err_min,
                                    groupnames = ['params'], 
                                    perm='rw', 
                                    name = 'min error',
                                    info ='Minimum error threshold to stop iterations.',
                                    digits_fraction= 4,
                                    ))
        self.gap_min = self.attrs.add(cm.AttrConf(  'gap_min', gap_min,
                                    groupnames = ['params'], 
                                    perm='rw', 
                                    unit = '%',
                                    name = 'min gap',
                                    info ='Minimum gap threshold to stop iterations.',
                                    digits_fraction= 2,
                                    ))

        
        
        
        params = self.parent.demand.getVehicles().get_row(self.vtype)                            
        self.a = params['decel']
        self.length = params['length']
        self.v_line = params['maxSpeed']

        self.attrs.add(cm.FuncConf(  'capa', self.get_capacity,
                                    groupnames = ['params'], 
                                    name = 'capacity',
                                    digits_fraction= 2,
                                    unit = 'veh/h', 
                                    info ='Vehicle capacity.',
                                    ))                    
        
        
        self.n_trips = self.attrs.add(cm.AttrConf(  'n_trips', 0,
                                    groupnames = ['result'], 
                                    perm='r', 
                                    name = 'No. trips',
                                    info ='Assigned number of trips.',
                                    ))
                                    
        self.time_total = self.attrs.add(cm.AttrConf(  'time_total', 0.0,
                                    groupnames = ['result'], 
                                    perm='r', 
                                    name = 'Total trip time',
                                    unit = 'h', 
                                    digits_fraction= 4,
                                    info ='Total trip time with last assignment.',
                                    ))
                                    
        self.time_per_trip = self.attrs.add(cm.AttrConf(  'time_per_trip', 0.0,
                                    groupnames = ['result'], 
                                    perm='r', 
                                    name = 'Time per trip',
                                    unit = 's', 
                                    info ='Time per trip with last assignment.',
                                    digits_fraction= 2,
                                    ))
        
        self.share_empty = self.attrs.add(cm.AttrConf(  'share_empty', 0.0,
                                    groupnames = ['result'], 
                                    perm='r', 
                                    name = 'Share of empty',
                                    unit = '%', 
                                    info ='Share of empty vehicle flows.',
                                    digits_fraction= 2,
                                    ))
        
        self.share_congested = self.attrs.add(cm.AttrConf(  'share_congested', 0.0,
                                    groupnames = ['result'], 
                                    perm='r', 
                                    name = 'Share congested',
                                    unit = '%', 
                                    info ='Share of congested links.',
                                    digits_fraction= 2,
                                    ))
        
        self.share_over_capacity = self.attrs.add(cm.AttrConf(  'share_over_capacity', 0.0,
                                    groupnames = ['result'], 
                                    perm='r', 
                                    name = 'Share over capacity',
                                    unit = '%', 
                                    info ='Share of links that exceed capacity limits.',
                                    digits_fraction= 2,
                                    ))
                                    
        self._rootfilepath = self.parent.get_rootfilepath()
        
        self._flows_total_mnet = {}
        self._flows_empty_mnet = {}   
    
    def get_capacity(self):
        return 3600*get_q(self.tau,self.a,self.length)
    
    def get_scenario(self):
        return self.parent

    def export_to_mnet(self):
        """
        Creates the files for mnet
        """
        print 'export_to_mnet'
        self._demand=self.parent.get_demand()
        self._net=self.parent.get_net()
        self._edges = self._net.getEdges().values()
        self._nodes = self._net.getNodes().values()
        self.make_odm()# this is also generating node index tables
        self._n_nodes = len(self._nodes)
        self._n_commod_empty = self.export_sup()
        self.export_arc()
        self._n_arcs = len(self._idx_edge)
        
        self.export_nod()
        
        
    def iterate_assign_uesmooth_mnet(self, debug = 1):
        self.export_to_mnet()# this will export with freeflow  edge costs
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        costs_k = get_c_ue(np.zeros(len(inds_edge),float),self.tau,self.a,self.length,v_line_edges,length_edges)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        #while (i_iter < self.iter_max)&((err1>self.err_min)&(err>self.err_min)):
        while (i_iter < self.iter_max):#&(gap>self.err_min):
            i_iter +=1
            
            z_k = get_obj_ue(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            if debug>3:
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            #dc_over_df = get_dc_over_df_smooth(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            c_k = get_c_ue(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            
            costs_dir = eta*c_k
            #print 'costs_lin pre assign\n',costs_lin
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
            costs_dir[costs_dir<0]=0.0   
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_ue(flows_k, flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            #z_lin = get_obj(flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbound = z_dir
            #else:
            #    lowerbound= np.max([lowerbounds[-1],z_dir])
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            
            #gap = 100*( upperbounds[-1]-lowerbounds[-1] )/ np.abs(lowerbounds[-1])
            gap = 100*(z_k-lowerbound)/np.abs(lowerbound)
            if debug>3: 
                print '\n*****************flows_lin,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_ue, [0.0], args=(flows_k,flows_dir, self.tau, self.a, self.length, v_line_edges,length_edges) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_ue(flows1,self.tau,self.a,self.length,v_line_edges,length_edges)
            z1 = get_obj_ue(flows1, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                
         
            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
    
    def iterate_assign_ue_mnet(self, debug = 1):
        """
        Iterative User Equilibrium assignment where
        cost function is the 4th order polynomial approximation
        of the exact cost function
        """
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        f_line,a0,a1,a2,a3,a4 = get_params_fourth(self.tau,self.a,self.length,v_line_edges,length_edges,mu)
        costs_k = get_c_fourth(np.zeros(len(inds_edge),float),f_line,a0,a1,a2,a3,a4)
        self.assign_mnet() # with freeflow costs
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        is_stop = False
        while (i_iter < self.iter_max)& (not is_stop):
            i_iter +=1
            
            z_k = get_obj_uefourth(flows_k, f_line,a0,a1,a2,a3,a4)
            
            if debug>3: 
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            #dc_over_df = get_dc_over_df_fourth(flows_k,f_line,a0,a1,a2,a3,a4)
            c_k = get_c_fourth(flows_k,f_line,a0,a1,a2,a3,a4)
            
            costs_dir = eta*c_k
            #print 'costs_dir pre assign\n',costs_dir
            if np.any(costs_dir<0):
                return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
               
            if debug>3: 
                print '\n*****************flows_k,costs_dir:'
                self.print_flows(nodepairs,flows_k,costs_dir,v_line_edges,length_edges)
                
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_uefourth(flows_k, flows_dir, f_line,a0,a1,a2,a3,a4)
            
            
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbound = z_dir
            #else:
            #    lowerbound = np.min([lowerbounds[-1],z_dir])
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            gap = 100*( z_k-lowerbound )/ np.abs(lowerbound)
            
            if debug>3: 
                print '\n*****************flows_dir,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            lamb = fmin( get_linobj_uefourth, [0.0], args=(flows_k,flows_dir, f_line,a0,a1,a2,a3,a4) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_fourth(flows1,f_line,a0,a1,a2,a3,a4)
            z1 = get_obj_uefourth(flows1,f_line,a0,a1,a2,a3,a4)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
        
            
    def iterate_assign_smooth_mnet(self, debug = 1):
        self.export_to_mnet()# this will export with freeflow  edge costs
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        print 'iterate_assign_smooth_mnet %d edges, %d lengths'%(len(nodepairs),len(length_edges))
        
        # initial assignment
        costs_k = get_c_smooth(np.zeros(len(inds_edge),float),self.tau,self.a,self.length,v_line_edges,length_edges)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        #while (i_iter < self.iter_max)&((err1>self.err_min)&(err>self.err_min)):
        while (i_iter < self.iter_max):#&(gap>self.err_min):
            i_iter +=1
            
            z_k = get_obj_smooth(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            if debug>3:
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df_smooth(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            c_k = get_c_smooth(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            
            costs_dir = eta*(dc_over_df * flows_k + c_k)
            #print 'costs_lin pre assign\n',costs_lin
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
            costs_dir[costs_dir<0]=0.0
            
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_smooth(flows_k, flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            #z_lin = get_obj(flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbound = z_dir
            #else:
            #    lowerbound= np.max([lowerbounds[-1],z_dir])
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            
            #gap = 100*( upperbounds[-1]-lowerbounds[-1] )/ np.abs(lowerbounds[-1])
            gap = 100*(z_k-lowerbound)/np.abs(lowerbound)
            if debug>3: 
                print '\n*****************flows_lin,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_smooth, [0.0], args=(flows_k,flows_dir, self.tau, self.a, self.length, v_line_edges,length_edges) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_smooth(flows1,self.tau,self.a,self.length,v_line_edges,length_edges)
            z1 = get_obj_smooth(flows1, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                
         
            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
        
    def iterate_assign_linear_mnet(self, debug = 1):
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        costs_k = get_c_linear(np.zeros(len(inds_edge),float),self.tau,self.a,self.length,v_line_edges,length_edges)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        #while (i_iter < self.iter_max)&((err1>self.err_min)&(err>self.err_min)):
        while (i_iter < self.iter_max):#&(gap>self.err_min):
            i_iter +=1
            
            z_k = get_obj_linear(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            if debug>3:
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df_linear(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            c_k = get_c_linear(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            
            costs_dir = dc_over_df * flows_k + c_k
            #print 'costs_lin pre assign\n',costs_lin
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
            costs_dir[costs_dir<0]=0.0
            
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_linear(flows_k, flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            #z_lin = get_obj(flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbounds = [z_dir]
            #else:
            #    lowerbounds.append(np.max([lowerbounds[-1],z_dir]))
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            
            #gap = 100*( upperbounds[-1]-lowerbounds[-1] )/ np.abs(lowerbounds[-1])
            gap = 100*(z_k-lowerbound)/np.abs(lowerbound)
            if debug>3: 
                print '\n*****************flows_dir,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_linear, [0.0], args=(flows_k,flows_dir, self.tau, self.a, self.length, v_line_edges,length_edges) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_linear(flows1,self.tau,self.a,self.length,v_line_edges,length_edges)
            z1 = get_obj_linear(flows1, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
    
    def iterate_assign_linear2_mnet(self, debug = 1):
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        costs_k = get_c_linear(np.zeros(len(inds_edge),float),self.tau,self.a,self.length,v_line_edges,length_edges)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        #while (i_iter < self.iter_max)&((err1>self.err_min)&(err>self.err_min)):
        while (i_iter < self.iter_max):#&(gap>self.err_min):
            i_iter +=1
            
            z_k = get_obj_linear(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            if debug>3:
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df_linear(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            c_k = get_c_linear(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            
            costs_dir = dc_over_df * flows_k + c_k
            #print 'costs_lin pre assign\n',costs_lin
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
            costs_dir[costs_dir<0]=0.0
            
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_linear(flows_k, flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            #z_lin = get_obj(flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbounds = [z_dir]
            #else:
            #    lowerbounds.append(np.max([lowerbounds[-1],z_dir]))
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            
            #gap = 100*( upperbounds[-1]-lowerbounds[-1] )/ np.abs(lowerbounds[-1])
            gap = 100*(z_k-lowerbound)/np.abs(lowerbound)
            if debug>3: 
                print '\n*****************flows_dir,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_linear, [0.0], args=(flows_k,flows_dir, self.tau, self.a, self.length, v_line_edges,length_edges) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_linear(flows1,self.tau,self.a,self.length,v_line_edges,length_edges)
            z1 = get_obj_linear(flows1, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            #costs_k = costs1# only for print
            costs_k = costs_k+lamb*(costs_dir-costs_k)
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
        
            
    def iterate_assign_quad_mnet(self, debug = 1):
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        costs_k = get_c_quad(np.zeros(len(inds_edge),float),self.tau,self.a,self.length,v_line_edges,length_edges)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = []
        objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        #while (i_iter < self.iter_max)&((err1>self.err_min)&(err>self.err_min)):
        while (i_iter < self.iter_max):#&(gap>self.err_min):
            i_iter +=1
            
            z_k = get_obj_quad(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            if debug>3:
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df_quad(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            c_k = get_c_quad(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            
            costs_dir = eta*(dc_over_df * flows_k + c_k)
            #print 'costs_lin pre assign\n',costs_lin
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
            costs_dir[costs_dir<0]=0.0   
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_quad(flows_k, flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            #z_lin = get_obj(flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbounds = [z_dir]
            #else:
            #    lowerbounds.append(np.max([lowerbounds[-1],z_dir]))
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            
            #gap = 100*( upperbounds[-1]-lowerbounds[-1] )/ np.abs(lowerbounds[-1])
            gap = 100*(z_k-lowerbound)/np.abs(lowerbound)
            if debug>3: 
                print '\n*****************flows_dir,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_quad, [0.0], args=(flows_k,flows_dir, self.tau, self.a, self.length, v_line_edges,length_edges) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_quad(flows1,self.tau,self.a,self.length,v_line_edges,length_edges)
            z1 = get_obj_quad(flows1, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
        
        
    def iterate_assign_fourth_mnet(self, debug = 1):
        print 'iterate_assign_fourth_mnet'
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        f_line,a0,a1,a2,a3,a4 = get_params_fourth(self.tau,self.a,self.length,v_line_edges,length_edges,mu)
        #print 'a0=\n',a0
        #print 'a1=\n',a1
        #print 'a2=\n',a2
        #print 'a3=\n',a3
        #print 'a4=\n',a4
        
        costs_k = get_c_fourth(np.zeros(len(inds_edge),float),f_line,a0,a1,a2,a3,a4)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        is_stop = False
        while (i_iter < self.iter_max)& (not is_stop):
            i_iter +=1
            
            z_k = get_obj_fourth(flows_k, f_line,a0,a1,a2,a3,a4)
            
            if debug>3: 
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df_fourth(flows_k,f_line,a0,a1,a2,a3,a4)
            
            #print 'dc_over_df pre assign\n',dc_over_df
            #c_k = get_c_fourth(flows_k,f_line,a0,a1,a2,a3,a4)
            
            costs_dir = eta*(dc_over_df * flows_k + costs_k)
            costs_dir[costs_dir<0]=0.0
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
                
            #print 'costs_dir pre assign\n',costs_dir
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_fourth(flows_k, flows_dir, f_line,a0,a1,a2,a3,a4)
      
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbound = z_dir
            #else:
            #    lowerbound = np.min([lowerbounds[-1],z_dir])
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            gap = 100*( z_k-lowerbound )/ np.abs(lowerbound)
            #is_stop = gap<self.gap_min
            if debug>3: 
                print '\n*****************flows_dir,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_fourth, [0.0], args=(flows_k,flows_dir, f_line,a0,a1,a2,a3,a4) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_fourth(flows1,f_line,a0,a1,a2,a3,a4)
            z1 = get_obj_fourth(flows1,f_line,a0,a1,a2,a3,a4)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
        
    def iterate_assign_fourth2_mnet(self, debug = 1):
        print 'iterate_assign_fourth2_mnet'
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        f_line,a0,a1,a2,a3,a4 = get_params_fourth(self.tau,self.a,self.length,v_line_edges,length_edges,mu)
        #print 'a0=\n',a0
        #print 'a1=\n',a1
        #print 'a2=\n',a2
        #print 'a3=\n',a3
        #print 'a4=\n',a4
        
        costs_k = get_c_fourth(np.zeros(len(inds_edge),float),f_line,a0,a1,a2,a3,a4)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        is_stop = False
        while (i_iter < self.iter_max)& (not is_stop):
            i_iter +=1
            
            z_k = get_obj_fourth(flows_k, f_line,a0,a1,a2,a3,a4)
            
            if debug>3: 
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df_fourth(flows_k,f_line,a0,a1,a2,a3,a4)
            
            #print 'dc_over_df pre assign\n',dc_over_df
            #c_k = get_c_fourth(flows_k,f_line,a0,a1,a2,a3,a4)
            
            costs_dir = eta*(dc_over_df * flows_k + costs_k)
            costs_dir[costs_dir<0]=0.0
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
                
            #print 'costs_dir pre assign\n',costs_dir
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir_fourth(flows_k, flows_dir, f_line,a0,a1,a2,a3,a4)
      
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbound = z_dir
            #else:
            #    lowerbound = np.min([lowerbounds[-1],z_dir])
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            gap = 100*( z_k-lowerbound )/ np.abs(lowerbound)
            #is_stop = gap<self.gap_min
            if debug>3: 
                print '\n*****************flows_dir,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj_fourth, [0.0], args=(flows_k,flows_dir, f_line,a0,a1,a2,a3,a4) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c_fourth(flows1,f_line,a0,a1,a2,a3,a4)
            z1 = get_obj_fourth(flows1,f_line,a0,a1,a2,a3,a4)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
            
            # prepare for nexi iteration
            costs_k = costs_k+lamb*(costs_dir-costs_k) #costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
                
    def iterate_assign_mnet(self, debug = 1):
        self.export_to_mnet()
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        #nodepairs = []
        #flows_total = []
        #v_line_edges = []
        #length_edges = []
        nodepairs = self._idx_edge
        v_line_edges =[]
        length_edges = []
        inds_edge = []
        for nn in self._idx_edge:
            v_line_edges.append(self._idx_to_v_line[nn])
            length_edges.append(self._idx_to_length[nn])
            inds_edge.append(self._idx_to_ind_edge[nn])
        v_line_edges = np.array(v_line_edges, float)
        length_edges = np.array(length_edges, float)
        
        #v_line_edges = np.array(self._idx_to_v_line.values(),float)
        #length_edges = np.array(self._idx_to_length.values(),float)
        #inds_edge = self._idx_to_ind_edge.values()
        
        
        # initial assignment
        costs_k = get_c(np.zeros(len(inds_edge),float),self.tau,self.a,self.length,v_line_edges,length_edges)
        self.assign_mnet()
        flows_k,flows_k_empty  = self._read_flowdata_mnet(nodepairs, timerange)
        i_iter = 0
        gap=np.inf
        err = 1.0
        err1 = 1.0
        errors = []
        objectives = [];objectives_dir = []
        lambdas = []
        lowerbounds = []
        upperbounds = []
        gaps = []
        #while (i_iter < self.iter_max)&((err1>self.err_min)&(err>self.err_min)):
        while (i_iter < self.iter_max):#&(gap>self.err_min):
            i_iter +=1
            
            z_k = get_obj(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            if debug>3:
                print '\n*****************flows_k,costs_k:'
                self.print_flows(nodepairs,flows_k,costs_k,v_line_edges,length_edges)
            
            dc_over_df = get_dc_over_df(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            c_k = get_c(flows_k,self.tau,self.a,self.length,v_line_edges,length_edges)
            
            costs_dir = eta * (dc_over_df * flows_k + c_k)
            #print 'costs_lin pre assign\n',costs_lin
            #if np.any(costs_dir<0):
            #    return self.return_error(i_iter,gaps,errors,objectives, objectives_dir,lambdas)
            costs_dir[costs_dir<0]=0.0
            
            flows_dir,flows_dir_empty = self.assign_congested_mnet(nodepairs, inds_edge, costs_dir, timerange)        
            
            z_dir = get_obj_dir(flows_k, flows_dir, self.tau,self.a,self.length,v_line_edges,length_edges)
            lowerbound=z_dir
            #if i_iter==1:
            #    lowerbound = z_dir
            #else:
            #    lowerbound = np.min([lowerbounds[-1],z_dir])
            lowerbounds.append(lowerbound)
            upperbounds.append(z_k)
            gap = 100*( z_k-lowerbound )/ np.abs(lowerbound)
            
            if debug>3: 
                print '\n*****************flows_lin,costs_dir:'
                self.print_flows(nodepairs,flows_dir,costs_dir,v_line_edges,length_edges)
                #print 'costs_lin post assign\n',costs_lin
            
            
            lamb = fmin( get_linobj, [0.0], args=(flows_k,flows_dir, self.tau, self.a, self.length, v_line_edges,length_edges) )[0]
            
            flows1 = flows_k+lamb*(flows_dir-flows_k)
            flows1_empty = flows_k_empty+lamb*(flows_dir_empty-flows_k_empty)
            
            #print 'flows1-flows_k',flows1-flows_k
            err = np.sum(np.abs(flows1-flows_k))/sum(np.abs(flows_k))
            err1 = err
            
            costs1 = get_c(flows1,self.tau,self.a,self.length,v_line_edges,length_edges)
            z1 = get_obj(flows1, self.tau,self.a,self.length,v_line_edges,length_edges)
            
            gap1 = 100*(z1-lowerbound)/np.abs(lowerbound)
            errors.append(err)
            objectives.append(z_k);objectives_dir.append(z_dir)
            lambdas.append(lamb);gaps.append(gap)
            
            if debug>3: 
                print '\n*****************flows1,costs1:'
                
                self.print_flows(nodepairs,flows1,costs1,v_line_edges,length_edges)
                

            
            if debug>0:
                print 79*'.'
                print '  iter=%d:lamb=%.4f, err=%.4f, z_k=%.2f, z_k+1=%.2f, z_dir=%.2f'%(i_iter,lamb,err,z_k,z1,z_dir)
                print '  LBD=%.3f, UBD=%.3f, GAP=%.3f,GAP1=%.3f,'%(lowerbounds[-1],upperbounds[-1],gap,gap1)
                
            # prepare for nexi iteration
            costs_k = costs1# only for print
            flows_k = flows1#self.assign_congested_mnet(nodepairs, inds_edge, costs1, timerange) 
        
        # final results
        self.make_results(flows_k,flows1_empty,v_line_edges,length_edges,timerange)
        
        self.time_total = get_obj(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)*timerange/3600.0
        self.time_per_trip = self.time_total*3600/self.n_trips
        
        flows_mnet ={}
        flows_empty_mnet ={}
        costs_mnet = {}
        flows_dir_mnet ={}
        flows_dir_empty_mnet ={}
        for nn,f,f_empty,c,f_dir, f_dir_empty in zip(nodepairs,flows1,flows1_empty,costs1,flows_dir,flows_dir_empty): 
            flows_mnet[nn]=f
            flows_empty_mnet[nn]=f_empty
            costs_mnet[nn]=c
            flows_dir_mnet [nn]=f_dir
            flows_dir_empty_mnet[nn] = f_dir_empty
            
        return flows_mnet,flows_empty_mnet, costs_mnet,i_iter,gaps,errors,objectives, objectives_dir,lambdas, flows_dir_mnet, flows_dir_empty_mnet
    
    def make_results(self,flows_k,flows_k_empty,v_line_edges,length_edges,timerange):
        # final results
        self.time_total = get_obj(flows_k, self.tau,self.a,self.length,v_line_edges,length_edges)*timerange/3600.0
        self.time_per_trip = self.time_total*3600/self.n_trips
        self.share_empty = np.sum(flows_k_empty)/np.sum(flows_k)*100
        self.share_congested = float(np.sum(flows_k>get_f(v_line_edges,self.tau,self.a,self.length)))/float(len(flows_k))*100
        self.share_over_capacity = float(np.sum(flows_k>get_q(self.tau,self.a,self.length)))/float(len(flows_k))*100
        
    def return_error(self,i_iter,gaps,errors,objectives, objectives_dir,lambdas):
        self.time_total = 0.0
        self.time_per_trip = 0.0
        self.share_empty = 0.0
        self.share_congested = 0.0
        self.share_over_capacity = 0.0
        return None,None, None,i_iter,gaps,errors,objectives, objectives_dir,lambdas, None, None
        
    def assign_congested_mnet(self, nodepairs, inds_edge, costs, timerange):
        self.export_arc_congested( nodepairs, inds_edge, costs)
        self.assign_mnet()
        return self._read_flowdata_mnet(nodepairs,timerange)
        
    def print_flows(self, nodepairs,flows,costs,v_line_edges,length_edges):
        # nodepairs,flows_lin,costs_lin,v_line_edges,length_edges
        q = get_q(self.tau,self.a,self.length)
        #print 'costs in print_flows\n',costs
        for nn,f,c,v_line,length_edge in zip(nodepairs,flows,costs,v_line_edges,length_edges):
            if True:#f>0:
                c_free = length_edge/v_line
                f_free = get_f(v_line,self.tau,self.a,self.length)
                id_edge = self._idx_to_edge[nn].getID()
                #get_f(self.v_line,self.tau,self.a,self.length)
                #print '  %s: f=%.2f/h, q=%.2f/h, f_free=%.2f/h, c=%.3fs, c_free=%.3fs'%(id_edge,f*3600,q*3600,f_free*3600,c,c_free)
                print '  %s: f=%.2f/h, vl=%.2fm/s, len=%.2fm, c=%.3fs, c_free=%.3fs'%(id_edge,f*3600,v_line,length_edge,c,c_free)
        
    def assign_mnet(self):
        #if platform.system()=='Windows':
        if os.path.isfile(self._get_filename_totalflow()):
            os.remove(self._get_filename_totalflow())
                
        if os.path.isfile(self._get_filename_emptyflow()):
            os.remove(self._get_filename_emptyflow())
        
        dirpath = os.path.dirname(self._rootfilepath)
        mnetname =  os.path.basename(self._rootfilepath)+'.mnet' 
        print 'assign_mnet',mnetname,dirpath
        
        curdir = os.path.abspath(os.curdir)
        
        cmd_chdid = 'cd '+ dirpath
        bin = os.path.join(BINDIR,"MCFP11_49")
        #bin = './MCFP11_49'
        cml = cmd_chdid+';'+bin+' '+mnetname+' 2 mnet parameter.par ./'
        #os.chdir(dirpath)
        print  '  cml =',cml
        #print '  cwd=',os.path.abspath(os.curdir)
        #self._subprocess = subprocess.Popen(cml, shell=True)
        os.system(cml)
        #os.chdir(curdir)
        #print '  back to',os.path.abspath(os.curdir)
        print '  done.'
        
    def _get_filename_totalflow(self):
        filelist =  glob.glob(self._rootfilepath+'.mnet*STD_totalflows_solution.data')
        if len(filelist)>0:
            return filelist[0]
        else:
            return  self._rootfilepath+'.mnet|UNS-0.000-1-1-1-6400-STD_totalflows_solution.data'
    
    def _get_filename_emptyflow(self):
        filelist =  glob.glob(self._rootfilepath+'.mnet*STD_solution.data')
        if len(filelist)>0:
            return filelist[0]
        else:
            return  self._rootfilepath+'.mnet|UNS-0.000-1-1-1-6400-STD_solution.data'
    
       
    def _read_flowdata_mnet(self, nodepairs, timerange):
        """
        Read total flow and empty flow from current assignment result files 
        and returns also compatible length and line speed data.
        """
        
        filepath = self._get_filename_totalflow()
        
        print '_read_flowdata_mnet',filepath
        if not os.path.isfile(filepath):
            print 'WARNING in read_flows_mnet: no file %s'%filepath
            return []
        
        
        
        
        flows = np.zeros(len(nodepairs),float)
        
        fd = open(filepath, 'r')
        for line in fd.readlines():
            cols = line.split(' = ')
            if len(cols)==2:
                s_edge,s_flow = cols
                #s_edge = s_edge.replace('[',' ').replace(']',' ')
                #print '  *%s*'%s_edge, s_edge.split('  ')
                s1,s_from,s_to = s_edge.replace('[',' ').replace(']',' ').split('  ')
                #nn = (int(s_from),int(s_to))
                flows[self._idx_to_ind_edge[(int(s_from),int(s_to))]-1] = float(s_flow)
                
        fd.close()
        
        #print '  flows_total_mnet=',flows_total_mnet
        
        flows_empty =  np.zeros(len(nodepairs),float)
        if self.is_emptyflows:
            
            filepath = self._get_filename_emptyflow()
            if not os.path.isfile(filepath):
                print 'WARNING in read_flows_mnet: no file %s'%filepath
                return {}
                
            fd = open(filepath, 'r')
            for line in fd.readlines():
                cols = line.split(' = ')
                if len(cols)==2:
                    s_data,s_flow = cols
                    s_edge,s_commod = s_data.split('_')
                    if int(s_commod)==self._n_commod_empty: # count only if empty commodity
                        #print '  ',s_edge.replace('[',' ').replace(']',' ').split(' ')
                        s1,s_from,s2,s_to,s3 = s_edge.replace('[',' ').replace(']',' ').split(' ')
                        
                        flows_empty[self._idx_to_ind_edge[(int(s_from),int(s_to))]-1] = float(s_flow)
           
        fd.close() 
        
        
        return flows/timerange, flows_empty/timerange
    
    
    def read_flows_total_mnet(self):
        filepath = self._get_filename_totalflow()
        if not os.path.isfile(filepath):
            print 'WARNING in read_flows_mnet: no file %s'%filepath
            return {}
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        flows_total_mnet = {}
        fd = open(filepath, 'r')
        for line in fd.readlines():
            cols = line.split(' = ')
            if len(cols)==2:
                s_edge,s_flow = cols
                #s_edge = s_edge.replace('[',' ').replace(']',' ')
                #print '  *%s*'%s_edge, s_edge.split('  ')
                s1,s_from,s_to = s_edge.replace('[',' ').replace(']',' ').split('  ')
                flows_total_mnet[(int(s_from),int(s_to))] = float(s_flow)/timerange
        fd.close()        
        #print 'read_flows_total_mnet  flows_total_mnet=',flows_total_mnet
        return flows_total_mnet
    
    
    def read_flows_empty_mnet(self):
        flows_empty_mnet = {} 
        if not self.is_emptyflows:
            return flows_empty_mnet
        
        filepath = self._get_filename_emptyflow()
        if not os.path.isfile(filepath):
            print 'WARNING in read_flows_mnet: no file %s'%filepath
            return {}
        demand = self.parent.get_demand()
        timerange = demand.get_time_end()-demand.get_time_start()
        fd = open(filepath, 'r')
        for line in fd.readlines():
            cols = line.split(' = ')
            if len(cols)==2:
                s_data,s_flow = cols
                s_edge,s_commod = s_data.split('_')
                if int(s_commod)==self._n_commod_empty: # count only if empty commodity
                    #print '  ',s_edge.replace('[',' ').replace(']',' ').split(' ')
                    s1,s_from,s2,s_to,s3 = s_edge.replace('[',' ').replace(']',' ').split(' ')
                    flows_empty_mnet[(int(s_from),int(s_to))] = float(s_flow)/timerange
       
        fd.close()        
        #print '  flows_empty_mnet=',flows_empty_mnet

        return flows_empty_mnet
            
    def get_edge_to_flows(self,flows_mnet,is_per_hour = True):
        if is_per_hour:
            demand = self.parent.get_demand()
            timerange = demand.get_time_end()-demand.get_time_start()
        else:
            timerange = 1.0
            
        flows = {}
        for edge in self._edges:
            flows[edge] = 0.0

        for nn, f in flows_mnet.iteritems():
            flows[self._idx_to_edge[nn]] = f*timerange
            
             
        return flows
        
            
    def export_nod(self):
        fd = open(self._rootfilepath+'.mnet.nod', 'w')
        fd.write('%d\n'%self._n_commod_empty)
        fd.write('%d\n'%self._n_nodes)
        fd.write('%d\n'%self._n_arcs)
        fd.write('%d\n'%self._n_arcs)
        fd.close()
        
    def export_arc_congested(self,  nodepairs, inds_edge, costs, costscale=10.0**6):
        
        print 'export_arc_congested'
        rows_arc = []
        
        #for nn, edge in self._idx_to_edge.iteritems():
        #for i in xrange(len(inds_edge)):
        for ind_edge, (n1,n2), c in zip(inds_edge, nodepairs, costscale*costs):
            #if nn in nodepairs:
            #print '  used:',nn,costs[nodepairs.index(nn)]
            #cost = costs[nodepairs.index(nn)]
            #else:
            #    #print '  unused:',nn,edge.getLength()/edge.getSpeed()
            #    cost = edge.getLength()/edge.getSpeed()
                
            #ind_edge = self.idx_edge(edge)   
            rows_arc.append([ind_edge,n1, n2, -1,'%.6f'%c, -1, ind_edge])
            
        self.write_rows(rows_arc, self._rootfilepath+'.mnet.arc')
        
    def export_arc(self, costscale=10.0**6):
        
        
        #a  from to comm cost      capa        sharedcapa ind
        #1	235	241	49	1.712063	-1.000000	1
        capa_inf = 10000
        rows_arc = []
        rows_mut = []
        idx_to_edge = {}
        idx_to_v_line = {}
        idx_to_length = {}
        idx_to_ind_edge = {} 
        idx_edge = []
        for edge in self._edges:
            
            n_from = self.idx_node(edge.getFromNode())
            n_to = self.idx_node(edge.getToNode())
            nn = (n_from, n_to)
            if idx_to_edge.has_key(nn):
                print 'WARNING:double',nn,idx_to_edge[nn].getLength(),edge.getLength()
            else:
                idx_to_edge[nn]=edge
                idx_edge.append(nn)
            
        idx_edge.sort()
        ind_edge = 1
        for nn in idx_edge:
            #ind_edge = self.idx_edge(edge)
            edge = idx_to_edge[nn]
            #print '  ind_edge =%03d: %s->%s'%(ind_edge,nn,edge.getID())
            idx_to_v_line[nn]=edge.getSpeed()
            idx_to_length[nn]=edge.getLength()
            idx_to_ind_edge[nn]=ind_edge
            #cost = edge.getCost() # freeflow
            cost = costscale*edge.getLength()/edge.getSpeed()
        
            rows_arc.append([ind_edge,nn[0], nn[1], -1,'%.6f'%cost, -1, ind_edge])
            rows_mut.append([ind_edge,'%.6f'%capa_inf])
            #rows_edgeindex([i_edge, edge.getID()])
            ind_edge+=1
        
        print 'export_arc %d idx_to_edge, %d rows_arc'%(len(idx_to_edge),len(rows_arc))
        self._idx_edge = idx_edge
        self._idx_to_edge = idx_to_edge
        self._idx_to_v_line = idx_to_v_line
        self._idx_to_length = idx_to_length
        self._idx_to_ind_edge = idx_to_ind_edge
        self.write_rows(rows_arc, self._rootfilepath+'.mnet.arc')
        self.write_rows(rows_mut, self._rootfilepath+'.mnet.mut')

    def export_sup(self):
        print 'export_sup'
        rows = []
        i_commod = 0

        # define commodities with occupied vehicles from od matrix
        
        
        # for calculating residual demand
        demand_in={}
        demand_out={}
        for n in self._idx_to_node.keys():
            demand_in[n]=0
            demand_out[n]=0
        
        for (o,d), demand in self._odm.iteritems():
            if o != d:
                i_commod +=1
                rows.append([o, i_commod, demand])
                rows.append([d, i_commod, -demand])
                
                demand_in[d]+=demand
                demand_out[o]+=demand
        
        # define commodity with residual demand= demand_out[n]-demand_in[n]
        #print '  demand_in',demand_in
        #print '  demand_out',demand_out
        if self.is_emptyflows:
            i_commod +=1  # one commodity or all residual demands
            for n in  self._idx_to_node.keys():
                if (demand_in[n]>0)|(demand_out[n]>0):
                    rows.append([n, i_commod, -demand_out[n]+demand_in[n]])
                    
        
        self.write_rows(rows, self._rootfilepath+'.mnet.sup')
        return i_commod
        
    def write_rows(self, rows, filepath, sep = '\t'):
        
        fd = open(filepath, 'w')
        for row in rows:
            fd.write('%s\n' % sep.join(map(str, row)))
        fd.close()   
            
             
    def make_odm(self):
        """
        Creates dictionary self._odm with (o,d) pair as key and
        the flow as value, where o,d are node indexes used in the assignment.
        
        It forther creates the dictionnaries
        self._idx_to_node with node index as key and node instance as value
        
        and 
        self._node_to_idx whing node instance as key and index as value 
         
        
        By convention,  o is the to-node of a departure edge and
        d is the from-node of an arrival edge.
        In this way the demand-edge becomes a station.
        
        Departure and arrivel edges are taken from the trip database.
        
        """
        demand = self._demand#self.parent.get_demand()
        
        
        net = self._net#self.parent.get_net()
        print 'get_odm',len(demand.get_trips())
        
        odm = {}
        idx_to_node = {}
        node_to_idx = {}
        nodes = set()
        for trip in demand.get_trips().itervalues():
            #print '  ',trip,trip['type'],self.vtype
            if trip['type']==self.vtype:
                node_from = net.getEdge(trip['from']).getFromNode()#.getToNode()
                node_to = net.getEdge(trip['to']).getFromNode()
                
                if node_from != node_to:
                    if node_from not in nodes:
                        nodes.add(node_from)
                        idx_node_from = self.idx_node(node_from)
                        idx_to_node[idx_node_from]=node_from
                        node_to_idx[node_from]=idx_node_from
                    else:
                        idx_node_from = node_to_idx[node_from]
                        
                    if node_to not in nodes:
                        nodes.add(node_to)
                        idx_node_to = self.idx_node(node_to)
                        idx_to_node[idx_node_to]=node_to
                        node_to_idx[node_to]=idx_node_to
                    else:
                        idx_node_to = node_to_idx[node_to]
                    
                    od = (idx_node_from,idx_node_to)    
                    if odm.has_key(od):
                        odm[od] += 1
                    else:
                        odm[od] = 1
        
        # multipy demand by c_demand 
        self.n_trips = 0
        for od,f in  odm.iteritems():
            n_add = int(self.c_demand *float(odm[od])+0.5)
            odm[od]= n_add
            self.n_trips += n_add
            
        # show 
        #for idx, node in  idx_to_node.iteritems():
        #    print ' %d->%s '%(idx,node.getID())
        
        #for (o,d),f in  odm.iteritems():
        #    print ' d(%d,%d) = %.2f '%(o,d,f)
            
        self._odm = odm
        self._idx_to_node = idx_to_node
        self._node_to_idx = node_to_idx
        
         
    def idx_node(self, node):
        return self._net.nodestab.get_ind_from_key(node.getID())  + 1
    
    def idx_edge(self, edge):
        return self._net.edgestab.get_ind_from_key(edge.getID())  + 1 
        
    #def export_edges(self, net,filepath, filepath_edgeconv,filepath_nodeconv):
        ##$l_{ij}$ $v_{lim,ij}=v_{ij}$ e $v_{crit}$
        #edges = net.getEdges()
        #nodes = net.getNodes()
        ##nodestab = net.nodestab
        #fd = open(filepath, 'w')
        #file_edgeconv = open(filepath_edgeconv, 'w')
        
        #fd.write(str(len(edges))+'\n')
        #fd.write(str(len(nodes))+'\n')    
        #for ident_edge,edge in edges.iteritems():
            #ind_edge = net.edgestab.get_ind_from_key(ident_edge)
            #ident_node_from = edge.getFromNode().getID()
            #ind_node_from = net.nodestab.get_ind_from_key(ident_node_from)
            #ident_node_to = edge.getToNode().getID()
            #ind_node_to = net.nodestab.get_ind_from_key(ident_node_to)
            #l = edge.getLength()
            #v = edge.getSpeed()
            #v_crit = get_v_crit(a,L)
            
            
            #row = '%d\t%d\t%d\t%.4f\t%.4f\t%.4f'%(ind_edge+1,ind_node_from+1,ind_node_to+1,l,v,v_crit)
            #print row
            #fd.write(row+'\n')
            #row = '%d,%s'%(ind_edge, ident_edge)
            #file_edgeconv.write(row+'\n')
            
        #fd.close()
        #file_edgeconv.close()
        
        #file_nodeconv = open(filepath_nodeconv, 'w')
        #ids_nodes =net.nodestab.get_ids()
        #inds_nodes = net.nodestab.get_inds(ids_nodes)
        #idents = net.nodestab.get_keys_from_ids(ids_nodes)
        #for i in xrange(len(ids_nodes)):
            #row = '%d,%s'%(inds_nodes[i]+1, idents[i]+1)
            #file_nodeconv.write(row+'\n')
        
        #file_nodeconv.close()   
        
    
        
##def cleanUp(self):
##    print 'cleanUp'
##    self.eliminateDummyNodes()
##    self.removeLoops()
##    self.eliminateUnreachableNodes()
##    self.eliminateUnescapableNodes()
##    self.eliminateMultipleEdges()
##    self.adjust_edgeshapes()
##    
##def adjust_edgeshapes(self, dist_clip = 5.0):
##    for node in self.getNodes().itervalues():
##        coord_node = np.array(node.getCoord()) 
##        edges_in = node.getIncoming()
##        for edge in edges_in:
##            shape = edge.getShape()
##            coord_end = np.array(shape[-1]) 
##            if np.sqrt(np.sum((coord_end-coord_node)**2))>dist_clip:
##                shape[-1] = tuple(coord_node)
##                edge.setShape(shape)
##        edges_out = node.getOutgoing()
##        for edge in edges_out:
##            shape = edge.getShape()
##            coord_start = np.array(shape[0]) 
##            if np.sqrt(np.sum((coord_start-coord_node)**2))>dist_clip:
##                shape[0] = tuple(coord_node)
##                edge.setShape(shape)
##        
##
##def eliminateUnreachableNodes(self):
##    print 'eliminateUnreachableNodes'
##    have_eliminated = True
##    nodes_rm = []
##    while have_eliminated:
##        have_eliminated = False
##        for node in self.getNodes().itervalues():
##            if len(node.getIncoming())==0:
##                edges_out = node.getOutgoing()
##                for edge in edges_out:
##                    self.removeEdge(edge)
##                self.removeNode(node)
##                have_eliminated = True
##                break
##                #nodes_rm.append(node)  
##        
##    #for node in  nodes_rm: 
##    #    # try to remove node if no longer connected
##    #    self.removeNode(node)
##    
##def eliminateUnescapableNodes(self):
##    print 'eliminateUnescapableNodes'
##    have_eliminated = True
##    #nodes_rm = []
##    while have_eliminated:
##        have_eliminated = False
##        for node in self.getNodes().itervalues():
##            if len(node.getOutgoing())==0:
##                edges_in = node.getIncoming()
##                for edge in edges_in:
##                    self.removeEdge(edge)
##                self.removeNode(node)
##                have_eliminated = True
##                #nodes_rm.append(node) 
##                break 
##        
##    #for node in  nodes_rm: 
##    #    # try to remove node if no longer connected
##    #    self.removeNode(node)
##
##        
##def eliminateDummyNodes(self):
##    print 'eliminateDummyNodes'
##    #  join edjes and eliminate node with one entrance and one exit
##    have_joined = True
##    nodes_rm = []
##    while have_joined:
##        have_joined = False
##        for node in self.getNodes().itervalues():
##            edges_in = node.getIncoming()
##            edges_out = node.getOutgoing()
##            n_in = len(edges_in)
##            n_out = len(edges_out)
##            if (n_in==1)&(n_out==1):
##                self.join_edges(edges_in[0],edges_out[0])
##                # this will remove nodes and edges 
##                # and we need to start all over again
##                have_joined = True
##                break
##    
##    
##def eliminateMultipleEdges(self):
##    for node in self.getNodes().itervalues():
##        edges_out = node.getOutgoing()
##        
##        nodes_out =[]
##        for edge in edges_out:
##            nodes_out.append(edge.getToNode())
##        
##        
##        edges_rm = []
##        for edge in edges_out:
##            if nodes_out.count(edge.getToNode())>1:
##                nodes_out.remove(edge.getToNode())
##                edges_rm.append(edge)
##                
##        
##        for edge in edges_rm:
##            self.removeEdge(edge)
##    
##    
##def removeLoops(self):
##    print 'removeLoops: remove double arcs pointing to the same node'
##    nodes_rm = []
##    for node in self.getNodes().itervalues():
##        edges_out = node.getOutgoing()
##        edges_rm = []
##        for edge in edges_out:
##            if edge.getToNode() == node:
##                edges_rm.append(edge)
##        
##        for edge in edges_rm:
##            self.removeEdge(edge)
##        
##        nodes_rm.append(node)  
##        
##    for node in  nodes_rm: 
##        # try to remove node if no longret connected
##        self.removeNode(node)
##
##            
##            
##  
##def removeEdge(self,edge):
##    tonode=edge.getToNode()
##    tonode.removeIncoming(edge)
##    fromnode=edge.getFromNode()
##    fromnode.removeOutgoing(edge)
##    
##    self.edgestab.del_row(edge.getID())
##    
##def removeNode(self,node):
##    edges_in = node.getIncoming()
##    edges_out = node.getOutgoing() 
##    if (len(edges_in)==0)&(len(edges_out)==0):
##        self.nodestab.del_row(node.getID())
##           
##def join_edges(self,edge1,edge2):
##    #print 'join_edges',edge1.getID(),edge2.getID()
##    node1 = edge1.getToNode()
##    node2 = edge2.getToNode()
##    
##    node1.removeIncoming(edge1)
##    node1.removeOutgoing(edge1)
##    self.removeNode(node1)
##    
##    edge1.setToNode(node2)
##    
##    node2 = edge2.getToNode()
##    node2.removeIncoming(edge2)
##    node2.addIncoming(edge1)
##    
##    lanes1 = edge1.getLanes()
##    lanes2 = edge2.getLanes()
##    i=0
##    for lane in lanes1:
##        
##        shape = lane.getShape()
##        #print '  shape =',lane.getShape()
##        if i<len(lanes2):
##            shape += lanes2[i].getShape()
##        else:
##            shape += lanes2[-1].getShape()
##        #print '  shape =',lane.getShape()
##        
##        lane.recalcLength()
##    
##    edge1.rebuildShape()
##    
##    self.removeEdge(edge2)