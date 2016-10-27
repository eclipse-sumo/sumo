
import os, sys, string, types
from xml.sax import saxutils, parse, handler#, make_parser
from collections import OrderedDict
 
from coremodules.modules_common import *
import numpy as np            
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlmanager as xm

from agilepy.lib_base.processes import Process,CmlMixin,ff,call,P
from coremodules.network.network import SumoIdsConf


class Tripresults(am.ArrayObjman):
    def __init__(self, parent, trips, edges, is_add_default=True, **kwargs):
        
        self._init_objman(  ident='tripresults', 
                            parent = parent, # main results object
                            name = 'Trip results', 
                            info = 'Table with simulation results for each trip made.', 
                            **kwargs)
        
        self.add_col(am.IdsArrayConf( 'ids_trip', trips, 
                                            groupnames = ['state'], 
                                            is_index = True,
                                            name = 'ID trip', 
                                            info = 'ID of trip.',
                                            ))
        attrinfos = OrderedDict([\
                ('duration', {'name':'Duration', 'xmltag':'duration',    'unit':'s',     'default':0, 'info':'Trip duration','groups':['tripinfo']}),
                ('depart',   {'name':'Dep. time', 'xmltag':'depart',   'unit':'s',     'default':0, 'info':'Departure time','groups':['tripinfo']}),
                ('arrival',   {'name':'Arr. time', 'xmltag':'arrival',   'unit':'s',    'default':0, 'info':'Departure time','groups':['tripinfo']}),
                ('departPos',   {'name':'depart pos', 'xmltag':'departPos',   'unit':'m',    'default':0.0, 'info':'depart position','groups':['tripinfo']}),
                ('arrivalPos',   {'name':'arrival pos','xmltag':'arrivalPos',    'unit':'m',    'default':0.0, 'info':'arrival position','groups':['tripinfo']}),
                ('routeLength',   {'name':'Length','xmltag':'routeLength',    'unit':'m',    'default':0.0, 'info':'Route length','groups':['tripinfo']}),
                ('waitSteps',   {'name':'wait steps', 'xmltag':'waitSteps',   'unit':None,    'default':0, 'info':'Time steps, the vehicle has been waiting during its trip','groups':['tripinfo']}),
                ('rerouteNo',   {'name':'reroute No', 'xmltag':'rerouteNo',   'unit':None,    'default':0, 'info':'Number of re-routes','groups':['tripinfo']}),
                ])
                
        
        for attrname, kwargs in attrinfos.iteritems():
            self.add_resultattr(attrname, **kwargs)
        
        # this is special for route info
        self.add_col(am.IdlistsArrayConf( 'ids_edges', edges,
                                            name = 'Edge IDs', 
                                            groups=['routeinfo'],
                                            info = 'List of edge IDs constituting the actually taken route.', 
                                            xmltag = 'edges',  
                                            ))  
                                                     
    def add_resultattr(self, attrname, **kwargs):
        
        # default cannot be kwarg
        default =  kwargs['default']
        del kwargs['default']
        
        self.add_col(am.ArrayConf(  attrname, default,
                                        groupnames = ['results'], 
                                        **kwargs
                                        ))
    
    def import_routesdata(self, filepath):
        # TODO
        pass
    
    def import_tripdata(self, filepath):
        #print 'import_edgedata',filepath
        attrnames_data = ['depart','arrival','duration','departPos','arrivalPos','routeLength','waitSteps','rerouteNo']
        self.import_sumoxml(filepath,attrnames_data)
        
    def import_sumoxml(self,filepath,
                        attrnames_data=[], 
                        attrnames_averaged=[], 
                        element = 'tripinfo'):    
        
        ids_raw, results, interval = read_interval(\
                                            filepath, 
                                            element, 
                                            attrnames=attrnames_data,
                                            attrnames_averaged=attrnames_averaged
                                            )
       
            
        # this procedure is necessary to create new result ids only
        # for trips that are not yet in the database
        n=len(ids_raw)
        #print '  n',n
        ind_range = np.arange(n, dtype=np.int32)
        ids = np.zeros(n, dtype=np.int32)
        for i in ind_range:
            id_trip = int(ids_raw[i])
            if self.ids_trip.has_index(id_trip):
                ids[i] =  self.ids_trip.get_id_from_index(id_trip)
            else:
                ids[i] =  self.add_row(ids_trip = id_trip)
                   
        
        for attrname in attrnames_data+attrnames_averaged: 
            default = self.get_config(attrname).get_default()
            if type(default) in (types.IntType, types.LongType):
                conversion = 'i'#int
                values_attr = np.zeros(n,int)
            elif type(default) in (types.FloatType,types.ComplexType):
                conversion = 'f'#float
                values_attr = np.zeros(n,float)
            else:
                conversion = 's'#str
                values_attr = np.zeros(n,obj)
            
            # this is a tricky way to read the data stored in
            # dictionarie into array tructures as used in results
            # problem is that not all dictionaries have all ids    
            for i in ind_range:
                val = results[attrname].get(ids_raw[i],default)
                
                if conversion=='i':
                    val=int(val)
                else:
                    values_attr[i]=val
                #print '   attrname',attrname,conversion,val,type(val)
                values_attr[i]=val

            #print '  attrname',attrname
            #print '  ids',type(ids),ids
            #print '  values_attr',type(values_attr),values_attr
            getattr(self,attrname).set(ids, values_attr)
            
                                        
