# cd /home/joerg/projects/sumopy/tools/sumopy/lib
# python classmanager.py

## To be or not to be.  -- Shakespeare 
## To do is to be.  -- Nietzsche 
## To be is to do.  -- Sartre 
## Do be do be do.  -- Sinatra 

# save with is saved flag
# xml mixin
# different attrconfig classe (numbers, strings, lists, colors,...)

import types, os, pickle, sys, string
import numpy as np
import xmlmanager as xm
from datetime import datetime

# oh my god....
import platform
global IS_WIN
if platform.system()=='Windows':
    IS_WIN = True
else:
    IS_WIN = False
 
# event triggers
#       plugtype    plugcode
EVTDEL =  0 # delete attribute 
EVTSET =  1 # set attribute
EVTGET =  2 # get attribute
EVTADD =  3 # add/create attribute

EVTDELITEM = 20 # delete attribute 
EVTSETITEM = 21 # set attribute
EVTGETITEM = 22 # get attribute
EVTADDITEM = 23 # add/create attribute


      
        
        

def filepathlist_to_filepathstring(filepathlist, sep=',',is_primed = False):
    if IS_WIN & is_primed:
        p = '"'
    else:
        p=''
    #print 'filepathlist_to_filepathstring',IS_WIN,p,filepathlist
    if type(filepathlist)==types.ListType:
        if len(filepathlist) == 0:
            return ''
        else:
            filepathstring = ''
            for filepath in filepathlist[:-1]:
                fp = filepath.replace('"','')
                filepathstring += p+fp+p+sep
            filepathstring += p+filepathlist[-1]+p
            return filepathstring
    else:
        fp = filepathlist.replace('"','')
        return p+filepathlist+p

def filepathstring_to_filepathlist(filepathstring, sep=',', is_primed = False):
    if IS_WIN & is_primed:
        p='"'
    else:
        p=''
    filepaths=[]
    for filepath in filepathstring.split(sep):
        filepaths.append(p+filepath.strip().replace('"','')+p)
    return filepaths



class PuginMixin:
    
    def _init_plugin(self,attrname,**attrs):
        AttrConf.__init__(self,attrname,**attrs)
        self._events = {}
        self._has_events = False
    
    
    def get(self):
        """
        Default get method with plugin for scalar attrs
        """
        #  return attribute, overridden with indexing for array and dict struct
        self.exec_events(EVTGET)
            
        return getattr(self._obj, self.attrname)
      
    def set(self, value):
        """
        Default set method with plugin for scalar attrs
        """
        # set attribute, overridden with indexing for array and dict struct
        if value != getattr(self._obj, self.attrname):
            setattr(self._obj, self.attrname, value)
            self.exec_events(EVTSET)
        return value 
    
    
        
    def add_event(self,trigger,function):
        """
        Standard plug types are automatically set but the system:

        """
        if not self._events.has_key(trigger):
            self._events[trigger] = []
        self._events[trigger].append(function) 
        self._has_events = True    

    def del_event(self,trigger):
        del self._events[trigger]
        if len(self._events)==0:
            self._has_events = False
    
    
    
    
        
    def exec_events(self,trigger):
        if self._has_events: 
            #print '**PuginMixin.exec_events',trigger,(EVTGETITEM,EVTGET)
            #if trigger!=EVTGET:
            #    print '  call set_modified',self._obj
            #    self._obj.set_modified(True)
            
            for function in self._events.get(trigger,[]): 
                function(self)
        

    
class NumConf:
    """
    Contains additional information on the object's attribute.
    """
    def __init__(self, attrname, default = None,
                    struct = 'scalar', metatype = '', 
                    groupnames = [], perm='rw', 
                    is_save = True, is_link = False,is_copy = True,
                    name = '', info = '', unit = '',
                    digits_integer = None, digits_fraction=None ,
                    minval = None, maxval = None,
                    **attrs):

        
        self.attrname = attrname
        self.groupnames = groupnames
        self.metatype = metatype
        
        self._default = default
        self.struct = struct
        
        self._is_save = is_save
        self._is_link = is_link
        self._is_copy = is_copy
        
        self.unit = unit
        self.info = info
        self.name = name 
        self._perm = perm
        
        # aux defaults which may be overwritten by args
        
        self.min = minval
        self.max = maxval
        self.digits_integer = digits_integer
        if type(default) in (types.IntType, types.LongType):
            self.digits_fraction = 0
        else:
            self.digits_fraction = digits_fraction
        
        
class AttrConf:
    """
    Contains additional information on the object's attribute.
    """
    def __init__(self, attrname, default = None,
                    groupnames = [], perm='rw', 
                    is_save = True, 
                    #is_link = False, # define link class
                    is_copy = True,
                    name = '', info = '', 
                    unit = '',
                    struct = 'scalar', # depricated, metatype is class
                    metatype = '', # depricated, metatype is class
                    **attrs):

        
        self.attrname = attrname
        self.groupnames = groupnames
        self.metatype = metatype
        
        self._default = default
        self.struct = struct
        
        self._is_save = is_save
        #self._is_link = is_link
        self._is_copy = is_copy
        
        self._unit = unit
        self.info = info
        self.name = name 
        self._perm = perm
        
        # set rest of attributes passed as keyword args
        # no matter what they are used for
        for attr, value in attrs.iteritems():
            setattr(self,attr,value)
        
       
            
    def set_manager(self, manager):
        """
        Method to set manager to attribute configuration object.
        This is either attribute manager or table manager.
        Used by add method of AttrManager
        """
        self._manager = manager
        
     
        
    def get_manager(self):
        """
        Method to get manager to attribute configuration object.
        """
        return self._manager 
           
    def set_obj(self, obj):
        """
        Method to set instance of managed object.
        Used by add method of AttrManager
        """
        self._obj = obj  
    
    def get_obj(self):
        return self._obj
        
    def get_attr(self):
        # always return attribute, no indexing
        
        return getattr(self._obj, self.attrname)
    
    
    def set_attr(self, value):
        #set entire attribute, no indexing
        
        return setattr(self._obj, self.attrname, value)    

    def get(self):
        #  return attribute, overridden with indexing for array and dict struct
        return getattr(self._obj, self.attrname)
      
    def set(self, value):
        # set attribute, overridden with indexing for array and dict struct
        setattr(self._obj, self.attrname, value)   
        return value 

    def get_default(self):
        return self._default
    
    def get_init(self):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        return self.get_default()
    
    def is_tableattr(self):
        return self.struct in ('dict','array','list')
    
    def set_perm(self, perm):
        self._perm = perm
         
    def get_perm(self):
        return self._perm
    
    def is_readonly(self):
        return 'w' not in self._perm
     
    def is_writable(self):
        return 'w' in self._perm   
    
    def is_editable(self):
        """Can attribute be edited """
        return 'e' in self._perm
    
    def is_save(self):
        return self._is_save
    
    def has_unit(self):
        return self.unit != ''
    
    def has_info(self):
        return self.info != ''
    
    def get_info(self):
        return self.info
       
    def format_unit(self, show_parentesis=False):
        if self.unit in ('',None):
            return ''
        if show_parentesis:
            return '[%s]'%self.unit
        else:
            return '%s'%self.unit
    
    def get_printformat(self, val = None):
        if val == None:
            tt = type(self.get_default())
        else:
            tt = type(val)
        if tt in (types.IntType, types.LongType):
            if self.digits_integer != None:
                return '%'+str(self.digits_integer)+'d'
            else:
                return '%d'
        
        elif tt in (types.FloatType,types.ComplexType):
            if self.digits_integer != None:
                format_digits_integer=str(self.digits_integer)
            else:
                format_digits_integer=''
            if self.digits_fraction != None:
                format_digits_fraction='.'+str(self.digits_fraction)
            else:
                format_digits_fraction=''
            return '%'+format_digits_integer+format_digits_fraction+'f'
        
        else:
            # its a string
            return '%s'
        
    def get_printformat_vec(self):
        """
        NOT IN USE!
        Returnes the standad fortran stringformat for attribute.
        """
        default = self.get_default()
        if type(default) in (types.TupleType,np.ndarray,types.ListType,types.XRangeType):
            if len(default)==0:
                return None
            else:
                format = self._get_printformat_scalar(default[0])
                for d in default[1:]:
                    format+=', '+self._get_printformat_scalar(d)
                return format
        else:
             return self._get_printformat_scalar(default)
        
       
            
        
    
    def format_value(self, show_unit = False, show_parentesis=False):
        if show_unit:
            unit = ' '+self.format_unit(show_parentesis)
        else:
            unit = ''
        #print '\n\n format_value'
        #print '  printformat',self.get_printformat()
        #print '  get_attr',self.get_attr(),type(self.get_attr())
        val = self.get_attr()   
        if type(val) in (types.TupleType,np.ndarray,types.ListType,types.XRangeType):
            return repr(val)+unit
        else:
            return self.get_printformat(val)%val+unit
                
    def format(self):
        return '%s = %s\n'%(self.attrname, self.format_value(show_unit = True))
    
    

        
    def  __getstate__(self):
        #print '__getstate__',self.ident
        #print '  self.__dict__=\n',self.__dict__.keys()
        
        state={} 
        for attr in self.__dict__.keys():
            if attr not in ['_events','_plugin']:
                state[attr] = self.__dict__[attr]
        return state
    
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        self._plugins={}
        
        for key in state.keys():
            #print '  set state',key
            self.__dict__[key]=state[key]
        self._events = {}
        
