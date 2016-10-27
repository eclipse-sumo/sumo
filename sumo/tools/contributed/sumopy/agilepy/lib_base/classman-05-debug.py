
# python classman.py

# TODO:
# - store old values in attrcons and recover with undo

# new structure
##obj0
##|
##|-attr1
##|-attr2
##|
##|-tab1
##|  |
##|  |-attr1
##|  |-attr2
##|  |
##|  |-arraycol1
##|  |  |
##|  |  |-1:x1
##|  |  |-2:x2
##|  |
##|  |-childcol1
##|  |  |
##|  |  |-1:obj1
##|  |  |-2:obj2
##|  |
##|  |
##|  |-tabchildcol1(tab2)
##|  |  |
##|  |  |-1:id1
##|  |  |-2:id2 
##|
##|
##|
##|
##|-child1[obj.child1]
##|   |
##|   |- attr1
##
##IDEA : make childof column
##tab2
##|
##|-arraycol1
##|  |-id1:x1
##|  |-id2:x2
##|
##|
##|-arraycol2
##|  |-id1:y1
##|  |-id2:y2

## To be or not to be.  -- Shakespeare 
## To do is to be.  -- Nietzsche 
## To be is to do.  -- Sartre 
## Do be do be do.  -- Sinatra 

# save with is saved flag
# xml mixin
# different attrconfig classe (numbers, strings, lists, colors,...)

import types, os, pickle, sys, string
from collections import OrderedDict
#import numpy as np
import xmlmanager as xm
from datetime import datetime
##########


# oh my god....:
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

ATTRS_NOSAVE = ('value','plugin','_obj','_manager','get','set','add','del','childs','parent')   
ATTRS_SAVE = ('ident','name','managertype','_info')  
ATTRS_SAVE_TABLE = ('_is_localvalue','attrname','_colconfigs','_ids','_attrconfigs','_groups','plugin')       
         
STRUCTS_COL = ('odict,')        

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
    
    # init_postload_internal is to restore INTERNAL states from INTERNAL states
    print 'load_obj->init_postload_internal',obj.ident
    obj.init_postload_internal(parent)
    
    # init_postload_external is to restore INTERNAL states from EXTERNAL states 
    # such as linking
    obj.init_postload_external()
    
    # _init4_ is to do misc stuff when everything is set
    #obj._init4_config()
    
    return obj

class Plugin:
    
    def __init__(self,obj):
        self._obj =  obj # this can be either attrconf or main object 
        self._events = {}
        self._has_events = False
        
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
                function(self._obj)
        
    
    def exec_events_ids(self,trigger, ids):
        """
        Executes all functions assigned for this trigger for multiple ids.
        """
        if self._has_events:
            #print '**ArrayConf._execute_events_keys',self.attrname,trigger,(EVTGETITEM,EVTGET)
            #if trigger!=EVTGETITEM:
            #    #print '  call set_modified',self._manager
            #    self._manager.set_modified(True)
                
            for function in self._events.get(trigger,[]): 
                function(self._obj, ids)
  

