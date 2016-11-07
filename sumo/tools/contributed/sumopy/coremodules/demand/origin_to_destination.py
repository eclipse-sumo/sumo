
import numpy as np
from numpy import random
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from agilepy.lib_base.processes import Process, CmlMixin
#import coremodules.demand.demand as dm
import demand as dm
# print 'dir(dm)',dir(dm)
#from demand import OPTIONMAP_POS_DEPARTURE
# OPTIONMAP_POS_ARRIVAL
# OPTIONMAP_SPEED_DEPARTURE
# OPTIONMAP_SPEED_ARRIVAL
# OPTIONMAP_LANE_DEPART
# OPTIONMAP_LANE_ARRIVAL


class OdTripgenerator(Process):

    def __init__(self, odintervals, trips, logger=None, **kwargs):
        """
        CURRENTLY NOT IN USE!!
        """
        self._init_common('odtripgenerator', name='OD tripgenerator',
                          logger=logger,
                          info='Generates trips from OD demand .',
                          )
        self._odintervals = odintervals

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


class OdFlowTable(am.ArrayObjman):

    def __init__(self, parent, modes, zones, **kwargs):
        self._init_objman(ident='odflowtab', parent=parent,
                          name='OD flows',
                          info='Table with intervals, modes, OD and respective number of trips.',
                          #xmltag = ('odtrips','odtrip',None),
                          **kwargs)

        self.add_col(am.ArrayConf('times_start', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Start time',
                                  unit='s',
                                  info='Start time of interval in seconds (no fractional seconds).',
                                  xmltag='t_start',
                                  ))

        self.add_col(am.ArrayConf('times_end', 3600,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='End time',
                                  unit='s',
                                  info='End time of interval in seconds (no fractional seconds).',
                                  xmltag='t_end',
                                  ))

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['state'],
                                     choices=MODES,
                                     name='ID mode',
                                     xmltag='vClass',
                                     info='ID of transport mode.',
                                     ))

        self.add_col(am.IdsArrayConf('ids_orig', zones,
                                     groupnames=['state'],
                                     name='Orig.',
                                     choices=zones.ids_sumo.get_indexmap(),
                                     info='traffic assignment zone of origin of trip.',
                                     xmltag='id_orig',
                                     ))

        self.add_col(am.IdsArrayConf('ids_dest', zones,
                                     groupnames=['state'],
                                     name='Dest.',
                                     choices=zones.ids_sumo.get_indexmap(),
                                     info='ID of traffic assignment zone of destination of trip.',
                                     xmltag='id_dest',
                                     ))

        self.add_col(am.ArrayConf('tripnumbers', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Trips',
                                  info='Number of trips from zone with ID Orig to zone with ID Dest.',
                                  xmltag='tripnumber',
                                  ))

        #self.add( cm.ObjConf( zones, is_child = False,groups = ['_private']))

    def add_flows(self,  time_start,
                  time_end,
                  id_mode,
                  ids_orig,
                  ids_dest,
                  tripnumbers,
                  ):
        n = len(tripnumbers)
        self.add_rows(n=n,
                      times_start=time_start * np.ones(n),
                      times_end=time_end * np.ones(n),
                      ids_mode=id_mode * np.ones(n),
                      ids_orig=ids_orig,
                      ids_dest=ids_dest,
                      tripnumbers=tripnumbers,
                      )


