"""
This package contains the base classes of al relevant classes.

TODO: Following issues must be resolved in the following version

- child  - parent - ident - named
    childs of named parents should have the name in the ident...and local ident
    create 2 ident entries for arrayobj: attr+id
    use a list for ident elements instead of a string wityh string conversion
    on the fly or permanently.
    May be of advantaqge (= fast) to store instances that are on the path of the 
    ident elements
    
    in particular: distiguish between an instance on the tree that is created with parent
    and an already initialized instance that is set and usd as a link.
    idea in this case get all confuguration info from instance itself
    or better link to it!
- ident should be a sequence or list of strings
- separate classes for array type objects
- classes for metatypes 
- try to reduce type recognition during init ?? 
- MIXIN for named operation just as for plugin
- make plugin faster by overwriting get/set attribute
- print attribute must also go in classes


- make common initialization priciples in particular for classes based on mixins
  who sets the name: the parent! name is option with default
-  simplify configurationwith key setting...should appear only once 
   try to catch it from within the object with __name__
- try to remove stored type identification...careful problems are nd scalars

"""
from numpy import *
# from numpy.oldnumeric.user_array import numpy.oldnumeric.user_array as
# UserArray
from copy import copy, deepcopy

from sys import exit
import types
import string
import pickle
#import networkx as nx

try:
    flatnonzero
except:
    def flatnonzero(x):
        return nonzero(x.ravel())

CONFIG_TEMPLATE = {
    'info': '',
            'doc': '',
            'metatype': '',
            'type': '',
            'group': [],
            'save': True,
            'copy': False,
            'struct': 'scalar',
            'perm': 'w',
            'unit': '',
            'do_init_arrayobj': False,
}

ARRAYTYPES = [types.TupleType, ndarray, types.ListType, types.XRangeType]

TYPES = {
    'BooleanType': {'dtype': '?', 'perm': 'w', },
    'BufferType': {'dtype': 'O', 'perm': 'r', },
    'BuiltinFunctionType': {'dtype': 'O', 'perm': 'r', },
    'BuiltinMethodType': {'dtype': 'O', 'perm': 'r', },
    'ClassType': {'dtype': 'O', 'perm': 'r', },
    'CodeType': {'dtype': 'O', 'perm': 'r', },
    'ComplexType': {'dtype': 'D', 'perm': 'w', },
    'DictProxyType': {'dtype': 'O', 'perm': 'r', },
    'DictType': {'dtype': 'O', 'perm': 'r', },
    'DictionaryType': {'dtype': 'O', 'perm': 'r', },
    'EllipsisType': {'dtype': 'O', 'perm': 'r', },
    'FileType': {'dtype': 'O', 'perm': 'w', },
    'FloatType': {'dtype': 'd', 'perm': 'w', },
    'FrameType': {'dtype': 'O', 'perm': 'r', },
    'FunctionType': {'dtype': 'O', 'perm': 'r', },
    'GeneratorType': {'dtype': 'O', 'perm': 'r', },
    'InstanceType': {'dtype': 'O', 'perm': 'r', },
    'IntType': {'dtype': 'i', 'perm': 'w', },
    'LambdaType': {'dtype': 'O', 'perm': 'r', },
    'ListType': {'dtype': 'O', 'perm': 'r', },
    'LongType': {'dtype': 'g', 'perm': 'w', },
    'MethodType': {'dtype': 'O', 'perm': 'r', },
    'ModuleType': {'dtype': 'O', 'perm': 'r', },
    'NoneType': {'dtype': 'O', 'perm': 'r', },
    'NotImplementedType': {'dtype': 'O', 'perm': 'r', },
    'ObjectType': {'dtype': 'O', 'perm': 'r', },
    'SliceType': {'dtype': 'O', 'perm': 'r', },
    'StringType': {'dtype': 'S', 'perm': 'w', },
    'StringTypes': {'dtype': 'O', 'perm': 'w', },
    'TracebackType': {'dtype': 'O', 'perm': 'r', },
    'TupleType': {'dtype': 'O', 'perm': 'r', },
    'TypeType': {'dtype': 'O', 'perm': 'r', },
    'UnboundMethodType': {'dtype': 'O', 'perm': 'r', },
    'UnicodeType': {'dtype': 'U', 'perm': 'w', },
    'XRangeType': {'dtype': 'O', 'perm': 'r', },
    '__builtins__': {'dtype': 'O', 'perm': 'r', },
    '__doc__': {'dtype': 'O', 'perm': 'r', },
    '__file__': {'dtype': 'O', 'perm': 'r', },
    '__name__': {'dtype': 'O', 'perm': 'r', },
}

IDENTSEP = '.'


def make_arrays(**attrs):
    """
    Makes an array of same type from all attributes in attrs dictionary
    A bit slow! Avoid using this function!
    """
    for attr in attrs.keys():
        obj = attrs[attr]
        if type(attrs[attr]) == ndarray:
            attrs[attr] = attrs[attr].reshape((1,) + obj.shape)
        elif type(obj) in (types.FloatType, types.LongType):
            attrs[attr] = array([obj], float)
        elif type(obj) == types.IntType:
            attrs[attr] = array([obj], int)
        elif type(obj) == types.BooleanType:
            attrs[attr] = array([obj], bool)
        else:
            # another strange type
            dt = 'O'
            for typename in dir(types):
                if type(obj) == types.__dict__[typename]:
                    dt = get_dtype(typename)
                    break
            attrs[attr] = array([obj], dtype=dt)
    return attrs


def is_integer(n):
    """
    Returns True if n is an integer type of any kind
    """
    return type(n) in (types.IntType, int32, int64, int0, int16, int8, types.LongType)


def get_type(obj, typelist=[]):
    """
    Returns a flat list of type-string(s) which correspond to the
    (nested) type(s) of  the object obj.

    The returned type(s)  are keys of the global TYPES dictionary.
    """

    typelist = copy(typelist)
    if type(obj) == ndarray:
        typelist.append(('ArrayType', obj.shape))
        # return typelist # let's assume homogenious array types
        if len(obj.ravel()) > 0:
            return get_type(obj.flat[0], typelist)
        else:
            return typelist

    elif hasattr(obj, 'dtype'):  # obj is an array element
        # attention this is not a string but maybe this should
        # also be changed for all other types
        typelist.append(obj.dtype.str)
        return typelist

    elif type(obj) in (types.ListType, types.TupleType):
        typelist.append('ListType')
        if obj:
            return get_type(obj[0], typelist)
        else:
            return typelist

    elif type(obj) in (types.DictType, types.DictionaryType):

        if obj:
            key = obj.keys()[0]
            keytype = get_type(key)
            if len(keytype) > 0:
                typelist.append(('DictType', keytype[0]))
            else:
                typelist.append(('DictType', ''))

            return get_type(obj[key], typelist)
        else:
            typelist.append(('DictType', ''))
            return typelist

    else:
        for typename in dir(types):
            if type(obj) == types.__dict__[typename]:
                typelist.append(typename)
                return typelist
                break
        typelist.append('')
        return typelist


def get_dtype(type):
    """
    Returns dtype as used in Nummeric package.
    The argument is a string defining the python objecy type
    and is a key of the TYPES dictionary.
    If type is not found in the dictionary, dtype 'O' is
    returned, which means a generic object.
    """
    if TYPES.has_key(type):
        return TYPES[type]['dtype']
    else:
        return 'O'


def save_obj(obj, filename):
    """
    Saves python object to a file with filename.
    Filename may also include absolute or relative path.
    If operation fails a False is returned and True otherwise.
    """
    try:
        file = open(filename, 'wb')
    except:
        print 'WARNING in save: could not open', simname
        return False

    # try:
    pickle.dump(obj, file, protocol=2)
    file.close()
    return True
    # except:
    #    file.close()
    #    print 'WARNING in save: could not pickle object'
    #    return False


def load_obj(filename, parent=None):
    """
    Reads python object from a file with filename and returns object.
    Filename may also include absolute or relative path.
    If operation fails a None object is returned.
    """
    print 'load_obj', filename
    try:
        file = open(filename, 'rb')
    except:
        print 'WARNING in load_obj: could not open', filename
        return None

    # try:
    # print '  pickle.load...'
    obj = pickle.load(file)
    file.close()
    # print '  obj._link2'

    # _init2_ is to restore INTERNAL states from INTERNAL states
    obj._init2_config()

    # _init3_ is to restore INTERNAL states from EXTERNAL states
    # such as linking
    obj._init3_config()

    # _init4_ is to do misc stuff when everything is set
    obj._init4_config()

    return obj

    # except:
    #	print 'WARNING in load: could not load object'
    #	return None


def is_objconfigured(obj):
    return hasattr(obj, 'init_config')


class ArrayObj:

    """
    Mixin for Classes or Instance which are used as array-element 
    within the framework of the ArrayObjConfigurator.

    Its main function is init_arrayobj where an id gets assigned
    which corresponds to the id of the element in the parent array.

    Also __getitem__ and __setitem__ functions are implemented through which 
    array elements with the same id of other attributes of the parent object 
    can be read and changed.
    """

    def init_arrayobj(self, id, parent, ident, **args):
        """
        Initialize by defining parent instance where all the
        attributes of this instance are stored in array form.
        Also define the id of the array coresponding to this instance. 

        Function will be called automatically from ArrayObjConfigurator
        when id is assigned.

        """
        self.parent = parent
        # if ident=='':
        #    ident=parent.ident+IDENTSEP+str(id)
        # else:
        self.ident = ident
        self.id = id
        self.config_with_id()
        # print 'init_arrayobj',self.ident,id#,parent

    def config_with_id(self):
        """
        Here the instance can be inizialized..
        Function called AFTER  self.id and self.parent have been assigned.
        """
        pass

    def copy_attrs(self, targetobj, attrs=None):
        """
        Copies  given list of attributes of this object 
        to given target object.

        If no attributes are given then all attributes of this object 
        are considered for copying.

        Clearly, the array attributes corresponding to the id of this
        object are copied to the array attributes of the arrays 
        with the id of the target object.

        To copy an attribute, the foolowing conditions must be satisfied:
            - the attribute must be configured with key copy=True
            - the attribute must be configured in targetobject 

        """
        self.parent.copy_attrs(targetobj, self.id, targetobj.id, attrs=attrs)

    def __getitem__(self, key):
        return self.getparent(key)

    def __setitem__(self, key, value):
        self.setparent(key, value)

    def setparent(self, key, value):
        """
        Sets entry with id of this instance of the attribute 'key' of parent object.

        Usage:
            self['attributename']=value  


        """
        # print key,value
        self.parent[key, self.id] = value

    def getparent(self, key):
        """
        Gets entry with id of this instance of the attribute in parent object.

        Usage:
            self['attributename']  
        """
        # print 'ArrayObject.__getitem__',key
        return self.parent[key, self.id]

    def get_groupattrs_obj(self, name):
        """
        Returns a dictionary with all attributes of a group with given name. 
        Key is attribute name and value is attribute value for the given id. 
        """
        # print 'get_groupattrs_obj',name,self.id
        return self.parent.get_groupattrs(name, self.id)


class PlugoutMixin:

    """
    Mixin to provide methods to plug  callback functions into other objects.
    """

    def init_plugout(self):
        """
        Initialize basic plugout variables
        """
        # dictionary with pluginobj as key and list with plugids
        # provided by obj as value
        self._plugoutobjs = {}

    def add_plugout(self, obj, plugtype, function, attr='_all'):
        """
        Make a plugin to object and retain plugin id for later unplugg.

        Standard plug types are automatically set bu the system:
        plugtype    plugcode
        'del'       0
        'set'       1
        'get'       2
        'add'       3

        'del_items' 20
        'set_items' 21
        'get_items' 22
        'add_items' 23



        """
        if not self._plugoutobjs.has_key(obj):
            self._plugoutobjs[obj] = []
        # make plugin to object
        plugid = obj.add_plugin(obj, plugtype, function, attr)

        # add plugginid of obj to list
        self._plugoutobjs[obj].append(plugid)

        return plugid

    def del_plugout(self, obj, plugid=-1):
        """
        Plug out from specified object.
        If plugid is specified, only this plug is pulled,
        otherwise all plugs to this object are pulled at once.
        """
        if self._plugoutobjs.has_key[obj]:
            if plugid < 0:
                if self._plugoutobjs[obj].count(plugid) > 0:
                    obj.del_plugin(plugid)
                    self._plugoutobjs[obj].remove(plugid)
            else:
                for plugids in self._plugoutobjs[obj]:
                    obj.del_plugin(plugid)
                del self._plugoutobjs[obj]
        else:
            print 'WARNING: no plugout ids for object', obj
            plugids = []

    def unplugout(self):
        """
        Unplug all plugs on other objects
        """
        for obj in self._plugoutobjs.keys():
            self.del_plugout(obj)


