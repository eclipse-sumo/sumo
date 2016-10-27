#from classman import *
 
from test_classman_classes import *
from arrayman import *
# TODOD: create a test object with all tests
is_all = 0

if 1|is_all:
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
    demand.zones = demand.attrsman.add(  ObjConf( zones) )
    shapes = [  [(0.0,10.0),(10.0,10.0),(10.0,0.0)],
                [(10.0,20.0),(20.0,20.0),(20.0,10.0)],
                [(20.0,30.0),(30.0,20.0),(30.0,20.0)],
                ]
    zones.add_rows(3, shapes=shapes)
    
    odintervals = OdIntervalsTab('odintervals', parent = demand, info = 'OD data for different time intervals')
    demand.odintervals = demand.attrsman.add(  ObjConf(odintervals))
    odintervals.add_rows(2, t_start=[0,3601], t_end=[3600, 7200])
    for id_odmodes in odintervals.get_ids():
        odmodes = OdModesTab( (odintervals.odmodes.attrname, id_odmodes), parent = odintervals)
        odintervals.odmodes[id_odmodes] = odmodes
        
        odmodes.add_rows(2)
        for id_odtrips in odmodes.get_ids():
            odtrips = OdTripsTab( (odmodes.odtrips.attrname, id_odtrips) , odmodes, zones )
            odtrips.add_rows(3, ids_orig=[3,2,1],ids_dest=[3,3,3], tripnumbers = [10,200,555])
            odmodes.odtrips[id_odtrips] = odtrips
    
    ## print        
    demand.attrsman.print_attrs() 
    odintervals.print_attrs() 
    for id_odmodes in odintervals.get_ids():  
        print '\nMODE:'
        odintervals.odmodes[id_odmodes].print_attrs()
        print '\nTRIPS:'
        for id_odtrips in odmodes.get_ids():
            odmodes.odtrips[id_odtrips].print_attrs()
    ## save/load  
    save_obj(demand,'test_demand_array.obj')
    del demand
    print '\nreload'+60*'.'
    demand = load_obj('test_demand_array.obj')
    
    ## print 
    demand.attrsman.print_attrs() 
    odintervals.print_attrs() 
    for id_odmodes in odintervals.get_ids():  
        print '\nMODE:'
        odintervals.odmodes[id_odmodes].print_attrs()
        print '\nTRIPS:'
        for id_odtrips in odmodes.get_ids():
            odmodes.odtrips[id_odtrips].print_attrs()
            
if 0|is_all:
    class ZonesTab(TableObjman):
        def __init__(self,ident,  parent=None, **kwargs):
                self._init_objman(ident,parent=parent, **kwargs)
                
                                        
                self.add_col(ColConf( 'shapes', [],
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Shape', 
                                        info = 'Shape of zone which is a list of (x,y) coordinates',
                                        ))
                                        
                
                                            
    class OdTripsTab(TableObjman):
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
                                        
    class OdModesTab(TableObjman):
        def __init__(self,ident,  parent=None, **kwargs):
                self._init_objman(ident,parent=parent, **kwargs)
                
                                                                
                self.add_col(ObjsConf( 'odtrips', 
                                        groupnames = ['state'], 
                                        is_save = True,
                                        name = 'OD matrix', 
                                        info = 'Matrix with trips from origin to destintion',
                                        ))
                                        
                                        
    class OdIntervalsTab(TableObjman):
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
    demand.zones = demand.attrsman.add(  ObjConf( zones) )
    shapes = [  [(0.0,10.0),(10.0,10.0),(10.0,0.0)],
                [(10.0,20.0),(20.0,20.0),(20.0,10.0)],
                [(20.0,30.0),(30.0,20.0),(30.0,20.0)],
                ]
    zones.add_rows(3, shapes=shapes)
    
    odintervals = OdIntervalsTab('odintervals', parent = demand, info = 'OD data for different time intervals')
    demand.odintervals = demand.attrsman.add(  ObjConf(odintervals, is_child = True))
    odintervals.add_rows(2, t_start=[0,3600], t_end=[3600, 7200])
    for id_odmodes in odintervals.get_ids():
        odmodes = OdModesTab( (odintervals.odmodes.attrname, id_odmodes), parent = odintervals)
        odintervals.odmodes[id_odmodes] = odmodes
        
        odmodes.add_rows(2)
        for id_odtrips in odmodes.get_ids():
            odtrips = OdTripsTab( (odmodes.odtrips.attrname, id_odtrips) , odmodes, zones )
            odtrips.add_rows(3, ids_orig=[3,2,1],ids_dest=[3,3,3], tripnumbers = [10,200,555])
            odmodes.odtrips[id_odtrips] = odtrips
    
    ## print        
    demand.attrsman.print_attrs() 
    odintervals.print_attrs() 
    for id_odmodes in odintervals.get_ids():  
        print '\nMODE:'
        odintervals.odmodes[id_odmodes].print_attrs()
        print '\nTRIPS:'
        for id_odtrips in odmodes.get_ids():
            odmodes.odtrips[id_odtrips].print_attrs()
    ## save/load  
    save_obj(demand,'test_demand.obj')
    del demand
    print '\nreload'+60*'.'
    demand = load_obj('test_demand.obj')
    
    ## print 
    demand.attrsman.print_attrs() 
    odintervals.print_attrs() 
    for id_odmodes in odintervals.get_ids():  
        print '\nMODE:'
        odintervals.odmodes[id_odmodes].print_attrs()
        print '\nTRIPS:'
        for id_odtrips in odmodes.get_ids():
            odmodes.odtrips[id_odtrips].print_attrs()
                    