class PuginAttrconf(AttrConf):
    
    def get(self):
        #  return attribute, overridden with indexing for array and dict struct
        if self._plugin != None:
            self._plugin.exec_events(EVTGET)
            
        return AttrConf.get(self._obj, self.attrname)
      
    def set(self, value):
        # set attribute, overridden with indexing for array and dict struct
        setattr(self._obj, self.attrname, value)   
        if self._plugin != None:
            self._plugin.exec_events(EVTSET)
        return value 
        
    def add_event(self,trigger,function):
        """
        Standard plug types are automatically set but the system:

        """
        if not self._events.has_key(trigger):
            self._events[trigger] = []
        self._events[trigger].append(function) 
        self._has_events = True    

    def del_event(self,trigger):
        del self._events[trigger]
        if len(self._events)==0:
            self._has_events = False
    
    
    
    
        
    def exec_events(self,trigger):
        if self._has_events: 
            #print '**PuginMixin.exec_events',trigger,(EVTGETITEM,EVTGET)
            #if trigger!=EVTGET:
            #    print '  call set_modified',self._obj
            #    self._obj.set_modified(True)
            
            for function in self._events.get(trigger,[]): 
                function(self.parent)
        
              
class FuncConf(AttrConf):
    def __init__(self, attrname, default, function,
                    groupnames = [],  
                    metatype = '', 
                    name = '', info = '', unit = '',
                    **kwargs):
        self._function =  function               
        AttrConf.__init__(self, attrname, default,
                    groupnames = groupnames, perm='r', is_save = False,
                    metatype = metatype, struct = 'scalar',
                    name = name, info = info, unit = unit,
                    **kwargs)
                    
    def get(self):
        #  return  function value 
        self.exec_events(EVTGET)
        return self._function()
      
    def set(self, value):
        # read only, do nothing
        return None
    
    def get_default(self):
        # return function value, not function
        return self._default
    
    def get_function(self):
        # return function 
        return self._function
    
    def format_value(self, show_unit = False, show_parentesis=False):
        if show_unit:
            unit = ' '+self.format_unit(show_parentesis)
        else:
            unit = ''
        #print '\n\n format_value'
        #print '  printformat',self.get_printformat()
        #print '  get_attr',self.get_attr(),type(self.get_attr())
        val = self._function()   
        if type(val) in (types.TupleType,np.ndarray,types.ListType,types.XRangeType):
            return repr(val)+unit
        else:
            return self.get_printformat(val)%val+unit

    def get_attr(self):
        # returns value or entire array, 
        # no indexing . this is the difference to the get method 
        return self._function()#self.format_value()
    
    
class ArrayConf(AttrConf):
    def __init__(self, attrname, default, 
                    groupnames = [], perm='rw', is_save = True,
                    metatype = '', 
                    name = '', info = '', unit = '', dtype=None,
                    **kwargs):
        self._dtype =  dtype               
        AttrConf.__init__(self, attrname, default,
                    groupnames = groupnames, perm=perm, is_save = is_save,
                    metatype = metatype, struct = 'array',
                    name = name, info = info, unit = unit,
                    **kwargs)
       
    
    def exec_events_keys(self,trigger, keys):
        """
        Executes all functions assigned for this trigger.
        Keys are ids if is_keyindex=False otherwise they are index keys (any object)
        """
        if self._has_events:
            #print '**ArrayConf.exec_events_keys',self.attrname,trigger,(EVTGETITEM,EVTGET)
            #if trigger!=EVTGETITEM:
            #    #print '  call set_modified',self._manager
            #    self._manager.set_modified(True)
                
            for function in self._events.get(trigger,[]): function(self, keys)
            
    def set_manager(self, manager):
        """
        Method to set manager to attribute configuration object.
        This is either attribute or table manager.
        Used by add method of AttrManager
        """
        AttrConf.set_manager(self, manager)
        self._is_keyindex = manager.is_keyindex()
           
                    
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.name,ids
        default = self.get_default()
        
        if hasattr(default, '__iter__'):
            default = np.asarray(default)
            if self._dtype!=None:
                dtype = self._dtype
            else:
                dtype = type(default.flatten()[0])
            #print ' default=',default,len(default)
            if len(ids)>0:
                defaults = np.array(  len(ids)*[default], dtype  )
                #print '  size,type',len(ids)*[default], type(default.flatten()[0])
            else:
                #defaults = np.zeros(  (0,len(default)),type(default.flatten()[0]) )
                defaults = np.zeros(  (0,)+default.shape, dtype )
            #print '  return',defaults,defaults.shape
            return defaults
        else:
            if self._dtype!=None:
                dtype = self._dtype
            else:
                dtype = type(default) 
            return np.array(  len(ids)*[default], dtype )
    
    
    def get_init(self, ids):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        return self.get_defaults(ids)
    
    
    
            
    
    def __delitem__(self, id):   
        #print '        before=\n',self.__dict__[attr]
        #attr = self.attrconf.get_attr()
        arr = self.get_attr()
        i = self._manager._inds[id]
        self.set_attr(np.concatenate( (arr[:i], arr[i+1:]) ) )
            
        #print '        after=\n',self.__dict__[attr]
                
    
    def __getitem__(self, ids):
        #print '__getitem__',key
        return  self.get_attr()[self._manager._inds[ids]] 
    
    def __setitem__(self, ids, values):
        #print '__setitem__',ids,values
        self.get_attr()[self._manager._inds[ids]] = values
    
    # use original one
    # def exec_events(self,trigger, keys=None):
    #    if keys==None:
    #        for function in self._events.get(trigger,[]): function(self)
    #    else:
    #        self.exec_events_keys(trigger, keys)

        
    
                                
    #def get_from_keys(self,keys):
    #    """
    #    Returns values of array element         
    #    wich corresponds to id the given name.
    #    ??? not in use????
    #    """
    #    values = []
    #    for key in keys:
    #        values.append[self.get_from_key(key)]#???not existant
    #    return values
    
    
    def add(self, ids, keys):
        if self._is_keyindex:
            if ids==None:
                ids = self._manager.get_ids_from_keys(keys)
            self.set_attr(np.concatenate((self.get_attr(),self.get_defaults(ids))) )
            self.exec_events_keys(EVTADDITEM,keys)
        else:
            self.set_attr(np.concatenate((self.get_attr(),self.get_defaults(ids))) )
            self.exec_events_keys(EVTADDITEM,ids)
      
            
    def get(self,keys):
        """
        Central function to get the attribute value associated with keys.
        should be overridden by specific array configuration classes
        """
        
        if self._is_keyindex:
            #ind = self._manager._inds[self._manager.get_id_from_key(keys)] 
            #print 'get',self.attrname,self._is_keyindex,keys,hasattr(keys, '__iter__'),self._manager.get_id_from_key(keys),ind
                
            if hasattr(keys, '__iter__'):
                values = []
                for n in keys:
                    values.append(self[self._manager.get_id_from_key(n)])
                self.exec_events_keys(EVTGETITEM,keys)
                return values
            
            else:    
                #print '  =',self[self._manager.get_id_from_key(keys)]
                self.exec_events_keys(EVTGETITEM,[keys])
                return self[self._manager.get_id_from_key(keys)]
        else:
            self.exec_events_keys(EVTGETITEM,keys)
            return  self.__getitem__(keys)
        
    def set(self, keys, values, is_add_row = True):
        """
        Returns value of array element 
        wich corresponds to id the given name.
        
        If is_add_row then table rows will be added is the key
        (which can be ids or keys) do not yet exist.
        
        should be  overridden by specific array configuration classes
        """
        #print 'set',self.attrname
        
        if self._is_keyindex:
            if hasattr(keys, '__iter__'):
                pass
                
            else:
                keys = [keys]
                values = [values]
            
            #print '  keys',keys
            #print '  names',names
            #print '  values',values,type(values)
            i = 0
            for n in keys:
                if is_add_row:
                    if not self._manager.contains(n):
                        self._manager.add_row(key = n)
                self[self._manager.get_id_from_key(n)]=values[i]
                i += 1
            #else:    
            #    self[self._manager.get_id_from_key(keys)]=values
            self.exec_events_keys(EVTSETITEM,keys)
        else:
            if is_add_row:
                if hasattr(keys, '__iter__'):
                    ids = keys
                else:
                    ids = [keys]
                    
                for id in ids:
                    if not self._manager.contains(id):
                        self._manager.add_row(id = id)
            
            self.__setitem__(keys,values)
            self.exec_events_keys(EVTSETITEM,keys)
            
        
    def delete(self,id=None,key=None):
        """
        removes key from array structure
        To be overridden
        """
        if self._is_keyindex:
            if id == None:# only keys are given
                self.exec_events_keys(EVTDELITEM,[key])
                del self[self._manager.get_id_from_key(key)]
                
            else: # ids are given
                if key==None:
                    key=self._manager.get_key_from_id(id)
                self.exec_events_keys(EVTDELITEM,[key])
                del self[id]
                         
        else:
            self.exec_events_keys(EVTDELITEM,[id])
            del self[id]
            

            
    def format_value(self,id, show_unit = False, show_parentesis=False):
        if show_unit:
            unit = ' '+self.format_unit(show_parentesis)
        else:
            unit = ''
        return self.get_printformat()%self[id]+unit

        
    def format(self, ids = None):
        text = ''
        
        if ids == None:
            ids = self._manager.get_ids()
        if not hasattr(ids, '__iter__'):
            ids = [ids]
            
        unit =  self.format_unit()
        attrname = self.attrname
        for id in ids:
            text += '%s[%d] = %s\n'%(attrname, id, self.format_value(id, show_unit = True))
            
        return text[:-1] #remove last newline