class PluginMixin:

    """
    Mixin to provide methods to plug in callback functions
    from other objects.
    """

    def init_plugin(self):
        """
        Initialize basic plugin variables
        """
        # self._plugins={} # compulsory in configurator!
        self._counter_plugid = 0

        self._plugcode = {'test': -1}
        # print 'init_plugin',self.ident,self._plugcode

        # dictionary with obj as key and dict with plugid,plugtype,attr as
        # value
        self._pluginobjs = {}

        # add basic plugincodes
        # for scalars
        self.add_plugcodes([('del', 0), ('set', 1), ('get', 2), ('add', 3)])

        if hasattr(self, 'get_ids'):
            # print '  add_plugcodes arrays for ',self.ident
            self.add_plugcodes([('del_items', 20),
                                ('set_items', 21),
                                ('get_items', 22),
                                ('add_items', 23), ])
        self.enable_plugins()

    def execute_plugin(self, plugtype):
        """
        Execute all callbacks for the plugin of the given type
        """
        self._execute_plugins(self._plugcode[plugtype], attr='_all')

    def add_plugcodes(self, plugcodes):
        """
        Plugcodes integers that correspond to a plugtype.
        These integers are used as keys for faster access.

        Argument plugcodes is a list with tuples 
        each tuple containing plugtype and plugcode.

        Standard plug types are automatically set bu the system:
        plugtype    plugcode
        'del'       0
        'set'       1
        'get'       2
        'add'       3

        'del_items' 20
        'set_items' 21
        'get_items' 22
        'add_items' 23

        """
        # print 'add_plugcodes',plugcodes
        for plug, plugcode in plugcodes:
            if not self._plugcode.has_key(plug):
                self._plugcode[plug] = plugcode
                # self._plugins[plugcode]={'_all':{}}
                # print '  added
                # self._plugcode[plug]',plug,self._plugcode[plug]
            else:
                print 'WARNING:_add_plugcodes: already existant code',\
                    plug, self._plugcode[plug]
                print '  Refused to insert.'

    def add_plugin(self, obj, plug, callbackfunction, attr='_all'):
        # print 'add_plugin',plug,callbackfunction,attr
        self._counter_plugid += 1
        plugid = self._counter_plugid
        plugcode = self._plugcode[plug]

        if not self._plugins.has_key(plugcode):
            self._plugins[plugcode] = {}

        if not self._plugins[plugcode].has_key(attr):
            self._plugins[plugcode][attr] = {plugid: callbackfunction}
        else:
            self._plugins[plugcode][attr][plugid] = callbackfunction

        if not self._pluginobjs.has_key(obj):
            self._pluginobjs[obj] = {}

        self._pluginobjs[obj][plugid] = (plugcode, attr)
        return plugid

    def disable_plugins(self):
        """
        Prevents plugins from execution
        """
        self._enable_plugins = False

    def enable_plugins(self):
        """
        Enable plugins for execution
        """
        self._enable_plugins = True

    def del_plugin(self, obj, plugid=-1):
        """
        Remove plugin with pluginid
        """

        if plugid >= 0:
            plugids = [plugid]
        else:
            plugids = self._pluginobjs[obj].keys()

        if self._pluginobjs.has_key(obj):
            # there exist ids associated with this object
            for pid in plugids:
                if self._pluginobjs[obj].has_key(pid):
                    # remove plugin with pid
                    plugcode, attr = self._pluginobjs[obj][pid]
                    del self._plugins[plugcode][attr][pid]
                    del self._pluginobjs[obj][pid]

                    if len(self._plugins[plugcode][attr]) == 0:
                        # no more plugins for this attribute...remove it
                        del self._plugins[plugcode][attr]
                        if len(self._plugins[plugcode]) == 0:
                            # np more plugins for this plugintype...remove it
                            del self._plugins[plugcode]

            if len(self._pluginobjs[obj]) == 0:
                # no more plugin ids associated with this object
                del self._pluginobjs[obj]


