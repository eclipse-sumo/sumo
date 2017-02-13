# size limit at 1280x1280
# http://maps.googleapis.com/maps/api/staticmap?size=500x500&path=color:0x000000|weight:10|44.35789,11.3093|44.4378,11.3935&format=GIF&maptype=satellite&scale=2

import os
import sys
import subprocess
import platform

from xml.sax import saxutils, parse, handler
if __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR, '..', '..')
    sys.path.append(os.path.join(SUMOPYDIR))

import time
import numpy as np
from collections import OrderedDict
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm

from agilepy.lib_base.processes import Process, CmlMixin, P

from agilepy.lib_base.geometry import *
from agilepy.lib_base.misc import filepathlist_to_filepathstring, filepathstring_to_filepathlist


#TESTNODES = [294,295]
MODES = OrderedDict([
                    ("ignoring", 0),
                    ("pedestrian", 1),
                    ("bicycle", 2),
                    ("motorcycle", 3),
                    ("passenger", 4),
                    ("bus", 5),
                    ("tram", 6),
                    ("rail_urban", 7),
                    ("delivery", 8),
                    ("private", 9),
                    ("taxi", 10),
                    ("hov", 11),
                    ("evehicle", 12),
                    ("emergency", 13),
                    ("authority", 14),
                    ("army", 15),
                    ("vip", 16),
                    ("coach", 17),
                    ("truck", 18),
                    ("trailer", 19),
                    ("rail", 20),
                    ("rail_electric", 21),
                    ("moped", 22),
                    ("custom1", 23),
                    ("custom2", 24),
                    ("ship", 25),
                    ])

OSMEDGETYPE_TO_MODES = {'highway.cycleway': ([MODES['bicycle']], 5.6),
                        'highway.pedestrian': ([MODES['pedestrian']], 0.8),
                        'highway.footway': ([MODES['pedestrian']], 0.8),
                        'highway.path': ([MODES['pedestrian'], MODES['bicycle']], 5.6),
                        'highway.service': ([MODES['delivery'], MODES['bicycle']], 13.8),
                        }


class SumoIdsConf(am.ArrayConf):

    """
    Sumo id array coniguration
    """
    # def __init__(self, **attrs):
    #    print 'ColConf',attrs

    def __init__(self, refname, name=None, info=None, perm='rw',  xmltag='id'):
        if name == None:
            name = 'ID ' + refname
        if info == None:
            info = refname + ' ID of SUMO network'
        am.ArrayConf.__init__(self, attrname='ids_sumo', default='',
                              dtype='object',
                              perm=perm,
                              is_index=True,
                              name=name,
                              info=info,
                              xmltag=xmltag,
                              )


class Modes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions

    def __init__(self, parent, **kwargs):
        ident = 'modes'
        self._init_objman(ident=ident, parent=parent, name='Transport Modes',
                          xmltag=('vClasses', 'vClass', 'names'),
                          **kwargs)
        self.add_col(am.ArrayConf('names', '',
                                  dtype=np.object,
                                  perm='r',
                                  is_index=True,
                                  name='Name',
                                  info='Name of mode. Used as key for implementing acces restrictions on edges as well as demand modelling.',
                                  xmltag='vClass',
                                  ))

    def get_id_mode(self, modename):
        return self.names.get_id_from_index(modename)


class TrafficLightProgram(am.ArrayObjman):

    def __init__(self, ident, parent, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='TLL Program',
                          info='Signale phases of a traffic light program.',
                          xmltag=('', 'phase', ''), **kwargs)

        self.add_col(am.NumArrayConf('durations', 0,
                                     dtype=np.int32,
                                     name='Duration',
                                     unit='s',
                                     info='The duration of the phase.',
                                     xmltag='duration',
                                     ))

        self.add_col(am.NumArrayConf('durations_min', 0,
                                     dtype=np.int32,
                                     name='Min. duration',
                                     unit='s',
                                     info='The minimum duration of the phase when using type actuated. Optional, defaults to duration.',
                                     xmltag='minDur',
                                     ))

        self.add_col(am.NumArrayConf('durations_max', 0,
                                     dtype=np.int32,
                                     name='Max. duration',
                                     unit='s',
                                     info='The maximum duration of the phase when using type actuated. Optional, defaults to duration.',
                                     xmltag='maxDur',
                                     ))

        self.add_col(am.ArrayConf('states', None,
                                  dtype=np.object,
                                  perm='rw',
                                  name='State',
                                  info="The traffic light states for this phase. Values can be one of these characters: 'r'=red, 'y'=yellow, 'g'=green give priority, 'G'=Green always priority, 'o'=blinking ,'O'=TLS switched off",
                                  xmltag='state',
                                  ))

    def add_multi(self, **kwargs):
        # print 'add_multi',self.ident
        # print '  durations',kwargs.get('durations',None)
        # print '  durations_min',kwargs.get('durations_min',None)
        # print '  durations_max',kwargs.get('durations_max',None)
        # print '  states',kwargs.get('states',None)
        return self.add_rows(durations=kwargs.get('durations', None),
                             durations_min=kwargs.get('durations_min', None),
                             durations_max=kwargs.get('durations_max', None),
                             states=kwargs.get('states', None),
                             )

    # def  write_xml(self, fd, indent, **kwargs):
    #
    #    # never print begin-end tags
    #    # this could go into xml config
    #    if kwargs.has_key('is_print_begin_end'):
    #        del kwargs['is_print_begin_end']
    #    am.ArrayObjman.write_xml(self, fd, indent,is_print_begin_end = False,**kwargs)


class TrafficLightLogics(am.ArrayObjman):

    def __init__(self, ident, tlss, **kwargs):
        self._init_objman(ident, parent=tlss,
                          name='Traffic Light Logics',
                          info='Traffic light Logics (TLLs) for Trafic Light Systems (TLSs).',
                          xmltag=('tlLogics', 'tlLogic', 'ids_tls'),
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_tls', tlss,
                                     groupnames=['state'],
                                     name='ID tls',
                                     info='ID of traffic light system.  Typically the id for a traffic light is identical with the junction id. The name may be obtained by right-clicking the red/green bars in front of a controlled intersection.',
                                     # this will be ID TLS tag used as ID in
                                     # xml file
                                     xmltag='id',
                                     ))

        self.add_col(am.ArrayConf('ids_prog', '',
                                  dtype=np.object,
                                  perm='rw',
                                  name='Prog ID',
                                  info='Sumo program ID, which is unique within the same traffic light system.',
                                  xmltag='programID',
                                  ))

        self.add_col(am.ArrayConf('ptypes', 1,
                                  choices={
                                      "static": 1,
                                      "actuated": 2,
                                  },
                                  dtype=np.int32,
                                  perm='rw',
                                  name='Prog. type',
                                  info='The type of the traffic light program (fixed phase durations, phase prolongation based time gaps between vehicles).',
                                  xmltag='type',
                                  ))

        self.add_col(am.NumArrayConf('offsets', 0,
                                     dtype=np.int32,
                                     perm='rw',
                                     name='Offset',
                                     unit='s',
                                     info='The initial time offset of the program.',
                                     #is_plugin = True,
                                     xmltag='offset',
                                     ))

        self.add_col(cm.ObjsConf('programs',
                                 name='program',
                                 info='Tls program.',
                                 ))

    def make(self, id_tls, id_prog=None, ptype=None, offset=None, **kwargs_prog):
        # print 'make',id_tls,id_prog
        if id_prog is None:
            id_prog = str(len(np.flatnonzero(self.ids_tls == id_tls)))

        id_tll = self.add_row(ids_tls=id_tls,
                              ids_prog=id_prog,
                              ptypes=ptype,
                              offsets=offset,
                              )
        # init programme
        program = TrafficLightProgram(('prog', id_tll), self)
        self.programs[id_tll] = program

        # add phases
        program.add_multi(**kwargs_prog)

        return id_tll


