from classman import *
import numpy as np


class ArrayConfMixin:

    def __init__(self, attrname, default, dtype=None, is_index=False, **attrs):
        self._is_index = is_index
        self._dtype = dtype
        AttrConf.__init__(self,  attrname, default,
                          struct='array',
                          **attrs)

        if is_index:
            self._init_indexing()

    # use original one from AttrConfig
    # def _write_xml_value(self,val,fd):
    #    #print 'write_xml',self.xmltag,type(val),hasattr(val, '__iter__')
    #    if hasattr(val, '__iter__'):
    #        if len(val)>0:
    #            if hasattr(val[0], '__iter__'):
    #                # matrix
    #                fd.write(xm.mat(self.xmltag,val))
    #            else:
    #                if type(val)==np.ndarray:
    #                    # vector
    #                    fd.write(xm.arr(self.xmltag,val,sep=','))
    #                else:
    #                    # list
    #                    fd.write(xm.arr(self.xmltag,val))
    #        else:
    #            # empty list
    #            fd.write(xm.arr(self.xmltag,val))
    #    else:
    #        # scalar number or string
    #        fd.write(xm.num(self.xmltag,val))

    def get_dtype(self):
        return self._dtype

    def convert_type(self, array):
        return np.array(array, dtype=self._dtype)

    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        # print '\n\nget_defaults',self._name,ids
        default = self.get_default()

        if hasattr(default, '__iter__'):
            default = np.asarray(default)
            if self._dtype != None:
                dtype = self._dtype
            else:
                dtype = type(default.flatten()[0])
            # print ' default=',default,len(default)
            if len(ids) > 0:
                defaults = np.array(len(ids) * [default], dtype)
                # print '  size,type',len(ids)*[default],
                # type(default.flatten()[0])
            else:
                #defaults = np.zeros(  (0,len(default)),type(default.flatten()[0]) )
                defaults = np.zeros((0,) + default.shape, dtype)
            # print '  return',defaults,defaults.shape,defaults.dtype
            return defaults
        else:
            if self._dtype != None:
                dtype = self._dtype
            else:
                dtype = type(default)
            #defaults=  np.array(  len(ids)*[default], dtype )
            # print '  return 1D',defaults,defaults.shape,defaults.dtype
            return np.array(len(ids) * [default], dtype)

    def get_init(self):
        """
        Returns initialization of attribute.
        Usually same as get_default for scalars.
        Overridden by table configuration classes
        """
        ids = self._manager.get_ids()

        # print '\n\nget_init',self.attrname,ids,self._is_localvalue
        values = self.get_defaults(ids)

        # store locally if required
        if self._is_localvalue:
            self.value = values
        # pass on to calling instance
        # in this cas the data is stored under self._obj
        return values

    def get_ids_sorted(self):
        inds = self._manager.get_inds()
        return self._manager.get_ids(inds[np.argsort(self.get_value()[inds])])

        #sortarray = np.concatenate((self.get_value()[inds],inds))

    def delete_ind(self, i):
        # called from del_rows
        if self._is_index:
            _id = self._manager._ids[i]
            self.del_index(_id)
        arr = self.get_value()
        self.set_value(np.concatenate((arr[:i], arr[i + 1:])))

    def __delitem__(self, ids):
        # print '        before=\n',self.__dict__[attr]
        #attr = self.attrconf.get_attr()
        if hasattr(ids, '__iter__'):
            for i in self._manager._inds[ids]:
                self.delete_ind[i]
        else:
            self.delete_ind(self._manager._inds[ids])

    def __getitem__(self, ids):
        # print '__getitem__',key
        return self.get_value()[self._manager._inds[ids]]

    def __setitem__(self, ids, values):
        # print
        # '__setitem__',ids,values,type(self.get_value()),self.get_value().dtype

        if self._is_index:
            if hasattr(ids, '__iter__'):
                self.set_indices(ids, values)

            else:
                self.set_index(ids, values)

        self.get_value()[self._manager._inds[ids]] = values

    def set(self, ids, values):
        if values == None:
            return

        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            _values = np.array([values], self._dtype)

        else:
            _ids = ids
            _values = np.array(values, self._dtype)
        # print 'set',  _ids ,_values
        self[_ids] = _values
        if self._is_index:
            self.set_indices(_ids, _values)
        self._is_modified = True

    def set_plugin(self, ids, values):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            _values = np.array([values], self._dtype)

        else:
            _ids = ids
            _values = np.array(values, self._dtype)

        self[ids] = _values
        if self._is_index:
            self.set_indices(_ids, _values)
        self._is_modified = True
        self.plugin.exec_events_ids(EVTSETITEM, _ids)

    def add(self, ids, values=None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.array([values], self._dtype)
            else:
                _values = self.get_defaults(_ids)

        else:
            # if values==None:
            #    print 'WARNING:',self.attrname, ids,self._dtype
            _ids = ids
            if values != None:
                _values = np.array(values, self._dtype)
            else:
                _values = self.get_defaults(_ids)
        # if values == None:
        #    _values = self.get_defaults(_ids)

        # print 'add ids',self.attrname,ids,_ids,self._is_modified
        # print '  values',values
        # print '  _values',_values
        # print '  self.get_value()',self.get_value()
        # print '  type(_values),type(self.get_value())',type(_values),type(self.get_value())
        # print '
        # _values.shape,self.get_value().shape',_values.shape,self.get_value().shape

        #newvalue = np.concatenate((self.get_value(),_values))
        # print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(), _values)))
        # print '  done:',self.attrname,self.get_value()
        if self._is_index:
            self.add_indices(_ids, _values)
        self._is_modified = True

    def add_plugin(self, ids, values=None):
        # print 'add_plugin',self.attrname
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.array([values], self._dtype)

        else:
            _ids = ids
            if values != None:
                _values = np.array(values, self._dtype)

        if values == None:
            _values = self.get_defaults(_ids)
        self._is_modified = True
        # print 'add ids',self.attrname,ids,_ids,self._is_modified
        # print '  values',values
        # print '  _values',_values
        # print '  self.get_value()',self.get_value()
        # print '  type(_values),type(self.get_value())',type(_values),type(self.get_value())
        # print '
        # _values.shape,self.get_value().shape',_values.shape,self.get_value().shape

        #newvalue = np.concatenate((self.get_value(),_values))
        # print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(), _values)))
        # print '  done:',self.attrname,self.get_value()

        if self._is_index:
            self.add_indices(_ids, _values)

        self.plugin.exec_events_ids(EVTADDITEM, _ids)

    def format_value(self, _id, show_unit=False, show_parentesis=False):
        if show_unit:
            unit = ' ' + self.format_unit(show_parentesis)
        else:
            unit = ''
        # return repr(self[_id])+unit

        #self.min = minval
        #self.max = maxval
        #self.digits_integer = digits_integer
        #self.digits_fraction = digits_fraction
        val = self[_id]
        tt = type(val)

        if tt in (np.int, np.int32, np.float64):
            return str(val) + unit

        elif tt in (np.float, np.float32, np.float64):
            if hasattr(attrconf, 'digits_fraction'):
                digits_fraction = self.digits_fraction
            else:
                digits_fraction = 3
            return "%." + str(digits_fraction) + "f" % (val) + unit

        else:
            return str(val) + unit


class ArrayConf(ArrayConfMixin, ColConf):

    """
    Column made of numeric array.

    """
    pass


class ListArrayConf(ArrayConfMixin, ColConf):

    """
    Column made of an array of lists.

    """

    def __init__(self, attrname, dtype=None,  **attrs):
        ArrayConfMixin.__init__(self,  attrname, None,
                                dtype=np.object,  **attrs)

    def add(self, ids, values=None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.zeros(1, self._dtype)
                _values[0] = values

        else:

            _ids = ids
            if values != None:
                _values = np.zeros(len(ids), self._dtype)
                _values[:] = values

        if values == None:
            _values = self.get_defaults(_ids)

        # print 'add ids, _values',self.attrname,ids
        # print '  values',values
        # print '  _values',_values
        # print '  self.get_value()',self.get_value()
        # print '  type(_values),type(self.get_value())',type(_values),type(self.get_value())
        # print '
        # _values.shape,self.get_value().shape',_values.shape,self.get_value().shape

        newvalue = np.concatenate((self.get_value(), _values))
        # print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(), _values)))

        if self._is_index:
            self.add_indices(_ids, _values)

    def add_plugin(self, ids, values=None):
        # print 'add_plugin',self.attrname
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.zeros(1, self._dtype)
                _values[0] = values

        else:

            _ids = ids
            if values != None:
                _values = np.zeros(len(ids), self._dtype)
                _values[:] = values

        if values == None:
            _values = self.get_defaults(_ids)

        # print 'add ids, _values',self.attrname,ids
        # print '  values',values
        # print '  _values',_values
        # print '  self.get_value()',self.get_value()
        # print '  type(_values),type(self.get_value())',type(_values),type(self.get_value())
        # print '
        # _values.shape,self.get_value().shape',_values.shape,self.get_value().shape

        newvalue = np.concatenate((self.get_value(), _values))
        # print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(), _values)))

        if self._is_index:
            self.add_indices(_ids, _values)

        if self.plugin:
            self.plugin.exec_events_ids(EVTADDITEM, _ids)


class NumArrayConf(ArrayConfMixin, ColConf):

    """
    Column made of numeric array.

    """
    # def __init__(self, **attrs):
    #    print 'ColConf',attrs

    def __init__(self, attrname, default,
                 digits_integer=None, digits_fraction=None,
                 minval=None, maxval=None,
                 **attrs):

        self.min = minval
        self.max = maxval
        self.digits_integer = digits_integer
        self.digits_fraction = digits_fraction

        ArrayConfMixin.__init__(self,  attrname, default,
                                metatype='number', **attrs)


class IdsArrayConf(ArrayConfMixin, ColConf):

    """
    Column, where each entry is the id of a single Table. 
    """

    def __init__(self, attrname, tab,  is_index=False, id_default=-1, **kwargs):
        self._tab = tab
        ArrayConfMixin.__init__(self,  attrname,
                                id_default,  # default id
                                dtype=int,
                                metatype='id',
                                perm='r',
                                is_index=is_index,
                                **kwargs
                                )
        self.init_xml()
        # print 'IdsConf.__init__',attrname
        # print '  ',self._tab.xmltag,self._attrconfig_id_tab

    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        # print '\n\nget_defaults',self.attrname,ids,self.get_default()
        return -np.ones(len(ids), dtype=self._dtype)


#-------------------------------------------------------------------------
    # copied from IdsConf!!!
    def set_linktab(self, tab):
        self._tab = tab

    def get_linktab(self):
        return self._tab

    def init_xml(self):
        # print 'init_xml',self.attrname
        if self._tab.xmltag != None:
            xmltag_tab, xmltag_item_tab, attrname_id_tab = self._tab.xmltag
            if (attrname_id_tab is None) | (attrname_id_tab is ''):
                self._attrconfig_id_tab = None
            else:
                self._attrconfig_id_tab = getattr(
                    self._tab, attrname_id_tab)  # tab = tabman !

            if not hasattr(self, 'is_xml_include_tab'):
                # this means that entire table rows will be included
                self.is_xml_include_tab = False
            # print '  xmltag_tab, xmltag_item_tab,
            # attrname_id_tab',xmltag_tab, xmltag_item_tab,
            # attrname_id_tab,self.is_xml_include_tab

        else:
            self._attrconfig_id_tab = None
            self.is_xml_include_tab = False

    def write_xml(self, fd, _id, indent=0):
        # print 'write_xml',self.attrname
        if (self.xmltag is not None) & (np.all(self[_id] >= 0)):
            # print 'write_xml',self.attrname, _id,'value',self[_id]
            if self._attrconfig_id_tab == None:
                self._write_xml_value(self[_id], fd)
            elif self.is_xml_include_tab:
                # print '    write table row(s)',self[_id]
                self._tab.write_xml(fd, indent, ids=self[_id],
                                    is_print_begin_end=False)
            else:
                # print '    write id(s)',self[_id]
                self._write_xml_value(self._attrconfig_id_tab[self[_id]], fd)

    def _write_xml_value(self, val, fd):
        # print 'write_xml',self.xmltag,hasattr(val, '__iter__')
        if hasattr(val, '__iter__'):
            if len(val) > 0:
                if hasattr(val[0], '__iter__'):
                    # matrix
                    fd.write(xm.mat(self.xmltag, val))
                else:
                    # list
                    fd.write(xm.arr(self.xmltag, val, self.xmlsep))
            else:
                # empty list
                # fd.write(xm.arr(self.xmltag,val))
                # don't even write empty lists
                pass

        elif type(self._default) in (types.UnicodeType, types.StringType):
            if len(val) > 0:
                fd.write(xm.num(self.xmltag, val))

        else:
            # scalar number or string
            fd.write(xm.num(self.xmltag, val))

    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        if self._is_save:
            # if self._is_child:
            #    # OK self.value already set in
            #    pass
            # else:
            #    # remove table reference and create ident
            state['_tab'] = None
            state['_ident_tab'] = self._tab.get_ident_abs()

    def init_postload_internal(self, man, obj):
        # print
        # 'IdsConf.init_postload_internal',self.attrname,hasattr(self,'value'),self._is_save,self._is_localvalue,'obj:',obj.ident

        AttrConf.init_postload_internal(self, man, obj)

        # if self._is_child:
        #    print '  make sure children get initialized'
        #    print '  call init_postload_internal of',self._tab.ident
        #    self._tab.init_postload_internal(obj)

    def init_postload_external(self):
        # if self._is_child:
        #    # restore normally
        #    AttrConf.init_postload_external(self)
        #    self._tab.init_postload_external()
        # else:

        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        #
        ident_abs = self._ident_tab
        # print 'reset_linkobj',self.attrname,ident_abs
        obj = self.get_obj()
        rootobj = obj.get_root()
        # print '  rootobj',rootobj.ident
        linkobj = rootobj.get_obj_from_ident(ident_abs)
        # print '  linkobj',linkobj.ident
        self._tab = linkobj
        self.init_xml()

    def is_modified(self):
        return False

    def set_modified(self, is_modified):
        pass


class IdlistsArrayConf(IdsArrayConf):

    """
    Column, where each entry is a list of ids of a single Table. 
    """

    def __init__(self, attrname, tab, metatype=None, **kwargs):
        self._is_index = False
        self._tab = tab
        ArrayConfMixin.__init__(self,  attrname,
                                # default, will be substituted by id list
                                None,
                                dtype='object',
                                metatype='ids',
                                perm='r',
                                **kwargs
                                )
        self.init_xml()

    def get_defaults(self, ids):
        # here we initialize with None for reach element
        return np.array(len(ids) * [None, ], self._dtype)

    def add(self, ids, values=None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.zeros(1, self._dtype)
                _values[0] = values

        else:

            _ids = ids
            _values = np.zeros(len(ids), self._dtype)
            _values[:] = values

        if values == None:
            _values = self.get_defaults(_ids)

        # print 'add ids, _values',self.attrname,ids
        # print '  values',values
        # print '  _values',_values
        # print '  self.get_value()',self.get_value()
        # print '  type(_values),type(self.get_value())',type(_values),type(self.get_value())
        # print '
        # _values.shape,self.get_value().shape',_values.shape,self.get_value().shape

        newvalue = np.concatenate((self.get_value(), _values))
        # print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(), _values)))

        if self._is_index:
            self.add_indices(_ids, _values)


class TabIdListArrayConf(ArrayConfMixin, ColConf):

    """
    Column made of an array of lists with (table,id) tupels.
    The tables are linked, and will not be saved.
    """

    def __init__(self, attrname, dtype=None,  **attrs):
        ArrayConfMixin.__init__(self, attrname,  None,  # default, will be substituted by (table,id) list
                                dtype='object',
                                metatype='tabidlist',
                                perm='r', **attrs)

    def add(self, ids, values=None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.zeros(1, self._dtype)
                _values[0] = values

        else:

            _ids = ids
            _values = np.zeros(len(ids), self._dtype)
            _values[:] = values

        if values == None:
            _values = self.get_defaults(_ids)

        # print 'add ids, _values',self.attrname,ids
        # print '  values',values
        # print '  _values',_values
        # print '  self.get_value()',self.get_value()
        # print '  type(_values),type(self.get_value())',type(_values),type(self.get_value())
        # print '
        # _values.shape,self.get_value().shape',_values.shape,self.get_value().shape

        newvalue = np.concatenate((self.get_value(), _values))
        # print '  ', type(newvalue),newvalue.dtype
        self.set_value(np.concatenate((self.get_value(), _values)))

        if self._is_index:
            self.add_indices(_ids, _values)

    def add_plugin(self, ids, values=None):
        if not hasattr(ids, '__iter__'):
            _ids = [ids]
            if values != None:
                _values = np.zeros(1, self._dtype)
                _values[0] = values

        else:

            _ids = ids
            _values = np.zeros(len(ids), self._dtype)
            _values[:] = values

        if values == None:
            _values = self.get_defaults(_ids)

    ###

    def format_value(self, _id, show_unit=False, show_parentesis=False):
        s = ''
        rowlist = self[_id]
        if rowlist == None:
            return s
        elif len(rowlist) == 0:
            return s
        elif len(rowlist) == 1:
            tab, ids = rowlist[0]
            return str(tab) + '[' + str(ids) + ']'
        elif len(rowlist) > 1:
            tab, ids = rowlist[0]
            s = str(tab) + '[' + str(ids) + ']'
            for tab, ids in rowlist[1:]:
                s += ',' + str(tab) + '[' + str(ids) + ']'
            return s

    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        # print '_getstate_specific',self.attrname,
        # self._is_save,len(self.get_value())
        if self._is_save:
            n = len(state['value'])
            state['value'] = None
            _tabidlists_save = n * [None]
            i = 0
            for rowlist in self.get_value():
                rowlist_save = []
                for tab, ids in rowlist:
                    rowlist_save.append([tab.get_ident_abs(), ids])
                    # print '    append',[tab.get_ident_abs(), ids]
                _tabidlists_save[i] = rowlist_save
                # print '  ',i,rowlist_save
                i += 1
            state['_tabidlists_save'] = _tabidlists_save

    def init_postload_external(self):
        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        #
        # print 'init_postload_external',self.attrname, len(self._tabidlists_save)
        #obj = self.get_obj()
        #rootobj = obj.get_root()
        # print '  rootobj',rootobj.ident
        #linkobj = rootobj.get_obj_from_ident(ident_abs)
        # print '  linkobj',linkobj.ident
        #self._tab = linkobj

        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        #
        _tabidlists_save = self._tabidlists_save
        #ident_abs = self._ident_value
        # print 'init_postload_external',self.attrname,_tabids_save
        obj = self.get_obj()
        rootobj = obj.get_root()
        # print '  rootobj',rootobj.ident
        tabidlists = np.zeros(len(_tabidlists_save), dtype=self._dtype)

        i = 0
        for rowlist_save in _tabidlists_save:
            rowlist = []
            # print '  rowlist_save',rowlist_save
            for tabident, ids in rowlist_save:
                tab = rootobj.get_obj_from_ident(tabident)
                # print '  ',tab.get_ident_abs(), ids
                rowlist.append([tab, ids])
            tabidlists[i] = rowlist
            i += 1
        # print '  tabidlists', tabidlists
        self.set_value(tabidlists)

    def is_modified(self):
        return False

    def set_modified(self, is_modified):
        pass


class TabIdsArrayConf(ArrayConfMixin, ColConf):

    """
    Column, where each entry contains a tuple with table object and id. 
    The tables are linked, and will not be saved.
    """

    def __init__(self, attrname,  is_index=False, **kwargs):
        self._is_index = is_index
        ArrayConfMixin.__init__(self,  attrname,
                                (None, -1),  # default id
                                dtype=[('ob', object), ('id', np.int)],
                                metatype='tabid',
                                perm='r',
                                **kwargs
                                )

    def get_defaults(self, ids):
        # create a list, should work for all types and dimensions
        # default can be scalar or an array of any dimension
        # print '\n\nget_defaults',self.attrname,ids,self.get_default()
        return np.zeros(len(ids), dtype=self._dtype)

    def _getstate_specific(self, state):
        """
        Called by __getstate__ to add/change specific states,
        before returning states.
        To be overridden.
        """
        if self._is_save:
            n = len(state['value'])
            state['value'] = None
            _tabids_save = n * [None]
            i = 0
            for tab, ids in self.get_value():
                _tabids_save[i] = [tab.get_ident_abs(), ids]
                i += 1
            state['_tabids_save'] = _tabids_save

    def init_postload_external(self):
        # if self._is_child:
        #    # restore normally
        #    AttrConf.init_postload_external(self)
        #    self._tab.init_postload_external()
        # else:

        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        #
        #ident_abs = self._ident_tab
        # print 'reset_linkobj',self.attrname,ident_abs
        #obj = self.get_obj()
        #rootobj = obj.get_root()
        # print '  rootobj',rootobj.ident
        #linkobj = rootobj.get_obj_from_ident(ident_abs)
        # print '  linkobj',linkobj.ident
        #self._tab = linkobj

        # Substitute absolute ident with link object.
        # Called from init_postload_external of attrsman during load_obj
        #
        _tabids_save = self._tabids_save
        #ident_abs = self._ident_value
        # print 'init_postload_external',self.attrname,_tabids_save
        obj = self.get_obj()
        rootobj = obj.get_root()
        # print '  rootobj',rootobj.ident
        tabids = np.zeros(len(self._tabids_save), dtype=self._dtype)

        i = 0
        for tabident, ids in self._tabids_save:
            tab = rootobj.get_obj_from_ident(tabident)
            # print '  ',tab.get_ident_abs(), ids
            tabids[i] = (tab, ids)
            i += 1

        self.set_value(tabids)

    def is_modified(self):
        return False

    def set_modified(self, is_modified):
        pass


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
        self._inds = np.zeros((0,), int)
        self._ids = np.zeros((0,), int)

    def get_inds(self, ids=None):
        if ids != None:
            return self._inds[ids]
        else:
            return self._inds[self._ids]

    def get_ids(self, inds=None):
        if inds != None:
            return self._ids[inds]
        else:
            return self._ids

        return

    def get_ind(self, id):
        return self._inds[id]

    def select_ids(self, mask):
        # print 'select_ids'
        # print '  mask\n=',mask
        # print '  self._ids\n=',self._ids
        # if len(self)>0:
        return np.take(self._ids, np.flatnonzero(mask))
        # else:
        #    return np.zeros(0,int)

    def suggest_id(self, is_zeroid=False):
        """
        Returns a an availlable id.

        Options:
            is_zeroid=True allows id to be zero.

        """
        return self.suggest_ids(1, is_zeroid)[0]

    def suggest_ids(self, n, is_zeroid=False):
        """
        Returns a list of n availlable ids.
        It returns even a list for n=1. 

        Options:
            is_zeroid=True allows id to be zero.
        """
        # TODO: does always return 1 if is_index is True ?????
        # print
        # 'suggest_ids',n,is_zeroid,self._inds,len(self._inds),self._inds.dtype
        ids_unused_orig = np.flatnonzero(np.less(self._inds, 0))

        if not is_zeroid:
            if len(self._inds) == 0:
                ids_unused = np.zeros(0, dtype=np.int32)
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
                    np.flatnonzero(np.greater(ids_unused_orig, 0))]
            zid = 1
        else:
            if len(self._inds) == 0:
                ids_unused = np.zeros(0, dtype=np.int32)
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
            ids = np.arange(zid, n + zid, dtype=np.int32)

        elif n_unused > 0:
            if n_unused >= n:
                ids = ids_unused[:n]
            else:
                # print '  ids_unused',ids_unused
                # print '  from to',n_max+1,n_max+1+n-n_unused
                # print '  arange=',arange(n_max+1,n_max+1+n-n_unused)
                # print '  type(ids_unused)',type(ids_unused)
                # print '  dtype(ids_unused)',ids_unused.dtype
                ids = np.concatenate((ids_unused, np.arange(
                    n_max + 1, n_max + 1 + n - n_unused)))

        else:
            ids = np.arange(n_max + 1, n_max + 1 + n, dtype=np.int32)

        return ids

    def _add_ids(self, ids):
        n = len(ids)
        if n == 0:
            return

        id_max = max(ids)
        id_max_old = len(self._inds) - 1
        n_array_old = len(self)

        ids_existing = np.take(ids, np.flatnonzero(np.less(ids, id_max_old)))
        # print '  ids',ids,'id_max_old',id_max_old,'ids_existing',ids_existing

        # check here if ids are still available
        # if np.sometrue(  np.not_equal( np.take(self._inds, ids_existing), -1)  ):
        #    print 'WARNING in create_ids: some ids already in use',ids_existing
        #    return np.zeros(0,int)

        # extend index map with -1 as necessary
        if id_max > id_max_old:
            # print 'ext',-1*ones(id_max-id_max_old)
            self._inds = np.concatenate(
                (self._inds, -1 * np.ones(id_max - id_max_old, int)))

        # assign n new indexes to new ids
        ind_new = np.arange(n_array_old, n_array_old + n)

        # print 'ind_new',ind_new
        np.put(self._inds, ids, ind_new)

        # print '  concat ids..',self._ids,ids
        self._ids = np.concatenate((self._ids, ids))

    def add_rows(self, n=None, ids=[], **attrs):

        if n != None:
            ids = self.suggest_ids(n)
        elif (len(ids) == 0) & (len(attrs) > 0):
            # get number of rows from any valye vector provided
            ids = self.suggest_ids(len(attrs.values()[0]))
        elif (n == None) & (len(ids) == 0) & (len(attrs) > 0):
            # nothing given really-> do nothing
            return np.zeros((0), np.int)

        else:
            # ids already given , no ids to create
            pass

        # print 'add_rows ids', ids, type(ids)
        self._add_ids(ids)

        for colconfig in self._colconfigs:
            colconfig.add(ids, values=attrs.get(colconfig.attrname, None))
        if self.plugin:
            self.plugin.exec_events_ids(EVTADDITEM, ids)
        return ids

    def set_rows(self, ids, **attrs):

        for colconfig in self._colconfigs:
            colconfig.set(ids, values=attrs.get(colconfig.attrname, None))
        if self.plugin:
            self.plugin.exec_events_ids(EVTSETITEM, ids)

    def add_row(self, _id=None, **attrs):
        if _id == None:
            _id = self.suggest_id()

        self._add_ids([_id])
        #ids = self.add_rows(1, **attrs)
        for colconfig in self._colconfigs:
            # print '
            # add_row',colconfig.attrname,attrs.get(colconfig.attrname, None )
            colconfig.add(_id, values=attrs.get(colconfig.attrname, None))

        if self.plugin:
            self.plugin.exec_events_ids(EVTADDITEM, [id])
        return _id

    def set_row(self, _id, **attrs):
        # if _id == None:
        # print ' set_row ',self.get_ident(),attrs
        for colconfig in self._colconfigs:  # TODO: run through keys!!!!
            # print '  add_row',_id,colconfig.attrname,attrs.get(colconfig.attrname, None )
            # if attrs.has_key(colconfig.attrname):
            #colconfig.set(_id, values = attrs[colconfig.attrname])
            colconfig.set(_id, values=attrs.get(colconfig.attrname, None))

        if self.plugin:
            self.plugin.exec_events_ids(EVTSETITEM, [id])

    def del_row(self, _id):
        # print 'del_row',id
        self.del_rows([_id])

    def del_rows(self, ids):
        # print 'del_rows',ids
        # TODO: this could be done in with array methods

        for _id in ids:
            i = self._inds[_id]

            for colconfig in self._colconfigs:
                # print '  colconfig',colconfig.attrname,i
                # colconfig.delete_ind(i)
                del colconfig[_id]  # this is universal, also for cm.ColConfigs

            # print '    del from id lookup'
            self._ids = np.concatenate((self._ids[:i], self._ids[i + 1:]))

            # print '    free index',id
            if _id == len(self._inds) - 1:
                # id is highest, let's shrink index array by 1
                self._inds = self._inds[:-1]
            else:
                self._inds[_id] = -1

            # get ids of all indexes which are above i
            ids_above = np.flatnonzero(self._inds > i)

            # decrease index from those wich are above the deleted one
            #put(self._inds, ids_above,take(self._inds,ids_above)-1)
            self._inds[ids_above] -= 1

            # print '    self._inds',self._inds

        if self.plugin:
            self.plugin.exec_events_ids(EVTDELITEM, ids)

        # print '  del',ids,' done.'

    def clear_rows(self):
        print 'clear_rows', self.ident

        if self.plugin:
            self.plugin.exec_events_ids(EVTDELITEM, self.get_ids())
        self._ids = []
        self._inds = np.zeros((0,), int)
        self._ids = np.zeros((0,), int)

        for colconfig in self._colconfigs:
            print 'clear_rows', colconfig.attrname, len(colconfig.get_value())
            colconfig.clear()
            print '  done', len(colconfig.get_value())


class ArrayObjman(Arrayman, TableMixin):

    """
    Array Object management manages objects with numeric Python arrays
     based columns. Can also handle list and dict based columns. 
    """

    def __init__(self, ident, **kwargs):
        self._init_objman(ident, **kwargs)

    def _init_objman(self, ident, is_plugin=False, **kwargs):
        BaseObjman._init_objman(self, ident, managertype='table', **kwargs)
        Arrayman.__init__(self, is_plugin=is_plugin)
        self.set_attrsman(self)

    def init_postload_internal(self, parent):
        """
        Called after set state.
        Link internal states.
        """
        TableMixin.init_postload_internal(self, parent)

    def init_postload_external(self):
        """
        Called after set state.
        Link internal states.
        """
        TableMixin.init_postload_external(self)
        self._init_attributes()
        self._init_constants()

    def clear_rows(self):
        if self.plugin:
            self.plugin.exec_events_ids(EVTDELITEM, self.get_ids())
        self._inds = np.zeros((0,), int)
        self._ids = np.zeros((0,), int)
        for colconfig in self.get_attrsman()._colconfigs:
            # print
            # 'ArrayObjman.clear_rows',colconfig.attrname,len(colconfig.get_value())
            colconfig.clear()
            # print '  done',len(colconfig.get_value())

    def clear(self):
        # print 'ArrayObjman.clear',self.ident
        # clear/reset scalars
        for attrconfig in self.get_attrsman().get_configs(structs=STRUCTS_SCALAR):
            attrconfig.clear()
        self.clear_rows()
        self.set_modified()
