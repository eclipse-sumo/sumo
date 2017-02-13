import os
import sys
import time
from xml.sax import saxutils, parse, handler
if __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR, '..', '..')
    sys.path.append(SUMOPYDIR)


from coremodules.modules_common import *
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.geometry import *
from agilepy.lib_base.processes import Process, CmlMixin
from coremodules.network.network import SumoIdsConf, MODES
import maps


def clean_osm(filepath_in, filepath_out):
    """
    Clean osm file from strange characters that compromize importing.
    """
    #
    substitutes = {"&quot;": "'", "&": "+"}
    fd_in = open(filepath_in, 'r')
    fd_out = open(filepath_out, 'w')
    for line in fd_in.readlines():
        for oldstr, newstr in substitutes.iteritems():
            line = line.replace(oldstr, newstr)
        fd_out.write(line)
    fd_in.close()
    fd_out.close()


class LanduseTypes(am.ArrayObjman):

    def __init__(self, parent, is_add_default=True, **kwargs):

        self._init_objman(ident='landusetypes',
                          parent=parent,
                          name='Landuse types',
                          info='Table with information on landuse types',
                          **kwargs)

        self._init_attributes()
        if is_add_default:
            self.add_types_default()

    def _init_attributes(self):
        # landuse types table
        self.add_col(am.ArrayConf('typekeys', '',
                                  is_index=True,
                                  # actually a string of variable length
                                  dtype='object',
                                  perm='r',
                                  name='Type',
                                  info='Type of facility. Must be unique, because used as key.',
                                  ))

        # self.add_col(am.ArrayConf( 'osmid', '',
        #                                dtype = 'object',# actually a string of variable length
        #                                perm='rw',
        #                                name = 'Name',
        #                                info = 'Name of facility type used as reference in OSM.',
        #                                ))

        self.add_col(am.ListArrayConf('osmfilters',
                                      perm='r',
                                      name='OSM filter',
                                      info='List of openstreetmap filters that allow to identify this facility type.',
                                      ))

        self.add_col(am.ArrayConf('colors', np.ones(4, np.float32),
                                  dtype=np.float32,
                                  metatype='color',
                                  perm='rw',
                                  name='Colors',
                                  info="Color corrispondig to landuse type as RGBA tuple with values from 0.0 to 1.0",
                                  xmltag='color',
                                  ))

    def add_types_default(self):
        # default types
        self.add_row(typekeys='leisure',
                     osmfilters=['sport', 'leisure.park', 'park'],
                     colors=(0.5, 0.8, 0.6, 1.0)
                     )
        self.add_row(typekeys='commercial',
                     osmfilters=['shop.*', 'building.commercial'],
                     colors=(0.6171875,  0.6171875,  0.875, 1.0),
                     )
        self.add_row(typekeys='industrial',
                     osmfilters=['building.industrial'],
                     colors=(0.89453125,  0.65625,  0.63671875, 1.0),
                     )
        self.add_row(typekeys='parking',
                     osmfilters=['building.parking', 'amenity.parking'],
                     colors=(0.52734375,  0.875,  0.875, 1.0),
                     )
        self.add_row(typekeys='residential',
                     osmfilters=['building.*', 'building'],
                     colors=(0.921875,  0.78125,  0.4375, 1.0),
                     )