class ListConf(ArrayConf):
    def __init__(self, attrname, default, 
                    groupnames = [], perm='rw', is_save = True,
                    metatype = '', name = '', info = '', unit = '',
                    **kwargs):
                        
        AttrConf.__init__(self, attrname, default,
                    groupnames = groupnames, perm=perm, is_save = is_save,
                    metatype = metatype, struct = 'list',
                    name = name, info = info, unit = unit,
                    **kwargs)
    
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        return len(ids)*[self.get_default()]    
    
    def get_init(self, ids):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        # print 'get_init',ids,self.get_defaults(ids)
        return self.get_defaults(ids)
    
    def get(self,keys):
        """
        Central function to get the attribute value associated with keys.
        should be overridden by specific array configuration classes
        """
        if self._is_keyindex:
            if hasattr(keys, '__iter__'):
                values = []
                i = 0
                for n in keys:
                    values.append(self[self._manager.get_id_from_key(n)])
                    i += 1
                self.exec_events_keys(EVTGETITEM,keys)
                return values
            
            else:    
                self.exec_events_keys(EVTGETITEM,[keys])
                return self[self._manager.get_id_from_key(keys)]
        else:
            self.exec_events_keys(EVTGETITEM,keys)
            return  self.__getitem__(keys)
        
    def set(self, keys, values, is_add_row = True):
        """
        Returns value of array element 
        wich corresponds to id the given name.
        
        If is_add_row then table rows will be added is the key
        (which can be ids or keys) do not yet exist.
        
        should be  overridden by specific array configuration classes
        """
        #print 'set',self.attrname
        
        if self._is_keyindex:
            if hasattr(keys, '__iter__'):
                pass
                
            else:
                keys = [keys]
                values = [values]
            
            #print '  keys',keys
            #print '  names',names
            #print '  values',values,type(values)
            i = 0
            for n in keys:
                if is_add_row:
                    if not self._manager.contains(n):
                        self._manager.add_row(key = n)
                self[self._manager.get_id_from_key(n)]=values[i]
                i += 1
            #else:    
            #    self[self._manager.get_id_from_key(keys)]=values
            self.exec_events_keys(EVTSETITEM,keys)
        else:
            if is_add_row:
                if hasattr(keys, '__iter__'):
                    ids = keys
                else:
                    ids = [keys]
                    
                for id in ids:
                    if not self._manager.contains(id):
                        self._manager.add_row(id = id)
            
            self.__setitem__(keys,values)
            self.exec_events_keys(EVTSETITEM,keys)
            
    def add(self,ids, keys=None):
        self.set_attr(self.get_attr() + self.get_defaults(ids) )
        if keys!=None:
            self.exec_events_keys(EVTADDITEM,keys)
        else:
            self.exec_events_keys(EVTADDITEM,ids)    
        
    #def delete(self,ids=None,keys=None):
    #    """
    #    removes key from array structure
    #    To be overridden
    #    """
    #    if self._is_keyindex:
    #        if ids == None:# only keys are given
    #            if hasattr(keys, '__iter__'): keys = [keys]
    #            self.exec_events_keys(EVTDELITEM,keys)
    #            del self[self._manager.get_ids_from_keys(keys)]
    #            
    #        else: # ids are given
    #            if hasattr(ids, '__iter__'): ids = [ids]
    #            
    #            if keys==None:
    #                keys=self._manager.get_keys_from_ids(ids)
    #            self.exec_events_keys(EVTDELITEM,keys)
    #            del self[ids]
    #                     
    #    else:
    #        self.exec_events_keys(EVTDELITEM,ids)
    #        del self[ids]
            
        
     
    def __getitem__(self, ids):
        #print '__getitem__',key
        lis = self.get_attr()
        if hasattr(ids, '__iter__'):
            values = []
            for i in self._manager._inds[ids]:
                values.append(lis[i])
            return values
        else:
            return lis[self._manager._inds[ids]]

    
    def __setitem__(self, ids, values):
        #print '__getitem__',key
        lis = self.get_attr()
        if hasattr(ids, '__iter__'):
            for i in self._manager._inds[ids]:
                lis[i] = values[i]      
        else:
            lis[self._manager._inds[ids]] = values
        
    def __delitem__(self, id):
        #print '__delitem__',key
        del self.get_attr()[self._manager._inds[id]]

           
class DictConf(ArrayConf):
    def __init__(self, attrname, default, 
                    groupnames = [], perm='rw', is_save = True,
                    metatype = '', name = '', info = '', unit = '',
                    **kwargs):
                        
        AttrConf.__init__(self, attrname, default,
                    groupnames = groupnames, perm=perm, is_save = is_save,
                    metatype = metatype, struct = 'dict',
                    name = name, info = info, unit = unit,
                    **kwargs)
    
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        return len(ids)*[self.get_default()]    
    
    def get_init(self, ids):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        values = self.get_defaults(ids)
        dic = {}
        i = 0
        for id in ids:
            dic[id] = values[i]
        return dic
    
    def get(self,keys):
        """
        Central function to get the attribute value associated with keys.
        should be overridden by specific array configuration classes
        """
        if self._is_keyindex:
            if hasattr(keys, '__iter__'):
                values = []
                i = 0
                # TODO: use __getitem__ and get_ids_from_keys
                for n in keys:
                    values.append(self[self._manager.get_id_from_key(n)])
                    i += 1
                self.exec_events_keys(EVTGETITEM,keys)
                return values
            
            else:    
                self.exec_events_keys(EVTGETITEM,[keys])
                return self[self._manager.get_id_from_key(keys)]
        else:
            self.exec_events_keys(EVTGETITEM,keys)
            return  self.__getitem__(keys)
        
    def set(self, keys, values, is_add_row = True):
        """
        Returns value of array element 
        wich corresponds to id the given name.
        
        If is_add_row then table rows will be added is the key
        (which can be ids or keys) do not yet exist.
        
        should be  overridden by specific array configuration classes
        """
        #print 'set',self.attrname
        
        if self._is_keyindex:
            if hasattr(keys, '__iter__'):
                pass
                
            else:
                keys = [keys]
                values = [values]
            
            #print '  keys',keys
            #print '  names',names
            #print '  values',values,type(values)
            i = 0
            for n in keys:
                if is_add_row:
                    if not self._manager.contains(n):
                        self._manager.add_row(key = n)
                self[self._manager.get_id_from_key(n)]=values[i]
                i += 1
            #else:    
            #    self[self._manager.get_id_from_key(keys)]=values
            self.exec_events_keys(EVTSETITEM,keys)
        else:
            if is_add_row:
                if hasattr(keys, '__iter__'):
                    ids = keys
                else:
                    ids = [keys]
                    
                for id in ids:
                    if not self._manager.contains(id):
                        self._manager.add_row(id = id)
            
            self.__setitem__(keys,values)
            self.exec_events_keys(EVTSETITEM,keys)
    
    def add(self,ids, keys=None):
        self[ids] = self.get_defaults(ids)
        
        if keys!=None:
            self.exec_events_keys(EVTADDITEM,keys)
        else:
            self.exec_events_keys(EVTADDITEM,ids)
                     
    def __getitem__(self, ids):
        #print '__getitem__',key
        dic = self.get_attr()
        if self._is_keyindex:
            # when keyindexed then use key to store data in dictionaries, 
            # could be speeded up in get method
            if hasattr(ids, '__iter__'):
                values = []
                for id in ids:
                    values.append(dic[self._manager.get_key_from_id(id)])
                return values
            else:
                return dic[self._manager.get_key_from_id(ids)]
        else:
            if hasattr(ids, '__iter__'):
                values = []
                for id in ids:
                    values.append(dic[id])
                return values
            else:
                return dic[ids]




    
    def __setitem__(self, ids, values):
        #print '__getitem__',key
        dic = self.get_attr()
        if self._is_keyindex:
            # when keyindexed then use key to store data in dictionaries, 
            # could be speeded up in get method
            if hasattr(ids, '__iter__'):
                i = 0
                for id in ids:
                    dic[self._manager.get_key_from_id(id)] = values[i]      
            else:
                dic[self._manager.get_key_from_id(ids)] = values
        else:
            if hasattr(ids, '__iter__'):
                i = 0
                for id in ids:
                    dic[id] = values[i]      
            else:
                dic[ids] = values
       
    def __delitem__(self, id):
        #print '__delitem__',key
        if self._is_keyindex:
            del self.get_attr()[self._manager.get_key_from_id(id)]    
        else:
            del self.get_attr()[id]
                    