class TrafficLightSystems(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Traffic_Light_Program_Definition

    def __init__(self, net, **kwargs):

        self._init_objman(ident='tlss', parent=net,
                          name='Traffic Light Systems',
                          # actually tlls table is exported, but ids_sumo is
                          # required for ID
                          xmltag=('tlSystems', 'tlSystem', 'ids_sumo'),
                          **kwargs)

        self.add_col(SumoIdsConf('TLS', info='SUMO ID of traffic light system.',
                                 xmltag='id'))

        self.add(cm.ObjConf(TrafficLightLogics('tlls', self)))

        self.add_col(am.IdlistsArrayConf('ids_tlls', self.tlls.value,
                                         groupnames=['state'],
                                         name='IDs TLL',
                                         info='ID list of available Traffic Light Logics (or programs) for the Traffic Light System.',
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_cons', self.parent.connections,
                                         groupnames=['state'],
                                         name='IDs con.',
                                         info='ID list of controlled connections. These connections corrispond to the elements of the state vector within the program-phases.',
                                         ))

    def make(self, id_sumo, **kwargs):
        if self.ids_sumo.has_index(id_sumo):
            # recycle ID from existing
            id_tls = self.ids_sumo.get_id_from_index(id_sumo)
        else:
            # make new TLS
            id_tls = self.add_row(ids_sumo=id_sumo, ids_tlls=[])

        # make a new TL logic for this traffic light systems
        id_tll = self.tlls.get_value().make(id_tls, **kwargs)
        # append new logic to list
        self.ids_tlls[id_tls].append(id_tll)
        return id_tls

    def set_connections(self, id_tls, ids_con):
        """
        Set connections, which represent the controlled links of TLD with id_tls
        Called after connections in ttl file have been parsed.
        """
        #id_tls = self.ids_sumo.get_id_from_index(id_sumo)
        self.ids_cons[id_tls] = ids_con

    def export_sumoxml(self, filepath=None, encoding='UTF-8'):
        """
        Export traffic light systems to SUMO xml file.
        """
        # here we export actually the traffic light logics table
        # and the controlled connections table
        tlls = self.tlls.get_value()
        connections = self.parent.connections  # self.ids_cons.get_linktab()
        lanes = self.parent.lanes
        edges = self.parent.edges

        # this is the preferred way to specify default filepath
        if filepath == None:
            filepath = self.parent.get_rootfilepath() + '.tll.xml'

        print 'export_sumoxml', filepath
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_sumoxml: could not open', filepath
            return False

        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        xmltag_ttl, xmltag_id, attrconf_id = tlls.xmltag
        fd.write(xm.begin(xmltag_ttl))
        indent = 2

        #ids_modes_used = set(self.parent.vtypes.ids_mode[self.ids_vtype.get_value()])
        ids_tlls = tlls.get_ids()
        tlls.write_xml(fd, indent=indent,
                       #xmltag_id = 'id',
                       #ids = ids_tlls,
                       #ids_xml = self.ids_sumo[tlls.ids_tls[ids_tlls]],
                       is_print_begin_end=False,
                       )
        # write controlled connections
        ids_tls = self.get_ids()
        xmltag_con = 'connection'
        for ids_con, id_sumo_tls in zip(self.ids_cons[ids_tls], self.ids_sumo[ids_tls]):
            ids_fromlane = connections.ids_fromlane[ids_con]
            ids_tolane = connections.ids_tolane[ids_con]
            inds_fromlane = lanes.indexes[ids_fromlane]
            inds_tolane = lanes.indexes[ids_tolane]
            ids_sumo_fromedge = edges.ids_sumo[lanes.ids_edge[ids_fromlane]]
            ids_sumo_toedge = edges.ids_sumo[lanes.ids_edge[ids_tolane]]

            ind_link = 0
            for id_sumo_fromedge, id_sumo_toedge, ind_fromlane, ind_tolane in \
                    zip(ids_sumo_fromedge, ids_sumo_toedge, inds_fromlane, inds_tolane):

                fd.write(xm.start(xmltag_con, indent))
                fd.write(xm.num('from', id_sumo_fromedge))
                fd.write(xm.num('to', id_sumo_toedge))
                fd.write(xm.num('fromLane', ind_fromlane))
                fd.write(xm.num('toLane', ind_tolane))
                fd.write(xm.num('tl', id_sumo_tls))
                fd.write(xm.num('linkIndex', ind_link))

                fd.write(xm.stopit())

                ind_link += 1

        fd.write(xm.end(xmltag_ttl))


class Crossings(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions

    def __init__(self, parent, **kwargs):
        ident = 'crossings'
        self._init_objman(ident=ident, parent=parent, name='Crossings',
                          xmltag=('crossings', 'crossing', ''),
                          **kwargs)

        self.add_col(am.IdsArrayConf('ids_node', parent.nodes,
                                     groupnames=['state'],
                                     name='ID node',
                                     info='ID of node where crossings are located.',
                                     xmltag='node',
                                     ))

        self.add_col(am.IdlistsArrayConf('ids_edges', parent.edges,
                                         groupnames=['state'],
                                         name='IDs Edge',
                                         info='Edge IDs at specific node, where street crossing is possible.',
                                         xmltag='edges',
                                         ))

        self.add_col(am.ArrayConf('widths', 4.0,
                                  dtype=np.float32,
                                  perm='rw',
                                  unit='m',
                                  name='Width',
                                  info='Crossing width.',
                                  xmltag='width',
                                  ))

        self.add_col(am.ArrayConf('are_priority', False,
                                  dtype=np.bool,
                                  perm='rw',
                                  name='Priority',
                                  info='Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections).',
                                  xmltag='priority',
                                  ))

        self.add_col(am.ArrayConf('are_discard', False,
                                  dtype=np.bool,
                                  perm='rw',
                                  name='Discard',
                                  info='Whether the crossing with the given edges shall be discarded.',
                                  xmltag='discard',
                                  ))

    def multimake(self, ids_node=[], **kwargs):
        n = len(ids_node)
        return self.add_rows(n=n,
                             ids_node=ids_node,
                             **kwargs
                             )

    def make(self, **kwargs):
        return self.add_row(ids_node=kwargs['id_node'],
                            ids_edges=kwargs['ids_edge'],
                            widths=kwargs.get('width', None),
                            are_priority=kwargs.get('is_priority', None),
                            are_discard=kwargs.get('is_discard', None),
                            )


class Connections(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions

    def __init__(self, parent, **kwargs):
        ident = 'connections'
        self._init_objman(ident=ident, parent=parent, name='Connections',
                          xmltag=('connections', 'connection', None),
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):

        lanes = self.parent.lanes
        self.add_col(am.IdsArrayConf('ids_fromlane', lanes,
                                     groupnames=['state'],
                                     name='ID from-lane',
                                     info='ID of lane at the beginning of the connection.',
                                     xmltag='fromLane',
                                     ))

        self.add_col(am.IdsArrayConf('ids_tolane', lanes,
                                     name='ID to-lane',
                                     info='ID of lane at the end of the connection.',
                                     xmltag='toLane',
                                     ))

        self.add_col(am.ArrayConf('are_passes', False,
                                  dtype=np.bool,
                                  perm='rw',
                                  name='Pass',
                                  info=' if set, vehicles which pass this (lane-to-lane) connection) will not wait.',
                                  xmltag='pass',
                                  ))

        self.add_col(am.ArrayConf('are_keep_clear',  True,
                                  dtype=np.bool,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='keep clear',
                                  info='if set to false, vehicles which pass this (lane-to-lane) connection will not worry about blocking the intersection.',
                                  xmltag='keepClear',
                                  ))

        self.add_col(am.ArrayConf('positions_cont', 0.0,
                                  dtype=np.float32,
                                  perm='rw',
                                  unit='m',
                                  name='Cont. Pos.',
                                  info='if set to 0, no internal junction will be built for this connection. If set to a positive value, an internal junction will be built at this position (in m) from the start of the internal lane for this connection. ',
                                  xmltag='contPos',
                                  ))

        self.add_col(am.ArrayConf('are_uncontrolled',  False,
                                  dtype=np.bool,
                                  perm='rw',
                                  name='uncontrolled',
                                  info='if set to true, This connection will not be TLS-controlled despite its node being controlled.',
                                  xmltag='uncontrolled',
                                  ))

    def make(self, **kwargs):
        return self.add_row(ids_fromlane=kwargs['id_fromlane'],
                            ids_tolane=kwargs['id_tolane'],
                            are_passes=kwargs.get('is_passes', None),
                            are_keep_clear=kwargs.get('is_keep_clear', None),
                            positions_cont=kwargs.get('position_cont', None),
                            are_uncontrolled=kwargs.get(
                                'is_uncontrolled', None),
                            )

    def multimake(self, ids_fromlane=[], ids_tolane=[], **kwargs):

        n = len(ids_fromlane)
        return self.add_rows(n=n,
                             ids_fromlane=ids_fromlane,
                             ids_tolane=ids_tolane,
                             **kwargs
                             )

    def get_id_from_sumoinfo(self, id_sumo_fromedge, id_sumo_toedge, ind_fromlane, ind_tolane):
        get_id_lane = self.parent.edges.get_id_lane_from_sumoinfo
        id_fromlane = get_id_lane(id_sumo_fromedge, ind_fromlane)
        id_tolane = get_id_lane(id_sumo_toedge, ind_tolane)
        ids_con = self.select_ids((self.ids_fromlane.value == id_fromlane) & (
            self.ids_tolane.value == id_tolane))
        if len(ids_con) == 1:
            return ids_con[0]
        else:
            return -1

    def export_sumoxml(self, filepath, encoding='UTF-8'):
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_sumoxml: could not open', filepath
            return False

        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        indent = 0
        self.write_xml(fd, indent)

        fd.close()

    def write_xml(self, fd, indent):
        # print 'Connections.write_xml'

        xmltag, xmltag_item, attrname_id = self.xmltag
        #attrsman = self.get_attrsman()
        #attrsman = self.get_attrsman()
        #config_fromlane = attrsman.get_config('ids_fromlane')
        #config_tolane = attrsman.get_config('ids_tolane')
        colconfigs = self.get_colconfigs(is_all=True)
        ids_sumoedges = self.parent.edges.ids_sumo
        ids_laneedge = self.parent.lanes.ids_edge
        # print '  header'
        fd.write(xm.start(xmltag, indent))
        # print '  ', self.parent.get_attrsman().get_config('version').attrname,self.parent.get_attrsman().get_config('version').get_value()
        #fd.write( self.parent.get_attrsman().get_config('version').write_xml(fd) )
        self.parent.get_attrsman().get_config('version').write_xml(fd)
        fd.write(xm.stop())

        for _id in self.get_ids():
            fd.write(xm.start(xmltag_item, indent + 2))

            # print ' make tag and id',_id
            # fd.write(xm.num(xmltag_id,attrconfig_id[_id]))

            # print ' write columns'
            for attrconfig in colconfigs:
                # print '    colconfig',attrconfig.attrname
                if attrconfig == self.ids_fromlane:
                    fd.write(xm.num('from', ids_sumoedges[
                             ids_laneedge[self.ids_fromlane[_id]]]))
                    attrconfig.write_xml(fd, _id)

                elif attrconfig == self.ids_tolane:
                    fd.write(xm.num('to', ids_sumoedges[
                             ids_laneedge[self.ids_tolane[_id]]]))
                    attrconfig.write_xml(fd, _id)

                else:
                    attrconfig.write_xml(fd, _id)
            fd.write(xm.stopit())

        self.parent.crossings.write_xml(
            fd, indent=indent + 2, is_print_begin_end=False)

        fd.write(xm.end(xmltag, indent))


class Lanes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions

    def __init__(self, parent, edges, modes, **kwargs):
        ident = 'lanes'
        self._init_objman(ident=ident, parent=parent, name='Lanes',
                          is_plugin=True,
                          xmltag=('lanes', 'lane', 'indexes'),
                          **kwargs)

        self.add_col(am.ArrayConf('indexes', 0,
                                  dtype=np.int32,
                                  perm='r',
                                  name='Lane index',
                                  info='The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one).',
                                  xmltag='index',
                                  ))

        self.add_col(am.ArrayConf('widths', 3.5,
                                  dtype=np.float32,
                                  perm='rw',
                                  unit='m',
                                  name='Width',
                                  info='Lane width.',
                                  is_plugin=True,
                                  xmltag='width',
                                  ))

        self.add_col(am.NumArrayConf('speeds_max', 50.0 / 3.6,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Max speed',
                                     unit='m/s',
                                     info='Maximum speed on lane.',
                                     xmltag='speed',
                                     ))

        self.add_col(am.NumArrayConf('offsets_end', 0.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='r',
                                     name='End offset',
                                     unit='m',
                                     info='Move the stop line back from the intersection by the given amount (effectively shortening the lane and locally enlarging the intersection).',
                                     xmltag='endOffset',
                                     ))

        self.add_col(am.IdlistsArrayConf('modes_allow', modes,
                                         name='IDs allowed',
                                         info='Allowed modes on this lane.',
                                         xmltag='allow',
                                         ))

        self.add_col(am.IdlistsArrayConf('modes_disallow', modes,
                                         name='IDs disallow',
                                         info='Disallowed modes on this lane.',
                                         xmltag='disallow',
                                         ))

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['state'],
                                     name='Main mode ID',
                                     info='ID of main mode of this lane.',
                                     is_plugin=True,
                                     ))

        self.add_col(am.IdsArrayConf('ids_edge', edges,
                                     groupnames=['state'],
                                     name='ID edge',
                                     info='ID of edge in which the lane is contained.',
                                     ))

        self.add_col(am.ListArrayConf('shapes',
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Shape',
                                      unit='m',
                                      info='List of 3D Shape coordinates to describe polyline.',
                                      is_plugin=True,
                                      ))

    def get_edges(self):
        return self.parent.edges

    def multimake(self, indexes=[], **kwargs):

        n = len(indexes)
        # print 'Lanes.make',kwargs
        #width = kwargs.get('widths',None)
        #speed_max = kwargs.get('speed_max',-1)
        #modes_allow = kwargs.get('modes_allow',[])

        return self.add_rows(n=n,
                             indexes=indexes,
                             widths=kwargs['widths'],
                             speeds_max=kwargs['speeds_max'],
                             offsets_end=kwargs['offsets_end'],
                             modes_allow=kwargs['modes_allow'],
                             modes_disallow=kwargs['modes_disallow'],
                             ids_mode=kwargs['ids_mode'],
                             ids_edge=kwargs['ids_edge'],
                             # shapes = kwargs.get('shapes',[]), # if empty,
                             # then computation later from edge shape
                             )

    def make(self, **kwargs):
        edges = self.get_edges()
        id_edge = kwargs['id_edge']
        index = kwargs['index']
        # print 'Lanes.make',kwargs
        width = kwargs.get('width', -1)
        speed_max = kwargs.get('speed_max', -1)
        modes_allow = kwargs.get('modes_allow', [])

        is_sidewalk_edge = False
        is_sidewalk = False
        if len(modes_allow) > 0:
            id_mode = modes_allow[0]  # pick first as major mode
        else:
            id_mode = -1  # no mode specified

        if index == 0:
            width_sidewalk_edge = edges.widths_sidewalk[id_edge]
            is_sidewalk_edge = width_sidewalk_edge > 0
            # test for pedestrian sidewalk
            is_sidewalk = (MODES['pedestrian'] in modes_allow)

        if speed_max < 0:
            if (index == 0) & is_sidewalk:
                speed_max = 0.8  # default walk speed
            else:
                speed_max = edges.speeds_max[id_edge]

        # print ' is_sidewalk_edge ,is_sidewalk',is_sidewalk_edge ,is_sidewalk
        if width < 0:
            width = edges.widths_lanes_default[id_edge]

            if index == 0:
                if is_sidewalk_edge:  # edge wants sidewalks
                    width = width_sidewalk_edge
                # edge does not want sidewalks, but actually there is a
                # sidewalk
                elif (not is_sidewalk_edge) & is_sidewalk:
                    width = 0.9  # default sidewalk width
                    edges.widths_sidewalk[id_edge] = width

        # if sidewalk, then the edge attribute widths_sidewalk
        # should be set to actual lane width in case it is less than zero
        elif index == 0:  # width set for lane 0
            # edge does not want sidewalks, but actually there is a sidewalk
            if (not is_sidewalk_edge) & is_sidewalk:
                edges.widths_sidewalk[id_edge] = width

        # if index == 0:
        #      edges.widths_sidewalk[id_edge]= width

        return self.add_row(indexes=index,
                            widths=width,
                            speeds_max=speed_max,
                            offsets_end=kwargs.get('offset_end', None),
                            modes_allow=modes_allow,
                            modes_disallow=kwargs.get('modes_disallow', []),
                            ids_mode=id_mode,
                            ids_edge=id_edge,
                            # if empty, then computation later from edge shape
                            shapes=kwargs.get('shapes', []),
                            )

    def reshape(self):
        for id_edge in self.parent.edges.get_ids():
            self.reshape_edgelanes(id_edge)

    def reshape_edgelanes(self, id_edge):
        """
        Recalculate shape of all lanes contained in edge id_edge
        based on the shape information of this edge.
        """
        #
        #lanes = self.get_lanes()
        edges = self.parent.edges
        ids_lane = edges.ids_lanes[id_edge]

        shape = np.array(edges.shapes[id_edge], np.float32)

        # print 'reshape: edgeshape id_edge,ids_lane=',id_edge,ids_lane
        # print '  shape =',shape
        n_lanes = len(ids_lane)
        n_vert = len(shape)

        angles_perb = get_angles_perpendicular(shape)

        dxn = np.cos(angles_perb)
        dyn = np.sin(angles_perb)

        #laneshapes = np.zeros((n_lanes,n_vert,3), np.float32)

        id_lane = ids_lane[0]
        # np.ones(n_lanes,np.float32)#lanes.widths[ids_lane]
        widths = self.widths[ids_lane]
        widths_tot = np.sum(widths)
        if edges.types_spread[id_edge] == 1:  # center lane spread
            widths2 = np.concatenate(([0.0], widths[:-1]))
            # print '  widths',widths_tot,widths
            # print '  widths2',widths2
            displacement = np.cumsum(widths2)
            displacement = 0.5 * (widths_tot) - displacement - 0.5 * widths
            # print '  displacement',displacement
        else:
            widths2 = np.concatenate(([0.0], widths[:-1]))
            displacement = np.cumsum(widths2)
            displacement = displacement[-1] - \
                displacement - 0.5 * widths + widths[-1]

        for i in range(n_lanes):
            id_lane = ids_lane[i]
            # print ' displacement[i] ',displacement[i]#,

            # if 1:#len(self.shapes[id_lane])==0: # make only if not existant
            laneshape = np.zeros(shape.shape, np.float32)
            # print ' dx \n',dxn*displacement[i]
            # print ' dy \n',dyn*displacement[i]
            laneshape[:, 0] = dxn * displacement[i] + shape[:, 0]
            laneshape[:, 1] = dyn * displacement[i] + shape[:, 1]
            laneshape[:, 2] = shape[:, 2]
            self.shapes[id_lane] = laneshape

        self.shapes.set_modified(True)


class Roundabouts(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions

    def __init__(self, parent, edges, nodes, **kwargs):
        ident = 'roundabouts'
        self._init_objman(ident=ident, parent=parent,
                          name='Roundabouts',
                          xmltag=('roundabouts', 'roundabout', ''),
                          **kwargs)

        self.add_col(am.IdlistsArrayConf('ids_edges', edges,
                                         groupnames=['state'],
                                         name='IDs edges',
                                         info='List with edges IDs.',
                                         xmltag='edges',
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_nodes', nodes,
                                         groupnames=['state'],
                                         name='IDs Nodes',
                                         info='List with node IDs.',
                                         xmltag='nodes',
                                         ))

    def multimake(self, ids_nodes=[], **kwargs):
        n = len(ids_nodes)
        return self.add_rows(n=n,
                             ids_nodes=ids_nodes, **kwargs
                             )

    def make(self, **kwargs):
        return self.add_row(ids_nodes=kwargs['ids_node'],
                            ids_edges=kwargs['ids_edge'],
                            )


class Edges(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Edge_Descriptions

    def __init__(self, parent, **kwargs):
        ident = 'edges'
        self._init_objman(ident=ident, parent=parent,
                          name='Edges',
                          xmltag=('edges', 'edge', 'ids_sumo'),
                          version=0.1,
                          **kwargs)

        self._init_attributes()

    def _init_attributes(self):
        if self.get_version() < 0.1:
            pass

        self.add_col(SumoIdsConf('Edge'))

        self.add_col(am.ArrayConf('types', '',
                                  dtype=np.object,
                                  perm='rw',
                                  name='Type',
                                  info='Edge reference OSM type.',
                                  xmltag='type',  # should not be exported?
                                  ))

        self.add_col(am.ArrayConf('nums_lanes', 1,
                                  dtype=np.int32,
                                  perm='r',
                                  name='# of lanes',
                                  info='Number of lanes.',
                                  xmltag='numLanes',
                                  ))

        self.add_col(am.NumArrayConf('speeds_max', 50.0 / 3.6,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Max speed',
                                     unit='m/s',
                                     info='Maximum speed on edge.',
                                     xmltag='speed',
                                     ))

        self.add_col(am.ArrayConf('priorities', 1,
                                  dtype=np.int32,
                                  perm='rw',
                                  name='Priority',
                                  info='Road priority (1-9).',
                                  xmltag='priority',
                                  ))

        self.add_col(am.NumArrayConf('lengths', 0.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='r',
                                     name='Length',
                                     unit='m',
                                     info='Edge length.',
                                     #xmltag = 'length ',
                                     ))

        self.add_col(am.NumArrayConf('widths', 0.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='r',
                                     name='Width',
                                     unit='m',
                                     info='Edge width.',
                                     is_plugin=True,
                                     #xmltag = 'width',
                                     ))

        self.add_col(am.ListArrayConf('shapes',
                                      groupnames=['_private'],
                                      perm='rw',
                                      name='Shape',
                                      unit='m',
                                      info='List of 3D Shape coordinates to describe polyline.',
                                      is_plugin=True,
                                      xmltag='shape',
                                      ))

        self.add_col(am.ArrayConf('types_spread', 0,
                                  choices={
                                      "right": 0,
                                      "center": 1,
                                  },
                                  dtype=np.int32,
                                  perm='rw',
                                  name='Spread type',
                                  info='Determines how the lanes are spread with respect to main link coordinates.',
                                  xmltag='spreadType',
                                  ))

        self.add_col(am.ArrayConf('names', '',
                                  dtype=np.object,
                                  perm='rw',
                                  name='Name',
                                  info='Road name, for visualization only.',
                                  xmltag='name',
                                  ))

        self.add_col(am.NumArrayConf('offsets_end', 0.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='r',
                                     name='End offset',
                                     unit='m',
                                     info='Move the stop line back from the intersection by the given amount (effectively shortening the edge and locally enlarging the intersection).',
                                     xmltag='endOffset',
                                     ))

        self.add_col(am.NumArrayConf('widths_lanes_default', 3.5,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Default lane width',
                                     unit='m',
                                     info='Default lane width for all lanes of this edge in meters (used for visualization).',
                                     #xmltag = '',
                                     ))

        self.add_col(am.NumArrayConf('widths_sidewalk', -1.0,
                                     dtype=np.float32,
                                     groupnames=['state'],
                                     perm='rw',
                                     name='Sidewalk width',
                                     unit='m',
                                     info='Adds a sidewalk with the given width (defaults to -1 which adds nothing).',
                                     #xmltag = 'sidewalkWidth',
                                     ))

    def set_nodes(self, nodes):
        # set ref to nodes table, once initialized
        self.add_col(am.IdsArrayConf('ids_fromnode', nodes,
                                     groupnames=['state'],
                                     name='ID from-node',
                                     info='ID of node at the beginning of the edge.',
                                     xmltag='from',
                                     ))

        self.add_col(am.IdsArrayConf('ids_tonode', nodes,
                                     groupnames=['state'],
                                     name='ID to-node',
                                     info='ID of node at the end of the edge.',
                                     xmltag='to',
                                     ))

    def set_lanes(self, lanes):
        self.add_col(am.IdlistsArrayConf('ids_lanes', lanes,
                                         groupnames=['state'],
                                         name='IDs Lanes',
                                         info='List with IDs of lanes.',
                                         xmltag='lanes',
                                         is_xml_include_tab=True,
                                         ))

    def get_outgoing(self, id_edge):
        # print
        # 'get_outgoing',id_edge,self.ids_tonode[id_edge],self.parent.nodes.ids_outgoing[self.ids_tonode[id_edge]]
        ids_edges = self.parent.nodes.ids_outgoing[self.ids_tonode[id_edge]]
        if ids_edges == None:  # dead end
            return []
        else:
            return ids_edges

    def get_incoming(self, id_edge):
        # TODO: would be good to have [] as default instead of None!!
        ids_edges = self.parent.nodes.ids_incoming[self.ids_fromnode[id_edge]]
        if ids_edges == None:  # dead end
            return []
        else:
            return ids_edges

    def get_lanes(self):
        return self.parent.lanes

    def get_id_lane_from_sumoinfo(self, id_sumo_edge,  ind_lane):
        id_edge = self.ids_sumo.get_id_from_index(id_sumo_edge)
        return self.ids_lanes[id_edge][ind_lane]

    def has_sidewalk(self, id_edge):
        return MODES["pedestrian"] in self.parent.lanes.modes_allow[self.ids_lanes[id_edge][0]]

    def get_laneindex_allowed(self, id_edge, id_mode):
        """
        Returns first lane index of edge id_edge on which id_mode
        is allowed.
        -1 means not allowed on edge
        """
        ind = 0
        modes_allow = self.parent.lanes.modes_allow
        modes_disallow = self.parent.lanes.modes_disallow
        is_disallowed = False
        id_lanes = self.ids_lanes[id_edge]
        is_cont = True
        while is_cont & (ind < len(id_lanes)):
            id_lane = id_lanes[ind]
            if len(modes_allow[id_lane]) > 0:
                if id_mode in modes_allow[id_lane]:
                    return ind
                else:
                    ind += 1

            elif len(modes_disallow[id_lane]) > 0:
                if id_mode in modes_disallow[id_lane]:
                    ind += 1
                else:
                    return ind
            else:
                # no restrictions
                return ind

        # no unrestricted lane found
        return -1  # not allowed on this edge

    def multimake(self, ids_sumo=[], **kwargs):
        # fixing of insufficient shape data in edge reader
        return self.add_rows(n=len(ids_sumo), ids_sumo=ids_sumo,  **kwargs)

    def make(self, id_fromnode=0,
             id_tonode=0,
             id_sumo='',
             type_edge='',
             num_lanes=1,
             speed_max=50.0 / 3.6,
             priority=1,
             #length = 0.0,
             shape=[],
             type_spread='right',
             name='',
             offset_end=0.0,
             width_lanes_default=None,
             width_sidewalk=-1,
             ):

        if len(shape) < 2:  # insufficient shape data
            #shape = np.array([ nodes.coords[id_fromnode], nodes.coords[id_tonode] ], np.float32)
            # shape should be a list of np array coords
            # ATTENTIOn: we need to copy here, otherwise the reference
            # to node coordinates will be kept!!
            coords = self.ids_tonode.get_linktab().coords
            shape = [1.0 * coords[id_fromnode], 1.0 * coords[id_tonode]]

        # print 'Edges.make'
        # print '  shape',shape,type(shape)

        return self.add_row(ids_sumo=id_sumo,
                            ids_fromnode=id_fromnode,
                            ids_tonode=id_tonode,
                            types=type_edge,
                            nums_lanes=num_lanes,
                            speeds_max=speed_max,
                            priorities=priority,
                            #lengths = length,
                            shapes=shape,
                            types_spread=self.types_spread.choices[
                                type_spread],
                            names=name,
                            offsets_end=offset_end,
                            widths_lanes_default=width_lanes_default,
                            widths_sidewalk=width_sidewalk,
                            )

    def make_segment_edge_map(self):
        """
        Generates a vertex matrix with line segments of all edges
        and a map that maps each line segment to edge index.
        """

        # here we can make some selection on edge inds
        inds = self.get_inds()
        # print 'make_linevertices',len(inds)

        linevertices = np.zeros((0, 2, 3), np.float32)
        vertexinds = np.zeros((0, 2), np.int32)
        polyinds = []

        lineinds = []
        #linecolors = []
        #linecolors_highl = []
        linebeginstyles = []
        lineendstyles = []

        i = 0
        ind_line = 0
        polylines = self.shapes.value[inds]
        for ind in inds:

            polyline = polylines[ind]
            n_seg = len(polyline)
            # print '  =======',n_seg#,polyline

            if n_seg > 1:
                polyvinds = range(n_seg)
                # print '  polyvinds\n',polyvinds
                vi = np.zeros((2 * n_seg - 2), np.int32)
                vi[0] = polyvinds[0]
                vi[-1] = polyvinds[-1]

                # Important type conversion!!
                v = np.zeros((2 * n_seg - 2, 3), np.float32)
                v[0] = polyline[0]
                v[-1] = polyline[-1]
                if len(v) > 2:

                    # print 'v[1:-1]',v[1:-1]
                    # print 'v=\n',v
                    #m = np.repeat(polyline[1:-1],2,0)
                    # print 'm\n',m,m.shape,m.dtype
                    #v[1:-1] = m
                    v[1:-1] = np.repeat(polyline[1:-1], 2, 0)
                    vi[1:-1] = np.repeat(polyvinds[1:-1], 2)
                #vadd = v.reshape((-1,2,3))
                # print '  v\n',v
                # print '  vi\n',vi

                n_lines = len(v) / 2
                # print '  v\n',v
                polyinds += n_lines * [ind]
                lineinds.append(np.arange(ind_line, ind_line + n_lines))
                ind_line += n_lines
                # print '  polyinds\n',polyinds,n_lines
                #linecolors += n_lines*[colors[ind]]
                #linecolors_highl += n_lines*[colors_highl[ind]]

                # print '  linebeginstyle',linebeginstyle,beginstyles[ind]

            else:
                # empty polygon treatment
                v = np.zeros((0, 3), np.float32)
                vi = np.zeros((0), np.int32)

            linevertices = np.concatenate(
                (linevertices, v.reshape((-1, 2, 3))))
            vertexinds = np.concatenate((vertexinds, vi.reshape((-1, 2))))
            # print '  linevertex\n',linevertices
            i += 1
        self._segvertices = linevertices

        self._edgeinds = np.array(polyinds, np.int32)
        self._seginds = lineinds
        self._segvertexinds = np.array(vertexinds, np.int32)

    def get_closest_edge(self, p):
        """
        Returns edge ids which is closest to point p.
        Requires execution of make_segment_edge_map
        """
        # print 'get_closest_edge',p
        if len(self) == 0:
            return np.array([], np.int)

        vertices = self._segvertices
        x1 = vertices[:, 0, 0]
        y1 = vertices[:, 0, 1]

        x2 = vertices[:, 1, 0]
        y2 = vertices[:, 1, 1]

        # print '  x1', x1
        # print '  x2', x2
        #halfwidths = 0.5*self.get_widths_array()[self._polyinds]
        d2 = get_dist_point_to_segs(p[0:2], x1, y1, x2, y2, is_ending=True)
        # print '
        # min(d2)=',np.min(d2),'argmin=',np.argmin(d2),self.get_ids(self._edgeinds[np.argmin(d2)])
        return self.get_ids(self._edgeinds[np.argmin(d2)])

    def export_sumoxml(self, filepath, encoding='UTF-8'):
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_sumoxml: could not open', filepath
            return False
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)

        fd.write(xm.begin('edges'))
        indent = 2
        self.write_xml(fd, indent=indent, is_print_begin_end=False)
        self.parent.roundabouts.write_xml(
            fd, indent=indent, is_print_begin_end=False)
        fd.write(xm.end('edges'))
        fd.close()

    def update(self, ids=None, is_update_lanes=False):
        # print 'Edges.update'

        if ids == None:
            self.widths.value = self.nums_lanes.value * self.widths_lanes_default.value \
                + (self.widths_sidewalk.value >= 0) * \
                (self.widths_sidewalk.value - self.widths_lanes_default.value)

            # print '  self.widths.values =  \n',self.widths.value
            #polylines = polypoints_to_polylines(self.shapes.value)
            # print '  polylines[0:4]=\n',polylines[0:4]
            # print '  polylines[3].shape',polylines[3].shape
            #self.lengths.value = get_length_polylines(polypoints_to_polylines(self.shapes.value))

            self.lengths.value = get_length_polypoints(self.shapes.value)
            ids = self.get_ids()
        else:
            self.widths[ids] = self.nums_lanes[ids] * self.widths_lanes_default[ids] \
                + (self.widths_sidewalk[ids] >= 0) * \
                (self.widths_sidewalk[ids] - self.widths_lanes_default[ids])
            # print '
            # self.shapes[ids]',self.shapes[ids],type(self.shapes[ids])
            self.lengths[ids] = get_length_polypoints(self.shapes[ids])

        self.widths.set_modified(True)
        self.lengths.set_modified(True)

        if is_update_lanes:
            # print 'recalc laneshapes',ids
            lanes = self.get_lanes()
            for id_edge in ids:
                lanes.reshape_edgelanes(id_edge)

    def set_shapes(self, ids, vertices, is_update_lanes=True):
        # print 'set_shapes',ids,vertices

        self.shapes[ids] = vertices
        if not hasattr(ids, '__iter__'):
            ids = [ids]
        self.update(ids, is_update_lanes=is_update_lanes)

    def update_lanes(self, id_edge, ids_lane):
        # print 'update_lanes',id_edge,self.ids_sumo[id_edge] ,ids_lanes,self.nums_lanes[id_edge]
        # if self._is_laneshape:
        #    laneshapes = edges.get_laneshapes(self._id_edge, )
        #    lanes.shapes[self._ids_lanes[0]]
        if len(ids_lane) == 0:
            # no lanes given...make some with default values
            ids_lane = []
            lanes = self.get_lanes()
            for i in xrange(self.nums_lanes[id_edge]):
                id_lane = lanes.make(index=i, id_edge=id_edge)
                ids_lane.append(id_lane)

        self.ids_lanes[id_edge] = ids_lane

    def correct_endpoint(self):
        """
        Corrects end-point for older versione. 
        """
        ids_sumo = self.ids_sumo.get_value()
        types_spread = self.types_spread.get_value()
        shapes = self.shapes.get_value()
        ids_fromnode = self.ids_fromnode.get_value()
        ids_tonode = self.ids_tonode.get_value()
        coords = self.parent.nodes.coords
        ind = 0
        is_corrected = False
        eps = 50.0
        for id_sumo, type_spread, shape, id_fromnode, id_tonode in zip(ids_sumo, types_spread, shapes, ids_fromnode, ids_tonode):

            inds_oppo = np.flatnonzero(
                (ids_tonode == id_fromnode) & (ids_fromnode == id_tonode))
            if len(inds_oppo) >= 1:
                ind_oppo = inds_oppo[0]
                # print '  correct',id_sumo,ids_sumo[ind_oppo]

                ind_oppo = inds_oppo[0]
                shape_oppo = list(shapes[ind_oppo])
                shape_oppo.reverse()
                # print '  shape',shape
                # print '  shape',shape_oppo
                # print '  id_fromnode',id_fromnode,ids_tonode[ind_oppo]
                # print '  id_tomnode',id_tonode,ids_fromnode[ind_oppo]
                # print '  coords',coords[id_fromnode], coords[id_tonode]
                if len(shape_oppo) == len(shape):

                    shapes[ind][0] = coords[id_fromnode]
                    shapes[ind_oppo][-1] = coords[id_fromnode]
                    #types_spread[inds_oppo[0]] = 0
                    #types_spread[ind] = 0
                    is_corrected = True
            ind += 1

        if is_corrected:
            self.update(is_update_lanes=True)

    def correct_spread(self):
        """
        Corrects spread type for older versione. 
        """
        ids_sumo = self.ids_sumo.get_value()
        types_spread = self.types_spread.get_value()
        shapes = self.shapes.get_value()
        ind = 0
        is_corrected = False
        eps = 50.0
        for id_sumo, type_spread, shape in zip(ids_sumo, types_spread, shapes):
            if type_spread == 1:
                if id_sumo[0] == '-':
                    inds_oppo = np.flatnonzero(ids_sumo == id_sumo[1:])
                    if len(inds_oppo) == 1:
                        ind_oppo = inds_oppo[0]
                        shape_oppo = np.array(shapes[ind_oppo], np.float32)
                        if len(shape_oppo) == len(shape):
                            shape_oppo = list(shapes[ind_oppo])
                            shape_oppo.reverse()
                            shape_oppo = np.array(shape_oppo, np.float32)
                            dist = np.sum(
                                np.abs(shape_oppo - np.array(shape, np.float32))) / float(len(shape))
                            # print '   id_sumo,dist',id_sumo,dist,eps
                            if dist < eps:
                                types_spread[inds_oppo[0]] = 0
                                types_spread[ind] = 0
                                is_corrected = True
            ind += 1

        if is_corrected:
            self.update(is_update_lanes=True)


class Nodes(am.ArrayObjman):
    # http://www.sumo.dlr.de/userdoc/Networks/Building_Networks_from_own_XML-descriptions.html#Node_Descriptions

    def __init__(self, parent,
                 **kwargs):
        ident = 'nodes'
        self._init_objman(ident=ident, parent=parent, name='Nodes',
                          xmltag=('nodes', 'node', 'ids_sumo'),
                          version=0.1,
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):

        self.add_col(SumoIdsConf('Node'))

        self.add_col(am.ArrayConf('coords',  np.zeros(3, dtype=np.float32),
                                  dtype=np.float32,
                                  groupnames=['state'],
                                  perm='r',
                                  name='Coords',
                                  unit='m',
                                  info='Node center coordinates.',
                                  ))

        self.add_col(am.ArrayConf('radii',  5.0,
                                  dtype=np.float32,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Radius',
                                  info='Node radius',
                                  ))

        self.add(cm.AttrConf('radius_default', 3.0,
                             groupnames=['options'],
                             perm='rw',
                             unit='m',
                             name='Default radius',
                             info='Default node radius.',
                             ))

        self.add_col(am.ArrayConf('types', 0,
                                  choices={
                                      "priority": 0,
                                      "traffic_light": 1,
                                      "right_before_left": 2,
                                      "unregulated": 3,
                                      "priority_stop": 4,
                                      "traffic_light_unregulated": 5,
                                      "allway_stop": 6,
                                      "rail_signal": 7,
                                      "zipper": 8,
                                      "traffic_light_right_on_red": 9,
                                      "rail_crossing": 10,
                                      "dead_end": 11,
                                  },
                                  dtype=np.int32,
                                  perm='rw',
                                  name='Type',
                                  info='Node type.',
                                  xmltag='type',
                                  ))

        # this is actually a property defined in the TLS logic
        self.add_col(am.ArrayConf('types_tl', 0,
                                  dtype=np.int32,
                                  choices={
                                      "none": 0,
                                      "static": 1,
                                      "actuated": 2,
                                  },
                                  perm='rw',
                                  name='TL type',
                                  info='Traffic light type.',
                                  xmltag='tlType',
                                  ))

        self.add_col(am.ArrayConf('turnradii',  1.5,
                                  dtype=np.float32,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Turn rad',
                                  unit='m',
                                  info='optional turning radius (for all corners) for that node.',
                                  xmltag='radius',
                                  ))

        self.add_col(am.ArrayConf('are_keep_clear',  True,
                                  dtype=np.bool,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='keep clear',
                                  info='Whether the junction-blocking-heuristic should be activated at this node.',
                                  xmltag='keepClear',
                                  ))

        if self.get_version() < 0.1:
            self.delete('ids_tl_prog')
            self.turnradii.xmltag = 'radius'
            self.are_keep_clear.xmltag = 'keepClear'
            self.types_tl.xmltag = 'tlType'
            self.add_col(am.IdlistsArrayConf('ids_controlled', edges,
                                             groupnames=['state'],
                                             name='IDs controlled',
                                             info='ID list of controlled edges. Edges which shall be controlled by a joined TLS despite being incoming as well as outgoing to the jointly controlled nodes.',
                                             ))

    def set_edges(self, edges):

        self.add_col(am.IdlistsArrayConf('ids_incoming', edges,
                                         groupnames=['state'],
                                         name='ID incoming',
                                         info='ID list of incoming edges.',
                                         ))

        self.add_col(am.IdlistsArrayConf('ids_outgoing', edges,
                                         groupnames=['state'],
                                         name='ID outgoing',
                                         info='ID list of outgoing edges.',
                                         ))
        self.add_col(am.IdlistsArrayConf('ids_controlled', edges,
                                         groupnames=['state'],
                                         name='IDs controlled',
                                         info='ID list of controlled edges. Edges which shall be controlled by a joined TLS despite being incoming as well as outgoing to the jointly controlled nodes.',
                                         xmltag='controlledInner',
                                         ))

    def set_tlss(self, tlss):

        self.add_col(am.IdsArrayConf('ids_tls',  tlss,
                                     groupnames=['state'],
                                     name='ID Tls',
                                     info='ID of traffic light system (TLS). Nodes with the same tls-value will be joined into a single traffic light system.',
                                     xmltag='tl',
                                     ))

    def multimake(self, ids_sumo=[], **kwargs):
        return self.add_rows(n=len(ids_sumo), ids_sumo=ids_sumo,  **kwargs)

    def make(self, id_sumo='', nodetype='priority', coord=[],
             type_tl='Static', id_tl_prog=0,
             turnradius=1.5, is_keep_clear=True):

        return self.add_row(ids_sumo=id_sumo,
                            types=self.types.choices[nodetype],
                            coords=coord,
                            types_tl=self.types_tl.choices[type_tl],
                            ids_tl_prog=id_tl_prog,
                            turnradii=turnradius,
                            are_keep_clear=is_keep_clear,
                            )

    def add_outgoing(self, id_node, id_edge):
        if self.ids_outgoing[id_node] != None:
            if id_edge not in self.ids_outgoing[id_node]:
                self.ids_outgoing[id_node].append(id_edge)
        else:
            self.ids_outgoing[id_node] = [id_edge]

    def add_incoming(self, id_node, id_edge):
        if self.ids_incoming[id_node] != None:
            if id_edge not in self.ids_incoming[id_node]:
                self.ids_incoming[id_node].append(id_edge)
        else:
            self.ids_incoming[id_node] = [id_edge]

    def export_sumoxml(self, filepath, encoding='UTF-8'):
        try:
            fd = open(filepath, 'w')
        except:
            print 'WARNING in export_sumoxml: could not open', filepath
            return False
        fd.write('<?xml version="1.0" encoding="%s"?>\n' % encoding)
        indent = 0
        self.write_xml(fd, indent)
        fd.close()

    def write_xml(self, fd, indent):
        # print 'Nodes.write_xml'
        xmltag, xmltag_item, attrname_id = self.xmltag
        attrsman = self.get_attrsman()
        # getattr(self.get_attrsman(), attrname_id)
        attrconfig_id = attrsman.get_config(attrname_id)
        xmltag_id = attrconfig_id.xmltag
        #attrsman = self.get_attrsman()
        coordsconfig = attrsman.get_config('coords')
        colconfigs = attrsman.get_colconfigs(is_all=True)

        # print '  header'
        fd.write(xm.start(xmltag, indent))
        # print '  ', self.parent.get_attrsman().get_config('version').attrname,self.parent.get_attrsman().get_config('version').get_value()
        #fd.write( self.parent.get_attrsman().get_config('version').write_xml(fd) )
        self.parent.get_attrsman().get_config('version').write_xml(fd)
        fd.write(xm.stop())

        fd.write(xm.start('location', indent + 2))
        # print '  groups:',self.parent.get_attrsman().get_groups()

        for attrconfig in self.parent.get_attrsman().get_group('location'):
            # print '    locationconfig',attrconfig.attrname
            attrconfig.write_xml(fd)
        fd.write(xm.stopit())

        for _id in self.get_ids():
            fd.write(xm.start(xmltag_item, indent + 2))

            # print ' make tag and id',_id
            fd.write(xm.num(xmltag_id, attrconfig_id[_id]))

            # print ' write columns'
            for attrconfig in colconfigs:
                # print '    colconfig',attrconfig.attrname
                if attrconfig == coordsconfig:
                    x, y, z = attrconfig[_id]
                    fd.write(xm.num('x', x))
                    fd.write(xm.num('y', y))
                    fd.write(xm.num('z', z))

                elif attrconfig != attrconfig_id:
                    attrconfig.write_xml(fd, _id)
            fd.write(xm.stopit())

        fd.write(xm.end(xmltag, indent))

    # def clean_node(self, id_node):

    def clean(self, is_reshape_edgelanes=False, nodestretchfactor=2.8, n_min_nodeedges=2):
        #is_reshape_edgelanes = False
        print 'Nodes.clean', len(self), 'is_reshape_edgelanes', is_reshape_edgelanes

        edges = self.parent.edges
        lanes = self.parent.lanes
        rad_min = self.radius_default.value

        # print '  id(edges.shapes),id(edges.shapes.value)', id(edges.shapes),id(edges.shapes.value)#,edges.shapes.value
        # print '  id(self.coords),id(self.coords.value)', id(self.coords),id(self.coords.value)#,self.coords.value
        # print '  self.coords.value.shape',self.coords.value.shape
        # print '
        # len(self.coords),self.coords.shape',len(self.coords.value),self.coords.value
        for id_node in self.get_ids():
            ind_node = self.get_inds(id_node)
            # if id_node in TESTNODES:
            #    print 79*'_'
            #    print '  node',id_node
            #    print '   coords',self.coords[id_node]
            #    print '   coords',TESTNODES[0],self.coords[TESTNODES[0]]
            #    print '   coords',TESTNODES[1],self.coords[TESTNODES[1]]
            #    print '   radii',self.radii[id_node]

            # distanza ad altri nodi
            #d = np.sum(np.abs(self.coords[id_node]-self.coords.value),1)
            #d = np.linalg.norm(self.coords[id_node]-self.coords.value,1)
            coords = self.coords[id_node]
            d = get_norm_2d(coords - self.coords.value)
            d[ind_node] = np.inf
            d_min = np.min(d)
            # print '  d_min',d_min
            ids_edge_out = edges.select_ids(
                edges.ids_fromnode.value == id_node)
            ids_edge_in = edges.select_ids(edges.ids_tonode.value == id_node)

            # estimate circumference of junction and determine node radius
            n_edges = len(ids_edge_in) + len(ids_edge_out)
            width_av = np.mean(np.concatenate(
                (edges.widths[ids_edge_in], edges.widths[ids_edge_out])))

            # here we assume a node with 6 entrance sides and a and 2 average width edges per side
            #circum = 2.0*max(6,n_edges)*width_av
            circum = nodestretchfactor * max(2, n_edges) * width_av

            # print '
            # n_edges,width_av,radius',n_edges,width_av,max(6,n_edges)*width_av/(2*np.pi)
            radius = min(
                max(circum / (n_min_nodeedges * np.pi), rad_min), 0.4 * d_min)
            self.radii[id_node] = radius

            # if id_node in TESTNODES:
            #    print '  AFTER change radius:'#OK
            #    print '   coords',TESTNODES[0],self.coords[TESTNODES[0]]
            #    print '   coords',TESTNODES[1],self.coords[TESTNODES[1]]

            for id_edge in ids_edge_in:
                # print '    in edge',id_edge
                shape = edges.shapes[id_edge]
                n_shape = len(shape)
                # edges.shapes[id_edge][::-1]:
                for i in xrange(n_shape - 1, -1, -1):
                    d = get_norm_2d(np.array([shape[i] - coords]))[0]
                    # print '      i,d,r',i , d, radius,d>radius
                    if d > radius:
                        # print '        **',i,d, radius
                        break
                x, y = shape[i][:2]
                # print 'shape',shape,
                #dx,dy = shape[i+1][:2] - shape[i][:2]
                dx, dy = coords[:2] - shape[i][:2]
                dn = np.sqrt(dx * dx + dy * dy)
                x1 = x + (d - radius) * dx / dn
                y1 = y + (d - radius) * dy / dn

                if i == n_shape - 1:

                    shape[-1][:2] = [x1, y1]
                    edges.shapes[id_edge] = shape

                else:  # elif i>0:
                    shape[i + 1][:2] = [x1, y1]
                    edges.shapes[id_edge] = shape[:i + 2]

                # print '    x,y',x,y
                # print '    x1,y1',x1,y1
                # print '  shape[:i+2]',shape[:i+2]
                # print '  shapes[id_edge]',edges.shapes[id_edge]
                if is_reshape_edgelanes:
                    lanes.reshape_edgelanes(id_edge)

            for id_edge in ids_edge_out:
                # print '    out edge',id_edge
                shape = edges.shapes[id_edge]
                n_shape = len(shape)
                # edges.shapes[id_edge][::-1]:
                for i in xrange(n_shape):
                    d = get_norm_2d(np.array([shape[i] - coords]))[0]
                    # print '      i,d,r',i , d, radius,d>radius
                    if d > radius:
                        # print '        **',i,d, radius
                        break
                x, y = coords[:2]  # shape[i-1][:2]
                # print 'shape',shape,
                #dx,dy = shape[i][:2]- shape[i-1][:2]
                dx, dy = shape[i][:2] - coords[:2]
                dn = np.sqrt(dx * dx + dy * dy)
                x1 = x + (radius) * dx / dn
                y1 = y + (radius) * dy / dn
                if i == 0:
                    shape[0][:2] = [x1, y1]
                    edges.shapes[id_edge] = shape

                elif i < n_shape:

                    shape[i - 1][:2] = [x1, y1]
                    edges.shapes[id_edge] = shape[i - 1:]
                # print '    x,y',x,y
                # print '    x1,y1',x1,y1
                # print '  shape[:i+2]',shape[:i+2]
                # print '  shapes[id_edge]',edges.shapes[id_edge]
                if is_reshape_edgelanes:
                    lanes.reshape_edgelanes(id_edge)

        self.radii.set_modified(True)
        edges.shapes.set_modified(True)


class Network(cm.BaseObjman):

    def __init__(self, parent=None, name='Network', **kwargs):
            # print 'Network.__init__',parent,name
        self._init_objman(ident='net', parent=parent, name=name,
                          # xmltag = 'net',# no, done by netconvert
                          **kwargs)
        attrsman = self.set_attrsman(cm.Attrsman(self))
        # print '  Network.parent',self.parent
        self._init_attributes()
        self.modes.add_rows(ids=MODES.values(), names=MODES.keys())

    def _init_attributes(self):
        attrsman = self.get_attrsman()
        self.version = attrsman.add(cm.AttrConf('version', '0.25',
                                                groupnames=['aux'],
                                                perm='r',
                                                name='Network version',
                                                info='Sumo network version',
                                                xmltag='version'
                                                ))

        self.modes = attrsman.add(cm.ObjConf(Modes(self)))
        # print 'Network.__init__'
        # print '  MODES.values()',MODES.values()
        # print '  MODES.keys()',MODES.keys()

        # self.modes.print_attrs()
        ##

        ##
        self.nodes = attrsman.add(cm.ObjConf(Nodes(self)))
        self.edges = attrsman.add(cm.ObjConf(Edges(self)))
        self.lanes = attrsman.add(cm.ObjConf(
            Lanes(self, self.edges, self.modes)))

        self.edges.set_nodes(self.nodes)
        self.edges.set_lanes(self.lanes)
        self.nodes.set_edges(self.edges)
        self.roundabouts = attrsman.add(cm.ObjConf(
            Roundabouts(self, self.edges, self.nodes)))
        self.connections = attrsman.add(cm.ObjConf(Connections(self)))
        self.crossings = attrsman.add(cm.ObjConf(Crossings(self)))
        self.tlss = attrsman.add(cm.ObjConf(TrafficLightSystems(self)))
        self.nodes.set_tlss(self.tlss)

        self._offset = attrsman.add(cm.AttrConf('_offset', np.array([0.0, 0.0], dtype=np.float32),
                                                groupnames=['location', ],
                                                perm='r',
                                                name='Offset',
                                                info='Network offset in WEP coordinates',
                                                xmltag='netOffset',
                                                xmlsep=',',
                                                ))

        self._projparams = attrsman.add(cm.AttrConf('_projparams', "!",
                                                    groupnames=['location', ],
                                                    perm='r',
                                                    name='Projection',
                                                    info='Projection parameters',
                                                    xmltag='projParameter',
                                                    ))

        self._boundaries = attrsman.add(cm.AttrConf('_boundaries', np.array([0.0, 0.0, 0.0, 0.0], dtype=np.float32),
                                                    groupnames=['location', ],
                                                    perm='r',
                                                    name='Boundaries',
                                                    unit='m',
                                                    info='Network boundaries',
                                                    xmltag='convBoundary',
                                                    xmlsep=',',
                                                    ))

        self._boundaries_orig = attrsman.add(cm.AttrConf('_boundaries_orig', np.array([0.0, 0.0, 0.0, 0.0]),
                                                         groupnames=[
                                                             'location', ],
                                                         perm='r',
                                                         name='Orig. boundaries',
                                                         info='Original network boundaries',
                                                         xmltag='origBoundary',
                                                         xmlsep=',',
                                                         ))

    def _init_constants(self):
        pass
        #self._oldoffset = self._offset.copy()
        # print 'net._init_constants',self._offset,self._oldoffset
    # def set_oldoffset(self, offset):
    #    """
    #    Set explicitely an old net offset, if existing.
    #    This allows to update coordinates and shapes outside the network.
    #
    #    """
    #    self._oldoffset = offset

    def set_version(self, version):
        self.version = version

    def get_version(self):
        return self.version

    def is_empty(self):
        return (len(self.nodes) == 0) & (len(self.edges) == 0)

    def set_offset(self, offset):
        # if (offset is not self._offset) :
        #        self._oldoffset = self._offset.copy()
        self._offset = offset

    def get_offset(self):
        return self._offset

    # def is_offset_change(self):
    #    """
    #    Returns true if offset changed be approx 1 mm after last net import
    #    """
    #    return np.sum(abs(self._oldoffset - self._offset))>0.002

    # def get_deltaoffset(self):
    #    return self._offset - self._oldoffset

    # def remove_oldoffset(self):
    #    self._oldoffset = None

    def set_boundaries(self, convBoundary, origBoundary=None):
        """
        Format of Boundary box
         [MinX, MinY ,MaxX, MaxY ]

        """
        self._boundaries = convBoundary
        if origBoundary == None:
            self._boundaries_orig = self._boundaries
        else:
            self._boundaries_orig = origBoundary

    def get_boundaries(self):
        return self._boundaries, self._boundaries_orig

    def merge_boundaries(self, convBoundary, origBoundary=None):
        """
        Format of Boundary box
         [MinX, MinY ,MaxX, MaxY ]

        """
        # print 'mergeBoundaries'
        self._boundaries = self.get_boundary_union(
            convBoundary, self._boundaries)
        if origBoundary == None:
            self._boundaries_orig = self._boundaries
        else:
            self._boundaries_orig = self.get_boundary_union(
                origBoundary, self._boundaries_orig)
        # print '  self._boundaries_orig =',self._boundaries_orig
        # print '  self._boundaries =',self._boundaries

    def get_boundary_union(self, BB1, BB2):
        return [min(BB1[0], BB2[0]), min(BB1[1], BB2[1]), max(BB1[2], BB2[2]), max(BB1[3], BB2[3])]

    def get_projparams(self):
        return self._projparams

    def set_projparams(self, projparams="!"):
        # print 'setprojparams',projparams
        self._projparams = projparams

    def get_rootfilename(self):
        if self.parent is not None:  # scenario exists
            return self.parent.get_rootfilename()
        else:
            return self.get_ident()

    def get_rootfilepath(self):
        if self.parent is not None:
            return self.parent.get_rootfilepath()
        else:
            return os.path.join(os.getcwd(), self.get_rootfilename())

    def get_filepath(self):
        """
        Default network filepath.
        """
        return self.get_rootfilepath() + '.net.xml'

    # def clear(self):
    #    """
    #    Remove all netelements.
    #    """
    #    #self.reset()
    #    # at some stage in the future this should be automatic
    #    self.nodes.clear()
    #    self.edges.clear()
    #    self.lanes.clear()
    #    self.roundabouts.clear()
    #    self.connections.clear()
    #    self.crossings.clear()
    #    self.tlss.clear()

    def call_netedit(self, filepath=None, is_maps=False):

        filepath = self.export_netxml()
        if filepath != "":
            # print '  netconvert: success'
            names = os.path.basename(filepath).split('.')
            dirname = os.path.dirname(filepath)
            if len(names) >= 3:
                rootname = '.'.join(names[:-2])
            elif len(names) <= 2:
                rootname = names[0]

            configfilepath = self._write_guiconfig(rootname, dirname, is_maps)

            cml = 'netedit '\
                + ' --sumo-net-file ' + filepathlist_to_filepathstring(filepath)\
                + ' --gui-settings-file ' + \
                filepathlist_to_filepathstring(configfilepath)
            #+ ' --output-prefix '+ filepathlist_to_filepathstring(os.path.join(dirname,rootname))

            proc = subprocess.Popen(cml, shell=True)
            # print '  run_cml cml=',cml
            # print '  pid = ',proc.pid
            proc.wait()
            if proc.returncode == 0:
                print '  netedit:success'

                return self.import_netxml()
                # return self.import_xml() # use if netedit exports to plain
                # xml files
            else:
                print '  netedit:error'
                return False
        else:
            print '  netconvert:error'
            return False

    def call_sumogui(self, filepath=None, is_maps=False):

        if filepath == None:
            filepath = self.get_filepath()
            dirname = os.path.dirname(filepath)
        names = os.path.basename(filepath).split('.')
        dirname = os.path.dirname(filepath)
        if len(names) >= 3:
            rootname = '.'.join(names[:-2])
        elif len(names) <= 2:
            rootname = names[0]

        configfilepath = self._write_guiconfig(rootname, dirname, is_maps)

        polyfilepath = os.path.join(dirname, rootname + '.poly.xml')
        if os.path.isfile(polyfilepath):
            option_addfiles = '  --additional-files ' + \
                filepathlist_to_filepathstring(polyfilepath)
        else:
            option_addfiles = ''

        cml = 'sumo-gui '\
            + ' --net-file ' + filepathlist_to_filepathstring(filepath)\
            + ' --gui-settings-file ' + filepathlist_to_filepathstring(configfilepath)\
            + option_addfiles

        proc = subprocess.Popen(cml, shell=True)
        print '  run_cml cml=', cml
        print '  pid = ', proc.pid
        proc.wait()
        return proc.returncode

    def _write_guiconfig(self, rootname, dirname, is_maps):
        # check if there are maps
        maps = None
        if is_maps:
            if self.parent != None:
                maps = self.parent.landuse.maps

        # write netedit configfile
        templatedirpath = os.path.dirname(os.path.abspath(__file__))
        fd_template = open(os.path.join(
            templatedirpath, 'netedit_config.xml'), 'r')
        configfilepath = os.path.join(dirname, rootname + '.netedit.xml')
        fd_config = open(configfilepath, 'w')
        for line in fd_template.readlines():
            if line.count('<decals>') == 1:
                fd_config.write(line)
                if is_maps:
                    maps.write_decals(fd_config, indent=12)
            else:
                fd_config.write(line)

        fd_template.close()
        fd_config.close()

        return configfilepath

    def import_netxml(self, filepath=None, rootname=None, is_clean_nodes=False, is_remove_xmlfiles=False):
        print 'import_netxml', filepath

        if rootname == None:
            rootname = self.get_rootfilename()

        if filepath == None:
            filepath = self.get_filepath()

        dirname = os.path.dirname(filepath)

        # print '  modes.names',self.modes.names
        cml = 'netconvert'\
            + ' --sumo-net-file ' + filepathlist_to_filepathstring(filepath)\
            + ' --plain-output-prefix ' + \
            filepathlist_to_filepathstring(os.path.join(dirname, rootname))
        proc = subprocess.Popen(cml, shell=True)
        print '  run_cml cml=', cml
        print '  pid = ', proc.pid
        proc.wait()
        if not proc.returncode:
            print '  modes.names', self.modes.names
            return self.import_xml(rootname, dirname)
        else:
            return False

    def export_netxml(self, filepath=None):

        if filepath == None:
            filepath = self.get_filepath()

        print 'Net.export_netxml', filepath

        # now create rootfilepath in order to export first
        # the various xml file , then call netconvert
        names = os.path.basename(filepath).split('.')
        dirname = os.path.dirname(filepath)
        if len(names) >= 3:
            rootname = '.'.join(names[:-2])
        elif len(names) <= 2:
            rootname = names[0]

        filepath_edges = os.path.join(dirname, rootname + '.edg.xml')
        filepath_nodes = os.path.join(dirname, rootname + '.nod.xml')
        filepath_connections = os.path.join(dirname, rootname + '.con.xml')
        filepath_tlss = os.path.join(dirname, rootname + '.tll.xml')

        self.edges.export_sumoxml(filepath_edges)
        self.nodes.export_sumoxml(filepath_nodes)
        self.connections.export_sumoxml(filepath_connections)
        if len(self.tlss) > 0:
            self.tlss.export_sumoxml(filepath_tlss)

        cml = 'netconvert --verbose --ignore-errors.edge-type'\
            + ' --node-files ' + filepathlist_to_filepathstring(filepath_nodes)\
            + ' --edge-files ' + filepathlist_to_filepathstring(filepath_edges)\
            + ' --connection-files ' + filepathlist_to_filepathstring(filepath_connections)\
            + ' --output-file ' + filepathlist_to_filepathstring(filepath)

        if len(self.tlss) > 0:
            cml += ' --tllogic-files ' + \
                filepathlist_to_filepathstring(filepath_tlss)

        proc = subprocess.Popen(cml, shell=True)
        print 'run_cml cml=', cml
        print '  pid = ', proc.pid
        proc.wait()
        if proc.returncode == 0:
            print '  success'
            return filepath
        else:
            print '  success'
            return ''

    def import_xml(self, rootname=None, dirname=None, is_clean_nodes=False, is_remove_xmlfiles=False):

        if not self.is_empty():
            oldoffset = self.get_offset()
        else:
            oldoffset = None
        print 'Network.import_xml oldoffset', oldoffset
        # remove current network
        # print '  remove current network'
        self.clear()
        # reload default SUMO MODES (maybe should not be here)
        self.modes.add_rows(ids=MODES.values(), names=MODES.keys())

        if rootname is None:
            rootname = self.get_rootfilename()

        if dirname == None:
            dirname = os.path.dirname(self.get_rootfilepath())

        # print 'import_xml',dirname,rootname
        nodefilepath = os.path.join(dirname, rootname + '.nod.xml')
        edgefilepath = os.path.join(dirname, rootname + '.edg.xml')
        confilepath = os.path.join(dirname, rootname + '.con.xml')
        tlsfilepath = os.path.join(dirname, rootname + '.tll.xml')

        if os.path.isfile(edgefilepath) & os.path.isfile(nodefilepath) & os.path.isfile(confilepath):
            nodereader = self.import_sumonodes(
                nodefilepath, is_remove_xmlfiles)
            edgereader = self.import_sumoedges(
                edgefilepath, is_remove_xmlfiles)
            if is_clean_nodes:
                # make edges and lanes end at the node boundaries
                # also recalculate lane shapes from edge shapes...if lane shapes are missing
                #self.lanes.reshape() #
                self.nodes.clean(is_reshape_edgelanes=True)
            else:
                # just recalculate lane shapes from edge shapes...if lane
                # shapes are missing
                self.lanes.reshape()
            #    #pass

            self.import_sumoconnections(confilepath, is_remove_xmlfiles)

            if os.path.isfile(tlsfilepath):
                self.import_sumotls(tlsfilepath, is_remove_xmlfiles)

            # this fixes some references to edges and tls
            nodereader.write_to_net_post()

            if oldoffset is not None:
                # check if offset changed
                # if self.is_offset_change():
                deltaoffset = self.get_offset() - oldoffset
                # print '  check
                # update_netoffset',deltaoffset,oldoffset,self.get_offset(),np.sum(abs(deltaoffset))>0.002
                if np.sum(abs(deltaoffset)) > 0.002:
                    # communicate to scenario
                    if self.parent is not None:
                        self.parent.update_netoffset(deltaoffset)

            # clean up ...should be done in each importer??
            # if is_remove_xmlfiles:
            #    os.remove(nodefilepath)
            #    os.remove(edgefilepath)
            #    os.remove(confilepath)

            #    if os.path.isfile(tlsfilepath):
            #        os.remove(tlsfilepath)
            return True
        else:
            self.get_logger().w(
                'import_sumonodes: files not found', key='message')
            return False

    def import_sumonodes(self, filename, is_remove_xmlfiles=False, logger=None, **others):
        print 'import_sumonodes', filename
        # print '  parent',self.parent
        self.get_logger().w('import_sumonodes', key='message')

        # timeit
        exectime_start = time.clock()

        counter = SumoNodeCounter()

        #reader = SumoEdgeReader(self, **others)
        # try:

        parse(filename, counter)
        # print '  after: n_edge', counter.n_edge
        fastreader = SumoNodeReader(self, counter)
        parse(filename, fastreader)

        fastreader.write_to_net()

        # timeit
        print '  exec time=', time.clock() - exectime_start
        return fastreader

    def import_sumoedges(self, filename, is_remove_xmlfiles=False, logger=None, **others):
        print 'import_sumoedges', filename
        logger = self.get_logger()
        logger.w('import_sumoedges', key='message')
        # timeit
        exectime_start = time.clock()

        counter = SumoEdgeCounter()

        #reader = SumoEdgeReader(self, **others)
        # try:

        parse(filename, counter)
        # print '  after: n_edge', counter.n_edge
        fastreader = SumoEdgeReader(self, counter)
        parse(filename, fastreader)

        fastreader.write_to_net()
        self.edges.update()

        if is_remove_xmlfiles:
            os.remove(filename)
        # timeit
        print '  exec time=', time.clock() - exectime_start

        # except KeyError:
        #    print >> sys.stderr, "Please mind that the network format has changed in 0.16.0, you may need to update your network!"
        #    raise
        return fastreader

    def import_sumoconnections(self, filename, is_remove_xmlfiles=False, logger=None, **others):
        print 'import_sumoedges', filename
        logger = self.get_logger()
        logger.w('import_sumoconnections', key='message')

        # timeit
        exectime_start = time.clock()

        counter = SumoConnectionCounter()

        parse(filename, counter)
        fastreader = SumoConnectionReader(self, counter)
        parse(filename, fastreader)

        fastreader.write_to_net()

        # timeit
        exectime_end = time.clock()
        print '  exec time=', exectime_end - exectime_start
        return fastreader

    def import_sumotls(self, filename, is_remove_xmlfiles=False, logger=None, **others):
        print 'import_sumotls', filename
        logger = self.get_logger()
        logger.w('import_sumotls', key='message')

        # timeit
        exectime_start = time.clock()

        reader = SumoTllReader(self)
        parse(filename, reader)

        # timeit
        exectime_end = time.clock()
        print '  exec time=', exectime_end - exectime_start
        return reader

    def get_id_mode(self, modename):
        return self.modes.get_id_mode(modename)

    def add_node(self, **kwargs):
        return self.nodes.make(**kwargs)

    def add_nodes(self,  **kwargs):
        # print 'add_nodes'
        return self.nodes.multimake(**kwargs)

    def add_edge(self,  **kwargs):
        # print 'add_edge'
        return self.edges.make(**kwargs)

    def add_edges(self,  **kwargs):
        # print 'add_edges'
        return self.edges.multimake(**kwargs)

    def add_roundabout(self, **kwargs):
        return self.roundabouts.make(**kwargs)

    def add_roundabouts(self, **kwargs):
        return self.roundabouts.multimake(**kwargs)

    def add_lane(self, **kwargs):
        # print 'add_lane\n',
        # for key, value in kwargs.iteritems():
        #    print '  ',key,type(value),value
        return self.lanes.make(**kwargs)

    def add_lanes(self, **kwargs):
        # print 'add_lanes\n',
        # for key, value in kwargs.iteritems():
        #    print '  ',key,type(value),value
        return self.lanes.multimake(**kwargs)

    def add_connection(self, id_fromlane=-1, id_tolane=-1, **kwargs):
        # print 'add_lane\n',
        # for key, value in kwargs.iteritems():
        #    print '  ',key,type(value),value

        id_fromedge = self.lanes.ids_edge[id_fromlane]
        id_toedge = self.lanes.ids_edge[id_tolane]
        id_node = self.edges.ids_tonode[id_fromedge]
        self.nodes.add_incoming(id_node, id_fromedge)
        self.nodes.add_outgoing(id_node, id_toedge)

        return self.connections.make(id_fromlane=id_fromlane, id_tolane=id_tolane, **kwargs)

    def add_connections(self, ids_fromlane=[], ids_tolane=[], **kwargs):
        # print 'add_lane\n',
        # for key, value in kwargs.iteritems():
        #    print '  ',key,type(value),value

        ids_fromedge = self.lanes.ids_edge[ids_fromlane]
        ids_toedge = self.lanes.ids_edge[ids_tolane]
        ids_node = self.edges.ids_tonode[ids_fromedge]
        add_incoming = self.nodes.add_incoming
        add_outgoing = self.nodes.add_outgoing
        for id_node, id_fromedge, id_toedge in zip(ids_node, ids_fromedge, ids_toedge):
            add_incoming(id_node, id_fromedge)
            add_outgoing(id_node, id_toedge)

        return self.connections.multimake(ids_fromlane=ids_fromlane, ids_tolane=ids_tolane, **kwargs)

    def add_crossing(self, **kwargs):
        # print 'add_crossing\n',
        return self.crossings.make(**kwargs)

    def add_crossings(self, **kwargs):
        # print 'add_crossings\n',
        return self.crossings.multimake(**kwargs)


class SumoConnectionCounter(handler.ContentHandler):

    """Parses a SUMO edge XML file and counts edges and lanes."""

    def __init__(self):
        self.n_con = 0
        self.n_cross = 0

    def startElement(self, name, attrs):
        if name == 'connection':
            self.n_con += 1

        if name == 'crossing':
            self.n_cross += 1


class SumoConnectionReader(handler.ContentHandler):

    """Parses a SUMO connection XML file"""

    def __init__(self, net, counter):
        self._net = net

        # print
        # 'SumoConnectionReader:n_con,n_cross',counter.n_con,counter.n_cross

        # connections
        self._ind_con = -1
        self.ids_fromlane = np.zeros(counter.n_con, np.int32)
        self.ids_tolane = np.zeros(counter.n_con, np.int32)
        self.are_passes = np.zeros(counter.n_con, np.bool)
        self.are_keep_clear = np.zeros(counter.n_con, np.bool)
        self.positions_cont = np.zeros(counter.n_con, np.float32)
        self.are_uncontrolled = np.zeros(counter.n_con, np.bool)

        # crossings
        self._ind_cross = -1
        self.ids_node = np.zeros(counter.n_cross, np.int32)
        self.ids_edges = np.zeros(counter.n_cross, np.object)
        self.widths = np.zeros(counter.n_cross, np.float32)
        self.are_priority = np.zeros(counter.n_cross, np.bool)
        self.are_discard = np.zeros(counter.n_cross, np.bool)

        self._ids_node_sumo = self._net.nodes.ids_sumo
        self._ids_edge_sumo = self._net.edges.ids_sumo
        self._ids_edgelanes = self._net.edges.ids_lanes

    def startElement(self, name, attrs):
        # print 'startElement',name

        if name == 'connection':
            # <connection from="153009994" to="153009966#1" fromLane="0" toLane="0" pass="1"/>
            self._ind_con += 1
            i = self._ind_con
            # print 'startElement',name,i
            id_fromedge = self._ids_edge_sumo.get_id_from_index(attrs['from'])
            id_toedge = self._ids_edge_sumo.get_id_from_index(attrs['to'])

            #id_fromlane = self._ids_edgelanes[id_fromedge][int(attrs.get('fromLane',0))]
            #id_tolane = self._ids_edgelanes[id_toedge][int(attrs.get('toLane',0))]

            # print '  id_sumo fromedge',
            # attrs['from'],len(self._ids_edgelanes[id_fromedge]) ,
            # int(attrs['fromLane'])
            self.ids_fromlane[i] = self._ids_edgelanes[
                id_fromedge][int(attrs['fromLane'])]
            self.ids_tolane[i] = self._ids_edgelanes[
                id_toedge][int(attrs['toLane'])]
            self.are_passes[i] = int(attrs.get('pass', 0))
            self.are_keep_clear[i] = int(attrs.get('keepClear ', 1))
            self.positions_cont[i] = float(attrs.get('contPos ', 0.0))
            self.are_uncontrolled[i] = int(attrs.get('uncontrolled', 0))

        if name == 'crossing':
            self._ind_cross += 1
            i = self._ind_cross
            # print 'startElement',name

            self.ids_node[i] = self._ids_node_sumo.get_id_from_index(attrs[
                                                                     'node'])
            self.ids_edges[i] = self._ids_edge_sumo.get_ids_from_indices(
                attrs['edges'].split(' '))
            self.widths[i] = float(attrs.get('width ', 4.0))
            self.are_priority[i] = int(attrs.get('priority ', 0))
            self.are_discard[i] = int(attrs.get('discard', 0))

    def write_to_net(self):

        # print 'write_to_net'
        ids_con = self._net.add_connections(
            ids_fromlane=self.ids_fromlane,
            ids_tolane=self.ids_tolane,
            sare_passes=self.are_passes,
            are_keep_clear=self.are_keep_clear,
            positions_cont=self.positions_cont,
            are_uncontrolled=self.are_uncontrolled,
        )

        ids_cross = self._net.add_crossings(
            ids_node=self.ids_node,
            ids_edges=self.ids_edges,
            widths=self.widths,
            are_priority=self.are_priority,
            are_discard=self.are_discard,
        )


class SumoNodeCounter(handler.ContentHandler):

    """Parses a SUMO edge XML file and counts edges and lanes."""

    def __init__(self):
        self.n_node = 0

    def startElement(self, name, attrs):
        # print 'startElement',name,self.n_edge,self.n_lane,self.n_roundabout
        if name == 'node':
            self.n_node += 1


class SumoNodeReader(handler.ContentHandler):

    """Parses a SUMO node XML file"""

    def __init__(self, net, counter):
        self._net = net

        # print 'SumoEdgeFastreader'

        #self._ids_node_sumo = net.nodes.ids_sumo
        #self._nodecoords = net.nodes.coords

        self._nodetypemap = self._net.nodes.types.choices
        self._tltypemap = self._net.nodes.types_tl.choices
        # node attrs
        self.ids_sumo = np.zeros(counter.n_node, np.object)
        self.types = np.zeros(counter.n_node, np.int32)
        self.coords = np.zeros((counter.n_node, 3), np.float32)
        self.types_tl = np.zeros(counter.n_node, np.int32)
        self.ids_sumo_tls = np.zeros(counter.n_node, np.object)
        self.turnradii = np.zeros(counter.n_node, np.float32)
        self.are_keep_clear = np.zeros(counter.n_node, np.bool)
        self._ind_node = -1
        self.ids_sumo_controlled = np.zeros(counter.n_node, np.object)
        self.ids_sumo_controlled[:] = None
        self._offset_delta = np.array([0.0, 0.0])
        self._isNew = len(self._net.nodes) == 0

    def write_to_net(self):

        # print 'write_to_net'
        self.ids_node = self._net.add_nodes(
            ids_sumo=self.ids_sumo,
            types=self.types,
            coords=self.coords,
            types_tl=self.types_tl,
            turnradii=self.turnradii,
            are_keep_clear=self.are_keep_clear,
        )
        # attention:
        # attributes ids_sumo_tls and ids_sumo_controlled will be added later
        # when tls and edges are read
        # see write_to_net_post

    def write_to_net_post(self):
        """
        To be called after edges and tls are read.
        """
        # print 'write_to_net_post'
        get_ids_edge = self._net.edges.ids_sumo.get_ids_from_indices
        ids_controlled = self._net.nodes.ids_controlled
        for id_node, ids_sumo_edge in zip(self.ids_node, self.ids_sumo_controlled):
            if ids_sumo_edge is not None:
                if len(ids_sumo_edge) == 0:
                    ids_controlled[id_node] = []
                else:
                    ids_controlled[id_node] = get_ids_edge(ids_sumo_edge)

        # convert sumo ids into internal ids and set to nodes
        # print '  self.ids_sumo_tls',self.ids_sumo_tls
        # print '  self._net.tlss.ids_sumo',self._net.tlss.ids_sumo.value
        self._net.nodes.ids_tls[
            self.ids_node] = self._net.tlss.ids_sumo.get_ids_from_indices_save(self.ids_sumo_tls)

    def startElement(self, name, attrs):
        # print 'startElement',name
        # if attrs.has_key('id'): print attrs['id']
        # elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        # elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        # else: print '.'

        if name == 'nodes':
            version = self._net.get_version()
            if self._isNew | (version == attrs['version']):
                self._net.set_version(attrs['version'])
            else:
                print 'WARNING: merge with incompatible net versions %s versus %s.' % (version, attrs['version'])

        elif name == 'location':  # j.s
            # print 'startElement',name,self._isNew
            netOffsetStrings = attrs['netOffset'].strip().split(",")
            offset = np.array([float(netOffsetStrings[0]),
                               float(netOffsetStrings[1])])
            offset_prev = self._net.get_offset()
            if self._isNew:
                self._net.set_offset(offset)
                # print '  offset_prev,offset',offset_prev,offset,type(offset)
            else:

                self._offset_delta = offset - offset_prev
                self._net.set_offset(offset)
                # print '
                # offset_prev,offset,self._offset_delta',offset_prev,offset,type(offset),self._offset_delta

            convBoundaryStr = attrs['convBoundary'].strip().split(",")
            origBoundaryStr = attrs['origBoundary'].strip().split(",")
            # print '  convBoundaryStr',convBoundaryStr
            # print '  origBoundary',origBoundaryStr

            if self._isNew:
                self._net.set_boundaries([float(convBoundaryStr[0]),
                                          float(convBoundaryStr[1]),
                                          float(convBoundaryStr[2]),
                                          float(convBoundaryStr[3])],
                                         [float(origBoundaryStr[0]),
                                          float(origBoundaryStr[1]),
                                          float(origBoundaryStr[2]),
                                          float(origBoundaryStr[3])]
                                         )
            else:
                self._net.merge_boundaries([float(convBoundaryStr[0]),
                                            float(convBoundaryStr[1]),
                                            float(convBoundaryStr[2]),
                                            float(convBoundaryStr[3])],
                                           [float(origBoundaryStr[0]),
                                            float(origBoundaryStr[1]),
                                            float(origBoundaryStr[2]),
                                            float(origBoundaryStr[3])]
                                           )
            if self._isNew:
                if attrs.has_key('projParameter'):
                    self._net.set_projparams(attrs['projParameter'])
            else:
                if attrs.has_key('projParameter'):
                    if self._net.get_projparams() != attrs['projParameter']:
                        print 'WARNING: merge with incompatible projections %s versus %s.' % (self._net.getprojparams(), attrs['projparams'])

        elif name == 'node':
            if attrs['id'][0] != ':':  # no internal node
                self._ind_node += 1
                i = self._ind_node
                x0, y0 = self._offset_delta

                self.ids_sumo[i] = attrs['id']
                sumotypes_node = str(attrs.get('type', 'priority'))

                self.types[i] = self._nodetypemap[sumotypes_node]
                self.coords[i] = [
                    float(attrs['x']) - x0, float(attrs['y']) - y0, float(attrs.get('z', 0.0))]

                sumotype_tl = attrs.get('tlType', 'none')
                if sumotypes_node == 'traffic_light':
                    if sumotype_tl == 'none':
                        sumotype_tl = 'static'

                self.types_tl[i] = self._tltypemap[sumotype_tl]
                self.ids_sumo_tls[i] = attrs.get('tl', None)
                self.turnradii[i] = attrs.get('radius', 1.5)
                self.are_keep_clear[i] = attrs.get('keepClear', True)

                #'controlledInner'
                # Edges which shall be controlled by a joined TLS
                # despite being incoming as well as outgoing to
                # the jointly controlled nodes
                # problem: we do not know yet the edge IDs
                #
                if attrs.has_key('controlledInner'):
                    self.ids_sumo_controlled[i] = attrs[
                        'controlledInner'].strip().split(' ')
                else:
                    self.ids_sumo_controlled[i] = []


# class SumoTllCounter(handler.ContentHandler):
#    """Parses a SUMO tll XML file and counts edges and lanes."""
#
#    def __init__(self):
#        self.n_tls = 0
#
#
#    def startElement(self, name, attrs):
#        #print 'startElement',name,self.n_tls
#        if name == 'tlLogic':
#            self.n_tls += 1

class SumoTllReader(handler.ContentHandler):

    """Parses a SUMO tll XML file and reads it into net."""

    def __init__(self, net):
        self.net = net
        self.connections = net.connections
        self.tlss = net.tlss
        # print 'SumoEdgeFastreader'

        self.get_id_tls = net.nodes.ids_sumo.get_id_from_index

        #n_tls = counter.n_tls
        self.ptypes_choices = self.tlss.tlls.value.ptypes.choices
        self.ids_sumo_tls = self.tlss.ids_sumo

        self.reset_prog()

        self.tlsconnections = {}

    def reset_prog(self):
        self.id_sumo_tls = None
        self.durations = []
        self.durations_min = []
        self.durations_max = []
        self.states = []

    def startElement(self, name, attrs):

        if name == 'tlLogic':
            # print '\n startElement',name,attrs['id']
            self.id_sumo_tls = attrs['id']
            self.ptype = self.ptypes_choices.get(attrs.get('type', None), 1)
            self.id_prog = attrs.get('programID', None)
            self.offset = attrs.get('offset', None)

        elif name == 'phase':
            # print 'startElement',name,self.id_sumo_tls
            if self.id_sumo_tls is not None:
                # print '
                # append',attrs.get('duration',None),attrs.get('state',None),len(attrs.get('state',''))
                duration = int(attrs.get('duration', 0))
                self.durations.append(duration)
                self.durations_min.append(int(attrs.get('minDur', duration)))
                self.durations_max.append(int(attrs.get('maxDur', duration)))
                self.states.append(attrs.get('state', None))

        # elif name == 'tlLogics':
        #    pass

        elif name == 'connection':
            id_sumo_tls = attrs['tl']
            # print 'startElement',name,id_sumo_tls,int(attrs['linkIndex'])
            # print '  self.tlsconnections',self.tlsconnections

            if not self.tlsconnections.has_key(id_sumo_tls):
                self.tlsconnections[id_sumo_tls] = {}

            id_con = self.connections.get_id_from_sumoinfo(attrs['from'],
                                                           attrs['to'], int(attrs['fromLane']), int(attrs['toLane']))
            if id_con >= 0:
                self.tlsconnections[id_sumo_tls][
                    int(attrs['linkIndex'])] = id_con

    def endElement(self, name):
        #edges = self._net.edges
        #lanes = self._net.lanes
        if name == 'tlLogic':
            # print 'endElement',name,self.id_sumo_tls
            # print '  ptype',self.ptype
            # print '  durations',self.durations
            # print '  durations_min',self.durations_min
            # print '  durations_max',self.durations_max
            # print '  states',self.states
            # print '  self.id_prog='+self.id_prog+'='
            self.tlss.make(self.id_sumo_tls,
                           id_prog=self.id_prog,
                           ptype=self.ptype,
                           offset=self.offset,
                           durations=self.durations,
                           durations_min=self.durations_min,
                           durations_max=self.durations_max,
                           states=self.states,
                           )

            self.reset_prog()

        elif name == 'tlLogics':
            # print 'endElement',name,len(self.tlss)
            # end of scanning. Write controlled connections to tlss
            # print '  tlsconnections',self.tlsconnections

            for id_sumo_tls, conmap in self.tlsconnections.iteritems():

                inds_con = np.array(conmap.keys(), dtype=np.int32)

                ids_con = np.zeros(np.max(inds_con) + 1, np.int32)
                # print '  cons for',id_sumo_tls,conmap
                # print '  inds',inds_con,len(ids_con)
                # print '  values',conmap.values(),len(ids_con)
                ids_con[inds_con] = conmap.values()  # <<<<<<<<<<<

                id_tls = self.tlss.ids_sumo.get_id_from_index(id_sumo_tls)
                self.tlss.set_connections(id_tls, ids_con)
                #self.tlss.set_connections(self.get_id_tls(id_sumo_tls), ids_con)


class SumoEdgeCounter(handler.ContentHandler):

    """Parses a SUMO edge XML file and counts edges and lanes."""

    def __init__(self):
        self.n_edge = 0
        self.n_lane = 0
        self.n_roundabout = 0
        self._n_edgelane = 0
        #self._net = net
        #self._ids_edge_sumo = net.edges.ids_sumo
        #self._ids_node_sumo = net.nodes.ids_sumo

    def startElement(self, name, attrs):
        # print 'startElement',name,self.n_edge,self.n_lane,self.n_roundabout
        if name == 'edge':
            self.n_edge += 1
            self.n_lane += int(attrs['numLanes'])

        elif name == 'roundabout':
            self.n_roundabout += 1


class SumoEdgeReader(handler.ContentHandler):

    """Parses a SUMO edge XML file and reads it into net."""

    def __init__(self, net, counter, offset_delta=np.array([0.0, 0.0])):
        self._net = net

        # print 'SumoEdgeFastreader'

        self._ids_node_sumo = net.nodes.ids_sumo
        self._nodecoords = net.nodes.coords
        self._modenames = net.modes.names
        self._offset_delta = offset_delta
        #self._isNew = len(self._net.nodes)==0

        # edge attrs
        self._ind_edge = -1
        # print '  n_edge',counter.n_edge
        self.ids_edge_sumo = np.zeros(
            counter.n_edge, np.object)  # net.edges.ids_sumo
        self.ids_edge_sumo[:] = None  # ??needed

        self.ids_fromnode = np.zeros(counter.n_edge, np.int32)
        self.ids_tonode = np.zeros(counter.n_edge, np.int32)
        self.types_edge = np.zeros(counter.n_edge, np.object)
        # used only for lane width if no lane data is given
        self.widths = np.zeros(counter.n_edge, np.float32)
        self.nums_lanes = np.zeros(counter.n_edge, np.int32)
        self.speeds_max = np.zeros(counter.n_edge, np.float32)
        self.priorities = np.zeros(counter.n_edge, np.int32)
        #length = 0.0,
        self.shapes = np.zeros(counter.n_edge, np.object)
        self.types_spread = np.zeros(counter.n_edge, np.int32)
        self.spread_choices = net.edges.types_spread.choices
        #"right": 0,
        #"center": 1,
        self.names = np.zeros(counter.n_edge, np.object)
        self.offsets_end = np.zeros(counter.n_edge, np.float32)
        self.widths_lanes_default = np.zeros(counter.n_edge, np.float32)
        self.widths_sidewalk = np.zeros(counter.n_edge, np.float32)
        self.inds_lanes_edges = np.zeros(counter.n_edge, np.object)
        #self.inds_lanes_edges[:] = None
        self._ind_lanes_edges = []

        #self.ids_sumoedge_to_ind = {}

        # lane attrs
        # print '  n_lane',counter.n_lane
        self._ind_lane = -1
        self.index_lanes = np.zeros(counter.n_lane, np.int32)
        self.width_lanes = np.zeros(counter.n_lane, np.float32)
        self.speed_max_lanes = np.zeros(counter.n_lane, np.float32)
        self.offset_end_lanes = np.zeros(counter.n_lane, np.float32)
        self.modes_allow = np.zeros(counter.n_lane, np.object)
        self.modes_disallow = np.zeros(counter.n_lane, np.object)
        self.ids_mode_lanes = np.zeros(counter.n_lane, np.int32)
        self.inds_edge_lanes = np.zeros(counter.n_lane, np.int32)
        #self.shapes_lanes = np.zeros(counter.n_lane,np.object)

        # roundabout attrs
        # print '  n_roundabout',counter.n_roundabout
        self._ind_ra = -1
        self.ids_sumoedges_ra = np.zeros(counter.n_roundabout, np.object)
        self.ids_nodes_ra = np.zeros(counter.n_roundabout, np.object)

        ############################

    def startElement(self, name, attrs):
        # print 'startElement',name
        # if attrs.has_key('id'): print attrs['id']
        # elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        # elif (attrs.has_key('from')&attrs.has_key('to')): print 'from',attrs['from'],'to',attrs['to']
        # else: print '.'

        if name == 'edge':
            # if not attrs.has_key('function') or attrs['function'] != 'internal':
            #id_fromnode = nodes.ids_sumo.get_id_from_index(id_fromnode_sumo)
            #id_tonode = nodes.ids_sumo.get_id_from_index(id_tonode_sumo)
            self._ind_edge += 1
            ind = self._ind_edge
            # print 'startElement edge',ind,attrs['id']
            self.ids_edge_sumo[ind] = attrs['id']

            id_fromnode = self._ids_node_sumo.get_id_from_index(
                str(attrs['from']))
            id_tonode = self._ids_node_sumo.get_id_from_index(str(attrs['to']))
            self.ids_fromnode[ind] = id_fromnode
            self.ids_tonode[ind] = id_tonode

            self.types_edge[ind] = str(attrs.get('type', ''))
            self.nums_lanes[ind] = int(attrs.get('numLanes', 1))
            self.widths[ind] = float(
                attrs.get('width', 3.5 * self.nums_lanes[ind]))
            self.types_spread[ind] = self.spread_choices[
                str(attrs.get('spreadType', 'right'))]  # usually center
            # print '  ',self.types_spread[ind]

            #length = 0.0,
            shape = np.array(xm.process_shape(
                attrs.get('shape', ''), offset=self._offset_delta))

            if len(shape) < 2:  # insufficient shape data
                # shape should be a list of np array coords
                # ATTENTIOn: we need to copy here, otherwise the reference
                # to node coordinates will be kept!!
                shape = np.array(
                    [1.0 * self._nodecoords[id_fromnode], 1.0 * self._nodecoords[id_tonode]])

                if self.types_spread[ind] == 1:  # center
                    angles_perb = get_angles_perpendicular(shape)
                    halfwidth = self.widths[ind]
                    shape[:, 0] += np.cos(angles_perb) * halfwidth
                    shape[:, 1] += np.sin(angles_perb) * halfwidth

            self.shapes[ind] = shape

            self.speeds_max[ind] = float(attrs.get('speed', 13.888))
            self.priorities[ind] = int(attrs.get('priority', 9))
            self.names[ind] = unicode(attrs.get('name', ''))
            self.offsets_end[ind] = float(attrs.get('endOffset', 0.0))
            self.widths_lanes_default[ind] = float(attrs.get('width ', 3.0))
            self.widths_sidewalk[ind] = float(attrs.get('sidewalkWidth', -1.0))

            #self._is_laneshape = True
            # print '  self._id_edge',self._id_edge

        elif name == 'lane':
            self._ind_lane += 1
            ind = self._ind_lane
            speed_max_default = -1
            if attrs.has_key('allow'):
                modes_allow = list(self._modenames.get_ids_from_indices(
                    attrs['allow'].split(' ')))
            else:
                edgetype = self.types_edge[self._ind_edge]

                if OSMEDGETYPE_TO_MODES.has_key(edgetype):
                    modes_allow, speed_max_default = OSMEDGETYPE_TO_MODES[
                        edgetype]
                else:
                    modes_allow = []

            if attrs.has_key('disallow'):
                modes_disallow = list(self._modenames.get_ids_from_indices(
                    attrs['disallow'].split(' ')))
            else:
                modes_disallow = []

            index = int(attrs.get('index', -1))
            width = float(attrs.get('width', -1))
            speed_max = float(attrs.get('speed', speed_max_default))

            is_sidewalk_edge = False
            is_sidewalk = False

            if len(modes_allow) == 1:
                id_mode_main = modes_allow[0]  # pick  as major mode
                # elif len(modes_allow) == 1:

            else:
                id_mode_main = -1  # no major mode specified

            if index == 0:
                width_sidewalk_edge = self.widths_sidewalk[
                    self._ind_edge]  # copy from edge
                is_sidewalk_edge = width_sidewalk_edge > 0
                # test for pedestrian sidewalk
                is_sidewalk = (MODES['pedestrian'] in modes_allow)

            if speed_max < 0:
                if (index == 0) & is_sidewalk:
                    speed_max = 0.8  # default walk speed
                else:
                    speed_max = self.speeds_max[
                        self._ind_edge]  # copy from edge

            # print ' is_sidewalk_edge ,is_sidewalk',is_sidewalk_edge
            # ,is_sidewalk
            if width < 0:
                width = self.widths_lanes_default[
                    self._ind_edge]  # copy from edge

                if index == 0:
                    if is_sidewalk_edge:  # edge wants sidewalks
                        width = width_sidewalk_edge
                    # edge does not want sidewalks, but actually there is a
                    # sidewalk
                    elif (not is_sidewalk_edge) & is_sidewalk:
                        width = 0.9  # default sidewalk width
                        # update edge attr!!
                        self.widths_sidewalk[self._ind_edge] = width

            # if sidewalk, then the edge attribute widths_sidewalk
            # should be set to actual lane width in case it is less than zero
            elif index == 0:  # width set for lane 0
                # edge does not want sidewalks, but actually there is a
                # sidewalk
                if (not is_sidewalk_edge) & is_sidewalk:
                    # update edge attr!!
                    self.widths_sidewalk[self._ind_edge] = width

            self.index_lanes[ind] = index
            self.width_lanes[ind] = width
            self.speed_max_lanes[ind] = speed_max
            self.offset_end_lanes[ind] = float(attrs.get('endOffset', 0.0))
            self.modes_allow[ind] = modes_allow
            self.modes_disallow[ind] = modes_disallow
            self.ids_mode_lanes[ind] = id_mode_main
            self.inds_edge_lanes[ind] = self._ind_edge
            #self.shapes_lanes[ind]  = self.getShape(attrs.get('shape',''), offset = self._offset_delta)

            self._ind_lanes_edges.append(ind)
            # self._ids_lane.append(id_lane)

        elif name == 'roundabout':
            self._ind_ra += 1
            self.ids_sumoedges_ra[self._ind_ra] = attrs.get(
                'edges', '').split(' ')
            self.ids_nodes_ra[self._ind_ra] = self._ids_node_sumo.get_ids_from_indices(
                attrs.get('nodes', '').split(' '))

    # def characters(self, content):
    #    if self._currentLane!=None:
    #        self._currentShape = self._currentShape + content

    def endElement(self, name):
        #edges = self._net.edges
        #lanes = self._net.lanes
        if name == 'edge':
            n_lane = self.nums_lanes[self._ind_edge]
            # print 'SumoEdgeReader.endElement',self._ind_lane,n_lane
            while len(self._ind_lanes_edges) < n_lane:
                # if len(self._ind_lanes_edges) ==0:
                # edge description provided no specific lane information
                # create n_lanes and us some properties from current edge
                self._ind_lane += 1
                ind = self._ind_lane

                edgetype = self.types_edge[self._ind_edge]

                if OSMEDGETYPE_TO_MODES.has_key(edgetype):
                    modes_allow, speed_max_default = OSMEDGETYPE_TO_MODES[
                        edgetype]
                else:
                    modes_allow = []
                if len(modes_allow) == 1:
                    id_mode_main = modes_allow[0]  # pick  as major mode
                else:
                    id_mode_main = -1  # no major mode specified

                self.index_lanes[ind] = 0
                self.width_lanes[ind] = self.widths[
                    self._ind_edge]  # copy from edge attr
                self.speed_max_lanes[ind] = self.speeds_max[
                    self._ind_edge]  # copy from edge attr
                self.offset_end_lanes[ind] = self.offset_end_lanes[
                    self._ind_edge]  # copy from edge attr

                self.modes_allow[ind] = modes_allow
                self.modes_disallow[ind] = []
                self.inds_edge_lanes[ind] = self._ind_edge
                #self.shapes_lanes[ind]  = self.getShape(attrs.get('shape',''), offset = self._offset_delta)
                self.ids_mode_lanes[ind] = id_mode_main
                self._ind_lanes_edges.append(ind)

            self.inds_lanes_edges[self._ind_edge] = self._ind_lanes_edges
            self._ind_lanes_edges = []

    def write_to_net(self):

        # print 'write_to_net'
        ids_edge = self._net.add_edges(
            ids_sumo=self.ids_edge_sumo,
            ids_fromnode=self.ids_fromnode,
            ids_tonode=self.ids_tonode,
            types=self.types_edge,
            nums_lanes=self.nums_lanes,
            speeds_max=self.speeds_max,
            priorities=self.priorities,
            #lengths = length,
            shapes=self.shapes,
            types_spread=self.types_spread,
            names=self.names,
            offsets_end=self.offsets_end,
            widths_lanes_default=self.widths_lanes_default,
            widths_sidewalk=self.widths_sidewalk,
        )

        # print '  self.inds_edge_lanes',self.inds_edge_lanes
        ids_lanes = self._net.add_lanes(
            indexes=self.index_lanes,
            widths=self.width_lanes,
            speeds_max=self.speed_max_lanes,
            offsets_end=self.offset_end_lanes,
            modes_allow=self.modes_allow,
            modes_disallow=self.modes_disallow,
            # main mode will be determined from other attributes
            ids_mode=self.ids_mode_lanes,
            ids_edge=ids_edge[self.inds_edge_lanes],
            # shapes = self.shapes_lanes, # lane shapes are not given -> must
            # be derived from edge shape
        )
        #edges.update_lanes(self._id_edge, self._ids_lane)
        ids_edgelanes = self._net.edges.ids_lanes
        ind = 0
        for inds_lane in self.inds_lanes_edges:
            ids_edgelanes[ids_edge[ind]] = ids_lanes[inds_lane]
            # print '
            # id_edge,ids_lanes[inds_lane]',ids_edge[ind],ids_lanes[inds_lane]
            ind += 1

        # roundaboutS
        ids_edge_sumo = self._net.edges.ids_sumo
        ids_roundabout = self._net.add_roundabouts(
            ids_nodes=self.ids_nodes_ra,
        )
        ids_edges_ra = self._net.roundabouts.ids_edges
        i = 0
        for id_roundabout in ids_roundabout:
            ids_edges_ra[id_roundabout] = ids_edge_sumo.get_ids_from_indices(
                self.ids_sumoedges_ra[i])
            i += 1


class SumonetImporter(CmlMixin, Process):

    def __init__(self, net, rootname=None, rootdirpath=None, netfilepath=None,
                 is_clean_nodes=False, logger=None, **kwargs):

        self._init_common('sumonetimporter', name='SUMO net import',
                          logger=logger,
                          info='Converts a SUMO .net.xml file to nod.xml, edg.xml and con.xml file and reads into scenario.',
                          )
        self._net = net

        self.init_cml('netconvert')

        if rootname == None:
            rootname = net.parent.get_rootfilename()

        if rootdirpath == None:
            if net.parent != None:
                rootdirpath = net.parent.get_workdirpath()
            else:
                rootdirpath = os.getcwd()

        if netfilepath == None:
            netfilepath = os.path.join(rootdirpath, rootname + '.net.xml')

        attrsman = self.get_attrsman()
        self.add_option('netfilepath', netfilepath,
                        # this will make it show up in the dialog
                        groupnames=['options'],
                        cml='--sumo-net-file',
                        perm='rw',
                        name='Net file',
                        wildcards='Net XML files (*.net.xml)|*.net.xml',
                        metatype='filepath',
                        info='SUMO Net file in XML format.',
                        )

        self.workdirpath = attrsman.add(cm.AttrConf('workdirpath', rootdirpath,
                                                    # ['options'],#['_private'],
                                                    groupnames=['_private'],
                                                    perm='r',
                                                    name='Workdir',
                                                    metatype='dirpath',
                                                    info='Working directory for this scenario.',
                                                    ))

        self.rootname = attrsman.add(cm.AttrConf('rootname', rootname,
                                                 groupnames=['_private'],
                                                 perm='r',
                                                 name='Scenario shortname',
                                                 info='Scenario shortname is also rootname of converted files.',
                                                 ))

        self.is_clean_nodes = attrsman.add(cm.AttrConf('is_clean_nodes', is_clean_nodes,
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Clean Nodes',
                                                       info='If set, then shapes around nodes are cleaned up.',
                                                       ))

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
        cml = self.get_cml() + ' --plain-output-prefix ' + \
            filepathlist_to_filepathstring(
                os.path.join(self.workdirpath, self.rootname))
        # print 'SumonetImporter.do',cml
        #import_xml(self, rootname, dirname, is_clean_nodes = True)
        self.run_cml(cml)
        if self.status == 'success':
            self._net.import_xml(
                self.rootname, self.workdirpath, is_clean_nodes=self.is_clean_nodes)

        # print 'do',self.newident
        # self._scenario = Scenario(  self.newident,
        #                                parent = None,
        #                                workdirpath = self.workdirpath,
        #                                logger = self.get_logger(),
        #                                )

    def get_net(self):
        return self._net


class OsmImporter(CmlMixin, Process):

    def __init__(self, net=None,
                 osmfilepaths=None,
                 netfilepath=None,
                 proj='',
                 is_import_elevation_osm=False,
                 typefilepath=None,
                 # ordinary roads+bikeways+footpath
                 roadtypes='ordinary roads+bikeways',
                 n_lanes_default=0,
                 edgespeed_default=13.9,
                 priority_default=-1,
                 is_remove_isolated_edges=True,
                 factor_edge_speed=1.0,
                 is_guess_sidewalks=False,
                 edgespeed_min_sidewalks=5.8,
                 edgespeed_max_sidewalks=13.89,
                 is_guess_sidewalks_from_permission=False,
                 width_sidewalks_default=2.0,
                 is_guess_crossings=False,
                 edgespeed_max_crossings=13.89,
                 is_join_nodes=True,
                 dist_join_nodes=15.0,
                 is_keep_nodes_clear=True,
                 is_keep_nodes_unregulated=False,
                 is_guess_tls=False,
                 is_join_tls=False,
                 joindist_tls=20.0,
                 is_uncontrolled_within_tls=False,
                 is_guess_signals_tls=False,
                 dist_guess_signal_tls=20.0,
                 #time_green_tls = 31,
                 time_yellow_tls=-1,
                 accel_min_yellow_tls=-1.0,
                 is_no_turnarounds=False,
                 is_no_turnarounds_tls=False,
                 is_check_lane_foes=False,
                 is_roundabouts_guess=True,
                 is_check_lane_foes_roundabout=False,
                 is_no_left_connections=False,
                 is_geometry_split=False,
                 is_geometry_remove=True,
                 length_max_segment=-1.0,
                 dist_min_geometry=-1.0,
                 is_guess_ramps=True,
                 rampspeed_max=-1,
                 highwayspeed_min=21.9444,
                 ramplength=100,
                 is_no_split_ramps=False,
                 #
                 is_clean_nodes=False,
                 #
                 logger=None, **kwargs):

        self._init_common('osmimporter', name='OSM import',
                          logger=logger,
                          info='Converts a OSM  file to SUMO nod.xml, edg.xml and con.xml file and reads into scenario.',
                          )
        if net == None:
            self._net = Network()
        else:
            self._net = net

        self.init_cml('netconvert')  # pass main shell command

        if net.parent != None:
            rootname = net.parent.get_rootfilename()
            rootdirpath = net.parent.get_workdirpath()
        else:
            rootname = net.get_ident()
            rootdirpath = os.getcwd()

        if netfilepath == None:
            netfilepath = os.path.join(rootdirpath, rootname + '.net.xml')

        if osmfilepaths == None:
            osmfilepaths = os.path.join(rootdirpath, rootname + '.osm.xml')

        if typefilepath == None:
            typefilepath = os.path.join(os.path.dirname(os.path.abspath(
                __file__)), '..', '..', 'typemap', 'osmNetconvert.typ.xml')

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

        self.add_option('netfilepath', netfilepath,
                        groupnames=[],  # ['_private'],#
                        cml='--output-file',
                        perm='r',
                        name='Net file',
                        wildcards='Net XML files (*.net.xml)|*.net.xml',
                        metatype='filepath',
                        info='SUMO Net file in XML format.',
                        )

        self.add_option('typefilepath', typefilepath,
                        groupnames=['options'],
                        cml='--type-files',
                        perm='rw',
                        name='Type files',
                        wildcards='Typemap XML files (*.typ.xml)|*.typ.xml',
                        metatype='filepaths',
                        info="""Typemap XML files. In these file, 
OSM road types are mapped to edge and lane parameters such as width, 
speeds, etc. These parameters are used as defaults in absents of explicit OSM attributes.
Use osmNetconvert.typ.xml as a base and additional type file to meet specific needs.""",
                        )

        # self.add_option('projparams','!',
        #                groupnames = ['options'],#
        #                cml = '--proj',
        #                perm='rw',
        #                name = 'projection',
        #                info = 'Uses STR as proj.4 definition for projection.',
        #                )
        # --offset.disable-normalization <BOOL> 	Turn off normalizing node positions; default: false
        # self.add_option('offset_x',0.0,
        #                groupnames = ['options','geometry'],#
        #                cml = '--offset.x ',
        #                perm='rw',
        #                unit = 'm',
        #                name = 'X-Offset',
        #                info = 'Adds offset to net x-positions; default: 0.0',
        #                )
        # self.add_option('offset_y',0.0,
        #                groupnames = ['options','geometry'],#
        #                cml = '--offset.y ',
        #                perm='rw',
        #                unit = 'm',
        #                name = 'Y-Offset',
        #                info = 'Adds offset to net x-positions; default: 0.0',
        #                )
        # --flip-y-axis <BOOL> 	Flips the y-coordinate along zero; default: false
        #----------------------------------------------------------------------
        # osm specific
        #--junctions.corner-detail <INT> 	Generate INT intermediate points to smooth out intersection corners; default: 0
        # --junctions.internal-link-detail <INT> 	Generate INT intermediate points to smooth out lanes within the intersection; default: 5
        self.add_option('is_import_elevation_osm', is_import_elevation_osm,
                        groupnames=['options', 'osm'],
                        cml='--osm.elevation',
                        perm='rw',
                        name='import elevation',
                        info='Imports elevation data.',
                        )
#-------------------------------------------------------------------------
        # edge options
        #--keep-edges.min-speed <FLOAT> 	Only keep edges with speed in meters/second > FLOAT
        modesset = set(MODES.keys())
        modesset_pt_rail = set(["rail_urban", "rail", "rail_electric"])
        modesset_pt_road = set(["bus", "taxi", "coach", "tram"])
        modesset_motorized = set(["private", "passenger", "emergency", "authority",
                                  "army", "vip", "hov", "motorcycle", "moped", "evehicle", "delivery", "truck"])

        roadtypes_to_disallowed_vtypes = {"roads for individual transport": ','.join(modesset.difference(["passenger", ])),
                                          "ordinary roads": ','.join(modesset.difference(modesset_pt_road | modesset_motorized)),
                                          "ordinary roads+bikeways": ','.join(modesset.difference(modesset_pt_road | modesset_motorized | set(['bicycle']))),
                                          "ordinary roads+bikeways+footpath": ','.join(modesset.difference(modesset_pt_road | modesset_motorized | set(['bicycle']) | set(['pedestrian']))),
                                          "ordinary roads+rails": ','.join(modesset.difference(modesset_pt_road | modesset_motorized | modesset_pt_rail)),
                                          "ordinary roads+rails+bikeways": ','.join(modesset.difference(modesset_pt_road | modesset_motorized | modesset_pt_rail | set(['bicycle']))),
                                          "ordinary roads+rails+bikeways+footpath": ','.join(modesset.difference(modesset_pt_road | modesset_motorized | modesset_pt_rail | set(['bicycle']) | set(['pedestrian']))),
                                          "all ways": ""
                                          }
        # print '  access_to_vtypes=',access_to_vtypes
        self.add_option('select_edges_by_access', roadtypes_to_disallowed_vtypes[roadtypes],
                        groupnames=['options', 'edges'],
                        cml='--remove-edges.by-vclass',
                        choices=roadtypes_to_disallowed_vtypes,
                        perm='rw',
                        name='Keep edge with acces',
                        info='Imports all edges with the given vehicle access patterns.',
                        is_enabled=lambda self: self.select_edges_by_access != "",
                        )

        self.add_option('n_lanes_default', n_lanes_default,
                        groupnames=['options', 'edges'],
                        cml='--default.lanenumber',
                        perm='rw',
                        name='Default lanenumber',
                        info='The default number of lanes in an edge.',
                        is_enabled=lambda self: self.n_lanes_default > 0,
                        )

        self.add_option('edgespeed_default', edgespeed_default,
                        groupnames=['options', 'edges'],
                        cml='--default.speed',
                        perm='rw',
                        unit='m/s',
                        name='Default edge speed',
                        info='The default speed on an edge.',
                        is_enabled=lambda self: self.edgespeed_default > 0,
                        )

        choices_priority = {}
        for i in range(11):
            choices_priority[str(i)] = i
        choices_priority['auto'] = -1

        self.add_option('priority_default', priority_default,
                        groupnames=['options', 'edges'],
                        cml='--default.priority',
                        choices=choices_priority,
                        perm='rw',
                        name='Default priority',
                        info='The default priority of an edge. Value of-1 means automatic assignment.',
                        is_enabled=lambda self: self.priority_default > 0,
                        )

        self.add_option('is_remove_isolated_edges', is_remove_isolated_edges,
                        groupnames=['options', 'edges'],
                        cml='--remove-edges.isolated',
                        perm='rw',
                        name='Remove isolated edges',
                        info='Remove isolated edges.',
                        )
        # --edges.join <BOOL> 	Merges edges whch connect the same nodes and are close to each other (recommended for VISSIM import); default: false
        # --speed.offset <FLOAT> 	Modifies all edge speeds by adding FLOAT; default: 0

        self.add_option('factor_edge_speed', factor_edge_speed,
                        groupnames=['options', 'edges'],
                        cml='--speed.factor',
                        perm='rw',
                        name='Edge speed factor',
                        info='Modifies all edge speeds by multiplying with edge speed factor.',
                        is_enabled=lambda self: self.factor_edge_speed == 1.0,
                        )

#-------------------------------------------------------------------------
        # pedestrians
        self.add_option('is_guess_sidewalks', is_guess_sidewalks,
                        groupnames=['options', 'pedestrians'],
                        cml='--sidewalks.guess',
                        perm='rw',
                        name='Guess sidewalks',
                        info='Guess pedestrian sidewalks based on edge speed.',
                        )

        self.add_option('edgespeed_min_sidewalks', edgespeed_min_sidewalks,
                        groupnames=['options', 'pedestrians'],
                        cml='--sidewalks.guess.min-speed',
                        perm='rw',
                        unit='m/s',
                        name='Min edge speed for sidewalk guess',
                        info='Add sidewalks for edges with a speed above the given limit.',
                        is_enabled=lambda self: self.is_guess_sidewalks | self.is_guess_sidewalks_from_permission,
                        )

        self.add_option('edgespeed_max_sidewalks', edgespeed_max_sidewalks,
                        groupnames=['options', 'pedestrians'],
                        cml='--sidewalks.guess.max-speed',
                        perm='rw',
                        unit='m/s',
                        name='Max edge speed for sidewalk guess',
                        info='Add sidewalks for edges with a speed equal or below the given limit.',
                        is_enabled=lambda self: self.is_guess_sidewalks | self.is_guess_sidewalks_from_permission,
                        )
        self.add_option('is_guess_sidewalks_from_permission', is_guess_sidewalks_from_permission,
                        groupnames=['options', 'pedestrians'],
                        cml='--sidewalks.guess.from-permissions ',
                        perm='rw',
                        name='Guess sidewalks from permission',
                        info='Add sidewalks for edges that allow pedestrians on any of their lanes regardless of speed.',
                        )

        self.add_option('width_sidewalks_default', width_sidewalks_default,
                        groupnames=['options', 'pedestrians'],
                        cml='--default.sidewalk-width',
                        perm='rw',
                        unit='m',
                        name='Min edge speed for sidewalk guess',
                        info='Add sidewalks for edges with a speed above the given limit.',
                        is_enabled=lambda self: self.is_guess_sidewalks | self.is_guess_sidewalks_from_permission,
                        )

        self.add_option('is_guess_crossings', is_guess_crossings,
                        groupnames=['options', 'pedestrians'],
                        cml='--crossings.guess',
                        perm='rw',
                        name='Guess crossings',
                        info='Guess pedestrian crossings based on the presence of sidewalks.',
                        )

        self.add_option('edgespeed_max_crossings', edgespeed_max_crossings,
                        groupnames=['options', 'pedestrians'],
                        cml='--crossings.guess.speed-threshold',
                        perm='rw',
                        unit='m/s',
                        name='Max edge speed for crossings',
                        info='At uncontrolled nodes, do not build crossings across edges with a speed above this maximum edge speed.',
                        is_enabled=lambda self: self.is_guess_crossings,
                        )

#-------------------------------------------------------------------------
        # node options

        self.add_option('is_join_nodes', is_join_nodes,
                        groupnames=['options', 'nodes'],
                        cml='--junctions.join',
                        perm='rw',
                        name='Join nodes',
                        info='Join nearby nodes. Specify with node join distance which nodes will be joined.',
                        )

        self.add_option('dist_join_nodes', dist_join_nodes,
                        groupnames=['options', 'nodes'],
                        cml='--junctions.join-dist',
                        perm='rw',
                        unit='m',
                        name='Node join distance',
                        info='Specify with node join distance which nodes will be joined. Join nodes option must be True.',
                        is_enabled=lambda self: self.is_join_nodes,
                        )

        self.add_option('is_keep_nodes_clear', is_keep_nodes_clear,
                        groupnames=['options', 'nodes'],
                        cml='--default.junctions.keep-clear',
                        perm='rw',
                        name='Keep nodes clear',
                        info='Whether junctions should be kept clear by default.',
                        )

        self.add_option('is_keep_nodes_unregulated', is_keep_nodes_unregulated,
                        groupnames=['options', 'nodes'],
                        cml='--keep-nodes-unregulated',
                        perm='rw',
                        name='Keep nodes unregulated',
                        info='Keep nodes unregulated.',
                        )
       # --default.junctions.radius <FLOAT> 	The default turning radius of intersections; default: 1.5

#-------------------------------------------------------------------------

        # TLS Building Options:
        self.add_option('is_guess_tls', is_guess_tls,
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.guess',
                        perm='rw',
                        name='TLS-guessing',
                        info='Turns on TLS guessing.',
                        )

        # now same as is_join_tls
        # self.add_option('is_guess_join_tls',True,
        #                groupnames = ['options','traffic lights'],#
        #                cml = '--tls-guess.joining',
        #                perm='rw',
        #                name = 'TLS-guess joining',
        #                info = 'Includes node clusters into guess.',
        #                )
        self.add_option('is_join_tls', is_join_tls,
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.join',
                        perm='rw',
                        name='TLS-joining',
                        info='Tries to cluster tls-controlled nodes.',
                        is_enabled=lambda self: self.is_guess_tls,
                        )
        self.add_option('joindist_tls', joindist_tls,
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.join-dist',
                        perm='rw',
                        unit='m',
                        name='TLS-join dist.',
                        info='Determines the maximal distance for joining traffic lights (defaults to 20)',
                        is_enabled=lambda self: self.is_guess_tls & self.is_join_tls,
                        )
        self.add_option('is_uncontrolled_within_tls', is_uncontrolled_within_tls,
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.uncontrolled-within',
                        perm='rw',
                        name='Uncontrolled within TLS.',
                        info='Do not control edges that lie fully within a joined traffic light. This may cause collisions but allows old traffic light plans to be used.',
                        is_enabled=lambda self: self.is_guess_tls,
                        )

        self.add_option('is_guess_signals_tls', is_guess_signals_tls,
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.guess-signals',
                        perm='rw',
                        name='Guess signals.',
                        info='Interprets tls nodes surrounding an intersection as signal positions for a  larger TLS. This is typical pattern for OSM-derived networks',
                        is_enabled=lambda self: self.is_guess_tls,
                        )

        self.add_option('dist_guess_signal_tls', dist_guess_signal_tls,
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.guess-signals.dist',
                        perm='rw',
                        unit='m',
                        name='Signal guess dist.',
                        info='Distance for interpreting nodes as signal locations',
                        is_enabled=lambda self: self.is_guess_tls & self.is_guess_signals_tls,
                        )

        # self.add_option('time_cycle_tls',90,
        #                groupnames = ['options','traffic lights'],#
        #                cml = '--tls.cycle.time',
        #                perm='rw',
        #                unit = 's',
        #                name = 'Cycle time',
        #                info = 'Cycle time of traffic light.',
        #                )

        # self.add_option('time_green_tls',31,
        #                groupnames = ['options','traffic lights'],#
        #                cml = '--tls.green.time',
        #                perm='rw',
        #                unit = 's',
        #                name = 'Green cycle time',
        #                info = 'reen cycle time of traffic light.',
        #                )
        # self.add_option('time_leftgreen_tls',6,
        #                groupnames = ['options','traffic lights'],#
        #                cml = '--tls.left-green.time',
        #                perm='rw',
        #                unit = 's',
        #               name = 'Left green cycle time',
        #                info = 'reen cycle time of traffic light.',
        #                )

        self.add_option('time_yellow_tls', time_yellow_tls,  # default 6
                        groupnames=['options', 'traffic lights'],
                        cml='--tls.yellow.time',
                        perm='rw',
                        unit='s',
                        name='Yellow cycle time',
                        info='Fixed yellow time of traffic light. The value of -1.0 means automatic determination',
                        is_enabled=lambda self: self.is_guess_tls & (
                            self.time_yellow_tls > 0),
                        )

        self.add_option('accel_min_yellow_tls', accel_min_yellow_tls,  # default 3.0
                        groupnames=['options'],
                        cml='--tls.yellow.min-decel',
                        perm='rw',
                        unit='m/s^2',
                        name='Min decel. at yellow',
                        info='Defines smallest vehicle deceleration at yellow. The value of -1.0 means automatic determination',
                        is_enabled=lambda self: self.is_guess_tls & (
                            self.accel_min_yellow_tls > 0),
                        )

        # --tls.yellow.patch-small             Given yellow times are patched even ifbeing too short

#-------------------------------------------------------------------------
        # topology
        self.add_option('is_no_turnarounds', is_no_turnarounds,
                        groupnames=['options', 'topology'],
                        cml='--no-turnarounds',
                        perm='rw',
                        name='no turnarounds',
                        info='Disables building turnarounds.',
                        )
        self.add_option('is_no_turnarounds_tls', is_no_turnarounds_tls,
                        groupnames=['options', 'topology'],
                        cml='--no-turnarounds.tls',
                        perm='rw',
                        name='no TLS turnarounds',
                        info='Disables building turnarounds at traffic lights.',
                        is_enabled=lambda self: self.is_guess_tls,
                        )

        self.add_option('is_check_lane_foes', is_check_lane_foes,
                        groupnames=['options', 'topology'],
                        cml='--check-lane-foes.all',
                        perm='rw',
                        name='Always allow entering multilane',
                        info='Always allow driving onto a multi-lane road if there are foes (arriving vehicles) on other lanes.',
                        )

        self.add_option('is_roundabouts_guess', is_roundabouts_guess,
                        groupnames=['options', 'topology'],
                        cml='--roundabouts.guess',
                        perm='rw',
                        name='Roundabouts guess',
                        info='Enable roundabout-guessing.',
                        )
        self.add_option('is_check_lane_foes_roundabout', is_check_lane_foes_roundabout,
                        groupnames=['options', 'topology'],
                        cml='--check-lane-foes.roundabout',
                        perm='rw',
                        name='Allow entering multilane roundabout',
                        info='Allow driving onto a multi-lane road at roundabouts if there are foes (arriving vehicles) on other lanes.',
                        is_enabled=lambda self: self.is_roundabouts_guess,
                        )

        # --lefthand <BOOL> 	Assumes left-hand traffic on the network; default: false
        self.add_option('is_no_left_connections', is_no_left_connections,
                        groupnames=['options', 'topology'],
                        cml='--no-left-connections',
                        perm='rw',
                        name='no left connections',
                        info='Disables building connections to left.',
                        )

#-------------------------------------------------------------------------
        # geometry options
        self.add_option('is_geometry_split', is_geometry_split,
                        groupnames=['options', 'geometry'],
                        cml='--geometry.split',
                        perm='rw',
                        name='geometry split',
                        info='Splits edges across geometry nodes.',
                        )

        self.add_option('is_geometry_remove', is_geometry_remove,
                        groupnames=['options', 'geometry'],
                        cml='--geometry.remove',
                        perm='rw',
                        name='geometry remove',
                        info='Replace nodes which only define edge geometry by geometry points (joins edges).',
                        )

        self.add_option('length_max_segment', length_max_segment,
                        groupnames=['options', 'geometry'],
                        cml='--geometry.max-segment-length',
                        perm='rw',
                        unit='m',
                        name='Max segment length',
                        info='Splits geometry to restrict segment length to maximum segment length. The value of -1.0 means no effect.',
                        is_enabled=lambda self: self.length_max_segment > 0,
                        )

        self.add_option('dist_min_geometry', dist_min_geometry,
                        groupnames=['options', 'geometry'],
                        cml='--geometry.min-dist',
                        perm='rw',
                        unit='m',
                        name='Min geom dist',
                        info='Reduces too similar geometry points. The value of -1.0 means no effect.',
                        is_enabled=lambda self: self.dist_min_geometry > 0,
                        )

        # self.add_option('angle_max_geometry',99.0,
        #                groupnames = ['options','geometry'],#
        #                cml = '--geometry.max-angle',
        #                perm='rw',
        #                unit = 'degree',
        #                name = 'Max geom angle',
        #                info = ' 	Warn about edge geometries with an angle above the maximum angle in successive segments.',
        #                )
        # --geometry.min-radius <FLOAT> 	Warn about edge geometries with a turning radius less than METERS at the start or end; default: 9
        # --geometry.junction-mismatch-threshold <FLOAT> 	Warn if the junction shape is to far away from the original node position; default: 20


#-------------------------------------------------------------------------
        # Ramps

        self.add_option('is_guess_ramps', is_guess_ramps,
                        groupnames=['options', 'ramps'],
                        cml='--ramps.guess',
                        perm='rw',
                        name='Guess ramps',
                        info='Enable ramp-guessing.',
                        )

        self.add_option('rampspeed_max', rampspeed_max,
                        groupnames=['options', 'ramps'],
                        cml='--ramps.max-ramp-speed',
                        perm='rw',
                        unit='m/s',
                        name='Max ramp speed',
                        info='Treat edges with a higher speed as no ramps',
                        is_enabled=lambda self: self.is_guess_ramps,
                        )

        self.add_option('highwayspeed_min', highwayspeed_min,
                        groupnames=['options', 'ramps'],
                        cml='--ramps.min-highway-speed',
                        perm='rw',
                        unit='m/s',
                        name='Min highway speed',
                        info='Treat edges with lower speed as no highways (no ramps will be build there)',
                        is_enabled=lambda self: self.is_guess_ramps,
                        )

        self.add_option('ramplength', ramplength,
                        groupnames=['options', 'ramps'],
                        cml='--ramps.ramp-length',
                        perm='rw',
                        unit='m',
                        name='Ramp length',
                        info='Ramp length',
                        is_enabled=lambda self: self.is_guess_ramps,
                        )

        self.add_option('is_no_split_ramps', is_no_split_ramps,
                        groupnames=['options', 'ramps'],
                        cml='--ramps.no-split',
                        perm='rw',
                        name='No ramp split',
                        info='Avoids edge splitting at ramps.',
                        is_enabled=lambda self: self.is_guess_ramps,
                        )

#-------------------------------------------------------------------------

        self.is_clean_nodes = attrsman.add(cm.AttrConf('is_clean_nodes', is_clean_nodes,
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Clean Nodes',
                                                       info='If set, then shapes around nodes are cleaned up after importing.',
                                                       ))

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
            print '  OSM->sumo.xml done'
            if os.path.isfile(self.netfilepath):
                print '  OSM->sumo.xml successful, start generation of xml files'
                cml = self._command + ' --sumo-net-file ' + filepathlist_to_filepathstring(
                    self.netfilepath) + ' --plain-output-prefix ' + filepathlist_to_filepathstring(os.path.join(self.workdirpath, self.rootname))
                self.run_cml(cml)
                if self.status == 'success':
                    print '  start import into sumopy'
                    self._net.import_xml(
                        self.rootname, self.workdirpath, is_clean_nodes=self.is_clean_nodes)
                    print '  import sumopy done.'
            #self._net.import_xml(self.rootname, self.workdirpath, is_clean_nodes = self.is_clean_nodes)

        # print 'do',self.newident
        # self._scenario = Scenario(  self.newident,
        #                                parent = None,
        #                                workdirpath = self.workdirpath,
        #                                logger = self.get_logger(),
        #                                )

    def get_net(self):
        return self._net


if __name__ == '__main__':
    ##########################################################################
    # print 'sys.path',sys.path
    from agilepy.lib_wx.objpanel import objbrowser
    from agilepy.lib_base.logger import Logger
    #net = Network(logger = Logger())
    net = Network(logger=Logger())
    net.import_xml('facsp2', 'testnet')

    objbrowser(net)