class AttrConf:
    """
    Contains additional information on the object's attribute.
    """
    def __init__(self, attrname, default,
                    groupnames = [], perm='rw', 
                    is_save = True, 
                    #is_link = False, # define link class
                    is_copy = True,
                    name = '', info = '', 
                    unit = '',
                    is_plugin = False,
                    struct = 'scalar', # depricated, metatype is class
                    metatype = '', # depricated, metatype is class
                    **attrs):

        # these states will be saved and reloaded
        self.attrname = attrname
        self.groupnames = groupnames
        self.metatype = metatype
        self.struct = struct
        
        self._default = default
        
        
        self._is_save = is_save
        self._is_copy = is_copy
        self._is_localvalue = True # value stored locally, set in set_manager
        self._unit = unit
        self._info = info
        self._name = name 
        self._perm = perm
        
        # states below need to be resored after load
        self._manager = None # set later by attrsman , necessary?
        self._obj = None # parent object, set later by attrsman
        
        self._is_modified = False
        self._is_saved = False
        
        self.init_plugin(is_plugin)
        
            
        # set rest of attributes passed as keyword args
        # no matter what they are used for
        for attr, value in attrs.iteritems():
            setattr(self,attr,value)
    
    
                
    def init_plugin(self, is_plugin):
        if  is_plugin:
            self.plugin = Plugin(self)
            self.set = self.set_plugin
            self.get = self.get_plugin
        else:
            self.plugin = None
            
    def get_name(self):
        return self._name
      
    def is_modified(self):
        return  self._is_modified
           
    def set_manager(self, manager):
        """
        Method to set manager to attribute configuration object.
        This is either attribute manager or table manager.
        Used by add method of AttrManager
        """
        self._manager = manager
        self._is_localvalue = manager.is_localvalue()
     
        
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
        
    
    def get(self):
        #  return attribute, overridden with indexing for array and dict struct
        return self.get_value()
      
    def set(self, value):
        # set attribute, overridden with indexing for array and dict struct
        if value != self.get_value():
            self.set_value(value)   
            self._is_modified = True
        return value 

    
    
    def get_plugin(self):
        """
        Default get method with plugin for scalar attrs
        """
        #  return attribute, overridden with indexing for array and dict struct
        self.plugin.exec_events(EVTGET)
            
        return self.get_value()
      
    def set_plugin(self, value):
        """
        Default set method with plugin for scalar attrs
        """
        # set attribute, overridden with indexing for array and dict struct
        if value != self.get_value():
            self.set_value(value)   
            self._is_modified = True
            self.plugin.exec_events(EVTSET)
            
        return value
    
    
    def get_default(self):
        return self._default
    
    def get_init(self):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        value = self.get_default()
        # store locally if required
        if self._is_localvalue:
            self.value = value
        return value
    
    #def is_tableattr(self):
    #    return self.struct in ('dict','array','list')
    
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
    
    
    def has_unit(self):
        return self._unit != ''
    
    
        
    def has_info(self):
        return self.get_info() != ''
    
    def get_info(self):
        return self.info
       
    def format_unit(self, show_parentesis=False):
        if self._unit in ('',None):
            return ''
        if show_parentesis:
            return '[%s]'%self._unit
        else:
            return '%s'%self._unit
        
    
    def format_value(self, show_unit = False, show_parentesis=False):
        if show_unit:
            unit = ' '+self.format_unit(show_parentesis)
        else:
            unit = ''
        return repr(self.get_value())+unit
    
    ####
    def get_value(self):
        # always return attribute, no indexing, no plugin
        if self._is_localvalue:
            return self.value
        else:
            return getattr(self._obj, self.attrname)
    
    
    def set_value(self, value):
        #set entire attribute, no indexing, no plugin
        print 'AttrConf.set_value',self.attrname, self._is_localvalue, value
        if self._is_localvalue:
            self.value = value
        else:
            return setattr(self._obj, self.attrname, value)
    
    
    
    
    
    
    

    def predelete(self):
        """
        Cleanup operations before deleting
        """
        if self._is_localvalue:
            del self.value # delete value
        else:
            del self._obj.__dict__[self.attrname] # delete value

    
    #def init_presave_internal(self, man, obj):
    #    pass
    # not a good idea to delete links, plugins here
    
    #def save_value(self, state):
    #    """
    #    Save attribute value of managed object to state dict.
    #    
    #    move this into __getstate__
    #    
    #    restore value in _obj during postllad_external
    #    
    #    make _getstate_ for speecific save 
    #    """
    #    #print 'save_value',self.attrname,self._is_save, self._is_localvalue,
    #    #
    #    # Attention can be called fron __getstate__ of obj if _is_localvalue=False
    #    # or from __getstate__ of attribute config if _is_localvalue=True 
        
    
    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        pass 
    
    def  __getstate__(self):
        #print '__getstate__',self.ident
        #print '  self.__dict__=\n',self.__dict__.keys()
        
        state={} 
        for attr in self.__dict__.keys():
            
            if attr =='plugin':
                plugin = self.__dict__[attr]
                if plugin!=None:
                    state[attr] = True
                else:
                    state[attr] = False
                
            elif attr not in ATTRS_NOSAVE:
                state[attr] = self.__dict__[attr]
        
        if self._is_save:
            self._is_modified = False
            state['value']= self.get_value()
            
        self._getstate_specific(state)            
        return state
    
          
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        self.plugins={}
        
        for attr in state.keys():
            #print '  set state',key
            
            #done in init_postload_internal...
            #if attr=='plugin':
            #    if state[attr]==True:
            #        self.__dict__[attr] = Plugin(self)
            #    else:
            #        self.__dict__[attr]= None
            #else:
            self.__dict__[attr]=state[attr]
                
    def init_postload_internal(self, man, obj):
        print 'AttrConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident
        self.set_manager(man)
        self.set_obj(obj)
        self.init_plugin(self.plugin)
        
        # set initial values for unsafed attributes
        if not self._is_save:
            self.set_value(self.get_init())
        else:
            if self._is_localvalue:
                # OK self.value already set in __setstate__
                pass
            else:
                setattr(self._obj, self.attrname, self.value) # TODO: could be made nicer with method
                del self.value # no longer needed
        #print '  check',hasattr(self,'value')
        
    def init_postload_external(self):
        pass
            