class OdTrips(am.ArrayObjman):

    def __init__(self, ident, parent, zones, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='OD trips',
                          info='Contains the number of trips between an origin and a destination zone.',
                          xmltag=('odtrips', 'odtrip', None), **kwargs)

        self.add_col(am.IdsArrayConf('ids_orig', zones,
                                     groupnames=['state'],
                                     name='Orig.',
                                     choices=zones.ids_sumo.get_indexmap(),
                                     info='traffic assignment zone of origin of trip.',
                                     xmltag='id_orig',
                                     ))

        self.add_col(am.IdsArrayConf('ids_dest', zones,
                                     groupnames=['state'],
                                     name='Dest.',
                                     choices=zones.ids_sumo.get_indexmap(),
                                     info='ID of traffic assignment zone of destination of trip.',
                                     xmltag='id_dest',
                                     ))

        self.add_col(am.ArrayConf('tripnumbers', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Trips',
                                  info='Number of trips from zone with ID Orig to zone with ID Dest.',
                                  xmltag='tripnumber',
                                  ))

        self.add(cm.ObjConf(zones, is_child=False, groups=['_private']))

    def _init_constants(self):
        #self.edgeweights_orig = None
        #self.edgeweights_dest = None
        pass

    def generate_odflows(self, odflowtab,  time_start, time_end, id_mode, **kwargs):
        """
        Insert all od flows in odflowtab.
        """
        # for id_od in self.get_ids():
        odflowtab.add_flows(time_start,
                            time_end,
                            id_mode,
                            self.ids_orig.get_value(),
                            self.ids_dest.get_value(),
                            self.tripnumbers.get_value()
                            )

    def generate_trips(self, demand, time_start, time_end, id_mode,
                       pos_depart_default=dm.OPTIONMAP_POS_DEPARTURE[
                           'random_free'],
                       pos_arrival_default=dm.OPTIONMAP_POS_ARRIVAL['max'],
                       speed_depart_default=0.0,
                       speed_arrival_default=0.0,
                       ind_lane_depart_default=dm.OPTIONMAP_LANE_DEPART[
                           'allowed'],  # pedestrians always depart on lane 0
                       ind_lane_arrival_default=dm.OPTIONMAP_LANE_ARRIVAL[
                           'current'],  # pedestrians always arrive on lane 0
                       ):
        """
        Generates trips in demand.trip table.
        """
        print 'generate_trips', time_start, time_end, id_mode
        id_mode_ped = MODES['pedestrian']
        #OPTIONMAP_POS_DEPARTURE = { -1:"random",-2:"free",-3:"random_free",-4:"base"}
        #OPTIONMAP_POS_ARRIVAL = { -1:"random",-2:"max"}
        #OPTIONMAP_SPEED_DEPARTURE = { -1:"random",-2:"max"}
        #OPTIONMAP_SPEED_ARRIVAL = { -1:"current"}
        #OPTIONMAP_LANE_DEPART = {-1:"random",-2:"free",-3:"departlane"}
        #OPTIONMAP_LANE_ARRIVAL = { -1:"current"}

        trips = demand.trips
        ids_vtypes_mode = demand.vtypes.select_by_mode(id_mode)
        # print '  ids_vtypes_mode', ids_vtypes_mode
        n_vtypes = len(ids_vtypes_mode)
        zones = self.zones.get_value()
        edges = zones.ids_edges_orig.get_linktab()
        edgelengths = edges.lengths

        n_trips_generated = 0
        n_trips_failed = 0

        for id_od in self.get_ids():
            id_orig = self.ids_orig[id_od]
            id_dest = self.ids_dest[id_od]
            tripnumber = self.tripnumbers[id_od]

            ids_edges_orig_raw = zones.ids_edges_orig[id_orig]
            ids_edges_dest_raw = zones.ids_edges_dest[id_dest]

            prob_edges_orig_raw = zones.probs_edges_orig[id_orig]
            prob_edges_dest_raw = zones.probs_edges_dest[id_dest]

            # check accessibility of origin edges
            ids_edges_orig = []
            prob_edges_orig = []
            inds_lane_orig = []
            for i in xrange(len(ids_edges_orig_raw)):
                id_edge = ids_edges_orig_raw[i]
                # if check accessibility...
                ind_lane_depart = edges.get_laneindex_allowed(id_edge, id_mode)
                # print '
                # get_laneindex_allowed',id_mode,id_edge,edges.ids_sumo[id_edge],ind_lane_depart
                if ind_lane_depart >= 0:
                    ids_edges_orig.append(id_edge)
                    prob_edges_orig.append(prob_edges_orig_raw[i])
                    inds_lane_orig.append(ind_lane_depart)

                #orig = net.getEdge(id)
                # if id_mode == MODES['pedestrian']:
                #    # pedestrians start on footpath
                #    ind_lane_depart = 0
                # else:
                #    # get lane index for non-pedestrians
                #    ind_lane_depart = ind_lane_depart_default
                #
                    # here we can serach better solutions...
                    # for ind_lane_depart_default == -4 we could choose the first allowed lane
                    # idLane=len(orig.getLanes())-1
                # print '   isAllowed orig',id,orig.isAllowed(mode, idLane = idLane)
                # if orig.isAllowed(mode, idLane = idLane):
                #    origids.append(id)
                #    origweights.append(ids_edges_orig[id])

            # check accessibility of destination edges
            ids_edges_dest = []
            prob_edges_dest = []
            inds_lane_dest = []
            for i in xrange(len(ids_edges_dest_raw)):
                id_edge = ids_edges_dest_raw[i]
                # if check accessibility...
                ind_lane_arrival = edges.get_laneindex_allowed(
                    id_edge, id_mode)
                if ind_lane_arrival >= 0:
                    ids_edges_dest.append(id_edge)
                    prob_edges_dest.append(prob_edges_dest_raw[i])
                    inds_lane_dest.append(ind_lane_arrival)

                # if id_mode == MODES['pedestrian']:
                #    # pedestrians start on footpath
                #    ind_lane_arrival = 0
                # else:
                #    # get lane index for non-pedestrians
                #    ind_lane_arrival = ind_lane_arrival_default

            n_edges_orig = len(ids_edges_orig)
            n_edges_dest = len(ids_edges_dest)

            if (n_edges_orig > 0) & (n_edges_dest > 0) & (tripnumber > 0):
                # renormalize weights
                prob_edges_orig = np.array(prob_edges_orig, np.float)
                prob_edges_orig = prob_edges_orig / np.sum(prob_edges_orig)
                prob_edges_dest = np.array(prob_edges_dest, np.float)
                prob_edges_dest = prob_edges_dest / np.sum(prob_edges_dest)

                for d in xrange(int(tripnumber + 0.5)):
                    time_depart = random.uniform(time_start, time_end)

                    i_orig = np.argmax(random.rand(
                        n_edges_orig) * prob_edges_orig)
                    id_edge_orig = ids_edges_orig[i_orig]
                    ind_lane_orig = inds_lane_orig[i_orig]

                    i_dest = np.argmax(random.rand(
                        n_edges_dest) * prob_edges_dest)
                    id_edge_dest = ids_edges_dest[i_dest]
                    ind_lane_dest = inds_lane_dest[i_dest]

                    pos_depart = pos_depart_default
                    pos_arrival = pos_arrival_default
                    # print '  bef:pos_depart,pos_arrival,id_mode,id_mode_ped',
                    # pos_depart,pos_arrival,id_mode,id_mode_ped
                    if id_mode_ped == id_mode:
                        # persons do not understand "random", "max" etc
                        # so produce a random number here

                        #{ -1:"random",-2:"free",-3:"random_free",-4:"base"}
                        edgelength = edgelengths[id_edge_orig]
                        if pos_depart in (-1, -2, -3):
                            pos_depart = random.uniform(
                                0.1 * edgelength, 0.9 * edgelength, 1)[0]
                        else:
                            pos_depart = 0.1 * edgelength

                        # { -1:"random",-2:"max"}
                        edgelength = edgelengths[id_edge_dest]
                        if pos_arrival == -1:
                            pos_arrival = random.uniform(
                                0.1 * edgelength, 0.9 * edgelength, 1)[0]
                        else:
                            pos_arrival = 0.9 * edgelength
                    # print '  af:pos_depart,pos_arrival,id_mode,id_mode_ped',  pos_depart,pos_arrival,id_mode,id_mode_ped
                    # print '  n_vtypes',n_vtypes
                    # print '
                    # random.randint(n_vtypes)',random.randint(n_vtypes)
                    id_vtype = ids_vtypes_mode[random.randint(n_vtypes)]

                    id_trip = trips.make_trip(id_vtype=id_vtype,
                                              time_depart=time_depart,
                                              id_edge_depart=id_edge_orig,
                                              id_edge_arrival=id_edge_dest,
                                              ind_lane_depart=ind_lane_orig,
                                              ind_lane_arrival=ind_lane_dest,
                                              position_depart=pos_depart,
                                              position_arrival=pos_arrival,
                                              speed_depart=speed_depart_default,
                                              speed_arrival=speed_arrival_default,
                                              )
                    # print '  ',id_trip,id_edge_orig,edges.ids_sumo[id_edge_orig],ind_lane_depart
                    # print '  ',id_trip,self.position_depart[id_trip],
                    n_trips_generated += 1

            else:
                n_trips_failed += tripnumber

        print '  n_trips_generated', n_trips_generated
        print '  n_trips_failed', n_trips_failed

    def add_od_trips(self, scale, names_orig, names_dest, tripnumbers):
        print 'OdTrips.add_od_trips'
        # print '  scale, names_orig, names_dest, tripnumbers',scale,
        # names_orig, names_dest, tripnumbers,len(tripnumbers)
        zones = self.get_zones()
        print '  ids_sumo', zones.ids_sumo.get_value()
        print '  ids_sumo._index_to_id', zones.ids_sumo._index_to_id
        for name_orig, name_dest, tripnumber in zip(names_orig, names_dest, tripnumbers):
            print '  check', name_orig, name_dest, tripnumbers, zones.ids_sumo.has_index(name_orig), zones.ids_sumo.has_index(name_dest)
            if (zones.ids_sumo.has_index(name_orig)) & (zones.ids_sumo.has_index(name_dest)):
                print '  add', zones.ids_sumo.get_id_from_index(name_orig), zones.ids_sumo.get_id_from_index(name_dest)
                self.add_row(ids_orig=zones.ids_sumo.get_id_from_index(name_orig),
                             ids_dest=zones.ids_sumo.get_id_from_index(
                                 name_dest),
                             tripnumbers=scale * tripnumber)

    def get_zones(self):
        return self.ids_dest.get_linktab()


