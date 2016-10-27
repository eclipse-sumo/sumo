
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
#from datetime import datetime
#import numpy as np
import xmlmanager as xm
from logger import Logger


##########


 
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
ATTRS_SAVE = ('ident','_name','managertype','_info')  
ATTRS_SAVE_TABLE = ('_is_localvalue','attrname','_colconfigs','_ids','_inds','_attrconfigs','_groups','plugin','_is_indexing', '_index_to_id','_id_to_index')       

STRUCTS_COL = ('odict','array') 
STRUCTS_SCALAR = ('scalar','list','matrix') 

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
        #if struct == 'None':
        #    if hasattr(default, '__iter__'):
        #        struct = 'scalar'
        #    else:
        #        struct = 'list'
         
            
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
    
    def get_metatype(self):
        return self.metatype
                
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
        return self.get_info() != None
    
    def is_colattr(self):
        return hasattr(self, '__getitem__')
    
    def get_info(self):
        if self._info == None:
            return self.__doc__
        else:
            return self._info
       
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
        #return repr(self.get_value())+unit
        return str(self.get_value())+unit
    ####
    def get_value(self):
        # always return attribute, no indexing, no plugin
        if self._is_localvalue:
            return self.value
        else:
            return getattr(self._obj, self.attrname)
    
    
    def set_value(self, value):
        #set entire attribute, no indexing, no plugin
        #print 'AttrConf.set_value',self.attrname, self._is_localvalue, value, type(value)
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
            
class NumConf(AttrConf):
    """
    Contains additional information on the object's attribute.
    Here specific number related attributes are defined.
    """
    def __init__(self, attrname, default,
                    digits_integer = None, digits_fraction=None ,
                    minval = None, maxval = None,
                    **kwargs):
        self.min = minval
        self.max = maxval
        self.digits_integer = digits_integer
        self.digits_fraction = digits_fraction
        
        AttrConf.__init__(self,  attrname, default, metatype = 'number',
                                **kwargs
                                )

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
        #print 'ObjConf.set_obj',self.attrname,obj.ident
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
     
    
    
     
 

class Indexing:
    """
    Mixing to allow any column attribute to be used as index. 
    """
    def _init_indexing(self):
        """
        Init Indexing management attributes.
        """
        
        self._index_to_id = {}   
        
        
    def get_id_from_index(self, index):
        return self._index_to_id[index]
    
    def get_ids_from_indices(self, indices):
        ids = len(indices)*[0]
        for i in range(len(indices)):
            ids[i]=self._index_to_id[indices[i]]
        return ids
    
    
    def add_indices(self, ids, indices):
        for _id, index in zip(ids,indices):
            self.add_index(_id, index)   
    
    def add_index(self,_id, index):
        self._index_to_id[index] = _id
    
    def del_indices(self, ids):
        for _id in ids:
            self.del_index(_id)   
    
    def del_index(self,_id):
        index = self[_id]
        del self._index_to_id[index]
  
  
  
        
class ColConf(Indexing, AttrConf):
    """
    Basic column configuration.
    Here an ordered dictionary is used to represent the data.
    #>>> from collections import OrderedDict
    #>>> spam = OrderedDict([('s',(1,2)),('p',(3,4)),('a',(5,6)),('m',(7,8))])
    >>> spam.values()
  
    """
    #def __init__(self, **attrs):
    #    print 'ColConf',attrs
        
    def __init__(self, attrname, default, is_index=False, **attrs):
        #print 'ColConf',attrs     
        self._is_index = is_index         
        AttrConf.__init__(self,  attrname, default,
                    struct = 'odict', 
                    **attrs)
        
        if is_index:
            self._init_indexing()
        
            
    def is_index(self):
        return   self._is_index    
    
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
            if self._is_index:
                self.del_indices(ids)
            
            array = self.get_value()
            for _id in ids:
                del array[_id]
            
        else:
            if self._is_index:
                self.del_index(ids)
            
            del self.get_value()[ids]
            
                
    def delete_item(self, _id):   
        #print '        before=\n',self.__dict__[attr]
        #attr = self.attrconf.get_attr()
        del self.get_value()[_id]
        
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
        if self._is_index:
                self.add_indices(_ids, _values)

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
        
        if self._is_index:
                self.add_indices(_ids, _values)
        
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
        #return repr(self[_id])+unit
        return str(self[_id])+unit
        
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