class ObjConfigurator:

    """
    Base mixin class for general scalar object configuration.

    Usage:

        During initialization of class call

        self.init_config('myclass_ident_string',name='my human readable class')

        or if there is a parent object availlable:

        self.init_config('myclass_ident_string',parent)

        In this case the ident of the parent obj is prependet



    """

    def init_config(self, ident='', parent=None, name='noname', **args):
        # print '---init_config ident',ident,hasattr(self,'init_plugin')
        # print '  parent',parent
        # print '  name',name,len(name)

        if hasattr(self, 'ident'):
            ident = self.ident

        if (name == 'noname') & (len(ident) > 0):
            self.name = ident
        elif (name == 'noname') & (len(ident) == 0):
            if self.__dict__.has_key('__name__'):
                ident = self.__name__  # use class name as identifier
            self.name = ident

        elif (len(name) > 0) & (len(ident) == 0):
            if self.__dict__.has_key('__name__'):
                ident = self.__name__  # use class name as identifier
            self.name = name
        else:
            self.name = name

        self.ident = ident
        self.parent = parent
        if parent != None:

            # print 'init_config',ident,parent.ident
            # present object is a child of a parent
            # => prepend parent identifier string
            if hasattr(parent, 'ident'):
                self.ident = parent.ident + IDENTSEP + ident

        self._config = {}
        self._groups = {}
        self._functions = []

        # define a list with attribute strings that need to be saved
        # apart from the cofigured attributes
        # this list may be extended by successive initializations
        self._attrs_save = [  # 'parent',
            'name', 'ident', '_config', '_groups',
            '_functions',
        ]

        # conditional plugin import
        self._init_plugging()

        # print 'Created configuration for',self.ident

        return self.ident

    def reset_parent(self, parent):
        """
        Resets previously assigned parent.
        This method is not used during initialization.  
        """
        ident = self.get_ident(is_local=True)
        self.parent = parent
        if parent != None:
            # print 'init_config',ident,parent.ident
            # present object is a child of a parent
            # => prepend parent identifier string
            if hasattr(parent, 'ident'):
                self.ident = parent.ident + IDENTSEP + ident

    def get_ident(self, is_local=False):
        """
        Returns identity string.
        By default it returns the entire identity string
        unless is_local==True. In this case only the local, the last
        element of the global identity is returned.
        """
        if is_local:
            return string.split(self.ident, IDENTSEP)[-1]
        else:
            return self.ident

    def _link(self, parent=None):
        """
        Called after load process to link with other instances 
        in the environment.
        """

        # self.parent=parent
        # for attr in self._config.keys():
        #    obj=getattr(self,attr)
        #    if is_objconfigured():
        #        obj._link(self)
        pass

    def _init2_config(self):
        """
        Called after load and link process to do make initializations
        of constants, dependent INTERNAL attributes such as hash tables, 
        NO LINKS OR DEPENDENCIES TO EXTERNAL INSTANCES ALLOWED

        Here only the configurator internal attributes are restored.
        All object specific in _init2() in 
        """
        print '\n _init2_config', self.ident

        print '  restore unsaved attributes', self.ident
        self._functions = []
        for attr in self._config.keys():

            # check
            # if hasattr(self,attr):
            #    value=getattr(self,attr)
                # print '  attr',attr,
                # if type(value)==ndarray:
                #    print value.dtype,value.shape
                # else:
                #    print value

            config = self._config[attr]
            if config['is_function']:
                # print '    resore function',attr
                self._functions.append(attr)
            elif not config['save']:
                # print '    resore attr',attr
                self.reset(attr)
            # check
            # value=getattr(self,attr)
            # print '  after: attr',attr,
            # if type(value)==ndarray:
            #    print value.dtype,value.shape
            # else:
            #    print value

        # conditional plugin import
        self._init_plugging()

        print '  call _init2', self.ident

        self._init2()

        print '  restore children', self.ident
        for attr in self._config.keys():
            config = self._config[attr]
            if (config['save']) & (not config['is_function']):
                if config['struct'] == 'scalar':
                    obj = getattr(self, attr)
                    if is_objconfigured(obj):
                        print '    call _init2_config of', obj.ident
                        obj._init2_config()
                else:
                    # attribute is an array so init here for arrays as well
                    # TODO: should go as special in arrayconfigurator
                    for id in self.get_ids():
                        obj = self[attr, id]
                        if is_objconfigured(obj):
                            print '    call _init2_config of', obj.ident
                            obj._init2_config()

    def _init2(self):
        """
        Called after load  process to do make initializations
        of constants, dependent INTERNAL attributes such as hash tables, 
        NO LINKS OR DEPENDENCIES TO EXTERNAL INSTANCES ALLOWED

        To be overridden.
        """
        pass

    def _init3_config(self):
        """
        Called after load and link process to do make initializations
        of constants, dependent attributes such as hash tables, 
        links to external instances etc.

        """
        print '\n_init3_config', self.ident
        # link with other, now already initialized objects
        # Those attributes which have NOT been saved, but have
        # an ident in they configuration can no be recovered
        # though their ident
        for attr in self._config.keys():
            config = self._config[attr]

            if config.has_key('link'):
                # try to recover object with ident

                if config['link']:

                    if config['struct'] in ('array', 'dict', 'marray'):
                        print '   detected link array ', attr
                        # here we want to recover links to objects
                        # using a dictionary with
                        # id as key and ident as value
                        for id in self.get_ids():
                            self[attr, id] = self.get_obj_instance(
                                config['ident'][id])

                    else:
                        print '   detected link ', attr
                        setattr(self, attr, self.get_obj_instance(
                            config['ident']))

        # make specific settings
        self._init3()

        # print '  restore children',self.ident
        for attr in self._config.keys():
            config = self._config[attr]
            if (config['save']) & (not config['is_function']):
                if config['struct'] == 'scalar':
                    obj = getattr(self, attr)
                    if is_objconfigured(obj):
                        # print '    call _init3_config of',obj.ident
                        obj._init3_config()
                else:
                    # attribute is an array so init here for arrays as well
                    # TODO: should go as special in arrayconfigurator
                    for id in self.get_ids():
                        obj = self[attr, id]
                        if is_objconfigured(obj):
                            # print '    call _init3_config of',obj.ident
                            obj._init3_config()

    def _init3(self):
        """
        Called after load and internal link process to do make initializations
        of constants, dependent attributes such as hash tables, 
        links to external instances etc.

        To be overridden.
        """
        pass

    def _init4_config(self):
        """
        Called after load and link process to do make initializations
        any other initializations.

        """
        print '\n_init4_config', self.ident

        # make specific settings
        self._init4()

        # print '  restore children',self.ident
        for attr in self._config.keys():
            config = self._config[attr]
            if (config['save']) & (not config['is_function']):
                if config['struct'] == 'scalar':
                    obj = getattr(self, attr)
                    if is_objconfigured(obj):
                        # print '    call _init4_config of',obj.ident
                        obj._init4_config()
                else:
                    # attribute is an array so init here for arrays as well
                    # TODO: should go as special in arrayconfigurator
                    for id in self.get_ids():
                        obj = self[attr, id]
                        if is_objconfigured(obj):
                            # print '    call _init4_config of',obj.ident
                            obj._init4_config()

    def _init4(self):
        """
        Called after load and link process to do make initializations
        any other initializations.

        To be overridden.
        """
        pass

    def _init_plugging(self):
        """
        Initializes plugin and plugout related variables.
        This function is called during initialization 
        and after load, before reinitializeing other attrubutes. 
        """
        # print '_init_plugging',self.ident,self
        # if make_plugins:
        #    from PluginMixin import *
        if hasattr(self, 'init_plugin'):
            # print '  ini_plugin',self.ident,self
            self.init_plugin()

        if hasattr(self, 'init_plugout'):
            # print '  ini_plugout',self.ident
            self.init_plugout()
        # this plugin dictionary is always required and remains
        # empty if no plugins are made
        self._plugins = {}

    def _execute_plugins(self, plugcode, attr='_all', **args):
        """
        This method executes all callbackfunctions associated with the
        given plugcode.
        This funcion is a core part because used in various
        methods. All other methods regarding plugins in PluginMixin.
        """
        # it has been checked that self._plugins has all keys
        print '_execute_plugins', plugcode, ' for', self.ident, attr
        # print '  self._plugins=',self._plugins
        # print '  self._plugins[plugcode]=',self._plugins[plugcode]

        if self._enable_plugins:
            args['attr'] = attr
            if self._plugins[plugcode].has_key('_all'):
                for func in self._plugins[plugcode]['_all'].values():
                    # print '  => execute function',func
                    # print '     with args',args
                    func(**args)
            elif self._plugins[plugcode].has_key(attr):
                # selective plugin for specific attribute
                for func in self._plugins[plugcode][attr].values():
                    func(**args)
            # else:
            #    print '  no plugin functions for plugcode',plugcode

    def get_name():
        """
        Returns formatted name string of object.
        """
        return self.name

    def print_config(self):

        # print 'inds=',self._inds
        print '===================================================================='
        print '   Config of', self.name, ' with ident:', self.ident
        print '===================================================================='
        for attr in self._config.keys():
            self.print_attr(attr)

        # for name in self.get_groupnames():
        #    if self.get_group(name):
        #        print 'Group: ',name
        #        for attr in self.get_group(name):
        #            self.print_attr(attr)

        # print all attributes which are not in a group
        # print '--'
        # for attr in self._config.keys():
        #    if len(self._config[attr]['group'])==0:
        #        self.print_attr(attr)

    def print_attr(self, attr):
        # config=self.get_config(attr)
        # print '   ', attr,'=',self.__dict__[attr],config['unit']
        print '   ' + attr + ' = ' + self.get_string_attr_value(attr, self[attr])

    def print_attrattrs(self, attrs=None, attrattrs=['info', 'group', 'struct'], filename=None, sep='\t'):
        """
        Prints attributes of attributes.
        If 
        """
        # print 'print_attrattrs'
        lines = []
        if attrs == None:
            attrs = self._config.keys()
        attrs.sort()
        line = u'attr'
        for attrattr in attrattrs:
            line += sep + attrattr
        lines.append(line)

        for attr in attrs:
            # print '  attr',attr
            line = u'' + attr
            for attrsattr in attrattrs:
                if self._config[attr].has_key(attrsattr):
                    line += sep + str(self._config[attr][attrsattr])
                else:
                    line += sep + ''
            lines.append(line)

        if filename != None:
            file = open(filename, 'w')
            for line in lines:
                # print line
                file.write(line + '\n')
            file.close()
        else:
            for line in lines:
                print line

        return lines

    def get_string_attr_value(self, attr, value):
        """
        Returns a string with correctly formatted value of attribute.
        """
        # print '\n\nget_string_attr_value',attr,value,type(value)
        # print self.get_printformat(attr)+self.get_unit(attr)
        #print (self.get_printformat(attr)+self.get_unit(attr))%(value)
        config = self.get_config(attr)
        tt = config['type'][0]
        unit = self.get_unit(attr, show_parentesis=True)
        if tt in ('BooleanType',):
            if value:
                s = 'True'
            else:
                s = 'False'
            return s + self.get_unit(attr, show_parentesis=True)

        elif type(value) in ARRAYTYPES:
            # value is an array=> print signe items
            s = '['
            for v in value:
                s += self.get_string_attr_value(attr, v) + ','
            s += ']'
            return s

        else:
            try:
                return (self.get_printformat(attr)
                        + ' ' + self.get_unit(attr, show_parentesis=True)) % value
            except:
                print '\n WARNING could not print', attr, '=', value
                return 'Error'

    def get_printformat(self, attr):
        """
        Returnes the standad fortran stringformat for attribute.
        """
        config = self.get_config(attr)
        tt = config['type'][0]
        if tt in ('IntType', 'LongType'):
            return '%' + str(config['digits_integer']) + 'd'

        elif tt in ('FloatType', 'ComplexType'):
            return '%' + str(config['digits_integer']) + '.'\
                + str(config['digits_fraction']) + 'f'

        else:
            # its a string
            return '%s'

        return printformat

    def get_numformat(self, attr, confog=None):
        """
        Returns a dictionary with all formatting infos on attribute.
        It is tried to detect the numerical datatype and define
        formatting info, if not already defined.
        """
        # determine format of numeric value

        config = self.get_config(attr)

        # take top level type
        tt = config['type'][0]
        numformat = {}
        if tt in ('IntType', 'LongType'):
            numformat['digits_integer'] = config.get('digits_integer', 13)
            numformat['digits_fraction'] = config.get('digits_fraction', 0)
        else:
            numformat['digits_integer'] = config.get('digits_integer', 9)
            numformat['digits_fraction'] = config.get('digits_fraction', 8)

        numformat['min'] = config.get('min', None)
        numformat['max'] = config.get('max', None)

        # print 'get_numformat',attr,numformat
        return numformat

    def config(self, attr, default=None, **args):
        """
        Configure attribute for Object.
        """
        # print '\n\n\n'+70*'*'
        # print 'config',attr,default,args

        # configure attribute with template and parameters from args
        self._config[attr] = deepcopy(
            CONFIG_TEMPLATE)  # CONFIG_TEMPLATE.copy()

        # print '  after template configured attr:',attr,self._config[attr]

        # try to guess type...as default only
        t = get_type(default)
        self._config[attr]['type'] = t
        # print '  get_type(default)=',get_type(default)

        # guess read/write permissions according to toplevel type
        if TYPES.has_key(t[0]):
            self._config[attr]['perm'] = TYPES[t[0]]['perm']

        # default name
        self._config[attr]['name'] = attr

        if args.has_key('metatype'):
            # TODO: fetch here metatype info from metatype module
            pass
        else:
            self._config[attr]['metatype'] = ''  # default

        # overwrite all defaults with arguments
        self._config[attr].update(args)

        # print '  after update configured attr:',attr,self._config[attr]

        if type(self._config[attr]['type']) != types.ListType:
            self._config[attr]['type'] = [self._config[attr]['type']]

        # complete with numerical format info
        self._config[attr].update(self.get_numformat(attr))

        # store default value here,
        # in case of an array this is the value of a single entry
        self._config[attr]['default'] = default

        # check if attribute is an existing function and if yes register
        # as such in config
        self._config[attr]['is_function'] = False
        if hasattr(self, attr):
            if type(getattr(self, attr)) == types.MethodType:
                # print '  ',attr,'is a function'
                self._config[attr]['is_function'] = True
                # self._config[attr]['perm']='r' # read only

        if self._config[attr]['is_function']:
            # register attribute in function list
            self._functions.append(attr)
        else:
            self.reset(attr)

        # print '  configured attr:',attr,self._config[attr]

        # this turnes group argument in a list when given a signle string
        # TODO: would be better to introduce groups as a list with all group
        # names
        groupnames = self._config[attr]['group']
        if type(groupnames) == types.StringType:
            self._config[attr]['group'] = [groupnames, ]

        if (self._config[attr]['metatype'] == 'id'):
            # explictly declared as id
            # make sure of array structure
            self._config[attr]['struct'] = 'array'
            # TODO: better identify object as arrayobjconfiguaror

        # put attribute into id_ref or obj ref group
        # if no reference is made then put in id group
        if self._config[attr].has_key('ident_ref'):
            if self._config[attr]['struct'] == 'array':

                if 'id_ref' not in self._config[attr]['group']:
                    # print '  ident_ref detected id_ref for',attr
                    self._config[attr]['group'].append('id_ref')
                    self._config[attr]['metatype'] = 'id'
                    # print '  check template',CONFIG_TEMPLATE.copy()
            else:
                # print '  ident_ref detected obj_ref for',attr
                if 'obj_ref' not in self._config[attr]['group']:
                    self._config[attr]['group'].append('obj_ref')
                    self._config[attr]['metatype'] = 'obj'

        elif (self._config[attr]['metatype'] == 'id'):
            # and add it to id group
            if 'id' not in self._config[attr]['group']:
                self._config[attr]['group'].append('id')

        for groupname in self._config[attr]['group']:
            self.config_group(groupname, attr)

        # this is a treatment fo linked objects, which are
        # not a parent or a child of this objects
        # they ill not be saved, but recovered bu their ident during _init3
        if self._config[attr].has_key('link'):
            if self._config[attr]['link']:
                # try to recover object with ident
                # however, ident itself will only be stored during saving
                self._config[attr]['save'] = False
                self._config[attr]['perm'] = 'r'

    def reset(self, attr):
        """
        Reset atttribute attr to default values
        """
        # create attribute and if there are already arrays with entries,
        # create the same number of entries with default value
        # default=self._config[attr]['default']
        # struct=self._config[attr]['struct']

        # if self._config[attr]['is_function']:
        #    # register attribute in function list
        #    #self._functions.append(attr)
        #    # already done in config
        #    pass
        # else:
        # attr is a scalar , simply assign default
        if not self._config[attr]['is_function']:
            self.__dict__[attr] = self._get_default(attr)

    def set_default(self, attr, value):
        """
        Sets a default value for this attribute.
        """
        self._config[attr]['default'] = value

    def _get_default(self, attr):
        """
        Returns default value for attribute with name attr_name

        Usage:
            Return a scalar default 
            self._get_default('attr_name')    


        """
        default = self._config[attr]['default']
        # struct=self._config[attr]['struct']

        if type(default) == ndarray:
            obj = default  # .tolist()

        elif (type(default) == types.ClassType):
            if self._config[attr]['init']:
                # Default is a class, needs to be initialized
                # TODO: initialization and global id needs to be reviewed
                obj = default(self)

        else:
            # otherwise just copy default value
            obj = default

        # print '_get_default',obj
        return obj

    def set(self, attr, val=None):
        """
        Sets value to attribute.

        Usage:

        Set scalar variable var with value val
        self.set('var',val) 


        """

        if self._config['attr']['struct'] == 'scalar':  # exclude functions
            self.__dict__[attr] = val

        if self._plugins.has_key(1):
            self._execute_plugins(1, key)

    def is_config(self, attr):
        """
        Returns true if attribute with name attr is configured.
        """
        return self._config.has_key(attr)

    def copy_attrs(self, targetobj):
        """
        Copies  given list of scalar attributes of this object 
        to given target object.

        If no attributes are given then all attributes of this object 
        are considered for copying.

        copy an attribute, the foolowing conditions must be satisfied:
            - the attribute must be configured with key copy=True
            - the attribute must be configured in targetobject 
            - the structure of the attribute must be scalar 
                (arrays done by copy method of ArrayObjConfigurator)
        """
        if attrs == None:
            # get all attributes
            attrs = self._config.keys()

        for attr in attrs:
            config = self._config[attr]
            if config['struct'] == 'scalar':
                if config['copy']:
                    if targetobj.is_config(attr):
                        targetobj[attr] = self[attr]

    def get_attrs(self, crits_atleastone=[], crits_all=[]):
        """
        Returns list of configured attribute names.

        crits_atleastone and crits_all are lists defining one or several
        criteria that the attribute configuration must satify.
        for example: [('struct','array'),('struct','dict'),('struct','marray')]
        """
        attrs = []
        if len(crits_atleastone) > 0:
            # get attribute names for attributes where at least one criteria
            # is satisfied
            for attr in self._config.keys():
                for criteria in crits_atleastone:
                    # print 'get_attrs:',attr,criteria
                    if self._config[attr][criteria[0]] == criteria[1]:
                        attrs.append(attr)
                        break
            return attrs

        elif len(crits_all) > 0:
            # get attribute names for attributes where all criteria
            # are satisfied
            for attr in self._config.keys():
                satisfied = True
                for criteria in crits_all:
                    # print 'get_attrs:',attr,criteria
                    satisfied &= (self._config[attr][
                                  criteria[0]] == criteria[1])
                if satisfied:
                    attrs.append(attr)

            return attrs

        else:
            # no criteria spacified, return all attribute names
            return self._config.keys()

    def get_config(self, attr=''):
        # print '_get_config:',attr
        if attr:
            return self._config.get(attr, {})
        else:
            return self._config

    def get_root_instance(self):
        """
        Returns instance of root obj of the tree.
        """
        obj = self
        while obj.parent != None:
            obj = obj.parent

        return obj

    def get_obj_instance(self, ident):
        """
        Returns the instance of the object of the given identity string ident.

        The researched object must reside within the object tree where also
        the self object is a part.
        """
        if ident == '':
            return None

        elif ident[0] == '.':
            # ident starts with a . means that it is a relative link
            # starting with this object
            obj = self
            idents = string.split(self.get_ident(
                is_local=True) + ident, IDENTSEP)

        else:
            obj = self.get_root_instance()
            idents = string.split(ident, IDENTSEP)

        print 'get_obj_instance ', idents
        print '  root_instance=', obj
        if len(idents) == 1:
            # just one element identification, means this is root
            return obj

        elif idents[1] == '':
            return obj

        else:
            oldobj = obj
            oldident = ident
            for ident in idents[1:]:

                print '  get attr', ident, 'from obj', obj
                if ident.isdigit():  # it's an id so get arrayobject

                    print '    get it through object[attr,id]', oldobj, oldident, string.atoi(ident)
                    obj = oldobj[oldident, string.atoi(ident)]
                    oldobj = obj
                    oldident = ident

                elif ident == '':
                    oldobj = obj
                    # obj=obj # do not dive

                else:
                    oldobj = obj
                    oldident = ident
                    obj = getattr(obj, ident)

        return obj

    def create_group(self, name, attr_list=[]):
        # print 'create_group',name,attr_list
        if type(attr_list) == types.ListType:
            # print '  groups before=',self._groups.keys()
            self._groups[name] = attr_list
            # print '  groups after =',self._groups.keys()
        else:
            print 'WARNING: attr_list must be list type'

    def config_group(self, name, attr, **args):
        """
        Add or append attribute to given name.
        """
        # print 'config_group',self._groups,type(self._groups)
        # print '  name,attr',name,attr

        if self._groups.has_key(name):
            if attr not in self._groups[name]:
                # print '  before append',self._groups[name]
                self._groups[name].append(attr)
                # print '  after append',self._groups[name]

        else:
            self.create_group(name, [attr])

    def get_groupnames(self): return self._groups.keys()

    def get_group(self, name):
        """
        Returns a list with attribute names that belong to that group name.
        """
        # print 'get_group self._groups=\n',self._groups
        return self._groups.get(name, [])

    def get_groupattrs(self, name):
        """
        Returns a dictionary with all attributes of a group. 
        Key is attribute name and value is attribute value. 
        """
        attrs = {}
        for attr in self._groups[name]:
            attrs[attr] = self['attr']
        return attrs

    def get_perm(self, attr):
        """
        Returns current permission of attribute
        """
        return self._config[attr]['perm']

    def get_unit(self, attr, show_parentesis=True):
        """
        Returns unit of attribute
        """

        if self._config[attr].has_key('unit'):
            unit = self._config[attr]['unit']
        else:
            unit = ''

        if show_parentesis:
            if len(unit) > 0:
                unit = '[' + unit + ']'

        return unit

    def __getitem__(self, attr):
        # print '__getitem__',key

        # plugin execution
        if self._plugins.has_key(0):
            self._execute_plugins(0, attr)
        return self.__dict__[attr]

    def __setitem__(self, attr, value):
        """
        Sets a value to an attribute


        """

        # print 'setitem',key,'to',value

        self.__dict__[attr] = value

        # plugin execution
        if self._plugins.has_key(1):
            self._execute_plugins(1, attr)

    def __getstate__(self):
        # print '__getstate__',self.ident
        # print '  self.__dict__=\n',self.__dict__.keys()

        state = {}
        if hasattr(self, '_config'):

            # store ident for linked objects prior to saving
            # will be recovered in _init3
            for attr in self._config.keys():
                config = self._config[attr]

                if config.has_key('link'):
                    # try to recover object with ident
                    if config['link']:
                        if config['struct'] in ('array', 'dict', 'marray'):
                            # here we want to retain the idents of all objects
                            # in this array, using a dictionary with
                            # id as key and ident as value
                            config['ident'] = {}
                            for id in self.get_ids():
                                config['ident'][id] = self[
                                    attr, id].get_ident()
                        else:
                            config['ident'] = getattr(self, attr).get_ident()

            # print '  self._config.keys()',self._config.keys()
            for attr in self._attrs_save:
                state[attr] = getattr(self, attr)  # self.__dict__[attr]
                # print '  saved state:',attr,state[attr]

            # decide whether to save parent:
            # the root object does not save parent because outside the tree
            if len(string.split(self.ident, IDENTSEP)) > 1:
                # print '  save parent for',string.split(self.ident,IDENTSEP)
                state['parent'] = self.parent
            else:
                state['parent'] = None

            if hasattr(self, 'init_arrayobj'):
                # it is actually an array-object, save also id
                if hasattr(self, 'id'):
                    # print '  saved attr id',self.id
                    state['id'] = self.id

            # save which attributes to save
            state['_attrs_save'] = self._attrs_save

            # now save attributes which have been configured to save
            for attr in self._config.keys():
                # print '  ',self._config[attr]['save'],attr
                if self._config[attr]['save']\
                        & (not self._config[attr]['is_function'])\
                        & (not (type(getattr(self, attr)) == types.MethodType)):
                        # print '  save attr:',attr
                    state[attr] = getattr(self, attr)

            self._getstate(state)

        return state

    def _getstate(self, state):
        """
        Complete state dict with state variables which are not 
        contained in the config scheme. All attributes which are
        configured and the save option is not explicitely set
        to False are automatically inserted in the state dictionay.
        """
        pass

    def __setstate__(self, state):
        # print '__setstate__',self

        # this is always required, but will not be saved
        self._plugins = {}

        for key in state.keys():
            # print '  set state',key
            self.__dict__[key] = state[key]

        # done in init2_config...
        # set default values for all states tha have not been saved
        # for attr in self._config.keys():
        #    if (not self._config[attr]['save']) & (not hasattr(self,attr)):
        #        print '  config attr',attr
        #        self.config(attr,**self._config[attr])

        # set other states
        self._setstate(state)

    def _setstate(self, state):
        """
        Complete state settings of state variables which are not 
        contained in the config scheme. All attributes which are
        configured and the save option is not explicitely set
        to False are automatically set.
        """
        pass

    def close(self):
        """
        Should be called before desroying the object.
        Does remove pugs is existent.
        """
        if hasattr(self, unplugout):
            self.unplugout()


