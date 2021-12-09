# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    detectorflows.py
# @author  Joerg Schweizer
# @date   2012

import os
import string
from collections import OrderedDict
import numpy as np
from numpy import random
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.geometry import *
#from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from agilepy.lib_base.processes import Process, P, call, CmlMixin
from coremodules.misc.shapeformat import ShapefileImporter


class Detectorflows(cm.BaseObjman):
    def __init__(self, ident='detectorflows', demand=None,  name='Detector flows',
                 info='Flows measured by detectors, which can be used to generate vehicle routes using the DFRouter.',
                 **kwargs):
        print 'Detectorflows.__init__', name, kwargs

        self._init_objman(ident=ident,
                          parent=demand,
                          name=name,
                          info=info,
                          **kwargs)

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        attrsman = self.get_attrsman()

        self.detectors = attrsman.add(cm.ObjConf(Detectors('detectors', self),
                                                 groupnames=['parameters'])
                                      )

        self.flowmeasurements = attrsman.add(cm.ObjConf(Flowmeasurements('flowmeasurements', self),
                                                        groupnames=['parameters'])
                                             )

    def get_scenario(self):
        return self.parent.get_scenario()

    def add_flows_to_edgeresults(self, edgeresults):
        if not edgeresults.has_attrname('detectorflows'):
            # add detector result attributes

            attrinfos = OrderedDict([
                ('detectorflows',  {'name': 'Detector flows',
                                    # 'symbol':'F_{MA}',
                                    'unit': None,
                                    'default': 0,
                                    'info': 'Flows measured by detectors.',
                                    'xmltag': 'detectorFlows',
                                    'groupnames': ['detectorflows']
                                    }),
                # more result attributes can be added ex. heavy duty flows
            ])

            for attrname, kwargs in attrinfos.iteritems():
                edgeresults.add_resultattr(attrname, **kwargs)

        # reset Detector flow attributes
        for attrconf in edgeresults.get_group('Detector flows'):
            attrconf.detectorflows.reset()

        # add flow measurements
        detectors = self.detectors
        flowmeasurements = self.flowmeasurements
        net = self.get_scenario().net
        ids_flow = flowmeasurements.get_ids()
        for id_detector, period, flow_pax, flow_heavyduty in zip(
                flowmeasurements.ids_detector[ids_flow],
                flowmeasurements.timeperiods[ids_flow],
                flowmeasurements.flows_passenger[ids_flow],
                flowmeasurements.flows_heavyduty[ids_flow],):

            if detectors.reliabilities[id_detector] > 0.8:
                if (flow_pax > 0) | (flow_heavyduty > 0):
                    flow = 0.0
                    if (flow_pax > 0):
                        flow += flow_pax
                    if (flow_heavyduty > 0):
                        flow += flow_heavyduty

                    ids_lane = detectors.ids_lanes[id_detector]
                    if ids_lane is not None:  # happens if no lenes could be matched
                        if len(ids_lane) > 0:
                            id_edge = net.lanes.ids_edge[ids_lane[0]]
                            if edgeresults.ids_edge.has_index(id_edge):
                                id_edgeres = edgeresults.ids_edge.get_id_from_index(id_edge)
                                edgeresults.detectorflows[id_edgeres] += flow

                            else:
                                edgeresults.add_row(ids_edge=id_edge,
                                                    detectorflows=flow
                                                    )