class AttrsManager(PuginMixin):
    """
    Manages all attributes of an object
    """
    def __init__(self, obj, attrname):
        
        self._obj = obj # managed object
        self._attrconfigs = []  # managed attribute config instances
        self.attrname = attrname # the manager's attribute name in the obj instance
        # groupes of attributes 
        # key=groupname, value = list of attribute config instances
        self._groups = {} 
        self._init_plugin()
    
    def exec_events(self,trigger, attrconfig):
        if self._has_events: 
            #if trigger!=EVTGET:
            #    print '  call set_modified',self._obj
            #    self._obj.set_modified(True)
            #print '**AttrsManager.exec_events NOMOD',attrconfig.attrname,trigger
            # overridden plugin default because we pass the attrconfig
            for function in self._events.get(trigger,[]): function(attrconfig)
        
    def has_attrname(self,attrname):
        # TODO: very inefficient, create hash table for attribute names
        #ans = False
        #for attrconfig in self._attrconfigs:
        #    ans = ans | (attrconfig.attrname == attrname)
        #return ans
        
        # attention this is a trick, exploiting the fact that the
        # attribute object with all the attr info is an attribute
        # of the attr manager (=self) 
        return hasattr(self,attrname)
        
    def get_configs(self, is_all = False):
        if is_all:
            return self._attrconfigs
        else:
            attrconfigs = []
            for attrconf in self._attrconfigs:
                if len(attrconf.groupnames)>0:
                   if attrconf.groupnames[0]!='_private':
                        attrconfigs.append(attrconf)
                else:
                    attrconfigs.append(attrconf)
                 
            return attrconfigs
        
    def get_obj(self):
        return self._obj
            
    def add(self, attrconf):
        """
        Add a one or several new attributes to be managed.
        kwargs has attribute name as key and Attribute configuration object
        as value.
        """  
        #for attrname, attrconf in attrconfs.iteritems():
        
        attrname = attrconf.attrname
        attrconf.set_obj(self._obj)
        attrconf.set_manager(self)
        
        # set configuration object sa attribute of AttrManager
        setattr(self, attrname, attrconf)
        
        # append also to the list of managed objects
        self._attrconfigs.append(attrconf)
        
        # insert in groups
        if len(attrconf.groupnames) > 0:
            for groupname in attrconf.groupnames:
                
                if not self._groups.has_key(groupname):
                    self._groups[groupname]=[]
                
                self._groups[groupname].append(attrconf)
            
        
        self.exec_events(EVTADD, attrconf)
            
        # return default value as attribute of managed object 
        if attrconf.struct == 'scalar':  
            return attrconf.get_init() 
        else:
            return None # table configs do their own init
        


        
    def get_groups(self):
        return self._groups
    
    def get_groupnames(self):
        return self._groups.keys()
    
    def has_group(self, groupname):
        return self._groups.has_key(groupname)
        
    def get_group(self,name): 
        """
        Returns a list with attributes that belong to that group name.
        """
        #print 'get_group self._groups=\n',self._groups.keys()
        return self._groups.get(name,[])

    def get_group_attrs(self,name):
        """
        Returns a dictionary with all attributes of a group. 
        Key is attribute name and value is attribute value. 
        """
        #print 'get_group_attrs', self._groups
        attrs={}
        for attrconf in self._groups[name]:
            attrs[attrconf.attrname]=getattr(self._obj, attrconf.attrname)
        return attrs
    
    def print_attrs(self, show_unit = True, show_parentesis=False):
        print 'Attributes of Table',self._obj.name,'(ident=%s)'%self._obj.ident
        for attrconf in self._attrconfigs:
            if not '_private' in attrconf.groupnames:
                print '  %s =\t %s'%(attrconf.attrname, attrconf.format_value(show_unit=True))       
           
                

    def delete(self, attrname):
        """
        Delete attibite with respective name
        """
        #print '.__delitem__','attrname=',attrname
        
        if hasattr(self,attrname): 
            attrconf = getattr(self,attrname)
            self.exec_events(EVTDEL, attrconf)
            if attrconf in self._attrconfigs:
                for groupname in attrconf.groupnames:
                        self._groups[groupname].remove(attrconf)
                        
                self._attrconfigs.remove(attrconf)
                del self.__dict__[attrname]
                del self._obj.__dict__[attrname]
                return True
            return False # attribute not managed
        return False # attribute not existant
     
    #def __delattr__(self, attr) :
    #    print '.__delattr__','attr=',attr      
       

class RowObj:
    """
    UNDER CONSTRUCTION, NOT USED
    This Class is to create an instance of a table row on the fly.
    
    The attributes, which can be read and written represent
    the columns of this row.
    
    Special attribute id  the ID of this row.
    
    """
    def __init__(self, tab, id, ident,**args):
        """
        Initialize by defining parent instance where all the
        attributes of this instance are stored in array form.
        Also define the id of the array coresponding to this instance. 

        Function will be called automatically from ArrayObjConfigurator
        when id is assigned.
        
        """
        self._tab=tab
        #if ident=='':
        #    ident=parent.ident+IDENTSEP+str(id)
        #else:
        self.ident=ident
        self.id=id
        self._coreattrs = ('setparent','__setattr__','__getattr__','getparent','id','ident','tab')
    

    def __getitem__(self,attr):
        return self.getparent(attr)
    
    def __setitem__(self,attr,value):
        self.setparent(attr,value)        
    
    #def __getattr__(self,attr):
    #    if attr in self._tab._attrs:
    #        return self.getparent(attr)
    #    else:
    #        return getattr(self, attr)
    #
    #def __setattr__(self,attr,value):
    #    if attr in self._tab._attrs:
    #        self.setparent(attr,value)  
    #    return setattr(self, attr, value)    
        
    def setparent(self,attr,value):
        """
        Sets entry with id of this instance of the attribute 'key' of parent object.
           
        """
        # print key,value
        getattr(self._tab, attr)[self.id]=value
        
        
    def getparent(self,attr):
        """
        Gets entry with id of this instance of the attribute in parent object.
        
        Usage:
            self['attributename']  
        """
        #print 'ArrayObject.__getitem__',key
        return getattr(self._tab, attr)[self.id]
    
    #def get_groupattrs_obj(self,name):
    #    """
    #    Returns a dictionary with all attributes of a group with given name. 
    #    Key is attribute name and value is attribute value for the given id. 
    #    """
    #    #print 'get_groupattrs_obj',name,self.id
    #    return self.parent.get_groupattrs(name,self.id)
 


class BasicManagerMixin:
    """
    Basic management methods to be inherited by all other managers.
    """
    def _init_objman(self, ident='no_ident', parent=None, name= None):
        self.managertype = 'basic'
        self.ident = ident                
        if name == None:
            self.name = ident
        else:
            self.name = name
        
        self._is_modified = False
        
    
    
        self._tablemans = []
        self.reset_parent(parent)
        
    def set_modified(self, is_modified):
        print '**set_modified',self.ident
        if is_modified & (not self._is_modified):
            self._is_modified = is_modified
            if (self.parent!=None)&is_modified:
                self.parent.set_modified(True)
        else:
            self._is_modified = is_modified
        
    def is_modified(self):
        return self._is_modified
        
    def get_name(self):
        return self.name

    def get_ident(self):
        return self.ident
    
    def format_ident(self):
        return str(self.ident)
    
    def get_root(self):
        if self.parent != None:
            return self.parent.get_root()
        else:
            return self
        
    def get_ident_abs(self):
        """
        Returns absolute identity.
        This is the ident of this object in the global tree of objects.
        If there is a parent objecty it must also be managed by the 
        object manager.
        """ 
        #print 'obj.get_ident_abs',self.ident,self.parent, type(self.parent)
        if self.parent != None:
            return self.parent.get_ident_abs()+[self.ident]
        else:
            return [self.get_ident()]
        
    def format_ident_abs(self):
        s = ''
        #print 'format_ident_abs',self.get_ident_abs()
        for ident in self.get_ident_abs():
            s += str(ident)+'.'
        return s[:-1]
    
    def get_parent(self):
        return self.parent
        
    def reset_parent(self, parent):
        self.parent=parent

    def set_attrman(self, attrman):
        # for quicker acces and because it is only on
        # the attribute management is public and also directly accessible
        #setattr(self, attrname,AttrsManager(self))# attribute management
        self._attrmanname = attrman.attrname
        return attrman
        
    def get_attrman(self):
        return getattr(self, self._attrmanname)
    
    def add_tableman(self, tableman):
        self._tablemans.append(tableman)
        return tableman
        
    def get_tablemans(self):
        return self._tablemans
        
class ArrayData:
    """
    Empty container used as default to store column data of tables
    """
    def __init__(self, table=None):
        self._table = table
        

class TableEntry:
    def __init__(self, table, id):
        self.table = table
        self.id = id
    
    def __getattr__(self, attrname):
        def method(*args,**kwargs):
                #print("tried to handle unknown method " + attrname)
                getattr(self.table,attrname)(self.id,*args,**kwargs)
        return method 
    
        