class ArrayObjConfigurator(ObjConfigurator):

    """
    Base class for general scalar object configuration.

    For usage see demo classes below.
    """

    def init_config(self, ident='', parent=None, name='noname', named=False,
                    **args):
        """
        Fore named objects use:
            self.add_id_named('name_of_id')
        """
        # print 'init_config array',ident,hasattr(self,'init_plugin')
        ObjConfigurator.init_config(self, ident, parent, name, **args)

        # else:
        # print 'no array plugins for ',self.ident

        # print 'check config'#,self.ident,self._config
        self._arrayconfig = {}
        self._dictconfig = {}
        self._inds = zeros((0,), int)
        self._ids = zeros((0,), int)
        self._marrayconfig = {}
        self._named = named
        if self._named:
            self._id_names = {}
            self.config('_name_ids', '', struct='dict',
                        info='Dictionary with id as key and name as value')
        else:
            self._id_names = None

        # save these attributes, apart from configured ones
        self._attrs_save += ['_arrayconfig', '_dictconfig', '_inds', '_ids',
                             '_marrayconfig', '_named', '_id_names']

    def is_named(self):
        """
        Returns true if array has named rows
        """
        return self._named

    def get_id_from_name(self, name):
        """
        Returns id that corresponds to name.
        """
        return self._id_names.get(name, -1)

    def get_name_from_id(self, id):
        """
        Returns name that corresponds to id.
        """
        return self._name_ids.get(id, '')

    def get_names(self):
        """
        Returns list of all named ids.
        """
        # print 'get_names',self.ident,self._id_names.keys()
        return self._id_names.keys()

    def get_named(self, attr, name):
        """
        Returns value corresponding to named id
        """
        return self[attr, self.get_id_from_name(name)]

    def get_ids(self, inds=[], ordered=False):
        """
        Return all ids corrisponding to array indexes inds.
        Options:
            if no inds are given, all ids are returnd

            if ordered is true, ids will be sorted before they are returned.

        """
        if (len(inds) > 0) & (not ordered):
            return take(self._ids, inds)

        elif len(inds) > 0:
            ids = take(self._ids, inds)

        else:
            ids = self._ids.copy()
        if ordered:
            return sort(ids)
        else:
            return ids

    def get_id(self, ind):
        """
        Returns scalar id corresponding to index ind
        """
        # print 'get_id',ind,self._ids
        return self._ids[ind]

    def is_scalarid(self, id):
        """
        Returns True if id is a zero positive scalar interger
        """
        if is_integer(id):
            if id >= 0:
                return True
            else:
                return False
        else:
            return False

    def is_idarray(self, ids):
        """
        Fast, formal chack whether ids is either a valid array with ids.
        """
        if type(ids) in (types.ListType, ndarray):
            # it is an array
            if len(ids) > 0:
                # it contains at least one id
                # return True if this id is a zaro positive integer
                return self.is_scalar_id(ids[0])
            else:
                return False
        else:
            return False

    def is_ids(self, ids):
        """
        Formal check whether ids contains either an array with ids
        or is a scalar zero positive interger.
        """
        return self.is_idarray(ids) | self.is_scalarid(ids)

    def select_ids(self, mask):
        """
        Returns an array of ids, corresponding to the True
        of the mask array.

        Usage:
            Select all ids for which array of attribute x is greater than zero.
            ids=obj.select_ids( obj.x>0 )
        """
        # print 'select_ids',mask,flatnonzero(mask)
        # print '  self._ids=',self._ids

        return take(self._ids, flatnonzero(mask))

    # def select(self,mask,ordered=False):
    #    """
    #    Returns an array of ids, corresponding to the True
    #    of the mask array.
    #
    #    Usage:
    #        Get all elements of attribute y for which array of attribute x is greater than zero.
    #        values=obj.select(obj.y, obj.x>0 )
    #    """
    #
    #
    #    return self.get_ids(flatnonzero(mask),ordered=ordered)

    def get_ids_marray(self, attr, inds_marray=[]):
        """
        Return all ids and relative indexes corresponding to m-array 
        indexes in list inds_marray.

        The relative index is the index within the array corresponding
        to an id.

        Examples:
            obj.get_ids_marray('attrname',[2,3,4])

            returns an array of the following shape:
            [[11,0],
             [11,1],
             [12,0],]

            here marray index 2 and 3 correspond to id=11 with relative index
            0 and 1 respectively, while marray index 3 corresponds to 
            element with id=12 and relative index 0 
            (means first item in element with id=12)

        """
        # print 'get_ids_marray',inds_marray
        if len(inds_marray) > 0:
            # case more complex because need to check with block pointers

            # get block pointers for this attribute
            bp = self._marrayconfig[attr]['bp']
            inds = zeros((len(inds_marray)), int)

            i = 0
            for ind_marray in inds_marray:
                # print '  bp, ind_marray',bp,ind_marray
                inds[i] = flatnonzero(
                    (bp[:, 0] <= ind_marray) & (bp[:, 1] > ind_marray))[0]
                i += 1
            inds_delta = inds_marray - take(bp[:, 0], inds)

            ids = take(self._ids, inds)
        else:
            ids = []
            inds_delta = []

        return concatenate((transpose([ids]), transpose([inds_delta])), (1))
        # return reshape(  concatenate((ids, inds_delta)) ,(-1,2) )

    def get_inds(self, ids=None):
        if ids != None:
            return self._inds[ids]
        else:
            return self._inds[self._ids]

    def get_ind(self, id):
        return self._inds[id]

    def get_inds_marray(self, attr, ids):
        """
        Returns a flat array with all array indexes of 
        marray with attribute attr that have an id contained in list ids. 
        """
        # get block pointers for this attribute
        bp = self._marrayconfig[attr]['bp']
        inds = []
        # print 'get_inds_marray: ids=',ids
        # print 'get_inds_marray: self._inds= ',self._inds
        # print 'get_inds_marray: self._inds[ids,:]= ',self._inds[ids,:]
        # Numeric
        # for i,j in take(bp, take(self._inds,ids) ):
        for i, j in bp[self._inds[ids], :]:
            # print '  i,j=',i,j
            inds += range(i, j)

        # print '  inds=',   array(inds,int)
        return array(inds, int)

    def suggest_id(self, zeroid=False):
        """
        Returns a an availlable id.

        Options:
            zeroid=True allows id to be zero.

        """
        return self.suggest_ids(1, zeroid)[0]

    def suggest_ids(self, n, zeroid=False):
        """
        Returns a list of n availlable ids.
        It returns even a list for n=1. 

        Options:
            zeroid=True allows id to be zero.
        """
        # print
        # 'suggest_ids',n,zeroid,self._inds,len(self._inds),self._inds.dtype
        ids_unused_orig = flatnonzero(less(self._inds, 0))

        if not zeroid:
            if len(self._inds) == 0:
                ids_unused = zeros(0, int)
            else:
                # avoid 0 as id:
                # ids_unused=take(ids_unused,flatnonzero(greater(ids_unused,0)))
                # print '  ids_unused_orig',ids_unused_orig,type(ids_unused_orig)
                # print '  len(ids_unused_orig)',len(ids_unused_orig),ids_unused_orig.shape
                # print '  greater(ids_unused_orig,0)',greater(ids_unused_orig,0)
                # print '  len(greater(ids_unused_orig,0))',len(greater(ids_unused_orig,0))
                # print '  flatnonzero(greater(ids_unused_orig,0))',flatnonzero(greater(ids_unused_orig,0))
                # print '
                # len(flatnonzero(greater(ids_unused_orig,0)))=',len(flatnonzero(greater(ids_unused_orig,0))
                # )
                ids_unused = ids_unused_orig[
                    flatnonzero(greater(ids_unused_orig, 0))]
            zid = 1
        else:
            if len(self._inds) == 0:
                ids_unused = zeros(0, int)
            else:
                ids_unused = ids_unused_orig.copy()

            zid = 0

        n_unused = len(ids_unused)
        n_max = len(self._inds) - 1
        # print '  ids_unused',ids_unused
        # print '  ids_unused.shape',ids_unused.shape
        # print '  len(ids_unused)',len(ids_unused)
        # print '  n_unused,n_max,zid=',n_unused,n_max,zid

        if n_max < zid:
            # first id generation
            ids = arange(zid, n + zid)

        elif n_unused > 0:
            if n_unused >= n:
                ids = ids_unused[:n]
            else:
                # print '  ids_unused',ids_unused
                # print '  from to',n_max+1,n_max+1+n-n_unused
                # print '  arange=',arange(n_max+1,n_max+1+n-n_unused)
                # print '  type(ids_unused)',type(ids_unused)
                # print '  dtype(ids_unused)',ids_unused.dtype
                ids = concatenate(
                    (ids_unused, arange(n_max + 1, n_max + 1 + n - n_unused)))

        else:
            ids = arange(n_max + 1, n_max + 1 + n)

        return ids  # .tolist()

    def add_id_named(self, name, id=-1, **args):
        """
        Creates array entries for id with given name.

        """
        if id < 0:
            id = self.suggest_id()
        # self._name_ids[name]=id
        # print 'add_id_named',self.ident,name,'with id',id
        self.create_ids(id, _name_ids=name, **args)
        return id

    def create_ids(self, ids, **args):
        """
        Creates for each id in ids list an entry for each attribute with 
        array or marray structure.

        If ids is a scalar integer, entries will be generated just for
        this id. 

        For named objects the list _name_ids_ is necessary 
        with a name correisponding to each id.  
        """
        # print 'create_ids',ids,args
        # this is a short dirty way to allow also scalars for ids and arguments
        if type(ids) not in ARRAYTYPES:
                # turn scalar ids in list-vector...
            ids = [ids]
            for key in args.keys():
                args[key] = [args[key]]

        # print '  create_ids',ids,args

        # no ids to create
        if len(ids) == 0:
            return True

        n = len(ids)
        id_max = max(ids)
        id_max_old = len(self._inds) - 1
        n_array_old = len(self)

        ids_existing = take(ids, flatnonzero(less(ids, id_max_old)))
        # print '  ids',ids,'id_max_old',id_max_old,'ids_existing',ids_existing

        # check here if ids are still available
        if sometrue(not_equal(take(self._inds, ids_existing), -1)):
            print 'WARNING in create_ids: some ids already in use', ids_existing
            return False

        # extend index map with -1 as necessary
        if id_max > id_max_old:
            # print 'ext',-1*ones(id_max-id_max_old)
            self._inds = concatenate(
                (self._inds, -1 * ones(id_max - id_max_old, int)))

        # assign n new indexes to new ids
        ind_new = arange(n_array_old, n_array_old + n)

        # print 'ind_new',ind_new
        put(self._inds, ids, ind_new)

        # print '  concat ids..',self._ids,ids
        self._ids = concatenate((self._ids, ids))

        # if each id is supposed to have a _name_ids
        # arguments must provide a list with names for each id
        if self._named:
            i = 0
            names = args['_name_ids']
            for id in ids:
                # print '  id,names[i]',id,names[i]
                self._id_names[names[i]] = id
                i += 1

        # Extend all arrays with n values from args or default
        for attr in self._arrayconfig.keys():
            # print '  attr',attr
            if args.has_key(attr):
                value = args[attr]
                if self._config[attr]['do_init_arrayobj']:
                    # newly assigned instances need to be array initialized
                    for i in range(len(ids)):
                        value[i].init_arrayobj(ids[i], self,
                                               self.make_item_ident(attr, ids[i]))
            else:
                value = self._get_default(attr, ids=ids)

            # print '
            # ',attr,'self.__dict__[attr]',self.__dict__[attr],'value',value
            if len(self.__dict__[attr]) > 0:
                # print '  concat item..',value
                self.__dict__[attr] = concatenate((self.__dict__[attr], value))
            else:
                # tc=self._config[attr]['dtype']
                # print '  first item with tc',tc,'and value',value
                # print '  ', array(value,dtype=tc)[0]
                # self.__dict__[attr]=array(value,dtype=tc)

                # print '  let NumPy, Numeric or whatever guess the first
                # arrayelement'
                self.__dict__[attr] = array(value)

        # Extend all marrays with n values from args or default
        for attr in self._marrayconfig.keys():

            # generate a scalar/list of copies of defaults
            items = args.get(attr, self._get_default(attr, ids=ids))
            self._append_items_marray(attr, ids, items)

        # assign defaults to all dicts
        for attr in self._dictconfig.keys():
            i = 0
            for id in ids:
                if args.has_key(attr):
                    value = args[attr][i]
                    if self._config[attr]['do_init_arrayobj']:
                        # print 'initialize',attr,value
                        value.init_arrayobj(
                            id, self, self.make_item_ident(attr, id))
                else:
                    value = self._get_default(attr, ids=id)

                # print '  dict attr=',attr,value
                self.__dict__[attr][id] = value

                i += 1

        if self._plugins.has_key(23):
            self._execute_plugins(23, ids=ids)

        return True

    def print_attr(self, attr):

        config = self.get_config(attr)
        if config:

            if config['struct'] == 'array':
                # print '   ', attr,'='
                for id in self.get_ids():
                    if config['is_function']:
                        val = self.__getitem__((attr, id))
                    else:
                        val = self.__getitem__((attr, id))

                    print self.get_string_arrayattr(attr, id, val)

            elif config['struct'] == 'marray':
                # print '   ', attr,'='
                for id in self.get_ids():
                    if config['is_function']:
                        val = self.__getitem__((attr, id))
                    else:
                        val = self.__getitem__((attr, id))

                    print self.get_string_arrayattr(attr, id, val)

                    # print '   ',id,self._inds[id],\
                    #    val,config['unit']

            elif config['struct'] == 'dict':
                print '   ', attr, '='
                for id in self.get_ids():
                    val = self.__dict__[attr][id]
                    print self.get_string_arrayattr(attr, id, val)

                    # print '   ',id,self._inds[id],\
                    #    self.__dict__[attr][id],config['unit']

            else:
                ObjConfigurator.print_attr(self, attr)

    def get_string_arrayattr(self, attr, id, value):
        """
        Returns a formatted string for array type attributes 
        with struct array, marray, dict 
        """
        # print 'get_string_arrayattr',attr,id,value
        return '   ' + attr + '[' + str(id) + '] = ' +\
            self.get_string_attr_value(attr, value)

    def reset(self, attr):
        """
        Reset attribute attr to default values
        """
        # create attribute and if there are already arrays with entries,
        # create the same number of entries with default value
        # print 'reset',attr,len(self)
        default = self._config[attr]['default']
        struct = self._config[attr]['struct']
        # declaredtype=self._config[attr]['type'][0]

        # configure if objects or instances need init_arrayobj with id
        # self._config[attr]['do_init_arrayobj']=False # moved to default
        if struct in ('array', 'marray', 'dict'):
            # try to detect from defaults
            if type(default) in [types.InstanceType, types.ClassType]:
                if hasattr(default, 'init_arrayobj'):
                    # print '  detected init_arrayobj!!'
                    self._config[attr]['do_init_arrayobj'] = True

                # done in config
                # elif type(default) == types.ClassType:
                #    if not self._config[attr].has_key('init'):
                #        # by default do not initialize classes
                #        # i they are nor arrayconfiguratored
                #        self._config[attr]['init']=False

            # check if instance or class type have been declared
            # if self._config[attr]['type'] in ['InstanceType','ClassType']:
            #    self._config[attr]['do_init_arrayobj']=True

        # do reset now
        if struct == 'array':
            # generate a scalar/list of copies of defaults
            defaults = self._get_default(attr, ids=self.get_ids())
            # print 'len:',self.__len__(),len(self)

            # register attribute in array database
            # currently no particular info, just key
            self._arrayconfig[attr] = {}

            # print 'reset',attr

            # all this not very clean, but works
            default = self._config[attr]['default']  # the array element
            if type(default) == ndarray:
                # use dtype given by default element which is an array
                tc = default.dtype.str
            else:
                # guess dtype from scalar types
                tc = get_dtype(self._config[attr]['type'][-1])

            self._config[attr]['dtype'] = tc

            # print '  tc=',tc,'type[-1]=',self._config[attr]['type'][-1]
            # this print causes a strange error:
            # 'FloatCanvas' object has no attribute 'this'

            self.__dict__[attr] = array(defaults, dtype=tc)

            # print '  created
            # type',self.__dict__[attr].dtype,self.__dict__[attr].shape

            # self.__dict__[attr]=array(\
            #    self._get_default(attr,ids=self.get_ids()), dtype=tc )

            # print 'new array',self.__dict__[attr]

        elif struct == 'marray':

            # register attribute in array database
            self._marrayconfig[attr] = {'bp': zeros((0, 2), int)}

            tc = get_dtype(self._config[attr]['type'][-1])
            self._config[attr]['dtype'] = tc

            # create new attribute with empty array
            # will be determined with first append
            self.__dict__[attr] = zeros((0), dtype=tc)

            # generate a scalar/list of copies of defaults
            ids = self.get_ids()
            defaults = self._get_default(attr, ids=ids)

            # print 'dtype',tc,default,'type =',self._config[attr]['type']
            if len(self._inds) > 0:
                self._append_items_marray(attr, ids, defaults)

            # print 'reset: new marray',self.__dict__[attr]

        elif struct == 'dict':
            # new attribute is a dictionary
            ids = self.get_ids()
            defaults = self._get_default(attr, ids=self.get_ids())

            # currently no particolar info, just key
            self._dictconfig[attr] = {}
            self.__dict__[attr] = {}
            # assign default value for each already existing id
            for i in range(len(ids)):
                id = ids[i]
                self.__dict__[attr][id] = defaults[i]

            # print 'reset: new dict',attr,'=',self.__dict__[attr]

        else:
            # handle other (scalar) attribute types with base configurator
            ObjConfigurator.reset(self, attr)

    def copy_attrs(self, targetobj, ids=None, targetids=None, attrs=None):
        """
        Copies  given list of attributes of this object to given target object.

        If no attributes are given then all attributes of this object 
        are considered for copying.

        To copy an attribute, the foolowing conditions must be satisfied:
            - the attribute must be configured with key copy=True
            - the attribute must be configured in targetobject 
        """
        if attrs == None:
            # get all attributes
            attrs = self._config.keys()

        if ids == None:
            # first copy all scalars
            ObjConfigurator.copy(targetobj)
            ids = self.get_ids()

        for attr in attrs:
            config = self._config[attr]
            if config['struct'] != 'scalar':
                if config['copy']:
                    if targetobj.is_config(attr):
                        targetobj[attr, targetids] = self[attr, ids]

    def add_items(self, n=0, **args):
        """
        Add new items into array structure.
        The ids for each newly created item are returned in an array.
        Usage:

            Add 3 new items:
            ids=obj.add_items(3)

            Add 3 new items and setting attributes:
            ids=obj.add_items(x=[11,22,33],v=[66,777,8888])

            Add a single new item and set values
            ids=obj.add_items(x=9,v=999)

            Add a single  multi array element composed of 2 scalar integers:
            ids=obj.add_items(s=[[111,2222]])

        """

        # determine number of items to be assigned
        if n == 0:
            if args:
                key = args.keys()[0]
                val = args[key]
                if type(val) not in ARRAYTYPES:
                    # turn scalar val in list-vector...
                    args[key] = [val]
                    n = 1
                else:
                    n = len(val)
            else:
                # add by default one item only
                n = 1
        else:
            pass  # use n from args

        # print 'base.add_items',n

        # determine new id
        ids = self.suggest_ids(n)

        # print ' create new ids',ids
        self.create_ids(ids, **args)

        # set values to attributes in arguments