class NumcolConf(ColConf):
    def __init__(   self, attrname, default, 
                    digits_integer = None, digits_fraction=None ,
                    minval = None, maxval = None,
                    **attrs):
        self.min = minval
        self.max = maxval
        self.digits_integer = digits_integer
        self.digits_fraction = digits_fraction
            
        ColConf.__init__(self,attrname, default,**attrs) 
        
class IdsConf(ColConf):
    """
    Column, where each entry is the id of a single Table. 
    """
    
    def __init__(self,attrname, tab,  is_index = False, **kwargs):
        self._is_index = is_index
        self._tab = tab
        AttrConf.__init__(self,  attrname, 
                                -1, # default id 
                                struct = 'odict', 
                                metatype = 'id',
                                perm = 'r',
                                **kwargs
                                )
                                                            
    
             
    def set_linktab(self, tab):
        self._tab = tab
        
    def get_linktab(self):
        return  self._tab     
    
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.attrname,ids,self.get_default()
        return len(ids)*[-1]


    
    
        


    
    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        if self._is_save:
            #if self._is_child:
            #    # OK self.value already set in
            #    pass
            #else:
            #    # remove table reference and create ident
            state['_tab']= None
            state['_ident_tab'] = self._tab.get_ident_abs()
    
    def init_postload_internal(self, man, obj):
        #print 'IdsConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident
        
        AttrConf.init_postload_internal(self, man, obj)
        #if self._is_child:
        #    print '  make sure children get initialized'
        #    print '  call init_postload_internal of',self._tab.ident
        #    self._tab.init_postload_internal(obj)
    
    def init_postload_external(self):
        #if self._is_child:
        #    # restore normally
        #    AttrConf.init_postload_external(self)
        #    self._tab.init_postload_external()
        #else:
        
        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        # 
        ident_abs = self._ident_tab
        #print 'reset_linkobj',self.attrname,ident_abs
        obj = self.get_obj()
        rootobj = obj.get_root()
        #print '  rootobj',rootobj.ident
        linkobj = rootobj.get_obj_from_ident(ident_abs)
        #print '  linkobj',linkobj.ident
        self._tab = linkobj
        

class TabIdsConf(ColConf):
    """
    Column, where each entry contains a tuple with table object and id. 
    """
    
    def __init__(self,attrname,  is_index = False, **kwargs):
        self._is_index = is_index
        AttrConf.__init__(self,  attrname, 
                                -1, # default id 
                                struct = 'odict', 
                                metatype = 'tabids',
                                **kwargs
                                )
                                                            
    
             
    
          
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.attrname,ids,self.get_default()
        return len(ids)*[(None,-1)]


    


    
    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        if self._is_save:
            n = len(state['value'])
            state['value']= None
            _tabids_save = n*[None]
            i = 0
            for tab, ids in self.get_value():
                _tabids_save[i] = [tab.get_ident_abs(), ids]
                i += 1
            state['_tabids_save'] = _tabids_save
            

    
    def init_postload_internal(self, man, obj):
        #print 'IdsConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident
        
        AttrConf.init_postload_internal(self, man, obj)
        #if self._is_child:
        #    print '  make sure children get initialized'
        #    print '  call init_postload_internal of',self._tab.ident
        #    self._tab.init_postload_internal(obj)
    
    def init_postload_external(self):
        #if self._is_child:
        #    # restore normally
        #    AttrConf.init_postload_external(self)
        #    self._tab.init_postload_external()
        #else:
        
        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        # 
        #ident_abs = self._ident_tab
        #print 'reset_linkobj',self.attrname,ident_abs
        #obj = self.get_obj()
        #rootobj = obj.get_root()
        #print '  rootobj',rootobj.ident
        #linkobj = rootobj.get_obj_from_ident(ident_abs)
        #print '  linkobj',linkobj.ident
        #self._tab = linkobj
        
        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        # 
        _tabids_save = self._tabids_save
        #ident_abs = self._ident_value
        print 'init_postload_external',self.attrname,_tabids_save
        obj = self.get_obj()
        rootobj = obj.get_root()
        print '  rootobj',rootobj.ident
        tabids = len(self._tabids_save)*[None]
        i = 0
        for tabident, ids in self._tabids_save:
            tab = rootobj.get_obj_from_ident(tabident)
            print '  ',tab.get_ident_abs(), ids
            tabids[i] = [tab, ids]
            i += 1
                
        self.set_value(tabids)
        
        
