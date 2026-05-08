
import numpy as np            
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import random_choice, get_inversemap
from . import results
from .simplaconfig import SimplaConfig


class Simulation(cm.BaseObjman):
        def __init__(self, scenario,  name = 'Simulation', 
                        info ='Simulation, contains simulation specific parameters and methods.', **kwargs):
            #print 'Network.__init__',name,kwargs
            self._init_objman(  ident= 'simulation', 
                                parent=scenario, 
                                name = name, 
                                info = info, 
                                version = 0.2,
                                **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            self._init_attributes()
            self._init_constants()
        
        def _init_attributes(self):
            #print 'Simulation._init_attributes id',id(self),self.parent.rootname#,dir(self)
            attrsman = self.get_attrsman()
            
            #if self.get_version()<0.2:
            #    self.delete('results')
            
                 
            self.results = attrsman.add(cm.ObjConf(\
                                results.Simresults('results', parent=self),
                                is_child = False, 
                                is_save = False,# will not be saved 
                                groups = ['results']))
            
            # upgrade
            #self.results.set_save(False)                    
            #print '  self.results', self.results
            
            # load taxi services
            #self.taxiservice = attrsman.add(cm.ObjConf(\
            #                                    TaxiService(ident='taxiexperimentservice',simulation=self),
            #                                    is_child = True, 
            #                                    groups = ['misc']
            #                                    ))
                                                
            # platooning simulation tool
            self.simplaconfig = attrsman.add(cm.ObjConf(\
                                                SimplaConfig(self),
                                                is_child = True, 
                                                groups = ['misc']
                                                ))
                                                
        
        def _init_constants(self):
            # no! for attrs onlyself.do_not_save_attrs(['results',])# redundant is_save = False
            pass
        
        def get_scenario(self):
            return self.parent
        
        def add_simobject(self, obj = None, ident = None, SimClass = None, **kwargs):
            #print('add_simobject',obj,ident,SimClass)
            attrsman = self.get_attrsman()    
            if obj is not None:
                ident = obj.get_ident()
            
            if not hasattr(self,ident): # is this sim object already present
                if obj is  None:
                    # init simobject and make it a child of simulation
                    obj = SimClass(ident, self, **kwargs)
                    is_child = True
                else:
                    # link to simobject, which must be a child of another object
                    is_child = False
                
                
                attrsman.add( cm.ObjConf(   obj, 
                                            groupnames = ['simulation objects'],
                                            is_child = is_child,
                                            ) )
                
                setattr(self,ident,obj)
                
            else:
                # make sure simobject has the group  'simulation objects'
                attrsman.get_config(ident).add_groupnames(['simulation objects'])
            
            return getattr(self, ident)
                
                
            
            
        
        def get_simobjects(self):
            #demandobjects = set([])
            #for ident, conf in self.get_group_attrs('').items():
            #    demandobjects.add(conf.get_value())
            return list(self.get_attrsman().get_group_attrs('simulation objects').values())