class Edgeresults(am.ArrayObjman):
    def __init__(self, parent, edges, is_add_default=True, **kwargs):
        
        self._init_objman(  ident='edgeresults', 
                            parent = parent, # main results object
                            name = 'Edge results', 
                            info = 'Table with simulation results for each network edge.', 
                            #xmltag = ('vtypes','vtype','ids_sumo'),
                            **kwargs)
        
        self.add_col(am.IdsArrayConf( 'ids_edge', edges, 
                                            groupnames = ['state'], 
                                            is_index = True,
                                            name = 'ID edge', 
                                            info = 'ID of edge.',
                                            ))
        
        
        
        attrinfos = OrderedDict([\
                ('entered',  {'name':'Entered',      'unit':None,    'default':0, 'info':'Entered number of vehicles','xmltag':'entered','groups':['edgedata']}),
                ('left',     {'name':'Left',         'unit':None,    'default':0, 'info':'Left number of vehicles','xmltag':'left','groups':['edgedata']}),
                ('arrived',  {'name':'Arrived',      'unit':None,    'default':0, 'info':'Arrived number of vehicles','xmltag':'arrived','groups':['edgedata']}),
                ('departed', {'name':'Departed',     'unit':None,    'default':0, 'info':'Departed number of vehicles','xmltag':'departed','groups':['edgedata']}),
                ('traveltime',{'name':'Av. times',   'unit':'s',     'default':0.0, 'info':'Av. Travel times','xmltag':'traveltime','groups':['edgedata']}),
                ('density',  {'name':'Av. Densities','unit':'veh/km','default':0.0, 'info':'Av. Density in vehicles of vehicles on this Edge/Lane','xmltag':'density','groups':['edgedata']}),
                ('waitingTime',{'name':'Av. waits',  'unit':'s',     'default':0.0, 'info':'Av. Waiting times of vehicles on this Edge/Lane','xmltag':'waitingTime','groups':['edgedata']}),
                ('speed',    {'name':'Av. speeds',   'unit':'m/s',   'default':0.0, 'info':'Av. velocity of vehicles on this Edge/Lane','xmltag':'speed','groups':['edgedata']}),
                ('fuel_abs', {'name':'Abs. Fuel',    'unit':'ml',    'default':0.0, 'info':'Absolute fuel consumption of vehicles on this Edge/Lane','xmltag':'fuel_abs','groups':['edgeemissions']}),
                ('CO_abs',   {'name':'Abs. CO',      'unit':'mg',    'default':0.0, 'info':'Absolute CO emission of vehicles on this Edge/Lane','xmltag':'CO_abs','groups':['edgeemissions']}),
                ('CO2_abs',  {'name':'Abs. CO2',     'unit':'mg',    'default':0.0, 'info':'Absolute CO2 emission of vehicles on this Edge/Lane','xmltag':'CO2_abs','groups':['edgeemissions']}),
                ('NOx_abs',  {'name':'Abs. NOx',     'unit':'mg',    'default':0.0, 'info':'Absolute NOx emission of vehicles on this Edge/Lane','xmltag':'NOx_abs','groups':['edgeemissions']}),
                ('PMx_abs',  {'name':'Abs. PMx',     'unit':'mg',    'default':0.0, 'info':'Absolute PMx  emission (Particle matter, all sizes) of vehicles on this Edge/Lane','xmltag':'PMx_abs','groups':['edgeemissions']}),
                ('fuel_normed',{'name':'Fuel',       'unit':'l/km/h','default':0.0, 'info':'Absolute fuel consumption of vehicles on this Edge/Lane','xmltag':'fuel_normed','groups':['edgeemissions'], 'is_average' : True}),
                ('CO_normed',{'name':'CO',           'unit':'g/km/h','default':0.0, 'info':'Normalized CO emission of vehicles on this Edge/Lane','xmltag':'CO_normed','groups':['edgeemissions'], 'is_average' : True}),
                ('CO2_normed',{'name':'CO2',         'unit':'g/km/h','default':0.0, 'info':'Normalized CO2 emission of vehicles on this Edge/Lane','xmltag':'CO2_normed','groups':['edgeemissions'], 'is_average' : True}),
                ('NOx_normed',{'name':'NOx',         'unit':'g/km/h','default':0.0, 'info':'Normalized NOx emission of vehicles on this Edge/Lane','xmltag':'NOx_normed','groups':['edgeemissions'], 'is_average' : True}),
                ('PMx_normed',{'name':'PMx',         'unit':'g/km/h','default':0.0, 'info':'Normalized PMx emission of vehicles on this Edge/Lane','xmltag':'PMx_normed','groups':['edgeemissions'], 'is_average' : True}),
                ('fuel_perVeh',{'name':'Fuel',       'unit':'l/veh','default':0.0, 'info':'Absolute fuel consumption of vehicles on this Edge/Lane','xmltag':'fuel_perVeh','groups':['edgeemissions'], 'is_average' : True}),
                ('CO_perVeh',{'name':'CO',           'unit':'g/veh','default':0.0, 'info':'CO emission per vehicle on this Edge/Lane','xmltag':'','groups':['edgeemissions'], 'is_average' : True}),
                ('CO2_perVeh',{'name':'CO2',         'unit':'g/veh','default':0.0, 'info':'CO2 emission per vehicle on this Edge/Lane','xmltag':'CO2_perVeh','groups':['edgeemissions'], 'is_average' : True}),
                ('NOx_perVeh',{'name':'NOx',         'unit':'g/veh','default':0.0, 'info':'NOx emission per vehicle on this Edge/Lane','xmltag':'NOx_perVeh','groups':['edgeemissions'], 'is_average' : True}),
                ('PMx_perVeh',{'name':'PMx',         'unit':'g/veh','default':0.0, 'info':'PMx emission per vehicle on this Edge/Lane','xmltag':'PMx_perVeh','groups':['edgeemissions'], 'is_average' : True}),
                ('noise',    {'name':'Noise',         'unit':'dB',   'default':0.0, 'info':'Noise of vehicles on this Edge/Lane','xmltag':'noise','groups':['edgenoise'], 'is_average' : True}),
                ])
                
        
        for attrname, kwargs in attrinfos.iteritems():
            self.add_resultattr(attrname, **kwargs)
            
                                                     
    def add_resultattr(self, attrname, **kwargs):
        
        # default cannot be kwarg
        default =  kwargs['default']
        del kwargs['default']
        
        self.add_col(am.ArrayConf(  attrname, default,
                                        groupnames = ['results'], 
                                        **kwargs
                                        ))
                                        
    def import_edgedata(self, filepath):
        #print 'import_edgedata',filepath
        attrnames_data = ['entered','left','arrived','departed']
        attrnames_averaged = ['traveltime','density','waitingTime','speed',]
        self.import_sumoxml(filepath,attrnames_data,attrnames_averaged)
    
    def import_edgenoise(self, filepath):
        #print 'import_edgedata',filepath
        self.import_sumoxml(filepath,[],['noise'])
    
    
    def import_edgeemissions(self, filepath):
        #print 'import_edgedata',filepath
        attrnames_data = ['fuel_abs','CO_abs','CO2_abs','NOx_abs','PMx_abs']
        attrnames_averaged = ['fuel_normed','CO_normed','CO2_normed',]
        self.import_sumoxml(filepath,attrnames_data,attrnames_averaged)
            
    def import_sumoxml(self,filepath,attrnames_data=[],attrnames_averaged=[]):    
        element = 'edge'
        #id_type = 'edge',
        #reader = 'interval',
        ids_sumo, results, interval = read_interval(\
                                            filepath, 
                                            element, 
                                            attrnames=attrnames_data,
                                            attrnames_averaged=attrnames_averaged
                                            )
        #print '  ids_sumo',ids_sumo
        #print '  results.keys()',results.keys()
        #print '  results',results
        # create ids for all colums
        #if fileinfo['id_type']=='edge':
            
        # this procedure is necessary to create new result ids only
        # for edges that are not yet in the database
        ids_sumoedge = self.ids_edge.get_linktab().ids_sumo
        n=len(ids_sumo)
        #print '  n',n
        ind_range = np.arange(n, dtype=np.int32)
        ids = np.zeros(n, dtype=np.int32)
        for i in ind_range:
            id_edge = ids_sumoedge.get_id_from_index(ids_sumo[i])
            if self.ids_edge.has_index(id_edge):
                ids[i] =  self.ids_edge.get_id_from_index(id_edge)
            else:
                ids[i] =  self.add_row(ids_edge = id_edge)
                   
        #ids = self.add_row()# here the ids_sumo can be strings too
        #elif fileinfo['id_type']=='trip':
        #    ids = self.tripresults.add_rows_keyrecycle(keys = ids_sumo)#
        #print '  ids=',ids    
        
        
        for attrname in attrnames_data+attrnames_averaged: 
            
            
            default = self.get_config(attrname).get_default()
            if type(default) in (types.IntType, types.LongType):
                conversion = 'i'#int
                values_attr = np.zeros(n,int)
            elif type(default) in (types.FloatType,types.ComplexType):
                conversion = 'f'#float
                values_attr = np.zeros(n,float)
            else:
                conversion = 's'#str
                values_attr = np.zeros(n,obj)
            
            # this is a tricky way to read the data stored in
            # dictionarie into array tructures as used in results
            # problem is that not all dictionaries have all ids    
            for i in ind_range:
                val = results[attrname].get(ids_sumo[i],default)
                
                if conversion=='i':
                    val=int(val)
                else:
                    values_attr[i]=val
                #print '   attrname',attrname,conversion,val,type(val)
                values_attr[i]=val

            #print '  attrname',attrname
            #print '  ids',type(ids),ids
            #print '  values_attr',type(values_attr),values_attr
            getattr(self,attrname).set(ids, values_attr)

            
                                        