class ObjConf(AttrConf):
    """
    Contains additional information on the object's attribute.
    Configures Pointer to another object .
    This other object must have an ident.
    it can be either a child (then it will be saved)
    or a link (then only the id will saved)
    If it is a child the is_child = True (default value) 
    """
    
    def __init__(self, valueobj, is_child = True, **kwargs):
        attrname = valueobj.get_ident()
        self._is_child = is_child
        AttrConf.__init__(self,  attrname, valueobj, 
                                struct = 'scalar', 
                                metatype = 'obj',
                                perm = 'r',
                                **kwargs
                                )
        
        
 
    
    def set_obj(self, obj):
        """
        Method to set instance of managed object.
        Used by add method of AttrManager
        """
        print 'ObjConf.set_obj',self.attrname,obj.ident
        AttrConf.set_obj(self, obj)
        
        if self._is_child:
            # tricky: during first initialization
            # child instance is stored in default
            obj.set_child(self) 
    
    def predelete(self):
        AttrConf.predelete(self)
        if self._is_child:
            self.get_obj().del_child(self.attrname)
    
     
    
    
    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        if self._is_save:
            if self._is_child:
                # OK self.value already set in
                pass
            else:
                # remove object reference from value and create ident
                state['value']= None
                state['_ident_value'] = self.get_value().get_ident_abs()
    
    def init_postload_internal(self, man, obj):
        print 'ObjConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident
        
        AttrConf.init_postload_internal(self, man, obj)
        if self._is_child:
            print '  make sure children get initialized'
            #self.get_value().init_postload_internal(obj)
            print '  call init_postload_internal of',self.get_value().ident,self.get_value(),self.get_value().__class__
            self.get_value().init_postload_internal(obj)
    
        
    def init_postload_external(self):
        if self._is_child:
            # restore normally
            AttrConf.init_postload_external(self)
            self.get_value().init_postload_external()
        else:
            # Substitute absolute ident with link object.
            # Called from init_postload_external of attrsman during load_obj
            # 
            ident_abs = self._ident_value
            print 'reset_linkobj',self.attrname,ident_abs
            obj = self.get_obj()
            rootobj = obj.get_root()
            print '  rootobj',rootobj.ident
            linkobj = rootobj.get_obj_from_ident(ident_abs)
            print '  linkobj',linkobj.ident
            self.set_value(linkobj)
        

    
    #def get_valueobj(self):
    #    """
    #    This is called by get_childobj to retrive the child instance.
    #    """
    #    return self.get_value()
        
    def get_name(self):
        return self.get_value().get_name()
    
    def get_info(self):
        return self.get_value().__doc__
       
    def format_value(self, show_unit = False, show_parentesis=False):
        return repr(self.get_value())
     
    
    
     
 