class Zones(am.ArrayObjman):

    def __init__(self, parent, edges, **kwargs):
        self._init_objman(ident='zones', parent=parent,
                          name='Zones',
                          info='Traffic Assignment Zones (TAZ)',
                          is_plugin=True,
                          **kwargs)

        self.add_col(SumoIdsConf('Zone', name='Name', perm='rw'))
        self.add_col(am.ArrayConf('coords',  np.zeros(3, dtype=np.float32),
                                  groupnames=['state'],
                                  perm='r',
                                  name='Coords',
                                  unit='m',
                                  info='Zone center coordinates.',
                                  is_plugin=True,
                                  ))

        self.add_col(am.ListArrayConf('shapes',
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Shape',
                                      unit='m',
                                      info='List of 3D Shape coordinates delimiting a zone.',
                                      is_plugin=True,
                                      ))

        self.add_col(am.IdlistsArrayConf('ids_edges_orig', edges,
                                         groupnames=['state'],
                                         name='IDs orig edges',
                                         info='List with IDs of network edges that can be used as origins for trips in this zone.',
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_edges_dest', edges,
                                         groupnames=['state'],
                                         name='IDs dest edges',
                                         info='List with IDs of network edges that can be used as origins for trips in this zone.',
                                         ))

        self.add_col(am.ListArrayConf('probs_edges_orig',
                                      groupnames=['state'],
                                      # perm='rw',
                                      name='edge probs origin',
                                      info='Probabilities of edges to be at the origin of a trip departing from this zone.',
                                      ))
        self.add_col(am.ListArrayConf('probs_edges_dest',
                                      groupnames=['state'],
                                      # perm='rw',
                                      name='edge probs dest',
                                      info='Probabilities of edges to be a destination of a trip arriving at this zone.',
                                      ))

    def make(self, zonename='', coord=np.zeros(3, dtype=np.float32),  shape=[]):
        """
        Add a zone
        """
        # print 'Zone.make',coord
        # print '  shape',type(shape),shape

        self.get_coords_from_shape(shape)
        id_zone = self.add_row(coords=self.get_coords_from_shape(shape),
                               shapes=shape,
                               )
        if zonename == '':
            self.ids_sumo[id_zone] = str(id_zone)
        else:
            self.ids_sumo[id_zone] = zonename

        self.identify_zoneedges(id_zone)
        # print '   shapes\n',self.shapes.value
        # print '   zone.shapes[id_zone]\n',self.shapes[id_zone]

        return id_zone

    def get_coords_from_shape(self, shape):
        # print
        # 'get_coords_from_shape',np.array(shape),np.mean(np.array(shape),0)
        return np.mean(np.array(shape), 0)

    def del_element(self, id_zone):
        # print 'del_element',id_zone
        self.del_row(id_zone)

    def get_edges(self):
        return self.ids_edges_dest.get_linktab()

    def refresh_zoneedges(self):
        for _id in self.get_ids():
            self.identify_zoneedges(_id)
            self.make_egdeprobs(_id)

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        # self.zones.update_netoffset(deltaoffset)
        self.coords.value[:, :2] = self.coords.value[:, :2] + deltaoffset
        shapes = self.shapes.value
        for i in xrange(len(shapes)):
            s = np.array(shapes[i])
            s[:, :2] = s[:, :2] + deltaoffset
            shapes[i] = list(s)

    def identify_zoneedges(self, id_zone):
        # print 'identify_zoneedges',id_zone
        inds_within = []
        ind = 0
        # print '  self.shapes[id_zone]',self.shapes[id_zone]

        polygon = np.array(self.shapes[id_zone])[:, :2]
        for polyline in self.get_edges().shapes.value:
            # print '  polygon',polygon,type(polygon)
            # print '
            # np.array(polyline)[:,:2]',np.array(polyline)[:,:2],type(np.array(polyline)[:,:2])
            if is_polyline_in_polygon(np.array(polyline)[:, :2], polygon):
                inds_within.append(ind)
            ind += 1

        # print '  inds_within',inds_within

        # select and determine weights
        self.ids_edges_orig[id_zone] = self.get_edges().get_ids(inds_within)
        self.ids_edges_dest[id_zone] = self.get_edges().get_ids(inds_within)

    def make_egdeprobs(self, id_zone):
        """
        Returns two dictionaries with normalized edge weight distribution 
        one for departures and one for arrivals.

        The  dictionaries have id_zone as key and a and an array of edge weights as value. 
        """
        #zones = self.zones.value
        #edgeweights_orig = {}
        #edgeweights_dest = {}

        # for id_zone in zones.get_ids():
        n_edges_orig = len(self.ids_edges_orig[id_zone])
        n_edges_dest = len(self.ids_edges_dest[id_zone])
        # da fare meglio...
        if n_edges_orig > 0:
            self.probs_edges_orig[id_zone] = 1.0 / \
                float(n_edges_orig) * np.ones(n_edges_orig, np.float)
        else:
            self.probs_edges_orig[id_zone] = 1.0

        if n_edges_dest > 0:
            self.probs_edges_dest[id_zone] = 1.0 / \
                float(n_edges_dest) * np.ones(n_edges_dest, np.float)
        else:
            self.probs_edges_dest[id_zone] = 1.0