# for attr in args.keys():
# if attr in self._config.keys():
# print attr,' before =' #,self.__dict__[attr]
# if self._config[attr]['struct']=='array':
# print '  insert in array',args[attr][0]
# print self._inds,ids,self._inds[ids]
# put(self.__dict__[attr],take(self._inds,ids),args[attr])
# print attr,' after =',self.__dict__[attr]
# elif self._config[attr]['struct']=='marray':
# for i in range(n):
# self._insert_item_marray(attr,ids[i],args[attr][i])
##
##
# elif self._config[attr]['struct']=='dict':
# for i in range(n):
# self.__dict__[attr][ids[i]]=args[attr][i]
# print '   dict',attr,self.__dict__[attr][ids[i]]
##
# else:
# print 'WARNING in add_items: unknown attribute:',attr

        return ids

    def __contains__(self, id):
        if (id < len(self._inds)) & (id >= 0):
            return self._inds[id] > -1
        else:
            return False

    def contains_name(self, name):
        return self._id_names.has_key(name)

    def __len__(self):
        """
        Determine current array length (same for all arrays)
        """

        return len(self._ids)

    # def get_index(self,ids):
    #    """
    #    returns index array from ids array
    #    Attetion thyis opration caused a crash during call from __del__
    #    """
    #    #print 'index',self._inds.dtype,type(ids),array(ids,int).dtype,len(ids)
    #
    #    return self._inds[array(ids,int)]
    #    #if type(ids) in ARRAYTYPES:
    #    #    #return take(self._inds,ids)
    #    #    return self._inds[array(ids,int)]
    #    #else:
    #    #    return self._inds[ids]

    def get_attr_named(self, attr, name):
        """
        Returns value of array element of attribute attr
        wich corresponds to id the given name.

        """
        if self._id_names.has_key(name):
            return self[attr, self._id_names[name]]
        else:
            return None

    def set_attr_named(self, attr, name, value):
        """
        Returns value of array element of attribute attr
        wich corresponds to id the given name.

        """
        self[attr, self._id_names[name]] = value

    def __getitem__(self, key):
        # print '__getitem__',key

        if type(key) != types.TupleType:
            # return entire contents of attribure for all ids
            attr = key
            struct = self._config[attr]['struct']
            if struct == 'scalar':
                if self._plugins.has_key(2):
                    self._execute_plugins(2, attr)

                if self._config[attr]['is_function']:
                    return getattr(self, attr)()
                else:
                    return self.__dict__[attr]

            else:
                # attribute is a kind of array
                # take all ids in ascending order
                ids = self.get_ids(ordered=True)
        else:
            # argument specify also id or ids to be returned
            attr, ids = key
            struct = self._config[attr]['struct']

        if type(ids) in ARRAYTYPES:
            # return array or list corresponding to ids
            # print 'get array types',attr,ids,struct
            # print '  self._inds=',self._inds
            # ids=array(ids,int)
            # print '  type (attr),type(ids)',type(self.__dict__[attr]),type(ids)
            # print '  ids.dtype 1=',ids.dtype
            # ids=array(ids,int)
            # print '  ids.dtype 2=',ids.dtype
            # print '  self._inds[ids]=',self._inds[ids]
            # print '  array(ids,int).dtype=',array(ids,int).dtype
            # executes plugins before values are returned to calling instance!!
            if self._plugins.has_key(22):
                self._execute_plugins(22, attr=attr, ids=ids)

            # this avoids problems with zero dimension arrays,
            # (bizzar but possible)

            #if ids.shape==(): ids=array([ids])
            # print '  get array type ids=',ids,struct

            if struct == 'array':
                if self._config[attr]['is_function']:
                    # print 'inds=',take(self._inds,ids)
                    return take(getattr(self, attr)(), take(self._inds, ids))
                else:
                    # print 'inds=',take(self._inds,ids)
                    # print take(self.__dict__[attr],take(self._inds,ids))
                    # return take(self.__dict__[attr],take(self._inds,ids))
                    return self.__dict__[attr][self._inds[array(ids, 'i')], :]

            elif struct == 'marray':
                # here an array is returned with all elements of the
                # those blocks that corresponds to the ids
                # Note that this is a homogenious array ,
                # arrays with single blocks can be obtained by passing
                # a single array.
                if self._config[attr]['is_function']:
                    return getattr(self, attr)()[self.get_inds_marray(attr, ids), :]
                    # Numeric:
                    # return take(getattr(self,attr)(),
                    #        self.get_inds_marray(attr,ids))
                else:

                    # print '  inds_marray=',self.get_inds_marray(attr,ids)
                    # print '  self.__dict__[attr]=\n',self.__dict__[attr]
                    # print '
                    # return=\n',self.__dict__[attr][self.get_inds_marray(attr,ids),:]
                    return self.__dict__[attr][self.get_inds_marray(attr, ids), :]
                    # Numeric:
                    # return take(self.__dict__[attr],
                    #        self.get_inds_marray(attr,ids))

            elif struct == 'dict':
                items = []
                for id in ids:
                    # print '  ',id,self.__dict__[attr][id]
                    items.append(self.__dict__[attr][id])
                return items

            # this will never be executed!!!!!!!!!!!
            # if self._plugins.has_key(22):
            #    self._execute_plugins(22,ids=ids)

        else:
            # print 'return one array element, ids is a single id'
            if self._plugins.has_key(22):
                self._execute_plugins(22, attr, ids=array([ids], int))

            if struct == 'array':
                # print 'inds=',take(self._inds,[ids]),attr,getattr(self,attr)

                if self._config[attr]['is_function']:
                    return getattr(self, attr)()[self._inds[ids]]
                else:
                    # try:
                    return self.__dict__[attr][self._inds[ids]]
                    # except:
                    #    print 'ERROR in __getitem__', attr,ids
                    #    print '  inds=',self._inds[ids]
                    #    print '  attr=',self.__dict__[attr]

            elif struct == 'marray':
                # here an array is returned with all elements of the
                # block that corresponds to this id
                if self._config[attr]['is_function']:
                    return getattr(self, attr)()[self.get_inds_marray(attr, [ids])]
                    # Numeric
                    # return take(getattr(self,attr)(),
                    #        self.get_inds_marray(attr,[ids]))
                else:
                    # print 'ids=',ids
                    # print
                    # 'self.get_inds_marray(attr,[ids]))=',self.get_inds_marray(attr,[ids])
                    return getattr(self, attr)[self.get_inds_marray(attr, [ids])]
                    # Numeric:
                    # return take(self.__dict__[attr],
                    #        self.get_inds_marray(attr,[ids]))

            elif struct == 'dict':
                # print 'attr=',attr,ids
                return self.__dict__[attr][ids]

    def __setitem__(self, key, value):
        """
        Sets a value to an attribute
        or to one or several elements of an attribute if
        a list with ids is passed.

        Usage:
            Scalar assignment
            obj['attrname']=0.0

            Single element asignment
            obj['attrname',3]=33

            Assign 11,22,33 to elements with ids 1,2,3 of attribite attrname
            obj['attrname',[1,2,3]]=[11,22,33]

            Assign 3 elements of different size to marray
            obj['attrname',[1,2,3]]=[[11,12,13],[21,22],[31]]
            Note that the assigned value must be a python list of elements
            and not a Numpy array

        """

        # print 'setitem',key,'to',value

        if type(key) != types.TupleType:
            self.__dict__[key] = value
            if self._plugins.has_key(1):
                self._execute_plugins(1, key)
            return

        else:
            attr, ids = key
            if type(ids) not in ARRAYTYPES:
                # ids is a scalar->make a list
                ids = array([ids], int)
                value = [value]

            struct = self._config[attr]['struct']
            # print 'inds=',self.get_index(ids)
            if struct == 'array':
                shape = self.__dict__[attr].shape
                if len(shape) == 1:
                    # print 'put value',value,type(value)
                    # print '  attr,type(attr)',attr,type(self.__dict__[attr])
                    # print '  dtype(attr)',self.__dict__[attr].dtype
                    # print '
                    # self.get_index(ids)=',self.get_index(ids),type(self.get_index(ids)),self.get_index(ids).dtype

                    self.__dict__[attr][self._inds[array(ids, 'i')]] = value
                    # put(self.__dict__[attr],self.get_index(ids),value)

                    if self._config[attr]['do_init_arrayobj']:
                        # newly assigned instances need to be array initialized
                        for i in range(len(ids)):
                            value[i].init_arrayobj(ids[i], self,
                                                   self.make_item_ident(attr, ids[i]))

                else:
                    # this is an array of matrix or tensors or higher order
                    # the array must be flattened and the indexes must be
                    # calculated accordingly

                    # m=product(shape[1:])
                    # print 'put marray',m,
                    #inds=ones( (len(ids),1),int )*arange(m)\
                    #    +transpose([take(self._inds,ids)])*m
                    # print inds

                    # print 'put value matrix',value,type(value)
                    # print '  attr,type(attr)',attr,type(self.__dict__[attr])
                    # print '  dtype(attr)',self.__dict__[attr].dtype
                    # print '  inds=\n',self._inds[ids]

                    if type(value) != ndarray:
                        # print 'value',value
                        value = array(value, dtype=self._config[attr]['dtype'])
                        # print 'value',value

                    self.__dict__[attr][self._inds[ids], :] = value
                    # Numeric
                    #put(self.__dict__[attr],inds.ravel(), value.ravel())

            if struct == 'marray':
                for i in range(len(ids)):
                    self._insert_item_marray(attr, ids[i], value[i])

            elif struct == 'dict':
                for i in range(len(ids)):
                    self.__dict__[attr][ids[i]] = value[i]
                    if self._config[attr]['do_init_arrayobj']:
                        # newly assigned instances need to be array initialized
                        # self.print_config()
                        value.init_arrayobj(
                            id, self, self.make_item_ident(attr, id))
                        # self.print_config()

            # print 'set_item:check plugins...',self.ident,self._plugins.keys()
            if self._plugins.has_key(21):
                self._execute_plugins(21, attr=attr, ids=ids)

            return

    def del_item_named(self, name):
        """
        Deletes id with given name from all attributes
        """
        del self[self._id_names[name]]

    def __delitem__(self, key):
        # print 'ArrayObjConfigurator.__delitem__',self.ident,'key=',key
        if key in self._config.keys():
            # delete attribute and its configuration
            if self._plugins.has_key(0):
                self._execute_plugins(0, key)

            if self._config[key]['struct'] == 'array':
                del self._arrayconfig[key]
            elif self._config[key]['struct'] == 'dict':
                del self._dictconfig[key][id]
            elif self._config[key]['is_function']:
                self._functions.remove(key)

            for groupname in self._config[key]['group']:
                self._groups[groupname].remove(key)

            del self._config[key]
            del self.__dict__[key]

        else:
            # key is one or multiple ids to be deleted from arrays
            if type(key) not in ARRAYTYPES:
                # ids is a scalar->make a list
                ids = [key]
            else:
                ids = key

            # TODO: here it is not verified whether ids exist
            #  => verify and delete element by element

            # plugin execution BEFORE entry is deleted
            if self._plugins.has_key(20):
                self._execute_plugins(20, ids=ids)

            self._del_ids(ids)

    ##########################################################################
    # private methods

    def _append_items_marray(self, attr, ids, items):
        """
        Appends a list of items to multiary with attribute attr.
        The block pointers are created automatically but
        the indexes and ids must already exist in self._inds 

        ids cannot be empty!
        """
        # print 'append_items_marray',attr
        # print '  ids=',ids
        # print '  items=',items
        marray = self.__dict__[attr]
        # print 'marray=\n',marray
        bp = self._marrayconfig[attr]['bp']
        len_bp = len(bp)

        # make sure that the bp vecor has enough index

        ind_max = max(self._inds)

        bpext = -1 * ones((ind_max - len_bp + 1, 2), int)

        if len_bp == 0:
            bp = bpext
        else:
            bp = concatenate((bp, bpext))
        # print '  after extension bp=',bp

        # set block pointers for each id according to length of each item
        i = len(marray)
        n_elems = 0  # gives the total number of elements inside all items
        for ii in range(len(ids)):
            id = ids[ii]
            ind = self._inds[id]

            # assume that id corresponds to a list with at least one item
            # thus, calculate new block pointers
            n = len(items[ii])
            j = i + n
            n_elems += n
            # print '  id,ind,i,j,len(items[ii])=',id,ind,i,j,len(items[ii])

            bp[ind] = [i, j]

            i = j

        # print '  after asssignment bp=',bp
        self._marrayconfig[attr]['bp'] = bp

        # put items in an array with coherent dimentions
        shape_elem = array(items[0], int).shape[1:]
        shape_array = [n_elems] + list(shape_elem)
        # print ' shape_elem=',shape_elem
        # print ' shape_array=',shape_array
        # extend current marray by the corrct number of new elements
        marrayext = zeros(shape_array, dtype=self._config[attr]['dtype'])
        if len(marray) > 0:
            # print '  concat extention to marray=\n',marrayext
            marray = concatenate((marray, marrayext))
        else:
            # print '  first  marray_extension=\n',marrayext
            marray = marrayext

        # now copy items into array is faster than concatenating each item
        for ii in range(len(ids)):
            id = ids[ii]
            ind = self._inds[id]
            i, j = bp[ind]
            # print '   add item items[ii]',items[ii]
            marray[i:j] = items[ii]

        self.__dict__[attr] = marray

    def _insert_item_marray(self, attr, id, item):
        # print 'insert_item_marray',attr,id,item
        # print '  attr=', self.__dict__[attr]

        # get block pointers for this attribute
        bp = self._marrayconfig[attr]['bp']

        # index of block pointers
        ind = self._inds[id]

        # get from to index of m array for this id
        [i, j] = bp[ind]

        # decide whether to overwrite existing
        if j - i != len(item):
            # first cut current data out of marray and adjust
            # block pointers
            self._del_item_marray(attr, id)

            # append data
            self._append_items_marray(attr, [id], [item])
        else:
            self.__dict__[attr][i:j] = item

        # print '  attr=', self.__dict__[attr]

    def _del_item_marray(self, attr, id):

        # get block pointers for this attribute
        bp = self._marrayconfig[attr]['bp']

        # index of block pointers
        ind = self._inds[id]

        # get from to index of m array for this id
        [i, j] = bp[ind]

        # print 'del_item_marray',attr,id,'from',i,'to',j

        # cut this piece out of the marray
        # print '  before:',self.__dict__[attr]
        self.__dict__[attr] = concatenate(
            (self.__dict__[attr][:i], self.__dict__[attr][j:]))

        # print '  after:',self.__dict__[attr]

        # indicate that block pointer does no more point to anywhere
        # calling method must take care of eliminating it or not
        bp[ind] = [-1, -1]
        # cut out index from blockpointers
        #bp=concatenate( (bp[:ind], bp[ind+1:]) )

        # now subtract j-i from all "to" index higher equal than j
        subtract(bp, (j - i) * (bp >= j), bp)

        self._marrayconfig[attr]['bp'] = bp

    # def _get_bp_marray(self,attr):
    #    """May be useful, just not used """
    #    return self._marrayconfig[attr]['bp']

    def make_item_ident(self, attr, id):
        """
        Generates the ident of attribute attr withy id.
        """
        return self.ident + IDENTSEP + attr + IDENTSEP + str(id)

    def get_groupattrs(self, name, id=-1):
        """
        Returns a dictionary with all attributes of a group. 
        Key is attribute name and value is attribute value for the given id. 
        """
        # print 'get_groupattrs  of',name,'from',self.ident,self._groups.keys()
        # print '  id=',id
        attrs = {}

        if (name in self._groups.keys()) & self.is_ids(id):
            for attr in self._groups[name]:
                # attrs[attr]=self['attr']
                struct = self._config[attr]['struct']
                if (struct in ('dict', 'array', 'marray')):
                    default = self[attr, id]
                    # if default is an initialized class or an instance
                    # and has also a dict or array structure,
                    # then try to configure array object with id

                    if type(default) == types.InstanceType:
                        # default is instance so just make a copy
                        obj = copy(default)
                        # now configure array object
                        # in this case class must have ArrayObj capabilities (mixin)
                        # if self._config[attr]['do_init_arrayobj']:
                        #    obj.init_arrayobj(id,self,self.make_item_ident(attr,id))

                    elif (type(default) == types.ClassType) & \
                            self._config[attr]['do_init_arrayobj']:
                        # default is a class that needs to be initialized first
                        obj = copy(default)
                        # obj.init_arrayobj(id,self,self.make_item_ident(attr,id))

                    elif type(default) == types.FunctionType:
                        # use base function to get other default types
                        obj = default

                    else:
                        # use base function to get other default types
                        obj = copy(default)

                else:
                    # use base function to get other default types
                    obj = self.__dict__[attr]  # attr() copy(default)

                attrs[attr] = obj
        # else:
        #    print '  no attr found',self._groups.keys()
        # print '  attrs=',attrs
        return attrs

    def _get_default(self, attr, n=1, ids=None):
        """
        Returns n default values for attribute with name attr_name
        By default a scalar is returned. 
        If n>1 a list with n defaults is returned.

        Usage:
            Return a scalar default 
            self._get_default('attr_name')    

            Return list with 5 defaults
            self._get_default('attr_name',5)

            Return list with 5 defaults, assigned with ids=[1,2,3,4,5].
            This usage makes only sense with Classes or instances
            with ArrayObj mixin.
            self._get_default('attr_name',ids=[1,2,3,4,5])

        """
        # print '_get_default',self.ident,attr

        default = self._config[attr]['default']
        struct = self._config[attr]['struct']
        defaults = []

        if ids == None:
            # create dummy ids
            ids = range(n)
            returnlist = False

        elif type(ids) not in ARRAYTYPES:
            # just a single id, do return a simple value, not a list
            ids = [ids]
            returnlist = False
        else:
            returnlist = True

        for id in ids:
            if struct in ('dict', 'array', 'marray'):
                # if default is an initialized class or an instance
                # and has also a dict or array structure,
                # then try to configure array object with id

                if type(default) == types.InstanceType:
                    # default is instance so just make a copy
                    obj = copy(default)
                    # now configure array object
                    # in this case class must have ArrayObj capabilities
                    # (mixin)
                    if self._config[attr]['do_init_arrayobj']:
                        obj.init_arrayobj(
                            id, self, self.make_item_ident(attr, id))

                elif (type(default) == types.ClassType) & \
                        self._config[attr]['do_init_arrayobj']:
                    # default is a class that needs to be initialized first
                    obj = default()
                    obj.init_arrayobj(id, self, self.make_item_ident(attr, id))

                elif type(default) == types.FunctionType:
                    # use base function to get other default types
                    obj = default

                else:
                    # use base function to get other default types
                    obj = copy(default)

            else:
                # use base function to get other default types
                obj = ObjConfigurator._get_default(self, attr)

            # append to default list
            defaults.append(obj)

        # print '_get_default array',defaults
        if (n > 1) | returnlist:

            return defaults
        else:
            return defaults[0]

    def _del_ids(self, ids):
        """
        Deletes id from all array - tpe attributes
        """

        # print 'ArrayConfigurator._del_ids',ids,'from',self.ident

        # delete ids from list , one by one
        for id in ids:
            # print '  start deleting id',id

            # print '      self._ids',self._ids
            # print '      self._inds',self._inds

            # i=self.get_index(id)
            i = self._inds[id]

            # print '     ind=',i
            # print '     del rows from array-types...'
            for attr in self._arrayconfig.keys():
                # print '      del',attr,id,i

                # print '        before=\n',self.__dict__[attr]
                self.__dict__[attr] = concatenate(
                    (self.__dict__[attr][:i], self.__dict__[attr][i + 1:]))
                # print '        after=\n',self.__dict__[attr]

            # print '    del from id lookup'
            self._ids = concatenate((self._ids[:i], self._ids[i + 1:]))

            # print '    del from dicts'
            for attr in self._dictconfig.keys():
                # print '      del',attr,id
                del self.__dict__[attr][id]

            # print '    del from marrays'
            for attr in self._marrayconfig.keys():
                self._del_item_marray(attr, id)

                # what's left is to cut out the db pointer with index i
                bp = self._marrayconfig[attr]['bp']

                # print '      del',attr,id,bp

                bp = concatenate((bp[:i], bp[i + 1:]))
                self._marrayconfig[attr]['bp'] = bp

            # print '    free index',id
            if id == len(self._inds) - 1:
                # id is highest, let's shrink index array by 1
                self._inds = self._inds[:-1]
            else:
                self._inds[id] = -1

            # get ids of all indexes which are above i
            ids_above = flatnonzero(self._inds > i)

            # decrease index from those wich are above the deleted one
            #put(self._inds, ids_above,take(self._inds,ids_above)-1)
            self._inds[ids_above] -= 1

            # print '    self._inds',self._inds

        if self._named:
            # print '  Remove named'
            for id in ids:
                name = self._name_ids[id]
                del self._id_names[name]
                # print '    removed',name,id

        # print '  del',ids,' done.'

    def __setstate__test(self, state):
        """
        This is setstate for array config only
        and overrides the scalar version.

        The main purpose is to correct a conversion error in numpy
        """
        print '__setstate__', self

        # this is alays required, but ill not be saved
        self._plugins = {}

        for key in state.keys():
            value = state[key]

            # print '  set state',key
            # print '    attr',key,type(value),
            # if type(value)==ndarray:
            #    print value.dtype,value.shape
            # else:
            #    print value

            if type(value) == ndarray:
                if value.dtype == dtype('|O4'):
                    value = array(value, '<f8')
                print '  set state', key, type(value),
                print '    dtype', value.dtype, value.shape

            self.__dict__[key] = value

        # set other states
        self._setstate(state)