class Detectors(am.ArrayObjman):
    def __init__(self, ident, detectorflows, **kwargs):
        self._init_objman(ident, parent=detectorflows,
                          name='Detectors',
                          info='Contains detector data.',
                          xmltag=('detectors', 'detectorDefinition', 'ids_sumo'),
                          **kwargs)

        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        self.add_col(SumoIdsConf('Detector', xmltag='id'))

        self.add_col(am.IdlistsArrayConf('ids_lanes', self.get_scenario().net.lanes,
                                         name='IDs Lane',
                                         info="""Lane IDs where detector is located. 
                                    Single or multiple lanes must be from the same egde.
                                    In case of multiple lanes the measured flows are distributed on the lanes using some heuristics.""",
                                         ))

        if hasattr(self, 'ids_lane'):
            self.delete('ids_lane')

        self.add_col(am.ArrayConf('names', "",
                                  dtype=np.object,
                                  perm='rw',
                                  name='Name',
                                  info='Human readable detector name. Can be used to indicate detector location.',
                                  ))

        self.add_col(am.ArrayConf('positions', 0.0,
                                  perm='r',
                                  name='Position',
                                  unit='m',
                                  xmltag='pos',
                                  info='Position on edge where detector is located.',
                                  ))

        self.add_col(am.ArrayConf('directions', 0.0,
                                  perm='rw',
                                  name='direction',
                                  unit='DEG',
                                  info='Direction of detector in degrees. North is zero degree. ',
                                  ))

        self.add_col(am.ArrayConf('coords',  np.zeros(3, dtype=np.float32),
                                  groupnames=['state'],
                                  perm='r',
                                  name='Coords',
                                  unit='m',
                                  info='Dedector coordinate.',
                                  is_plugin=True,
                                  ))

        self.add_col(am.ArrayConf('reliabilities', 1.0,
                                  perm='rw',
                                  name='Reliability',
                                  info='Reliability of detector: 0 = broken, 1 = fully reliable. Some detectors provide this information which can help to eliminate unreliable flow measurements.',
                                  ))

    def get_scenario(self):
        return self.parent.parent.get_scenario()

    def match_detectors_to_lane(self, modename='passenger',
                                is_edgedetectors=True, is_check_direction=True,
                                n_targetedge=5, d_max=50.0,
                                phidelta_max=np.pi/180*10):
        net = self.get_scenario().net
        lanes = net.lanes
        edges = net.edges
        id_mode = net.modes.get_id_mode(modename)
        accesslevels = edges.get_accesslevels(net.modes.get_id_mode(modename=modename))

        get_ids_edge_from_inds_seg = edges.get_ids_edge_from_inds_seg
        get_dist_point_to_edge = edges.get_dist_point_to_edge
        get_closest_edge = edges.get_closest_edge

        ids = self.get_ids()
        for id_detect, point, phi in zip(ids, self.coords[ids], self.directions[ids]/180.0*np.pi+np.pi/2.0):
            print '  Detector id_detect', id_detect, 'point', point
            ids_edge_target, dists = get_closest_edge(point,  n_best=n_targetedge,
                                                      d_max=d_max,
                                                      is_ending=True,
                                                      is_detect_initial=False,
                                                      is_detect_final=False,
                                                      accesslevels=accesslevels
                                                      )

            print '    ids_edge_target', ids_edge_target
            print '    dists', dists

            if is_check_direction:
                id_edge_found = -1
                i = 0
                n = len(ids_edge_target)
                while (id_edge_found < 0) & (i < n):
                    print '    check ids_edge', ids_edge_target[i], dists[i]
                    dist_point_edge, segment = get_dist_point_to_edge(point, ids_edge_target[i],
                                                                      is_ending=True,
                                                                      is_detect_initial=False,
                                                                      is_detect_final=True,
                                                                      is_return_segment=True,
                                                                      )
                    x1, y1, x2, y2 = segment
                    phi_seg = np.arctan2(y2-y1, x2-x1)
                    phi_delta = np.abs(anglediff(phi, phi_seg))
                    if phi_delta < phidelta_max:
                        id_edge_found = ids_edge_target[i]
                    i += 1
            elif len(ids_edge_target) > 0:
                id_edge_found = ids_edge_target[0]
            else:
                id_edge_found = -1

            if id_edge_found >= 0:
                # select lane
                ids_lane = edges.ids_lanes[id_edge_found]
                print '  id_edge_found', id_edge_found, 'ids_lane', edges.ids_lanes[id_edge_found]
                ids_lane_access = ids_lane[lanes.get_laneindexes_allowed(ids_lane, id_mode)]
                print '  ids_lane_access', ids_lane_access

                if len(ids_lane_access) > 0:
                    if is_edgedetectors:
                        # associate all lanes with detector
                        self.ids_lanes[id_detect] = ids_lane_access

                    else:
                        # coose closest lane....TODO
                        self.ids_lanes[id_detect] = ids_lane_access

    def write_xml(self, fd=None, indent=0):
        # <detectors>
        #    <detectorDefinition id="<DETECTOR_ID>" lane="<LANE_ID>" pos="<POS>"/>
        #    ... further detectors ...
        # </detectors>
        print 'Detectors.write_xml'
        fd.write(xm.begin('detectors', indent))

        ids = self.get_ids()
        scenario = self.get_scenario()
        get_sumoinfo_from_id_lane = scenario.net.lanes.get_sumoinfo_from_id_lane
        for id_detector, ids_lane, pos in zip(
            ids,
            self.ids_lanes[ids],
            self.positions[ids],
        ):

            print '  write id_detector', id_detector, 'ids_lane', ids_lane
            if ids_lane is not None:
                ind_lane = 0
                for id_lane in ids_lane:
                    fd.write(xm.start('detectorDefinition', indent=indent+2))
                    fd.write(xm.num('id', self.get_id_xml_detector(id_detector, ind_lane)))
                    fd.write(xm.num('lane', get_sumoinfo_from_id_lane(id_lane)))
                    fd.write(xm.num('pos', pos))

                    fd.write(xm.stopit())  # ends detector defs
                    ind_lane += 1
        fd.write(xm.end('detectors', indent))

    def get_id_xml_detector(self, id_detector, ind_lane):
        return self.ids_sumo[id_detector]+'/'+str(ind_lane)

    def get_id_detector_ind_lane(self, id_xml):
        if id_xml.count('/') > 0:
            elements = id_xml.split('/')
            id_sumo = string.join(elements[:-1], '/')
            return self.ids_sumo.get_index(id_sumo), int(elements[-1])
        else:
            return self.ids_sumo.get_index(id_xml), 0

    def export_sumoxml(self, filepath):
        xm.write_obj_to_xml(self, filepath)


