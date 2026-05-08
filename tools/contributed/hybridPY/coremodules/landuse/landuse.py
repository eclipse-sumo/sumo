import os, sys, time, shutil
from xml.sax import saxutils, parse, handler
if  __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except Exception:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    hybridPYDIR = os.path.join(APPDIR,'..','..')
    sys.path.append(hybridPYDIR)
try:
    import pyproj
except Exception:
    from mpl_toolkits.basemap import pyproj

# for elevation api
import json
import urllib
import requests
import pickle

import numpy as np  
from numpy import random 
from numpy.linalg import inv
from collections import OrderedDict
       
from coremodules.modules_common import *
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from  agilepy.lib_base.geometry import *
from  agilepy.lib_base.misc import string_to_float,clean_datarow
from agilepy.lib_base.processes import Process,CmlMixin
from coremodules.network.network import SumoIdsConf, MODES
#from coremodules.network.routing import get_mincostroute_edge2edges
from coremodules.misc.shapeformat import ShapefileImporter
from . import maps 
try:
    #zones clusterization
    from scipy.cluster.vq import  kmeans2
    from scipy.spatial import Voronoi, voronoi_plot_2d
except Exception:
    print('No Scipy module installed. Clustarization functions will not work.')
    
import matplotlib.pyplot as plt
try:
    from shapely.geometry import MultiPoint, Polygon
    from shapely.ops import unary_union
    IS_SHAPELY = True                              
except Exception:
    IS_SHAPELY = False

def qspline(x,a0,a1,a2):
    return a0 + a1*x**2 + a2*x**3

def line(x,a0,m):
    return a0 + m*x

def clean_osm(filepath_in, filepath_out):
    """
    Clean osm file from strange characters that compromize importing.
    """
    #
    substitutes ={ "&quot;":"'","&":"+"}
    fd_in=open(filepath_in,'r', encoding="utf-8")
    fd_out=open(filepath_out,'w', encoding="utf-8")
    for line in fd_in.readlines():
        for oldstr, newstr in substitutes.items():
            line = line.replace(oldstr, newstr)
        fd_out.write(line)
    fd_in.close()
    fd_out.close()
    




                                                                               
class LanduseTypes(am.ArrayObjman):
    def __init__(self, parent,is_add_default = True, **kwargs):
        
        self._init_objman(  ident='landusetypes', 
                            parent=parent, 
                            name = 'Landuse types', 
                            info = 'Table with information on landuse types', 
                            **kwargs)
        
        self._init_attributes()
        if is_add_default:
            self.add_types_default()
        
    def _init_attributes(self):     
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
                                        
        
                                        
        self.add_col(am.ArrayConf(  'colors', np.ones(4,np.float32),
                                        dtype=np.float32,
                                        metatype = 'color',
                                        perm='rw', 
                                        name = 'Colors',
                                        info = "Color corrispondig to landuse type as RGBA tuple with values from 0.0 to 1.0",
                                        xmltag = 'color',
                                        )) 
        
        self.add_col(am.ArrayConf( 'descriptions', '',
                                    dtype = 'object',# actually a string of variable length
                                    perm='r', 
                                    name = 'Info',
                                    info = 'Information about this landuse.',
                                    ))
        
        self.add_col(am.ArrayConf( 'are_area',False,
                                        perm='rw', 
                                        name = 'is area',
                                        info = 'True if this is an area of particular use, instead of a single facility. An area can include several facilities. If False, then it is dealt with a single facility.',
                                        ))
                                        
        self.add_col(am.ListArrayConf( 'osmfilters',
                                        perm='r', 
                                        name = 'OSM filter',
                                        info = 'List of openstreetmap filters that allow to identify a landuse type.',
                                        ))
                                        
        self.add_col(am.ListArrayConf( 'poifilters',
                                        perm='r', 
                                        name = 'POI filter',
                                        info = 'List of openstreetmap POI filters that allow to identify a landuse type.',
                                        ))
        # this will remove all previous and setnw                             
        if len(self)>0:
            self.clear()
            self.add_types_default()
            
    def get_landusetype_facility_from_area(self, id_landuse_area):
        # get typekey
        typekey_area = self.typekeys[id_landuse_area]
        typekey_info = typekey_area.split('_')
        if len(typekey_info) == 2:
            if typekey_info[1] == 'area':
                return self.typekeys.get_id_from_index(typekey_info[0])
            else:
                # no area, return unchanged
                return id_landuse_area
        else:
            # no area, return unchanged
            return id_landuse_area
                    
    def format_ids(self,ids):
        #print('landusetype.format_ids typekeys',self.typekeys[ids])
        return ','.join(self.typekeys[ids])
    
    def get_id_from_formatted(self,idstr):
        return self.typekeys.get_id_from_index(idstr)  
    
    def get_ids_from_formatted(self,idstrs):
        return self.typekeys.get_ids_from_indices_save(idstrs.split(','))
                        
    def add_types_default(self):                                
        # default types
        
        self.add_row(   typekeys='leisure',
                        descriptions = 'Facilities which offer leasure type activities',
                        osmfilters = ['historic.church','building.church','building.restaurant','building.cafe','restaurant','sport','sport.*'],
                        poifilters = ['leisure.*','sport.*','amenity.restaurant','amenity.bar','amenity.cafe','amenity.pub','amenity.theatre','amenity.cinema','amenity.nightclub','amenity.fast_food','tourism.*','historic.*'],
                        colors=(  0.2, 0.5, 0.3 , 0.7)
                        )
                        # leisure.nature_reserve is actually an area so remove 'leisure.*'
        
                        
        self.add_row(   typekeys='commercial',
                        descriptions = 'Facility with trade, offices, banks, shopping opportunitties, etc.',
                        osmfilters = ['building.hospital','building.retail','building.shop','shop.*','building.commercial',],
                        # amenity.taxi
                        poifilters = ['shop.*','amenity.bank','amenity.post_office','amenity.veterinary','amenity.pharmacy','amenity.hospital'],
                        colors=( 0.6171875,  0.6171875,  0.875, 0.7),
                        )
                        
                       
                    
        self.add_row(   typekeys= 'industrial',
                        descriptions = 'industrial production facilities.',
                        osmfilters = ['building.industrial'],
                        colors=(0.89453125,  0.65625   ,  0.63671875,0.7),
                        poifilters = [],
                        )
                        
        
                        
        self.add_row(   typekeys='parking',
                        descriptions = 'Areas reserved for car parking. This can be either area or building.',
                        osmfilters = ['building.parking',],
                        colors=(0.52734375,  0.875     ,  0.875, 0.7), 
                        poifilters = ['amenity.parking'],
                        )
                        
        self.add_row(   typekeys= 'residential',
                        descriptions = 'Facilities with residential use',
                        osmfilters = ['building.house','building.apartments','building.*','building'],
                        colors= (0.921875,  0.78125 ,  0.4375 , 0.7),
                        poifilters = [],
                        ) 
                        
        
                        
        self.add_row(   typekeys= 'mixed',  
                        descriptions = 'Facilities with mixed land use, which cannot be clearly assigned to one of the other landuse types.',
                        osmfilters = [],
                        colors= ( 0.5, 0.9, 0.5 , 0.7),
                        poifilters = [],
                        )  
        
        
                        
        self.add_row(   typekeys= 'sink',
                        descriptions = 'Areas where vehicles disappear (evaporate). These zones are used for turn-flow demand models in order to avoid the creation of routes with loops.',
                        osmfilters = [],
                        colors= (0.5,  0.0 ,  0.1 , 1.0),
                        poifilters = [],
                        ) 
        self.add_row(   typekeys= 'education',
                        descriptions = 'Educational facilities such as schools, universities',
                        # with amenity and landuse, we should look inside and assign all buildings
                        osmfilters = ['building.school','building.university'],
                        colors= (0.89453125,  0.65625   ,  0.89453125,0.7),
                        poifilters = ['amenity.university','amenity.school'],
                        ) 
        
        self.add_row(   typekeys='leisure_area',
                        descriptions = 'Areas on which leasure type activitiesbare offered',
                        osmfilters = ['leisure.park','park','amenity.park','landuse.park','amenity.sport','landuse.sport'],
                        are_area = True,
                        colors=(  0.2, 0.5, 0.3 , 0.7),
                        poifilters = [],
                        )
        
        self.add_row(   typekeys= 'industrial_area',
                        descriptions = 'Area with industrial production facilities.',
                        osmfilters = ['amenity.industrial','landuse.industrial',],
                        are_area = True,
                        colors=(0.89453125,  0.65625   ,  0.63671875,0.7),
                        poifilters = [],
                        )
        
        self.add_row(   typekeys='commercial_area',
                        descriptions = 'Areas with trade, offices, banks, shopping opportunitties, or other customer services.',
                        osmfilters = ['amenity.hospital','amenity.commercial','landuse.commercial',],
                        colors=( 0.6171875,  0.6171875,  0.875, 0.7),
                        are_area = True,
                        poifilters = [],
                        ) 
                                                        
        self.add_row(   typekeys= 'residential_area',
                        descriptions = 'Areas with residential facilities.',
                        osmfilters = ['amenity.residential','landuse.residential'],
                        colors= (0.921875,  0.78125 ,  0.4375 , 0.7),
                        are_area = True,
                        poifilters = [],
                        ) 
                        
        self.add_row(   typekeys= 'mixed_area',  
                        descriptions = 'Facilities with mixed land use, which cannot be clearly assigned to one of the other landuse types.',
                        osmfilters = [],
                        colors= ( 0.5, 0.9, 0.5 , 0.7),
                        are_area = True,
                        poifilters = [],
                        )
                        
        self.add_row(   typekeys= 'education_area',
                        descriptions = 'Educational facilities such as schools, universities',
                        osmfilters = ['landuse.university','landuse.school','amenity.school','amenity.university'],
                        colors= (0.89453125,  0.65625   ,  0.89453125,0.7),
                        are_area = True,
                        poifilters = [],
                        )   
                        
        self.add_row(   typekeys='parking_area',
                        descriptions = 'Areas reserved for car parking. This can be either area or building.',
                        osmfilters = ['amenity.parking'],
                        colors=(0.52734375,  0.875     ,  0.875, 0.7), 
                        are_area = True,
                        poifilters = [],
                        )

class POIs(am.ArrayObjman):
    
    def __init__(self, landuse, **kwargs):
        self._init_objman(  ident='pois', parent = landuse, 
                            name = 'POIs', 
                            info = 'Points of interest',
                            is_plugin = True,
                            version = 0.1,
                            **kwargs)
        
        self._init_attributes()
        self._init_constants()
        
        
    def _init_attributes(self):
        #print 'POIs._init_attributes'
        self.add_col(SumoIdsConf('POI', name = 'ID', perm = 'rw', info = 'Unique short name or ID of Point of interest.'))     
        
    
                                                                                                                                
        self.add_col(am.ArrayConf( 'names', '',
                                        dtype = np.object_,
                                        groupnames = ['parameter'], 
                                        perm='rw', 
                                        name = 'Extended name',
                                        info = 'Extended, human readable name, no uniqueness required, not used as key.',
                                        ))
                                    
        # TODO: change this name to something like ids_buildingfacility_nearest
        self.add_col(am.IdsArrayConf(  'nearest_buildings', self.parent.facilities,
                                    groupnames = ['parameter'], 
                                    name = 'Building', 
                                    info = 'Building representing the house in which the POI is located. Amenities are not considered.',
                                    ))
                                    
        self.add_col(am.ArrayConf( 'osmkeys', 'building.yes',
                                        groupnames = ['parameter'], 
                                        dtype = 'object',# actually a string of variable length
                                        perm='rw', 
                                        name = 'OSM key',
                                        info = 'OSM key of facility.',
                                        xmltag = 'type',
                                        is_plugin = True,
                                        ))
                                        
                                        
        self.add_col(am.IdsArrayConf( 'ids_landusetype',  self.parent.landusetypes,
                                        id_default = 6,
                                        #choices = self.parent.landusetypes.typekeys.get_indexmap(),
                                        #choiceattrname = 'typekeys',
                                        groupnames = ['parameter'],
                                        perm='rw', 
                                        name = 'Type',
                                        info = 'Zone type. This is actually the landuse type.',
                                        ))
                                    
        
        if self.get_version() < 0.1:
            # there is a problem with dicts as array objects
            # a special class needs to be created similar to ListArrayConf
            if hasattr(self,'openings'):
                self.delete('openings')

                                           
        self.add_col(am.ArrayConf( 'coords',  np.zeros(3, dtype=np.float32),
                                        groupnames = ['parameter'],
                                        perm='r', 
                                        name = 'Coords',
                                        unit = 'm',
                                        info = 'Zone center coordinates.',
                                        is_plugin = True,
                                        ))
    
    
    def _init_attributes_extra(self):
        """
        Init extra attributes to accomodate data from google places
        """
        self.add_col(am.ArrayConf( 'openinghours',  np.zeros(4, dtype=np.float32),
                                        groupnames = ['parameter'],
                                        perm='r', 
                                        name = 'Opening hours',
                                        unit = 'h',
                                        info = 'Array with opening hours. [morning open, morning close, evening open, evening close]',
                                        is_plugin = True,
                                        ))
        self.add_col(am.ArrayConf( 'rating', -1.,
                                        groupnames = ['parameter'], 
                                        perm='rw', 
                                        name = 'Rating',
                                        info = 'Rating by service users.',
                                        ))
                                        
        self.add_col(am.ArrayConf( 'ratingcount', 0,
                                        groupnames = ['parameter'], 
                                        perm='rw', 
                                        name = 'Rating count',
                                        info = 'Number of users rating this service.',
                                        ))
        
    def add_pois(self, ids_sumo = [], **kwargs):
        """
        Adds POIs as used on sumo poly xml info
        """
        return self.add_rows(n=len(ids_sumo),    ids_sumo = ids_sumo, **kwargs)
        
    def guess_facilities(self):
        """
        Associate the nearest building to each POI
        """
        facilities = self.parent.facilities
        landuse = facilities.parent
        ids_fac = facilities.get_ids()
        ids_build = facilities.get_ids_building()
            
        ids_poi = self.get_ids()
        centroids_facs = facilities.centroids[ids_build]
        for id_poi in ids_poi:
            coords_poi = self.coords[id_poi]
            diff_coords = centroids_facs-coords_poi
            dists = np.abs(np.sqrt(diff_coords[:,0]**2 + diff_coords[:,1]**2))
            id_fac = ids_build[np.argmin(dists)]
            self.nearest_buildings[id_poi] = id_fac
    
        return True
    
    def update_landusetype_facilities(self):
        """
        Modifies the landuse type of facilities depending on the POI's landuse.
        If POI and facility have different landuse types then chenge to 'mixed' landuse type 
        """
        print('update_landusetype_facilities')
        facilities = self.parent.facilities
        landusetypes = self.get_landusetypes()
        ids = self.get_ids()
        ids_fac = self.nearest_buildings[ids]
        
        # filter facilities without landuse:
        inds_filter = np.flatnonzero(facilities.ids_landusetype[ids_fac] == -1)
        print('  substituting landuse with POI ids_fac',ids_fac[inds_filter])
        # set facility landuse from poi
        facilities.ids_landusetype[ids_fac[inds_filter]] = self.ids_landusetype[ids[inds_filter]]
        
        # filter failities with differences in landuse
        inds_filter = np.flatnonzero(self.ids_landusetype[ids] != facilities.ids_landusetype[ids_fac])
        print('  ids',len(ids),ids)
        print('  ids_fac',len(ids_fac),ids_fac)
        print('  inds_filter',self.ids_landusetype[ids] != facilities.ids_landusetype[ids_fac])
        print('  inds_filter',inds_filter)
        print('  change to mixed landuse ids_fac',landusetypes.typekeys.get_id_from_index('mixed') ,ids_fac[inds_filter])
        
        # set mixed mode for these fac
        facilities.ids_landusetype[ids_fac[inds_filter]] = landusetypes.typekeys.get_id_from_index('mixed')  
        
        
        
       
        
    def import_poly(self, polyfilepath,is_remove_xmlfiles=False, is_clear = True, **kwargs):
        print('import_poly from %s '%(polyfilepath,))
        if is_clear:
            self.clear()
        # let's read first the offset information, which are in the 
        # comment area
        fd = open(polyfilepath,'r', encoding="utf-8")
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
                break
        fd.close()
        offset_delta = offset - self.get_net().get_offset()
        
        exectime_start = time.perf_counter()
            
        counter = SumoPoiCounter()
        parse(polyfilepath, counter)
        fastreader = SumoPoiReader(self, counter, offset_delta, **kwargs)
        parse(polyfilepath, fastreader)
        fastreader.finish()
        
        # update ids_landuse...
        #self.update()
       
        # timeit
        print('  exec time=',time.perf_counter() - exectime_start)
            
        #print '  self.shapes',self.shapes.value
        
    def get_landusetypes(self):
        return self.ids_landusetype.get_linktab() 
        
    def get_net(self):
        return self.parent.parent.net




def download_google_places(latlon,radius,key, places, next_page_token = None):
    #print ('download_places')
    baserurl = f"https://maps.googleapis.com/maps/api/place/nearbysearch/json"
    
    # Anfrage an die API senden
    if next_page_token is None:
        #print ('  new center at latlon',latlon)
        url_all = baserurl+f"?location={latlon[0]},{latlon[1]}&radius={radius}&key={key}"
    else:
        url_all = baserurl+f"?key={key}"+'&pagetoken='+next_page_token
    
    #print ('  url=',url_all)
    try:
        response = requests.get(url_all)
    except Exception:
        print('ERROR: there is probably a problem with your Internet connection.')
    
    print ('  response status_code',response.status_code)
    
    
    # Überprüfen, ob die Anfrage erfolgreich war
    if response.status_code == 200:
        data = response.json()
        #with open(filename, 'wb') as file:
        #    pickle.dump(data, file)
                
        if 'next_page_token' in data:
            next_page_token = data['next_page_token']
            
        else:
            next_page_token = None
        
        #print ('  next_page_token',next_page_token)
        print ('  contains results','results' in data,'status',data['status'],)    
        # places durchgehen und Informationen anzeigen
        if 'results' in data:
            print ('    found',len(data['results']),'places in page')
            for place in data['results']:
                id_place = str(place['place_id']) # need strings for database
                #print ('\n\n',place)
                if id_place not in places:
                    places[id_place] = {}
                    
                    if 'geometry' in place:
                        places[id_place]['latlon'] = (place['geometry']['location']['lat'],place['geometry']['location']['lng'])
                    else:
                        places[id_place]['latlon'] = (0.0,0.0)
                        
                    
                    places[id_place]['name'] = place.get('name')
                    places[id_place]['address'] = place.get('vicinity', '') # empty string means no address available
                    places[id_place]['types'] = ", ".join(place.get('types', []))
                    if 0:
                        print(f"    ID: {id_place}")
                        print(f"    Name: {places[id_place]['name']}")
                        print(f"    Adresse: {places[id_place]['address']}")
                        print(f"    Typen: {places[id_place]['types']}")
                        print("    latlon: ",places[id_place]['latlon'])
                        print("-" * 40)
                
                else:
                    #print('    Already in database ID:',id_place)
                    pass
        else:
            print("WARNING: No places found.")
    else:
        
        print(f"ERROR: {response.status_code}")
    
    return next_page_token
    
def download_google_places_extra(key, places, day_of_week, openinghours_default, 
            fields = 'userRatingCount,rating,currentOpeningHours' ):
    """
    Enrich dictionary with place IDs as key with opening hours 
    and user rating
    """
    baserurl = "https://places.googleapis.com/v1/places/"
    n_places = len(places)
    n = 0
    for id_place, place in  places.items():
        n += 1
        print("-" * 10+'progress: %d/%d places %.2f%%'%(n,n_places,float(n)/n_places*100))
        print ('  id_place',id_place,place['name'])
        url_all = baserurl+id_place+'?fields='+fields+'&key='+key
        
        #print ('  url=',url_all)
        try:
            response = requests.get(url_all)
        except Exception:
            print('ERROR: there is probably a problem with your Internet connection.')
            
        print ('    response status_code',response.status_code)
    
        # Überprüfen, ob die Anfrage erfolgreich war
        if response.status_code == 200:
            data = response.json()
            #print ('\n  data=',data)
            openinghours = []
            if 'currentOpeningHours' in data:
                if 'periods' in data['currentOpeningHours']:
                    for period in data['currentOpeningHours']['periods']:
                        poi_open = period['open']
                        poi_close = period['close']
                        if (poi_open['day'] == day_of_week) & (poi_close['day'] == day_of_week):
                            openinghours.append( float(poi_open['hour'])+float(poi_open['minute'])/60)
                            openinghours.append( float(poi_close['hour'])+float(poi_close['minute'])/60)
            
            if (len(openinghours) <= 1):
                # no opening hour detected
                openinghours = openinghours_default
                
            elif len(openinghours) == 2:
                # only one period detected
                # create artificial lunch brake of zero seconds
                lunchtime = 0.5*(openinghours[0]+openinghours[1])
                openinghours = [openinghours[0],lunchtime,lunchtime,openinghours[1]]
            
            elif (len(openinghours) > 4):
                # some wired opening hours
                # try a fix:
                openinghours.sort()
                if openinghours[-1] > openinghours[2]:
                    openinghours[2] = openinghours[-1]
                    openinghours = openinghours[:4]
                else:
                    # give up an put default
                    openinghours = openinghours_default
            
            # update database    
            place['openinghours'] =  openinghours
            
            print ('    openinghours',openinghours)
            
            if 'userRatingCount' in data:
                place['ratingcount'] =  data['userRatingCount']
            else:
                place['ratingcount'] = -1
            
            if 'rating' in data:
                place['rating'] =  data['rating']
            else:
                place['rating'] = -1
                
    