if 0|is_all:
    net = BaseObjman('net')
    
    # TODO: could be put in 2 statements
    edges = TableObjman('edges', parent = net, info = 'Network edges')
    nodes = TableObjman('nodes', parent = net, info = 'Network nodes')
    
    net.edges = net.attrsman.add(  ObjConf(edges, is_child = True))
    net.nodes = net.attrsman.add(  ObjConf(nodes, is_child = True))
    
    
    

    net.edges.add_col(IdsConf( 'ids_node_from', net.nodes, is_child = False,
                                        groupnames = ['state'], 
                                        is_save = True,
                                        name = 'ID from nodes', 
                                        info = 'ID from nodes',
                                        ))
    
    
    net.edges.add_col(IdsConf( 'ids_node_to', net.nodes, is_child = False,
                                        groupnames = ['state'], 
                                        is_save = True,
                                        name = 'ID to nodes', 
                                        info = 'ID to nodes',
                                        ))
                                        
                                                                            
    net.nodes.add_col(ColConf( 'coords', (0.0,0.0),
                                groupnames = ['state'], 
                                perm='rw', 
                                is_save = True,
                                name = 'Coords', 
                                info = 'Coordinates',
                                ))
    
    
    net.nodes.add_rows(  4,
                        #            1         2          3          4
                        coords = [(0.0,0.0),(1.0,0.0),(1.0,1.0),(0.0,1.0)],
                        )
    
    net.edges.add_rows(2)
    net.edges.ids_node_from[[1,2]]=[1,4]
    net.edges.ids_node_to[[1,2]]=[3,2]                           
    net.attrsman.print_attrs()
    net.edges.print_attrs()
    net.nodes.print_attrs()
    save_obj(net,'test_net.obj')
    del net
    print '\nreload'+60*'.'
    net_new = load_obj('test_net.obj')
    net_new.attrsman.print_attrs()
    net_new.edges.print_attrs()
    net_new.nodes.print_attrs()
    

if 0|is_all:
    tab1 = TableObjman('simple_table')
    
    

    tab1.add_col(ColConf( 'surname', 'xx',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Family name', 
                                        info = 'Name of Family',
                                        ))
                                        
    tab1.add_col(ColConf( 'streetname', 'via della...',
                                groupnames = ['state'], 
                                perm='rw', 
                                is_save = False,
                                name = 'Street name', 
                                info = 'Name of the street',
                                ))
    tab1.add_rows(  4,
                    surname = ['walt','greg','bob','duck'],
                    streetname=['a','bb','ccc','dddd'],
                    )
                    
    
    
    print 'direct access: tab1.surname.value',tab1.surname.value
    tab1.print_attrs()
    
    save_obj(tab1,'test_tab.obj')
    del tab1
    print '\nreload'+60*'.'
    tab1_new = load_obj('test_tab.obj')
    print 'direct access: tab1_new.surname.value',tab1_new.surname.value
    