class Flowmeasurements(am.ArrayObjman):
    def __init__(self, ident, detectorflows, **kwargs):
        self._init_objman(ident, parent=detectorflows,
                          name='Flow measurements',
                          info='Flow measurements at detectors.',
                          #xmltag = ('detectors','detectorDefinition','ids_sumo'),
                          **kwargs)

        self._init_attributes()
        self._init_constants()

    def _init_attributes(self):
        """
        Detector;Time;qPKW;qLKW;vPKW;vLKW
        myDet1;0;10;2;100;80
        ... further entries ...

        This means the first time has to name the entries (columns). Their order is not of importance, but at least the following columns must be included:

            Detector: A string holding the id of the detector this line describes; should be one of the ids used in <DETECTOR_FILE>
            Time: The time period begin that this entry describes (in minutes)
            qPKW: The number of passenger cars that drove over the detector within this time period
            vPKW: The average speed of passenger cars that drove over the detector within this time period in km/h

        The following columns may optionally be included:

            qLKW: The number of transport vehicles that drove over the detector within this time period
            vLKW: The average speed of transport vehicles that drove over the detector within this time period in km/h
        """

        self.add_col(am.IdsArrayConf('ids_detector', self.parent.detectors,
                                     name='ID Detector',
                                     info='Detector ID which measured this flow.',
                                     ))

        self.add_col(am.ArrayConf('timeperiods', 0.0,
                                  perm='rw',
                                  name='Time period',
                                  unit='s',
                                  info='Time period of flow measurement.',
                                  ))
        self.add_col(am.ArrayConf('flows_passenger', 0.0,
                                  perm='rw',
                                  name='Passenger car flow',
                                  info='Number of passenger cars detected during the corrisponding time period.',
                                  ))

        self.add_col(am.ArrayConf('flows_heavyduty', 0.0,
                                  perm='rw',
                                  name='Heavy duty vehicle flow',
                                  info='Number of heavy duty vehicles detected during the corrisponding time period.',
                                  ))

        self.add_col(am.ArrayConf('speeds_passenger', 0.0,
                                  perm='rw',
                                  name='Passenger car speed',
                                  unit='m/s',
                                  info='Average velocity of passenger cars, detected during the corrisponding time period.',
                                  ))

        self.add_col(am.ArrayConf('speeds_heavyduty', 0.0,
                                  perm='rw',
                                  name='Heavy duty vehicle speed',
                                  unit='m/s',
                                  info='Average velocity of heavy duty vehicles, detected during the corrisponding time period.',
                                  ))

        # self.add_col(am.ArrayConf( 'types', '',
        #                        perm='rw',
        #                        choices = ["source", "sink", "between", "discarded", ""]
        #                        name = 'Type',
        #                        xmltag = 'type',
        #                        info = 'Detector type.',
        #                        ))

    def get_demand(self):
        return self.parent.parent

    def add_flow(self, id_detector, timeperiod, flow_passenger,
                 flow_heavyduty=0.0, speed_passenger=-1.0, speed_heavyduty=-1.0):

        if id_detector in self.parent.ids_sumo.get_value():
            id_flow = self.add_row(ids_detector=id_detector,
                                   timeperiods=timeperiod,
                                   flows_passenger=flow_passenger,
                                   flows_heavyduty=flow_heavyduty,
                                   speeds_passenger=speed_passenger,
                                   speeds_heavyduty=speed_heavyduty,
                                   )
        else:
            id_flow = -1

        return id_flow

    def export_csv(self, filepath, t_offset=0, sep=";"):
        net = self.parent.parent.get_net()
        detectors = self.parent.detectors
        ids_sumo_detector = detectors.ids_sumo
        get_id_xml_detector = detectors.get_id_xml_detector

        f = open(filepath, 'w')

        f.write('Detector'+sep+'Time'+sep+'qPKW'+sep+'qLKW'+sep+'vPKW'+sep+'vLKW'+'\n')
        #ids_flow = self.select_ids(self.flows_passenger.get_value()>=0)
        print '  flows_passenger', self.flows_passenger.get_value()
        ids_flow = self.get_ids()
        for id_detector, t, flow_passenger, flow_heavyduty, speed_passenger, speed_heavyduty in \
            zip(
                self.ids_detector[ids_flow],
                self.timeperiods[ids_flow],
                self.flows_passenger[ids_flow],
                self.flows_heavyduty[ids_flow],
                self.speeds_passenger[ids_flow],
                self.speeds_heavyduty[ids_flow]):

            ids_lane = detectors.ids_lanes[id_detector]
            if (ids_lane is not None) & (flow_passenger >= 0):
                print '  id_detector', id_detector, 't', t, 'flow_passenger', flow_passenger, len(ids_lane)

                n_lane = len(ids_lane)
                for ind_lane, passengerflow_lane, heavyflow_lane in\
                    zip(xrange(n_lane),
                        self.distribute_passengerflow_over_lanes(ids_lane, flow_passenger),
                        self.distribute_heavyflow_over_lanes(ids_lane, flow_heavyduty)
                        ):

                    f.write(get_id_xml_detector(id_detector, ind_lane))
                    f.write(sep+str(t))
                    f.write(sep+str(passengerflow_lane))

                    if flow_heavyduty >= 0:
                        f.write(sep+str(heavyflow_lane))

                    if speed_passenger >= 0:
                        f.write(sep+str(speed_passenger))

                    if speed_heavyduty >= 0:
                        f.write(sep+str(speed_heavyduty))

                    f.write("\n")

        # format from
        # https://sumo.dlr.de/wiki/Demand/Routes_from_Observation_Points
        # Detector;Time;qPKW;qLKW;vPKW;vLKW
        # myDet1;0;10;2;100;80
        # ... further entries ...
        f.close()

    def distribute_heavyflow_over_lanes(self, ids_lane, flow, flow_max=600):
        """
        Distribute measured edge traffic flow of lorries over the given lanes.
        """
        n_lane = len(ids_lane)
        flows_lane = np.zeros(n_lane, dtype=np.float32)

        if n_lane <= 2:
            flows_lane[0] = flow

        else:
            flows_lane[0:2] = [0.95*flow, 0.05*flow]
        return np.array(flows_lane+0.5, dtype=np.int32)

    def distribute_passengerflow_over_lanes(self, ids_lane, flow, flow_max=600):
        """
        Distribute measured edge traffic flow of passenger cars over the given lanes.
        """
        n_lane = len(ids_lane)
        flows_lane = np.zeros(n_lane, dtype=np.float32)

        if n_lane == 1:
            flows_lane[0] = flow

        elif n_lane == 2:
            if flow/2 > flow_max:
                flows_lane[0:2] = flow/2

            elif flow > flow_max:
                flows_lane[0:2] = [flow_max, flow-flow_max]

            else:
                flows_lane[0:2] = [0.8*flow, 0.2*flow]

        elif n_lane == 3:
            if flow/3 > flow_max:
                flows_lane[0:3] = flow/3

            elif flow/2 > flow_max:
                flows_lane[0:3] = [flow_max, flow_max, flow-2*flow_max]

            elif flow > flow_max:
                flows_lane[0:3] = [0.5*(flow-flow_max), flow_max, 0.5*(flow-flow_max)]

            else:
                flows_lane[0:3] = [0.2*flow, 0.7*flow, 0.1*flow]

        elif n_lane == 4:
            if flow/4 > flow_max:
                flows_lane[0:4] = flow/4

            elif flow/3 > flow_max:
                flows_lane[0:4] = [flow_max, flow_max, flow_max, flow-3*flow_max]

            elif flow/2 > flow_max:
                flows_lane[0:4] = [0.5*(flow-2*flow_max), flow_max, flow_max, 0.5*(flow-2*flow_max)]

            elif flow > flow_max:
                flows_lane[0:4] = [1.0/3.0*(flow-flow_max), flow_max, 1.0/3.0*(flow-flow_max), 1.0/3.0*(flow-flow_max)]

            else:
                flows_lane[0:4] = [0.2*flow, 0.5*flow, 0.2*flow, 0.1*flow]

        else:
            flows_lane[0:4] = 1.0*flow/n_lane

        return np.array(flows_lane+0.5, dtype=np.int32)

    def import_csv(self, filepath, t_offset=0, t_start=0, t_end=24*3600, sep=";"):
        net = self.parent.parent.get_net()
        detectors = self.parent.detectors
        ids_sumo = detectors.ids_sumo

        ids_detector = []
        times = []
        flows_passenger = []
        flows_heavyduty = []
        speeds_passenger = []
        speeds_heavyduty = []

        f = open(filepath, 'r')

        # format from
        # https://sumo.dlr.de/wiki/Demand/Routes_from_Observation_Points
        # Detector;Time;qPKW;qLKW;vPKW;vLKW
        # myDet1;0;10;2;100;80
        # ... further entries ...

        print 'import_csv', filepath
        ind_col = 0
        cols = f.readline().strip()
        ind_col = 0
        ind_detector = -1
        ind_time = -1
        ind_flow_passenger = -1
        ind_flow_heavyduty = -1
        ind_speed_passenger = -1
        ind_speed_heavyduty = -1
        if len(cols) >= 3:
            for col in cols.split(sep):
                if col == 'Detector':
                    ind_detector = ind_col
                elif col == 'Time':
                    ind_time = ind_col
                elif col == 'qPKW':
                    ind_flow_passenger = ind_col
                elif col == 'qLKW':
                    ind_flow_heavyduty = ind_col
                elif col == 'vPKW':
                    ind_speed_passenger = ind_col
                elif col == 'vLKW':
                    ind_speed_heavyduty = ind_col
                ind_col += 1
        else:
            return False

        for line in f.readlines():
            cols = line.split(sep)
            # print '    cols=',cols
            if len(cols) >= 3:
                ind_col = 0
                is_valid = True
                t = 0.0
                flow_passenger = -1
                flow_heavyduty = -1
                speed_passenger = -1
                speed_heavyduty = -1

                for col_raw in cols:
                    col = col_raw.strip()
                    if ind_col == ind_detector:
                        if ids_sumo.has_index(col):
                            id_sumo_detector = col
                        else:
                            is_valid = False

                    elif col.isdigit() & is_valid:
                        num = int(col)
                        if ind_col == ind_time:
                            t = num

                        elif ind_col == ind_flow_passenger:
                            flow_passenger = num

                        elif ind_col == ind_flow_heavyduty:
                            flow_heavyduty = num

                        elif ind_col == ind_speed_passenger:
                            speed_passenger = num

                        elif ind_col == ind_speed_heavyduty:
                            speed_heavyduty = num
                    else:
                        # invalid value
                        is_valid = False

                    ind_col += 1

                if is_valid:
                    # print '  t',t,t_start,t_end,t+t_offset>=t_start,t+t_offset<t_end
                    if (t+t_offset >= t_start) & (t+t_offset < t_end):
                        ids_detector.append(ids_sumo.get_id_from_index(id_sumo_detector))
                        times.append(t)
                        flows_passenger.append(flow_passenger)
                        flows_heavyduty.append(flow_heavyduty)
                        speeds_passenger.append(speed_passenger)
                        speeds_heavyduty.append(speed_heavyduty)

        # insert in database
        ids_measure = self.add_rows(
            ids_detector=ids_detector,
            timeperiods=times,
            flows_passenger=flows_passenger,
            flows_heavyduty=flows_heavyduty,
            speeds_passenger=speeds_passenger,
            speeds_heavyduty=speeds_heavyduty,
        )

        f.close()

        return ids_measure

    def turnflows_to_routes(self, is_clear_trips=True, is_export_network=True,
                            is_make_probabilities=True, cmloptions=None,):
        #  jtrrouter --flow-files=<FLOW_DEFS>
        # --turn-ratio-files=<TURN_DEFINITIONS> --net-file=<SUMO_NET> \
        # --output-file=MySUMORoutes.rou.xml --begin <UINT> --end <UINT>

        if is_make_probabilities:
            self.normalize_turnprobabilities()

        scenario = self.parent.get_scenario()
        if cmloptions is None:
            cmloptions = '-v --max-edges-factor 1  --seed 23423 --repair --ignore-vclasses false --ignore-errors --turn-defaults 5,90,5'

        trips = scenario.demand.trips
        if is_clear_trips:
            # clear all current trips = routes
            trips.clear_trips()

        rootfilepath = scenario.get_rootfilepath()
        netfilepath = scenario.net.get_filepath()
        flowfilepath = rootfilepath+'.flow.xml'
        turnfilepath = rootfilepath+'.turn.xml'

        routefilepath = trips.get_routefilepath()

        # first generate xml for net
        scenario.net.export_netxml()

        ids_mode = self.get_modes()
        print 'turnflows_to_routes', ids_mode  # scenario.net.modes.get_ids()
        print '  cmloptions', cmloptions

        # route for all modes and read in routes
        for id_mode in ids_mode:
            # write flow and turns xml file for this mode
            time_start, time_end = self.export_flows_and_turns(flowfilepath, turnfilepath, id_mode)
            print '  time_start, time_end =', time_start, time_end
            if time_end > time_start:  # means there exist some flows for this mode
                cmd = 'jtrrouter --route-files=%s --turn-ratio-files=%s --net-file=%s --output-file=%s --begin %s --end %s %s'\
                    % (P+flowfilepath+P,
                       P+turnfilepath+P,
                       P+netfilepath+P,
                       P+routefilepath+P,
                       time_start,
                       time_end,
                       cmloptions,
                       )
                # print '\n Starting command:',cmd
                if call(cmd):
                    if os.path.isfile(routefilepath):
                        trips.import_routes_xml(routefilepath, is_generate_ids=True)
                        os.remove(routefilepath)

            else:
                print 'jtrroute: no flows generated for id_mode', id_mode

        # self.simfiles.set_modified_data('rou',True)
        # self.simfiles.set_modified_data('trip',True)
        # trips and routes are not yet saved!!


