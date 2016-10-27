import os, sys
from xml.sax import saxutils, parse, handler
if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR,'..','..')
    sys.path.append(os.path.join(SUMOPYDIR))

from modules_common import *
import numpy as np            
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlmanager as xm
from agilepy.lib_base.geometry import find_area

from coremodules.network.network import OsmIdsConf
                                
                             
        
class LanduseTypes(am.ArrayObjman):
    def __init__(self, parent, **kwargs):
        
        self._init_objman(  ident='landusetypes', 
                            parent=parent, 
                            name = 'Landuse types', 
                            info = 'Table with information on landuse types', 
                            **kwargs)
        
             
        # landuse types table    
        self.add_col(am.ArrayConf( 'typekeys', '',
                                    is_index = True,
                                    dtype = 'object',# actually a string of variable length
                                    perm='r', 
                                    name = 'Type',
                                    info = 'Type of facility. Must be unique, because used as key.',
                                    ))
                                        
        #self.add_col(am.ArrayConf( 'osmid', '',
        #                                dtype = 'object',# actually a string of variable length
        #                                perm='rw', 
        #                                name = 'Name',
        #                                info = 'Name of facility type used as reference in OSM.',
        #                                ))
                                        
        self.add_col(am.ListArrayConf( 'osmfilters',
                                        perm='r', 
                                        name = 'OSM filter',
                                        info = 'List of openstreetmap filters that allow to identify this facility type.',
                                        ))
                                        
        self.add_col(am.ArrayConf(  'colors', np.ones(4,np.float32),
                                        dtype=np.float32,
                                        metatype = 'color',
                                        perm='rw', 
                                        name = 'Colors',
                                        info = "This facility's color as RGB tuple with values from 0.0 to 1.0",
                                        )) 
                                        
        # default types
        self.add_row(   typekeys='leisure',
                        osmfilters = ['sport','leisure.park','park'],
                        colors=(  0.5, 0.8, 0.6 , 1.0)
                        )
        self.add_row(   typekeys='commercial',
                        osmfilters = ['shop.*','building.commercial'],
                        colors=( 0.6171875,  0.6171875,  0.875, 1.0),
                        )
        self.add_row(   typekeys= 'industrial',
                        osmfilters = ['building.industrial'],
                        colors=(0.89453125,  0.65625   ,  0.63671875, 1.0),
                        )
        self.add_row(   typekeys='parking',
                        osmfilters = ['building.parking','amenity.parking'],
                        colors=(0.52734375,  0.875     ,  0.875, 1.0), 
                        )
        self.add_row(   typekeys= 'residential',
                        osmfilters = ['building.*','building'],
                        colors= (0.921875,  0.78125 ,  0.4375 , 1.0),
                        )                                                        


class Zones(am.ArrayObjman):
    def __init__(self, parent, **kwargs):
        self._init_objman(  ident='zones', parent=parent, name = 'Zones', 
                            is_plugin = True,**kwargs)
        
             
        self.add_col(am.ArrayConf( 'coords',  np.zeros(3, dtype=np.float32),
                                        dtype=np.float32,
                                        groupnames = ['state'],
                                        perm='r', 
                                        name = 'Coords',
                                        unit = 'm',
                                        info = 'Zone center coordinates.',
                                        is_plugin = True,
                                        ))
        
        self.add_col(am.ListArrayConf( 'shapes', 
                                        groupnames = ['_private'], 
                                        perm='rw', 
                                        name = 'Shape',
                                        unit = 'm',
                                        info = 'List of 3D Shape coordinates delimiting a zone.',
                                        is_plugin = True,
                                        ))
    def make(self, coord = np.zeros(3, dtype=np.float32),  shape = []):
        """
        Add a zone
        """
        print 'Zone.make',coord
        print '  shape',type(shape),shape
        self.get_coords_from_shape(shape)
        id_zone = self.add_row( #coords = coord, 
                                shapes =shape, 
                                )
        #print '   shapes\n',self.shapes.value
        #print '   zone.shapes[id_zone]\n',self.shapes[id_zone]

        return id_zone
    
    def get_coords_from_shape(self, shape):
        print 'get_coords_from_shape',np.array(shape),np.sum(np.array(shape),0)
        
    def del_element(self, id_zone):
        print 'del_element',id_zone
        self.del_row(id_zone)
       
                            
               
        