class ColConf(AttrConf):
    """
    #>>> from collections import OrderedDict
    #>>> spam = OrderedDict([('s',(1,2)),('p',(3,4)),('a',(5,6)),('m',(7,8))])
    >>> spam.values()
  
    """
    #def __init__(self, **attrs):
    #    print 'ColConf',attrs
        
    def __init__(self, attrname, default,  **attrs):
        #print 'ColConf',attrs                
        AttrConf.__init__(self,  attrname, default,
                    struct = 'odict', 
                    **attrs)
        

          
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.attrname,ids,self.get_default()
        values = []
        for _id in ids:
            values.append(self.get_default())
        #len(ids)*self.get_default() # makes links, not copies
        return values

    def get_init(self):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        ids = self._manager.get_ids()
            
        #print '\n\nget_init',self.attrname,ids
        values = self.get_defaults(ids)
        i = 0
        odict = OrderedDict()
        for _id in ids:
            odict[_id] = values[i]
            i +=1 
        # store locally if required
        if self._is_localvalue:
            self.value = odict
        # pass on to calling instance
        # in this cas the data is stored under self._obj
        return odict
    
    def init_plugin(self, is_plugin):
        if  is_plugin:
            self.plugin = Plugin(self)
            self.set = self.set_plugin
            self.get = self.get_plugin
            self.add = self.add_plugin
            self.delete = self.delete_plugin
        else:
            self.plugin = None
            
    
    
    
            
    
    def __delitem__(self, ids):   
        #print '        before=\n',self.__dict__[attr]
        #attr = self.attrconf.get_attr()
        if hasattr(ids, '__iter__'):
            array = self.get_value()
            for _id in ids:
                del array[_id]
        else:
            del self.get_value()[ids]
                
    
    def __getitem__(self, ids):
        #print '__getitem__',key
        if hasattr(ids, '__iter__'):
            items = len(ids)*[None]
            i = 0
            array = self.get_value()
            for _id in ids:
                items[i] = array[_id]
                i+=1
            return  items
        else:
            return self.get_value()[ids]
    
    def __setitem__(self, ids, values):
        #print '__setitem__',ids,values,type(self.get_value())
        if hasattr(ids, '__iter__'):
            i = 0
            array = self.get_value()
            for _id in ids:
                array[_id] = values[i]
                i+=1
        else:
            self.get_value()[ids]=values


    
    
            
    def add(self, ids, values = None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = [values]
        else:
            _ids = ids
            _values = values
          
        if values == None:
            _values = self.get_defaults(_ids)
            
        #print 'add ids, _values',ids, _values 
        self[_ids] = _values

    def add_plugin(self, ids, values = None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = [values]
        else:
            _ids = ids
            _values = values
            
        print 'add ids, _values',ids, _values     
        if values == None:
            _values = self.get_defaults(_ids)
        self[_ids] = _values

        if self._plugin:
            self._plugin.exec_events_ids(EVTADDITEM, _ids)
        
          
    def get(self,ids):
        """
        Central function to get the attribute value associated with ids.
        should be overridden by specific array configuration classes
        """
        return self[ids]
            
    def get_plugin(self,ids):
        """
        Central function to get the attribute value associated with ids.
        should be overridden by specific array configuration classes
        """
        if self._plugin:
            if not hasattr(ids, '__iter__'):
                self._plugin.exec_events_ids(EVTGETITEM, [ids])
            else:
                self._plugin.exec_events_ids(EVTGETITEM, ids)
        return self[ids]
            
    def set(self, ids, values):
        """
        Returns value of array element for all ids.
        """
        self[ids] = values
        #print 'set',self.attrname
        

        
    def set_plugin(self, ids, values):
        """
        Returns value of array element for all ids.
        """
        self[ids] = values
        #print 'set',self.attrname
        
        if self._plugin:
            if not hasattr(ids, '__iter__'):
                self._plugin.exec_events_ids(EVTGETITEM, [ids])
            else:
                self._plugin.exec_events_ids(EVTGETITEM, ids)
        
           
            
    def delete(self,ids):
        """
        removes key from array structure
        To be overridden
        """
        del self[ids] 
            
    def delete_plugin(self,ids):
        """
        removes key from array structure
        To be overridden
        """
        if self._plugin:
            if not hasattr(ids, '__iter__'):
                self._plugin.exec_events_ids(EVTGETITEM, [ids])
            else:
                self._plugin.exec_events_ids(EVTGETITEM, ids)
        
        del self[ids]        


    def format_value(self,_id, show_unit = False, show_parentesis=False):
        if show_unit:
            unit = ' '+self.format_unit(show_parentesis)
        else:
            unit = ''
        return repr(self[_id])+unit

        
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

class IdsConf(ColConf):
    """
    Column, where each entry is the id of a single Table. 
    """
    
    def __init__(self,attrname, tab, is_child = True, **kwargs):
        self._tab = tab
        self._is_child = is_child
        AttrConf.__init__(self,  attrname, 
                                -1, # default id 
                                struct = 'odict', 
                                metatype = 'id_tabobj',
                                perm = 'r',
                                **kwargs
                                )
                                                            
    
             
    
          
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.attrname,ids,self.get_default()
        #values = []
        #for _id in ids:
        #    values.append(-1)
        return len(ids)*[-1]


    
    
        
    def set_obj(self, obj):
        """
        Method to set instance of managed object.
        Used by add method of AttrManager
        """
        print 'IdsConf.set_obj',self.attrname,obj.ident
        AttrConf.set_obj(self, obj)
        
        if self._is_child:
            obj.set_child(self) 
    
    #def get_valueobj(self, id = None):
    #    """
    #    This is called by get_childobj to retrive the child instance.
    #    Here this is just the table.
    #    """
    #    return self._tab
    
    def predelete(self):
        AttrConf.predelete(self)
        if self._is_child:
            self.get_obj().del_child(self.attrname)
    
     
    
    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        if self._is_save:
            if self._is_child:
                # OK self.value already set in
                pass
            else:
                # remove table reference and create ident
                state['_tab']= None
                state['_ident_tab'] = self._tab.get_ident_abs()
    
    def init_postload_internal(self, man, obj):
        print 'IdsConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident
        
        AttrConf.init_postload_internal(self, man, obj)
        if self._is_child:
            print '  make sure children get initialized'
            print '  call init_postload_internal of',self._tab.ident
            self._tab.init_postload_internal(obj)
    
    def init_postload_external(self):
        if self._is_child:
            # restore normally
            AttrConf.init_postload_external(self)
            self._tab.init_postload_external()
        else:
            # Substitute absolute ident with link object.
            # Called from init_postload_external of attrsman during load_obj
            # 
            ident_abs = self._ident_tab
            print 'reset_linkobj',self.attrname,ident_abs
            obj = self.get_obj()
            rootobj = obj.get_root()
            print '  rootobj',rootobj.ident
            linkobj = rootobj.get_obj_from_ident(ident_abs)
            print '  linkobj',linkobj.ident
            self._tab = linkobj
        
    #def get_name(self):
    #    return self.'Table ID for '+self._tab.get_name()
    #
    #def get_info(self):
    #    return 'ID for Table:\n'+self._tab.get_info()
       

class ObjsConf(ColConf):
    """
    Column, where each entry is an object of class objclass with 
    ident= (attrname, id). 
    """
    
    def __init__(self,attrname,  **kwargs):
        self._is_child = True# at the moment no links possible
        AttrConf.__init__(self,  attrname, 
                                BaseObjman('empty'), # default id 
                                struct = 'odict', 
                                metatype = 'objs',
                                perm = 'r',
                                **kwargs
                                )
                                                            
    
             
    
          
    
        
    def set_obj(self, obj):
        """
        Method to set instance of managed object.
        Used by add method of AttrManager
        """
        print 'set_obj',self.attrname,obj.ident
        AttrConf.set_obj(self, obj)
        
        #if self._is_child:
        obj.set_child(self) 
    
    #def get_valueobj(self, id = None):
    #    """
    #    This is called by get_childobj to retrive the child instance.
    #    Here this is just the table.
    #    """
    #    return self._tab
    
    def predelete(self):
        AttrConf.predelete(self)
        #if self._is_child:
        self.get_obj().del_child(self.attrname)
    
     
    
    #def _getstate_specific(self, state):
    #    """
    #    Called by __getstate__ to add/change specific states,
    #    before returning states.
    #    To be overridden.
    #    """
    #    if self._is_save:
    #        if self._is_child:
    #            # OK self.value already set in
    #            pass
    #        else:
    #            # remove column reference and create column with idents
    #            state['value']= None
    #            idents_obj = OrderedDict()
    #            linkobjs = self.get_value()
    #            for _id in self.get_ids():
    #                idents_obj[_id] = linkobjs[_id].get_ident_abs()
    #            state['_idents_obj'] = idents_obj
    
    
    def init_postload_internal(self, man, obj):
        print 'ObjsConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident
        
        AttrConf.init_postload_internal(self, man, obj)
        #if self._is_child:
            
        # make sure all children in column get initialized
        print '  make sure childrenS get initialized'
        childobjs = self.get_value()
        
        obj = self.get_obj()
        #print 'init_postload_internal',self.attrname,obj,obj.ident
        for _id in self.get_obj().get_ids():
            print '  call init_postload_internal of',childobjs[_id].ident
            childobjs[_id].init_postload_internal(obj)# attention obj is the parent object!

    def init_postload_external(self):
        #if self._is_child:
        # restore normally
        AttrConf.init_postload_external(self)
        childobjs = self.get_value()
        
        for _id in self.get_obj().get_ids():
            childobjs[_id].init_postload_external()

        
    #def get_name(self):
    #    return self.'Table ID for '+self._tab.get_name()
    #
    #def get_info(self):
    #    return 'ID for Table:\n'+self._tab.get_info()
       
          
    
class Attrsman:
    """
    Manages all attributes of an object
    
    if argument obj is specified with an instance
    then  attributes are stored under this instance.
    The values of attrname is then directly accessible with
    
    obj.attrname
    
    If nothing is specified, then column attribute will be stored under
    the respective config instance of this  attrsman (self).
    The values of attrname is then directly accessible with
    
    self.attrname.value
    """
    def __init__(self, obj, attrname = 'attrsman', is_plugin = False):
        
        
        
        if obj==None:
            # this means that column data will be stored 
            # in value attribute of attrconfigs
            obj = self 
            self._is_localvalue = True
        else:
            # this means that column data will be stored under obj
            self._is_localvalue = False
        
        self._obj = obj # managed object
        self._attrconfigs = []  # managed attribute config instances
        self.attrname = attrname # the manager's attribute name in the obj instance
        
        # groupes of attributes 
        # key=groupname, value = list of attribute config instances
        self._groups = {} 
        
        if is_plugin:
            self.plugin = Plugin(obj)
        else:
            self.plugin = None
        
    def is_localvalue(self):
        return self._is_localvalue   
    
    
     
   
           
    def has_attrname(self,attrname):
        # attention this is a trick, exploiting the fact that the
        # attribute object with all the attr info is an attribute
        # of the attr manager (=self) 
        return hasattr(self,attrname)
    
    def is_modified(self):
        for attrconf in self._attrconfigs:
            if attrconf.is_modified(): return True
        return False
    
    def get_modified(self):
        modified = []
        for attrconf in self._attrconfigs:
            if attrconf.is_modified(): 
                modified.append(attrconf)
        return modified

    def get_configs(self, is_all = False, structs = None):
        if is_all:
            return self._attrconfigs
        else:
            attrconfigs = []
            for attrconf in self._attrconfigs:
                is_check = True
                if (structs != None):
                    if (attrconf.struct not in structs):
                       is_check = False
                
                if is_check:     
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
        
        # set configuration object as attribute of AttrManager
        setattr(self, attrname, attrconf)
        
        # append also to the list of managed objects
        self._attrconfigs.append(attrconf)
        
        # insert in groups
        if len(attrconf.groupnames) > 0:
            for groupname in attrconf.groupnames:
                
                if not self._groups.has_key(groupname):
                    self._groups[groupname]=[]
                
                self._groups[groupname].append(attrconf)
            
        if self.plugin:
            self.plugin.exec_events(EVTADD, attrconf)
            
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
    
    def print_attrs(self, show_unit = True, show_parentesis=False, attrconfigs = None):
        print 'Attributes of',self._obj.name,'ident_abs=',self._obj.get_ident_abs()
        if attrconfigs== None:
            attrconfigs = self.get_configs()
            
        for attrconf in attrconfigs:
            print '  %s =\t %s'%(attrconf.attrname, attrconf.format_value(show_unit=True))       
           
                
    def save_values(self, state):
        """
        Called by the managed object during save to save the
        attribute values. 
        """
        for attrconfig in self.get_configs():
            attrconfig.save_value(state)
            
    def delete(self, attrname):
        """
        Delete attibite with respective name
        """
        #print '.__delitem__','attrname=',attrname
        
        #if hasattr(self,attrname): 
        attrconf = getattr(self,attrname)
        
        
        if attrconf in self._attrconfigs:
            if self.plugin:
                self.plugin.exec_events(EVTDEL, attrconf)
            
            for groupname in attrconf.groupnames:
                    self._groups[groupname].remove(attrconf)
                    
            self._attrconfigs.remove(attrconf)
            attrconf.predelete() # this will remove also the value attribute
            
            #attrname = attrconf.attrname
            del self.__dict__[attrname]    # delete config
            return True
        
        return False # attribute not managed
        #return False # attribute not existant
    
    
    def  __getstate__(self):
        #print '__getstate__',self.attrname
        #print '  self.__dict__=\n',self.__dict__.keys()
        
        state={} 
        for attr in self.__dict__.keys():
                #print '  attr',attr,self.__dict__[attr]
                if attr =='plugin':
                    plugin = self.__dict__[attr]
                    if plugin!=None:
                        state[attr] = True
                    else:
                        state[attr] = False
                    
                elif attr not in ATTRS_NOSAVE:
                    state[attr] = self.__dict__[attr]
        #print  '  state=', state          
        return state
    
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        #self.plugins={}
        
        for attr in state.keys():
            #print '  set state',attr
            if attr=='plugin':
                if state[attr]==True:
                    self.__dict__[attr] = Plugin(self)
                else:
                    self.__dict__[attr]= None
            else:
                self.__dict__[attr]=state[attr]

    def init_postload_internal(self, obj):
        """
        Called after set state.
        Link internal states.
        """
        print 'Attrsman.init_postload_internal of obj:',obj.ident
        
        self._obj = obj
        for attrconfig in  self.get_configs(is_all = True):
            print '  call init_postload_internal of',attrconfig.attrname
            attrconfig.init_postload_internal(self, obj)
            
    def init_postload_external(self):
        """
        Called after set state.
        Link external states.
        """
        #print 'init_postload_external',self._obj.get_ident()
        
            
        for attrconfig in  self.get_configs(is_all = True):
            #print '  **',attrconfig.attrname,attrconfig.metatype
            attrconfig.init_postload_external()

        

class Tabman(Attrsman):
    """
    Manages all table attributes of an object.
    
    if argument obj is specified with an instance
    then column attributes are stored under this instance.
    The values of attrname is then directly accessible with
    
    obj.attrname
    
    If nothing is specified, then column attribute will be stored under
    the respective config instance of this tab man (self).
    The values of attrname is then directly accessible with
    
    self.attrname.value
    
    """
    def __init__(self, obj=None, **kwargs):
                
        Attrsman.__init__(self, obj, **kwargs)
        self._colconfigs = [] 
        self._ids = []
        
     
    
    def add_col(self,attrconf):
        Attrsman.add(self, attrconf) # insert in common attrs database
        self._colconfigs.append(attrconf)
        # returns initial array and also create local array if self._is_localvalue == True
        return  attrconf.get_init()
    
    def delete(self, attrname):
        """
        Delete attibite with respective name
        """
        #print '.__delitem__','attrname=',attrname
        
        if hasattr(self,attrname): 
            attrconf = getattr(self,attrname)
            if Attrsman.delete(self,attrname):
                if attrconf in self._colconfigs:
                    self._colconfigs.remove(attrconf)
            
    
    
        
    def get_colconfigs(self, is_all = False):
        if is_all:
            return self._colconfigs
        else:
            colconfigs = []
            for colconfig in self._colconfigs:
                if len(colconfig.groupnames)>0:
                   if colconfig.groupnames[0]!='_private':
                        colconfigs.append(colconfig)
                else:
                    colconfigs.append(colconfig)
                 
            return colconfigs
                    
    def get_ids(self):
        return self._ids
    
    def __len__(self):
        """
        Determine current array length (same for all arrays)
        """
        
        return len(self._ids)
    
    def select_ids(self,mask):
        
        ids_mask = []
        i= 0
        for _id in self.get_ids():
            if mask[i]:
               ids_mask.append(_id)
            i+=1
            
        return ids_mask
    
    def suggest_id(self,is_zeroid=False):
        """
        Returns a an availlable id.
        
        Options:
            is_zeroid=True allows id to be zero.
            
        """
        if is_zeroid:
            id0=0
        else:
            id0=1
        
        id_set = set(self.get_ids())
        if len(id_set)==0:
            id_max = 0
        else:
            id_max = max(id_set)
        #print  'suggest_id',id0,
        return list(id_set.symmetric_difference(xrange(id0,id_max+id0+1)))[0]
        
    
    def suggest_ids(self,n,is_zeroid=False):
        """
        Returns a list of n availlable ids.
        It returns even a list for n=1. 
        
        Options:
            is_zeroid=True allows id to be zero.
        """
        if is_zeroid:
            id0=0
        else:
            id0=1
        id_set = set(self.get_ids())
        if len(id_set)==0:
            id_max = 0
        else:
            id_max = max(id_set)
        return list(id_set.symmetric_difference(xrange(id0,id_max+id0+n)))[:n]
    
    def add_rows(self, n=None, ids = [], **attrs):
        if n != None:
            ids = self.suggest_ids(n)
        self._ids += ids
        #print 'add_rows ids', ids
        for colconfig in self._colconfigs:
            colconfig.add(ids, values = attrs.get(colconfig.attrname, None ))
        return ids    
    
    def del_rows(self, ids):
        for colconfig in self._colconfigs:
            del colconfig[ids]
        
        for _id in ids:
                self._ids.remove(_id)

    
    def del_row(self, _id):
        for colconfig in self._colconfigs:
            del colconfig[_id]
        self._ids.remove(_id)
    
    
    def __delitem__(self,ids):
        """
        remove rows correspondent to the given ids from all array and dict
        attributes
        """
        if hasattr(ids, '__iter__'):
            self.del_rows(ids)
        else:
            self.del_row(ids)
                        
    def print_attrs(self, **kwargs):
        #print 'Attributes of',self._obj.name,'(ident=%s)'%self._obj.ident
        Attrsman.print_attrs(self, attrconfigs = self.get_configs(structs = ['scalar']),**kwargs)
        for _id in self.get_ids():
            for attrconf in self.get_configs(structs = STRUCTS_COL):
                print '  %s[%d] =\t %s'%(attrconf.attrname, _id, attrconf.format_value(_id,show_unit=True)) 
                
class BaseObjman:
    """
    Object management base methods to be inherited by all object managers.
    """
    def __init__(self, ident,  **kwargs):
            self._init_objman(ident, **kwargs)
            self.attrsman = self.set_attrsman(Attrsman(self))
            
    def _init_objman(self, ident='no_ident', parent=None, name= None, managertype = 'basic', info = None):
        self.managertype = managertype
        self.ident = ident                
        if name == None:
            self.name = ident
        else:
            self.name = name
        
  
        self.parent = parent
        self.childs = {}# dict with attrname as key and child instance as value
        
        self._info = info
        
        self._is_saved = False
    
  
        
    def __repr__(self):
        #return '|'+self.name+'|'
        return self.format_ident()
        
    def is_modified(self):
        return self.attrsman.is_modified()
        
    def get_name(self):
        return self.name
    
    def get_info(self):
        if self._info == '':
            return self.__doc__
        else:
            return self._info
    
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
    
    def get_obj_from_ident(self,ident_abs):
        #print 'get_obj_from_ident',self.ident,ident_abs
        if len(ident_abs)==1:
            # arrived at the last element
            # check if it corresponds to the present object
            if ident_abs[0] == self.ident:
                return self
            else:
                return None # could throw an error
        else:
            return self.get_childobj(ident_abs[1]).get_obj_from_ident(ident_abs[1:])
    
    def get_childobj(self,attrname):
        """
        Return child instance
        """  
        config = self.childs[attrname]
        return config.get_value()

    
    
    
    def set_child(self,childconfig):
        """
        Set child childconfig
        """  
        self.childs[childconfig.attrname] = childconfig
    
    def del_child(self,attrname):
        """
        Return child instance
        """  
        del self.childs[attrname]
    
    
    def format_ident_abs(self):
        s = ''
        #print 'format_ident_abs',self.get_ident_abs()
        for ident in self.get_ident_abs():
            s += str(ident)+'.'
        return s[:-1]
    
    def get_parent(self):
        return self.parent
        
    #def reset_parent(self, parent):
    #    self.parent=parent
    
     
    def set_attrsman(self, attrsman):
        # for quicker acces and because it is only on
        # the attribute management is public and also directly accessible
        #setattr(self, attrname,Attrsman(self))# attribute management
        self.attrsman = attrsman
        return attrsman
        
    def get_attrsman(self):
        return self.attrsman
       
    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        pass      
              
    def  __getstate__(self):
        #print '__getstate__',self.ident,self._is_saved
        #print '  self.__dict__=\n',self.__dict__.keys()
        state={}
        if not self._is_saved:
            
            #print  '  save standart values'
            for attr in ATTRS_SAVE:
                if hasattr(self,attr):
                    state[attr]=getattr(self,attr) 
            
            #print '  save all scalar stuctured attributes'
            # attrsman knows which and how
            ## self.attrsman.save_values(state)
            #
            # values of configured attributes are not saved here
            # values are now ALWAYS stored in the value attribute of the 
            # attrconfig and reset in main obj 
            
            #print '  save also attrsman'
            state['attrsman'] = self.attrsman
            self._getstate_specific(state)
            self._is_saved = True
            
        else:
            print 'WARNING in __getstate__: object %s already saved'%self.ident    
        return state
    
    def __setstate__(self,state):
        #print '__setstate__',self
        
        # this is always required, but will not be saved
        #self.plugins={}
        
        for key in state.keys():
            #print '  set state',key
            self.__dict__[key]=state[key]
        
        
        # done in init2_config...
        # set default values for all states tha have not been saved
        #for attr in self._config.keys():
        #    if (not self._config[attr]['save']) & (not hasattr(self,attr)):
        #        print '  config attr',attr
        #        self.config(attr,**self._config[attr])
            
        # set other states       
        #self._setstate(state)
        
    def init_postload_internal(self, parent):
        """
        Called after set state.
        Link internal states.
        """
        print 'BaseObjman.init_postload_internal',self.ident,'parent:',
        if parent != None:
            print parent.ident
        else:
            print 'ROOT'
        self.parent = parent
        self.childs = {}
        self.attrsman.init_postload_internal(self)
        self._is_saved = False
        
       
    def  init_postload_external(self):
        """
        Called after set state.
        Link internal states.
        """
        #for child in self.childs.values():
        #    child.reset_parent(self)
        self.attrsman.init_postload_external()
        
         

class TableObjman(Tabman, BaseObjman):
    """
    Table Object management manages objects with list and dict based columns. 
    For faster operation use ArrayObjman in arrayman package, which requires numpy.
    """
    def __init__(self,ident, **kwargs):
        self._init_objman( ident, **kwargs)
        
    def _init_objman(self, ident,**kwargs):
        BaseObjman._init_objman(self, ident, managertype = 'table', **kwargs)
        Tabman.__init__(self)
        #self.set_attrsman(self)
   
        
    
    def get_obj_from_ident(self,ident_abs):
        print 'get_obj_from_ident',self.ident,ident_abs,type(ident_abs)
        if len(ident_abs)==1:
            # arrived at the last element
            # check if it corresponds to the present object
            ident_check = ident_abs[0]
            
            #  now 2 things can happen:
            # 1.) the ident is a simple string identical to ident of the object
            # in this case, return the whole object
            # 2.) ident is a tuple with string and id
            # in this case return object and ID 
            #if hasattr(ident_check, '__iter__'):
            #    #if (ident_check[0] == self.ident)&(ident_check[1] in self._ids):
            #    if ident_check[1] in self._ids:
            #        return (self, ident_check[1])
            #    else:
            #       return None # could throw an error
            #else:
            if ident_check == self.ident:
                    return self
        else:
            childobj = self.get_childobj(ident_abs[1])
            return childobj.get_obj_from_ident(ident_abs[1:])

    def get_childobj(self,ident):
        """
        Return child instance.
        This is any object with ident
        """  
        if hasattr(ident, '__iter__'):
            # access of ObjsConf configured child
            # get object from column attrname
            attrname , _id = ident
            config = self.childs[attrname]
            return config[_id] #config.get_valueobj(_id)
        else:
            # access of ObjConf configured child
            # get object from  attrname
            config = self.childs[ident]
            return config.get_value()
    

    


    
             
    def  __getstate__(self):
        #print '__getstate__',self.ident,self._is_saved
        #print '  self.__dict__=\n',self.__dict__.keys()
        state={}
        if not self._is_saved:
            
            #print  '  save standart values'
            for attr in ATTRS_SAVE+ATTRS_SAVE_TABLE:
                if attr =='plugin':
                    plugin = self.__dict__[attr]
                    if plugin!=None:
                        state[attr] = True
                    else:
                        state[attr] = False
                        
                elif hasattr(self,attr):
                    state[attr]=getattr(self,attr) 
            
            # save managed attributes !!!
            for attrconfig in self.get_configs(is_all = True):
                state[attrconfig.attrname]=attrconfig
                
            #print '  save all scalar stuctured attributes'
            # attrsman knows which and how
            #self.save_values(state)
            
            #print '  save also attrsman'
            #state['attrsman'] = self.attrsman
            self._is_saved = True
            
        else:
            print 'WARNING in __getstate__: object %s already saved'%self.ident    
        return state
    
    
    
    
    def __setstate__(self,state):
        #print '__setstate__',self.ident
        
        # this is always required, but will not be saved
        self.plugins={}
        
        for attr in state.keys():
            #print '  set state',key
            if attr=='plugin':
                if state[attr]==True:
                    self.__dict__[attr] = Plugin(self)
                else:
                    self.__dict__[attr]= None
            else:
                self.__dict__[attr]=state[attr]
        
        
        # done in init2_config...
        # set default values for all states tha have not been saved
        #for attr in self._config.keys():
        #    if (not self._config[attr]['save']) & (not hasattr(self,attr)):
        #        print '  config attr',attr
        #        self.config(attr,**self._config[attr])
            
        # set other states       
        #self._setstate(state)
        
    
    
    
        
    def init_postload_internal(self, parent):
        """
        Called after set state.
        Link internal states.
        """
        print 'TableObjman.init_postload_internal',self.ident,'parent:',
        if parent != None:
            print parent.ident
        else:
            print 'ROOT'
            
        self.parent = parent
        self.childs = {}
        Attrsman.init_postload_internal(self, self)
        self._is_saved = False
        
       
    def  init_postload_external(self):
        """
        Called after set state.
        Link internal states.
        """
        Attrsman.init_postload_external(self)
              
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
    
        
class TableManager(Attrsman,BaseObjman):
    """
    This class is for managing a table within a class.
    In this case specified indexable attributes are columns
    and their entries are the rows.
    
    An element of the table can be called with:
        obj.attrsman.attr[id]
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
            #BaseObjman._init_objman(self, ident=obj.ident, parent=obj.parent, name= obj.name)
        
        # these makes it a full object with attribute management
        # and itself as trhe only table
        BaseObjman._init_objman(self, ident=ident, parent=parent, name= name)
        self.managertype = 'table'
        self.attrsman = self.set_attrsman(Attrsman(self,'attrsman'))
        self.add_tableman(self)
        
        # publicly accessible column instance
        # where attributes corrispond to table columns
        # columns are either numpy arrays, lists or dictionaries      
        # (this is the same as the self._obj)
        self.cols = obj # 
        
        
        
        Attrsman.__init__(self, obj,'_attrs',**kwargs )


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
            return BaseObjman.get_ident_abs(self)
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
        
        Attrsman.add(self, attrconf) # insert in common attrs database
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
    #    # return [self.attrsman.add_myrow,]
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
        #if Attrsman.__delitem__(attr):
        
        
        
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
        
        if Attrsman.delete(self,attr):
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
    
 

class MultitabManagerMixin:
    """
    TODO...
    Multiple table management methods.
    """
    def _init_objman(self, ident='no_ident', parent=None, name= None):
        self.managertype = 'multitab'
        self.ident = ident                
        if name == None:
            self.name = ident
        else:
            self.name = name
        
        #self._is_modified = False
        
    
    
        self._tablemans = []
        self.reset_parent(parent)
        
    #def set_modified(self, is_modified):
    #    print '**set_modified',self.ident
    #    if is_modified & (not self._is_modified):
    #        self._is_modified = is_modified
    #        if (self.parent!=None)&is_modified:
    #            self.parent.set_modified(True)
    #    else:
    #        self._is_modified = is_modified
        
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

    def set_attrsman(self, attrsman):
        # for quicker acces and because it is only on
        # the attribute management is public and also directly accessible
        #setattr(self, attrname,Attrsman(self))# attribute management
        self._attrsmanname = attrsman.attrname
        return attrsman
        
    def get_attrsman(self):
        return getattr(self, self._attrsmanname)
    
    def add_tableman(self, tableman):
        self._tablemans.append(tableman)
        return tableman
        
    def get_tablemans(self):
        return self._tablemans
    

    
        


 
 
                        


                                  
###############################################################################
if __name__ == '__main__':
    """
    Test
    """
    
    pass
    
    
                                                           