class Facilities(am.ArrayObjman):

    def __init__(self, parent, landusetypes, zones, net=None, **kwargs):
        # print 'Facilities.__init__',hasattr(self,'lanes')
        self._init_objman(ident='facilities',
                          parent=parent,
                          name='Facilities',
                          info='Information on buildings, their type of usage and access to the transport network.',
                          xmltag=('polys', 'poly', 'ids_sumo'),
                          **kwargs)

        self.add_col(SumoIdsConf('Facility', info='SUMO facility ID'))

        self.add_col(am.IdsArrayConf('ids_landusetype', landusetypes,
                                     groupnames=['landuse'],
                                     choices=landusetypes.typekeys.get_indexmap(),
                                     name='ID landuse',
                                     info='ID of landuse.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_zone', zones,
                                     groupnames=['landuse'],
                                     choices=zones.ids_sumo.get_indexmap(),
                                     name='ID zone',
                                     info='ID of traffic zone, where this facility is located.',
                                     ))

        self.add_col(am.ArrayConf('osmkeys', '',
                                  # actually a string of variable length
                                  dtype='object',
                                  perm='rw',
                                  name='OSM key',
                                  info='OSM key of facility.',
                                  xmltag='type',
                                  ))

        self.add_col(am.ArrayConf('areas', 0.0,
                                  dtype=np.float32,
                                  groupnames=['landuse'],
                                  perm='r',
                                  name='Area',
                                  unit='m^2',
                                  info='Area of this facility.',
                                  ))

        self.add_col(am.ArrayConf('centroids',  np.zeros(3, dtype=np.float32),
                                  dtype=np.float32,
                                  groupnames=['state', '_private'],
                                  perm='r',
                                  name='Center',
                                  unit='m',
                                  info='Center coordinates of this facility.',
                                  ))

        self.add_col(am.ListArrayConf('shapes',
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Shape',
                                      unit='m',
                                      info='List of 3D Shape coordinates of facility.',
                                      xmltag='shape',
                                      ))

        # configure only if net is initialized
        if net != None:
            self.add_col(am.IdsArrayConf('ids_roadedge_closest', net.edges,
                                         groupnames=['landuse'],
                                         name='Road edge ID',
                                         info='ID of road edge which is closest to this facility.',
                                         ))

            self.add_col(am.ArrayConf('positions_roadedge_closest', 0.0,
                                      dtype=np.float32,
                                      groupnames=['landuse'],
                                      perm='r',
                                      name='Road edge pos',
                                      unit='m',
                                      info='Position on road edge which is closest to this facility',
                                      ))

            # self.ids_stop_closest = self.facilities.add(cm.ArrayConf( 'ids_stop_closest', None,
            #                        dtype = 'object',
            #                        name = 'ID stops',
            #                        perm='rw',
            #                        info = 'List of IDs of closest public transport stops.',
            #                        ))

    def write_xml(self, fd, indent=0, is_print_begin_end=True):
        xmltag, xmltag_item, attrname_id = self.xmltag
        layer_default = -1
        fill_default = 1
        ids_landusetype = self.ids_landusetype
        landusecolors = self.get_landusetypes().colors

        if is_print_begin_end:
            fd.write(xm.begin(xmltag, indent))

        attrsconfigs_write = [self.ids_sumo, self.osmkeys, self.shapes]
        for _id in self.get_ids():
            fd.write(xm.start(xmltag_item, indent + 2))
            for attrsconfig in attrsconfigs_write:
                attrsconfig.write_xml(fd, _id)

            landusecolors.write_xml(fd, ids_landusetype[_id])
            fd.write(xm.num('layer', layer_default))
            fd.write(xm.num('fill', fill_default))

            fd.write(xm.stopit())

        if is_print_begin_end:
            fd.write(xm.end(xmltag, indent))

    def get_landusetypes(self):
        return self.ids_landusetype.get_linktab()

    def get_net(self):
        # print
        # 'get_net',self.ids_edge_closest_road.get_linktab(),self.ids_edge_closest_road.get_linktab().parent
        return self.ids_roadedge_closest.get_linktab().parent
        # if self.parent != None:
        #    return self.parent.get_net()
        # else:
        #    return None

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        # self.zones.update_netoffset(deltaoffset)
        self.centroids.value[:, :2] = self.centroids.value[:, :2] + deltaoffset
        shapes = self.shapes.value
        for i in xrange(len(shapes)):
            s = np.array(shapes[i])
            s[:, :2] = s[:, :2] + deltaoffset
            shapes[i] = list(s)

    def get_edges(self):
        return self.ids_roadedge_closest.get_linktab()

    def identify_taz(self):
        """
        Identifies id of traffic assignment zone for each facility.
        Note that not all facilities are within such a zone.
        """
        zones = self.ids_zone.get_linktab()
        # self.get_demand().get_districts()
        for id_fac in self.get_ids():
            for id_zone in zones.get_ids():
                if is_polyline_in_polygon(self.shapes[id_fac], zones.shapes[id_zone]):
                    self.ids_zone[id_fac] = id_zone
                    break

    def get_departure_probabilities(self):
        """
        Returns the dictionnary of dictionaries with departure (or arrival)
        probabilities where probabilities[id_zone][id_landusetype]
        is a probability distribution vector  giving for each facility the
        probability to depart/arrive in zone id_zone with facility type ftype.

        The ids_fac is an array that contains the facility ids in correspondence
        to the probability vector.
        """
        print 'get_departure_probabilities'
        probabilities = {}
        zones = self.ids_zone.get_linktab()
        inds_fac = self.get_inds()
        for id_zone in zones.get_ids():
            probabilities[id_zone] = {}
            for id_landusetype in set(self.ids_landusetype.value):
                # print '  id_zone,id_landusetype',id_zone,id_landusetype
                # print '  ids_landusetype',self.ids_landusetype.value[inds_fac]
                # print '  ids_zone',self.ids_zone.value[inds_fac]
                # print ''
                util = self.areas.value[inds_fac] * ((self.ids_landusetype.value[
                                                     inds_fac] == id_landusetype) & (self.ids_zone.value[inds_fac] == id_zone))
                util_tot = np.sum(util)
                # print '\n\n  [id_taz][ftype]',id_taz,ftype,util_tot,np.sum(util/np.sum(util))
                # print '  self.type==ftype',self.type==ftype
                # print '  self.id_taz==id_taz',self.id_taz==id_taz
                # print '  util',util
                if util_tot > 0.0:
                    probabilities[id_zone][id_landusetype] = util / util_tot
                else:
                    probabilities[id_zone][
                        id_landusetype] = util  # all zero prob

        return probabilities, self.get_ids(inds_fac)

    def update(self, ids):
        for _id in ids:
            self.centroids[_id] = np.mean(self.shapes[_id], 0)
            self.areas[_id] = find_area(
                np.array(self.shapes[_id], float)[:, :2])

        # self.identify_closest_edge(ids)

    def identify_closest_edge(self, ids=None, priority_max=5):
        """
        Identifies edge ID and position on this edge that 
        is closest to the centoid of each facility and the satisfies certain
        conditions.
        """
        print 'identify_closest_edge'
        edges = self.get_edges()

        # select edges...if (edges.priorities[id_edge]<=priority_max) &
        # edges.has_sidewalk(id_edge):
        edges.make_segment_edge_map()

        if ids == None:
            ids = self.get_ids()
        for id_fac in ids:
            id_edge = edges.get_closest_edge(self.centroids[id_fac])

            # determin position on edeg where edge is closest to centroid
            # TODO: solve this faster with precalculated maps!!
            xc, yc, zc = self.centroids[id_fac]
            shape = edges.shapes[id_edge]
            n_segs = len(shape)

            d_min = 10.0**8
            x_min = 0.0
            y_min = 0.0
            j_min = 0
            p_min = 0.0
            pos = 0.0
            x1, y1, z1 = shape[0]
            edgelength = edges.lengths[id_edge]
            for j in xrange(1, n_segs):
                x2, y2, z2 = shape[j]
                d, xp, yp = shortest_dist(x1, y1, x2, y2, xc, yc)
                # print '    x1,y1=(%d,%d)'%(x1,y1),',x2,y2=(%d,%d)'%(x2,y2),',xc,yc=(%d,%d)'%(xc,yc)
                # print '    d,x,y=(%d,%d,%d)'%shortest_dist(x1,y1, x2,y2,
                # xc,yc)
                if d < d_min:
                    d_min = d
                    # print '    **d_min=',d_min,[xp,yp]
                    x_min = xp
                    y_min = yp
                    j_min = j
                    p_min = pos
                # print '    pos',pos,[x2-x1,y2-y1],'p_min',p_min
                pos += np.linalg.norm([x2 - x1, y2 - y1])
                x1, y1 = x2, y2

            x1, y1, z1 = shape[j_min - 1]
            pos_min = p_min + np.linalg.norm([x_min - x1, y_min - y1])
            # print '  k=%d,d_min=%d, x1,y1=(%d,%d),xmin,ymin=(%d,%d),xc,yc=(%d,%d)'%(k,d_min,x1,y1,x_min,y_min,xc,yc)
            # print '  pos=%d,p_min=%d,pos_min=%d'%(pos,p_min,pos_min)

            if pos_min > edgelength:
                pos_min = edgelength

            # print '  id_fac,id_edge',id_fac,id_edge,pos_min
            self.ids_roadedge_closest[id_fac] = id_edge
            self.positions_roadedge_closest[id_fac] = pos_min

    def add_polys(self, ids_sumo=[], **kwargs):
        """
        Adds a facilities as used on sumo poly xml info
        """
        # stuff with landusetype must be done later
        return self.add_rows(n=len(ids_sumo),    ids_sumo=ids_sumo, **kwargs)

    def add_poly(self, id_sumo, id_landusetype=None, osmkey=None, shape=np.array([], np.float32)):
        """
        Adds a facility as used on sumo poly xml info
        """
        # print 'add_poly',id_sumo,id_landusetype,osmkey

        landusetypes = self.get_landusetypes()
        if id_landusetype != None:
            # this means that landusetype has been previousely identified
            if osmkey == None:
                # use filter as key
                osmkey = landusetypes.osmfilters[id_landusetype][0]

            return self.add_row(ids_sumo=id_sumo,
                                ids_landusetype=id_landusetype,
                                osmkeys=osmkey,
                                shapes=shape,
                                areas=find_area(shape[:, :2]),
                                centroids=np.mean(shape, 0)
                                )

        else:
            # identify ftype from fkeys...
            keyvec = osmkey.split('.')
            len_keyvec = len(keyvec)
            is_match = False
            for id_landusetype in landusetypes.get_ids():
                # print '  ',landusetypes.osmfilters[id_landusetype]
                # if fkeys==('building.industrial'): print '
                # check',facilitytype
                for osmfilter in landusetypes.osmfilters[id_landusetype]:
                    # print '     ',
                    osmfiltervec = osmfilter.split('.')
                    if osmkey == osmfilter:  # exact match of filter
                        is_match = True
                        # if fkeys==('building.industrial'):print '    found
                        # exact',osmkey
                    elif (len(osmfiltervec) == 2) & (len_keyvec == 2):

                        if osmfiltervec[0] == keyvec[0]:

                            if osmfiltervec[1] == '*':
                                is_match = True
                                # if fkeys==('building.industrial'):print '
                                # found match',osmkeyvec[0]

                            # redundent to exact match
                            # elif osmkeyvec[1]==keyvec[1]:
                            #    is_match = True
                            #   if is_match:
                            #       print '    found exact',osmkey

                    if is_match:
                        # if fkeys==('building.industrial'):print '  *found:',facilitytype,fkeys
                        # return self.facilities.set_row(ident, type =
                        # facilitytype, polygon=polygon, fkeys =
                        # fkeys,area=find_area(polygon),centroid=np.mean(polygon,0))
                        return self.add_row(ids_sumo=id_sumo,
                                            ids_landusetype=id_landusetype,
                                            osmkeys=osmkey,
                                            shapes=shape,
                                            areas=find_area(shape[:, :2]),
                                            centroids=np.mean(shape, 0)
                                            )

    def clear(self):
        self.reset()
        self.clear_rows()

    def set_shapes(self, ids, vertices):
        self.shapes[ids] = vertices
        if not hasattr(ids, '__iter__'):
            ids = [ids]
        self.update(ids)

    def import_poly(self, polyfilepath, is_remove_xmlfiles=False):
        print 'import_poly from %s ' % (polyfilepath,)
        self.clear()
        # let's read first the offset information, which are in the
        # comment area
        fd = open(polyfilepath, 'r')
        is_comment = False
        is_processing = False
        offset = self.get_net().get_offset()  # default is offset from net
        # print '  offset,offset_delta',offset,type(offset)
        #offset = np.array([0,0],float)
        for line in fd.readlines():
            if line.find('<!--') >= 0:
                is_comment = True
            if is_comment & (line.find('<processing') >= 0):
                is_processing = True
            if is_processing & (line.find('<offset.x') >= 0):
                offset[0] = float(xm.read_keyvalue(line, 'value'))
            elif is_processing & (line.find('<offset.y') >= 0):
                offset[1] = float(xm.read_keyvalue(line, 'value'))
                break
        fd.close()
        offset_delta = offset - self.get_net().get_offset()

        exectime_start = time.clock()

        counter = SumoPolyCounter()
        parse(polyfilepath, counter)
        fastreader = SumoPolyReader(self, counter, offset_delta)
        parse(polyfilepath, fastreader)
        fastreader.finish()

        # update ids_landuse...
        # self.update()

        # timeit
        print '  exec time=', time.clock() - exectime_start

        # print '  self.shapes',self.shapes.value