class GooglePOIImporter(Process):
    def __init__(self,  scenario, logger = None, **kwargs):
        print ('GooglePOIImporter.__init__',scenario.landuse)
        self._init_common(  'googlepoiimporter', name = 'Google POI Importer', 
                            logger = logger,
                            info ='Downloads POI data through Google API.',
                            )
        
        self._scenario = scenario
        
        attrsman = self.set_attrsman(cm.Attrsman(self))


        self.is_source_from_file = attrsman.add(cm.AttrConf( 'is_source_from_file',kwargs.get('is_source_from_file',False),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Use place data from file',
                            info = """Use place data from a previously created file, the filepath must be given below. In this kase no key is required. 
                                      Otherwise, or if no file exists, place data are downloaded via Google API.
                                      """,
                            )) 
        
        rootname = scenario.get_rootfilename()
        rootdirpath = scenario.get_workdirpath()

        
        self.placesfilepath = attrsman.add(cm.AttrConf('placesfilepath',os.path.join(rootdirpath,rootname+'.poi.pkl'),
                        groupnames = ['options'],# 
                        perm='rw', 
                        name = 'Placedata file', 
                        wildcards = 'Pickle files (*.pkl)|*.pkl*',
                        metatype = 'filepath',
                        info = 'POI file to temporarily store downloaded information. This file can later be used to updayte POIs.',
                        ))
                                            
        self.apikey = attrsman.add(cm.AttrConf(  'apikey',kwargs.get('apikey',''),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'API key', 
                            info = 'API key for google maps. API key can be obtained from Google at https://cloud.google.com/maps-platform/?refresh=1&pli=1#get-started.',
                            ))
        

                            
        #self.coords_center = attrsman.add(cm.ListConf('coords_center',kwargs.get('bbox_approx',[0.0,0.0]), 
        #                                    groupnames = ['options'], 
        #                                    name = 'Center coordinates', 
        #                                    unit = "m",
        #                                    info = """All places within a circle are downloaded. This defines the center coordinates [x, y].""",
        #                                    ))
        
        
        
        self.bbox = attrsman.add(cm.ListConf('bbox',kwargs.get('bbox',self._scenario.net.get_boundaries(is_netboundaries = True)), 
                                            groupnames = ['options'], 
                                            name = 'Bounding box', 
                                            info = """Expicit bounding box of the format [x_min, y_min,x_max, y_max]. 
                                            Empty list means no explicit bounding borders are applied.
                                            Default is the boundary of the entire network.""",
                                            ))
                                            
        self.radius = attrsman.add(cm.AttrConf( 'radius',kwargs.get('radius',500),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Download radius',
                            unit = "m",
                            info = """Most important places within a circle are downloaded. 
                                        The smaller the circle the more places are downloaded. 
                                        This defines the radius of the circle.""",
                            )) 
        
        
        
        
        self.is_openinghours = attrsman.add(cm.AttrConf( 'is_openinghours',kwargs.get('is_openinghours',False),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Download opening hours',
                            info = """Download opening hours of places if available. This will trigger costs for > 1000 places.""",
                            )) 
  
  
        self.openinghours_default  = attrsman.add(cm.ListConf('openinghours_default',kwargs.get('openinghours_default',[8.,12.0,14.,17.]), 
                                            groupnames = ['options'], 
                                            name = 'Default opening hours', 
                                            unit = "h",
                                            info = """Default opening hours are used for all places if no opening hour data is provided.""",
                                            ))
                                                            
        self.day_of_week = attrsman.add(cm.AttrConf( 'day_of_week',kwargs.get('day_of_week',4),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Day of week',
                            info = """Day of week for which opening hours are recorded, where Sunday = 0, Monday = 1,....""",
                            )) 
                            
        self.is_update = attrsman.add(cm.AttrConf( 'is_update',kwargs.get('is_update',False),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Update existing',
                            info = """Update existing POIs with Google places data instead of creating new enries.""",
                            )) 
    
    def do(self):
        print ('GooglePOIImporter.do')
        scenario = self._scenario
        landuse = scenario.landuse
        
        pois = landuse.pois
        pois._init_attributes_extra()
        bbox = self.bbox #[x_min, y_min,x_max, y_max]
        net = scenario.net
        
        params_proj = net.get_projparams()
        self._proj = pyproj.Proj(str(params_proj))
        self._offset = net.get_offset()
            
        # temporary dictionary with all google place info
        # this dictionary will be saved in the file 
        places = {}
        
         
           
         
        print ('download?',not os.path.isfile(self.placesfilepath) ,(not self.is_source_from_file),((not os.path.isfile(self.placesfilepath)) | (not self.is_source_from_file)))
        if ((not os.path.isfile(self.placesfilepath)) | (not self.is_source_from_file)):
            
            # if there is existant poi file use it 
            # and enrich with newly downloaded places
            if os.path.isfile(self.placesfilepath):
                with open(self.placesfilepath, 'rb') as file:    
                    places = pickle.load(file)
                    print ('  loaded',len(places),'places')
                    file.close()
            
            print ('  Download place IDs and basic attributes...')
            d = np.sqrt(2)*self.radius
            
            xcoords = np.arange(bbox[0],bbox[2],d)
            ycoords = np.arange(bbox[1],bbox[3],d)
            n_coords = len(xcoords) * len(ycoords)
            n = 0
            for x in xcoords:
                for y in ycoords:
                    n += 1
                    print("-" * 40)
                    print ('Download progress: %d/%d %.2f%%, downloaded %d places'%(n,n_coords,float(n)/n_coords*100,len(places)))
                    print ('  coords',(x,y))
                    lon_center, lat_center = self._proj(x - self._offset[0], y - self._offset[1], inverse = True)
                    
                    
                    next_page_token = download_google_places((lat_center,lon_center),self.radius,self.apikey, places)
                    while (next_page_token is not None):
                        time.sleep(2) # this is necessary otherwise returns error
                        next_page_token = download_google_places((lat_center,lon_center),self.radius,self.apikey, places, next_page_token)
            
            print('  Save',len(places),'to',self.placesfilepath)
            with open(self.placesfilepath, 'wb') as file:
                pickle.dump(places, file)
            
                file.close()
                
            if self.is_openinghours:
                print ('  Download opening hours for',len(places),'places...')
                download_google_places_extra(self.apikey, places)
                
            
            
        if (os.path.isfile(self.placesfilepath) & (self.is_source_from_file)):
            print ('  Update POI database from ',self.placesfilepath)
            with open(self.placesfilepath, 'rb') as file:    
                places = pickle.load(file)
                print ('  loaded',len(places),'places')
                file.close()
                
                if self.is_openinghours & (len(places) > 0):
                    print ('  Download opening hours for',len(places),'places...')
                    download_google_places_extra(self.apikey, places, self.day_of_week, self.openinghours_default)
                    
                print('Save',len(places),'to',self.placesfilepath)
                with open(self.placesfilepath, 'wb') as file:
                    pickle.dump(places, file)
                
                    file.close()
                        
        n_places = len(places)
        if n_places > 0:    
            print ('  Import',n_places,'places into scenario.')
            #print ('places.keys()',places.keys())
            if self.is_update:
                ids_poi = pois.ids_sumo.get_ids_from_indices(list(places.keys()))
            else:    
                ids_poi = pois.add_pois(ids_sumo = list(places.keys()))
            
            for id_poi in ids_poi:
                
                id_place = pois.ids_sumo[id_poi]
                place = places[id_place]
                #print("-" * 40)
                #print ('  id_place',id_place)
                lat,lon  = place['latlon']
                x_abs, y_abs = self._proj(lon,lat)
                pois.coords[id_poi] = [x_abs+ self._offset[0], y_abs + self._offset[1], 0.0]
                
                pois.names[id_poi] = place['name']
                
                placetypes = place['types'].split(',')
                len_placetypes = len(placetypes)
                if len_placetypes >= 2:
                    if placetypes[1].strip() in ('point_of_interest','establishment'):
                        pois.osmkeys[id_poi] = placetypes[0].strip()
                    elif placetypes[0].strip() in ('point_of_interest','establishment'):
                        pois.osmkeys[id_poi] = placetypes[1].strip()
                    else:
                        pois.osmkeys[id_poi] = placetypes[1].strip()+'.'+placetypes[0].strip()
                        
                elif len_placetypes == 1:
                    pois.osmkeys[id_poi] = placetypes[0].strip()
                elif len_placetypes == 0:
                    pois.osmkeys[id_poi] = 'point_of_interest'
                
                pois.ids_landusetype[id_poi] = 2 # all commercial, this could be refined later from osmkeys
                
                if 'openinghours' in place: # extrta data available
                    if len(place['openinghours']) == 4: # this is to prevent eerror for an old bug
                        pois.openinghours[id_poi] = place['openinghours']
                    else:
                        pois.openinghours[id_poi] = self.openinghours_default
                        
                    pois.rating[id_poi] = place['rating']
                    pois.ratingcount[id_poi] = place['ratingcount']
        return True 
        
class SumoPoiCounter(handler.ContentHandler):
    """Counts POIs from poly.xml file into POI structure"""

    def __init__(self):
        self.n = 0
    
    def startElement(self, name, attrs):
        #print 'startElement',name,len(attrs)
        if name == 'poi':
            self.n += 1

class SumoPoiReader(handler.ContentHandler):
    """Reads points of interest from poly.xml file into POIs structure"""

    def __init__(self, pois, counter, offset_delta, height_default=7.0, type_default=''):
        
        self._type_default = type_default
        self._pois = pois
        self._ids_landusetype_all = self._pois.get_landusetypes().get_ids()
        self._poifilters = self._pois.get_landusetypes().poifilters
        
        self._ind = -1
        self.ids_sumo = np.zeros(counter.n,np.object_)
        self.ids_landusetype = -1*np.ones(counter.n,np.int32) 
        self.osmkeys = np.zeros(counter.n,np.object_)
        self.names = np.zeros(counter.n,np.object_)
        self.coords = np.zeros((counter.n,3),np.float32) 
                          
        
        #self._id_facility = None
        self._offset_delta = offset_delta
            
    def startElement(self, name, attrs):
        
        #print 'startElement', name, len(attrs)
        if name == 'poi':
            self._ind += 1
            i = self._ind
            
            osmkey = attrs.get('type',self._type_default)
            #print '  id_sumo=',attrs['id'],osmkey
            id_landuse = self.get_landuse(osmkey)
            if id_landuse>=0: # land use is interesting
                self.ids_sumo[i] = attrs['id']
                self.ids_landusetype[i] = id_landuse
                self.osmkeys[i] = osmkey
                self.coords[i] =  [float(attrs['x']),float(attrs['y']),0.0]
                self.names[i] = ''
                
                #print '    control id_sumo',self.ids_sumo[i]
                
        elif name == 'param':
            i = self._ind
            if attrs['key'] == 'name':
                self.names[i] = attrs['value']
                                    
            # color info in this file no longer used as it is defined in
            # facility types table
            #color = np.array(xm.parse_color(attrs['color']))*0.8,# make em darker!! 
    
    def get_landuse(self, osmkey):
        keyvec = osmkey.split('.')
        len_keyvec=len(keyvec)
        #print 'get_landuse',len_keyvec,keyvec
        #is_match = False
        for id_landusetype in self._ids_landusetype_all:
            #print '    id_landusetype',id_landusetype
            # first filter only exact matches before wildcards
            for osmfilter in self._poifilters[id_landusetype]:
                #print '       ?osmfiltervec',osmfilter,osmkey==osmfilter
                if osmkey==osmfilter: # exact match of filter
                    #print '      exact',osmkey
                    return id_landusetype
        
        # now check for wildcards     
        for id_landusetype in self._ids_landusetype_all:
            #print '    *id_landusetype',id_landusetype
                       
            for osmfilter in self._poifilters[id_landusetype]:
                osmfiltervec = osmfilter.split('.')
                #print '       ?osmfiltervec',osmfiltervec,(len(osmfiltervec)==2)&(len_keyvec==2)
                if (len(osmfiltervec)==2)&(len_keyvec==2):    
                    if osmfiltervec[0]==keyvec[0]:
                        if osmfiltervec[1]=='*':
                            #print '      *',keyvec[0]
                            return id_landusetype
        return -1
                                        
    def finish(self):
        
        inds_valid = np.flatnonzero(self.ids_landusetype>=0)
        ids_fac = self._pois.add_pois(
                        ids_sumo = self.ids_sumo[inds_valid],
                        ids_landusetype = self.ids_landusetype[inds_valid] ,
                        osmkeys = self.osmkeys[inds_valid],
                        coords = self.coords[inds_valid], 
                        names = self.names[inds_valid]
                    )
                    