class OdModes(am.ArrayObjman):

    def __init__(self, ident, parent, modes, zones, **kwargs):
        self._init_objman(ident, parent=parent,
                          name='Mode OD tables',
                          info='Contains for each transport mode an OD trip table.',
                          xmltag=('modesods', 'modeods', 'ids_mode'), **kwargs)

        self.add_col(am.IdsArrayConf('ids_mode', modes,
                                     groupnames=['state'],
                                     choices=MODES,
                                     name='ID mode',
                                     xmltag='vClass',
                                     info='ID of transport mode.',
                                     ))

        self.add_col(cm.ObjsConf('odtrips',
                                 groupnames=['state'],
                                 is_save=True,
                                 name='OD matrix',
                                 info='Matrix with trips from origin to destintion for a specific mode.',
                                 ))
        self.add(cm.ObjConf(zones, is_child=False, groups=['_private']))

    def generate_trips(self, demand, time_start, time_end, **kwargs):
        for id_od_mode in self.get_ids():
            self.odtrips[id_od_mode].generate_trips(
                demand, time_start, time_end, self.ids_mode[id_od_mode], **kwargs)

    def generate_odflows(self, odflowtab, time_start, time_end, **kwargs):
        for id_od_mode in self.get_ids():
            self.odtrips[id_od_mode].generate_odflows(
                odflowtab, time_start, time_end, self.ids_mode[id_od_mode], **kwargs)

    def add_od_trips(self, id_mode, scale, names_orig, names_dest, tripnumbers):
        print 'OdModes.add_od_trips', id_mode, scale, names_orig, names_dest, tripnumbers
        ids_mode = self.select_ids(self.ids_mode.get_value() == id_mode)
        if len(ids_mode) == 0:
            id_od_modes = self.add_row(ids_mode=id_mode)
            print '  create', id_od_modes
            odtrips = OdTrips((self.odtrips.attrname, id_od_modes),
                              self, self.zones.get_value())
            self.odtrips[id_od_modes] = odtrips
            odtrips.add_od_trips(scale, names_orig, names_dest, tripnumbers)
        else:
            id_od_modes = ids_mode[0]  # modes are unique
            print '  use', id_od_modes
            self.odtrips[id_od_modes].add_od_trips(
                scale, names_orig, names_dest, tripnumbers)