if 0|is_all:
    tab1 = TableObjman('tab1')
    print '\ntab1.ident',tab1.ident
    
    tab2 = TableObjman('tab2', parent = tab1)
    print '\ntab2.ident',tab2.ident
    
    
    # TODO: seperate attrname from linked obj ident because restrictive and makes problems with multiple tab destinations
    # this should be possible ...following the path of attrnames of absident
    # -
    tab1.add_col(IdsConf( tab2) )
    
    tab2.add_col(IdsConf( tab1, is_child = False)  )
    
    tab2.add_col(ColConf( 'surname', 'xx',
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        is_save = True,
                                        name = 'Family name', 
                                        info = 'Name of Family',
                                        ))
                                        
    tab2.add_col(ColConf( 'streetname', 'via della...',
                                groupnames = ['state'], 
                                perm='rw', 
                                is_save = False,
                                name = 'Street name', 
                                info = 'Name of the street',
                                ))
    tab2.add_rows(  4,
                    surname = ['walt','greg','bob','duck'],
                    streetname=['a','bb','ccc','dddd'],
                    tab1 = [2,1,3,1,],
                    )
                    
    
                    
    tab2.print_attrs()
    
    tab1.add_rows(  3,
                    tab2 = [3,1,2],
                    )
    
    tab1.print_attrs()
    
    save_obj(tab1,'test_tab.obj')
    del tab1
    print '\nreload'+60*'.'
    tab1_new = load_obj('test_tab.obj')
    
    tab1_new.print_attrs()
    tab2_new = tab1_new.tab2.get_valueobj()
    tab2_new.print_attrs()
    print tab2_new.get_ident_abs()
    
if 0|is_all:
    obj2 = TestClass2()
    
    
    obj2.child1.attrsman.x.set(1.0)
    
    #obj3 = TestClass3(ident = 'testobj3',  parent=obj2, name = 'Test Object3')
    obj2.attrsman.print_attrs()
    obj2.child1.attrsman.print_attrs()
    obj2.child3.attrsman.print_attrs()
    save_obj(obj2,'test_obj2.obj')
    del obj2
    print '\nreload'+60*'.'
    obj2_new = load_obj('test_obj2.obj')
    obj2_new.attrsman.print_attrs()
    
    obj2_new.child1.attrsman.print_attrs()
    obj2_new.child3.attrsman.print_attrs()
    #sys.exit()

if False|is_all:#False:#True:
    obj = TestClass()
    print 'obj.ident',obj.ident
    
    print 'This is the value of the attribute: obj.x=',obj.x
    #print 'This is the configuration instance of the attribute x',obj.attrsman.x
    obj.attrsman.print_attrs()
    #obj.attrsman.x.plugin.add_event(EVTSET,on_event_setattr)
    #obj.attrsman.x.add_event(EVTGET,on_event_getattr)
    #print 'obj.attrsman.get_groups()',obj.attrsman.get_groups()
    #print 'obj.tab.get_groups()',obj.tab.get_groups()
    
    #print 'Test func...',obj.attrsman.testfunc.get()
    #obj.attrsman.testfunc.add_event(EVTGET,on_event_getattr)
    #obj.attrsman.testfunc.get()
    print 'obj.attrsman.x.get()',obj.attrsman.x.get(),'is_modified',obj.is_modified()
    obj.attrsman.x.set(1.0)
    print 'obj.attrsman.x.get()',obj.attrsman.x.get(),'is_modified',obj.is_modified()
    
    #obj.attrsman.delete('x')
    obj.attrsman.print_attrs()
    save_obj(obj,'test_obj.obj')
    del obj
    print '\nreload'+60*'.'
    obj_new = load_obj('test_obj.obj')
    obj_new.attrsman.print_attrs()
    #print 'obj.attrsman.x.get_formatted()=',obj.attrsman.x.get_formatted()
    #print 'obj.x',obj.x
    
if False|is_all:#False:#True:  
    obj = TestTabman()
    
    print 'obj.ident',obj.ident
    
    obj.attrsman.print_attrs()
    save_obj(obj,'test_obj.obj')
    del obj
    print '\nreload'+60*'.'
    obj_new = load_obj('test_obj.obj')
    obj_new.attrsman.print_attrs()
    #streetname
    #print 'This is the value of the attribute: obj.streetname=',obj.streetname
    #print 'This is the configuration instance of the attribute x',obj.attrsman.x
    #

if 0|is_all:#False:#True:  
    obj = TestTableObjMan()
    
    print 'obj.ident',obj.ident
    
    obj.x.set(1.0/3)
    #obj.is_pos_ok.set(True)
    
    obj.print_attrs()
    save_obj(obj,'test_obj.obj')
    del obj
    print '\nreload'+60*'.'
    obj_new = load_obj('test_obj.obj')
    obj_new.x.set(2.0/3)
    obj_new.print_attrs()
    
    #streetname
    #print 'This is the value of the attribute: obj.streetname=',obj.streetname
    #print 'This is the configuration instance of the attribute x',obj.attrsman.x
    #   