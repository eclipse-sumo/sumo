"""
This is a dummy plugin preserved only to preserve downward compatibility with old scenarios.
"""
           
import agilepy.lib_base.classman as cm

from coremodules.demand.demandbase import DemandobjMixin


class MCube(DemandobjMixin,cm.BaseObjman):
        def __init__(self, ident = 'mcube', demand = None,  
                        workdirpath = None, **kwargs):
                            
            self._init_objman(ident= ident, parent = demand, 
                    name = 'MCube Integration', info = 'MCube Integration', 
                    version = 0.0,
                    **kwargs)
                    