# def del_manage_ids(self,ids):
# """
# Specific item delete, called before removing data.
# """
# pass

class EmptyClass:

    """
    This claass is empty and used as default.

    """

    def __init__(self, parent=None): pass

    def get_ident(self, is_local=None): return 'empty'


class IntegerQueues(ArrayObjConfigurator):

    """
    Class implements multiple queues with the following properties:
        - only integers  can be queued
        - each id can be only in one queue

    Methods are implemented to insert and remove ids in queues
    also in vectorial form.
    """

    def __init__(self, parent, key='queues', name='Queues', items=None):
        """
        Main init used for stand alone.
        """
        self.init_config(key, parent, name)

        self._init_queues(items)

    def _init_queues(self, items=None):
        """
        Queue specific initializations

        items = optional instance that is an array of the integer items
                in the integer queue. The item must inherit IntegerItems
                methods and have this (self) queue class as parent. 
        """

        self.config('ids_first', -1,
                    name='First id',
                    info='ID of first element of each queue',
                    metatype='id',
                    ident_ref='.items',
                    struct='array',
                    )

        self.config('ids_last', -1,
                    name='Last id',
                    info='ID of last element of each queue',
                    metatype='id',
                    ident_ref='.items',
                    struct='array',
                    )

        if items == None:
            # use default items class
            items = IntegerItems(self)

        self.config('items', items,
                    name='items',
                    info='Contain items that are queued in each queue',
                    ident_ref='.items',
                    group='options')

    def add_items(self, ids_item):
        """
        Add ne items for queuing in database
        """
        self.items.create_ids(ids_item)

    def is_empty(self, ids_queue):
        """
        Returns True or False for each queue in the array
        if respective queue is empty or non-empty  
        """
        return self['ids_first', ids_queue] == -1

    def pop_items(self, ids_queue):
        """
        Remove and return last items from queues in array  ids_queue
        """
        ids_items = self['ids_last', ids_queue]

        self.remove_items(ids_items[ids_items > -1])

        return ids_items

    def remove_items(self, ids_item, ids_queue=None):
        """
        Remove integers in array ids_item from queues in array ids_queue
        If no ids_queue is given, it will be recovered from the integer items 
        database. This may only be a disadvantage in terms of time.
        """
        # print 'remove_items'
        # print '  ids_item',ids_item
        # print '  ids_queue',ids_queue

        if ids_queue == None:
            ids_queue = self.items['ids_queue', ids_item]

        inds_queue = self.get_inds(ids_queue)
        inds_item = self.items.get_inds(ids_item)

        # identify single
        inds_single =   (self.ids_last[inds_queue] == ids_item) & \
                        (self.ids_first[inds_queue] == ids_item)

        if any(inds_single):
            # remove single item in queues
            inds_queue_single = inds_queue[inds_single]
            self.ids_last[inds_queue_single] = -1
            self.ids_first[inds_queue_single] = -1

        # identify last
        inds_last = (self.ids_last[inds_queue] == ids_item) &\
            logical_not(inds_single)

        if any(inds_last):
            # remove last items in queues
            inds_queue_last = inds_queue[inds_last]
            inds_item_last = inds_item[inds_last]

            ids_item_new_last = self.items.ids_prev[inds_item_last]
            self.items['ids_next', ids_item_new_last] = -1
            self.ids_last[inds_queue_last] = ids_item_new_last

        # identify first
        inds_first = (self.ids_first[inds_queue] == ids_item) &\
            logical_not(inds_single)

        if any(inds_first):
            # remove first items in queues
            inds_queue_first = inds_queue[inds_first]
            inds_item_first = inds_item[inds_first]

            ids_item_new_first = self.items.ids_next[inds_item_first]
            self.items['ids_prev', ids_item_new_first] = -1
            self.ids_first[inds_queue_first] = ids_item_new_first

        # identify middle
        inds_middle =   logical_not(inds_single) &\
            logical_not(inds_last) & logical_not(inds_first)

        if any(inds_middle):
            # remove first items in queues
            inds_queue_middle = inds_queue[inds_middle]
            inds_item_middle = inds_item[inds_middle]

            # cutting items out of chains
            ids_item_new_next = self.items.ids_next[inds_item_middle]
            ids_item_new_prev = self.items.ids_prev[inds_item_middle]

            self.items['ids_next', ids_item_new_prev] = ids_item_new_next
            self.items['ids_prev', ids_item_new_next] = ids_item_new_prev

        # finally remove pointer from items to queues
        self.items.ids_queue[inds_item] = -1
        self.items.ids_next[inds_item] = -1
        self.items.ids_prev[inds_item] = -1

    def insert_items_behind(self, ids_item, ids_item_beh, ids_queue):
        """
        Insert integers in array ids_item into queues in array ids_queue
        behind integers in array ids_item_beh
        """
        # print 'insert_items_beh'
        # print '  ids_item',ids_item
        # print '  ids_queue',ids_queue
        # print '  ids_item_beh',ids_item_beh

        # identify prepend
        inds_prepend = ids_item_beh == -1
        if any(inds_prepend):
            self.prepend_items(ids_item[inds_prepend],
                               ids_queue[inds_prepend])

        # identify append
        inds_append = (self['ids_last', ids_queue] == ids_item_beh) \
            & logical_not(inds_prepend)
        if any(inds_append):
            self.append_items(ids_item[inds_append],
                              ids_queue[inds_append])

        # select rest
        inds = logical_not(inds_prepend) & logical_not(inds_append)

        if any(inds):
            # insert items where insertion somewhere in the middle
            # of the buffer where first and last item is not effected

            # this task can therefore be handeled by item instance
            self.items.insert_items_behind(ids_item[inds],
                                           ids_item_beh[inds],
                                           ids_queue[inds],
                                           )

    def append_items(self, ids_item, ids_queue):
        """
        Append integers in array ids_items to queues in array ids_queue
        """
        # print 'append_items'
        # print '  ids_item',ids_item
        # print '  ids_queue',ids_queue

        # different treatmet of empty and not empty queues
        inds_empty = self.is_empty(ids_queue)
        inds_not_empty = logical_not(inds_empty)

        # treat first empty queues
        if len(inds_empty) > 0:
            # print '  inds_empty',inds_empty
            self['ids_first', ids_queue[inds_empty]] = ids_item[inds_empty]
            self['ids_last', ids_queue[inds_empty]] = ids_item[inds_empty]

        if any(inds_not_empty):

            # treat queues with at least one item
            ids_not_empty = ids_queue[inds_not_empty]
            ids_item_not_empty = ids_item[inds_not_empty]

            # fix new first and last item
            ids_item_prev = self['ids_last', ids_not_empty]
            self['ids_last', ids_not_empty] = ids_item_not_empty

            # backward chaining
            self.items['ids_prev', ids_item_not_empty] = ids_item_prev

            # forward chaining
            self.items['ids_next', ids_item_prev] = ids_item_not_empty

        # assign queue id
        self.items['ids_queue', ids_item] = ids_queue

    def prepend_items(self, ids_item, ids_queue):
        """
        Prepend integers in array ids_items to queues in array ids_queue
        """
        # print 'prepend_items'
        # print '  ids_item',ids_item
        # print '  ids_queue',ids_queue

        # different treatmet of empty and not empty queues
        inds_empty = self.is_empty(ids_queue)
        inds_not_empty = logical_not(inds_empty)

        # treat first empty queues
        if len(inds_empty) > 0:
            # print '  inds_empty',inds_empty
            self['ids_first', ids_queue[inds_empty]] = ids_item[inds_empty]
            self['ids_last', ids_queue[inds_empty]] = ids_item[inds_empty]

        if any(inds_not_empty):

            # treat queues with at least one item
            ids_not_empty = ids_queue[inds_not_empty]
            ids_item_not_empty = ids_item[inds_not_empty]

            # fix new first and last item
            ids_item_next = self['ids_first', ids_not_empty]
            self['ids_first', ids_not_empty] = ids_item_not_empty
            # self['ids_last',ids_not_empty] = ids_items_next # not changed

            # forward chaining
            self.items['ids_next', ids_item_not_empty] = ids_item_next

            # backward chaining
            self.items['ids_prev', ids_item_next] = ids_item_not_empty

        # assign queue id
        self.items['ids_queue', ids_item] = ids_queue

    def print_info(self):
        """
        Print all usueful info
        """
        self.print_config()
        self.items.print_config()