class DetectorMatcher(Process):
    def __init__(self, ident, detectors,  logger=None, **kwargs):
        print 'DetectorMatcher.__init__'

        # TODO: let this be independent, link to it or child??

        self._init_common(ident,
                          parent=detectors,
                          name='Detector Matcher',
                          logger=logger,
                          info='Match detectors to lanes.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        net = detectors.get_scenario().net

        self.modename = attrsman.add(cm.AttrConf('modename', kwargs.get('modename', 'passenger'),
                                                 groupnames=['options'],
                                                 choices=net.modes.names.get_indexmap().keys(),
                                                 name='Mode name',
                                                 info='Matched lanes must be accessible at least for this mode.',
                                                 ))

        print 'net.modes.names.get_indexmap().keys()', net.modes.names.get_indexmap().keys(),  self.modename

        self.is_edgedetectors = attrsman.add(cm.AttrConf('is_edgedetectors', kwargs.get('is_edgedetectors', False),
                                                         groupnames=['options'],
                                                         name='One detector per edge',
                                                         info="""If True, it is assumed that there is only one detector per edge
                            and detected edge flows will be distributed accross accessible lanes according to some heuristics.
                            If False then each detector is matched to one lane. This is somewhat risky and requires a good 
                            geometrical coincidence between detector position and lane. 
                            """,
                                                         ))

        self.is_check_direction = attrsman.add(cm.AttrConf('is_check_direction', kwargs.get('is_check_direction', False),
                                                           groupnames=['options'],
                                                           name='Use detector direction',
                                                           info="""If True, use director direction information to improve matching.
                            """,
                                                           ))
        self.phidelta_max = attrsman.add(cm.AttrConf('phidelta_max', kwargs.get('phidelta_max', 10.0),
                                                     groupnames=['options'],
                                                     name='Max. angle diff.',
                                                     unit='deg',
                                                     info="""Maximum difference of angels between detector and nearest edge segment.
                            To be used in conjunction with "Use detector direction" """,
                                                     ))

        self.d_max = attrsman.add(cm.AttrConf('d_max', kwargs.get('d_max', 20.0),
                                              groupnames=['options'],
                                              name='Maximum search dist.',
                                              unit='m',
                                              info="""Maximum search distance around detectors.""",
                                              ))

        self.n_targetedge = attrsman.add(cm.AttrConf('n_targetedge', kwargs.get('n_targetedge', 5),
                                                     groupnames=['options'],
                                                     name='Number of target edges',
                                                     info="""Maximum number of considered target edges when searching for the nearest edge.
                            """,
                                                     ))

        self.d_max = attrsman.add(cm.AttrConf('d_max', kwargs.get('d_max', 20.0),
                                              groupnames=['options'],
                                              name='Maximum search dist.',
                                              unit='m',
                                              info="""Maximum search distance around detectors.""",
                                              ))

    def do(self):
        print 'DetectorMatcher.do'
        self.parent.match_detectors_to_lane(modename=self.modename,
                                            is_edgedetectors=self.is_edgedetectors,
                                            is_check_direction=self.is_check_direction,
                                            n_targetedge=self.n_targetedge,
                                            d_max=self.d_max,
                                            phidelta_max=self.phidelta_max)
        return True


class DFRouter(CmlMixin, Process):
    def __init__(self, detectorflows, logger=None, **kwargs):

        self._init_common('detectorflowrouter', name='Detectorflow Router',
                          parent=detectorflows,
                          logger=logger,
                          info='Generates routes from measured flows at detectors using the DFrouter.',
                          )

        self.init_cml('dfrouter')  # pass  no commad to generate options only

        attrsman = self.get_attrsman()

        self.add_option('timestep', kwargs.get('timestep', 3600),
                        groupnames=['options', 'processing'],
                        cml='--time-step',
                        name='Time step',
                        info='Expected distance between two successive data sets.',
                        untit='s',
                        )

        self.add_option('is_respect_concurrent_inflows', kwargs.get('is_respect_concurrent_inflows', False),
                        groupnames=['options', 'processing'],
                        cml='--respect-concurrent-inflows',
                        name='Respect concurrent inflows',
                        info='Try to determine further inflows to an inbetween detector when computing split probabilities.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('is_routes_for_all', kwargs.get('is_routes_for_all', False),
                        groupnames=['options', 'processing'],
                        cml='--routes-for-all',
                        name='Routes for inbetween detectors',
                        info='Forces DFROUTER to compute routes for in-between detectors.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        # self.add_option('is_guess_empty_flows', kwargs.get('is_guess_empty_flows',False),
        #                groupnames = ['processing'],#
        #                cml = '--guess-empty-flows',
        #                name = 'Guess empty flows',
        #                info = 'Derive missing flow values from upstream or downstream (not working!)',
        #                #is_enabled = lambda self: self.width_sublanes > 0,
        #                )

        # self.add_option('is_ignore_invalid_detectors', kwargs.get('is_ignore_invalid_detectors',False),
        #                groupnames = ['processing',],#
        #                cml = '--ignore-invalid-detectors',
        #                name = 'Ignore invalid detectors',
        #                info = 'Only warn about unparseable detectors.',
        #                #is_enabled = lambda self: self.width_sublanes > 0,
        #                )

        self.add_option('is_keep_unfinished_routes', kwargs.get('is_keep_unfinished_routes', False),
                        groupnames=['options', 'processing'],
                        cml='--keep-unfinished-routes',
                        name='Keep unfinished routes',
                        info='Keeps routes even if they have exhausted max-search-depth.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('is_keep_longer_routes', kwargs.get('is_keep_longer_routes', False),
                        groupnames=['options', 'processing'],
                        cml='--keep-longer-routes',
                        name='Keep longer routes',
                        info='eeps routes even if a shorter one exists.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('n_max_search_depth', kwargs.get('n_max_search_depth', 30),
                        groupnames=['options', 'processing'],
                        cml='--max-search-depth',
                        name='Max. search depth',
                        info='Number of edges to follow a route without passing a detector.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('is_keep_turnarounds', kwargs.get('is_keep_turnarounds', False),
                        groupnames=['options', 'processing'],
                        cml='--keep-turnarounds',
                        name='keep turnarounds',
                        info='Allow turnarounds as route continuations.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('length_min_route', kwargs.get('length_min_route', 500.0),
                        groupnames=['options', 'processing'],
                        cml='--min-route-length',
                        name='Min route length',
                        info='Minimum distance in meters between start and end node of every route.',
                        unit='m',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('is_randomize_flows', kwargs.get('is_randomize_flows', False),
                        groupnames=['options', 'processing'],
                        cml='--randomize-flows',
                        name='Randomize flows',
                        info='Generate random departure times for emitted vehicles.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('c_flows', kwargs.get('c_flows', 1.0),
                        groupnames=['options', 'processing'],
                        cml='--scale',
                        name='Flow scale factor',
                        info='Scale factor for flows.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('c_time', kwargs.get('c_time', 1.0),
                        groupnames=['options', 'processing'],
                        cml='--time-factor',
                        name='Time factor',
                        info='Multiply flow times with TIME to get seconds.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('time_offset', kwargs.get('time_offset', 0),
                        groupnames=['options', 'processing'],
                        cml='--time-offset',
                        name='Time offset',
                        info='Multiply flow times with TIME to get seconds.',
                        #is_enabled = lambda self: self.width_sublanes > 0,
                        )

        self.add_option('seed', 1234,
                        groupnames=['options', 'processing'],
                        cml='--seed',
                        name='Random seed',
                        info='Initialises the random number generator with the given value.',
                        )

        self.is_export_network = attrsman.add(am.AttrConf('is_export_network', True,
                                                          groupnames=['options', ],
                                                          perm='rw',
                                                          name='Export network',
                                                          info='Export network before routing.',
                                                          ))

        self.is_clear_trips = attrsman.add(am.AttrConf('is_clear_trips', True,
                                                       groupnames=['options'],
                                                       perm='rw',
                                                       name='Clear trips',
                                                       info='Clear all trips in current trips database before routing.',
                                                       ))

        # self.add_option('turnratiofilepath', turnratiofilepath,
        #                groupnames = ['_private'],#
        #                cml = '--turn-ratio-files',
        #                perm='r',
        #                name = 'Net file',
        #                wildcards = 'Net XML files (*.net.xml)|*.net.xml',
        #                metatype = 'filepath',
        #                info = 'SUMO Net file in XML format.',
        #                )

    def do(self):
        cmloptions = self.get_cml()
        scenario = self.parent.get_scenario()
        trips = scenario.demand.trips
        if self.is_clear_trips:
            # clear all current trips = routes
            trips.clear_trips()

        rootfilepath = scenario.get_rootfilepath()
        netfilepath = scenario.net.get_filepath()
        measuresfilepath = rootfilepath+'.measure.csv'
        detectorsfilepath = rootfilepath+'.detect.xml'
        emittersfilepath = rootfilepath+'.emitter.xml'

        self.parent.flowmeasurements.export_csv(measuresfilepath)
        self.parent.detectors.export_sumoxml(detectorsfilepath)

        routefilepath = trips.get_routefilepath()

        # first generate xml for net
        if self.is_export_network:
            scenario.net.export_netxml()

        print 'DFRouter.do'
        print '  cmloptions', cmloptions

        # dfrouter --net-file bonet190614_ms_dflows.net --routes-output bonet190614_ms_dflows.rou.xml --emitters-output vehicles.xml --detector-files detectors.xml --measure-files bonet190614_ms_dflows.dflows2.csv
        cmd = cmloptions + ' --net-file %s --detector-files %s --measure-files %s --routes-output %s --emitters-output %s'\
            % (P+netfilepath+P,
               P+detectorsfilepath+P,
               P+measuresfilepath+P,
               P+routefilepath+P,
               P+emittersfilepath+P,
               )
        # print '\n Starting command:',cmd
        if call(cmd):
            if os.path.isfile(routefilepath):
                scenario.demand.trips.import_routes_xml(routefilepath)
                os.remove(routefilepath)
                return True
            else:
                return False
        else:
            return False


class FlowsImporter(Process):
    def __init__(self, ident='flowsimporter', flows=None,
                 name='Flows Importer',
                 info='Reads and imports flow measurements from detectors.',
                 filepath='', logger=None, **kwargs):
        demand = flows.parent.parent

        self._init_common(ident, name=name,
                          parent=flows,
                          logger=logger,
                          info=info,
                          )

        attrsman = self.get_attrsman()

        self.t_start = attrsman.add(am.AttrConf('t_start', 0,
                                                groupnames=['options'],
                                                perm='rw',
                                                name='Start time',
                                                unit='s',
                                                info='Start reading detector data at this time.',
                                                ))

        self.t_end = attrsman.add(am.AttrConf('t_end', 24*3600,
                                              groupnames=['options'],
                                              perm='rw',
                                              name='End time',
                                              unit='s',
                                              info='Stop reading detector data at this time.',
                                              ))

        self.t_offset = attrsman.add(am.AttrConf('t_offset', 0,
                                                 groupnames=['options'],
                                                 perm='rw',
                                                 name='Time offset',
                                                 unit='s',
                                                 info='Time offset added to the time of the measurement values prior to the verification of the time window.',
                                                 ))

        # here we get currently available vehicle classes not vehicle type
        # specific vehicle type within a class will be generated later
        self.id_mode = attrsman.add(am.AttrConf('id_mode', MODES['passenger'],
                                                groupnames=['options'],
                                                choices=demand.vtypes.get_modechoices(),
                                                name='ID mode',
                                                info='ID of transport mode.',
                                                ))

        self.filepath = attrsman.add(am.AttrConf('filepath', filepath,
                                                 groupnames=['options'],  # this will make it show up in the dialog
                                                 perm='rw',
                                                 name='Flow file',
                                                 wildcards="Flow CSV files (*.csv)|*.csv|CSV files (*.txt)|*.txt|All files (*.*)|*.*",
                                                 metatype='filepath',
                                                 info='CSV file with flow measurements from detectors for the specific mode and time interval.',
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
        # self.update_params()
        if os.path.isfile(self.filepath):
            ids_measure = self.parent.import_csv(self.filepath,
                                                 t_offset=self.t_offset,
                                                 t_start=self.t_start,
                                                 t_end=self.t_end,
                                                 sep=";")
            return True
        else:
            return False


class DetectorsFromShapeImporter(ShapefileImporter):
    def __init__(self, ident='detectorsfromshapeimporter', detectors=None,
                 name='Detectors from Shapefile importer',
                 filepath='', logger=None, **kwargs):

        net = detectors.parent.parent.get_net()
        ShapefileImporter.__init__(self, ident, detectors,
                                   name=name,
                                   filepath=filepath,
                                   coordsattr='coords',
                                   attrnames_to_shapeattrs={'ids_sumo': 'ID',
                                                            'directions': '',
                                                            'names': '',
                                                            },
                                   projparams_target=net.get_projparams(),
                                   offset=net.get_offset(),
                                   boundaries=net.get_boundaries(is_netboundaries=True),
                                   logger=logger,
                                   **kwargs)
