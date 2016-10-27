"""
Test for callsman
Provides test classes and some test functions for plugin.
"""

from classman import *

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
                                        
            self.x = self.attrsman.add(AttrConf(  'x', 0.0,
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
            self.child1 = self.attrsman.add(  LinkConf(parent.child1))
            self.y = self.attrsman.add(AttrConf(  'y', 0.0,
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
            
            self.child1 = self.attrsman.add(  ChildConf( TestClass('child1',self) )
                                           )  
            
            print 'TestClass2.child1', self.child1
            
            self.child3 = self.attrsman.add(  ChildConf( TestClass3('child3',self) )
                                            )          

class TestTabman(BaseObjman):
    def __init__(self,ident = 'test_tabman',  parent=None, name = 'Test Tabman'):
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
    def __init__(self,ident = 'test_tableobjman',  parent=None, name = 'Test Table Object Manager'):
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
            self.add_rows(5)
            self.streetname[3]='ssss'
            self.surname[[1,2,3,4]]=['a','bb','ccc','dddd']
            self.streetname[[1,2]]=['vv',55]
            print '\n\ntest get',self.streetname[[1,2,3]]
            #self.streetname[1]+='zzz' 
            self.del_rows([1,3])        
            self.del_row(5) 
            self.delete('streetname')  
            self.delete('is_pos_ok')
            #print 'dir',dir(self)          