class IntegerItems(ArrayObjConfigurator):

    """
    Class cotaining integer items (the ids) of the integer queue.

    This class can be used as mixin in order to add customized attributes to
    each item.

    """

    def __init__(self, parent, key='items', name='Integer items'):
        """
        Main init used for stand alone.
        """
        self.init_config(key, parent, name)

        self._init_items()

    def _init_items(self):
        """
        Here the item specific states are initialized.

        queue_data is an optional database with additional information 
            on the queues where each id correspnds to a queue

        """

        self.config('ids_queue', -1,
                    name='queue id',
                    info='ID of queue',
                    metatype='id',
                    ident_ref=self.parent.ident,
                    struct='array',
                    )

        self.config('ids_next', -1,
                    name='next id',
                    info='ID of next',
                    metatype='id',
                    ident_ref='.',
                    struct='array',
                    )

        self.config('ids_prev', -1,
                    name='previous id',
                    info='ID of previous',
                    metatype='id',
                    ident_ref='.',
                    struct='array',
                    )

    def insert_items_behind(self, ids_item, ids_item_beh, ids_queue):
        """
        UNUSED!! see integerqueue!!

        Insert integers in array ids_item into queues in array ids_queue
        behind integers in array ids_item_beh.

        Please use insert_behind of the governing queue class!

        Attention this method is called by insert_behind of the 
        governing queue class, which selected already the cases where
        this insertion is in the middle of the queue and does not effect
        first or last item. 
        """
        # print 'IntegerItems.insert_items_beh'
        # print '  ids_item',ids_item
        # print '  ids_queue',ids_queue
        # print '  ids_item_beh',ids_item_beh

        inds_item = self.get_inds(ids_item)
        inds_item_beh = self.get_inds(ids_item_beh)

        # forward chaining
        ids_item_next = self.ids_next[inds_item_beh]
        self.ids_next[inds_item] = ids_item_next
        self.ids_next[inds_item_beh] = ids_item

        # backward chaining
        self.ids_prev[inds_item] = ids_item_beh
        self['ids_prev', ids_item_next] = ids_item

        # assign queue id to inserted elements
        self.ids_queue[inds_item] = ids_queue