class TableManager(AttrsManager,BasicManagerMixin):
    """
    This class is for managing a table within a class.
    In this case specified indexable attributes are columns
    and their entries are the rows.
    
    An element of the table can be called with:
        obj.attrs.attr[id]
    where attr id the attribute's name and id is the row id.
    """
    def __init__(   self,  ident='table', parent = None, name = None, 
                    obj=None, is_keyindex= False, **kwargs ):
        """
        Table manager allows to define arrays and dictionaries
        the will be treated as columns of one and the same table.
        Table column data are kept as attribute with column names of 
        in obj instance.
        """
        
        
        if obj==None:
            obj = ArrayData(self)
            self._is_coldata = True
            
        else:
            self._is_coldata = False
            #BasicManagerMixin._init_objman(self, ident=obj.ident, parent=obj.parent, name= obj.name)
        
        # these makes it a full object with attribute management
        # and itself as trhe only table
        BasicManagerMixin._init_objman(self, ident=ident, parent=parent, name= name)
        self.managertype = 'table'
        self.attrs = self.set_attrman(AttrsManager(self,'attrs'))
        self.add_tableman(self)
        
        # publicly accessible column instance
        # where attributes corrispond to table columns
        # columns are either numpy arrays, lists or dictionaries      
        # (this is the same as the self._obj)
        self.cols = obj # 
        
        
        
        AttrsManager.__init__(self, obj,'_attrs',**kwargs )


        self._is_keyindex = is_keyindex
        
        #print 'check config'#,self.ident,self._config
        self._arrayconfigs=[]
        self.__dictconfigs=[]
        self.__listconfigs=[]
        self._inds = np.zeros((0,),int)
        self._ids = np.zeros((0,),int)
        #self._marrayconfig={}
        
        # TODO create an extra class
        if self._is_keyindex:
            self._key_to_id = {}# Dictionary mapping key to id
            self._id_to_key = {} # Dictionary mapping id to key
        else:
            self._key_to_id = None
            self._id_to_key = None
        # save these attributes, apart from configured ones
        # self._attrconfigs_save+=[ '_arrayconfigs','__dictconfigs','_inds','_ids',
        #                    '_marrayconfig','_is_keyindex','_key_to_id']
    
    #def exportXml(self,f,xmlname=None):
    #    f.write(xm.start(self.ident))
    #    for key in self.get_keys():
    #        f.write(xm.begin('od'))
    #        orig,dest = key
    #        f.write(xm.num('orig',orig))
    #        f.write(xm.num('dest',dest))
    #        f.write(xm.num('dem',self._n_trips[key]))
    #        f.write(xm.end('od'))
    #    f.write(xm.end('ods'))
    def __repr__(self):
        #return '|'+self.name+'|'
        return self.name
    
    def exec_events_keys(self,trigger, attrconf, keys):
        """
        Executes all functions assigned for this trigger.
        Keys are ids if is_keyindex=False otherwise they are index keys (any object)
        """
        if self._has_events:
            #print '**TableManager.exec_events_keys NOMOD',attrconf.attrname,self.ident
            for function in self._events.get(trigger,[]): function(attrconf, keys)
             
    def get_obj(self):
        if self._is_coldata:
            return self
        else:
            return self._obj
       
    def get_ident_abs(self):
        """
        Returns absolute identity.
        This is the ident of this object in the global tree of objects.
        If there is a parent objecty it must also be managed by the 
        object manager.
        """ 
        if self._is_coldata:
            # table has its own column data and an ident of its own right
            return BasicManagerMixin.get_ident_abs(self)
        else:
            # table mimics the ident of the object containing the column data
            return self._obj.get_ident_abs()
     
    def get_name(self):
        if self._is_coldata:
            return self.name
        else:
            return self._obj.get_name()

    def get_ident(self):
        if self._is_coldata:
            return self.ident
        else:
            return self._obj.get_ident()
        
    def add(self, attrconf):
        """
        Add a one or several new attributes to be managed.
        kwargs has attribute name as key and Attribute configuration object
        as value.
        """  
        
        AttrsManager.add(self, attrconf) # insert in common attrs database
        #if self.get_ident_abs() == "gen5test.net.1/1.0":
        #    print 'add',attrconf.attrname,attrconf.struct,self._is_coldata
        
        attrname = attrconf.attrname
        
        if attrconf.struct == 'array':
            # init empty zero array of correct type
            self._arrayconfigs.append(attrconf)
            # initial array as large as the currently existing ids   
            val_init = attrconf.get_init(self.get_ids())
            if self._is_coldata: 
                setattr(self._obj, attrconf.attrname, val_init)
                #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
            
        elif attrconf.struct == 'dict':
            # init empty dictionary
            self.__dictconfigs.append(attrconf)
            # initial array as large as the currently existing ids   
            val_init = attrconf.get_init(self.get_ids())
            if self._is_coldata: 
                setattr(self._obj, attrconf.attrname, val_init)
                #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
            
        elif attrconf.struct == 'list':
            self.__listconfigs.append(attrconf)
            # initial array as large as the currently existing ids   
            val_init = attrconf.get_init(self.get_ids())
            if self._is_coldata: 
                setattr(self._obj, attrconf.attrname, val_init)
                #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
        
        elif attrconf.struct == 'scalar':
            val_init = attrconf.get_init()
        
        
        # set initial value if table contains its own arraydata
        #print 'Tableman %s: set attr %s to %s'%(self.name,attrconf.attrname,self._obj)
        #print '  with value',val_init
        #print '  DIR=',self._obj
        #if self._is_coldata: 
        #    setattr(self._obj, attrconf.attrname, val_init)
        #    #print '  self._obj.attr',getattr(self._obj, attrconf.attrname)
            
            
        
        # Return  initial array         
        return val_init
            
   
    
    def is_keyindex(self):
        """
        Returns true if array has key indexed rows
        """    
        return self._is_keyindex
    
    def get_id_from_key(self,key):
        """
        Returns id that corresponds to key.
        """
        return self._key_to_id[key]
    
    def get_key_from_id(self,id):
        """
        Returns key that corresponds to id.
        """
        return self._id_to_key[id]
    
    
    
    def get_ids_from_keys(self,keys):
        ids = [0]*len(keys)
        i=0
        for key in keys:
            ids[i]=self._key_to_id[key]
            i+=1
        return ids   
    
    def get_ind_from_key(self,key):
        return self.get_ind(self.get_id_from_key(key))
    
    def get_inds_from_keys(self,keys):
        return self.get_inds(self.get_ids_from_keys(keys))
        
    def get_keys_from_ids(self,ids):
        keys = ['']*len(ids)
        i=0
        for id in ids:
            keys[i]=self._id_to_key[id]
            i+=1
        return keys 
        # return self._id_to_key[ids] # TODO:no because it is a dict at the moment :(
    
    def get_keys(self,is_ordered = False):
        """
        Returns list of all keyindex .
        """
        #print 'get_keys',self.ident,self._key_to_id.keys()
        if is_ordered:
            return np.array(self.get_keys_from_ids(self._ids),object)
        else:
            return np.array(self._key_to_id.keys())# faste, but random orders
    
    #def get(self,attr,name):
    #    """
    #    Returns value corresponding to is_keyindex id
    #    """
    #    return getattr(self._obj, self.attrname)[self.get_id_from_key(name)]
        
    def get_ids(self,inds = None, is_copy=False):
        """
        Return all ids corrisponding to array indexes inds.
        Options:
            if no inds are given, all ids are returnd
            
            if ordered is true, ids will be sorted before they are returned.
            
        """
        if inds == None:
            if is_copy: 
                return self._ids.copy()
            else:
                return self._ids
        else:
            return self._ids[inds]
    
    def get_id(self,ind):
        """
        Returns scalar id corresponding to index ind
        """
        #print 'get_id',ind,self._ids
        return self._ids[ind]
    
    def select_ids(self,mask):
        """
        Returns an array of ids, corresponding to the True
        of the mask array.
        
        Usage:
            Select all ids for which array of attribute x is greater than zero.
            ids=obj.select_ids( obj.x>0 )
        """
        #print 'select_ids',mask,flatnonzero(mask)
        #print '  self._ids=',self._ids
        
        return np.take(self._ids,np.flatnonzero(mask))
    
    def get_inds(self,ids=None):
        if ids!=None:
            return self._inds[ids]
        else:
            return self._inds[self._ids]
    
    def get_ind(self,id):
        return self._inds[id]
    
    
    def suggest_id(self,is_zeroid=False):
        """
        Returns a an availlable id.
        
        Options:
            is_zeroid=True allows id to be zero.
            
        """
        return self.suggest_ids(1,is_zeroid)[0]
        
    def suggest_ids(self,n,is_zeroid=False):
        """
        Returns a list of n availlable ids.
        It returns even a list for n=1. 
        
        Options:
            is_zeroid=True allows id to be zero.
        """
        # TODO: does always return 1 if is_index is True ?????
        #print 'suggest_ids',n,is_zeroid,self._inds,len(self._inds),self._inds.dtype
        ids_unused_orig = np.flatnonzero(np.less(self._inds,0))
        
        if not is_zeroid:   
            if len(self._inds)==0:
                ids_unused=np.zeros(0,int)
            else:
                # avoid 0 as id:
                #ids_unused=take(ids_unused,flatnonzero(greater(ids_unused,0)))
                #print '  ids_unused_orig',ids_unused_orig,type(ids_unused_orig)
                #print '  len(ids_unused_orig)',len(ids_unused_orig),ids_unused_orig.shape
                #print '  greater(ids_unused_orig,0)',greater(ids_unused_orig,0)
                #print '  len(greater(ids_unused_orig,0))',len(greater(ids_unused_orig,0))
                #print '  flatnonzero(greater(ids_unused_orig,0))',flatnonzero(greater(ids_unused_orig,0))
                #print '  len(flatnonzero(greater(ids_unused_orig,0)))=',len(flatnonzero(greater(ids_unused_orig,0)) )
                ids_unused=ids_unused_orig[np.flatnonzero(np.greater(ids_unused_orig,0))]
            zid=1
        else:
            if len(self._inds)==0:
                ids_unused=np.zeros(0,int) 
            else:
                ids_unused=ids_unused_orig.copy()
            
            zid=0
               
        n_unused=len(ids_unused)
        n_max=len(self._inds)-1
        #print '  ids_unused',ids_unused
        #print '  ids_unused.shape',ids_unused.shape
        #print '  len(ids_unused)',len(ids_unused)
        #print '  n_unused,n_max,zid=',n_unused,n_max,zid
        
        
        
        if n_max<zid:
            # first id generation
            ids=np.arange(zid,n+zid) 
            
        elif n_unused > 0:
            if n_unused >= n:
                ids=ids_unused[:n]
            else:
                #print '  ids_unused',ids_unused 
                #print '  from to',n_max+1,n_max+1+n-n_unused
                #print '  arange=',arange(n_max+1,n_max+1+n-n_unused)
                #print '  type(ids_unused)',type(ids_unused)
                #print '  dtype(ids_unused)',ids_unused.dtype
                ids=np.concatenate((ids_unused,np.arange(n_max+1,n_max+1+n-n_unused))) 
        
        else:
            ids=np.arange(n_max+1,n_max+1+n)
 
        return ids
    
    def suggest_key(self):
        id0 = self.suggest_id(is_zeroid=True)
        n=1
        keys = self.get_keys()
        while str(id0+n+1) in keys:
            n += 1
        
        return str(id0+n+1)
    
    #def export_rowfunctions(self):# use group rowfunction
    #    # return [self.attrs.add_myrow,]
    #    return []
    
    def add_row(self, id = None, key = None):
        
        if id == None:
            return self.add_rows(keys = [key])[0]
        else:
            return self.add_rows(ids = [id])[0]
        
    def add_rows_keyrecycle(self, keys=[]):
        """
        Like add-rows but creates only row with new ids for
        keys that do not exist.
        Returns the complted ids for each key in keys. 
        """
        
        
        ids = np.zeros(len(keys),int)
        inds_new = []
        keys_new =[]
        i = 0
        
        for key in keys:
            if not self.contains_key(key):
                inds_new.append(i)
                keys_new.append(key)
            else:
                ids[i] = self.get_id_from_key(key)
            i+=1
            
        ids_new = self.suggest_ids(len(inds_new))
        self.add_rows(ids_new,keys_new) 
        ids[inds_new]=ids_new
        return ids
                
    def add_rows(self, ids = None, keys = None):
        """
        Creates for each id in ids list an entry for each attribute with 
        array or dict  structure.
        
        If ids is a scalar integer, entries will be generated just for
        this id. 
        
        For is_keyindex objects the list _id_to_key_ is necessary 
        with a key correisponding to each id.  
        """
        #print '\nadd_rows',self.format_ident_abs(),ids,keys
        if keys != None:
            if ids == None:
                n = len(keys)
                ids = self.suggest_ids(n,is_zeroid=True)
            # if each id is supposed to have a _id_to_key 
            # arguments must provide a list with keys for each id
            i=0
            for id in ids:
                #print '  id,keys[i]',id,keys[i],self._key_to_id
                self._key_to_id[keys[i]]=id
                #self.set_attr[id] = 
                self._id_to_key[id] = keys[i]
                i+=1
            #print 'add_rows ids=', ids
            #print '  _id_to_key=',self._id_to_key   
        
        
        
        # no ids to create
        n=len(ids)
        if n==0: return np.zeros(0,int)
                
        id_max=max(ids)
        id_max_old = len(self._inds)-1
        n_array_old = len(self)
        
        ids_existing = np.take(  ids,np.flatnonzero( np.less(ids,id_max_old) )  )
        #print '  ids',ids,'id_max_old',id_max_old,'ids_existing',ids_existing
        
        # check here if ids are still available
        if np.sometrue(  np.not_equal( np.take(self._inds, ids_existing), -1)  ):
            print 'WARNING in create_ids: some ids already in use',ids_existing
            return np.zeros(0,int)
        
        
        
        # extend index map with -1 as necessary
        if id_max > id_max_old:
            #print 'ext',-1*ones(id_max-id_max_old) 
            self._inds = np.concatenate((self._inds, -1*np.ones(id_max-id_max_old,int)))
               
        # assign n new indexes to new ids
        ind_new = np.arange(n_array_old,n_array_old+n)
        
        #print 'ind_new',ind_new
        np.put(self._inds,ids,ind_new)
        
        #print '  concat ids..',self._ids,ids
        self._ids = np.concatenate((self._ids,ids))
        
        
            
        # Extend all arrays with n values from args or default        
        for attrconf in self._arrayconfigs:
            #print '  attr',attrconf.name
            attrconf.add(ids, keys)
            
           
                
        
        # assign defaults to all dicts
        for attrconf in self.__dictconfigs:
            #if attrconf.attrname != '_id_to_key':
            attrconf.add(ids, keys)
            
            #i = 0
            # element by element assignment
            #for id in ids:
            #    #print '  dict attr=',attr,value
            #    dic[id] = defaults[i]
            #    i += 1
        
        # assign defaults to all dicts
        for attrconf in self.__listconfigs:
            attrconf.add(ids, keys)
            
        
        return ids
    
    def get_row(self,key):
        if self._is_keyindex:
            id = self.get_id_from_key(key)
            #attrconf.exec_events_item(EVTADDITEM,keys=[key])
        else:
            id = key
            #attrconf.exec_events_item(EVTADDITEM,ids=[id])
        attrs={}    
        for attrconfig in self._attrconfigs:
            if attrconfig.struct in ('array','dict','list'):
                attrs[attrconfig.attrname] = attrconfig[id]
        
        return attrs
    
    def get_rowobj(self, id ):
        """
        Returns an instance of Rowobject of row id
        """  
        if self._is_coldata:
            return RowObj(self, id, self.ident)
        else:
            return RowObj(self, id, self._obj.ident)
              
                    
    def set_row(self,key,**attrs):
        """
        Sets and creates a row if not existant:
            for keyindex key is the key
            for id index key is id
        Attention returns always the id
        """
        if self._is_keyindex:
            if not self.contains_key(key):
                self.add_row(key = key)
            id = self.get_id_from_key(key)
            
        else:
            id = key
            if id not in self:
                self.add_row(id = id)
            
            
        for attrname,value in attrs.iteritems():
            # overwrite only existant attributes
            if hasattr(self,attrname):
                getattr(self,attrname)[id] = value
        
        
        return id
        
    def del_row(self, key):
        if self._is_keyindex:
            id = self.get_id_from_key(key)
        else:
            id = key
        del self[id]
     
    def del_rows(self, keys):
        for key in keys:
            self.del_row(key)
    
    def print_attrs(self, show_unit = True, show_parentesis=False):
        print 'Attributes of Table',self.get_name(),'(ident=%s)'%self.get_ident()
        for attrconf in self._attrconfigs:
            #print '\n %s %s'%(attrconf.name,attrconf.format_unit(show_parentesis = True))
            #print 'attrconf.name,attrconf.groupnames',attrconf.name,attrconf.groupnames
            if attrconf.groupnames[0] != '_private':
                for id in self.get_ids():
                    if self._is_keyindex:
                        print '  %s[%s] =\t %s'%(attrconf.attrname,self.get_key_from_id(id),attrconf.format_value(id,show_unit=False))
                    else:
                        #print '             **',attrconf.attrname,id,'*',attrconf.format_value(id,show_unit=False),'*'
                        print '  %s[%d] =\t %s'%(attrconf.attrname,id,attrconf.format_value(id,show_unit=False))   
                       
                   
    def export_csv(self, filepath=None, sep=',', name_id='ID', 
                    file=None, attrconfigs = None, ids = None, groupname = None,
                    is_header = True, is_ident = False, is_timestamp = True):
        
        
        if filepath!=None:
            file=open(filepath,'w')
            
        if ids == None:
            ids = self.get_ids()
            
        if groupname !=None:
            attrconfigs = self.get_group(groupname)
            is_exportall = False
        if attrconfigs == None:
            attrconfigs = self._attrconfigs
            is_exportall = False
        else:
            is_exportall = True
        
        # header
        if is_header:
            
            row = self._clean_csv(self.get_name(),sep)
            if is_ident:
                row+= sep+'(ident=%s)'%self.format_ident_abs()
            file.write(row+'\n')
            if is_timestamp:
                now = datetime.now()
                file.write(self._clean_csv(now.isoformat(),sep)+'\n')
            file.write('\n\n')
        
        # first table row
        row = name_id
        for attrconf in attrconfigs:
            
            if len(attrconf.groupnames)>0:
                is_private = attrconf.groupnames[0] == '_private'
            else:
                is_private = False
                
            if ((not is_private)&(attrconf.is_save()))|is_exportall:
                row +=sep+self._clean_csv(self.format_symbol(attrconf),sep)
        file.write(row+'\n')
        
        # rest
        for id in ids:
            if self._is_keyindex:
                row = str(self.get_key_from_id(id))#.__repr__()
            else:
                row = str(id)
            row = self._clean_csv(row,sep)
            for attrconf in attrconfigs:
                if len(attrconf.groupnames)>0:
                    is_private = attrconf.groupnames[0] == '_private'
                else:
                    is_private = False
                if ((not is_private)&(attrconf.is_save()))|is_exportall:
                    row+= sep+self._clean_csv('%s'%(attrconf.format_value(id,show_unit=False)),sep)
            
            # make sure there is no CR in the row!!
            #print  row
            file.write(row+'\n')    
        
        if filepath!=None:
            file.close()
    
    def _clean_csv(self,row,sep):
        row=row.replace('\n',' ')
        #row=row.replace('\b',' ')
        row=row.replace('\r',' ')
        #row=row.replace('\f',' ')
        #row=row.replace('\newline',' ')
        row=row.replace(sep,' ')
        return row
    
    def format_symbol(self, attrconf):
        if hasattr(attrconf,'symbol'):
            symbol = attrconf.symbol
        else:
            symbol = attrconf.name
        unit=attrconf.format_unit(show_parentesis=True)
        symbol+=' '+unit  
        return symbol
                  
    def print_rows(self, show_unit = True, show_parentesis=False):
        
        print 'Rows of Table',self.get_name(),'(ident=%s)'%self.format_ident_abs()
        for id in self.get_ids():
            if self._is_keyindex:
                print '\n ID=%s'%self.get_key_from_id(id)
            else:
                print '\n ID=%d'%id
            for attrconf in self._attrconfigs:
                if attrconf.groupnames[0] != '_private':
                    print '  %s =\t %s'%(attrconf.attrname,attrconf.format_value(id,show_unit=True))
       
            
    def __contains__(self,id):
        if (id<len(self._inds))&(id>=0):
            return self._inds[id]>-1
        else:
            return False
        
    def contains_key(self,key):
        return self._key_to_id.has_key(key)
    
    def has_key(self,key):
        return self._key_to_id.has_key(key)
    
    def contains(self,key):
        if self._is_keyindex:
            return self._key_to_id.has_key(key) 
        else:
            return key in self   
        
    def __len__(self):
        """
        Determine current array length (same for all arrays)
        """
        
        return len(self._ids)
    
    def delete(self,attr):
        """
        Delete an attribite from table management and managed object
        """
        #if AttrsManager.__delitem__(attr):
        
        
        
        attrconf = getattr(self, attr)
        #attrconf.exec_events(EVTDEL)
        if attrconf in self._attrconfigs:
            
            # delete attribute and its configuration
            if attrconf.struct == 'array':
                self._arrayconfigs.remove(attrconf)
            
            elif attrconf.struct=='dict':
                self.__dictconfigs.remove(attrconf)
                
            elif attrconf.struct=='list':
                self.__listconfigs.remove(attrconf)
        
        if AttrsManager.delete(self,attr):
            # basic removal successful
            return True
        else:
            print 'WARNING could not remove attr "%s" from Table ident "%s"'%(attr,self.ident)
            return False

    #def _del_item(self, ids):
    #    """
    #    Class specific delete operations 
    #    """
    #    pass
        
    def __delitem__(self,ids):
        """
        remove rows correspondent to the given ids from all array and dict
        attributes
        """
        if not hasattr(ids, '__iter__'):
            ids = [ids]
        
        # note clean: decision: make it before calling del[ids]
        #self._del_item(ids)
        
        
            
        for id in ids:
            if self._is_keyindex:
                key = self._id_to_key[id]
            else:
                key = None
            #print '    removed',key,id
            
            #print '  start deleting id',id
            
            #print '      self._ids',self._ids
            #print '      self._inds',self._inds
            
            #i=self.get_index(id) 
            i=self._inds[id]
            
            #print '     ind=',i
            #print '     del rows from array-types...'
            
            # TODO: this could be done in with array methods
            for attrconf in self._arrayconfigs:
                #print '      del',attr,id,i 
                attrconf.delete(id)
                
                
                
            
            
            
            #print '    del from dicts'
            for attrconf in self.__dictconfigs:  
                #print '      del',attr,id
                attrconf.delete(id)
                
            for attrconf in self.__listconfigs:  
                #print '      del',attr,id
                attrconf.delete(id)
            
            if self._is_keyindex:
                #print '  Remove is_keyindex'
                key=self._id_to_key[id]
                del self._key_to_id[key]
                del self._id_to_key[id]
                
            #print '    del from id lookup'
            self._ids=np.concatenate((self._ids[:i],self._ids[i+1:]))
            
            #print '    free index',id
            if id == len(self._inds)-1:
                # id is highest, let's shrink index array by 1
                self._inds=self._inds[:-1]
            else:
                self._inds[id]=-1   
                    
            # get ids of all indexes which are above i
            ids_above=np.flatnonzero(self._inds>i)
                
            # decrease index from those wich are above the deleted one
            #put(self._inds, ids_above,take(self._inds,ids_above)-1)
            self._inds[ids_above]-=1
            
            #print '    self._inds',self._inds
            
        
                
                    
        #print '  del',ids,' done.'
    
 

    
