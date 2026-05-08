
           
import agilepy.lib_base.classman as cm

from coremodules.demand.demandbase import DemandobjMixin
from . import matsim_network
from . import matsim_config
from . import matsim_pt
from . import matsim_demand
from . import matsim_mapmatch



class Matsim(DemandobjMixin,cm.BaseObjman):
        def __init__(self, ident = 'matsim', demand = None,  
                        workdirpath = None, **kwargs):
                            
            self._init_objman(ident= ident, parent = demand, 
                    name = 'MATSim Scenario', info = 'Main MATSim scenario instance.', 
                    version = 0.0,
                    **kwargs)
                    
            attrsman = self.set_attrsman(cm.Attrsman(self))

            self.config = attrsman.add(   cm.ObjConf( matsim_config.Configuration(self) ) )
            self.net = attrsman.add(   cm.ObjConf( matsim_network.Network(self) ) )
            self.pt = attrsman.add(   cm.ObjConf( matsim_pt.PublicTransport(self) ) )
            self.mapmatch = attrsman.add(   cm.ObjConf( matsim_mapmatch.mapmatch(self) ) )
            #self.demand = attrsman.add(   cm.ObjConf( matsim_demand.Demand(self) ) )
            
        def get_scenario(self):
            return self.parent.parent
  