class Facilities(am.ArrayObjman):
    def __init__(self, parent, landusetypes, zones, **kwargs):
        
        self._init_objman(  ident='facilities', 
                            parent=parent, 
                            name = 'Facilities', 
                            info = 'Information on buildings, their type of usage and access to the transport network.', 
                            **kwargs)
                            
        self.add_col(OsmIdsConf('Facility'))
        
        self.add_col(am.IdsArrayConf( 'ids_landusetype', landusetypes, 
                                            groupnames = ['landuse'], 
                                            name = 'ID landuse', 
                                            info = 'ID of landuse.',
                                            ))
                                            

        self.add_col(am.IdsArrayConf( 'ids_zone', zones, 
                                            groupnames = ['landuse'], 
                                            name = 'ID zone', 
                                            info = 'ID of traffic zone, where this facility is located.',
                                            ))
                                            

        
        self.add_col(am.ArrayConf( 'osmkeys', '',
                                    dtype = 'object',# actually a string of variable length
                                    perm='rw', 
                                    name = 'OSM key',
                                    info = 'OSM key of facility.',
                                    ))
                                    
        self.add_col(am.ArrayConf( 'areas', 0.0,
                                        dtype=np.float32,
                                        groupnames = ['landuse'], 
                                        perm='r', 
                                        name = 'Area',
                                        unit = 'm^2',
                                        info = 'Area of this facility.',
                                        ))
                                        
        
        self.add_col(am.ArrayConf( 'centroids',  np.zeros(3, dtype=np.float32),
                                        dtype=np.float32,
                                        groupnames = ['state'],
                                        perm='r', 
                                        name = 'Center',
                                        unit = 'm',
                                        info = 'Center coordinates of this facility.',
                                        ))
                                        
        self.add_col(am.ListArrayConf( 'shapes', 
                                        groupnames = ['_private'], 
                                        perm='rw', 
                                        name = 'Shape',
                                        unit = 'm',
                                        info = 'List of 3D Shape coordinates of facility.',
                                        ))
                                        
        # configure only if net is initialized
        if self.get_net() != None:
            net = self.get_net()
            self.add_col(am.IdsArrayConf( 'ids_edge_closest_road', net.edges, 
                                                groupnames = ['landuse'], 
                                                name = 'Road edge ID', 
                                                info = 'ID of road edge which is closest to this facility.',
                                                ))
                                                                                                                
            
            self.add_col(am.ArrayConf( 'positions_edge_closest_road', 0.0,
                                            dtype=np.float32,
                                            groupnames = ['landuse'], 
                                            perm='r', 
                                            name = 'Road edge pos',
                                            unit = 'm',
                                            info = 'Position on road edge which is closest to this facility',
                                            ))
                                                                                                                                            
    
            
            #self.ids_stop_closest = self.facilities.add(cm.ArrayConf( 'ids_stop_closest', None,
            #                        dtype = 'object',
            #                        name = 'ID stops', 
            #                        perm='rw',
            #                        info = 'List of IDs of closest public transport stops.',
            #                        ))
    
    def get_landusetypes(self):
        return self.ids_landusetype.get_linktab()    
    
    def get_net(self):
        # parent must be scenario
        if self.parent != None:
            return self.parent.get_net()
        else:
            return None
        
    
    def add_poly(self, id_osm, id_landusetype =None, osmkey = None, shape = np.array([],np.float32)):
        """
        Adds a facility as used on sumo poly xml info
        """
        #print 'add_poly',id_osm,id_landusetype,osmkey
        
        
        landusetypes = self.get_landusetypes()
        if id_landusetype!=None:
            # this means that landusetype has been previousely identified
            if osmkey == None:
                # use filter as key
                osmkey = landusetypes.osmfilters[id_landusetype][0]
                
                
            return self.add_row(    ids_osm = id_osm, 
                                    ids_landusetype = id_landusetype, 
                                    osmkeys = osmkey,
                                    shape =shape, 
                                    areas = find_area(shape[:,:2]),
                                    centroids =np.mean(shape,0)
                                    )
        
        else:
            # identify ftype from fkeys...
            keyvec = osmkey.split('.')
            len_keyvec=len(keyvec)
            is_match = False
            for id_landusetype in landusetypes.get_ids():
                #print '  ',landusetypes.osmfilters[id_landusetype]
                #if fkeys==('building.industrial'): print ' check',facilitytype
                for osmfilter in landusetypes.osmfilters[id_landusetype]:
                    #print '     ',
                    osmfiltervec = osmfilter.split('.')
                    if osmkey==osmfilter: # exact match of filter
                        is_match = True
                        #if fkeys==('building.industrial'):print '    found exact',osmkey
                    elif (len(osmfiltervec)==2)&(len_keyvec==2):    
                        
                        if osmfiltervec[0]==keyvec[0]:
                            
                            if osmfiltervec[1]=='*':
                                is_match = True
                                #if fkeys==('building.industrial'):print '    found match',osmkeyvec[0]
                                
                            #redundent to exact match
                            #elif osmkeyvec[1]==keyvec[1]:
                            #    is_match = True
                            #   if is_match:
                            #       print '    found exact',osmkey
                   
                        
                    
                    if is_match:
                        #if fkeys==('building.industrial'):print '  *found:',facilitytype,fkeys
                        #return self.facilities.set_row(ident, type = facilitytype, polygon=polygon, fkeys = fkeys,area=find_area(polygon),centroid=np.mean(polygon,0))
                        return self.add_row(    ids_osm = id_osm, 
                                    ids_landusetype = id_landusetype, 
                                    osmkeys = osmkey,
                                    shapes = shape, 
                                    areas = find_area(shape[:,:2]),
                                    centroids =np.mean(shape,0)
                                    )
    
    
                        
    def import_poly(self, polyfilepath):
        print 'import_poly from %s '%(polyfilepath,)
        
        # let's read first the offset information, which are in the 
        # comment area
        fd = open(polyfilepath, 'r')
        is_comment = False
        is_processing = False
        offset = self.get_net().get_offset() # default is offset from net
        #print '  offset,offset_delta',offset,type(offset)
        #offset = np.array([0,0],float)
        for line in fd.readlines():
            if line.find('<!--')>=0:
                is_comment = True
            if is_comment & (line.find('<processing')>=0):
                is_processing = True
            if is_processing & (line.find('<offset.x')>=0):
                offset[0]= float(xm.read_keyvalue(line,'value'))
            elif is_processing & (line.find('<offset.y')>=0):
                offset[1]= float(xm.read_keyvalue(line,'value'))
        fd.close()
        offset_delta = offset - self.get_net().get_offset()
        #print '  offset,offset_delta',offset,offset_delta
        facilityreader = FacilityReader(self, offset_delta)
        try:
            parse(polyfilepath, facilityreader)
        except KeyError:
            print >> sys.stderr, "Error: Problems with reading facilities!"
            raise
        #print '  self.shapes',self.shapes.value
   
    