class ObjManagerMixin(BasicManagerMixin):
    """
    Manages attributes of an object 
    """
    def _init_objman(self, ident='no_ident', parent=None, name= None, info ='',
                        is_tab = True):

        BasicManagerMixin._init_objman(self, ident=ident, parent=parent, name= name)
        self.info = info
        if is_tab:
            self._tablemans  = [] # list of table managements
    
        return True
    
    
        

def save_obj(obj,filename, is_not_save_parent=False):
    """
    Saves python object to a file with filename.
    Filename may also include absolute or relative path.
    If operation fails a False is returned and True otherwise.
    """
    try:
        file=open(filename,'wb')
    except:
        print 'WARNING in save: could not open',simname
        return False
	
	if is_not_save_parent:
	    parent = obj.parent
	    obj.parent = None
    
    pickle.dump(obj, file, protocol=2)
    file.close()
    
    if is_not_save_parent:
        obj.parent=parent
    return True

def load_obj(filename,parent=None):
    """
    Reads python object from a file with filename and returns object.
    Filename may also include absolute or relative path.
    If operation fails a None object is returned.
    """
    print 'load_obj',filename
    try:
        file=open(filename,'rb')
    except:
        print 'WARNING in load_obj: could not open',filename
        return None
    
    #try:
    #print '  pickle.load...'
    obj=pickle.load(file)
    file.close()
    #print '  obj._link2'
    
    # _init2_ is to restore INTERNAL states from INTERNAL states
    #obj._init2_config()
    
    # _init3_ is to restore INTERNAL states from EXTERNAL states 
    # such as linking
    #obj._init3_config()
    
    # _init4_ is to do misc stuff when everything is set
    #obj._init4_config()
    
    return obj

 