class OdIntervals(am.ArrayObjman):

    def __init__(self, ident='odintervals',  parent=None, net=None, zones=None, **kwargs):
        self._init_objman(ident, parent=parent,  # = demand
                          name='OD Demand',
                          info='Contains origin-to-destination zone transport demand for different time intervals.',
                          xmltag=('odintervals', 'odinteval', None), **kwargs)

        self.add_col(am.ArrayConf('times_start', 0,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='Start time',
                                  unit='s',
                                  info='Start time of interval in seconds (no fractional seconds).',
                                  xmltag='t_start',
                                  ))

        self.add_col(am.ArrayConf('times_end', 3600,
                                  groupnames=['state'],
                                  perm='rw',
                                  name='End time',
                                  unit='s',
                                  info='End time of interval in seconds (no fractional seconds).',
                                  xmltag='t_end',
                                  ))

        self.add_col(cm.ObjsConf('odmodes',
                                 groupnames=['state'],
                                 is_save=True,
                                 name='OD modes',
                                 info='OD transport demand for all transport modes within the respective time interval.',
                                 ))
        self.add(cm.ObjConf(net, is_child=False, groups=['_private']))
        self.add(cm.ObjConf(zones, is_child=False, groups=['_private']))

    def generate_trips(self, **kwargs):
        """
        Generates trips in trip table.
        """
        # make sure zone edges are up to date
        self.get_zones().refresh_zoneedges()
        demand = self.parent
        for id_inter in self.get_ids():
            self.odmodes[id_inter].generate_trips(demand,   self.times_start[id_inter],
                                                  self.times_end[id_inter],
                                                  **kwargs)

    def generate_odflows(self, **kwargs):
        odflowtab = OdFlowTable(self, self.get_modes(), self.get_zones())
        for id_inter in self.get_ids():
            self.odmodes[id_inter].generate_odflows(odflowtab,   self.times_start[id_inter],
                                                    self.times_end[id_inter],
                                                    **kwargs)
        return odflowtab

    def clear_od_trips(self):
        self.clear()

    def add_od_trips(self, t_start, t_end, id_mode, scale,
                     names_orig, names_dest, tripnumbers):

        print 'OdIntervals.add_od_trips', t_start, t_end, id_mode, scale, names_orig, names_dest, tripnumbers
        ids_inter = self.select_ids((self.times_start.get_value() == t_start) & (
            self.times_end.get_value() == t_end))
        if len(ids_inter) == 0:

            id_inter = self.add_row(times_start=t_start, times_end=t_end,)
            print '  create new', id_inter
            #odintervals.add_rows(2, t_start=[0,3600], t_end=[3600, 7200])
            odmodes = OdModes((self.odmodes.attrname, id_inter), parent=self,
                              modes=self.get_net().modes, zones=self.get_zones())
            # NO!! odmodes = OdModes( ('ODMs for modes', id_inter), parent =
            # self, modes = self.get_net().modes, zones = self.get_zones())
            self.odmodes[id_inter] = odmodes

            odmodes.add_od_trips(id_mode, scale, names_orig,
                                 names_dest, tripnumbers)

        else:

            # there should be only one demand table found for a certain
            # interval
            id_inter = ids_inter[0]
            print '  use', id_inter
            self.odmodes[id_inter].add_od_trips(
                id_mode, scale, names_orig, names_dest, tripnumbers)

    def get_net(self):
        return self.net.get_value()

    def get_zones(self):
        return self.zones.get_value()

    def get_modes(self):
        return self.net.get_value().modes