class Zones(am.ArrayObjman):
    def __init__(self, parent, **kwargs):
        self._init_objman(  ident='zones', parent=parent, 
                            name = 'Zones', 
                            info = 'Traffic Zones which can be used for zone-to-zone traffic transport demand or to specify zones for traffic evaporation.',
                            is_plugin = True,
                            version = 0.4,
                            **kwargs)
        
        self._init_attributes()
        self._init_constants()
        
        
    def _init_attributes(self):
        #print 'Zones._init_attributes',hasattr(self,'are_evaporate'),'version',self.get_version()
        edges = self.parent.get_net().edges
        self.add_col(SumoIdsConf('Zone', name = 'Name', perm = 'rw', info = 'Unique short name or ID to identify zone.'))     
        

                                                                                                                                
        self.add_col(am.ArrayConf( 'names_extended', '',
                                        dtype = np.object_,
                                        groupnames = ['parameter'], 
                                        perm='rw', 
                                        name = 'Extended name',
                                        info = 'Extended, human readable name, no uniqueness required, not used as key.',
                                        ))
                                        
        self.add_col(am.IdsArrayConf( 'ids_landusetype',  self.parent.landusetypes,
                                        id_default = 6,
                                        #choices = self.parent.landusetypes.typekeys.get_indexmap(),
                                        #choiceattrname = 'typekeys',
                                        groupnames = ['state'],
                                        perm='rw', 
                                        name = 'Type',
                                        info = 'Zone type. This is actually the landuse type.',
                                        ))
                                        
        self.add_col(am.ArrayConf( 'coords',  np.zeros(3, dtype=np.float32),
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
        
        if self.get_version()<0.4:
            # the attribute areas have been configured with a 
            # ListArrayConfing stead of an ArrayConf, 
            # delete old attr
            areas_old = self.areas.get_value()
            self.delete('areas')
            
        self.add_col(am.ArrayConf( 'areas', 0.0,
                                        groupnames = ['state'], 
                                        perm='rw', 
                                        name = 'Area',
                                        unit = 'km^2',
                                        info = 'Area of the zone in square kilometers.',
                                        is_plugin = True,
                                        ))
                                              
        
        if self.get_version()<0.4:
            # the attribute areas have been configured with a 
            # ListArrayConfing stead of an ArrayConf, change this
            # set old value to new array
            self.areas.set_value(areas_old)    

        
        self.add_col(am.IdlistsArrayConf( 'ids_edges_inside', edges,
                                            groupnames = ['state','_private'], 
                                            name = 'Edge IDs inside', 
                                            info = 'List with IDs of network edges that are entirely inside each zone.',   
                                            ))
                                            

        
        if self.get_version()<0.4:
            self.ids_edges_inside.add_groupnames(['_private'])
            

            
                
        self.set_version(0.4)                             
                                        
    def _init_constants(self):
        self._proj = None
        self._offset = None
        self._mode_to_edges_inside = {}
        attrsman = self.get_attrsman()
        attrsman.do_not_save_attrs(['_proj', '_offset','_mode_to_edges_inside'])  
        #self.set_version(0.2)
        
    def make(self, zonename = '', 
                coord = np.zeros(3, dtype=np.float32),  
                shape = [],
                id_landusetype = 6,# mixed landuse
                name_extended = '',
                id_zone = None,# some issues when predifining
                is_identify_zoneedges = True):
        """
        Add a zone
        """
        #print 'Zone.make',coord
        #print '  shape',type(shape),shape
        
        self.get_coords_from_shape(shape)
        id_zone = self.add_row( _id =id_zone, coords = self.get_coords_from_shape(shape), 
                                shapes =shape, areas = find_area(shape)/1000000. ,
                                ids_landusetype = id_landusetype,
                                names_extended = name_extended,
                                )
        if zonename == '':
            self.ids_sumo[id_zone] = str(id_zone)
        else:
            self.ids_sumo[id_zone] = zonename
            
        if is_identify_zoneedges:
            self.identify_zoneedges(id_zone)
        #print '   shapes\n',self.shapes.value
        #print '   zone.shapes[id_zone]\n',self.shapes[id_zone]

        return id_zone
            
    def format_ids(self,ids):
        return ','.join(self.ids_sumo[ids])
    
    def get_id_from_formatted(self,idstr):
        return self.ids_sumo.get_id_from_index(idstr)  
    
    def get_ids_from_formatted(self,idstrs):
        return self.ids_sumo.get_ids_from_indices_save(idstrs.split(','))
    
    
    def get_coords_from_shape(self, shape):
        #print 'get_coords_from_shape',np.array(shape),np.mean(np.array(shape),0)
        return np.mean(np.array(shape),0)
        
    def del_element(self, id_zone):
        #print 'del_element',id_zone
        self.del_row(id_zone)
    
    def get_edges(self):
        return self.ids_edges_inside.get_linktab()
    
    def update_zones(self):
        """Update all dependent attributes of all zones"""
        ids = self.get_ids()
        for id_zone, shape in zip(ids, self.shapes[ids]):
            self.coords[id_zone] = self.get_coords_from_shape(shape)
            self.areas[id_zone] = find_area(shape)/1000000.0
            self.identify_zoneedges(id_zone)
            
    def refresh_zoneedges(self):
        for _id in self.get_ids():
            self.identify_zoneedges(_id)
            #self.make_egdeprobs(_id)
            self.coords[_id] = self.get_coords_from_shape(self.shapes[_id])
            
    def refresh_zonearea(self):
        """
        Update the area of each zone
        """
        ids_zone = self.get_ids()
        for id_zone in ids_zone:
            self.identify_zonearea(id_zone)
    
    
    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        #self.zones.update_netoffset(deltaoffset)
        self.coords.value[:,:2]= self.coords.value[:,:2] + deltaoffset
        shapes = self.shapes.value
        for i in range(len(shapes)):
            s = np.array(shapes[i])
            s[:,:2] = s[:,:2] + deltaoffset
            shapes[i] = list(s)
            
    def identify_zonearea(self,id_zone):
        #print 'identify_zonearea',id_zone
        shape = self.shapes[id_zone]
        self.areas[id_zone] = find_area(shape)/1000000.0 
        
    def identify_zoneedges(self, id_zone):
        print('identify_zoneedges of id_zone',id_zone)
        
        if len(self.shapes[id_zone])>=3:
            # must have at least 3 vertices to be an area
            
            inds_within = []
            ind = 0
            #print '  self.shapes[id_zone]',self.shapes[id_zone]
            if not IS_SHAPELY:
                polygon = np.array(self.shapes[id_zone])[:,:2]
                #print '  polygon',type(polygon)
                for polyline in self.get_edges().shapes.get_value():
                    
                    #print '    np.array(polyline)[:,:2]',np.array(polyline)[:,:2],type(np.array(polyline)[:,:2])
                    if is_polyline_in_polygon(np.array(polyline)[:,:2],polygon):
                        inds_within.append(ind)
                    ind +=1
            else:
                polygon = Polygon(np.array(self.shapes[id_zone])[:,:2])
                for polyline in self.get_edges().shapes.get_value():
                    if MultiPoint(np.array(polyline)[:,:2]).within(polygon):
                        inds_within.append(ind)
                    ind +=1
                
            #print '  inds_within',inds_within
            
            # select and determine weights
            self.ids_edges_inside[id_zone] = self.get_edges().get_ids(inds_within)    
            print('  found',len(self.ids_edges_inside[id_zone]),'edges')
            if len(self.ids_edges_inside[id_zone]) == 0:
                print('WARNING in identify_zoneedges: no edges found in zone',id_zone)
    
    def get_zoneedges_by_mode_fast(self, id_zone, id_mode, speed_max =None, 
                                modeconst_excl = 0.0, modeconst_mix = 0.0,
                                weights = None, fstar = None):
        """
        Returns a list of edge IDs which are connected and accessible by mode id_mode.
        Uses cashed results, if possible.
        
        Optionally weights and fstar can be provided to improve computational speed.
        Otherwise weights and fstar are calculated with the optional parameters
        modeconst_excl and modeconst_mix
        """
        
        #print 'get_zoneedges_by_mode_fast id_zone',id_zone,'id_mode',id_mode
        if id_mode not in self._mode_to_edges_inside:
            self._mode_to_edges_inside[id_mode] = {}

        if id_zone not in self._mode_to_edges_inside[id_mode]:
            self._mode_to_edges_inside[id_mode][id_zone] = self.get_zoneedges_by_mode(id_zone, id_mode,weights = weights, fstar = fstar)
            #print '  found edges',len(self._mode_to_edges_inside[id_mode][id_zone])
        return self._mode_to_edges_inside[id_mode][id_zone]
                
    def get_zoneedges_by_mode(  self, id_zone, id_mode, speed_max =None, 
                                modeconst_excl = 0.0, modeconst_mix = 0.0,
                                weights = None, fstar = None):
        """
        Returns a list of edge IDs which are connected and accessible by mode id_mode.
        The algorithm tries to maximize the number of reachabe edges.
        
        Optionally weights and fstar can be provided to improve computational speed.
        Otherwise weights and fstar are calculated with the optional parameters
        modeconst_excl and modeconst_mix
        """
        #print 'get_zoneedges_by_mode id_zone',id_zone,'id_mode',id_mode

            
        return self.get_edges().get_max_connected(
                                ids_edge = self.ids_edges_inside[id_zone], 
                                id_mode = id_mode, speed_max =speed_max, 
                                modeconst_excl = modeconst_excl, 
                                modeconst_mix = modeconst_mix,
                                weights = weights, fstar = fstar,
                                is_bidir = True)

        
    def get_egdeprobs(self, id_zone, n_edges_min_length, n_edges_max_length, priority_max, speed_max, is_normalize = True, is_dict = False):
        """
        OBSOLETE!!!
        Returns vectors ids_edge and edge_props
        where ids_edge are the edge IDs of edges in id_zone and edge_props are 
        the respective probabilities to start/end a trip
        
        if is_dict is True then a dictionary is returnd with edge IDs as key 
        and arrival/departure probability as value
        """
        # todo: rename ids_edges_orig to simply ids_edges
        ids_edge_orig =self.ids_edges_orig[id_zone]
        if ids_edge_orig is None:
            print('WARNING: no edges in zone',id_zone,'. Run edge detection first.')
            if is_dict:
                return {}
            else:
                return [],[]
                
        n_edges_orig = len(ids_edge_orig)
        
        if n_edges_orig>0:
            weights = self.get_edgeweights(ids_edge_orig, n_edges_min_length, n_edges_max_length, priority_max, speed_max)
            if is_normalize:
                weights = weights/np.sum(weights)
            if is_dict:
                probs = {}
                for id_edge, prob in zip(ids_edge_orig, weights):
                    probs[id_edge] = prob
                return probs
            else:
                return ids_edge_orig, weights
        else:
            if is_dict:
                return {}
            else:
                return [],[]
              
    def make_egdeprobs(self, id_zone, n_edges_min_length, n_edges_max_length, priority_max, speed_max,  is_normalize = True):
        """
        OBSOLETE!!! This funzione has moved to origin_to_destination
        Update  probs_edges_orig and probs_edges_dest distribution 
        one for departures and one for arrivals.
        """
        
        
        #print 'make_egdeprobs',id_zone
        #zones = self.zones.value
        #edgeweights_orig = {}
        #edgeweights_dest = {}
        
        #for id_zone in zones.get_ids():
        ids_edge_orig =self.ids_edges_orig[id_zone]
        ids_edge_dest =self.ids_edges_dest[id_zone]
        n_edges_orig = len(ids_edge_orig)
        n_edges_dest = len(ids_edge_dest)
        # da fare meglio...
        if n_edges_orig>0:
            #self.probs_edges_orig[id_zone] = 1.0/float(n_edges_orig)*np.ones(n_edges_orig,np.float)
            weights = self.get_edgeweights(ids_edge_orig, n_edges_min_length, n_edges_max_length, priority_max, speed_max)
            if is_normalize:
                self.probs_edges_orig[id_zone] = weights/np.sum(weights)
            else:
                self.probs_edges_orig[id_zone] = weights
        else:
            self.probs_edges_orig[id_zone] = []
            
        if n_edges_dest>0:
            #self.probs_edges_dest[id_zone] = 1.0/float(n_edges_dest)*np.ones(n_edges_dest,np.float)
            weights = self.get_edgeweights(ids_edge_dest, n_edges_min_length, n_edges_max_length, priority_max, speed_max)
            if is_normalize:
                self.probs_edges_dest[id_zone] = weights/np.sum(weights)
            else:
                self.probs_edges_dest[id_zone] = weights
        else:
            self.probs_edges_dest[id_zone] = []  
    
    def get_edgeweights(self, ids_edge, n_edges_min_length, n_edges_max_length, priority_max, speed_max):
        #OBSOLETE!!! This funzione has moved to origin_to_destination
        #print 'get_edgeweights ids_edge',ids_edge
        edges = self.get_edges()
        n_edges = len(ids_edge)
        if (n_edges > n_edges_min_length)&(n_edges < n_edges_max_length):
            return edges.lengths[ids_edge]*((edges.priorities[ids_edge]<priority_max) & (edges.speeds_max[ids_edge] < speed_max))
        else:
            return np.ones(n_edges,dtype = np.float32)*((edges.priorities[ids_edge]<priority_max) & (edges.speeds_max[ids_edge] < speed_max))
    
    def write_kml(self, fd=None, indent = 0):
 
        ids_zone = self.get_ids()
        for id_zone in ids_zone:
            fd.write(xm.begin('Placemark',indent + 2))
            fd.write((indent + 4)*' '+'<name>%s</name>\n'%self.names_extended[id_zone])
            fd.write(xm.begin('LineString',indent + 4)) 
            fd.write(xm.begin('coordinates',indent + 6)) 
            
            for point in self.shapes[id_zone]:
                
                projection = self.project(point[0],point[1])
                fd.write((indent+8)*' '+'%f,%f\n'%(projection[0],projection[1]))
    
            fd.write(xm.end('coordinates',indent + 6))
            fd.write(xm.end('LineString',indent + 4))
            fd.write(xm.end('Placemark',indent + 2))

    def write_xml(self, fd=None, indent = 0, zoneinfo = None):
        print('Zones.write_xml')
        net = self.parent.parent.net
        ids_edge_sumo = net.edges.ids_sumo
        ids_zone = self.get_ids()
        fd.write(xm.begin('tazs',indent))
        for id_zone, id_zone_sumo, shape, ids_edges in zip(\
                ids_zone, self.ids_sumo[ids_zone], self.shapes[ids_zone], 
                self.ids_edges_inside[ids_zone]):
            
            if ids_edges is not None:
                if len(ids_edges) > 0:
                    #print '  id_zone_sumo',id_zone_sumo,'id_edges_orig',id_edges_orig,'probs_edge_orig',probs_edge_orig
                    #    <taz id="<TAZ_ID>">
                    #      <tazSource id="<EDGE_ID>" weight="<PROBABILITY_TO_USE>"/>
                    #      ... further source edges ...
                    #
                    #      <tazSink id="<EDGE_ID>" weight="<PROBABILITY_TO_USE>"/>
                    #      ... further destination edges ...
                    #   </taz>
           
                    fd.write(xm.start('taz',indent + 2))
                    fd.write(xm.num('id',id_zone_sumo))
                    fd.write(xm.mat('shape',shape))
                    fd.write(xm.stop())
                    if zoneinfo is not None:
                        for id_edge_sumo, prob_edge in zip(ids_edge_sumo[zoneinfo[id_zone][0]], zoneinfo[id_zone][1]):
                           fd.write(xm.start('tazSource',indent + 4))
                           fd.write(xm.num('id',id_edge_sumo))
                           fd.write(xm.num('weight',prob_edge))
                           fd.write(xm.stopit())
                        #
                        for id_edge_sumo, prob_edge in zip(ids_edge_sumo[zoneinfo[id_zone][0]], zoneinfo[id_zone][1]):
                           fd.write(xm.start('tazSink',indent + 4))
                           fd.write(xm.num('id',id_edge_sumo))
                           fd.write(xm.num('weight',prob_edge))
                           fd.write(xm.stopit())    
                        
                    
                    fd.write(xm.end('taz',indent + 2))
            
        fd.write(xm.end('tazs',indent))
            

            
    def project(self, x,y):
        if self._proj is None:
            self._proj, self._offset = self.get_proj_and_offset()
        lons, lats = self._proj(x-self._offset[0], y-self._offset[1], inverse = True)
        return np.transpose(np.concatenate(([lons],[lats]),axis=0))
        
    def get_proj_and_offset(self):
        if self._proj is None:
            net = self.parent.parent.net
            proj_params = str(net.get_projparams())
            #try:
            self._proj = pyproj.Proj(proj_params)
            #except:
            #    proj_params ="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
            #    self._proj = pyproj.Proj(self.proj_params)  
            
            self._offset = net.get_offset()
                    
            return self._proj, self._offset 
        
    def get_zonefilepath(self):
        return self.parent.parent.get_rootfilepath()+'.taz'
    
    def export_kml(self, filepath=None, encoding = 'UTF-8'):
        """
        Export zones to Google kml file formate.
        """
        print('export_sumoxml',filepath,len(self))
        if len(self)==0: return None
        
        if filepath  is None:
            filepath = self.get_zonefilepath()+'.kml'
        
        try:
            fd=open(filepath,'w', encoding="utf-8")
        except Exception:
            print('WARNING in write_obj_to_xml: could not open',filepath)
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
        fd.write('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
        indent = 0
        #fd.write(xm.begin('routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"',indent))

    
        fd.write(xm.begin('Document',indent = 0))
        
        self.write_kml(fd,indent = 0)
        
        fd.write(xm.end('Document',indent = 0))
        fd.write(xm.end('kml',indent = 0))
        fd.close() 
        return filepath

    def export_sumoxml(self, filepath=None, encoding = 'UTF-8', zoneinfo = None):
        """
        Export zones to SUMO xml file formate.
        """
        print('export_sumoxml',filepath,len(self))
        if len(self)==0: return None
        
        if filepath  is None:
            filepath = self.get_zonefilepath()+'.xml'
        
        try:
            fd=open(filepath,'w', encoding="utf-8")
        except Exception:
            print('WARNING in export_sumoxml: could not open',filepath)
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        #fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
        #fd.write('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
        indent = 0
        #fd.write(xm.begin('routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"',indent))

    
        
        
        self.write_xml(fd,indent = 0, zoneinfo = zoneinfo)
        
        
        fd.close() 
        return filepath

class ZonesFromShapeImporter(ShapefileImporter):
    def __init__(self, ident='zonesfromshapeimporter', zones = None, 
                    name = 'Zones from Shapefile importer',
                    shapeattr_ids_zone = 'ID',
                    shapeattr_names_zone = '',
                    shapeattr_names_landuse = '',
                    filepath = '',logger=None, 
                    boundaries =  None,
                    **kwargs):
        
        net = zones.parent.parent.net
        
        if boundaries is None:
            boundaries = net.get_boundaries(is_netboundaries = True)
                                                        
        ShapefileImporter.__init__( self, ident, parent = zones,
                                    name = name,
                                    filepath = filepath, 
                                    coordsattr = 'shapes', 
                                    attrnames_to_shapeattrs = { 'ids_sumo': shapeattr_ids_zone,
                                                                'names_extended': shapeattr_names_zone,
                                                                'ids_landusetype':shapeattr_names_landuse},
                                    projparams_target = net.get_projparams(),
                                    offset = net.get_offset(),
                                    boundaries =  boundaries,
                                    logger = logger,
                                    **kwargs)
        attrsman = self.get_attrsman()                            
        self.is_update_zones = attrsman.add(cm.AttrConf(  'is_update_zones', kwargs.get("is_update_zones",True),
                                                        groupnames = ['options',], 
                                                        perm='rw', 
                                                        name = 'Update Zones?', 
                                                        info = 'Update zones: identify zone edges, calculate area, etc.',
                                                        ))
        
        
    def do(self):
        if ShapefileImporter.do(self):
            print('ZonesFromShapeImporter: check shapes:')
            for shape in self.parent.shapes.get_value():
                print('  shape',shape)
            if self.is_update_zones:
                # update zones
                self.parent.update_zones()
            return True
        
        else:
            return False
   
    
class FacilityTypeMixin(cm.BaseObjman):
        def __init__(   self, ident, parent,
                        name = 'Facility Type Mixin', 
                        info ='Provides methods to handle specific facility functions.',
                        **kwargs):
            """
            To be overridden.
            """
            # attention parent is the Strategies table
            self._init_objman(ident, parent, **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            self._init_attributes()
            self._init_constants()
        
        def _init_attributes(self):
            self._init_attributes_common()
        
        def _init_attributes_common(self):
            #print 'StrategyMixin._init_attributes'
            attrsman = self.get_attrsman()
            landusetypes = self.get_landuse().landusetypes
            self.ids_landusetype = attrsman.add(cm.AttrConf(  'ids_landusetype', 
                                    [landusetypes.get_id_from_formatted('residential'),],
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    #choices = landusetypes.typekeys.get_indexmap(),
                                    name = 'landuse', 
                                    info = 'Default landuse type of this facility.',
                                    ))
                                    
  
            
            self.osmkey = attrsman.add(cm.AttrConf(  'osmkey', 'building.yes',
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'OSM key', 
                                    info = 'Default Open Street Map key for this facility.',
                                    ))
                                    
            self.length_min = attrsman.add(cm.AttrConf(  'length_min', 15.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Min. length', 
                                    unit = 'm',
                                    info = 'Minimum length of entire property.',
                                    ))
                                    
            self.length_max = attrsman.add(cm.AttrConf(  'length_max', 100.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Max. length', 
                                    unit = 'm',
                                    info = 'Maximum length of entire property.',
                                    ))
            self.width_min = attrsman.add(cm.AttrConf(  'width_min', 20.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Min. width',
                                    unit = 'm', 
                                    info = 'Minimum width of entire property.',
                                    ))
                                    
            self.width_max = attrsman.add(cm.AttrConf(  'width_max', 80.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Max. width', 
                                    unit = 'm',
                                    info = 'Maximum width of entire property.',
                                    ))
            
            self.height_min = attrsman.add(cm.AttrConf(  'height_min', 15.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Min. height',
                                    unit = 'm', 
                                    info = 'Minimum height of facility.',
                                    ))
                                    
            self.height_max = attrsman.add(cm.AttrConf(  'height_max', 35.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Max. height', 
                                    unit = 'm',
                                    info = 'Maximum height of facility.',
                                    ))
                                                            
            self.dist_road_min = attrsman.add(cm.AttrConf(  'dist_road_min', 1.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Min. dist road',
                                    unit = 'm', 
                                    info = 'Minimum distance from road.',
                                    ))
                                    
            
            self.dist_road_max = attrsman.add(cm.AttrConf(  'dist_road_max', 5.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Max. dist road', 
                                    info = 'Maximum distance from road.',
                                    ))
            
            
            self.dist_prop_min = attrsman.add(cm.AttrConf(  'dist_prop_min', 1.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Min. prop dist',
                                    unit = 'm', 
                                    info = 'Minimum distance to other properties.',
                                    ))
                                    
            
            self.dist_prop_max = attrsman.add(cm.AttrConf(  'dist_prop_max', 4.0,
                                    groupnames = ['parameters'], 
                                    perm='rw', 
                                    name = 'Max. prop dist', 
                                    info = 'Maximum distance to other properties.',
                                    ))
                                                                                    
            self.shape_default = attrsman.add(cm.AttrConf(  'shape_default', [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [1.0, 1.0, 0.0],[0.0,1.0, 0.0] ],
                                    groupnames = ['parameters'], 
                                    perm='r', 
                                    name = 'Defaut shape', 
                                    info = 'Basic facility shape.',
                                    ))
             
            self.unitvolume_default = attrsman.add(cm.AttrConf(  'unitvolume_default', 
                                    default = 100.0,
                                    dtype = np.float32,
                                    perm = 'r', 
                                    name = 'Unit volume',
                                    info = 'Default value of the volume necessary to store one person or container. Volume used to calculate capacity.',
                                    ))                        
        def _init_constants(self):
            
            #self.get_attrsman().do_not_save_attrs([
            #                '_id_mode_bike','_id_mode_auto','_id_mode_moto',
            #                        ])        
            pass
    
        def get_id_type(self):
            #print 'get_id_type from ',self.parent.get_ident_abs()
            #print '  names',self.parent.names.get_value()
            return self.parent.names.get_id_from_index(self.get_ident())
            
        def get_facilities(self):
            return self.parent.parent
        
        def get_landuse(self):
            return self.parent.parent.parent
        
        def get_scenario(self):
            return self.parent.parent.get_scenario()
        
        def get_shape0(self, length_fac, width_fac, height_max, capacity):
            """
            Returns facility shape in origin coordinate system
            and height as a function of the capacity and available space
            """
            # One could do some random operations on the default shape
            # here just stretch default shape to fit area
            shape = np.array(self.shape_default, dtype = np.float32)* [length_fac, width_fac, 0.0]
            # y-axes must be flipped so that hoses grow
            # to the right side of the road
            shape[:,1] *= -1 
            #Calculates height of the building in function of the 
            #required capacity and available space.
            
            # here just use a random number
            # to be overridden
            height = min(random.uniform(self.height_min, self.height_max),height_max) 
            
            
            return shape, height
        
        
        def generate(self,  offset = [0.0,0.0,0.0],
                            length = 10.0,
                            width = 10.0,
                            angle = 0.0,
                            pos_edge =  0.0,
                            capacity = None,
                            id_landusetype = None,
                            height_max = 30.0,
                            id_edge = None,
                            width_edge = 3.0,
                            ):
            n_shape = len(self.shape_default)
            shape_fac = np.zeros((n_shape,3), dtype = np.float32)
            
            
            
            # determine effecive land area        
            dist_edge = random.uniform(self.dist_road_min, self.dist_road_max) + width_edge
            width_fac = width-dist_edge
            
            dist_prop = random.uniform(self.dist_prop_min, self.dist_prop_max)
            length_fac = length-2*self.dist_prop_max
            
            # do offset
            dxn = np.cos(angle-np.pi/2)
            dyn = np.sin(angle-np.pi/2)
            
            offset_fac = offset\
                        + np.array([dxn,dyn,0.0],dtype = np.float32)*dist_edge\
                        + np.array([dyn,dxn,0.0],dtype = np.float32)*dist_prop
            
            
            
            shape, height_fac = self.get_shape0(length_fac, width_fac, height_max, capacity)
            
            # transform in to the right place
            shape_fac[:,0] = shape[:,0]*np.cos(angle) - shape[:,1]*np.sin(angle)
            shape_fac[:,1] = shape[:,0]*np.sin(angle) + shape[:,1]*np.cos(angle)
            shape_fac += offset_fac 
            
            if id_landusetype is None:
                id_landusetype = self.ids_landusetype[0]
                
            id_fac = self.get_facilities().make(   id_landusetype = id_landusetype,
                                                        id_zone = None,
                                                        id_facilitytype = self.get_id_type(),
                                                        osmkey = self.osmkey,
                                                        area = None,
                                                        height = height_fac, 
                                                        centroid = None,
                                                        shape =  list(shape_fac),
                                                        id_roadedge_closest = id_edge,
                                                        position_roadedge_closest = pos_edge + 0.5* length,
                                                        )
        
        
class FacilityTypeHouse(FacilityTypeMixin):
        def __init__(   self, ident, parent,
                        name = 'Single House', 
                        info ='Parameters and methods for residential house.',
                        **kwargs):
            
            self._init_objman(ident, parent, name = name, info = info, **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            
            self._init_attributes()
            self._init_constants()
            # specific init
        
        
              
class FacilityTypes(am.ArrayObjman):
    def __init__(self,ident, facilities,is_add_default = True, **kwargs):
        self._init_objman(  ident= ident, 
                            parent = facilities, 
                            name = 'Facility types', 
                            info = 'Table holding facility type specific parameters and an object with methods ', 
                            **kwargs)
        
        self._init_attributes()
        if is_add_default:
            self.add_default()
                                
    def _init_attributes(self):     
        # landuse types table    
        self.add_col(am.ArrayConf(  'names', 
                                    default = '',
                                    dtype = 'object',
                                    perm = 'r', 
                                    is_index = True,
                                    name = 'Short name',
                                    info = 'Strategy name. Must be unique, used as index.',
                                    ))
        
        self.add_col(am.ArrayConf(  'unitvolumes', 
                                    default = 100.0,
                                    dtype = np.float32,
                                    perm = 'rw', 
                                    name = 'Unit volume',
                                    info = 'The volume necessary to store one person or container. Volume used to calculate capacity.',
                                    ))
                                                                
        self.add_col(cm.ObjsConf( 'typeobjects', 
                                    #groupnames = ['state'], 
                                    name = 'Type objects', 
                                    info = 'Facility type object.',
                                    )) 
       
        self.add_default()
        
        
    def format_ids(self,ids):
        return ','.join(self.names[ids])
    
    def get_id_from_formatted(self,idstr):
        return self.names.get_id_from_index(idstr)  
    
    def get_ids_from_formatted(self,idstrs):
        return self.names.get_ids_from_indices_save(idstrs.split(','))
        
    def add_default(self):
        self.clear()
        self.add_type('house',FacilityTypeHouse)
        
    def get_typeobj(self, id_type):
        return self.typeobjects[id_type]
            
    def add_type(self, ident, TypeClass, **kwargs):
        #print 'add_strategy', ident
        if not self.names.has_index(ident):
            factypeobj = TypeClass(ident, self)
            id_type = self.add_row(   names = ident,
                            typeobjects = factypeobj,
                            unitvolumes = factypeobj.unitvolume_default
                            )   
            return  id_type 
        else:
            return  self.get_id_from_formatted(id_type)   
              
class Facilities(am.ArrayObjman):
    def __init__(self, landuse, landusetypes, zones, net = None, **kwargs):
        #print 'Facilities.__init__',hasattr(self,'lanes')
        self._init_objman(  ident='facilities', 
                            parent=landuse, 
                            name = 'Facilities', 
                            info = 'Information on buildings, their type of usage and access to the transport network.', 
                            xmltag = ('polys','poly','ids_sumo'),
                            is_plugin = True,
                            **kwargs)
        
        self._init_attributes()
        self._init_constants()
        
        if net  is not None:
            self.add_col(am.IdsArrayConf( 'ids_roadedge_closest', net.edges, 
                                                groupnames = ['landuse'], 
                                                name = 'Road edge ID', 
                                                info = 'ID of road edge which is closest to this facility.',
                                                ))
                                                                                                                
            
            self.add_col(am.ArrayConf( 'positions_roadedge_closest', 0.0,
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
    def _init_constants(self):
        self._proj = None
        self._offset = None

        attrsman = self.get_attrsman()
        attrsman.do_not_save_attrs(['_proj', '_offset'])     
        
    def _init_attributes(self):
        landusetypes  = self.parent.landusetypes 
        zones =   self.parent.zones
        self.add(cm.ObjConf(FacilityTypes('facilitytypes', self,) ))
                        
        self.add_col(SumoIdsConf('Facility',info = 'SUMO facility ID'))
        
        self.add_col(am.ArrayConf( 'names', '',
                                        dtype = 'object',# actually a string of variable length
                                        perm='rw', 
                                        name = 'Name',
                                        info = 'Name of facility.',
                                        is_plugin = True,
                                        ))
                                        
        self.add_col(am.IdsArrayConf( 'ids_landusetype', landusetypes, 
                                        groupnames = ['landuse'], 
                                        perm='rw', 
                                        name = 'ID landuse', 
                                        info = 'ID of landuse.',
                                        is_plugin = True,
                                        ))
                                            
        
                                        
        self.add_col(am.IdsArrayConf( 'ids_zone', zones, 
                                        groupnames = ['landuse'], 
                                        perm='r', 
                                        name = 'ID zone', 
                                        info = 'ID of traffic zone, where this facility is located.',
                                        ))
        facilitytypes = self.get_facilitytypes()
        if len(facilitytypes)>0:                               
            id_default = facilitytypes.get_ids()[0]
        else:
            id_default = -1
        self.add_col(am.IdsArrayConf( 'ids_facilitytype', facilitytypes, 
                                        id_default = id_default,
                                        groupnames = ['landuse'], 
                                        perm='rw', 
                                        name = 'ID fac. type', 
                                        info = 'ID of facility type (house, scycraper, factory, parking,...).',
                                        is_plugin = True,
                                        ))
                                        
        self.add_col(am.ArrayConf( 'osmkeys', 'building.yes',
                                        dtype = 'object',# actually a string of variable length
                                        perm='rw', 
                                        name = 'OSM key',
                                        info = 'OSM key of facility.',
                                        xmltag = 'type',
                                        is_plugin = True,
                                        ))
                                    
        self.add_col(am.ArrayConf( 'capacities', 0.,
                                        dtype=np.int32,
                                        groupnames = ['landuse'], 
                                        perm='r', 
                                        name = 'Capacity',
                                        unit = 'People',
                                        info = 'Person capacity for this facility.',
                                        ))
                                    
        # ~ self.add_col(am.ArrayConf( 'occupancies', 0.,
                                        # ~ dtype=np.int32,
                                        # ~ groupnames = ['landuse'], 
                                        # ~ perm='r', 
                                        # ~ name = 'Occupancy',
                                        # ~ unit = 'People',
                                        # ~ info = 'Maximum occupancy for this facility.',
                                        # ~ ))
                                        
                                        
        self.add_col(am.ArrayConf( 'areas', 0.0,
                                        dtype=np.float32,
                                        groupnames = ['landuse'], 
                                        perm='r', 
                                        name = 'Area',
                                        unit = 'm^2',
                                        info = 'Area of this facility.',
                                        ))
                                        
        
        self.add_col(am.ArrayConf( 'heights', 10.0,
                                        dtype=np.float32,
                                        groupnames = ['landuse'], 
                                        perm='r', 
                                        name = 'Height',
                                        unit = 'm',
                                        info = 'Height above ground of this facility.',
                                        ))
                                        
        self.add_col(am.ArrayConf( 'centroids',  np.zeros(3, dtype=np.float32),
                                        dtype=np.float32,
                                        groupnames = ['state','_private'],
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
                                        xmltag = 'shape',
                                        is_plugin = True,
                                        ))
                                        
        
        #self.add_col(TabIdsArrayConf( 'ftypes',
        #                            name = 'Types',
        #                            info = 'Draw obj and ids',
        #                            ))
                                    
                                        
        if self.plugin is None:
            self.init_plugin(True)
            self.shapes.init_plugin(True)
            self.osmkeys.init_plugin(True)
            self.ids_landusetype.init_plugin(True)
        # configure only if net is initialized
        
    
    def make(self, id_sumo = None, 
                id_landusetype = None,
                id_zone = None,
                id_facilitytype = None,
                osmkey = None,
                area = None,
                capacity = None,
                height = None,
                centroid = None,
                shape =  [],
                id_roadedge_closest = None,
                position_roadedge_closest = None,
                ):
        """
        Adds a facilities 
        """
        id_fac = self.suggest_id()
        if id_sumo is None:
            id_sumo = str(id_fac)

       
        # stuff with landusetype must be done later
        id_fac = self.add_row(  _id = id_fac,
                                ids_sumo = id_sumo,
                                ids_landusetype = id_landusetype,
                                ids_zone = id_zone,
                                ids_facilitytype = id_facilitytype,
                                osmkeys = osmkey,
                                areas = area,
                                capacities = capacity,
                                heights = height,
                                centroids = centroid,
                                shapes =shape,
                                ids_roadedge_closest = id_roadedge_closest,
                                positions_roadedge_closest = position_roadedge_closest,
                                ) 
        # do area calcs and other
        if area is None:
            self.update_area(id_fac)
            
        if capacity is None:
             self.update_capacity(id_fac)
             
        if centroid is None:
            self.update_centroid(id_fac)           
        return  id_fac 
     
    
    
    def generate(self, facilitytype, **kwargs):
        """
        Generates a facility. The generation of the facility will be
        performed by the faciliy type instance.
        """
        return facilitytype.generate(**kwargs)
        
                                                            
        
    def format_ids(self,ids):
        return ','.join(self.ids_sumo[ids])
    
    def get_id_from_formatted(self,idstr):
        return self.ids_sumo.get_id_from_index(idstr)  
    
    def get_ids_from_formatted(self,idstrs):
        return self.ids_sumo.get_ids_from_indices_save(idstrs.split(','))
    
    def del_element(self, id_fac):
        #print 'del_element',id_zone
        self.del_row(id_fac)
    
    
    def write_xml(self, fd, indent=0, is_print_begin_end = True, delta = np.zeros(3,dtype=np.float32)):
        xmltag, xmltag_item, attrname_id = self.xmltag
        layer_default = -1
        fill_default = 0
        fill_area = 0
        ids_landusetype = self.ids_landusetype
        landusetypes = self.get_landusetypes()
        landusecolors = landusetypes.colors
        are_area = landusetypes.are_area
        if is_print_begin_end:
            fd.write(xm.begin(xmltag, indent))
        
        attrsconfigs_write = [self.ids_sumo, self.osmkeys]
        xmltag_shape = self.shapes.xmltag
        sep_shape = self.shapes.xmlsep
        for _id in self.get_ids():
            fd.write(xm.start(xmltag_item,indent+2))
            for attrsconfig in attrsconfigs_write:
                attrsconfig.write_xml(fd,_id)
            
            fd.write(xm.mat(xmltag_shape ,self.shapes[_id] - delta)) 
            
            landusecolors.write_xml(fd, ids_landusetype[_id])   
            fd.write(xm.num('layer',layer_default))
            
            if are_area[ids_landusetype[_id]]:
                fd.write(xm.num('fill',fill_area))
            else:
                fd.write(xm.num('fill',fill_default))
            
            fd.write(xm.stopit()) 
        
        
            
        if is_print_begin_end:
            fd.write(xm.end(xmltag, indent))
        
        
    def get_landusetypes(self):
        return self.ids_landusetype.get_linktab()  
     
    def get_facilitytypes(self):
        return self.facilitytypes.get_value()    
    
    def get_net(self):
        #print 'get_net',self.ids_edge_closest_road.get_linktab(),self.ids_edge_closest_road.get_linktab().parent
        return self.ids_roadedge_closest.get_linktab().parent

    
    def get_scenario(self):
        return self.ids_roadedge_closest.get_linktab().parent.parent
        
    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        #self.zones.update_netoffset(deltaoffset)
        self.centroids.value[:,:2]= self.centroids.value[:,:2] + deltaoffset
        shapes = self.shapes.value
        for i in range(len(shapes)):
            s = np.array(shapes[i])
            s[:,:2] = s[:,:2] + deltaoffset
            shapes[i] = list(s)
            
            
                
    def get_edges(self):
        return self.ids_roadedge_closest.get_linktab()
    
    def identify_taz(self):
        """
        Identifies id of traffic assignment zone for each facility.
        Note that not all facilities are within such a zone.
        """
        zones = self.ids_zone.get_linktab() 
        #self.get_demand().get_districts()
        for id_fac in self.get_ids():
            for id_zone in zones.get_ids():
                if is_polyline_in_polygon(self.shapes[id_fac],zones.shapes[id_zone]):
                    self.ids_zone[id_fac] = id_zone
                    break
    
    def get_departure_probabilities(self):
        """
        Returns a dictionary, where probabilities[id_zone]
        is a vector of departure probabilities for each facility
        of zone id_zone.
        """
        zones = self.ids_zone.get_linktab()
        #print 'get_departure_probabilities in n_zones',len(zones)
        probabilities={} 
        
        inds_fac =  self.get_inds()
        for id_zone in  zones.get_ids():
            #probabilities[id_zone]={}
            #for id_landusetype in  set(self.ids_landusetype.value):
            #print '  id_zone',id_zone
            #print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
            #print '  ids_zone',self.ids_zone.value[inds_fac]
            #print ''
            util = self.capacities.value[inds_fac].astype(np.float32)*(self.ids_zone.value[inds_fac]==id_zone)
            util_tot = np.sum(util)
            #print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
            #print '  self.type==ftype',self.type==ftype
            #print '  self.id_taz==id_taz',self.id_taz==id_taz
            #print '  util',util 
            if util_tot>0.0:
                probabilities[id_zone] = util/util_tot
            else:
                probabilities[id_zone]= util # all zero prob
             
        return  probabilities, self.get_ids(inds_fac)  
    
    def get_departure_probabilities_landuse2(self, ids_landusetype):
        """
        Returns a dictionary, where probabilities[id_zone]
        is a vector of departure probabilities for each facility
        of zone id_zone.
        Probabilities are proportional to the capacity attribute of the facility.
        
        Only facilities with one of the landuse given in ids_landusetype
        have non-zero probabilities.

        The ids_fac is an array that contains the facility ids in correspondence
        to the probability vector.
        """
        print('get_departure_probabilities_landuse2 ids_landusetype',ids_landusetype)
        probabilities={} 
        zones = self.ids_zone.get_linktab()
        inds_fac =  self.get_inds()
        n_frac = len(inds_fac)
        for id_zone in  zones.get_ids():
            #probabilities[id_zone]={}
            utils = np.zeros(n_frac, dtype = np.float32)
            util_tot = 0.0
            for id_landusetype in  ids_landusetype:
                #print '    id_zone,id_landusetype',id_zone,id_landusetype
                #print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
                #print '  ids_zone',self.ids_zone.value[inds_fac]
                #print ''
                utils_new = self.capacities.value[inds_fac].astype(np.float32)*np.logical_and((self.ids_landusetype.value[inds_fac]==id_landusetype),(self.ids_zone.value[inds_fac]==id_zone))
                utils += utils_new
                util_tot += np.sum(utils_new)
                #print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
                #print '  self.type==ftype',self.type==ftype
                #print '  self.id_taz==id_taz',self.id_taz==id_taz
                #print '      util',np.sum(utils)
            
            if util_tot>0.0:
                probabilities[id_zone] = utils/util_tot
            else:
                probabilities[id_zone] = utils # all zero prob
            
            if 0: # debug
                print('    sum(probs)',np.sum(probabilities[id_zone]))
                if np.sum(probabilities[id_zone])>0:
                    ids_fac = self.get_ids(inds_fac) 
                    for id_fac, id_landusetype, id_thiszone, prob in zip( ids_fac, self.ids_landusetype[ids_fac], self.ids_zone[ids_fac], probabilities[id_zone]):
                        if (id_thiszone == id_zone):# & (id_landusetype in ids_landusetype):
                            if prob > 0:
                                print('        id_fac',id_fac,'id_landusetype',id_landusetype,'prob',prob)
             
        return  probabilities, self.get_ids(inds_fac) 
                            
    def get_departure_probabilities_landuse(self):
        """
        Returns the dictionnary of dictionaries with departure (or arrival)
        probabilities where probabilities[id_zone][id_landusetype]
        is a probability distribution vector  giving for each facility the
        probability to depart/arrive in zone id_zone with facility type ftype.
        
        The ids_fac is an array that contains the facility ids in correspondence
        to the probability vector.
        """
        print('get_departure_probabilities_landuse')
        probabilities={} 
        zones = self.ids_zone.get_linktab()
        inds_fac =  self.get_inds()
        for id_zone in  zones.get_ids():
            probabilities[id_zone]={}
            for id_landusetype in  set(self.ids_landusetype.value):
                print('  id_zone,id_landusetype',id_zone,id_landusetype)
                #print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
                #print '  ids_zone',self.ids_zone.value[inds_fac]
                #print ''
                util = self.capacities.value[inds_fac].astype(np.float32)*((self.ids_landusetype.value[inds_fac]==id_landusetype)&(self.ids_zone.value[inds_fac]==id_zone))
                util_tot = np.sum(util)
                #print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
                #print '  self.type==ftype',self.type==ftype
                #print '  self.id_taz==id_taz',self.id_taz==id_taz
                #print '  util',util 
                if util_tot>0.0:
                    probabilities[id_zone][id_landusetype] = util/util_tot
                else:
                    probabilities[id_zone][id_landusetype] = util # all zero prob
             
        return  probabilities, self.get_ids(inds_fac)  
    
    def update(self, ids = None):
        #print 'update',ids
        if ids is None:
            ids = self.get_ids()
            
        for _id in ids:
            #print '  self.centroids[_id]',self.centroids[_id]
            #print '  self.shapes[_id]',self.shapes[_id],np.mean(self.shapes[_id],0)
            print('update centroids')
            self.update_centroid(_id)
            #self.areas[_id] = find_area(np.array(self.shapes[_id],float)[:,:2])

            print('update area')
            self.update_area(_id)
            #self.areas[_id] = get_polygonarea_fast(np.array(self.shapes[_id],float)[:,0], np.array(self.shapes[_id],float)[:,1])
        print('update landuse from areas')
        self.identify_landuse_from_area(ids)
        print('update capacities')
        self.update_capacities(ids)    
        #self.identify_closest_edge(ids)
    
    def get_ids_area(self, ids = None):
        if ids is None:
            ids = self.get_ids()
        return ids[self.get_landusetypes().are_area[self.ids_landusetype[ids]]]
    
    def get_ids_building(self, ids = None):
        """Returns all building type of facilities"""
        #print 'get_ids_building' 
        if ids is None:
            ids = self.get_ids()
        # debug
        #landusetypes =  self.get_landusetypes()
        #for id_fac in ids[self.get_landusetypes().are_area[self.ids_landusetype[ids]] == False]:
        #    id_landusetype = self.ids_landusetype[id_fac]
        #    print '  id_fac',id_fac,id_landusetype,'is_area',landusetypes.are_area[id_landusetype]
            
        return ids[self.get_landusetypes().are_area[self.ids_landusetype[ids]] == False]
    
    def identify_landuse_from_area(self, ids_fac = None):
        """Determines the landuse of facilities from the landuse of areas in which their are located"""
        print('identify_landuse_from_area',ids_fac)
        # TODO:
        landusetypes = self.get_landusetypes()
        ids_area = self.get_ids_area(ids_fac)
        ids_build = self.get_ids_building(ids_fac)
        
        for id_area, shape, id_landuse in zip(ids_area, self.shapes[ids_area],self.ids_landusetype[ids_area]):
            id_landusetype_fac = landusetypes.get_landusetype_facility_from_area(self.ids_landusetype[id_area])
            for id_fac, osmkey, coord in zip(ids_build, self.osmkeys[ids_build], self.centroids[ids_build]):
                
                if osmkey == 'building.yes':
                    if is_point_in_polygon(coord[:2],np.array(shape, dtype = np.float32)[:,:2], is_use_shapely = IS_SHAPELY):
                            print('  found id_fac',id_fac,osmkey,'in id_area',id_area)
                            print('    id_landusetype',self.ids_landusetype[id_fac],'is_area',landusetypes.are_area[self.ids_landusetype[id_fac]],'->',id_landusetype_fac)
                            self.ids_landusetype[id_fac] = id_landusetype_fac
            
    def update_centroid(self, _id):
        self.centroids[_id] = np.mean(self.shapes[_id],0)
          
    def update_area(self, _id):
        
        if IS_SHAPELY:
            area = find_area(self.shapes[_id])
            self.areas[_id] += area        
        else:
            area = get_polygonarea_fast(np.array(self.shapes[_id],float)[:,0], np.array(self.shapes[_id],float)[:,1])
            self.areas[_id] += area    
        
        ids_build = self.get_ids_building()
        ids_area = self.get_ids_area()
        coords_fac = self.centroids[_id][:-1]
        print(_id)
        
        stop_criteria = 0
        if _id in ids_build:
            coords_build = self.centroids[ids_build][:,:-1]
            diff_coords = coords_build - coords_fac
            dists = np.abs(np.sqrt(diff_coords[:,0]**2 + diff_coords[:,1]**2))
            #10 is sufficient? test..
            ids_nearest_build =  ids_build[np.argsort(dists)[:10]]
            for id_fac in ids_nearest_build:
                if len(self.shapes[_id])>2 and len(self.shapes[id_fac])>2:
                    if is_polyline_in_polygon(self.shapes[_id], self.shapes[id_fac]):
                        print('facility', _id, 'inside facility', id_fac)
                        self.areas[_id] = 0.
                        self.areas[id_fac] -= area
                        
        elif _id in ids_area:
            coords_area = self.centroids[ids_area][:,:-1]
            diff_coords = coords_area - coords_fac
            dists = np.abs(np.sqrt(diff_coords[:,0]**2 + diff_coords[:,1]**2))
            #30 is sufficient? test..
            ids_nearest_area =  ids_area[(np.argsort(dists)<10)]
            for id_fac in ids_nearest_area:
                if len(self.shapes[_id])>2 and len(self.shapes[id_fac])>2:
                    if is_polyline_in_polygon(self.shapes[_id], self.shapes[id_fac]):
                        self.areas[_id] = 0.
                        print('area', _id, 'inside area', id_fac)

                        #~ self.areas[id_fac] -= area
            

            
                
    def update_capacity(self, id_fac):
        self.update_capacities([id_fac])
        
    def update_capacities(self, ids):
        ids_fac = self.get_ids_building(ids)
        ids_area = self.get_ids_area(ids)
        volumes_unit = self.get_facilitytypes().unitvolumes[self.ids_facilitytype[ids_fac]]
        
        self.capacities[ids_fac] = self.areas[ids_fac]*self.heights[ids_fac]/volumes_unit
        
        
        # here we assume that pure areas do not have capacities
        # this will prevent that activities are assigned to areas
        # instead of facilities (buildings)
        # TODO: problem is for example parks with no buildings
        # fixed: parks etc. are areas
        
        self.capacities[ids_area] = 0.0
            
    def get_dists(self, ids_fac_from, ids_fac_to):
        """
        Returns centroid to centroid distance from facilities in vector
        ids_fac_from to facilities in vector ids_fac_to.
        """    
        
        return np.sqrt(np.sum((self.centroids[ids_fac_to]-self.centroids[ids_fac_from])**2))
        
    def identify_closest_edge(self, ids = None, priority_max = 5, has_sidewalk = True, n_best = 10):
        """
        Identifies edge ID and position on this edge that 
        is closest to the centoid of each facility and the satisfies certain
        conditions.
        """
        print('identify_closest_edge')
        edges = self.get_edges()
        id_ped = self.get_net().modes.get_id_mode('pedestrian')
        # select edges...if (edges.priorities[id_edge]<=priority_max) & edges.has_sidewalk(id_edge):
        
        #ids_edge = edges.select_ids((edges.priorities.get_value()<priority_max)\
        #                             & (edges.widths_sidewalk.get_value()>0.0))
        accesslevels = edges.get_accesslevels(id_ped)
        #edges.make_segment_edge_map()
        
       
        for id_fac in self.get_ids():
            ids_edge, dists = edges.get_closest_edge(self.centroids[id_fac], n_best = n_best, accesslevels=accesslevels)
            
            if len(ids_edge)>0:
                id_edge = ids_edge[0]
            
            
                # determin position on edeg where edge is closest to centroid
                # TODO: solve this faster with precalculated maps!!
                xc,yc,zc = self.centroids[id_fac]
                shape = edges.shapes[id_edge]
                n_segs = len(shape)
                
                
                d_min = 10.0**8
                x_min = 0.0
                y_min = 0.0
                j_min = 0
                p_min = 0.0
                pos = 0.0
                x1,y1,z1 = shape[0]
                edgelength = edges.lengths[id_edge]
                for j in range(1,n_segs):
                    x2,y2,z2 = shape[j]
                    d, xp, yp = shortest_dist(x1,y1, x2,y2, xc,yc)
                    #print '    x1,y1=(%d,%d)'%(x1,y1),',x2,y2=(%d,%d)'%(x2,y2),',xc,yc=(%d,%d)'%(xc,yc)
                    #print '    d,x,y=(%d,%d,%d)'%shortest_dist(x1,y1, x2,y2, xc,yc)
                    if d<d_min:
                        d_min = d
                        #print '    **d_min=',d_min,[xp,yp]
                        x_min = xp
                        y_min = yp
                        j_min = j
                        p_min= pos
                    #print '    pos',pos,[x2-x1,y2-y1],'p_min',p_min
                    pos+= np.linalg.norm([x2-x1,y2-y1])
                    x1,y1 = x2,y2
                
                x1,y1,z1 =  shape[j_min-1]
                pos_min = p_min+np.linalg.norm([x_min-x1,y_min-y1])
                #print '  k=%d,d_min=%d, x1,y1=(%d,%d),xmin,ymin=(%d,%d),xc,yc=(%d,%d)'%(k,d_min,x1,y1,x_min,y_min,xc,yc)
                #print '  pos=%d,p_min=%d,pos_min=%d'%(pos,p_min,pos_min) 
                
                if pos_min>edgelength: 
                    pos_min = edgelength
                    
                if pos_min<0:
                    pos_min = 0
                #print '  id_fac,id_edge',id_fac,id_edge,pos_min
                self.ids_roadedge_closest[id_fac] = id_edge
                self.positions_roadedge_closest[id_fac] = pos_min
    
        
   
   

    def set_shape(self, id_fac, shape):
        #print 'set_shape',id_fac,shape
        self.shapes[id_fac] =shape
        self.update([id_fac])
        #self.areas[id_fac] = find_area(shape[:,:2])
        #self.centroids[id_fac] =np.mean(shape,0)
    
    def add_polys(self, ids_sumo = [], **kwargs):
        """
        Adds a facilities as used on sumo poly xml info
        """
        # stuff with landusetype must be done later
        return self.add_rows(n=len(ids_sumo),    ids_sumo = ids_sumo, **kwargs)
                        
    def add_poly(self, id_sumo, id_landusetype = None, osmkey = None, shape = np.array([],np.float32)):
        """
        Adds a facility as used on sumo poly xml info
        """
        #print 'add_poly',id_sumo,id_landusetype,osmkey
        
        
        landusetypes = self.get_landusetypes()
        
        if id_landusetype is  None:
            # make default landuse
            id_landusetype = landusetypes.typekeys.get_id_from_index('residential')
   
                
        if osmkey  is None:
            # use first filter as key
            osmkey = landusetypes.osmfilters[id_landusetype][0]
                
        id_fac = self.add_row(  ids_sumo = id_sumo, 
                                    ids_landusetype = id_landusetype, 
                                    osmkeys = osmkey,
                                    )
        self.set_shape(id_fac,shape)
        return id_fac

    
    def clear(self):
        #self.reset()
        self.clear_rows()
    
    def set_shapes(self, ids, vertices):
        self.shapes[ids] = vertices
        if not (isinstance(ids, cm.Iterable) and not isinstance(ids, str)):
            ids = [ids]
        self.update(ids)
    
    
                                
    def import_poly(self, polyfilepath,is_remove_xmlfiles=False, is_clear = True, **kwargs):
        print('import_poly from %s '%(polyfilepath,))
        if is_clear:
            self.clear()
        # let's read first the offset information, which are in the 
        # comment area
        fd = open(polyfilepath,'r', encoding="utf-8")
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
                break
        fd.close()
        offset_delta = offset - self.get_net().get_offset()
        
        exectime_start = time.perf_counter()
            
        counter = SumoPolyCounter()
        parse(polyfilepath, counter)
        fastreader = SumoPolyReader(self, counter, offset_delta, **kwargs)
        parse(polyfilepath, fastreader)
        fastreader.finish()
        
        # update ids_landuse...
        #self.update()
       
        # timeit
        print('  exec time=',time.perf_counter() - exectime_start)
            
        #print '  self.shapes',self.shapes.value
        
    def write_kml(self, fd=None, indent = 0):
        #  <busStop id="busstop4" lane="1/0to2/0_0" startPos="20" endPos="40" lines="100 101"/>
        
        ids_fac = self.get_ids()
        for id_fac in ids_fac:
            fd.write(xm.begin('Placemark',indent + 2))
            fd.write((indent+4)*' '+'<name>%s</name>\n'%id_fac)
            fd.write(xm.begin('Polygon',indent + 4))
            fd.write((indent+6)*' '+'<extrude>1</extrude>\n'%id_fac)
            fd.write((indent+6)*' '+'<altitudeMode>relativeToGround</altitudeMode>\n'%id_fac)
            fd.write(xm.begin('outerBoundaryIs',indent + 6))
            
            fd.write(xm.begin('LinearRing',indent + 8)) 
            fd.write(xm.begin('coordinates',indent + 10)) 
            
            for point in self.shapes[id_fac]:
                
                projection = self.project(point[0],point[1])
                fd.write((indent+12)*' '+'%f,%f,%f\n'%(projection[0],projection[1], self.heights[id_fac]))
    
            fd.write(xm.end('coordinates',indent + 10))
            fd.write(xm.end('LinearRing',indent + 8))
            fd.write(xm.end('outerBoundaryIs',indent + 6))
            fd.write(xm.end('Polygon',indent + 4))
            fd.write(xm.end('Placemark',indent + 2))

    def project(self, x,y):
        if self._proj is None:
            self._proj, self._offset = self.get_proj_and_offset()
        lons, lats = self._proj(x-self._offset[0], y-self._offset[1], inverse = True)
        return np.transpose(np.concatenate(([lons],[lats]),axis=0))

    def get_proj_and_offset(self):
        if self._proj is None:
            net = self.parent.parent.net
            proj_params = str(net.get_projparams())
            #try:
            self._proj = pyproj.Proj(proj_params)
            #except:
            #    proj_params ="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
            #    self._proj = pyproj.Proj(self.proj_params)  
            
            self._offset = net.get_offset()
                    
            return self._proj, self._offset 
        
    def get_facfilepath(self):
        return self.parent.parent.get_rootfilepath()+'.fac.kml'

    def export_sumokml(self, filepath=None, encoding = 'UTF-8'):
        """
        Export stops to SUMO stop xml file.
        """
        print('export_sumoxml',filepath,len(self))
        if len(self)==0: return None
        
        if filepath  is None:
            filepath = self.get_facfilepath()
        
        try:
            fd=open(filepath,'w', encoding="utf-8")
        except Exception:
            print('WARNING in write_obj_to_xml: could not open',filepath)
            return False
        #xmltag, xmltag_item, attrname_id = self.xmltag
        fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
        fd.write('<kml xmlns="http://www.opengis.net/kml/2.2">\n')
        indent = 0
        #fd.write(xm.begin('routes xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/routes_file.xsd"',indent))

    
        fd.write(xm.begin('Document',indent = 0))
        
        self.write_kml(fd, indent = 0)
        
        fd.write(xm.end('Document',indent = 0))
        fd.write(xm.end('kml',indent = 0))
        fd.close() 
        return filepath
 



class SumoPolyCounter(handler.ContentHandler):
    """Counts facilities from poly.xml file into facility structure"""

    def __init__(self):
        self.n_fac = 0
    
    def startElement(self, name, attrs):
        #print 'startElement',name,len(attrs)
        if name == 'poly':
            self.n_fac += 1
        
            
class SumoPolyReader(handler.ContentHandler):
    """Reads facilities from poly.xml file into facility structure"""

    def __init__(self, facilities, counter, offset_delta, type_default='building.yes',height_default = 7.0):
        
        self._type_default = type_default
        self._height_default = height_default
        self._facilities = facilities
        self._ids_landusetype_all = self._facilities.get_landusetypes().get_ids()
        self._osmfilters = self._facilities.get_landusetypes().osmfilters
        
        self._ind_fac = -1
        self.ids_sumo = np.zeros(counter.n_fac,np.object_)
        self.ids_landusetype = -1*np.ones(counter.n_fac,np.int32) 
        self.osmkeys = np.zeros(counter.n_fac,np.object_)
        self.names = np.zeros(counter.n_fac,np.object_)
        self.shape = np.zeros(counter.n_fac,np.object_) 
        self.areas = np.zeros(counter.n_fac,np.float32)
        self.heights = self._height_default * np.ones(counter.n_fac,np.float32)
        self.centroids = np.zeros((counter.n_fac,3),np.float32) 
                          
        
        #self._id_facility = None
        self._offset_delta = offset_delta
            
    def startElement(self, name, attrs):
        
        #print 'startElement', name, len(attrs)
        if name == 'poly':
            self._ind_fac += 1
            i = self._ind_fac
            
            osmkey = attrs.get('type',self._type_default)
            #print '  id_sumo=',attrs['id'],osmkey
            id_landuse = self.get_landuse(osmkey)
            if id_landuse>=0: # land use is interesting
                shape = xm.process_shape(attrs.get('shape',''), offset = self._offset_delta)
                shapearray = np.array(shape,np.float32)
                #print '  shapearray',shapearray
                self.ids_sumo[i] = attrs['id']
                self.ids_landusetype[i] = id_landuse
                self.osmkeys[i] = osmkey
                self.shape[i] = shape 
                self.areas[i] = find_area(shapearray[:,:2])
                self.centroids[i] = np.mean(shapearray,0)
                self.names[i] = ''
                
                #print '    control id_sumo',self.ids_sumo[i]
                
        elif name == 'param':
            i = self._ind_fac
            if  attrs['key'] == 'height':
                self.heights[i] = string_to_float(attrs['value'])
            elif attrs['key'] == 'name':
                self.names[i] = attrs['value']
                                    
            # color info in this file no longer used as it is defined in
            # facility types table
            #color = np.array(xm.parse_color(attrs['color']))*0.8,# make em darker!! 
    
    def get_landuse(self, osmkey):
        keyvec = osmkey.split('.')
        len_keyvec=len(keyvec)
        print('get_landuse',len_keyvec,keyvec)
        #is_match = False
        for id_landusetype in self._ids_landusetype_all:
            #print '    id_landusetype',id_landusetype
            #if fkeys==('building.industrial'): print ' check',facilitytype
            
            # first filter only exact matches before wildcards
            for osmfilter in self._osmfilters[id_landusetype]:
                #print '       ?osmfiltervec',osmfilter,osmkey==osmfilter
                if osmkey==osmfilter: # exact match of filter
                    print('      exact',osmkey)
                    return id_landusetype
        
        # now check for wildcards     
        for id_landusetype in self._ids_landusetype_all:
            #print '    *id_landusetype',id_landusetype
                       
            for osmfilter in self._osmfilters[id_landusetype]:
                osmfiltervec = osmfilter.split('.')
                #print '       ?osmfiltervec',osmfiltervec,(len(osmfiltervec)==2)&(len_keyvec==2)
                if (len(osmfiltervec)==2)&(len_keyvec==2):    
                    if osmfiltervec[0]==keyvec[0]:
                        if osmfiltervec[1]=='*':
                            print('      *',keyvec[0])
                            return id_landusetype
        return -1
                                        
    def finish(self):
        
        #print 'write_to_net'
        inds_valid = np.flatnonzero(self.ids_landusetype>=0)
        ids_fac = self._facilities.add_polys(
                        ids_sumo = self.ids_sumo[inds_valid],
                        ids_landusetype = self.ids_landusetype[inds_valid] ,
                        osmkeys = self.osmkeys[inds_valid],
                        shapes = self.shape[inds_valid],
                        areas = self.areas[inds_valid], 
                        centroids = self.centroids[inds_valid], 
                        heights = self.heights[inds_valid],
                        names = self.names[inds_valid]
                    )

    #def characters(self, content):
    #    if self._id is not None:
    #        self._currentShape = self._currentShape + content


    #def endElement(self, name):
    #    pass
    
    
                                                          

class Parking(am.ArrayObjman):
    def __init__(self, landuse, lanes, **kwargs):
        #print 'Parking.__init__',lanes,hasattr(self,'lanes')
        self._init_objman(  ident='parking', parent=landuse, 
                            name = 'Parking', 
                            info = 'Information on private car parking.',
                            #is_plugin = True,
                            #**kwargs
                            )                                           

        self._init_attributes(lanes)
        
        
    def _init_attributes(self, lanes = None):
        #print 'Parkin._init_attributes',lanes,hasattr(self,'lanes'),hasattr(self,'ids_lane')
        if lanes is None:
            # upgrade call
            # lanes exists already as link 
            lanes = self.get_lanes()
            
        #print '  lanes',lanes
        #-------------------------------------------------------------------- 
        # misc params...
        
        # these are options for assignment procedure!!
        #self.add(AttrConf(  'length_noparking', kwargs.get('length_noparking',20.0),
        #                                groupnames = ['options'], 
        #                                perm='wr', 
        #                                unit = 'm',
        #                                name = 'Min Length', 
        #                                info = 'Minimum edge length for assigning on-road parking space.' ,
        #                                #xmltag = 'pos',
        #                                ))
        # 
        #self.add(AttrConf(  'length_space', kwargs.get('length_space',20.0),
        #                                groupnames = ['options'], 
        #                                perm='wr', 
        #                                unit = 'm',
        #                                name = 'Lot length', 
        #                                info = 'Length of a standard parking lot.' ,
        #                                #xmltag = 'pos',
        #                                )) 
        
                            
        
        self.add_col(am.IdsArrayConf( 'ids_lane', lanes, 
                                        name = 'ID Lane', 
                                        info = 'ID of lane for this parking position. ',
                                        ))
        
        
                                                                                        
        self.add_col(am.ArrayConf(  'positions', 0.0,
                                        dtype=np.float32,
                                        perm='r', 
                                        name = 'Pos',
                                        unit = 'm',
                                        info = "Position on lane for this parking.",
                                        ))
                                        
        self.add_col(am.ArrayConf(  'lengths', 0.0,
                                        dtype=np.float32,
                                        #choices = OPTIONMAP_POS_DEPARTURE,
                                        perm='r', 
                                        name = 'Length',
                                        unit = 'm',
                                        info = "Length of parking lot in edge direction.",
                                        ))
                                                                        
        self.add_col(am.ArrayConf(  'angles', 0.0,
                                        dtype=np.float32,
                                        perm='rw', 
                                        name = 'Angle',
                                        unit = 'deg',
                                        info = "Parking angle with respect to lane direction.",
                                        ))
        
                                        
        
                                        
        
        
                                        
        self.add_col(am.ArrayConf(   'vertices',  np.zeros((2,3), dtype=np.float32),
                                        dtype=np.float32,
                                        groupnames = ['_private'], 
                                        perm='r', 
                                        name = 'Coords',
                                        unit = 'm',
                                        info = "Start and end vertices of right side of parking space.",
                                        ))
                                        
        
        
        self.add_col(am.ArrayConf(  'numbers_booking', 0,# ???
                                        dtype=np.int32,
                                        perm='r', 
                                        name = 'booked',
                                        info = "Number of vehicles booked for this parking.",
                                        ))
                                                                        
        #self.add_col(am.ArrayConf(  'durations', 0.0,# ???
        #                                dtype=np.float32,
        #                                perm='r', 
        #                                name = 'Parking duration',
        #                                unit = 's',
        #                                info = "Default duration of car parking.",
        #                                ))
        
        
        
                                       
        self.add( cm.ObjConf( lanes, is_child = False, groups = ['_private']))                                                                    

        self.add( cm.ObjConf( lanes.parent.edges, is_child = False, groups = ['_private'])) 
    
    def get_edges(self):
        return self.edges.get_value()
        
    def get_lanes(self):
        return self.lanes.get_value()
        
    def link_vehiclefleet(self, vehicles):
        """
        Links to table with vehicle info.
        """
        self.add_col(am.IdsArrayConf( 'ids_bookedveh', vehicles, 
                                        name = 'ID booked veh', 
                                        info = 'ID of vehicle which has booked this parking position.',
                                        ))
                                        
    
    
    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust their coordinates.
        """
        pass
            
    def get_parkinglane_from_edge(self, id_edge, id_mode, id_mode_fallback = MODES['private'], length_min=15.0, priority_max=8, n_freelanes_min = 1, speed_max = 13.89):
        """
        Check if edge can have on-road parking.
        Returns lane ID if parking is possible and -1 otherwise.
        """
        edges = self.edges.get_value()
        lanes = self.lanes.get_value()
        id_mode_ped = edges.parent.modes.get_id_mode('pedestrian')
        # check access
        ids_lane = edges.ids_lanes[id_edge]
        edgelength = edges.lengths[id_edge]
        
        if (len(ids_lane)>=2):
            if (lanes.get_accesslevel([ids_lane[0]], id_mode_ped)>-1)&((lanes.get_accesslevel([ids_lane[1]], id_mode)>-1)|(lanes.get_accesslevel([ids_lane[1]], id_mode_fallback)>-1)):
                # check size
                #laneindex = 
                #print 'get_parkinglane_from_edge',id_edge, id_mode,priority_max,length_min
                #print '   check',(edges.priorities[id_edge]<=priority_max),(edges.lengths[id_edge]>length_min),(edges.widths_sidewalk[id_edge]>-1)
                
                if  (edges.speeds_max[id_edge] <= speed_max)&(edges.priorities[id_edge]<=priority_max)&(edgelength>length_min)&(edges.widths_sidewalk[id_edge]>0):
                    
                    laneindex = 1
                    #print '  found',laneindex,edges.nums_lanes[id_edge]-laneindex > n_freelanes_min
                    if (len(ids_lane)-laneindex >= n_freelanes_min):
                        return ids_lane[laneindex]
                    else:
                        return -1
                else:
                    return -1
            else:
                return -1
            
        return -1 # no parking possible by default 
    
    def get_edge_pos_parking(self, id_parking):
        lanes = self.lanes.get_value()
        return lanes.ids_edge[self.ids_lane[id_parking]], self.positions[id_parking]
                
    #def get_edge_pos_parking(self, id_parking):
    #    """
    #    Retuens edge and position of parking with id_parking
    #    """
    #    ind = self.parking.get_ind(id_parking)
    #    
    #    return self.edges.get_value()(self.id_edge_parking[ind]),self.pos_edge_parking[ind]
            
    def make_parking(self,  is_selected_zones = False, ids_zone = [], is_near_facilities = False, id_mode = MODES['passenger'], 
                            id_mode_fallback = MODES['private'], 
                            length_min = 42.0, length_noparking = 15.0, 
                            length_lot = 6.0, angle = 0.0, 
                            is_clear = True, 
                            logger = None,
                            **kwargs):
        print('make_parking')
        if is_clear:
            self.clear()
        edges = self.edges.get_value()
        lanes = self.lanes.get_value()
        
        
        ids_zoneedge = set()
        if is_selected_zones:
            zones = self.parent.zones
            #print '  ids_zoneedge',zones.ids_edges_inside[self.ids_zone]
            
            for ids_edge in zones.ids_edges_inside[ids_zone]:
                ids_zoneedge.update(ids_edge)
            
            
        
            
        
        ids_edge_near_facilities = set(self.parent.facilities.ids_roadedge_closest.get_value())
        print('  ids_edge_near_facilities',ids_edge_near_facilities)
        n_parking = 0
        ids_parking = []
        ids_lane_current = self.ids_lane.get_value().copy()
        ii = 0.0
        n_edges = len(edges.get_ids())
        
        # TODO: here we could loop only edges which satisfy conditions
        for id_edge in edges.get_ids():
            ii += 1
            if logger is not None:
                logger.progress(int(ii/n_edges*100))
            
            if is_selected_zones:
                is_eligible = id_edge in ids_zoneedge
            else: 
                is_eligible = True
            
            if is_near_facilities:
                is_eligible &= id_edge in ids_edge_near_facilities
            print('  id_edge,',id_edge, id_edge in ids_zoneedge,id_edge in ids_edge_near_facilities,is_eligible)
            if is_eligible:
                # check if edge is suitable...
                #print '  id_edge,length,n_lanes,',id_edge
                id_lane = self.get_parkinglane_from_edge(id_edge, id_mode, id_mode_fallback, length_min, **kwargs)
                
                is_eligible = id_lane >= 0
                if not is_clear:
                    if id_lane not in ids_lane_current:
                        is_eligible = False
                        
                if is_eligible:
                    n_spaces = int((edges.lengths[id_edge]-2*length_noparking)/length_lot)
                    #print '    create',id_edge,lanes.indexes[id_lane],edges.lengths[id_edge],n_spaces
                    #print '  delta',lanes.shapes[id_lane][0]-lanes.shapes[id_lane][-1]
                    pos_offset = length_noparking
                    pos = pos_offset
                    if n_spaces>0:
                        for i in range(n_spaces):
                            #id_park = self.suggest_id()
                            #print '    pos=',pos,pos/edges.lengths[id_edge]
                            
                            #print '    vertices',get_vec_on_polyline_from_pos(lanes.shapes[id_lane],pos, length_lot, angle = angle)
                            n_parking+=1
                            
                            id_park = self.add_row( ids_lane = id_lane, 
                                                    positions = pos,
                                                    lengths = length_lot,
                                                    angles = angle,
                                                    vertices = get_vec_on_polyline_from_pos(lanes.shapes[id_lane],pos, length_lot-0.5, angle = angle)
                                                   )
                            #print '    created id_park,pos', id_park,pos#,get_coord_on_polyline_from_pos(lanes.shapes[id_lane],pos),lanes.shapes[id_lane]
                            ids_parking.append(id_park)
                            pos = pos_offset+(i+1)*length_lot
                    
        print('  created %d parking spaces'%n_parking)  
        return ids_parking
    
    def clear_booking(self):
        self.numbers_booking.reset()
        if hasattr(self,'ids_bookedveh'):
            self.ids_bookedveh.reset()
    
    def get_closest_parking(self, id_veh, coord, c_spread = 2.0):
        """
        Returns  parking space for  id_veh as close as possible to coord.
        """
        
        #inds_person = self.persons.get_inds(ids_person)
        print('get_closest_parking')
        ind_parking_closest = self.get_inds()[np.argmin(np.sum((coord-self.vertices.value[:, 1, :])**2 ,1)+ c_spread*lengths*self.lengths.get_value())]
        self.numbers_booking.get_value()[ind_parking_closest] +=1
        return self.get_ids(ind_parking_closest), ind_parking_closest
         
    def get_closest_parkings(self, ids_veh, ids_mode, coords, dists_walk_max, c_spread = 2.0, 
                                n_retrials = 20, id_mode_fallback = None):
        """
        Returns  parking space for each vehicle in ids_veh as close as possible to coords.
        """
        
        ##Used by virtualpop
        
        lanes = self.ids_lane.get_linktab()
        ids_lane = self.ids_lane.get_value()
        #inds_person = self.persons.get_inds(ids_person)
        n=len(ids_veh)
        
        #print 'get_closest_parking',n,len(self),'n_retrials',n_retrials
        if len(self)==0:
            print('WARNING in get_closest_parkings: there is no parking.')
            return [],[]
        
        #parking = self.get_landuse().parking
        #inds_parking = parking.get_inds()
        coord_parking = self.vertices.value[:, 1, :]
        #print '  coord_parking',coord_parking
        numbers_booking = self.numbers_booking.get_value()
        lengths = self.lengths.get_value()
        inds_vehparking = np.zeros(n,int)
        are_fallback = np.zeros(n, bool)
        
        #inds_parking_avail = np.flatnonzero( self.ids_bookedveh.value == -1).tolist()
        inds_parking_avail = self.get_inds().copy()

        #ids_veh = np.zeros(n,object)
        i = 0
        for id_veh,id_mode, coord, dist_walk_max in zip(ids_veh,ids_mode,coords,dists_walk_max):
            #print '  search parking for id_veh',id_veh
            #print '    landuse.id_bookedveh_parking',landuse.id_bookedveh_parking
            # 
            dists = np.sqrt(np.sum((coord-coord_parking)**2 ,1))
            dist_min = np.min(dists)
            #print '    inds_parking_avail',inds_parking_avail
            #print '    dists',np.sum((coord-coord_parking[inds_parking_avail])**2,1),np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2,1))
            is_fallback = False
            penalties_inaccessible = np.ones(len(numbers_booking), dtype = np.float32)
            n_search = n_retrials
            is_search = True
            while (n_search>0) & is_search:
                
                ind_parking_closest = inds_parking_avail[np.argmin(dists + c_spread*lengths*numbers_booking*penalties_inaccessible)]
                #print '    ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail),'n_search',n_search,'is_search',is_search
                is_search = (not (lanes.get_accesslevel([ids_lane[ind_parking_closest]],id_mode) >= 0))\
                            | (dists[ind_parking_closest]-dist_min > dist_walk_max) 
                            # this means walk dist sees only walking from closest possible to actal parking
                penalties_inaccessible[ind_parking_closest] = np.inf # prevent reselection of this parking
                n_search -= 1
                #print '    n_search',n_search,'is_search',is_search
            
            #print '  done with id_mode is_search',is_search
            if is_search & (id_mode_fallback is not None):
                # search mode means no parking has bee found for ordinary mode
                # now try with fallback mode
                
                is_fallback = True
                penalties_inaccessible = np.ones(len(numbers_booking), dtype = np.float32)
                n_search = n_retrials
                while (n_search>0) & is_search:
                    ind_parking_closest = inds_parking_avail[np.argmin(dists + c_spread * lengths * numbers_booking * penalties_inaccessible)]
                    #print '  ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail)
                    is_search = (not (lanes.get_accesslevel([ids_lane[ind_parking_closest]],id_mode_fallback) >= 0))\
                                | (dists[ind_parking_closest]-dist_min > dist_walk_max) 
                    penalties_inaccessible[ind_parking_closest] = np.inf # prevent reselection of this parking
                    n_search -= 1
                    #print '    fallback n_search',n_search,'is_search',is_search
                    
            if is_search:
                print('WARNING: inaccessible parking for id_veh',id_veh,'is_fallback',is_fallback)
                print('  dist=%.1f'%(np.sqrt(dists[ind_parking_closest])),'id_lane',ids_lane[ind_parking_closest],'al',lanes.get_accesslevel([ids_lane[ind_parking_closest]],id_mode_fallback))
            
                
            inds_vehparking[i] = ind_parking_closest
            are_fallback[i] = is_fallback
            #print '    coords_veh',coord
            #print '    coord_park',coord_parking[ind_parking_closest]
            numbers_booking[ind_parking_closest] +=1
            
            #id_parking = self.get_ids([ind_parking_closest])
            #id_edge, pos = self.get_edge_pos_parking(id_parking)
            #print '    id_veh=%s,id_parking_closest=%s, dist =%.2fm'%(id_veh,id_parking,np.sqrt(np.sum((coord-coord_parking[ind_parking_closest])**2)))
            #ids_bookedveh[ind_parking_closest]=id_veh # occupy parking
            #print '    id_edge, pos',id_edge, pos
            #inds_parking_avail.remove(ind_parking_closest)
            i += 1
        
        #print '  inds_vehparking',  inds_vehparking  
        #print '  ids_vehparking',  self.get_ids(inds_vehparking) 
        #print '  ids_veh',ids_veh
        #self.ids_bookedveh.value[inds_vehparking] = ids_veh
        #self.ids_bookedveh.[ids_parking] =ids_bookedveh 
        return self.get_ids(inds_vehparking), are_fallback
    
    def assign_parking(self, ids_veh, coords, is_overbook = False):
        """
        Assigns a parking space to each vehicle as close as possible to coords.
        Only one vehicle can be assigned to a parking space.
        """
        
        #inds_person = self.persons.get_inds(ids_person)
        n=len(ids_veh)
        #print 'assign_parking',n
        
        
        #parking = self.get_landuse().parking
        #inds_parking = parking.get_inds()
        coord_parking = self.vertices.value[:, 1, :]
        
        inds_vehparking = np.zeros(n,int)
        
        inds_parking_avail = np.flatnonzero( self.ids_bookedveh.value == -1).tolist()
        
        #ids_veh = np.zeros(n,object)
        i = 0
        for id_veh, coord in zip(ids_veh,coords):
            #print '\n  id_veh,coord',id_veh,coord
            #print '    landuse.id_bookedveh_parking',landuse.id_bookedveh_parking
            # 
            
            
            #print '    inds_parking_avail',inds_parking_avail
            #print '    dists',np.sum((coord-coord_parking[inds_parking_avail])**2,1),np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2,1))
            ind_parking_closest = inds_parking_avail[np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2,1))]
            #print '  ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail)
            inds_vehparking[i] = ind_parking_closest
            #print '  id_veh=%s,id_parking_closest=%s, dist =%.2fm'%(id_veh,self.get_ids([ind_parking_closest]),np.sqrt(np.sum((coord-coord_parking[ind_parking_closest])**2)))
            #ids_bookedveh[ind_parking_closest]=id_veh # occupy parking
            
            inds_parking_avail.remove(ind_parking_closest)
            i += 1
        
        #print '  inds_vehparking',  inds_vehparking  
        #print '  ids_vehparking',  self.get_ids(inds_vehparking) 
        #print '  ids_veh',ids_veh
        self.ids_bookedveh.value[inds_vehparking] = ids_veh
        #self.ids_bookedveh.[ids_parking] =ids_bookedveh 
        return self.get_ids(inds_vehparking), inds_vehparking
           

class ElevationImporter(Process):
    def __init__(self,  scenario, logger = None, **kwargs):
        print ('ElevationImporter.__init__',scenario.landuse)
        self._init_common(  'elevationimporter', name = 'Elevation importer', 
                            logger = logger,
                            info ='Downloads elevation data and adds elevation to different elements of the scenario.',
                            )
        self._landuse = scenario.landuse
        self._net = scenario.net
        
        attrsman = self.set_attrsman(cm.Attrsman(self))


        self.apikey = attrsman.add(cm.AttrConf(  'apikey',kwargs.get('apikey',''),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'API key', 
                            info = 'API key for google maps. API key can be obtained from Google at https://cloud.google.com/maps-platform/?refresh=1&pli=1#get-started.',
                            ))
        
        #self.add_option(  'password',kwargs.get('password',''),
        #                     perm='rw', 
        #                     name = 'User', 
        #                     info = 'User name of map server (if required).',
        #                     )
        
        self.is_apply_bbox_explicit = attrsman.add(cm.AttrConf( 'is_apply_bbox_explicit',kwargs.get('is_apply_bbox_explicit',False),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Apply expicit bounding box', 
                            info = 'Apply expicit bounding box to GPS points. If False, network boundaries are applied.',
                            ))
                            
        self.bbox_explicit = attrsman.add(cm.ListConf('bbox_explicit',kwargs.get('bbox_explicit',[0.0,0.0,0.0,0.0]), 
                                            groupnames = ['options'], 
                                            name = 'Explicit bounding box', 
                                            info = """Expicit bounding box of the format [x_min, y_min,x_max, y_max]. Empty list means no explicit bounding borders are applied.""",
                                            ))
        

                            
        self.is_add_net_elevation = attrsman.add(cm.AttrConf( 'is_add_net_elevation',kwargs.get('is_add_net_elevation',True),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Add elevation to network',
                            info = 'Downlod and add elevations to the network edges, nodes and lane shapes.',
                            )) 
        self.is_interpolate_geompoint = attrsman.add(cm.AttrConf( 'is_interpolate_geompoint',kwargs.get('is_interpolate_geompoint',True),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Interpolate geometry points',
                            info = 'Interpolate geometry points of the network edges, insted of downloading them.',
                            )) 
                            
        self.is_add_zone_elevation = attrsman.add(cm.AttrConf( 'is_add_zone_elevation',kwargs.get('is_add_zone_elevation',True),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Add elevation to zones',
                            info = 'Downlod and add elevations to zones.',
                            ))  
        
        self.is_add_facility_elevation = attrsman.add(cm.AttrConf( 'is_add_facility_elevation',kwargs.get('is_add_facility_elevation',True),
                            groupnames = ['options'],
                            perm='rw',
                            name = 'Add elevation to facilities',
                            info = 'Downlod and add elevations to facilities.',
                            ))  
        self.n_coords_max = attrsman.add(cm.AttrConf(  'n_coords_max',kwargs.get('n_coords_max',100),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Max. coordinates per query', 
                            info = 'Maximum number of coordinates used in each API queuery, lower this number if no elevation results are returned.',
                            ))
    
    def do(self):
        print ('ElevationImporter.do')
        
        
        params_proj = self._net.get_projparams()
        self._proj = pyproj.Proj(str(params_proj))
        self._offset = self._net.get_offset()
        
        if self.is_apply_bbox_explicit:
            bbox_sumo = self.bbox_explicit
        else:
            bbox_sumo = None
            
        if self.is_add_net_elevation:
            
            
            # add elevation to nodes
            ids = self._net.nodes.get_ids()
            self._net.nodes.coords[ids] = self.import_elevation(self._net.nodes.coords[ids])
            print ('  node coords check =',self._net.nodes.coords[ids])
            
            if not self.is_interpolate_geompoint:     
                # add elevation to edges and then update to lanes
                edges = self._net.edges
                lanes = self._net.lanes
                ids = edges.get_ids()
                shapes = edges.shapes
                
                ind = 0
                inds_shape = []
                vertexes = []
                for shape in zip(shapes[ids]):
                    n_vert = len(shape[0])
                    inds_shape.append([ind, ind+n_vert])
                    ind += n_vert
                    vertexes += list(shape[0])
                    #print ('  shape',type(shape[0]),n_vert,shape[0])

                vertexes = self.import_elevation(np.array(vertexes, dtype = np.float32))
                
                for ind, _id in zip(xrange(len(ids)),ids):
                    shapes[_id] = vertexes[inds_shape[ind][0]:inds_shape[ind][1]]
                    #print ('  _id',_id,vertexes[inds_shape[ind][0]:inds_shape[ind][1]])
                    
                    # update lanes of this edge too
                    lanes.reshape_edgelanes(_id)
                
                edges.make_segment_edge_map()
            
            if len(self._landuse.parking) > 0:
                # add elevatio of parking
                coords = self._landuse.parking.vertices.get_value().reshape((-1,3))
                #print ('  coords',coords)
                coords = self.import_elevation(coords)
                #print ('  coords',coords.reshape((-1,2,3)))
                self._landuse.parking.vertices.set_value(coords.reshape((-1,2,3)))
        
        if self.is_interpolate_geompoint:
            edges = self._net.edges
            lanes = self._net.lanes
            nodes = self._net.nodes
            ids = edges.get_ids()
            shapes = edges.shapes
            
            ind = 0
            inds_shape = []
            vertexes = []
            for id_edge, L, (x1,y1,z1), (x2,y2,z2)  in zip(ids,edges.lengths[ids], nodes.coords[edges.ids_fromnode[ids]],nodes.coords[edges.ids_tonode[ids]] ):
                n_shapes = len(shapes[id_edge])
                if n_shapes == 2:
                    # no interpolation necessary
                    shapes[id_edge][0][2] = z1
                    shapes[id_edge][1][2] = z2
                    
                elif L < np.abs(z1-z2):# this is for short edges
                
                    positions = np.zeros(n_shapes, dtype = np.float32)
                    for i, point in zip(range(n_shapes), shapes[id_edge]):
                        positions[i] = edges.get_pos_from_coord(id_edge, point)
                    
                    # simple linear interpolation
                    elevations = line(positions, z1, (z2-z1)/L) 
                    
                    for i in  range(n_shapes) :
                        shapes[id_edge][i][2] = elevations[i]
                else:
                    
                    positions = np.zeros(n_shapes, dtype = np.float32)
                    for i, point in zip(range(n_shapes), shapes[id_edge]):
                        positions[i] = edges.get_pos_from_coord(id_edge, point)
                    
                    # qspline interpolation
                    a0 = z1
                    splinemat = np.array([[L**2, L**3],[2*L, 3*L**2]], dtype = np.float32)
                    a1, a2 = np.dot(inv(splinemat),np.array([z2-z1, 0]))
                    elevations = qspline(positions,a0,a1,a2)
                    
                    for i in  range(n_shapes) :
                        shapes[id_edge][i][2] = elevations[i]
                    
                    #if id_edge in [3049,3183]:
                    #    print ('  L=',L,'z1',z1,'z2',z2)
                    #    print ('  pos =',positions)
                    #    print ('  ele =',elevations)
            
            for ind, _id in zip(range(len(ids)),ids):
                    # update lanes of this edge too
                    lanes.reshape_edgelanes(_id)
                
            edges.make_segment_edge_map()
                
            
        if self.is_add_facility_elevation:
            ids = self._landuse.facilities.get_ids()
            shapes = self._landuse.facilities.shapes
            # with shapes we pick only first coordiate as sample and apply
            # its elevation to all coordinates
            coords_sample = np.zeros((len(ids),3), dtype = np.float32)
            ind = 0
            for  _id in ids:
                coords_sample[ind,:] = shapes[_id][0]
                ind += 1
            
            # one call for all facility sample coordinated    
            coords_sample = self.import_elevation(coords_sample)
            #print ('  coords_sample',coords_sample)
            
            # insert elevation to buildings
            ind = 0
            for  _id, shape in zip(ids, shapes[ids]):
                shape_array = np.array(shape, dtype = np.float32)
                shape_array[:,2] = coords_sample[ind,2]
                shapes[_id] = list(shape_array)
                
                #print ('  shapes[_id]',edges.shapes[_id])
                ind += 1
        return True

    def import_elevation(self, coords):
        """
        Imports elevation for the given coords with the specified methods.
        In particular it queueries in junks of length defined by the attribute n_coords_max
        Takes coords as an Nx3 numpy array.
        Returns the coords with the elevation in the third dimension. 
        """
        importmethod = self._download_google_elevation
        n = len(coords)
        if n < self.n_coords_max:
            return importmethod(coords)
        else:
            
            i = self.n_coords_max
            while i <= n:
                print ('  import coords from', i - self.n_coords_max,'to',i)
                coords[i - self.n_coords_max:i,:] = importmethod(coords[i - self.n_coords_max:i,:])
                i += self.n_coords_max
            
            i -= self.n_coords_max
            
            if i < n:
                print ('  import coords from', i,'to',n)
                coords[i:n, :] = importmethod(coords[i:n, :])
            
            return coords
    
    def _download_google_elevation(self, coords):
        """
        Downloads the elevation for the given coords and substitutes the third 
        dimention of the coords array with the respective elevation.
        """
        print ('_download_google_elevation for %d coords'%len(coords))
        urlbase = 'https://maps.googleapis.com/maps/api/elevation/json?locations='

        
        lons, lats = self._proj(coords[:,0]-self._offset[0], coords[:,1]-self._offset[1], inverse = True)
        
        str_coords = ''
        for lon, lat in zip(lons, lats):
            if str_coords is '':
                str_coords = str(lat)+','+str(lon)
            else:
                str_coords += '|'+str(lat)+','+str(lon)
        
        #print ('str_coords',str_coords)
        
        str_url = urlbase+str_coords+'&key='+self.apikey
        
        #print ('str_url',str_url)
        
        web_url = urllib.request.urlopen(str_url)
        data = web_url.read()
        json_object = json.loads(data.decode('utf-8'))
        
        
        #print ('json_object',json_object)
        #print ('status',json_object['status'],json_object['status'] == 'OK')
        #print ('results',json_object['results'])
        
        if json_object['status'] == 'OK':
            results = json_object['results']
            
            row = 0
            for coord_data in results:
                #print '  location',coord_data['location']['lat'],coord_data['location']['lng'],'elev',coord_data['elevation'],'reso',coord_data['resolution']
                coords[row,2] = coord_data['elevation']
                #print '  location',coords[row,:],'elev',coord_data['elevation'],'reso',coord_data['resolution']
                
                row += 1
        else:
            print ('WARNING: elevation import failed')
            print ('  Returned status',json_object['status'])
        return  coords   
           

class Landuse(cm.BaseObjman):
        def __init__(self, scenario=None, net = None,  **kwargs):
            self._init_objman(ident= 'landuse', parent=scenario, name = 'Landuse', **kwargs)
            attrsman = self.set_attrsman(cm.Attrsman(self))
            
            

            
            self._init_attributes()
            self._init_constants()
        
        def _init_attributes(self,scenario=None, net = None):
            attrsman = self.get_attrsman()
            
            if scenario is not None:
                net = scenario.net
            else:
                net = self.parent.net

            self.landusetypes = attrsman.add(   cm.ObjConf( LanduseTypes(self) ) )
            self.zones = attrsman.add(   cm.ObjConf( Zones(self) ) )
            self.facilities = attrsman.add(   cm.ObjConf( Facilities(self,self.landusetypes, self.zones, net = net) ) )
            self.parking = attrsman.add(   cm.ObjConf( Parking(self,net.lanes) ) )
            
            self.maps = attrsman.add(   cm.ObjConf( maps.Maps(self) ) )

            
            self.pois = attrsman.add(   cm.ObjConf( POIs(self) ) )
            
        def update_netoffset(self, deltaoffset):
            """
            Called when network offset has changed.
            Children may need to adjust theur coordinates.
            """
            self.zones.update_netoffset(deltaoffset)
            self.facilities.update_netoffset(deltaoffset)
            self.parking.update_netoffset(deltaoffset)
            self.maps.update_netoffset(deltaoffset)
                   
        def get_net(self):
            # parent of landuse must be scenario
            if self.parent is not None:
                return self.parent.net
            else:
                return None
            
        
        def export_polyxml(self, filepath=None, encoding = 'UTF-8', delta = np.zeros(3,dtype=np.float32)):
            """
            Export landuse facilities to SUMO poly.xml file.
            """
            if len(self.facilities) == 0:
                return None
            
            if filepath  is None:
                if self.parent is not None:
                    filepath = self.get_filepath()
                else:
                    filepath = os.path.join(os.getcwd(),'landuse.poly.xml')
                    
            print('export_polyxml',filepath)
            try:
                fd=open(filepath,'w', encoding="utf-8")
            except Exception:
                print('WARNING in export_poly_xml: could not open',filepath)
                return None
            
            #xmltag, xmltag_item, attrname_id = self.xmltag
            xmltag_poly = 'additional'
            fd.write('<?xml version="1.0" encoding="%s"?>\n'%encoding)
            fd.write(xm.begin(xmltag_poly))
            indent = 2
            
            fd.write(xm.start('location',indent+2))
            #print '  groups:',self.parent.net.get_attrsman().get_groups()
            for attrconfig in self.parent.net.get_attrsman().get_group('location'):
                #print '    locationconfig',attrconfig.attrname
                if attrconfig.attrname == '_boundaries':
                    delta_bb = np.zeros(4, dtype = np.float32)
                    delta_bb[0:2] = delta[:2]
                    delta_bb[2:4] = delta[:2]
                    fd.write(xm.arr('convBoundary',attrconfig.get_value()-delta_bb))
                else:
                    attrconfig.write_xml(fd)
            fd.write(xm.stopit())
            
            self.facilities.write_xml(fd, indent=indent+2, is_print_begin_end = False, delta = delta)
            
            fd.write(xm.end(xmltag_poly))
            fd.close()
            return filepath
    
        def get_filepath(self):
            return self.parent.get_rootfilepath() + '.poly.xml'
        
        def import_polyxml(self, rootname=None, dirname='', filepath = None, is_clear = True, is_pois = True, **kwargs):
            if filepath is None:
                if rootname is not None:
                    filepath = os.path.join(dirname,rootname+'.poly.xml')
                else:
                    filepath = self.get_filepath()
            
            if os.path.isfile(filepath):
                self.facilities.import_poly(filepath, is_clear = is_clear,**kwargs)
                if is_pois:
                    self.pois.import_poly(filepath, is_clear = is_clear,**kwargs)
            else:
                self.get_logger().w('import_xml: files not found:'+filepath, key='message')                            
            
            #
            # here may be other relevant imports
            #

class ZoneFromGeojsonImporter(Process):
    def __init__(self, zones,  logger = None, **kwargs):
        print('ZoneFromGeojsonImporter.__init__')
                
           
        self._init_common(  'zonefromgeojsonimporter', 
                            parent = zones,
                            name = 'Zone from Geojson importer', 
                            logger = logger,
                            info ='Imports zone data from geojason file.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.jsonfilepath = attrsman.add(cm.AttrConf('jsonfilepath',kwargs.get('jsonfilepath',''),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Geo-Json file', 
                                    wildcards = 'json file (*.json)|*.json',
                                    metatype = 'filepath',
                                    info = "Geo jason file path with data.",
                                    ))
                                    
                                    
        self.attrname_id = attrsman.add(cm.AttrConf('attrname_id',kwargs.get('attrname_id',''),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'ID attribute name', 
                                    info = "Zone ID attribute name.Ids generated automatically when left blank.",
                                    ))
        self.attrname_name = attrsman.add(cm.AttrConf('attrname_name',kwargs.get('attrname_name','name'),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Name attribute name', 
                                    info = "Zone Name attribute name.",
                                    ))
        self.prefix = attrsman.add(cm.AttrConf('prefix',kwargs.get('prefix',''),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Prefix', 
                                    info = "Prefix will be automatically prepended to zone ID, Leave blank if no prefix is desired.",
                                    ))
        
    def do(self):
        print(self.get_name()+'.do')
        zones = self.parent
        # 
        
        net = zones.parent.get_net()
        proj_params = str(net.get_projparams())
        self._proj = pyproj.Proj(proj_params)
        self._offset = net.get_offset()
                
        ids_zone, names_extended, shapes = self.extract_zones_from_jasonfile()

        print('  Found',len(ids_zone),'Zones in jason file.')
        
        for id_zone, name_extended, shape in zip(ids_zone, names_extended, shapes):
            print('    add zone id',id_zone,name_extended)
            lonlats = np.array(shape,dtype = np.float32)
            shapes_3d = self.project(lonlats[:,0],lonlats[:,1])
            print ('      shapes_3d',shapes_3d)
            zones.make(     zonename = id_zone, 
                            name_extended = name_extended,
                            id_landusetype = 6,# 6=mixed landuse as default
                            shape = list(shapes_3d),
                            is_identify_zoneedges = False)
            
                            
        return True
        
    
    def project(self, lons, lats, alts = None):
        """Projecting lon,lat vector in local coordinate system"""
        # TODO: this function could go into net
  
        coords_x, coords_y = self._proj(lons, lats)
        if alts is None:
           alts =  np.zeros(len(lons),dtype = np.float32)
        #print('  ',(coords_x+self._offset[0]).shape,(coords_y+self._offset[1]).shape,alts.shape)
        return np.transpose(np.concatenate(([coords_x+self._offset[0]],[coords_y+self._offset[1]],[alts]), axis=0))
        
    def extract_zones_from_jasonfile(self):
        with open(self.jsonfilepath, "r", encoding="utf-8") as f:
            geojson = json.load(f)
    
        ids_zone = []
        names_extended = []
        shapes = []
        
        for idx, feature in enumerate(geojson["features"]):
            geometry = feature.get("geometry", {})
            props = feature.get("properties", {})
    
            if geometry.get("type") != "Polygon":
                continue
    
            if self.attrname_id is not '':
                zone_id = self.prefix+str(props.get(self.attrname_id))
            else:
                zone_id = self.prefix+str(idx)
       
            if self.attrname_name is not '':
                name = props.get(self.attrname_name)
            else:
                name = ''
    
            # Polygon coordinates: [outer_ring, hole1, hole2, ...]
            coordinates = geometry["coordinates"][0]
    
            ids_zone.append(zone_id)
            names_extended.append(name)
            shapes.append(coordinates)
          
            
        n_zones = len(ids_zone)
   
        return ids_zone,names_extended,shapes
        
class ZonePopulationImporter(Process):
    def __init__(self, zones,  logger = None, **kwargs):
        print('ZonePopulationImporter.__init__')
                
           
        self._init_common(  'zonepopulationimporter', 
                            parent = zones,
                            name = 'Zone Population Generator', 
                            logger = logger,
                            info ='Imports population related zone information from CSV files.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        self.datafilepath = attrsman.add(cm.AttrConf('datafilepath',kwargs.get('datafilepath',''),
                                    groupnames = ['options'],
                                    perm='rw', 
                                    name = 'Data file', 
                                    wildcards = 'CSV file (*.csv)|*.csv',
                                    metatype = 'filepath',
                                    info = "CSV text file with data.",
                                    ))
                                    
        self.sep = attrsman.add(cm.AttrConf( 'sep',kwargs.get('sep',';'),
                            groupnames = ['options'], 
                            options = [',',';'],
                            perm='rw', 
                            name = 'Column separator', 
                            info = 'Column separator character.',
                            ))
        
        self.field_zones = attrsman.add(cm.AttrConf( 'field_zones',kwargs.get('field_zones','zones'),
                                                         groupnames = ['options'],
                                                         name = 'Fieldname for Zones',
                                                         info = """ Fieldname for zone names or IDs. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.""",
                                                         ))
                                                         
        
        self.field_population = attrsman.add(cm.AttrConf( 'field_population',kwargs.get('field_population','population'),
                                                         groupnames = ['options'],
                                                         name = 'Fieldname Population',
                                                         info = """ Fieldname for Population. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.""",
                                                         ))
                                                         
        self.is_est_pop_from_households = attrsman.add(cm.AttrConf( 'is_est_pop_from_households',kwargs.get('is_est_pop_from_households',False),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Est. Pop from Households', 
                            info = """If true, estimate the population from number of households.""",
                            ))
                                                         
        self.field_pop_minors = attrsman.add(cm.AttrConf( 'field_pop_minors',kwargs.get('field_pop_minors',''),
                                                         groupnames = ['options','demographics'],
                                                         name = 'Fieldname Minors',
                                                         info = """ Fieldname for the number of minors in the population. Minors are usually considered persons below 14 years of age. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.""",
                                                         ))
        
        self.field_pop_majors = attrsman.add(cm.AttrConf( 'field_pop_majors',kwargs.get('field_pop_majors',''),
                                                         groupnames = ['options','demographics'],
                                                         name = 'Fieldname Majors',
                                                         info = """ Fieldname for the number of majors in the population. Majors are usually considered persons over 65 years of age. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.""",
                                                         ))
                                                         
        self.field_households_1 = attrsman.add(cm.AttrConf( 'field_households_1',kwargs.get('field_households_1',''),
                                                         groupnames = ['options','households'],
                                                         name = 'Fieldname 1 member households',
                                                         info = """ Fieldname for 1 member households. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.""",
                                                         ))
        
        self.field_households_2 = attrsman.add(cm.AttrConf( 'field_households_2',kwargs.get('field_households_2',''),
                                                         groupnames = ['options','households'],
                                                         name = 'Fieldname 2 members households',
                                                         info = """ Fieldname for 2 members households. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.
                                                                    Leave empty if information is not available.""",
                                                         ))
        self.field_households_3 = attrsman.add(cm.AttrConf( 'field_households_3',kwargs.get('field_households_3',''),
                                                         groupnames = ['options','households'],
                                                         name = 'Fieldname 3 members households',
                                                         info = """ Fieldname for 3 members households. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.
                                                                    Leave empty if information is not available.""",
                                                         ))
        
        
        self.field_households_4 = attrsman.add(cm.AttrConf( 'field_households_4',kwargs.get('field_households_4',''),
                                                         groupnames = ['options','households'],
                                                         name = 'Fieldname 4 members households',
                                                         info = """ Fieldname for 4 members households. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.
                                                                    Leave empty if information is not available.""",
                                                         ))
        
        self.field_households_5 = attrsman.add(cm.AttrConf( 'field_households_5',kwargs.get('field_households_5',''),
                                                         groupnames = ['options','households'],
                                                         name = 'Fieldname 5 members households',
                                                         info = """ Fieldname for 5 members households. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.
                                                                    Leave empty if information is not available.""",
                                                         ))
        self.field_households_6 = attrsman.add(cm.AttrConf( 'field_households_6',kwargs.get('field_households_6',''),
                                                         groupnames = ['options','households'],
                                                         name = 'Fieldname greater 6 members households',
                                                         info = """ Fieldname for greater 6 members households. 
                                                                    Fieldnames must be indicated exactly in the first line of the CSV file in order to identify the correct column.
                                                                    Leave empty if information is not available.""",
                                                         ))
        
        
        
        
        self.is_remove_quotes = attrsman.add(cm.AttrConf( 'is_remove_quotes',kwargs.get('is_remove_quotes',True),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Remove quotes', 
                            info = """If true, Remove quotes of quoted text in the CSV file.""",
                            ))
                            
    def do(self):
        print(self.get_name()+'.do')
        zones = self.parent
        # 
        
        # make a list with all potential column names
        attrnames_check = [ ('population', self.field_population),
                            ('pop_minors',self.field_pop_minors),
                            ('pop_majors',self.field_pop_majors),
                            ]
        attrsman = self.get_attrsman()                    
        for hh_size in range(1,7):# check attrsman.get_group('households'):
            
            #_,_,hh_size_str = option.get_value().split('_')
            hh_size_str = str(hh_size)
            optionname = 'field_households_'+hh_size_str
            attrname = 'households_'+hh_size_str
            fieldname = getattr(self,optionname)
            #print (   optionname,attrname,fieldname)
            attrnames_check.append((attrname,fieldname))
            
            
        
        
        f = open(self.datafilepath,'r', encoding="utf-8")
        indexmap = {}
        
        row = f.readline()
        row = clean_datarow(row, self.is_remove_quotes)
        
        
        cols = row.split(self.sep)
        n_cols = len(cols)
        #print ('first row:',cols)
        if n_cols >2:
            if self.field_zones in cols:
            
                # identify indexes
                ind_zones = cols.index(self.field_zones)
                for attrname,fieldname in attrnames_check:
                    if fieldname in cols:
                        indexmap[attrname] = cols.index(fieldname)
                
                # print ('  indexmap',indexmap)
                # here create available columns in zone table
                # add columns if needed
                if True:# create always 'population' in indexmap:
                    zones.add_col(am.ArrayConf( 'population', 0,
                                                    groupnames = ['parameter','population'], 
                                                    perm='rw', 
                                                    name = 'Population',
                                                    symbol = 'Pop',
                                                    info = 'Total population of zone.',
                                                    ))
                                                    
                if 'pop_minors' in indexmap:
                    zones.add_col(am.ArrayConf( 'pop_minors', 0,
                                                    groupnames = ['parameter','population'], 
                                                    perm='rw', 
                                                    name = 'Minors',
                                                    info = 'Minors are usually considered persons below 14 years of age. '
                                                    ))  
                                                     
                if 'pop_majors' in indexmap:
                    zones.add_col(am.ArrayConf( 'pop_majors', 0,
                                                    groupnames = ['parameter','population'], 
                                                    perm='rw', 
                                                    name = 'Majors',
                                                    info = 'Majors are usually considered persons above 65 years of age. '
                                                    )) 
                                                
                for hh_size in range(1,7):# check attrsman.get_group('households'):
            
                    #_,_,hh_size_str = option.get_value().split('_')
                    hh_size_str = str(hh_size)
                    optionname = 'field_households_'+hh_size_str
                    attrname = 'households_'+hh_size_str
                    if attrname in indexmap:
                        zones.add_col(am.ArrayConf( attrname, 0,
                                                        groupnames = ['parameter','population','households'], 
                                                        perm='rw', 
                                                        name = 'Number of '+hh_size_str+' member households',
                                                        symbol = 'HH_%s'%hh_size_str,
                                                        info = 'Number of '+hh_size_str+' member households in zone.',
                                                        ))
                        
                f.close()
                
                
                # read lines
                f = open(self.datafilepath,'r', encoding="utf-8")
                next(f)
                i_line = 0
                for row in f:
                    i_line += 1
                    #row = f.readline()
                    row = clean_datarow(row, self.is_remove_quotes)
                    row = row.strip() # to remove blank characters
                    print ('imported row:',row)
                    cols = row.split(self.sep)
                    if len(cols) == n_cols:
                        zonename = cols[ind_zones].strip() # to remove blank characters
                        if zones.ids_sumo.has_index(zonename):
                            id_zone = zones.ids_sumo.get_id_from_index(zonename)
                            for attrname, ind in indexmap.items():
                                getattr(zones,attrname)[id_zone] = int(cols[ind])
                        else:
                            print('    unknown zone %s in line %d.'%(zonename,i_line))
                    else:
                        print('    inconsistent number of columns in line %d, %d instead of %d.'%(i_line,len(cols),n_cols))
                
            else:
                print('WARNING in ZonePopulationImporter: no zone column field found.')
        else:
            print('WARNING in ZonePopulationImporter: insufficien number of columns: %d.'%n_cols)
        
        
        if self.is_est_pop_from_households:
            # estimate population from HH, if available
            zones.population.get_value()[:] = 0  # Reset before calculating
            for hh_size in range(1,7):# check attrsman.get_group('households'):
            
                    #_,_,hh_size_str = option.get_value().split('_')
                    hh_size_str = str(hh_size)
                    optionname = 'field_households_'+hh_size_str
                    attrname = 'households_'+hh_size_str
                    if attrname in indexmap:
                        # add number of households * household size to population column
                        #print ('   multiply col',attrname,'with',hh_size)
                        zones.population.set_value(zones.population.get_value() + getattr(zones,attrname).get_value()*hh_size)
                        #print ('   check',getattr(zones,attrname)[1],zones.population[1])
        return True
        
        
class ZoneGenerator(Process):
    def __init__(self, ident='zonegenerator', zones = None,  logger = None, **kwargs):
        print('ZoneGenerator.__init__')
                
           
        self._init_common(  ident, 
                            parent = zones,
                            name = 'Zone Generator', 
                            logger = logger,
                            info ='Generates zones in a given study area.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
                                 
        self.clear_zones = attrsman.add(cm.AttrConf( 'clear_zones',kwargs.get('clear_zones',True),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Clear zones', 
                            info = """If true, delete existing zones.""",
                            ))
                            
        self.method = attrsman.add(cm.AttrConf( 'method',kwargs.get('method',True),
                                                         choices = {'Facilities Clusterization': True, 'Grid Zoning': False},
                                                         groupnames = ['options'],
                                                         name = 'Creation method',
                                                         info = 'Choose wether to create the zones through a clusterization of Facilities and/or Points of Interest, or generating a grid zoning',
                                                         ))
                            
        self.n_zones = attrsman.add(cm.AttrConf( 'n_zones',kwargs.get('n_zones',49),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Number of zones', 
                            info = """Number of zones to be created""",
                            )) 
        self.use_landusetype = attrsman.add(cm.AttrConf( 'use_landusetype',kwargs.get('use_landusetype',True),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Use Landuse Type', 
                            info = """If true, the landuse of facilities and eventually of the points of interest will be used for the eventual cluster analysis and for guessing the landuse type of the created zones.""",
                            ))
        self.include_pois = attrsman.add(cm.AttrConf( 'include_pois',kwargs.get('include_pois',False),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Include POIs', 
                            info = """If true, include points of interest in the eventual cluster analysis and in the eventual guessing of the landuse type related to the created zones.""",
                            ))
        # ~ self.include_nodes = attrsman.add(cm.AttrConf( 'include_nodes',kwargs.get('include_nodes',False),
                            # ~ groupnames = ['options'], 
                            # ~ perm='rw', 
                            # ~ name = 'Include Nodes', 
                            # ~ unit= 'm',
                            # ~ info = """If true, include network Nodes in the clusterization analysis.""",
                            # ~ ))
        self.is_apply_bbox_explicit = attrsman.add(cm.AttrConf( 'is_apply_bbox_explicit',kwargs.get('is_apply_bbox_explicit',False),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Apply expicit bounding box', 
                            info = 'Apply expicit bounding box for the zone creation. If False, landuse boundaries are applied.',
                            ))
        self.bbox_explicit = attrsman.add(cm.ListConf('bbox_explicit',kwargs.get('bbox_explicit',[0.0,0.0,0.0,0.0]), 
                                            groupnames = ['options'], 
                                            name = 'Explicit bounding box', 
                                            info = """Expicit bounding box of the format [x_min, y_min,x_max, y_max]. Empty list means no explicit bounding borders are applied, and the zoning will be performed in the area occupied by facilities.""",
                                            ))
        

                    
    def do(self):
        print(self.get_name()+'.do')

        # links
        zones = self.parent
        facilities = zones.parent.facilities
        landuse = zones.parent
        ids_fac = facilities.get_ids()
        pois = zones.parent.pois
        ids_poi = pois.get_ids()
        
        if self.clear_zones:
            zones.clear()
        if self.method:
            print('k-Mean cluster')
            features = facilities.centroids[ids_fac][:,:-1]
            if self.include_pois:
                features = np.concatenate((features, landuse.pois.coords[zones.parent.pois.get_ids()][:,:-1]), axis=0)
            if self.use_landusetype:
                if self.include_pois:
                    landusetypes = np.transpose(np.array([np.concatenate((facilities.ids_landusetype[ids_fac],
                        pois.ids_landusetype[ids_poi]))]))
                    features = np.concatenate((np.array(features), landusetypes ), axis = 1)
                else:
                    features = np.concatenate((np.array(features), np.transpose(np.array([facilities.ids_landusetype[ids_fac]]))), axis = 1)
            if self.is_apply_bbox_explicit:
                features = features[(features[:,0]>self.bbox_explicit[0])]
                features = features[(features[:,1]>self.bbox_explicit[1])]
                features = features[(features[:,0]<self.bbox_explicit[2])]
                features = features[(features[:,1]<self.bbox_explicit[3])]    
            codebook, label = kmeans2(features, self.n_zones)
            vor = Voronoi(codebook[:,:2])
            zone_name = 1
            regions, vertices = self.voronoi_finite_polygons_2d(vor)
            for region, i in zip(regions, list(range(len(regions)))):
                if region != []:
                    zone_shape = []
                    for vertice in region:  
                        zone_shape.append(np.array([vertices[vertice][0], vertices[vertice][1],0.], dtype = np.float32))

                    if self.use_landusetype:
                        cluster_landusetypes = np.array(features[(label == i)][:,2], dtype = np.int32)
                        if cluster_landusetypes != []:
                            zones.make(zonename = zone_name, shape = zone_shape, id_landusetype = np.bincount(cluster_landusetypes).argmax() )
                        else:
                            zones.make(zonename = zone_name, shape = zone_shape, id_landusetype = landuse.landusetypes.get_id_from_formatted('mixed'))
                    else:
                        zones.make(zonename = zone_name, shape = zone_shape, id_landusetype = landuse.landusetypes.get_id_from_formatted('mixed'))
                    zone_name +=1
        else:
            zone_binx = np.int(np.sqrt(self.n_zones))
            zone_biny = np.int(np.sqrt(self.n_zones))
            if self.is_apply_bbox_explicit:
                x_min = self.bbox_explicit[0]
                y_min = self.bbox_explicit[1]
                x_max = self.bbox_explicit[2]
                y_max = self.bbox_explicit[3]
            else:
                x_min = np.min(facilities.centroids[ids_fac][:,0])
                y_min = np.min(facilities.centroids[ids_fac][:,1])
                x_max = np.max(facilities.centroids[ids_fac][:,0])
                y_max = np.max(facilities.centroids[ids_fac][:,1])
            x_bin = (x_max-x_min)/(zone_binx)
            y_bin = (y_max-y_min)/(zone_biny)
            zone_name = 1
            for i in range(zone_binx):
                for j in range(zone_biny):
                    x_min_i = x_min+i*x_bin
                    y_min_i = y_min+j*y_bin
                    x_max_i = x_min+(i+1)*x_bin
                    y_max_i = y_min+(j+1)*y_bin
                    zone_shape = [np.array([x_min_i,y_min_i,0.], dtype = np.float32), np.array([x_min_i,y_max_i,0.], dtype = np.float32),
                        np.array([x_max_i,y_max_i,0.], dtype = np.float32),np.array([x_max_i,y_min_i,0.], dtype = np.float32)]
                    if self.use_landusetype:
                        zone_landusetypes = facilities.ids_landusetype[ids_fac][(facilities.centroids[ids_fac][:,0]>x_min_i)&(facilities.centroids[ids_fac][:,1]>y_min_i)&(facilities.centroids[ids_fac][:,0]<x_max_i)&(facilities.centroids[ids_fac][:,1]<y_max_i)]
                        if self.include_pois:
                            zone_landusetypes = np.concatenate((zone_landusetypes, pois.ids_landusetype[ids_poi][(pois.coords[ids_poi][:,0]>x_min_i)&(pois.coords[ids_poi][:,1]>y_min_i)&(pois.coords[ids_poi][:,0]<x_max_i)&(pois.coords[ids_poi][:,1]<y_max_i)]))
                        if zone_landusetypes != []:
                            zones.make(zonename = zone_name,  shape = zone_shape, id_landusetype =  np.bincount(zone_landusetypes).argmax() )
                        else:
                            zones.make(zonename = zone_name, shape = zone_shape, id_landusetype = landuse.landusetypes.get_id_from_formatted('mixed'))
                    else:
                        zones.make(zonename = zone_name, shape = zone_shape, id_landusetype = landuse.landusetypes.get_id_from_formatted('mixed'))
                    zone_name += 1
        print('  Done, generated %d zones'%(zone_name-1))

        return True

                    
    def voronoi_finite_polygons_2d(self, vor, radius=None):
        """
        Reconstruct infinite voronoi regions in a 2D diagram to finite
        regions.
        Parameters
        ----------
        vor : Voronoi
            Input diagram
        radius : float, optional
            Distance to 'points at infinity'.
        Returns
        -------
        regions : list of tuples
            Indices of vertices in each revised Voronoi regions.
        vertices : list of tuples
            Coordinates for revised Voronoi vertices. Same as coordinates
            of input vertices, with 'points at infinity' appended to the
            end.
        """
    
        if vor.points.shape[1] != 2:
            raise ValueError("Requires 2D input")
    
        new_regions = []
        new_vertices = vor.vertices.tolist()
    
        center = vor.points.mean(axis=0)
        if radius is None:
            radius = vor.points.ptp().max()*2
    
        # Construct a map containing all ridges for a given point
        all_ridges = {}
        for (p1, p2), (v1, v2) in zip(vor.ridge_points, vor.ridge_vertices):
            all_ridges.setdefault(p1, []).append((p2, v1, v2))
            all_ridges.setdefault(p2, []).append((p1, v1, v2))
    
        # Reconstruct infinite regions
        for p1, region in enumerate(vor.point_region):
            vertices = vor.regions[region]
    
            if all(v >= 0 for v in vertices):
                # finite region
                new_regions.append(vertices)
                continue
    
            # reconstruct a non-finite region
            ridges = all_ridges[p1]
            new_region = [v for v in vertices if v >= 0]
    
            for p2, v1, v2 in ridges:
                if v2 < 0:
                    v1, v2 = v2, v1
                if v1 >= 0:
                    # finite ridge: already in the region
                    continue
    
                # Compute the missing endpoint of an infinite ridge
    
                t = vor.points[p2] - vor.points[p1] # tangent
                t /= np.linalg.norm(t)
                n = np.array([-t[1], t[0]])  # normal
    
                midpoint = vor.points[[p1, p2]].mean(axis=0)
                direction = np.sign(np.dot(midpoint - center, n)) * n
                far_point = vor.vertices[v2] + direction * radius
    
                new_region.append(len(new_vertices))
                new_vertices.append(far_point.tolist())
    
            # sort region counterclockwise
            vs = np.asarray([new_vertices[v] for v in new_region])
            c = vs.mean(axis=0)
            angles = np.arctan2(vs[:,1] - c[1], vs[:,0] - c[0])
            new_region = np.array(new_region)[np.argsort(angles)]
    
            # finish
            new_regions.append(new_region.tolist())
    
        return new_regions, np.asarray(new_vertices)
    

class FacilityGenerator(Process):
    def __init__(self, ident='facilitygenerator', facilities = None,  logger = None, **kwargs):
        print('FacilityGenerator.__init__')
        
        # TODO: let this be independent, link to it or child??
        
           
        self._init_common(  ident, 
                            parent = facilities,
                            name = 'Facility Generator', 
                            logger = logger,
                            info ='Generates facilities (buildigs, factories, parks, etc.) in a given street network.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        
        # make for each possible pattern a field for prob
                         
        
        self.edgelength_min = attrsman.add(cm.AttrConf( 'edgelength_min',kwargs.get('edgelength_min',50.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Minimum edge length', 
                            unit= 'm',
                            info = """Minimum edge length for which houses are generated.""",
                            ))
        
        self.priority_max = attrsman.add(cm.AttrConf( 'priority_max',kwargs.get('priority_max',7),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Max. priority', 
                            info = """Maximum edge priority where facilities will be created.""",
                            )) 
                            
        self.height_max = attrsman.add(cm.AttrConf( 'height_max',kwargs.get('height_max',20.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            unit= 'm',
                            name = 'Max facility height', 
                            info = """Maximum height of facilities.""",
                            ))
        
        
        self.capacity_max = attrsman.add(cm.AttrConf( 'capacity_max',kwargs.get('capacity_max',1000),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Max. facility capacity', 
                            info = """Maximum capacity of a facility. Capacity is the number of adulds living in a house or working in a factory.""",
                            ))
                            
        self.n_retry = attrsman.add(cm.AttrConf( 'n_retry',kwargs.get('n_retry',5),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Retry number', 
                            info = """Number of times the algorithm is trying to fit a facility in a road-gap.""",
                            ))
                                                
        #self.id_facilitytype = attrsman.add(cm.AttrConf( 'id_facilitytype',kwargs.get('id_facilitytype',1),
        #                    groupnames = ['options'], 
        #                    perm='rw', 
        #                    choices = self.parent.facilities.facilitytypes.get_value().names.get_indexmap(),
        #                    name = 'Facility type', 
        #                    info = """Facility type to be generated.""",
        #                    ))
                                                
        
    
    def do(self):
        print(self.get_name()+'.do')
        # links
        facilities = self.parent
        net = facilities.parent.get_net()
        edges = net.edges
        nodes = net.nodes
        #self._edges = edges
        
        #self._segvertices = edges.get_segvertices_xy()
        x1,y1,x2,y2 = edges.get_segvertices_xy()
        
        logger = self.get_logger()
        
        
        ids_edge = edges.select_ids(    (edges.widths_sidewalk.get_value()>0)\
                                        &(edges.lengths.get_value()>self.edgelength_min)\
                                        &(edges.priorities.get_value()<self.priority_max)
                                        )
        facilitytypes = facilities.facilitytypes.get_value()
        
        # here we can make a selection 
        facilitytypeobjs = facilitytypes.typeobjects[facilitytypes.get_ids()]
        
        #print '  facilitytypes, facilitytypeobjs',facilitytypes,facilitytypeobjs
        n_factypes = len(facilitytypes)
        n_fac = 0
        n_edges = len(ids_edge)
        #logger.w('Add facilities to %d edges')
        #print '  eligible edges =',ids_edge
        i = 0.0
        for id_edge, edgelength,id_fromnode,id_tonode, shape, edgewidth\
                in zip( ids_edge,edges.lengths[ids_edge],
                        edges.ids_fromnode[ids_edge],
                        edges.ids_tonode[ids_edge], 
                        edges.shapes[ids_edge],
                        edges.widths[ids_edge],
                        ):
            pos = 5.0
            #print '  Build at edge',id_edge,edgelength
            #logger.w('Add facilities to %d edges')
            i +=1 
            logger.progress(i/n_edges*100)
            
            
            # identify opposite edge, which needs to be excluded
            # from bulding overlapping check
            if (nodes.ids_incoming[id_fromnode] is not None)\
                &(nodes.ids_outgoing[id_tonode] is not None):
                ids_incoming_fomnode = set(nodes.ids_incoming[id_fromnode])
                ids_outgoing_tonode = set(nodes.ids_outgoing[id_tonode])
                
                id_edge_opp_set = ids_incoming_fomnode.intersection(ids_outgoing_tonode)
                if len(id_edge_opp_set) >0:
                    id_edge_opp = id_edge_opp_set.pop()
                    inds_seg_opp = edges.get_inds_seg_from_id_edge(id_edge_opp)
                else:
                    #print '   no edge in opposite direction'
                    id_edge_opp = -1
                    inds_seg_opp = None
            else:
                 id_edge_opp = -1
                 inds_seg_opp = None   
            #ids_tonode_outgoing = edges.ids_tonode[nodes.ids_outgoing[id_tonode]]   
            #net.get_ids_edge_from_inds_seg(inds_seg)
            #net.get_inds_seg_from_id_edge(id_edge)
            
            
            while pos < edgelength:
                facilitytype = facilitytypeobjs[0]# could be according to statistics
                #print '    next position',pos
                n_trials = self.n_retry
                is_success = False
                while (n_trials>0) & (not is_success):
                    length_fac = random.uniform(facilitytype.length_min, facilitytype.length_max)
                    width_fac = random.uniform(facilitytype.width_min, facilitytype.width_max)
                    
                    # fix from to positions
                    pos11 = pos
                    pos21 = pos + length_fac
                    if pos21 < edgelength:
                        #print '      try place',n_trials,facilitytype,'id_edge',id_edge,pos11,pos21,edgelength
                        
                        coord11, angle = get_coord_angle_on_polyline_from_pos(shape, pos11)
                        dxn = np.cos(angle-np.pi/2)
                        dyn = np.sin(angle-np.pi/2)
                        coord12 = [coord11[0]+width_fac*dxn, coord11[1]+width_fac*dyn, coord11[2]]
                        
                        coord21, angle = get_coord_angle_on_polyline_from_pos(shape, pos21)
                        dxn = np.cos(angle-np.pi/2)
                        dyn = np.sin(angle-np.pi/2)
                        coord22 = [coord21[0]+width_fac*dxn, coord21[1]+width_fac*dyn, coord21[2]]
                        
                        id_edge1 = edges.get_ids_edge_from_inds_seg(self.get_segind_closest_edge(coord12, x1,y1,x2,y2, inds_seg_exclude = inds_seg_opp))
                        
                        #id_edge2 = edges.get_ids_edge_from_inds_seg(self.get_segind_closest_edge(coord22, x1,y1,x2,y2, inds_seg_exclude = inds_seg_opp))
                        #print '      id_edge,id_edge1,id_edge2',id_edge,id_edge1,id_edge2
                        #print '      shape =',np.array([coord11, coord12, coord22, coord21,], dtype = np.float32)
                        if id_edge1 == id_edge:
                            id_edge2 = edges.get_ids_edge_from_inds_seg(self.get_segind_closest_edge(coord22, x1,y1,x2,y2, inds_seg_exclude = inds_seg_opp))
                        
                            if id_edge2 == id_edge:
                                id_fac = facilities.generate(   facilitytype, 
                                                                offset = coord11,# offset
                                                                length = length_fac,
                                                                width = width_fac,
                                                                #bbox = [coord11, coord12, coord22, coord21,],
                                                                id_landusetype = None,
                                                                angle = angle,
                                                                pos_edge =  pos11,
                                                                capacity = self.capacity_max,# could be function of dist to center/pop
                                                                height_max = self.height_max,# could be function of dist to center
                                                                id_edge = id_edge,
                                                                width_edge = edgewidth,
                                                                )
                                
                                if id_fac != -1:
                                    #print '    ****generation successful id_fac=',id_fac
                                    is_success = True
                                    n_fac += 1
                            
                    n_trials -= 1
                    
                pos = pos21
                #print '    update with pos',pos   
                # generate a parallel shape with distance  width_fac
                #angles_perb = get_angles_perpendicular(shape)
                #dxn = np.cos(angles_perb)
                #dyn = np.sin(angles_perb)
                #shape2 = np.zeros(shape.shape, np.float32)
                #shape2[:,0] = dxn*width_fac + shape[:,0]
                #shape2[:,1] = dyn*width_fac + shape[:,1]
                #shape2[:,2] = shape[:,2]
                
                # check if positions on parallel shape are closest to
                # this edge or closer to another edge
        print('  Done, generated %d facilities'%n_fac)
        return True
    
    def get_segind_closest_edge(self, p, x1,y1,x2,y2, inds_seg_exclude = None):         
        d2 = get_dist_point_to_segs(p[0:2],x1,y1,x2,y2, is_ending=True)
        if inds_seg_exclude is not None:
            d2[inds_seg_exclude] = np.inf
        #print '  min(d2)=',np.min(d2),'argmin=',np.argmin(d2),self.get_ids(self._edgeinds[np.argmin(d2)])
        return np.argmin(d2)
    

class ParkingGenerator(Process):
    def __init__(self, ident='parkinggenerator', parking=None,  logger = None, **kwargs):
        print('ParkingGenerator.__init__')
        
        # TODO: let this be independent, link to it or child??
        
           
        self._init_common(  ident, 
                            parent = parking,
                            name = 'On Road parking generator', 
                            logger = logger,
                            info ='Generates on road parking.',
                            )
        
        
        attrsman = self.set_attrsman(cm.Attrsman(self))
        scenario = parking.parent.parent

        self.id_mode = attrsman.add(cm.AttrConf( 'id_mode',kwargs.get('id_mode',MODES['passenger']),
                            groupnames = ['options'], 
                            choices = scenario.net.modes.names.get_indexmap(),
                            perm='rw', 
                            name = 'Mode ID', 
                            info = """Mode of parked vehicles. This is to select lanes which must be accessible for this mode.""",
                            ))
        
        self.id_mode_fallback = attrsman.add(cm.AttrConf( 'id_mode_fallback',kwargs.get('id_mode_fallback',MODES['private']),
                            groupnames = ['options'], 
                            choices = scenario.net.modes.names.get_indexmap(),
                            perm='rw', 
                            name = 'Fallback mode ID', 
                            info = """Fallback mode of parked vehicles. If parking is not accessible by the primary mode then it is tried if parking is accessible by the fallback mode.""",
                            ))
        
        self.length_min = attrsman.add(cm.AttrConf( 'length_min',kwargs.get('length_min',42.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            unit= 'm',
                            name = 'Min. edge length', 
                            info = """Minimum edge length in order to qualify for parking.""",
                            ))
        
        self.length_noparking = attrsman.add(cm.AttrConf( 'length_noparking',kwargs.get('length_noparking',15.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            unit= 'm',
                            name = 'No parking length', 
                            info = """Length from junction to the first or last parking on an edge.""",
                            ))
        
        
        self.length_lot = attrsman.add(cm.AttrConf( 'length_lot',kwargs.get('length_lot',6.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            unit= 'm',
                            name = 'Lot length', 
                            info = """Length of a single parking lot.""",
                            ))  
                                              
        self.angle = attrsman.add(cm.AttrConf( 'angle',kwargs.get('angle',0.0),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Angle', 
                            info = """Angle of parking with respect ti lane direction. Currently only 0.0 is possible.""",
                            ))
        
        self.is_selected_zones = attrsman.add(cm.AttrConf(  'is_selected_zones', kwargs.get('is_selected_zones',False),
                                    groupnames = ['options'], 
                                    name = 'Selected zones',
                                    info = "Place parking only on edges of specified zone list.",
                                    )) 
                         
        zones = scenario.landuse.zones
        ids_zone = zones.get_ids()
        zonechoices = {}
        for id_zone, name_zone in zip(ids_zone, zones.ids_sumo[ids_zone]):
              zonechoices[name_zone] = id_zone
        #print '  zonechoices',zonechoices
        #make for each possible pattern a field for prob
        #if len(zonechoices) > 0:
        self.ids_zone = attrsman.add(cm.ListConf('ids_zone',kwargs.get('ids_zone',[]), 
                                                  groupnames = ['options'], 
                                                  choices = zonechoices,
                                                  name = 'Zones', 
                                                  info = """Zones where to place taxis. Taxis are distributed proportional to road lengths in zones.""",
                                                  ))
        
        
        self.is_near_facilities = attrsman.add(cm.AttrConf(  'is_near_facilities', kwargs.get('is_near_facilities',False),
                                    groupnames = ['options'], 
                                    name = 'Parking only near facilities',
                                    info = "This would place parking only if facilities have an access to this edge.",
                                    ))       
        
        # not needed parking only on footpath
        #self.is_with_footpath = attrsman.add(cm.AttrConf(  'is_with_footpath', kwargs.get('is_with_footpath',False),
        #                            groupnames = ['options'], 
        #                            name = 'Only road has footpath',
        #                            info = "This would place parking only if t this edge.",
        #                            ))  
                                                                                                                       
        self.priority_max = attrsman.add(cm.AttrConf( 'priority_max',kwargs.get('priority_max',7),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Max. priority', 
                            info = """Maximum edge priority where parkings will be created.""",
                            ))                    
        
        self.speed_max = attrsman.add(cm.AttrConf( 'speed_max',kwargs.get('speed_max',13.89),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Max. speed', 
                            info = """Maximum allowed speed on edge where parkings will be created.""",
                            ))

        self.n_freelanes_min = attrsman.add(cm.AttrConf( 'n_freelanes_min',kwargs.get('n_freelanes_min',1),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Min. free lanes', 
                            info = """Minimum number of free lanes on the edge. These is the minimum number of lanes excluding the parking lane.""",
                            ))                                           
        self.is_clear = attrsman.add(cm.AttrConf( 'is_clear',kwargs.get('is_clear',True),
                            groupnames = ['options'], 
                            perm='rw', 
                            name = 'Clear', 
                            info = """Clear previous parking areas from ntework.""",
                            ))   
    
    def do(self):
        print(self.get_name()+'.do')
        # links
        #print '  self.id_mode',self.id_mode
        #print '  self.get_kwoptions()',self.get_kwoptions()
        logger = self.get_logger()
        
        
        self.parent.make_parking(logger = logger,**self.get_kwoptions() )
        return True
        
class OsmPolyImporter(CmlMixin,Process):
    def __init__(self,  landuse = None,  
                        osmfilepaths = None, 
                        typefilepath = None,
                        polyfilepath = None, 
                        projparams = None,
                        offset_x = None, 
                        offset_y = None,   
                        is_keep_full_type = True,
                        is_import_all_attributes = True,
                        is_use_name_for_id = False,
                        polytypefilepath = '',
                        is_clean_osmfile = True,
                        is_merge = False,
                        logger = None,**kwargs):
        print('OsmPolyImporter.__init__',landuse,landuse.parent.get_rootfilename())
        self._init_common(  'osmpolyimporter', name = 'OSM Poly import', 
                            logger = logger,
                            info ='Converts a OSM  file to a SUMO Poly file and read facilities into scenario.',
                            )
        if landuse  is None:
            self._landuse = Landuse()
        else:
            self._landuse = landuse
        
        self.init_cml('polyconvert')# pass main shell command
        
        
            
        
        if landuse.parent  is not None:
                scenario = landuse.parent
                rootname = scenario.get_rootfilename()
                rootdirpath = scenario.get_workdirpath()
                if hasattr(scenario,'net'):
                    if projparams  is None:
                        projparams = scenario.net.get_projparams()
                    if (offset_x is None)&(offset_y is None):
                        offset_x, offset_y = scenario.net.get_offset()
        else:
                rootname = landuse.get_ident()
                rootdirpath = os.getcwd()
               
                
        if polyfilepath  is None:
            polyfilepath =os.path.join(rootdirpath,rootname+'.poly.xml')
        
        if  osmfilepaths  is None: 
            osmfilepaths = os.path.join(rootdirpath,rootname+'.osm.xml')
        
        if typefilepath  is None:
            if 'SUMO_HOME' in os.environ:
                typemapdir = os.path.join(os.environ['SUMO_HOME'], 'data','typemap')
                typefilepath = os.path.join(typemapdir,'osmPolyconvert.typ.xml')
            else:
                print("No poly typemaps found. Please declare environment variable 'SUMO_HOME'")
                typefilepath = ''
            
     
        
        
        attrsman = self.get_attrsman()
        
        self.rootdirpath = rootdirpath
        
        self.rootname = rootname
        
        self.add_option('osmfilepaths',osmfilepaths,
                        groupnames = ['options'],# 
                        cml = '--osm-files',
                        perm='rw', 
                        name = 'OSM files', 
                        wildcards = 'OSM XML files (*.osm)|*.osm*',
                        metatype = 'filepaths',
                        info = 'Openstreetmap files to be imported.',
                        )
        
        
        
        self.is_merge = attrsman.add(cm.AttrConf(  'is_merge',is_merge,
                        groupnames = ['options'], 
                        perm='rw', 
                        name = 'Merge', 
                        info = 'If set, imported polygons are merged with existing.',
                        ))
                        
        self.is_clean_osmfile = attrsman.add(cm.AttrConf(  'is_clean_osmfile',is_clean_osmfile,
                        groupnames = ['options'], 
                        perm='rw', 
                        name = 'Clean OSM files', 
                        info = 'If set, OSM files are cleaned from strange characters prior to import (recommended).',
                        ))
                                        
        self.height_default = attrsman.add(cm.AttrConf(  'height_default',kwargs.get('height_default',7.0),
                        groupnames = ['options'], 
                        perm='rw', 
                        name = 'Default height', 
                        info = 'Default height of facilities in case no height information is available.',
                        ))
                        
        self.type_default = attrsman.add(cm.AttrConf(  'type_default',kwargs.get('type_default','building.yes'),
                        groupnames = ['options'], 
                        perm='rw', 
                        name = 'Default facility type', 
                        info = 'Default type of facilities in case no type information is available.',
                        ))                
        
        self.add_option('polyfilepath',polyfilepath,
                        groupnames = ['_private'],# 
                        cml = '--output-file',
                        perm='r', 
                        name = 'Poly file', 
                        wildcards = 'Poly XML files (*.poly.xml)|*.poly.xml',
                        metatype = 'filepath',
                        info = 'SUMO Poly file in XML format.',
                        )
        
        self.add_option('typefilepath',typefilepath,
                        groupnames = ['options'],
                        cml = '--type-file',
                        perm='rw', 
                        name = 'Type file', 
                        wildcards = 'Typemap XML files (*.typ.xml)|*.typ.xml',
                        metatype = 'filepath',
                        info = """Typemap XML files. In these file, 
OSM building types are mapped to specific facility parameters, is not explicitely set by OSM attributes.""",
                        )
                        
        # --net-file <FILE>     Loads SUMO-network FILE as reference to offset and projection 
        self.add_option('projparams',projparams,
                        groupnames = ['options'],# 
                        cml = '--proj',
                        perm='rw', 
                        name = 'projection', 
                        info = 'Uses STR as proj.4 definition for projection. Default is the projection of the network, better do not touch!',
                        is_enabled = lambda self: self.projparams  is not None,
                        )                
        
        self.add_option('offset_x',offset_x,
                        groupnames = ['options','geometry'],# 
                        cml = '--offset.x ',
                        perm='rw', 
                        unit = 'm',
                        name = 'X-Offset', 
                        info = 'Adds offset to net x-positions; default: 0.0',
                        is_enabled = lambda self: self.offset_x  is not None,
                        )
        self.add_option('offset_y',offset_y,
                        groupnames = ['options','geometry'],# 
                        cml = '--offset.y ',
                        perm='rw', 
                        unit = 'm',
                        name = 'Y-Offset', 
                        info = 'Adds offset to net x-positions; default: 0.0',
                        is_enabled = lambda self: self.offset_y  is not None,
                        )
        
        self.add_option('is_keep_full_type',is_keep_full_type,
                        groupnames = ['options'],# 
                        cml = '--osm.keep-full-type',
                        perm='rw', 
                        name = 'keep full OSM type', 
                        info = 'The type will be made of the key-value - pair.',
                        )
        
        self.add_option('is_import_all_attributes',is_keep_full_type,
                        groupnames = ['options'],# 
                        cml = '--all-attributes',
                        perm='rw', 
                        name = 'import all attributes', 
                        info = 'Imports all OSM attributes.',
                        )
        
        
                        
        self.add_option('is_use_name_for_id',is_use_name_for_id,
                        groupnames = ['options'],# 
                        cml = '--osm.use-name',
                        perm='rw', 
                        name = 'use OSM name for id', 
                        info = '     The OSM id (not internal ID) will be set from the given OSM name attribute.',
                        )
        
        self.add_option('polytypefilepath',polytypefilepath,
                        groupnames = [],#['_private'],# 
                        cml = '--type-file',
                        perm='rw', 
                        name = 'Poly type file', 
                        wildcards = 'Net XML files (*.xml)|*.xml',
                        metatype = 'filepath',
                        info = 'SUMO Poly type file in XML format.',
                        is_enabled = lambda self: self.polytypefilepath != '',
                        )
    def update_params(self):
        """
        Make all parameters consistent.
        example: used by import OSM to calculate/update number of tiles
        from process dialog
        """
        pass
        #self.workdirpath = os.path.dirname(self.netfilepath)
        #bn =  os.path.basename(self.netfilepath).split('.')
        #if len(bn)>0:
        #    self.rootname = bn[0]
    
    def do(self):
        self.update_params()
        cml = self.get_cml()
        
        
        if self.is_clean_osmfile:
            for path in self.osmfilepaths.split(','):
                path_temp = path+'.clean'
                clean_osm(path, path_temp)
                #shutil.copy (path_temp, path)
                shutil.move(path_temp, path)
        
        #print 'SumonetImporter.do',cml
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        self.run_cml(cml)
        if self.status == 'success':
            if os.path.isfile(self.polyfilepath):
                print('  OSM->poly.xml successful, start importing xml files')
                self._landuse.import_polyxml(   self.rootname, self.rootdirpath, 
                                                is_clear = not self.is_merge,
                                                type_default = self.type_default,
                                                height_default = self.height_default)
                
                print('  import poly in hybridPY done.')
                return True
            return False
        else:
            return False
    
    
    def get_landuse(self):
        # used to het landuse in case landuse has been created  
        return self._landuse
    

                                            
if __name__ == '__main__':          
    ###############################################################################
    #print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #from coremodules.scenario import scenario
    from coremodules.network import network
    logger = Logger()
    NETPATH = os.path.join(hybridPYDIR,'coremodules','network','testnet')
    net = network.Network(logger = logger)
    rootname = 'facsp2'
    net.import_xml(rootname, NETPATH)
    #net.read_sumonodes(os.path.join(NETPATH,'facsp2.nod.xml'))
    #net.read_sumoedges(os.path.join(NETPATH,'facsp2.edg.xml'))
    landuse = Landuse(net = net, logger = logger)
    
    #landuse.facilities.import_poly(os.path.join(NETPATH,'facsp2.poly.xml'))
    landuse.import_xml(rootname, NETPATH)
    objbrowser(landuse)
    
    
