"""
Test for callsman
Provides test classes and some test functions for plugin.
"""

from classman import *
from arrayman import *

def on_event_delattr(attrconfig):
    print 'EVENT: Attribute',attrconfig.attrname,'will be deleted!!'                            
    
def on_event_setattr(attrconfig):
    print 'EVENT: Attribute',attrconfig.attrname,'has been set to a new value',attrconfig.format_value()

def on_event_getattr(attrconfig):
    print 'EVENT: Attribute',attrconfig.attrname,'has been retrieved the value',attrconfig.format_value()

def on_event_additem(attrconfig,keys):
    print 'EVENT: Attribute',attrconfig.attrname,':added keys:',keys

def on_event_delitem(attrconfig,keys):
    print 'EVENT: Attribute',attrconfig.attrname,':delete keys:',keys

def on_event_setitem(attrconfig,keys):
    print 'EVENT: Attribute',attrconfig.attrname,':set keys:',keys

def on_event_getitem(attrconfig,keys):
    print 'EVENT: Attribute',attrconfig.attrname,':get keys:',keys



class TestClass(BaseObjman):
        def __init__(self, ident = 'testobj',  parent=None, name = 'Test Object'):
            self._init_objman(ident, parent=parent, name = name)
            self.attrsman = self.set_attrsman(Attrsman(self))
            
            
            
            
            self.access = self.attrsman.add(AttrConf(  'access', ['bus','bike','tram'],
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Access list', 
                                        info = 'List with vehicle classes that have access',
                                        ))
                                        
            self.emissiontype = self.attrsman.add(AttrConf(  'emissiontype', 'Euro 0',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Emission type', 
                                        info = 'Emission type of vehicle',
                                        ))
                                        
            self.x = self.attrsman.add(NumConf(  'x', 0.0,
                                        digits_integer = None, digits_fraction=4,
                                        minval = 0.0, maxval = None,
                                        groupnames = ['state'], 
                                        perm='r', 
                                        is_save = True,
                                        unit = 'm',
                                        metatype = 'length', 
                                        is_plugin = True,
                                        name = 'position', 
                                        info = 'Test object x position',
                                        ))
                                        
            self.is_pos_ok = self.attrsman.add(AttrConf(  'is_pos_ok', False,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Pos OK', 
                                        info = 'True if position is OK',
                                        ))
                                        
            
                                            


class TestClass3(BaseObjman):
        def __init__(self,ident = 'testobj3',  parent=None, name = 'Test Object3'):
            self._init_objman(ident = ident,  parent=parent, name = name)
            self.attrsman = self.set_attrsman(Attrsman(self))
            
            self.child1 = self.attrsman.add(  ObjConf(parent.child1, is_child = False))
            
            self.y = self.attrsman.add( AttrConf(  'y', 0.0,
                                            groupnames = ['state'], 
                                            perm='r', 
                                            is_save = True,
                                            unit = 'm',
                                            metatype = 'length', 
                                            is_plugin = True,
                                            name = 'position', 
                                            info = 'Test object  y position',
                                            ))
                                        
            

class TestClass2(BaseObjman):
        def __init__(self,ident = 'testobj2',  parent=None, name = 'Test Object2'):
            self._init_objman(ident,parent=parent, name = name)
            self.attrsman = self.set_attrsman(Attrsman(self))
            
            self.child1 = self.attrsman.add(  ObjConf( TestClass('child1',self) )
                                           )  
            
            print 'TestClass2.child1', self.child1
            
            self.child3 = self.attrsman.add(  ObjConf( TestClass3('child3',self) )
                                            )          

class TestTabman(BaseObjman):
    def __init__(self,ident = 'test_tabman',  parent=None, name = 'Test Table manage'):
            self._init_objman(ident,parent=parent, name = name)
            tm = Tabman(obj = self)
            self.set_attrsman(tm)
            self.surname = self.attrsman.add_col(ColConf( 'surname', 'xx',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Family name', 
                                        info = 'Name of Family',
                                        ))
                                        
            self.streetname = self.attrsman.add_col(ColConf( 'streetname', 'via della...',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = False,
                                        name = 'Street name', 
                                        info = 'Name of the street',
                                        ))
            #_id = self.attrsman.suggest_id()
            #print '_id =',_id 
            #self.attrman.add(_id)
            
            #print 'self.streetname',self.streetname,type(self.streetname)
            #self.streetname[1]='yyy'
            #print 'self.streetname',self.streetname,type(self.streetname)
            self.attrsman.add_rows(5)
            self.attrsman.streetname[3]='ssss'
            self.attrsman.streetname[[1,2]]=['aa',55]
            print 'test get',self.attrsman.streetname[[1,2]]
            #self.streetname[1]+='zzz' 
            self.attrsman.del_rows([1,3])        
            self.attrsman.del_row(5) 
            #self.attrsman.delete('streetname')                    
                                        