class MetaList(ArrayObjConfigurator):

    """
    Empty class with ArrayObjConfigurator properties.
    Can be freely configured to contain any kind of database like,
    ordered list structures.

    Usage:
         l=MetaList()
         l.config('m',0.0,struct='marray')
         l.create_ids([1,2,3,4])
         l.print_config()
    """

    def __init__(self, **args):
        self.init_config('metalist')


class TestNamed(ArrayObjConfigurator):

    def __init__(self, **args):
        self.init_config('test', named=True)
        self.config('s', zeros((0,), float), struct='marray',
                    type=['IntType'])

        self.config('v', 0, struct='array')
        self.add_id_named('one')
        self.add_id_named('two')


class TestMod(ObjConfigurator, ArrayObj):

    """
    test
    """

    def __init__(self, **args):
        """
        Mod specific Initialization.
        self.id an parent is yet unknown 
        """
        pass
        # print 'initialized TestMod'

    def config_with_id(self):
        """
        Mod specific Initialization.
        Now id and parent is known.
        """
        self.init_config()
        # print 'config_with_id',self.ident
        self.config('y', 0.0, group=['state'], perm='w', digits_fraction=2)
        self.config('z', 'text', group=['state'], perm='w')
        self.config('cabins', Cabs(self))
        # self.print_config()


class Car(ArrayObj):

    """
    test
    """

    def __init__(self, **args):
        """
        Mod specific Initialization.
        self.id an parent is yet unknown 
        """
        print 'initialized CarMod'

    def config_with_id(self):
        """
        Mod specific Initialization.
        Now id and parent is known.
        """
        pass


class CarTest(ArrayObjConfigurator):

    """
    test
    """

    def __init__(self, **args):
        """Variable
        """
        self.init_config('carstest')
        self.config('a', 0.0, struct='array', group=[
                    'state', 'costs'], unit='m/s^2')
        # self.config('mods',TestMod,struct='dict')
        #self.config('m',[0.0, 0.0, 0.0],struct='array')
        self.config('g', zeros((3, 4)), struct='array', unit='earth accel')
        newids = self.suggest_ids(3)
        self.create_ids(newids)


class Cars(ArrayObjConfigurator):

    """
    test
    """

    def __init__(self, **args):
        """Variable
        """
        self.init_config('cars')
        self.config('x', 0.456685, group=[
                    'state'], perm='w', digits_fraction=2)

        self.config('color', [0.2, 0.8, 0.2], metatype='color',
                    struct='array', group=['parameter'], perm='w')
        self.config('load', 'user', struct='dict', perm='r')
        self.config('n', 0,  struct='array', info='number', perm='w',
                    group=['state', 'parameters'],
                    min=0, max=5)
        self.config('b', False, struct='array', perm='w',
                    info='bool...', group=['state', 'costs'])
        self.config('a', 0.0, struct='array', info='acceleration',
                    group=['state', 'costs'], digits_fraction=2)
        self.config('v', 0.0, struct='array', info='velocity',
                    group=['state'], unit='m/s', digits_fraction=2)
        self.config('d', 0.0, unit='m', struct='array',
                    info='this is \nthe distance', digits_fraction=2)
        self.config('j', 0.0, struct='array', perm='r', digits_fraction=2)
        self.add_items(v=[8.7, 5.5, 34.1, 2], a=[-8, -5, -34, -2])

        self.config('mods', TestMod, struct='dict')
        self.config('m', [0.0, 0.0, 0.0], struct='array')
        self.config('g', zeros((3, 4)), struct='array')
        self.config('coords', array([0.0, 0.0, 0.0], float),
                    # type=['FloatType'],
                    struct='array',
                    name='coordinates',
                    info='Array with xyz coordinates of each node')
        newids = self.suggest_ids(3)
        self.create_ids(newids, v=[5, 6, 7])
        # print 'config=\n',self.get_config()


class ObjConfiguratorTest(ObjConfigurator):

    """
    test
    """

    def __init__(self, parent=None, **args):
        """Variable
        """
        self.init_config('ObjConfiguratorTest', parent)
        self.config('d', [[1, 2]], type=[('ArrayType', (1, 2)), 'IntType'])


class Cabs(ArrayObjConfigurator):

    """
    test
    """

    def __init__(self, parent=None, **args):
        """Variable
        """
        self.init_config('cabs', parent)
        self.config('d', [[1, 2]], type=[
                    ('ArrayType', (1, 2)), 'IntType'], struct='dict')
        self.create_ids([1, 2, 3, 4])
        self.config('a', 0.0, struct='array', group=[
                    'state', 'costs'], unit='m/s^2')
        self.config('n', -1, struct='array')
        self.config('g', zeros((3, 4)), struct='array', perm='r')
        #self.config('v',0.0, struct='array', group=['state'])
        # self.config('d',0.0,unit='m')
        # self.config('j',0.0,struct='array')
        # self.add_items(v=[8.7,5.5,34.1,2],a=[-8,-5,-34,-2])
        # self.config('coords',array([0.0,0.0,0.0]),
        #            struct='array',
        #            name='coordinates',
        #            info='Array with xyz coordinates of each node')


class MarrayTest(ArrayObjConfigurator):

    """
    test
    from base import *;t=MarrayTest();t['s',1]=[11,22,33,44];t['s',3]=[-33,-44]
    t.get_ids_marray('s',[1,])
    t['s',2]=[22,222,2222,22222]
    t['s',[1,2]]=[[3,33,333],[55,555,5555,5555]]
    t.get_ids_marray('s',[1,2,3])
    t.get_ids_marray('s',[1,])

    """

    def __init__(self, **args):
        self.init_config('test')
        self.config('s', zeros((0,)), struct='marray',
                    type=['IntType'])

        self.config('v', 0, struct='array')

        self.config('x', [[-1, -1]],
                    name='forward star',
                    info='Dict with node id as key and forward nodes as values',
                    doc='A dictionary with node id as key and \n' +
                         'a dictionary with follower nodes as value. \n' +
                         'Each follower node is a dictionary with \n' +
                         'predecessor node id as key and arc id as value',
                    type=[('ArrayType', (1, 2)), 'IntType'],
                    struct='marray',
                    )

        self.config('f', zeros((0,), dtype='i'),
                    name='forward nodes',
                    info='Dict with node id as key and forward nodes as values',
                    doc='A dictionary with node id as key and \n' +
                         'a dictionary with follower nodes as value. \n' +
                         'Each follower node is a dictionary with \n' +
                         'predecessor node id as key and arc id as value',
                    type=[('ArrayType', (1,)), 'IntType'],
                    struct='marray',
                    )

        self.config('m', [[0.0, 0.0, 0.0], [10.0, 10.0, 0.0]], struct='marray')
        self.create_ids([1, 2, 3, 4])
        # self.config('s',[-1],struct='marray')
        self.print_config()


def test_queue():
    q = IntegerQueues(None)
    q.create_ids([1, 2, 3, 4])
    q.add_items(arange(0, 20, 2, int))

    q.print_info()

    # q.prepend_items(array([2,1],int),array([4,3],int))
    # q.prepend_items(array([2,],int),array([3,],int))
    q.insert_items_behind(array([2, ], int), array(
        [-1, ], int), array([2, ], int))

    q.print_info()

    q.insert_items_behind(array([16, ], int), array(
        [2, ], int), array([2, ], int))

    q.print_info()

    q.prepend_items(array([4, 8], int), array([3, 2], int))

    q.print_info()

    q.append_items(array([6, ], int), array([1, ], int))

    q.print_info()

    q.append_items(array([10, 12], int), array([1, 3], int))

    q.print_info()

    q.insert_items_behind(array([18, ], int), array(
        [2, ], int), array([2, ], int))

    q.print_info()

    q.remove_items(array([18, ], int))

    q.print_info()

    q.remove_items(array([16, ], int))

    q.print_info()

    q.remove_items(array([8, ], int))

    q.print_info()

    q.remove_items(array([2, ], int))

    q.print_info()

    print '\npop all', q.pop_items(q.get_ids())
    print '\npop all', q.pop_items(q.get_ids())
    print '\npop all', q.pop_items(q.get_ids())

###############################################################################
if __name__ == '__main__':
    """
    Test
    """

    test_queue()

    exit(0)

    # a=array([[1,2,3],[4,5,6],[7,8,9],[11,22,33]])
    # b=array([[1,0,1],[1,1,1],[1,0,0],[0,0,0]])

    # cd C:\Documents and Settings\joerg\Documenti\projects\its
    # import sys; sys.path[:0] = ['C:\Users\joerg\projects\sim\its']
    #from lib.base import *
    cabs = Cabs()
    cabs.print_config()

    # t=MarrayTest();t['s',1]=[11,22,33,44];t['s',3]=[-33,-44]
    # t.get_ids_marray('s',[1,])
    # t['s',2]=[22,222,2222,22222]
    # t['s',[1,2]]=[[3,33,333],[55,555,5555,5555]]
    # t.get_ids_marray('s',[1,2,3])
    # t.get_ids_marray('s',[1,])

    t = ObjConfiguratorTest()
    t.print_config()

    save_obj(t, 'test.txt')
    t = load_obj('test.txt')