class ObjsConf(ColConf):
    """
    Column, where each entry is an object of class objclass with 
    ident= (attrname, id). 
    """
    
    def __init__(self,attrname,  is_index = False, **kwargs):
        self._is_index = is_index 
        self._is_child = True# at the moment no links possible
        AttrConf.__init__(self,  attrname, 
                                BaseObjman('empty'), # default id 
                                struct = 'odict', 
                                metatype = 'obj',
                                perm = 'r',
                                **kwargs
                                )
                                                            
    
             
    
          
    
        
    def set_obj(self, obj):
        """
        Method to set instance of managed object.
        Used by add method of AttrManager
        """
        #print 'set_obj',self.attrname,obj.ident
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
        #print 'get_configs',self._obj.ident,structs
        if is_all:
            return self._attrconfigs
        else:
            attrconfigs = []
            for attrconf in self._attrconfigs:
                #print '  found',attrconf.attrname,attrconf.struct
                is_check = True
                if (structs != None):
                    if (attrconf.struct not in structs):
                       is_check = False
                
                if is_check:     
                    #print '  **is_check',is_check
                    if len(attrconf.groupnames)>0:
                       if attrconf.groupnames[0]!='_private':
                            attrconfigs.append(attrconf)
                    else:
                        attrconfigs.append(attrconf)
                     
            return attrconfigs
    
    #def get_colconfigs(self, is_all = False):
    #    return []
        
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
        print 'Attributes of',self._obj._name,'ident_abs=',self._obj.get_ident_abs()
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
    def __init__(self, obj=None,  **kwargs):
        Attrsman.__init__(self, obj, **kwargs)
        self._colconfigs = [] 
        self._ids = []
        
        
    
     
    
    def add_col(self,attrconf):
        #print 'add_col',attrconf.attrname,attrconf.is_index()
        Attrsman.add(self, attrconf) # insert in common attrs database
        self._colconfigs.append(attrconf)
        # returns initial array and also create local array if self._is_localvalue == True
        
            
        return  attrconf.get_init()
    
    def delete(self, attrname):
        """
        Delete attribute with respective name
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
        elif len(ids)==0:
            # get number of rows from any valye vector provided
            ids = self.suggest_ids(len(attrs.values()[0]))
        else:
            # ids already given , no ids to create
            pass
        
        
        self._ids += ids
        #print 'add_rows ids', ids
        for colconfig in self._colconfigs:
            colconfig.add(ids, values = attrs.get(colconfig.attrname, None ))
            
        return ids   
     
    def add_row(self,  **attrs):
        _id = self.suggest_id()
        self._ids += [_id,]
        for colconfig in self._colconfigs:
            colconfig.add(_id, values = attrs.get(colconfig.attrname, None ))
        return _id 
        
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
        #print 'Attributes of',self._obj._name,'(ident=%s)'%self._obj.ident
        Attrsman.print_attrs(self, attrconfigs = self.get_configs(structs = ['scalar']),**kwargs)
        #print '   ids=',self._ids
        for _id in self.get_ids():
            for attrconf in self.get_configs(structs = STRUCTS_COL):
                print '  %s[%d] =\t %s'%(attrconf.attrname, _id, attrconf.format_value(_id,show_unit=True)) 
    
                
class BaseObjman:
    """
    Object management base methods to be inherited by all object managers.
    """
    def __init__(self, ident,  **kwargs):
        #print 'BaseObjman.__init__',kwargs
        self._init_objman(ident, **kwargs)
        self.set_attrsman(Attrsman(self))
        #print 'BaseObjman.__init__',self.format_ident(),'parent=',self.parent
    
    def set_attrsman(self, attrsman):
        self._attrsman = attrsman
        return attrsman
    
    def _init_objman(self, ident='no_ident', parent=None, name= None, 
                        managertype = 'basic', info = None, logger = None):
        #print 'BaseObjman._init_objman',ident,logger
        self.managertype = managertype
        self.ident = ident                
        
        self.set_logger(logger)
  
        self.parent = parent
        self.childs = {}# dict with attrname as key and child instance as value
        
        self._info = info
        
        self._is_saved = False
    
        if name == None:
            self._name = self.format_ident()
        else:
            self._name = name
    
    def get_logger(self):
        #print 'get_logger',self.ident,self._logger,self.parent
        if self._logger!= None:
            return self._logger
        else:
            return self.parent.get_logger()
    
    def set_logger(self, logger):
        #print 'set_logger',self.ident,logger
        self._logger = logger
    
    def __repr__(self):
        #return '|'+self._name+'|'
        return self.format_ident()
        
    def is_modified(self):
        return self._attrsman.is_modified()
        
    def get_name(self):
        return self._name
    
    def get_info(self):
        if self._info == None:
            return self.__doc__
        else:
            return self._info
    
    def get_ident(self):
        return self.ident
    
    def _format_ident(self, ident):
        if hasattr(ident,'__iter__'):
            return str(ident[0])+'#'+str(ident[1])
        else:
            return str(ident)
        
    def format_ident(self):
        return self._format_ident(self.ident)
    
    
    
    def format_ident_abs(self):
        s = ''
        #print 'format_ident_abs',self.get_ident_abs()
        for ident in self.get_ident_abs():
            s += self._format_ident(ident)+'.'
        return s[:-1]
    
    
        
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
    
    
    
    
    def get_parent(self):
        return self.parent
        
    #def reset_parent(self, parent):
    #    self.parent=parent
    
     
    #def set_attrsman(self, attrsman):
    #    # for quicker acces and because it is only on
    #    # the attribute management is public and also directly accessible
    #    #setattr(self, attrname,Attrsman(self))# attribute management
    #    self._attrsman = attrsman
    #    #return attrsman
        
    def get_attrsman(self):
        return self._attrsman
       
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
            ## self._attrsman.save_values(state)
            #
            # values of configured attributes are not saved here
            # values are now ALWAYS stored in the value attribute of the 
            # attrconfig and reset in main obj 
            
            #print '  save also attrsman'
            state['_attrsman'] = self._attrsman
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
        self._attrsman.init_postload_internal(self)
        self._is_saved = False
        
       
    def  init_postload_external(self):
        """
        Called after set state.
        Link internal states.
        """
        #for child in self.childs.values():
        #    child.reset_parent(self)
        self._attrsman.init_postload_external()
        
class TableMixin(BaseObjman):
    
    def format_ident_row(self, _id):
        #print 'format_ident_row',_id
        return self.format_ident()+'['+str(_id)+']'
    
    def format_ident_row_abs(self, _id):
        return self.format_ident_abs()+'['+str(_id)+']'
             
    def get_obj_from_ident(self,ident_abs):
        #print 'get_obj_from_ident',self.ident,ident_abs,type(ident_abs)
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
            #state['attrsman'] = self._attrsman
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
        self.set_attrsman(self)
        Attrsman.init_postload_internal(self, self)
        
        self._is_saved = False
        
       
    def  init_postload_external(self):
        """
        Called after set state.
        Link internal states.
        """
        Attrsman.init_postload_external(self)
        
class TableObjman(Tabman, TableMixin):
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
        self.set_attrsman(self)
        
    
    



                     


                                  
###############################################################################
if __name__ == '__main__':
    """
    Test
    """
    
    pass
    
    
                                                           