
import numpy as np            
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import random_choice, get_inversemap


class SimobjMixin:
    def prepare_sim(self, process):
        return [] # [(steptime1,func1),(steptime2,func2),...]
   
    
    def config_simresults(self, results):
        #tripresults = res.Tripresults(          'tripresults', results,
        #                                        self, 
        #                                        self.get_net().edges
        #                                        )
        #        
        #
        #results.config(tripresults, groupnames = ['Trip results'])
        pass
    
    def process_results(self, results, process = None):
        pass
    
    def finish_sim(self):
        pass
    