class EmptyClass(ObjManagerMixin):
    """
    This claass is empty and used as default.
    
    """
    
    def __init__(self, ident = 'empty',  parent = None, name = 'Empty Object'):
        self._init_objman(ident, parent = parent, name = name)
        self.attrs = self.set_attrman(AttrsManager(self,'attrs'))
            
class TestClass(ObjManagerMixin):
        def __init__(self,parent=None):
            self._init_objman('testobj',parent=parent, name = 'Test Object')
            self.attrs = self.set_attrman(AttrsManager(self,'attrs'))
            
            
            self.attrs.add(FuncConf(  'testfunc', self.testfunc,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        name = 'Test function', 
                                        info = 'This is a test function',
                                        ))
            
            self.access = self.attrs.add(AttrConf(  'access', ['bus','bike',5,2.5],
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Access list', 
                                        info = 'List with vehicle classes that have access',
                                        ))
                                        
            self.emissiontype = self.attrs.add(AttrConf(  'emissiontype', 'Euro 0',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Emission type', 
                                        info = 'Emission type of vehicle',
                                        ))
                                        
            self.x = self.attrs.add(AttrConf(  'x', 0.0,
                                        groupnames = ['state'], 
                                        perm='r', 
                                        is_save = True,
                                        unit = 'm',
                                        metatype = 'length', 
                                        name = 'position', 
                                        info = 'Test object position',
                                        ))
                                        
            self.is_pos_ok = self.attrs.add(AttrConf(  'is_pos_ok', False,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Pos OK', 
                                        info = 'True if position is OK',
                                        ))
                                        
            self.child = self.attrs.add(AttrConf(  'child', EmptyClass(),
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Child',
                                        info = 'Child object',
                                        ))
                                        
                                        
            self.v = self.attrs.add( AttrConf( 'v', 0.0,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        unit = 'm/s',
                                        metatype = 'velocity', 
                                        name = 'speed', 
                                        info = 'Vehicle speed',
                                        digits_fraction = 2,
                                        ))
            self.configfilename = self.attrs.add(AttrConf( 
                                        'configfilename', 'thisconfig.txt',
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        metatype = 'filepath', 
                                        name = 'config file', 
                                        wildcards = "XML files (*.xml)|*.xml",
                                        info = 'Configuration file name',
                                        ))
                                        
            self.dirname_output = self.attrs.add(AttrConf( 
                                        'dirname_output', os.getcwd(),
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        metatype = 'dirpath', 
                                        name = 'output dir', 
                                        info = 'Output dir name.',
                                        ))
            
            self.tab = self.add_tableman(TableManager(obj=self, ident='test_table'))
            self.flows = self.tab.add(ArrayConf( 'flows', 0.1,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        unit = '1/s',
                                        metatype = 'flow', 
                                        name = 'vehicleflow', 
                                        info = 'Flow of vehicles',
                                        digits_fraction = 2,
                                        ))
                                        
            self.filenames = self.tab.add(DictConf( 'filenames', 'source_v001.csv',
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        metatype = 'filepath', 
                                        name = 'file name', 
                                        info = 'file name of source data',
                                        ))
                                        
            self.childlist = self.tab.add(ListConf(  'childlist', EmptyClass(),
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Child',
                                        info = 'Child object',
                                        ))
                                        
            
            self.ntab = self.add_tableman(TableManager(  obj=self, 
                                                        ident='keyindexed_test_table',
                                                        is_keyindex = True ),
                                                    )
                                            
            self.configfilenames = self.ntab.add(DictConf( 'configfilenames', 'source_v001.csv',
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        metatype = 'filepath', 
                                        name = 'config files', 
                                        info = 'Configuration file names',
                                        ))
            
            self.modified = self.ntab.add(ArrayConf( 'modified', False,
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        name = 'modified',
                                        is_save = True,
                                        info = 'Has file been modifie.',
                                        ))
        def testfunc(self):
            return 'Hello from testfunc'
                                        
            
                                        
                                            
