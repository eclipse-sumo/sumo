from classman import *
import numpy as np
class ArrayConfMixin:
    def __init__(self, attrname, default, dtype=None, is_index= False, **attrs):
        self._is_index = is_index 
        self._dtype =  dtype               
        AttrConf.__init__(self,  attrname, default,
                    struct = 'array', 
                    **attrs)   
    
        if is_index:
            self._init_indexing()
    
    def get_dtype(self):
        return  self._dtype  
    
    def convert_type(self, array):
        return np.array(array, dtype = self._dtype)
         
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self._name,ids
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
            #print '  return',defaults,defaults.shape,defaults.dtype
            return defaults
        else:
            if self._dtype!=None:
                dtype = self._dtype
            else:
                dtype = type(default) 
            #defaults=  np.array(  len(ids)*[default], dtype )  
            #print '  return 1D',defaults,defaults.shape,defaults.dtype
            return np.array(  len(ids)*[default], dtype )

    def get_init(self):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        ids = self._manager.get_ids()
            
        #print '\n\nget_init',self.attrname,ids,self._is_localvalue
        values = self.get_defaults(ids)

        # store locally if required
        if self._is_localvalue:
            self.value = values
        # pass on to calling instance
        # in this cas the data is stored under self._obj
        return values
            
    
    def __delitem__(self, ids):   
        #print '        before=\n',self.__dict__[attr]
        #attr = self.attrconf.get_value()
        if hasattr(ids, '__iter__'):
            if self._is_index:
                self.del_indices(ids)
            arr = self.get_value()
            for _id in ids:
                i = self._manager._inds[_id]
                self.set_value(np.concatenate( (arr[:i], arr[i+1:]) ) )
        else:
            
            self.delete_item(ids)
                
    def delete_item(self, _id): 
        print 'delete_item'  
        print '  before=\n',self.attrconf.get_value()
        if self._is_index:
            self.del_index(ids)
        arr = self.get_value()
        i = self._manager._inds[_id]
        self.set_value(np.concatenate( (arr[:i], arr[i+1:]) ) )
            
        print '  after=\n',self.attrconf.get_value()
        
    def __getitem__(self, ids):
        #print '__getitem__',key
        return  self.get_value()[self._manager._inds[ids]] 
    
    def __setitem__(self, ids, values):
        #print '__setitem__',ids,values,type(self.get_value()),self.get_value().dtype
        self.get_value()[self._manager._inds[ids]]=values
        
  
            
    def add(self, ids, values = None):
        #print 'add',ids,values
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.array([values],self._dtype)
        else:
            
            _ids = ids
            _values = np.array(values,self._dtype)
          
        if values == None:
            _values = self.get_defaults(_ids)
            
        #print 'add ids, _values',ids, _values
        newvalue = np.concatenate((self.get_value(),_values)) 
        #print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(),_values)) )
        
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
          
        if values == None:
            _values = self.get_defaults(_ids)
            
        #print 'add ids, _values',ids, _values 
        self.set_value(np.concatenate((self.get_value(),_values)) )

        if self._plugin:
            self._plugin.exec_events_ids(EVTADDITEM, _ids)
    
    
   
class ArrayConf(ArrayConfMixin,ColConf):
    """
    Column made of numeric array.
  
    """
    pass
            
class NumArrayConf(ArrayConfMixin,ColConf):
    """
    Column made of numeric array.
  
    """
    #def __init__(self, **attrs):
    #    print 'ColConf',attrs
    def __init__(self, attrname, default, 
                digits_integer = None, digits_fraction=None ,
                minval = None, maxval = None,
                **attrs):
                    
        self.min = minval
        self.max = maxval
        self.digits_integer = digits_integer
        self.digits_fraction = digits_fraction
        
        ArrayConfMixin.__init__(self,  attrname, default, metatype = 'number',**attrs)   
    
  
            