class SumoPolyCounter(handler.ContentHandler):

    """Counts facilities from poly.xml file into facility structure"""

    def __init__(self):
        self.n_fac = 0

    def startElement(self, name, attrs):
        # print 'startElement',name,len(attrs)
        if name == 'poly':
            self.n_fac += 1


class SumoPolyReader(handler.ContentHandler):

    """Reads facilities from poly.xml file into facility structure"""

    def __init__(self, facilities, counter, offset_delta):

        self._facilities = facilities
        self._ids_landusetype_all = self._facilities.get_landusetypes(
        ).get_ids()
        self._osmfilters = self._facilities.get_landusetypes().osmfilters

        self._ind_fac = -1
        self.ids_sumo = np.zeros(counter.n_fac, np.object)
        self.ids_landusetype = -1 * np.ones(counter.n_fac, np.int32)
        self.osmkeys = np.zeros(counter.n_fac, np.object)
        self.shape = np.zeros(counter.n_fac, np.object)
        self.areas = np.zeros(counter.n_fac, np.float32)
        self.centroids = np.zeros((counter.n_fac, 3), np.float32)

        #self._id_facility = None
        self._offset_delta = offset_delta

    def startElement(self, name, attrs):

        # print 'startElement', name, len(attrs)
        if name == 'poly':
            self._ind_fac += 1
            i = self._ind_fac

            osmkey = attrs['type']

            id_landuse = self.get_landuse(osmkey)
            if id_landuse >= 0:  # land use is interesting
                shape = xm.process_shape(
                    attrs.get('shape', ''), offset=self._offset_delta)
                shapearray = np.array(shape, np.float32)
                # print '  shapearray',shapearray
                self.ids_sumo[i] = attrs['id']
                self.ids_landusetype[i] = id_landuse
                self.osmkeys[i] = osmkey
                self.shape[i] = shape
                self.areas[i] = find_area(shapearray[:, :2])
                self.centroids[i] = np.mean(shapearray, 0)

            # color info in this file no longer used as it is defined in
            # facility types table
            # color = np.array(xm.parse_color(attrs['color']))*0.8,# make em
            # darker!!

    def get_landuse(self, osmkey):
        keyvec = osmkey.split('.')
        len_keyvec = len(keyvec)
        # print 'get_landuse',len_keyvec,keyvec
        #is_match = False
        for id_landusetype in self._ids_landusetype_all:
            # print '  ',landusetypes.osmfilters[id_landusetype]
            # if fkeys==('building.industrial'): print ' check',facilitytype
            for osmfilter in self._osmfilters[id_landusetype]:
                osmfiltervec = osmfilter.split('.')
                # print '
                # osmfiltervec',id_landusetype,osmfiltervec,osmkey==osmfilter,(len(osmfiltervec)==2)&(len_keyvec==2)

                if osmkey == osmfilter:  # exact match of filter
                    return id_landusetype

                elif (len(osmfiltervec) == 2) & (len_keyvec == 2):

                    if osmfiltervec[0] == keyvec[0]:

                        if osmfiltervec[1] == '*':
                            return id_landusetype
        return -1

    def finish(self):

        # print 'write_to_net'
        inds_valid = np.flatnonzero(self.ids_landusetype >= 0)
        ids_fac = self._facilities.add_polys(
            ids_sumo=self.ids_sumo[inds_valid],
            ids_landusetype=self.ids_landusetype[inds_valid],
            osmkeys=self.osmkeys[inds_valid],
            shapes=self.shape[inds_valid],
            areas=self.areas[inds_valid],
            centroids=self.centroids[inds_valid],
        )

    # def characters(self, content):
    #    if self._id!=None:
    #        self._currentShape = self._currentShape + content

    # def endElement(self, name):
    #    pass