class Simresults(cm.BaseObjman):
        def __init__(self, ident= 'simresults', scenario=None, 
                        name = 'Simulation results', 
                        info ='Results of SUMO simulation run.', 
                        outfile_prefix = 'out',
                        **kwargs):
                            
            #print 'Network.__init__',name,kwargs
            ident_scenario = scenario.get_ident()
            rootdirpath = scenario.get_workdirpath()
        
            self._init_objman(ident= ident, parent=scenario, name = name, 
                                info = info, **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            
            self.edgeresults = attrsman.add(     cm.ObjConf( Edgeresults(self, scenario.net.edges) ) )
            self.tripresults = attrsman.add(     cm.ObjConf( Tripresults(self, scenario.demand.trips, scenario.net.edges) ) )
                                           
        def get_scenario(self):
            return self.parent
        

 
class IntervalAvReader(handler.ContentHandler):
    """
    Reads edge or lane based intervals
    and returns time averaged values for each attribute name.
       
    """

    def __init__(self, element, attrnames,attrnames_averaged):
        """
        element is "lane" or "edge" or "tripinfo"
        attrnames is a list of attribute names to read.
        """
        self._element = element
        self._attrnames = attrnames
        self._attrnames_averaged = attrnames_averaged
        self._time_begin = None
        self._time_end = None
        self._values= {}
        self._ids = []
        #self._n_values= {}
        
        self.n_test = 0
        self.n_test2 = 0
        # TODO: if we knew here all ids then we 
        # could create a numeric array per attribute
        # idea: pass ids as input arg
        for  attrname in attrnames+attrnames_averaged:
            self._values[attrname]={}
            #self._n_values= {}
        
    def startElement(self, name, attrs):
        if attrs.has_key('id'):
            print '  parse',name,self._element,name == self._element, attrs['id']
        
        if name == 'interval':
            self._time_inter = int(float(attrs['end']))-int(float(attrs['begin']))
            # here we just take the start and end time ofthe whole
            # measurement period
            if self._time_begin==None:# take very first time only
                self._time_begin = int(float(attrs['begin']))
            self._time_end = int(float(attrs['end']))
            
            
        if name == self._element:
            id = attrs['id']
            #print '--'
            if id not in self._ids:
                self._ids.append(id)
            
            # no arrival data availlable if trip has not been finished!!
            for attrname in self._attrnames:
                
                
                
                if attrs.has_key(attrname):
                    #print '  attrname',attrname,attrs.has_key(attrname),'*'+attrs[attrname]+'*'
                    a = attrs[attrname]
                    if a.strip()!='':
                        if self._values[attrname].has_key(id):
                            self._values[attrname][id] += float(a)
                        else:
                            self._values[attrname][id] = float(a)
                        
                    #if (id in ('1/0to1/1','1/0to2/0')) & (attrname == 'entered'):
                    #    self.n_test+=int(attrs[attrname])
                    #    print '  -read ',id,attrname,attrs[attrname],self.n_test,self._values[attrname][id]
                    #    
                    #if (id in ('0/0to1/0')) & (attrname == 'left'):
                    #    self.n_test2+=int(attrs[attrname])
                    #    print '  +read ',id,attrname,attrs[attrname],self.n_test2,self._values[attrname][id]   
                        
            for attrname in self._attrnames_averaged:
                if attrs.has_key(attrname):
                    a = attrs[attrname]
                    if a.strip()!='':
                        if self._values[attrname].has_key(id):
                            self._values[attrname][id] += float(a)/self._time_inter
                            #self._n_values[attrname][id] += 1
                        else:
                            self._values[attrname][id] = float(a)/self._time_inter
                            #self._n_values[attrname][id] = 1

    def get_data(self):
        return self._values
    
    def get_interval(self):
        return (self._time_begin,self._time_end)
        
    def get_ids(self):
        return self._ids
           
def read_interval(filepath, element, attrnames=[],attrnames_averaged=[]):
    reader = IntervalAvReader(element, attrnames, attrnames_averaged)
    #parser = make_parser()
    #parser.setContentHandler(reader)
    #fn = '"'+filepath+'"'
    #print 'read_interval >'+fn+'<'
    #print '     >'+filepath+'<'
    #parser.parse(filepath)
    parse(filepath, reader)
    return reader.get_ids(), reader.get_data(),reader.get_interval()       