class TestClassKeyindexed(ObjManagerMixin):
        def __init__(self, ident, name,parent = None):# position of parent is an exception here
            self._init_objman(ident, name = name,parent = parent)
            self.attrs = self.set_attrman(AttrsManager(self,'attrs'))
            self.tab = self.add_tableman(TableManager(  obj = self, 
                                                        ident='keyindexed_test_table',
                                                        is_keyindex = True ),
                                                    )
                                            
            self.filenames = self.tab.add(DictConf( 'filenames', 'source_v001.csv',
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        metatype = 'filepath', 
                                        name = 'file name', 
                                        info = 'file name of source data',
                                        ))
            
            self.modified = self.tab.add(ArrayConf( 'modified', False,
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        info = 'Has file been modifie.',
                                        ))
            
            
###############################################################################
if __name__ == '__main__':
    """
    Test
    """
    
    def on_event_delattr(attrconfig):
        print 'EVENT: Attribute',attrconfig.attrname,'will be deleted!!'                            
    
    def on_event_setattr(attrconfig):
        print 'EVENT: Attribute',attrconfig.attrname,'has been set to a new value',attrconfig.format()
    
    def on_event_getattr(attrconfig):
        print 'EVENT: Attribute',attrconfig.attrname,'has been retrieved the value',attrconfig.format()
    
    def on_event_additem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':added keys:',keys
    
    def on_event_delitem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':delete keys:',keys
    
    def on_event_setitem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':set keys:',keys
    
    def on_event_getitem(attrconfig,keys):
        print 'EVENT: Attribute',attrconfig.attrname,':get keys:',keys
        
    obj = TestClass()
    print 'obj.ident,obj.tab.get_ident(),obj.tab.get_ident_abs()',obj.ident,obj.tab.get_ident(),obj.tab.get_ident_abs()
    
    print 'This is the value of the attribute: obj.x=',obj.x
    print 'This is the configuration instance of the attribute x',obj.attrs.x
    print 'obj.attrs.x.format():',obj.attrs.x.format()
    obj.attrs.x.add_event(EVTSET,on_event_setattr)
    obj.attrs.x.add_event(EVTGET,on_event_getattr)
    #print 'obj.attrs.get_groups()',obj.attrs.get_groups()
    #print 'obj.tab.get_groups()',obj.tab.get_groups()
    
    print 'Test func...',obj.attrs.testfunc.get()
    obj.attrs.testfunc.add_event(EVTGET,on_event_getattr)
    obj.attrs.testfunc.get()
    
    obj.attrs.x.set(1.0)
    obj.attrs.x.get()
    
    obj.attrs.delete('x')
    
    #print 'obj.attrs.x.get_formatted()=',obj.attrs.x.get_formatted()
    #print 'obj.x',obj.x
    
    
    ids = obj.tab.suggest_ids(5)
    print 'obj.childlist',obj.childlist
    #sys.exit(0)
    obj.tab.add_rows(ids)
    
    #
    # to access attributes through the manager always use the respective
    # table or attribute manager...indipendent from where the real attributes 
    # are stored ( self.cols or self. )
    #
    # This means attrconfig is ALWAYS stored in the attr or table manager
    #
    # IF the attributes are sored in self (obj=self) 
    # THEN the sollowing syntax must be used:
    # self.modified = self.tab.add(ArrayConf( 'modified', False,
    #                                    groupnames = ['files'], 
    #                                    perm='rw', 
    #                                    is_save = True,
    #                                    info = 'Has file been modifie.',
    #                                    ))
    # if the arribute is stored in .cols (obj=None) the 
    # THEN the sollowing syntax must be used:
    # self.tab.add(ArrayConf( 'modified', False,
    #                                    groupnames = ['files'], 
    #                                    perm='rw', 
    #                                    is_save = True,
    #                                    info = 'Has file been modifie.',
    #                                    ))
    obj.tab.flows[[1,2]] = [5,6]
    print 'obj.flows:',obj.flows
    print 'obj.filenames:',obj.filenames
    print 'obj.tab.get_ids():',obj.tab.get_ids()
    
    print 'obj.tab.flows.format():\n',obj.tab.flows.format()
    
    print 'delete row  with id 1' 
    del obj.tab[1]
    
    print 'obj.tab.flows.format():\n',obj.tab.flows.format()
    
    print 'obj.tab.filenames.format():\n',obj.tab.filenames.format()
    
    id = obj.tab.suggest_id()
    print 'obj.tab.suggest_id =',id 
    
    
    obj.status = obj.tab.add(DictConf( 'status', 'occupied',
                                        groupnames = ['files'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'file status', 
                                        info = 'Status of file',
                                        ))
    print 'obj.tab.status.format():\n',obj.tab.status.format()
    
    obj.accels = obj.tab.add(ArrayConf( 'accels', 0.0,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'accelerations', 
                                        unit = '1/s^2',
                                        info = 'Vehicle accelerations',
                                        ))
    
    
    print 'obj.tab.accels.format():\n',obj.tab.accels.format()  
    
    print 'delete rows with id 3,4' 
    del obj.tab[[3,4]]
    print 'obj.tab.status.format():\n',obj.tab.status.format()
    print 'obj.tab.accels.format():\n',obj.tab.accels.format()    
    
    print 'Add row with index 10'
    obj.tab.add_row(10)
    print 'obj.tab.status.format():\n',obj.tab.status.format()
    print 'obj.tab.accels.format():\n',obj.tab.accels.format() 
    print 'obj.accels', obj.accels    
    
    print '\n\n'
    obj.attrs.print_attrs()
    obj.tab.print_rows()  
    obj.tab.print_attrs()   
    
    rowobj =  obj.tab.get_rowobj(10)
    rowobj['status'] = 'free'
    rowobj['accels'] = np.pi
    print "rowobj['status']", rowobj['status']   
    print "rowobj['accels']", rowobj['accels']
    #print 'rowobj.accels=',rowobj.accels
    
    print '\n\n\n\n'
    objn = TestClassKeyindexed('testobj_keyindexed','Test object for keyindexed rows')
    
    objn.tab.add_rows(keys = ['routes','polygons','net'])
    #print '_id_to_key',objn._id_to_key
    objn.tab.filenames.set('net','ssss.ss')
    objn.tab.print_attrs()
    print 'objn.tab._key_to_id',objn.tab._key_to_id
    print 'objn.tab._id_to_key',objn.tab._id_to_key
    print 'objn.tab.get_ids()',objn.tab.get_ids()
    print "objn.tab.filenames.get('routes')=",objn.tab.filenames.get('routes')
    
    del objn.tab[1]
    objn.tab.print_attrs()
    objn.tab.add_row(key='logs')
    objn.tab.print_rows()
    
    t = TableManager(obj=None, ident='standalonetable', name = 'Standalone Table' , is_keyindex= False, )
    print 't.get_ident(),t.get_ident_abs()',t.get_ident(),t.get_ident_abs()
    
    t.add(ArrayConf( 'flows', 0.1,
                        groupnames = ['state'], 
                        perm='rw', 
                        is_save = True,
                        unit = '1/s',
                        metatype = 'flow', 
                        name = 'vehicleflow', 
                        info = 'Flow of vehicles',
                        digits_fraction = 2,
                        ))
                        
    t.add(ArrayConf( 'vertex', np.array([0.0,0.0],float),
                        groupnames = ['state'], 
                        perm='rw', 
                        is_save = True,
                        metatype = 'coord', 
                        name = 'Vertex', 
                        info = 'Vertex vector',
                        digits_fraction = 2,
                        )) 
                        
    
    t.add(ArrayConf( 'transforms', np.zeros((4,4),float),
                        groupnames = ['state'], 
                        perm='rw', 
                        is_save = True,
                        metatype = 'coord', 
                        name = 'transforms', 
                        info = 'transformation matrix',
                        digits_fraction = 2,
                        )) 
                        
     
                                                          
    t.add(DictConf( 'filenames', 'source_v001.csv',
                        groupnames = ['files'], 
                        perm='rw', 
                        is_save = True,
                        metatype = 'filepath', 
                        name = 'file name', 
                        info = 'file name of source data',
                        ))
    
    t.flows.add_event(EVTADDITEM,on_event_additem)
    t.flows.add_event(EVTDELITEM,on_event_delitem)
    t.vertex.add_event(EVTSETITEM,on_event_setitem)
    t.vertex.add_event(EVTGETITEM,on_event_getitem)
    
    ids = t.suggest_ids(5)
    t.add_rows(ids)   
    t.vertex[5]=[11,22] # this will not trigger an event
    t.vertex.set([4,5],[[41,42],[53,54]])
    t.vertex[1:3]=[[33,44],[55,66]]
    
    t.transforms[1:3]=[11*np.identity(4),22*np.identity(4)]
    
    t.print_rows()        
    print 't.cols.flows',t.cols.flows# because table manager has obj=None
    
    
    
    print 't.cols.transforms',t.cols.transforms
    
    t.del_rows([1,2,3])
    #print ' t.__class__',t.__class__
    t.export_csv(   filepath='classmanager_test.csv', sep=',', name_id='ID', 
                    file=None, attrconfigs = None, groupname = None,
                    is_header = True)
                    
    tt=TableManager( ident='testtable',is_keyindex= True)
    tt.add(ArrayConf(  'intervals', [0.0,0.0],
                            groupnames = ['params',], 
                            perm='rw', 
                            name = 'Intervals', 
                            unit = 's',
                            info = 'Identified time intervals for traces.'
                            ))
    
                                    
    id1 = tt.suggest_id()
    tt.set_row(id1, intervals=[11,11])
    id2 = tt.suggest_id()
    tt.set_row(id2, intervals=[22,22])
    print 'id1,id2',id1,id2
    tt.print_rows()   
    
    
                                                           