class TestTableObjMan(TableObjman):
    def __init__(self,ident = 'test_tableobjman_simple',  parent=None, name = 'Test Table Object Manager'):
            self._init_objman(ident,parent=parent, name = name)
            
            # ATTENTION!!
            # do not use x = self.add(...) or self.add_col(...)
            # This would overwrite the configuration with the value
            # because  the attribute is the configuration, which is set by Attrman
            # While the add method is returning the value
            self.add(AttrConf(  'x', 0.0,
                                        groupnames = ['state'], 
                                        perm='r', 
                                        is_save = True,
                                        unit = 'm',
                                        metatype = 'length', 
                                        is_plugin = False,
                                        name = 'position', 
                                        info = 'Test object x position',
                                        ))
                                        
            self.add(AttrConf(  'is_pos_ok', False,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Pos OK', 
                                        info = 'True if position is OK',
                                        ))
                                        
            self.add_col(ColConf( 'surname', 'xx',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Family name', 
                                        info = 'Name of Family',
                                        ))
                                        
            self.add_col(ColConf( 'streetname', 'via della...',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Street name', 
                                        info = 'Name of the street',
                                        ))
            #_id = self.attrsman.suggest_id()
            #print '_id =',_id 
            #self.attrman.add(_id)
            
            #print 'self.streetname',self.streetname,type(self.streetname)
            #self.streetname[1]='yyy'
            #print 'self.streetname',self.streetname,type(self.streetname)
            self.add_rows(5)
            self.streetname[3]='ssss'
            self.surname[[1,2,3,4]]=['a','bb','ccc','dddd']
            self.streetname[[1,2]]=['vv',55]
            #print '\n\ntest get',self.streetname[[1,2,3]]
            #self.streetname[1]+='zzz' 
            #self.del_rows([1,3])        
            #self.del_row(5) 
            #self.delete('streetname')  
            #self.delete('is_pos_ok')
            #print 'dir',dir(self)  
            
            

class ZonesTab(ArrayObjman):
    def __init__(self,ident,  parent=None, **kwargs):
            self._init_objman(ident,parent=parent, **kwargs)
            
                                    
            self.add_col(ColConf( 'shapes', [],
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    is_save = True,
                                    name = 'Shape', 
                                    info = 'Shape of zone which is a list of (x,y) coordinates',
                                    ))
                                    
            
                                        
class OdTripsTab(ArrayObjman):
    def __init__(self,ident,  parent, zones, **kwargs):
            self._init_objman(ident, parent=parent, **kwargs)
            
            self.add_col(IdsConf( 'ids_orig', zones, is_child = False,
                                    groupnames = ['state'], 
                                    is_save = True,
                                    name = 'ID Orig', 
                                    info = 'ID of traffic assignment zone of origin of trip.',
                                    ))


            self.add_col(IdsConf( 'ids_dest', zones, is_child = False,
                                    groupnames = ['state'], 
                                    is_save = True,
                                    name = 'ID Dest', 
                                    info = 'ID of traffic assignment zone of destination of trip.',
                                    ))
                                    
            self.add_col(ColConf( 'tripnumbers', 0,
                                    groupnames = ['state'], 
                                    perm = 'rw', 
                                    is_save = True,
                                    name = 'Trip number', 
                                    info = 'Number of trips from zone with ID Orig to zone with ID Dest.',
                                    ))
                                    
class OdModesTab(ArrayObjman):
    def __init__(self,ident,  parent=None, **kwargs):
            self._init_objman(ident,parent=parent, **kwargs)
            
                                                            
            self.add_col(ObjsConf( 'odtrips', 
                                    groupnames = ['state'], 
                                    is_save = True,
                                    name = 'OD matrix', 
                                    info = 'Matrix with trips from origin to destintion',
                                    ))
                                        
class OdIntervalsTab(ArrayObjman):
    def __init__(self,ident,  parent=None, **kwargs):
            self._init_objman(ident,parent=parent, **kwargs)
            
            self.add_col(ColConf( 't_start', 0.0,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    is_save = True,
                                    name = 'Start time', 
                                    unit = 's',
                                    info = 'Start time of interval',
                                    ))
            
            self.add_col(ColConf( 't_end', 3600.0,
                                    groupnames = ['state'], 
                                    perm='rw', 
                                    is_save = True,
                                    name = 'End time', 
                                    unit = 's',
                                    info = 'End time of interval',
                                    ))
                                    
            self.add_col(ObjsConf( 'odmodes', 
                                    groupnames = ['state'], 
                                    is_save = True,
                                    name = 'Modes', 
                                    info = 'Transport mode',
                                    )) 
                                    
###########################################################################
## Instance creation
                                    
demand = BaseObjman('demand')

zones = ZonesTab('zones', parent = demand )
demand.zones = demand.get_attrsman().add(  ObjConf( zones) )
shapes = [  [(0.0,10.0),(10.0,10.0),(10.0,0.0)],
            [(10.0,20.0),(20.0,20.0),(20.0,10.0)],
            [(20.0,30.0),(30.0,20.0),(30.0,20.0)],
            ]
zones.add_rows(3, shapes=shapes)

odintervals = OdIntervalsTab('odintervals', parent = demand, info = 'OD data for different time intervals')
demand.odintervals = demand.get_attrsman().add(  ObjConf(odintervals))
odintervals.add_rows(2, t_start=[0,3600], t_end=[3600, 7200])
for id_odmodes in odintervals.get_ids():
    odmodes = OdModesTab( (odintervals.odmodes.attrname, id_odmodes), parent = odintervals)
    odintervals.odmodes[id_odmodes] = odmodes
    
    odmodes.add_rows(2)
    for id_odtrips in odmodes.get_ids():
        odtrips = OdTripsTab( (odmodes.odtrips.attrname, id_odtrips) , odmodes, zones )
        odtrips.add_rows(3, ids_orig=[3,2,1],ids_dest=[3,3,3], tripnumbers = [10,200,555])
        odmodes.odtrips[id_odtrips] = odtrips
        
#demand.attrsman.print_attrs() 
#odintervals.print_attrs()         