class FacilityReader(handler.ContentHandler):
    """Reads facilities from poly.xml file into facility structure"""

    def __init__(self, facilities, _offset_delta):
        
            
        self._facilities = facilities
        
        self._id_facility = None
        self._offset_delta = _offset_delta
            
    def startElement(self, name, attrs):
        #print 'startElement',name,len(attrs)
        if name == 'poly':
                
            shape = self.getShape(      attrs.get('shape',""),
                                        offset = self._offset_delta, 
                                        ) 
            
            self._facilities.add_poly(  id_osm = attrs['id'],
                                                id_landusetype = attrs.get('lutype',None),
                                                osmkey=attrs['type'],
                                                shape = shape,
                                                )
            
 
                                    
            # color info in this file no longer used as it is defined in
            # facility types table
            #color = np.array(xm.parse_color(attrs['color']))*0.8,# make em darker!! 
            


    #def characters(self, content):
    #    if self._id!=None:
    #        self._currentShape = self._currentShape + content


    def endElement(self, name):
        pass
    
    def getShape(self, shapeString, offset = [0.0,0.0]):
        cshape = []
        es = shapeString.rstrip().split(" ")
        for e in es:
            p = e.split(",")
            #if len(p)==2:
            #    p = c+['0.0']
            #elif 
            
            if len(p) == 2:
                # 2D coordinates with elevetion = 0
                cshape.append(np.array([float(p[0])-offset[0], float(p[1]) - offset[1], 0.0], np.float32))
            elif len(p) == 3:
                # 3D coordinates 
                
                #cshape.append(np.array([0,0,0],np.float))
                cshape.append([ float(p[0])-offset[0], float(p[1]) - offset[1], float(p[2]) ] )
            else:
                #print 'WARNING: illshaped shape',e
                #cshape.append(np.array([0,0,0],np.float))
                return None
            
        return np.array(cshape, np.float32)
        
                                                                

                                            
    
                                        

class Landuse(cm.BaseObjman):
        def __init__(self, parent=None, **kwargs):
            self._init_objman(ident= 'landuse', parent=parent, name = 'Landuse', **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            #self.net = None
            self.zones = attrsman.add(   cm.ObjConf( Zones(self) ) )
            self.landusetypes = attrsman.add(   cm.ObjConf( LanduseTypes(self) ) )
            self.facilities = attrsman.add(   cm.ObjConf( Facilities(self,self.landusetypes, self.zones) ) )
        
        
        #def set_net(self, net):
        #    self.net = attrsman.add(   cm.ObjConf( net, is_child = False ) )# link only
            
                   
        def get_net(self):
            # parent of landuse must be scenario
            if self.parent != None:
                return self.parent.net
            else:
                return None 
        
        def import_xml(self, rootname, dirname=''):
            filepath = os.path.join(dirname,rootname+'.poly.xml')
            if os.path.isfile(filepath):
                self.facilities.import_poly(filepath)
            else:
                self.get_logger().w('import_xml: files not found:'+filepath, key='message')                            
    

if __name__ == '__main__':          
    ###############################################################################
    #print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #from coremodules.scenario import scenario
    from coremodules.network import network
    logger = Logger()
    NETPATH = os.path.join(SUMOPYDIR,'coremodules','network','testnet')
    net = network.Network(logger = logger)
    ident_scenario = 'facsp2'
    net.import_xml(ident_scenario, NETPATH)
    #net.read_sumonodes(os.path.join(NETPATH,'facsp2.nod.xml'))
    #net.read_sumoedges(os.path.join(NETPATH,'facsp2.edg.xml'))
    landuse = Landuse( logger = logger)
    landuse.set_net(net)
    #landuse.facilities.import_poly(os.path.join(NETPATH,'facsp2.poly.xml'))
    landuse.import_xml(ident_scenario, NETPATH)
    objbrowser(landuse)
    
    
