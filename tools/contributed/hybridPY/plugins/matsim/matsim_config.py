from xml.sax import saxutils, parse, handler
import os, sys, shutil
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
from xml.sax import parse, handler
try:
    import pyproj
except Exception:
    from mpl_toolkits.basemap import pyproj
    
from .matsim_base import  *
pathsep = os.path.sep
#from coremodules.network.network import SumoIdsConf

                          
duration_last_activity = 3*3600 # this is arbitrary, but does not have any effect on the simuation

class Configuration(cm.BaseObjman):
        def __init__(self, parent=None, name = 'Configuration', **kwargs):
            print ('Configuration',parent,name)
            self._init_objman(  ident= 'config', parent=parent, name = name,
                                #xmltag = 'net',# no, done by netconvert
                                version = 0.1,
                                **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            self._init_attributes()
            self.matsimconfig = attrsman.add(   cm.ObjConf( ConfigFile(self) ) )

        def _init_attributes(self):
            attrsman = self.get_attrsman()
            self.population_config = attrsman.add(cm.AttrConf( 'population_config','path to file',
                            groupnames = ['options'], 
                            name = 'population_config', 
                            info = "population_config.",
                            ))
            self.general_config = attrsman.add(cm.AttrConf( 'general_config','path to file',
                            groupnames = ['options'], 
                            name = 'general_config', 
                            info = "general_config",
                            ))


        def write_config(self,path):
                #path = 'C:\\models\\sumopytest\\base-data\\config_out.xml'
                simfile = open(path, 'w', encoding="utf-8")

                simfile.write(\
                        """<?xml version="1.0" encoding="UTF-8"?>\n<!DOCTYPE config SYSTEM "http://www.matsim.org/files/dtd/config_v2.dtd">\n<config>\n """)
                # <node id="1000056237" x="4422218.544286327" y="5375842.302200025" />
                module_old = ''
                paramset1_old = ''
                paramset2_old = ''
                xmlcounter_old = ''

                for id in self.matsimconfig.get_ids():
                    module, paramset1, paramset2, param, value, xmlcounter = self.matsimconfig.get_row(id)['module'],self.matsimconfig.get_row(id)['paramset1'],self.matsimconfig.get_row(id)['paramset2'],self.matsimconfig.get_row(id)['param'],self.matsimconfig.get_row(id)['value'],self.matsimconfig.get_row(id)['xmlcounter']
                    
                    #Prüfe, ob es ein neues Modul ist!
                    if module != module_old:
                        if module_old != '':

                            if paramset2_old != '':
                                simfile.write('\t\t\t\t</parameterset>\n')                     
                            if paramset1_old != '':
                                simfile.write('\t\t</parameterset>\n')
                            #Wenn es vorher bereits ein Modul gab, dann erst abschließen und counter zurücksetzen!
                            simfile.write('\t</module>\n')
                            
                            paramset1_old = ''
                            paramset2_old = ''
                            xmlcounter_old = ''
                        simfile.write('\t<module name=\"' + module + '\" >\n')
                        module_old = module
                        #if paramset1 != paramset1_old:
                        #    simfile.write('<parameterset type=\"' + paramset1 + '\" >\n')
                        simfile.write('\t\t<param name=\"' + param + '\" value=\"' + value + '\" />\n')
                    else:
                        if (paramset1 != paramset1_old and paramset1_old == ''):
                            #neues Parameterset 
                            simfile.write('\t\t<parameterset type=\"' + paramset1 + '\" >\n')
                            paramset1_old = paramset1
                        elif (paramset1 != paramset1_old and paramset1 == ''):
                            #neues Parameterset 
                            simfile.write('\t\t</parameterset>\n')
                            paramset1_old = paramset1
                            
                        elif (paramset1 == paramset1_old and xmlcounter == '0' and paramset2 == '') or (paramset1 != paramset1_old and paramset1_old != ''and paramset2 == ''):
                            #neues Parameterset und altes abschließen -> Wechsel 
                            simfile.write('\t\t</parameterset>\n')
                            simfile.write('\t\t<parameterset type=\"' + paramset1 + '\" >\n')
                            paramset1_old = paramset1
                            
                        elif (paramset2 != paramset2_old and paramset2_old == ''):
                            simfile.write('\t\t\t\t<parameterset type=\"' + paramset2 + '\" >\n')
                            paramset2_old = paramset2
                        
                        elif (paramset2 != paramset2_old and paramset2 == ''):
                            simfile.write('\t\t\t\t</parameterset>\n')
                            paramset2_old = paramset2
                            
                        elif (paramset2 == paramset2_old and xmlcounter == '0') or (paramset2 != paramset2_old and paramset2_old != ''):
                            simfile.write('\t\t\t\t</parameterset>\n')
                            simfile.write('\t\t\t\t<parameterset type=\"' + paramset2 + '\" >\n')
                            paramset2_old = paramset2
                        
                        if paramset2 != '':
                            simfile.write('\t\t\t\t<param name=\"' + param + '\" value=\"' + value + '\" />\n')
                        else:
                            simfile.write('\t\t<param name=\"' + param + '\" value=\"' + value + '\" />\n')

                            
                simfile.write('\t</module>\n')
                simfile.write("""</config>\n""")
                simfile.close()



class ConfigFile(am.ArrayObjman):
    def __init__(   self, parent,
                    **kwargs):
        ident = 'matsimconfig'
        self._init_objman( ident=ident, parent=parent, name = 'matsimconfig',
                            xmltag = ('config'),
                            version = 0.0,
                            info = 'matsimconfig',
                            **kwargs)
        self._init_attributes()

    def _init_attributes(self):


        self.add_col(am.ArrayConf('module', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'module',
                                info = 'module',
                                xmltag = 'module',
                                ))
        self.add_col(am.ArrayConf('paramset1', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'paramset1',
                                info = 'paramset1',
                                xmltag = 'paramset1',
                                ))
        
        self.add_col(am.ArrayConf('paramset2', default = '',
                                dtype = str,
                                perm = 'r',
                                is_index = False,
                                name = 'paramset2',
                                info = 'paramset2',
                                xmltag = 'paramset2',
                                ))
                                
        self.add_col(am.ArrayConf( 'param',  default = '',
                            dtype = str,
                            groupnames = ['state'],
                            perm = 'r',
                            name = 'param',
                            unit = 'param',
                            info = 'param',
                            ))



        self.add_col(am.ArrayConf( 'value',  default = '',
                            dtype = str,
                            groupnames = ['state'],
                            perm = 'rw',
                            name = 'value',
                            unit = 'value',
                            info = 'value',
                            ))
        self.add_col(am.ArrayConf( 'xmlcounter',  default = '',
                            dtype = str,
                            groupnames = ['state'],
                            perm = 'r',
                            name = 'xmlcounter',
                            unit = 'xmlcounter',
                            info = 'xmlcounter',
                            ))
    
class ConfigParser(handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self,number):
        self.ids_node_matsim = np.zeros(number, dtype = object)
        self.module_name  = np.zeros(number, dtype = object) 
        self.paramset1 = np.zeros(number, dtype = object)
        self.paramset2 = np.zeros(number, dtype = object)
        self.param_name = np.zeros(number, dtype = object)
        self.param_value = np.zeros(number, dtype = object)
        self.counter_var = np.zeros(number, dtype = object)
        
        self.module_name_temp  = ''
        self.paramset1_temp = ''
        self.paramset2_temp = ''
        self.param_name_temp = ''
        self.param_value_temp = ''
        self.counter_temp = 0
        self.counter_abs = 0

    # module # paramset1 #paramset2 #param 

    def startElement(self, name, attrs):
        
        if name == 'module':
            self.module_name_temp = attrs['name']
            self.counter_temp = 0
        elif name == 'parameterset' and self.paramset1_temp == '':
            self.paramset1_temp = attrs['type']
            self.counter_temp = 0 
        elif name == 'parameterset' and self.paramset1_temp != '' and self.paramset2_temp == '':
            self.paramset2_temp = attrs['type']
            self.counter_temp = 0
        elif name == 'param':
            self.param_name_temp = attrs['name'] 
            self.param_value_temp = attrs['value']

            self.module_name[self.counter_abs]   = self.module_name_temp
            self.paramset1[self.counter_abs]  = self.paramset1_temp
            self.paramset2[self.counter_abs]  =  self.paramset2_temp
            self.param_name[self.counter_abs]  = self.param_name_temp
            self.param_value[self.counter_abs]  = self.param_value_temp
            self.counter_var[self.counter_abs]  = self.counter_temp
            
            self.counter_temp +=1
            self.counter_abs +=1

    def endElement(self, name):
        
        if name == 'parameterset' and self.paramset1_temp != '' and self.paramset2_temp != '':
            self.paramset2_temp = ''
        elif name == 'parameterset' and self.paramset2_temp == '' and self.paramset1_temp != '':
            self.paramset1_temp = ''
        elif name == 'module':
            self.paramset1_temp = ''
            self.paramset2_temp = ''
            self.module_name_temp = ''

class ConfigCounter(handler.ContentHandler):
    """Reads preliminary information from Matsim plan xml file.
    """

    def __init__(self):
        self.counter = 0

    # module # paramset1 #paramset2 #param 

    def startElement(self, name, attrs):
        
        if name == 'param':
            self.counter +=1



class MATSimConfigImport(Process):

    def __init__(self,  matsim, configpath = '',
                        logger = None):
        #
        #
        #
        self._init_common(  'Import Configuration', name = 'MATSIM import configuration', 
                            logger = logger,
                            info ='Imports MATSIM network XML file into network database.',
                            )
        self._scenario = matsim.get_scenario()
        self._matsim = matsim
        self._net = matsim.net
        self.projparams_matsim = matsim.net.projparams
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()
                
        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.configpath = attrsman.add(cm.AttrConf('configpath',configpath,
                        groupnames = ['options'],# 
                        name = 'Matsim Configuration File', 
                        wildcards = 'Net XML file (*.xml)|*.xml*',
                        metatype = 'filepath',
                        info = 'MATSIM network file to be imported.',
                        ))
        self.poppath = attrsman.add(cm.AttrConf('poppath','',
                        groupnames = ['options'],# 
                        name = 'Matsim Population File', 
                        wildcards = 'Net XML file (*.xml)|*.xml*',
                        metatype = 'filepath',
                        info = 'MATSIM network file to be imported.',
                        ))

    
    def do(self):

        self._matsim.config.population_config = self.poppath

        self._matsim.config.general_config = self.configpath

        if os.path.exists(self.configpath):
            self._matsim.config.matsimconfig.clear()
            parser = ConfigCounter()
            parse(self.configpath, parser)
            counts = parser.counter
            parser2 = ConfigParser(counts)
            parse(self.configpath, parser2)
            #Problem: Parser läuft noch, während der Dialog beendet wird

            self._matsim.config.matsimconfig.add_rows(None, #suggest id
                            module = parser2.module_name,
                            paramset1 = parser2.paramset1,
                            paramset2 = parser2.paramset2,
                            param = parser2.param_name,
                            value = parser2.param_value,
                            xmlcounter = parser2.counter_var,
                            )
            print ('Config Parsing done')
            return True

        