class Parking(am.ArrayObjman):

    def __init__(self, landuse, lanes, **kwargs):
        # print 'Parking.__init__',hasattr(self,'lanes')
        self._init_objman(ident='parking', parent=landuse,
                          name='Parking',
                          info='Information on private car parking.',
                          #is_plugin = True,
                          #**kwargs
                          )

        #--------------------------------------------------------------------
        # misc params

        # these are options for assignment procedure!!
        # self.add(AttrConf(  'length_noparking', kwargs.get('length_noparking',20.0),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                unit = 'm',
        #                                name = 'Min Length',
        #                                info = 'Minimum edge length for assigning on-road parking space.' ,
        #                                #xmltag = 'pos',
        #                                ))
        #
        # self.add(AttrConf(  'length_space', kwargs.get('length_space',20.0),
        #                                groupnames = ['options'],
        #                                perm='wr',
        #                                unit = 'm',
        #                                name = 'Lot length',
        #                                info = 'Length of a standard parking lot.' ,
        #                                #xmltag = 'pos',
        #                                ))

        self.add_col(am.IdsArrayConf('ids_lane', lanes,
                                     name='ID Lane',
                                     info='ID of lane for this parking position. ',
                                     ))

        self.add_col(am.ArrayConf('positions', 0.0,
                                  dtype=np.float32,
                                  perm='r',
                                  name='Pos',
                                  unit='m',
                                  info="Position on lane for this parking.",
                                  #xmltag = 'departPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))
        self.add_col(am.ArrayConf('lengths', 0.0,
                                  dtype=np.float32,
                                  #choices = OPTIONMAP_POS_DEPARTURE,
                                  perm='r',
                                  name='Length',
                                  unit='m',
                                  info="Length of parking lot in edge direction.",
                                  #xmltag = 'departPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))

        self.add_col(am.ArrayConf('angles', 0.0,
                                  dtype=np.float32,
                                  perm='r',
                                  name='Angle',
                                  unit='deg',
                                  info="Parking angle with respect to lane direction.",
                                  #xmltag = 'departPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))

        self.add_col(am.ArrayConf('vertices',  np.zeros((2, 3), dtype=np.float32),
                                  dtype=np.float32,
                                  groupnames=['_private'],
                                  perm='r',
                                  name='Coords',
                                  unit='m',
                                  info="Start and end vertices of right side of parking space.",
                                  #xmltag = 'departPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))

        self.add_col(am.ArrayConf('durations', 0.0,  # ???
                                  dtype=np.float32,
                                  perm='r',
                                  name='Parking duration',
                                  unit='s',
                                  info="Default duration of car parking.",
                                  #xmltag = 'departPos',
                                  #xmlmap = get_inversemap(OPTIONMAP_POS_ARRIVAL),
                                  ))

        self.add(cm.ObjConf(lanes, is_child=False, groups=['_private']))

        self.add(cm.ObjConf(lanes.parent.edges,
                            is_child=False, groups=['_private']))

    def get_edges(self):
        self.edges.get_value()

    def get_lanes(self):
        self.lanes.get_value()

    def link_vehiclefleet(self, vehicles):
        """
        Links to table with vehicle info.
        """
        self.add_col(am.IdsArrayConf('ids_bookedveh', vehicles,
                                     name='ID booked veh',
                                     info='ID of vehicle which has booked this parking position.',
                                     ))

    def update_netoffset(self, deltaoffset):
        """
        Called when network offset has changed.
        Children may need to adjust theur coordinates.
        """
        pass

    def get_parkinglane_from_edge(self, id_edge, id_mode, length_min=15.0, priority_max=8, n_freelanes_min=1):
        """
        Check if edge can have on-road parking
        """
        edges = self.edges.get_value()
        lanes = self.lanes.get_value()
        # check size
        # laneindex =
        # print 'check
        # prio,l_min,nlanes,nlanesOK,mode',id_edge,edges.priorities[id_edge]<=priority_max,edges.lengths[id_edge]>length_min,edges.nums_lanes[id_edge],lanes.modes_allow[edges.ids_lanes[id_edge][0]]

        if (edges.priorities[id_edge] <= priority_max) & (edges.lengths[id_edge] > length_min):

            laneindex = edges.get_laneindex_allowed(id_edge, id_mode)
            # print '  found',laneindex,edges.nums_lanes[id_edge]-laneindex >
            # n_freelanes_min
            if (laneindex >= 0) & (edges.nums_lanes[id_edge] - laneindex >= n_freelanes_min):
                return edges.ids_lanes[id_edge][laneindex]

        return -1  # no parking possible by default

    def get_edge_pos_parking(self, id_parking):
        lanes = self.lanes.get_value()
        return lanes.ids_edge[self.ids_lane[id_parking]], self.positions[id_parking]

    # def get_edge_pos_parking(self, id_parking):
    #    """
    #    Retuens edge and position of parking with id_parking
    #    """
    #    ind = self.parking.get_ind(id_parking)
    #
    # return
    # self.edges.get_value()(self.id_edge_parking[ind]),self.pos_edge_parking[ind]

    def make_parking(self, id_mode=MODES['private'], length_min=20.0, length_noparking=12.0, length_lot=6.0, angle=0.0):
        print 'make_parking'
        edges = self.edges.get_value()
        lanes = self.lanes.get_value()
        n_parking = 0
        ids_parking = []
        for id_edge in edges.get_ids():
            # check if edge is suitable...
            # print '
            # id_edge,length,n_lanes,',id_edge,edge.getLength(),len(edge.getLanes()),edge.getLane(0).isAllowed('pedestrian')
            id_lane = self.get_parkinglane_from_edge(
                id_edge, id_mode, length_min)
            if id_lane >= 0:
                n_spaces = int(
                    (edges.lengths[id_edge] - length_noparking) / length_lot + 0.5)
                # print '  create',lanes.indexes[id_lane],edges.lengths[id_edge],n_spaces
                # print '
                # delta',lanes.shapes[id_lane][0]-lanes.shapes[id_lane][-1]
                pos_offset = 0.5 * length_noparking
                pos = pos_offset
                for i in xrange(n_spaces):
                    #id_park = self.suggest_id()
                    # print '    pos=',pos,pos/edges.lengths[id_edge]

                    # print '
                    # vertices',get_vec_on_polyline_from_pos(lanes.shapes[id_lane],pos,
                    # length_lot, angle = angle)
                    n_parking += 1

                    id_park = self.add_row(ids_lane=id_lane,
                                           positions=pos,
                                           lengths=length_lot,
                                           angles=angle,
                                           vertices=get_vec_on_polyline_from_pos(
                                               lanes.shapes[id_lane], pos, length_lot - 0.5, angle=angle)
                                           )
                    # print '    created id_park,pos',
                    # id_park,pos#,get_coord_on_polyline_from_pos(lanes.shapes[id_lane],pos),lanes.shapes[id_lane]
                    ids_parking.append(id_park)
                    pos = pos_offset + (i + 1) * length_lot

        print '  created %d parking spaces' % n_parking
        return ids_parking

    def assign_parking(self, ids_veh, coords):
        """
        Assigns a parking space to each vehicle as close as possible to coords.
        """

        #inds_person = self.persons.get_inds(ids_person)
        n = len(ids_veh)
        print 'assign_parking', n

        #parking = self.get_landuse().parking
        #inds_parking = parking.get_inds()
        coord_parking = self.vertices.value[:, 1, :]

        inds_vehparking = np.zeros(n, int)
        inds_parking_avail = np.flatnonzero(
            self.ids_bookedveh.value == -1).tolist()

        #ids_veh = np.zeros(n,object)
        i = 0
        for id_veh, coord in zip(ids_veh, coords):
            # print '\n  id_veh,coord',id_veh,coord
            # print '    landuse.id_bookedveh_parking',landuse.id_bookedveh_parking
            #

            # print '    inds_parking_avail',inds_parking_avail
            # print '
            # dists',np.sum((coord-coord_parking[inds_parking_avail])**2,1),np.argmin(np.sum((coord-coord_parking[inds_parking_avail])**2,1))
            ind_parking_closest = inds_parking_avail[
                np.argmin(np.sum((coord - coord_parking[inds_parking_avail])**2, 1))]
            # print '
            # ind_parking_closest,n_avail',ind_parking_closest,len(inds_parking_avail)
            inds_vehparking[i] = ind_parking_closest
            # print '  id_veh=%s,id_parking_closest=%s, dist =%.2fm'%(id_veh,self.get_ids([ind_parking_closest]),np.sqrt(np.sum((coord-coord_parking[ind_parking_closest])**2)))
            # ids_bookedveh[ind_parking_closest]=id_veh # occupy parking
            inds_parking_avail.remove(ind_parking_closest)
            i += 1

        # print '  inds_vehparking',  inds_vehparking
        # print '  ids_vehparking',  self.get_ids(inds_vehparking)
        # print '  ids_veh',ids_veh
        self.ids_bookedveh.value[inds_vehparking] = ids_veh
        # self.ids_bookedveh.[ids_parking] =ids_bookedveh
        return self.get_ids(inds_vehparking), inds_vehparking


class Landuse(cm.BaseObjman):

    def __init__(self, scenario=None, net=None,  **kwargs):
        self._init_objman(ident='landuse', parent=scenario,
                          name='Landuse', **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))

        if scenario != None:
            net = scenario.net
        # self.net = attrsman.add(   cm.ObjConf( net, is_child = False ) )#
        # link only

        self.zones = attrsman.add(cm.ObjConf(Zones(self, net.edges)))
        self.landusetypes = attrsman.add(cm.ObjConf(LanduseTypes(self)))
        self.facilities = attrsman.add(cm.ObjConf(
            Facilities(self, self.landusetypes, self.zones, net=net)))
        self.parking = attrsman.add(cm.ObjConf(Parking(self, net.lanes)))
        self.maps = attrsman.add(cm.ObjConf(maps.Maps(self)))

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

    def export_polyxml(self, filepath=None, encoding='UTF-8'):
        """
        Export landuse facilities to SUMO poly.xml file.
        """
        if filepath == None:
            if self.parent is not None:
                filepath = self.parent.get_rootfilepath() + '.poly.xml'
            else:
                filepath = os.path.join(os.getcwd(), 'landuse.poly.xml')

        print 'export_polyxml', filepath
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_poly_xml: could not open', filepath
            return False

        #xmltag, xmltag_item, attrname_id = self.xmltag
        xmltag_poly = 'additional'
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        fd.write(xm.begin(xmltag_poly))
        indent = 2

        fd.write(xm.start('location', indent + 2))
        # print '  groups:',self.parent.net.get_attrsman().get_groups()
        for attrconfig in self.parent.net.get_attrsman().get_group('location'):
            # print '    locationconfig',attrconfig.attrname
            attrconfig.write_xml(fd)
        fd.write(xm.stopit())

        self.facilities.write_xml(
            fd, indent=indent + 2, is_print_begin_end=False)

        fd.write(xm.end(xmltag_poly))
        fd.close()
        return filepath

    def import_polyxml(self, rootname, dirname=''):
        filepath = os.path.join(dirname, rootname + '.poly.xml')
        if os.path.isfile(filepath):
            self.facilities.import_poly(filepath)

        else:
            self.get_logger().w(
                'import_xml: files not found:' + filepath, key='message')

        #
        # here may be other relevant imports
        #


class OsmPolyImporter(CmlMixin, Process):

    def __init__(self,  landuse=None,
                 osmfilepaths=None,
                 typefilepath=None,
                 polyfilepath=None,
                 projparams=None,
                 offset_x=None,
                 offset_y=None,
                 is_keep_full_type=True,
                 is_import_all_attributes=True,
                 is_use_name_for_id=False,
                 polytypefilepath='',
                 logger=None, **kwargs):
        print 'OsmPolyImporter.__init__', landuse, landuse.parent.get_rootfilename()
        self._init_common('osmpolyimporter', name='OSM Poly import',
                          logger=logger,
                          info='Converts a OSM  file to a SUMO Poly file and read facilities into scenario.',
                          )
        if landuse == None:
            self._landuse = Landuse()
        else:
            self._landuse = landuse

        self.init_cml('polyconvert')  # pass main shell command

        if landuse.parent != None:
            scenario = landuse.parent
            rootname = scenario.get_rootfilename()
            rootdirpath = scenario.get_workdirpath()
            if hasattr(scenario, 'net'):
                if projparams == None:
                    projparams = scenario.net.get_projparams()
                if (offset_x == None) & (offset_y == None):
                    offset_x, offset_y = scenario.net.get_offset()
        else:
            rootname = landuse.get_ident()
            rootdirpath = os.getcwd()

        if polyfilepath == None:
            polyfilepath = os.path.join(rootdirpath, rootname + '.poly.xml')

        if osmfilepaths == None:
            osmfilepaths = os.path.join(rootdirpath, rootname + '.osm.xml')

        if typefilepath == None:
            typefilepath = os.path.join(os.path.dirname(os.path.abspath(
                __file__)), '..', '..', 'typemap', 'osmPolyconvert.typ.xml')
        attrsman = self.get_attrsman()

        self.workdirpath = rootdirpath

        self.rootname = rootname

        self.add_option('osmfilepaths', osmfilepaths,
                        groupnames=['options'],
                        cml='--osm-files',
                        perm='rw',
                        name='OSM files',
                        wildcards='OSM XML files (*.osm)|*.osm*',
                        metatype='filepaths',
                        info='Openstreetmap files to be imported.',
                        )

        self.add_option('polyfilepath', polyfilepath,
                        groupnames=[],  # ['_private'],#
                        cml='--output-file',
                        perm='r',
                        name='Net file',
                        wildcards='Net XML files (*.poly.xml)|*.poly.xml',
                        metatype='filepath',
                        info='SUMO Poly file in XML format.',
                        )

        self.add_option('typefilepath', typefilepath,
                        groupnames=['options'],
                        cml='--type-file',
                        perm='rw',
                        name='Type file',
                        wildcards='Typemap XML files (*.typ.xml)|*.typ.xml',
                        metatype='filepath',
                        info="""Typemap XML files. In these file, 
OSM building types are mapped to specific facility parameters, is not explicitely set by OSM attributes.""",
                        )

        # --net-file <FILE> 	Loads SUMO-network FILE as reference to offset and projection
        self.add_option('projparams', projparams,
                        groupnames=['options'],
                        cml='--proj',
                        perm='rw',
                        name='projection',
                        info='Uses STR as proj.4 definition for projection. Default is the projection of the network, better do not touch!',
                        is_enabled=lambda self: self.projparams != None,
                        )

        self.add_option('offset_x', offset_x,
                        groupnames=['options', 'geometry'],
                        cml='--offset.x ',
                        perm='rw',
                        unit='m',
                        name='X-Offset',
                        info='Adds offset to net x-positions; default: 0.0',
                        is_enabled=lambda self: self.offset_x != None,
                        )
        self.add_option('offset_y', offset_y,
                        groupnames=['options', 'geometry'],
                        cml='--offset.y ',
                        perm='rw',
                        unit='m',
                        name='Y-Offset',
                        info='Adds offset to net x-positions; default: 0.0',
                        is_enabled=lambda self: self.offset_y != None,
                        )

        self.add_option('is_keep_full_type', is_keep_full_type,
                        groupnames=['options'],
                        cml='--osm.keep-full-type',
                        perm='rw',
                        name='keep full OSM type',
                        info='The type will be made of the key-value - pair.',
                        )

        self.add_option('is_import_all_attributes', is_keep_full_type,
                        groupnames=['options'],
                        cml='--all-attributes',
                        perm='rw',
                        name='import all attributes',
                        info='Imports all OSM attributes.',
                        )

        self.add_option('is_use_name_for_id', is_use_name_for_id,
                        groupnames=['options'],
                        cml='--osm.use-name',
                        perm='rw',
                        name='use OSM name for id',
                        info=' 	The OSM id (not internal ID) will be set from the given OSM name attribute.',
                        )

        self.add_option('polytypefilepath', polytypefilepath,
                        groupnames=[],  # ['_private'],#
                        cml='--type-file',
                        perm='rw',
                        name='Poly type file',
                        wildcards='Net XML files (*.xml)|*.xml',
                        metatype='filepath',
                        info='SUMO Poly type file in XML format.',
                        is_enabled=lambda self: self.polytypefilepath != '',
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
        # if len(bn)>0:
        #    self.rootname = bn[0]

    def do(self):
        self.update_params()
        cml = self.get_cml()

        # print 'SumonetImporter.do',cml
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        self.run_cml(cml)
        if self.status == 'success':
            if os.path.isfile(self.polyfilepath):
                print '  OSM->poly.xml successful, start importing xml files'
                self._landuse.import_polyxml(self.rootname, self.workdirpath)
                print '  import poly in sumopy done.'

    def get_landuse(self):
        # used to het landuse in case landuse has been created
        return self._landuse


if __name__ == '__main__':
    ##########################################################################
    # print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #from coremodules.scenario import scenario
    from coremodules.network import network
    logger = Logger()
    NETPATH = os.path.join(SUMOPYDIR, 'coremodules', 'network', 'testnet')
    net = network.Network(logger=logger)
    rootname = 'facsp2'
    net.import_xml(rootname, NETPATH)
    # net.read_sumonodes(os.path.join(NETPATH,'facsp2.nod.xml'))
    # net.read_sumoedges(os.path.join(NETPATH,'facsp2.edg.xml'))
    landuse = Landuse(net=net, logger=logger)

    # landuse.facilities.import_poly(os.path.join(NETPATH,'facsp2.poly.xml'))
    landuse.import_xml(rootname, NETPATH)
    objbrowser(landuse)