class IdsArrayConf(ArrayConfMixin,ColConf):
    """
    Column, where each entry is the id of a single Table. 
    """
    
    def __init__(self,attrname, tab,  is_index = False, **kwargs):
        self._is_index = is_index
        self._tab = tab
        ArrayConfMixin.__init__(self,  attrname, 
                                -1, # default id 
                                dtype = int, 
                                metatype = 'id',
                                perm = 'r',
                                **kwargs
                                )
                                                            
    
             
    
        
    
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.attrname,ids,self.get_default()
        return -np.ones(len(ids),dtype = self._dtype)


    
    
        
#-------------------------------------------------------------------------------
    # copied from IdsConf!!!
    def get_linktab(self):
        return  self._tab 
    
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
      


class TabIdsArrayConf(ArrayConfMixin,ColConf):
    """
    Column, where each entry contains a tuple with table object and id. 
    """
    
    def __init__(self,attrname,  is_index = False, **kwargs):
        self._is_index = is_index
        ArrayConfMixin.__init__(self,  attrname, 
                                (None, -1), # default id 
                                dtype = [('ob', object), ('id', np.int)],
                                metatype = 'tabid',
                                perm = 'r',
                                **kwargs
                                )
                                                            
    
             
    
        
    
    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        #print '\n\nget_defaults',self.attrname,ids,self.get_default()
        return np.zeros(len(ids),dtype = self._dtype)

    
    


    


    
    

    

    
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
        tabids = np.zeros(len(self._tabids_save) , dtype= self.dtype)

        i = 0
        for tabident, ids in self._tabids_save:
            tab = rootobj.get_obj_from_ident(tabident)
            print '  ',tab.get_ident_abs(), ids
            tabids[i] = (tab, ids)
            i += 1
                
        self.set_value(tabids)
            
    
class Arrayman(Tabman):
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
        self._inds = np.zeros((0,),int)
        self._ids = np.zeros((0,),int)
           
    
    def select_ids(self,mask):
        print 'select_ids'
        print '  mask\n=',mask
        print '  self._ids\n=',self._ids
        if len(self)>0:
            return np.take(self._ids,np.flatnonzero(mask))
        else:
            return np.zeros(0,int)
    
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
    

    def add_rows(self, n=None, ids = [], **attrs):
        if n != None:
            ids = self.suggest_ids(n)
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
        
        #print 'add_rows ids', ids
        for colconfig in self._colconfigs:
            colconfig.add(ids, values = attrs.get(colconfig.attrname, None ))
        return ids
    
    def del_row(self, _id):
        print 'del_row',id
        self.del_rows([_id])
        
    def del_rows(self, ids):
        print 'del_rows',ids
        # TODO: this could be done in with array methods
        for colconfig in self._colconfigs:
            del colconfig[ids]
                
                
        for _id in ids:        
            i=self._inds[_id]
                
            #print '    del from id lookup'
            self._ids=np.concatenate((self._ids[:i],self._ids[i+1:]))
            
            #print '    free index',id
            if _id == len(self._inds)-1:
                # id is highest, let's shrink index array by 1
                self._inds=self._inds[:-1]
            else:
                self._inds[_id]=-1   
                    
            # get ids of all indexes which are above i
            ids_above=np.flatnonzero(self._inds>i)
                
            # decrease index from those wich are above the deleted one
            #put(self._inds, ids_above,take(self._inds,ids_above)-1)
            self._inds[ids_above]-=1
            
            #print '    self._inds',self._inds
            
        
                
                    
        #print '  del',ids,' done.'

        
        
        
       
                    

    
    

class ArrayObjman(Arrayman, TableMixin):
    """
    Array Object management manages objects with numeric arrays based columns.
    Can also handle list and dict based columns. 
    """
    def __init__(self,ident, **kwargs):
        self._init_objman( ident, **kwargs)
    
        
    def _init_objman(self, ident,**kwargs):
        BaseObjman._init_objman(self, ident, managertype = 'table', **kwargs)
        Arrayman.__init__(self)
        #self.set_valuesman(self)
        self.set_attrsman(self)
        
    
    
    