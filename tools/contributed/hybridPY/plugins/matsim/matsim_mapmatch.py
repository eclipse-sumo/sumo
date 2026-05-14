# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    matsim_mapmatch.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

from .matsim_base import *
import math
import matplotlib.pyplot as plt
from xml.sax import saxutils, parse, handler
import os
import sys
import shutil
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.processes import Process
from xml.sax import parse, handler
import sys
import os
BASE_DIR = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, os.path.dirname(__file__))

try:
    import networkx as nx
    import geonetworkx as gnx
except:
    print('Matsim mapmatching cannot be used. Please install networkx and geonetworkx')


try:
    import pyproj
except:
    from mpl_toolkits.basemap import pyproj

pathsep = os.path.sep
# from coremodules.network.network import SumoIdsConf


class mapmatch(am.ArrayObjman):
    def __init__(self, parent=None, name='mapmatch', **kwargs):
        print('mapmatch', parent, name)
        self._init_objman(ident='mapmatch', parent=parent, name=name,
                          # xmltag = 'net',# no, done by netconvert
                          version=0.1,
                          **kwargs)
        self._init_attributes()

    def _init_attributes(self):

        self.add_col(am.ArrayConf('ids_matsim', default='',
                                  dtype=object,
                                  perm='r',
                                  is_index=True,
                                  name='id_matsim',
                                  info='Node ID from MATSim',
                                  xmltag='id',
                                  ))
        self.add_col(am.ListArrayConf('ids_sumo',
                                      groupnames=['parameters'],
                                      name='ids_sumo',
                                      info='Sequence of SUMO IDs corresponding to MATSIM ID',
                                      ))


class MapMatcher(Process):
    def __init__(self,  matsim, ids_mode=None,
                 logger=None, **kwargs):
        print('PopulationImporter.__init__', matsim)
        self._init_common('MapMatcher', name='MATSIM SUMO MapMatcher',
                          logger=logger,
                          info='Matches MATSim and SUMO Simulation Graphs',
                          )

        self._matsim = matsim
        self._scenario = matsim.get_scenario()
        self._net = self._scenario.net
        rootname = self._scenario.get_rootfilename()
        rootdirpath = self._scenario.get_workdirpath()

        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.search_ortho = attrsman.add(cm.AttrConf('search_ortho', False,
                                                     groupnames=['options'],
                                                     name='search_ortho',
                                                     info='Search for vectors using orthogonal projection',
                                                     ))
        self.use_shape_for_matching = attrsman.add(cm.AttrConf('use_shape_for_matching', True,
                                                               groupnames=['options'],
                                                               name='use_shape_for_matching',
                                                               info='Use the shapes of the vector instead of the nodes ',
                                                               ))
        self.radius_start = attrsman.add(cm.AttrConf('radius_start', 4,
                                                     groupnames=['options'],
                                                     name='radius_start',
                                                     info='Search radius for SUMO vectors',
                                                     ))
        self.cosine_threshold = attrsman.add(cm.AttrConf('cosine_threshold', 0.95,
                                                         groupnames=['options'],
                                                         name='cosine_threshold',
                                                         info='Cosine Similarity threshold',
                                                         ))
        self.ortho_dist = attrsman.add(cm.AttrConf('ortho_dist', 20,
                                                   groupnames=['options'],
                                                   name='ortho_dist',
                                                   info='Allowed orthogonal distance',
                                                   ))
        self.ortho_cosine_threshold = attrsman.add(cm.AttrConf('ortho_cosine_threshold', 0.9,
                                                               groupnames=['options'],
                                                               name='ortho_cosine_threshold',
                                                               info='Cosine Similarity threshold during orthogonal projection',
                                                               ))
        self.ortho_tol = attrsman.add(cm.AttrConf('ortho_tol', 0.2,
                                                  groupnames=['options'],
                                                  name='ortho_tol',
                                                  info='Allowed differenze in orthogonal projection',
                                                  ))

        self.tol_node = attrsman.add(cm.AttrConf('tol_node', 1,
                                                 groupnames=['options'],
                                                 name='tol_node',
                                                 info='Difference in incoming / out-going edges',
                                                 ))
        self.min_length = attrsman.add(cm.AttrConf('min_length', 30,
                                                   groupnames=['options'],
                                                   name='min_length',
                                                   info='Minimal length of a MATSim Edge to be matched',
                                                   ))

    def do(self):
        self.match_maps(self._matsim.mapmatch, self._scenario.net, self._matsim.net)
        return True

    # def orthogonal_distance_and_check_projection(self, point, start_point, end_point):
    #     # Vektor vom Startpunkt zum Endpunkt
    #     line_vector = end_point - start_point
    #     # Vektor vom Startpunkt zum Punkt
    #     point_vector = point - start_point
    #     # Orthogonaler Abstand berechnen
    #     orthogonal_distance = np.abs(np.cross(line_vector, point_vector)) / np.linalg.norm(line_vector)
    #     # Projektierter Punkt auf der Linie
    #     projected_point = start_point + np.dot(point_vector, line_vector) / np.dot(line_vector, line_vector) * line_vector
    #     # Überprüfen, ob der projizierte Punkt zwischen Start- und Endpunkt liegt
    #     is_between = np.all(np.logical_and(projected_point >= start_point, projected_point <= end_point))
    #     return orthogonal_distance, projected_point, is_between

    def find_nearest_neighbor_strict(self, vector_set_id, sumonetwork, start_point, end_point, vector_set_start, vector_set_end, vector_prev, vector_after, target_prev, target_after, search_ortho=False, radius_start=5, cosine_threshold=0.8, ortho_dist=5, ortho_cosine_threshold=0.8, ortho_tol=0.01, tol_node=1, min_length=30, use_shapes=False):

        if np.linalg.norm(end_point - start_point) > min_length:

            # Suche innerhalb der Knoten -> Eingrenzung
            # Calculate Euclidean distances
            euclidean_distances = np.linalg.norm(vector_set_start - start_point, axis=1)
            indices_of_lowest = np.argsort(euclidean_distances)

            # Cosine similarities
            vector_set = vector_set_end - vector_set_start
            target_vector = end_point - start_point

            dot_product = np.dot(vector_set, target_vector)
            norm_vector = np.linalg.norm(target_vector)
            norm_array = np.linalg.norm(vector_set, axis=1)
            cosine_similarities = dot_product / (norm_vector * norm_array)

            counter = 0
            return_index = None
            stop_searching = False

            indices_less_than = np.where(euclidean_distances < radius_start)[0]

            modified_similarities = np.zeros_like(euclidean_distances) - 1

            modified_similarities[indices_less_than] = cosine_similarities[indices_less_than]

            sorted_results = np.argsort(modified_similarities)[::-1]

            index = 0

            while modified_similarities[sorted_results[index]] > 0:

                # hat ein Knoten zwei ähnliche Ausgänge? -> Ich erwarte eine eindeutige Zuordnung
                # Kleiner Suchradius (circa eine Straßenbreite) und eine kleine Cosine_Similarity -> Hier ist nun die Vektorlänge egal -> Wenn es mehrere Lösungen gibt, dann wird diese Ignoriert!

                if euclidean_distances[sorted_results[index]] < radius_start and cosine_similarities[sorted_results[index]] > cosine_threshold and (abs(vector_prev[sorted_results[index]]-target_prev) <= tol_node or vector_prev[sorted_results[index]] == -1):

                    if return_index == None:

                        return_index = sorted_results[index]

                    else:

                        return_index = 'double-match'

                index += 1
            # verifiziere Ergebnis!
            # if return_index != None:
            #    result = sumonetwork.edges.get_closest_edge(start_point + 0.5 * (end_point - start_point))

            #    if vector_set_id[return_index] == result[0][0]:
            #        print('verified')
            #    else:
            #        print('error')
            #        return_index = None
        # Suche innerhalb der Geometrie
        # ToDo: Avoid double matches! -> Done!
        # ToDo: Double Matches
            if return_index == None and use_shapes:
                ignore = False
                result_02 = sumonetwork.edges.get_closest_edge(
                    start_point + 0.2 * (end_point - start_point), d_max=ortho_dist, n_best=20)

                comp_result_02 = None
                index = 0

                vector_set_id = list(vector_set_id)

                while len(result_02[0]) > index:
                    try:
                        cosine = cosine_similarities[vector_set_id.index(result_02[0][index])]
                    except:
                        cosine = -1
                    if cosine > ortho_cosine_threshold and result_02[1][index] < ortho_dist:
                        if comp_result_02 == None:
                            comp_result_02 = result_02[0][index]
                        else:
                            ignore = True
                    index += 1

                result_04 = sumonetwork.edges.get_closest_edge(
                    start_point + 0.4 * (end_point - start_point), d_max=ortho_dist, n_best=20)

                comp_result_04 = None
                index = 0

                while len(result_04[0]) > index:
                    try:
                        cosine = cosine_similarities[vector_set_id.index(result_04[0][index])]
                    except:
                        cosine = -1
                    if cosine > ortho_cosine_threshold and result_04[1][index] < ortho_dist:
                        if comp_result_04 == None:
                            comp_result_04 = result_04[0][index]
                        else:
                            ignore = True
                    index += 1

                result_06 = sumonetwork.edges.get_closest_edge(
                    start_point + 0.6 * (end_point - start_point), d_max=ortho_dist, n_best=20)

                comp_result_06 = None
                index = 0

                while len(result_06[0]) > index:

                    try:
                        cosine = cosine_similarities[vector_set_id.index(result_06[0][index])]
                    except:
                        cosine = -1

                    if cosine > ortho_cosine_threshold and result_06[1][index] < ortho_dist:

                        if comp_result_06 == None:
                            comp_result_06 = result_06[0][index]
                        else:
                            ignore = True

                    index += 1

                if comp_result_02 == comp_result_04 and comp_result_04 == comp_result_06 and comp_result_04 != None and ignore == False:
                    return_index = comp_result_02
                    try:
                        return_index = vector_set_id.index(comp_result_02)
                    except:
                        return_index = None

                if ignore:
                    return_index = 'double-match'

            if return_index == None and search_ortho:

                return_index_list = []
                num_candidates = 0
                vector_set = vector_set_end - vector_set_start
                point_vector = start_point - vector_set_start
                orthogonal_distance = np.linalg.norm(
                    np.cross(vector_set, point_vector, axis=1), axis=1) / np.linalg.norm(vector_set, axis=1)

                indices_of_lowest = np.argsort(orthogonal_distance)

                counter = 0
                return_index = None
                stop_searching = False

                # Wir bneötigen eine ein-eindeutige Zuordnung! Könnte der Startpunkt auf mehreren Vektoren liegen -> ignoriere ihn!

                while counter < len(indices_of_lowest) and not (stop_searching):
                    # kleinste Distanz zu erst
                    index = indices_of_lowest[counter]
                    counter += 1
                    if (orthogonal_distance[index] < ortho_dist) and (cosine_similarities[index] > ortho_cosine_threshold):

                        dot_product = np.dot(point_vector[index], vector_set[index])
                        mag_squared_u = np.dot(vector_set[index], vector_set[index])

                        # Calculate the projection
                        projection = (dot_product / mag_squared_u) * vector_set[index]

                        # Check if the projection point lies between the start and end points of u
                        # Distanz = Distanz to Start
                        distance = np.linalg.norm(projection)
                        ref_distance = np.linalg.norm(vector_set[index])

                        dot_product = np.dot(vector_set[index], projection)
                        distance_vec = np.linalg.norm(end_point - start_point)
                        norm_vector = np.linalg.norm(projection)
                        norm_array = np.linalg.norm(vector_set[index])
                        cosine_similarities_einzel = dot_product / (norm_vector * norm_array)

                        # Vektor ragt hinten raus
                        # if ((cosine_similarities_einzel > 0.95 and (distance+ distance_vec) < (1.0 + ortho_tol) *ref_distance) or (cosine_similarities_einzel < -0.95 and distance < ortho_tol*ref_distance)) and (abs(vector_prev[index]-target_prev)<=tol_node or vector_prev[index] == -1) and (abs(vector_after[index]-target_after)<=tol_node or vector_after[index] == -1):
                        #    return_index = index
                        #    stop_searching = True
                        # if (cosine_similarities_einzel < -0.95 and distance < ortho_tol*ref_distance) and (abs(vector_prev[index]-target_prev)<=tol_node or vector_prev[index] == -1):
                        #    return_index = index
                        #    stop_searching = True

                        if ((cosine_similarities_einzel > 0.95 and (distance) < ref_distance)) or ((cosine_similarities_einzel < -0.95 and distance < 0.2*ref_distance)):
                            num_candidates += 1
                        if (cosine_similarities_einzel > 0.95 and (distance + distance_vec) < ref_distance):
                            return_index_list.append(index)
                        elif (cosine_similarities_einzel > 0.95 and (distance + distance_vec) < (1.0 + ortho_tol) * ref_distance) and (abs(vector_after[index]-target_after) <= tol_node or vector_after[index] == -1):
                            return_index_list.append(index)

                    elif orthogonal_distance[index] > ortho_dist:
                        if euclidean_distances[0] > 50:
                            print("Matching not possible!")
                        # print("No match found for vector")
                        stop_searching = True
                if num_candidates == 1 and len(return_index_list) == 1:
                    return_index = return_index_list[0]

            return return_index

        return None

    def np_graph_matsim(self, nodes, edges, boundaries=None, x_border=0, y_border=0):
        # .get_ids()
        # .get_row(id)['ids_fromnode']
        # ['coords'][0]
        # ['ids_fromnode']

        # ['ids_tonode']

        # ACHTUNG: Hier ist es wichtig auf die Modes zu achten!
        relevant_nodes = []
        vector_set_start = []
        vector_set_end = []
        vector_set_id = []
        vector_prev = []
        vector_after = []
        for id in nodes.get_ids():
            if ((boundaries[2]-x_border >= nodes.get_row(id)['coords'][0]) & (boundaries[0]+x_border <= nodes.get_row(id)['coords'][0]) & (boundaries[3]-y_border >= nodes.get_row(id)['coords'][1]) & (boundaries[1]+y_border <= nodes.get_row(id)['coords'][1])):
                relevant_nodes.append(id)
        for id in edges.get_ids():
            if edges.get_row(id)['ids_fromnode'] in relevant_nodes or edges.get_row(id)['ids_tonode'] in relevant_nodes:
                vector_set_start.append(nodes.get_row(edges.get_row(id)['ids_fromnode'])['coords'])
                vector_set_end.append(nodes.get_row(edges.get_row(id)['ids_tonode'])['coords'])
                vector_set_id.append(id)
                # Anzahl an Edges, welche in den Vektor führen
                vector_prev.append(len(edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])))
                vector_after.append(len(edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])))
        return np.array(vector_set_start), np.array(vector_set_end), np.array(vector_set_id), np.array(vector_prev), np.array(vector_after)

    def np_graph_sumo(self, nodes, edges, filter=True):
        # ACHTUNG: Hier ist es wichtig auf die Modes zu achten!
        vector_set_start = []
        vector_set_end = []
        vector_set_id = []
        vector_prev = []
        vector_after = []
        for id in edges.get_ids():
            if filter != None:
                if filter.get(edges.get_row(id)['types']):
                    # g.add_edge(str(edges.get_row(id)['ids_fromnode']),str(edges.get_row(id)['ids_tonode']),weight=edges.get_row(id)['lengths'],id=str(id))
                    # node_list.append(edges.get_row(id)['ids_fromnode'])
                    # node_list.append(edges.get_row(id)['ids_tonode'])

                    # Achtung: shapes der vektoren und Knoten / Kanten varrieren stark!

                    vector_set_start.append(nodes.get_row(edges.get_row(id)['ids_fromnode'])['coords'])
                    vector_set_end.append(nodes.get_row(edges.get_row(id)['ids_tonode'])['coords'])

                    vector_set_id.append(id)

                    in_coming = []
                    in_coming = edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])
                    # Detect fringe edges!
                    if len(in_coming) == 0:
                        vector_prev.append(-1)
                    elif len(in_coming) == 1 and edges.get_row(in_coming[0])['ids_fromnode'] == edges.get_row(id)['ids_tonode']:
                        vector_prev.append(-1)
                    else:
                        vector_prev.append(len(in_coming))

                    out_going = []
                    out_going = edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])
                    # Detect fringe edges!
                    if len(out_going) == 0:
                        vector_after.append(-1)
                    elif len(out_going) == 1 and edges.get_row(out_going[0])['ids_tonode'] == edges.get_row(id)['ids_fromnode']:
                        vector_after.append(-1)
                    else:
                        vector_after.append(len(out_going))

            else:

                vector_set_start.append(nodes.get_row(edges.get_row(id)['ids_fromnode'])['coords'])
                vector_set_end.append(nodes.get_row(edges.get_row(id)['ids_tonode'])['coords'])

                vector_set_id.append(id)

                vector_prev.append(len(edges.select_ids(edges.ids_tonode.value == edges.get_row(id)['ids_fromnode'])))
                vector_after.append(len(edges.select_ids(edges.ids_fromnode.value == edges.get_row(id)['ids_tonode'])))

        return np.array(vector_set_start), np.array(vector_set_end), np.array(vector_set_id), np.array(vector_prev), np.array(vector_after)

    def match_maps(self, mapmatch, sumonetwork, matsimnetwork):
        # Generate graphs
        print("Generating Graphs")
        # Sumo Road Types
        roadTypes = {'highway.cycleway': False,  # NO
                     'highway.path': False,  # NO
                     'highway.living_street': False,  # YES
                     'highway.residential': True,  # YES
                     'highway.trunk': True,  # YES
                     'highway.service': False,  # NO
                     'highway.primary': True,  # YES
                     'highway.secondary': True,  # NO
                     'highway.tertiary': True,  # NO
                     'highway.primary_link': True,  # Yes
                     'highway.secondary_link': False,  # NO
                     'highway.motorway': True,  # Yes
                     'highway.motorway_link': True,  # YES
                     'highway.trunk_link': True,  # YES
                     'highway.unclassified': True,  # YES
                     'highway.track': False,  # NO
                     'railway.tram': False,
                     '': False,  # NO
                     }

        vector_set_start, vector_set_end, vector_set_id, vector_prev, vector_after = self.np_graph_sumo(
            sumonetwork.nodes, sumonetwork.edges, filter=roadTypes)

        target_set_start, target_set_end, target_set_id, target_set_prev, target_set_after = self.np_graph_matsim(
            matsimnetwork.nodes, matsimnetwork.edges, sumonetwork.get_boundaries(is_netboundaries=True))

        edge_map_id = {}

        counter_none = 0
        counter_abs = 0

        for target_start, target_end, target_id, target_prev, target_after in zip(target_set_start, target_set_end, target_set_id, target_set_prev, target_set_after):
            index = self.find_nearest_neighbor_strict(vector_set_id, sumonetwork, target_start, target_end, vector_set_start, vector_set_end, vector_prev, vector_after, target_prev, target_after,
                                                      self.search_ortho, self.radius_start, self.cosine_threshold, self.ortho_dist, self.ortho_cosine_threshold, self.ortho_tol, self.tol_node, self.min_length, self.use_shape_for_matching)

            if index != None and index != 'double-match':
                edge_map_id[target_id] = vector_set_id[index]
                counter_abs += 1
            else:
                print(target_id)
                counter_none += 1

        print("Insgesamt waren folgende vorgesehen:" + str(counter_abs + counter_none) + "; nicht gematcht: " +
              str(counter_none) + "nicht gemacht in Prozent: " + str(counter_none / (counter_abs + counter_none)))
        print(counter_none)

        mapmatch.clear_rows()

        for element in edge_map_id:
            element1 = np.zeros(1, np.int64)
            element2 = np.zeros(1, object)

            element1[0] = element
            element2[0] = edge_map_id[element]

            mapmatch.add_rows(None,
                              ids_matsim=element1,
                              ids_sumo=element2,
                              )

        # print(edge_map)
        # print("...    matched edges:", len(edge_map), "/", len(matsim_red.edges), '->',  round(len(edge_map)/len(matsim_red.edges)*100,2), '%')

    """ def match_maps_obsolete(mapmatch,workdir,sumonetwork,matsimnetwork,tolerance = 10):
        #Generate graphs
        print("Generating Graphs")
        #Sumo Road Types
        roadTypes = {'highway.cycleway' : False, #NO
                    'highway.path' : False, #NO
                    'highway.living_street' : False, #YES
                    'highway.residential' : True, #YES
                    'highway.trunk' : True, #YES
                    'highway.service' : False, #NO 
                    'highway.primary' : True, #YES
                    'highway.secondary' : True, #NO
                    'highway.tertiary' : True, #NO
                    'highway.primary_link' : True, #Yes
                    'highway.secondary_link' : False, #NO
                    'highway.motorway' : True, #Yes
                    'highway.motorway_link' : True, #YES
                    'highway.trunk_link' : True, #YES
                    'highway.unclassified' : True, #YES
                    'highway.track' : False, #NO
                    'railway.tram' : False,
                    '':False, #NO
                    }
        sumo = self.graph(sumonetwork.nodes,sumonetwork.edges,filter=roadTypes)
        
        matsim = self.graph(matsimnetwork.nodes,matsimnetwork.edges,sumonetwork.get_boundaries(is_netboundaries = True))

        plot = False
        if plot:
            print("... ploting original graphs")
            sumo_g = plt.figure()
            ax = sumo_g.add_subplot(1,1,1)
            sumo_g = plot_Graph(G = sumo, label='sumo', fig=sumo_g, node_color='blue', node_size = 5, width = 0.5, \
                            edge_color='blue', style='dashed', with_labels=False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_1.png', dpi=1000, bbox_inches='tight')

        if plot:
            matsim_g = plt.figure()
            ax = matsim_g.add_subplot(1,1,1)
            matsim_g = plot_Graph(G=matsim, label='matsim', fig=matsim_g, node_color='red', node_size = 5, width = 0.5,\
                            edge_color='red', style='solid',with_labels = False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_2.png', dpi=1000, bbox_inches='tight')

        if plot:
            overlay_g = plt.figure()
            ax = overlay_g.add_subplot(1,1,1)
            overlay_g = plot_Graph(G = sumo, label='sumo', fig=sumo_g, node_color='blue', node_size = 5, width = 0.5, \
                            edge_color='blue', style='dashed', with_labels=False)
            overlay_g = plot_Graph(G=matsim, label='matsim', fig=overlay_g, node_color='red', node_size = 5, width = 0.5,\
                            edge_color='red', style='solid',with_labels = False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_3.png', dpi=1000, bbox_inches='tight')

        #Clean up matsim borders
        print("Cleaning up matsim borders")
        cleaning = False 
        if cleaning:
            clean_up_borders(matsim=matsim, sumo=sumo)

        if plot:
            overlay_clean_border = plt.figure()
            ax = overlay_clean_border.add_subplot(1,1,1)
            overlay_clean_border = plot_Graph(G = sumo, label='sumo', fig=sumo_g, node_color='blue', node_size = 5, width = 0.5, \
                            edge_color='blue', style='dashed', with_labels=False)
            overlay_clean_border = plot_Graph(G=matsim, label='matsim', fig=overlay_g, node_color='red', node_size = 5, width = 0.5,\
                            edge_color='red', style='solid',with_labels = False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_4.png', dpi=1000, bbox_inches='tight')

        #Reduced graph
        print("Reducing Graphs")
        if cleaning:
            sumo_red, sumo_merged_edges = reduce(sumo)
            matsim_red, matsim_merged_edges = reduce(matsim)
        else:
            sumo_red = sumo
            matsim_red = matsim
        
        if plot:
            print("... ploting reduced graphs")
            sumo_g_red = plt.figure()
            ax = sumo_g_red.add_subplot(1,1,1)
            sumo_g_red = plot_Graph(G = sumo_red, label='sumo', fig=sumo_g, node_color='blue', node_size = 5, width = 0.5, \
                            edge_color='blue', style='dashed', with_labels=False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_5.png', dpi=1000, bbox_inches='tight')

        if plot:
            matsim_g_red = plt.figure()
            ax = matsim_g_red.add_subplot(1,1,1)
            matsim_g_red = plot_Graph(G=matsim_red, label='matsim', fig=matsim_g, node_color='red', node_size = 5, width = 0.5,\
                            edge_color='red', style='solid',with_labels = False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_6.png', dpi=1000, bbox_inches='tight')

        if plot:
            overlay_g_red = plt.figure()
            ax = overlay_g_red.add_subplot(1,1,1)
            overlay_g_red = plot_Graph(G = sumo_red, label='sumo', fig=sumo_g, node_color='blue', node_size = 5, width = 0.5, \
                            edge_color='blue', style='dashed', with_labels=False)
            overlay_g_red = plot_Graph(G=matsim_red, label='matsim', fig=overlay_g, node_color='red', node_size = 5, width = 0.5,\
                            edge_color='red', style='solid',with_labels = False)
            ax.legend(loc='center right')
            plt.savefig(workdir + 'plot_7.png', dpi=1000, bbox_inches='tight')

        #Match nodes
        print("Matching Graphs")
        node_map, tol = match_nodes(matsim=matsim_red, sumo=sumo_red, tolerance=tolerance)
        print("...    matched nodes:", len(node_map), "/", len(matsim_red.nodes),'->',  round(len(node_map)/len(matsim_red.nodes)*100,2), '%')

        count = 0
        for key, value in node_map.items():
            if "+" in value:
                count += 1
        print("...    multiple matches:", count)

        #Match edges
        print("... matching edges")
        edge_map, edge_map_id = match_edges(matsim_red, sumo_red, node_map)
        
        mapmatch.clear_rows()

        for element in edge_map_id:
            if len(element.split('+')) ==1:
                element1 = np.zeros(len(element.split('+')),np.int64)
                element2 = np.zeros(len(element.split('+')),object)
                for i in range(len(element.split('+'))):
                    if edge_map_id[element] != '':
                        element1[i] = int(element.split('+')[i])
                        element2[i] = np.array(list(map(int, edge_map_id[element].split('+'))))

            mapmatch.add_rows(  None,
                                ids_matsim = element1,
                                ids_sumo = element2,
                                )


        print(edge_map)
        print("...    matched edges:", len(edge_map), "/", len(matsim_red.edges), '->',  round(len(edge_map)/len(matsim_red.edges)*100,2), '%')

        #Plot graphs
        if plot:
            print("Ploting final graphs")
            matched_overlay = plt.figure()
            ax = matched_overlay.add_subplot(1,1,1)
            matched_overlay = plot_Graph(G = sumo_red, label='sumo', fig=matched_overlay, node_color='blue', node_size = 5, width = 0.5, \
                            edge_color='blue', style='dashed', with_labels=False)

            matched_overlay = plot_Graph(G=matsim_red, label='matsim', fig=matched_overlay, node_map=node_map, node_size = 5, width = 0.5,\
                            edge_map=edge_map, style='solid',with_labels = False)
            ax.legend(loc='center right')
            plt.savefig(workdir + '_' + str(tol) + 'plot_8.png', dpi=1000, bbox_inches='tight')

    def graph(nodes, edges, boundaries = None,x_border = 1000, y_border = 1000, filter = True):
        g = nx.DiGraph()
        # .get_ids()
        #.get_row(id)['ids_fromnode']
        #['coords'][0]
        #['ids_fromnode']
        #['ids_tonode']

        #ACHTUNG: Hier ist es wichtig auf die Modes zu achten!


        if boundaries == None:
            node_list = []
            for id in edges.get_ids():
                if filter != None:
                    if filter.get(edges.get_row(id)['types']):
                        g.add_edge(str(edges.get_row(id)['ids_fromnode']),str(edges.get_row(id)['ids_tonode']),weight=edges.get_row(id)['lengths'],id=str(id))
                        node_list.append(edges.get_row(id)['ids_fromnode'])
                        node_list.append(edges.get_row(id)['ids_tonode'])
                else:
                    g.add_edge(str(edges.get_row(id)['ids_fromnode']),str(edges.get_row(id)['ids_tonode']),weight=edges.get_row(id)['lengths'],id=str(id))
            if filter != None:
                node_list = list(set(node_list))
                for id in node_list:
                    g.add_node(str(id), pos=(nodes.get_row(id)['coords'][0], nodes.get_row(id)['coords'][1]))
            else:
                for id in nodes.get_ids():
                    g.add_node(str(id), pos=(nodes.get_row(id)['coords'][0], nodes.get_row(id)['coords'][1]))
            
        else:
            relevant_nodes = []
            for id in nodes.get_ids():
                if ((boundaries[2]-x_border >= nodes.get_row(id)['coords'][0]) & (boundaries[0]+x_border <= nodes.get_row(id)['coords'][0])&(boundaries[3]-y_border >= nodes.get_row(id)['coords'][1]) & (boundaries[1]+y_border <= nodes.get_row(id)['coords'][1])):
                    g.add_node(str(id), pos=(nodes.get_row(id)['coords'][0], nodes.get_row(id)['coords'][1]))
                    relevant_nodes.append(id)
            for id in edges.get_ids():
                if edges.get_row(id)['ids_fromnode'] in relevant_nodes or edges.get_row(id)['ids_tonode'] in relevant_nodes:
                    g.add_edge(str(edges.get_row(id)['ids_fromnode']),str(edges.get_row(id)['ids_tonode']),weight=edges.get_row(id)['lengths'],id=str(id))
        return g
        
    def reduce(graph):
        #Make a copy
        G = graph.copy(as_view=False)

        #Transform graph in geonetwork graph
        G = gnx.parse_graph_as_geograph(G)

        #Order lines
        gnx.order_well_lines(G)

        #Merge Nodes
        merged_edges = gnx.two_degree_node_merge_for_directed_graphs(G)

        G = nx.DiGraph(G)

        #Add ids reduced graph
        for edge in G.edges:
            try:
                G.edges[edge]['id'] = graph.edges[edge]['id']
                G.edges[edge]['weight'] = graph.edges[edge]['weight']

            except:
                value = merged_edges[edge]
                id1 = graph[value[0][0]][value[0][1]]['id']
                id2 = graph[value[1][0]][value[1][1]]['id']
                length1 = graph[value[1][0]][value[1][1]]['weight']
                length2 = graph[value[1][0]][value[1][1]]['weight']

                G.edges[edge]['id'] = str(id1)+ "+" + str(id2)
                G.edges[edge]['weight'] = length1+length2
                
        return G, merged_edges

    def clean_up_borders(matsim, sumo):
        node_map, tol = match_nodes(matsim=matsim, sumo=sumo, tolerance=40)

        for node in list(matsim.nodes):
            if not node_map.get(node):
                print('... removing', node)
                matsim.remove_node(node)
                
        
        return matsim



    def match_edges(matsim, sumo, matched_nodes):
        #Match 1 to 1
        matched_edges, matched_edges_id = match_1to1(matsim=matsim, sumo=sumo, matched_nodes=matched_nodes)
        print("...    matched edges 1-to-1:", len(matched_edges), "/", len(matsim.edges), '->',  round(len(matched_edges)/len(matsim.edges)*100,2), '%')

        #Match remaining edges based on shortest path
        matched_edges, matched_edges_id = match_shortest_paths(matsim=matsim, sumo=sumo, node_map=matched_nodes, edge_map=matched_edges, edge_map_id=matched_edges_id)

        #Save dictionary
        #with open(config.edge_map, 'w', encoding="utf-8") as fp:
        #    json.dump(matched_edges_id, fp)

        return matched_edges, matched_edges_id

    def match_1to1(matsim, sumo, matched_nodes, matched_edges={}, matched_edges_id={}):
        for me in matsim.edges:
            m_f, m_t = me
            s_f = matched_nodes.get(m_f)
            s_t = matched_nodes.get(m_t)

            if s_f is not None and s_t is not None:
                s_f = s_f.split('+')
                s_t = s_t.split('+')
                for f in s_f:
                    for t in s_t:
                        try:
                            s_id = sumo.get_edge_data(f,t)['id']
                        except:
                            #print("... no edge in sumo map for matsim edge:", me)
                            pass
                        else:
                            matched_edges[me] = (f,t)
                            m_id = matsim.get_edge_data(m_f,m_t)['id']
                            matched_edges_id[m_id] = s_id

        return matched_edges, matched_edges_id

    def match_shortest_paths(matsim, sumo, node_map, edge_map, edge_map_id):
        matsim_red = matsim.copy(as_view=False)
        sumo_red = sumo.copy(as_view=False)

        filtered_matched_nodes = removed_matched_edges(matsim_red, sumo_red, node_map, edge_map)
        
        for from_node_matsim in filtered_matched_nodes.keys():
            matsim_paths = nx.single_source_shortest_path(matsim,from_node_matsim,5)
            matsim_paths = {k: v for k, v in matsim_paths.items() if k in node_map}
            matsim_paths.pop(from_node_matsim, None)
            if matsim_paths:
                for to_node_matsim, matsim_path in matsim_paths.items():

                    from_node_sumo = node_map[from_node_matsim]
                    to_node_sumo = node_map[to_node_matsim]
                    
                    try:
                        sumo_path = nx.shortest_path(sumo, from_node_sumo, to_node_sumo)
                    except:
                        continue
                    finally:
                        local_edge_map, local_edge_map_id = match_edges_from_paths(matsim, matsim_path, sumo, sumo_path)
                        edge_map = {**edge_map, **local_edge_map}
                        edge_map_id = {**edge_map_id, **local_edge_map_id}
        
        return edge_map, edge_map_id
        
    def removed_matched_edges(matsim, sumo, matched_nodes, matched_edges):
        #Delete already matched edges
        matsim.remove_edges_from(list(matched_edges.keys()))
        sumo.remove_edges_from(list(matched_edges.values()))

        #Delete isolated nodes
        matsim.remove_nodes_from(list(nx.isolates(matsim)))
        sumo.remove_nodes_from(list(nx.isolates(sumo)))

        #Filter matched_nodes that still exist in map
        filtered_matched_nodes = {k: v for k, v in matched_nodes.items() if k in matsim.nodes}

        return filtered_matched_nodes

    def match_edges_from_paths(matsim, matsim_path, sumo, sumo_path):
        matsim_edges = path_to_edges_weights(matsim, matsim_path)
        
        sumo_edges = path_to_edges_weights(sumo, sumo_path)

        map_edges, map_edges_id = matched_edges_on_weight(edge_weights_1=matsim_edges, edge_weights_2=sumo_edges, weight_tol = 2)
        map_edges_id = split_keys(map_edges_id)
        
        return map_edges, map_edges_id

    def path_to_edges_weights(graph, path):
        edges_weights = []
        
        for i in range(len(path)-1):
            f_m, t_m = path[i], path[i+1]
            id_m = graph.get_edge_data(f_m,t_m)['id']
            w_m = graph.get_edge_data(f_m,t_m)['weight']

            edges_weights.append([w_m, id_m, f_m, t_m])

        return edges_weights

    def matched_edges_on_weight(edge_weights_1, edge_weights_2, weight_tol = 10):
        sum_weights_1 = 0
        sum_weights_2 = 0
        edges1 = []
        edges2 = []
        map_edges = {}

        edges1_id = []
        edges2_id = []
        map_edges_id = {}

        while abs(sum_weights_2-sum_weights_1) > weight_tol or not sum_weights_1:
            if not edge_weights_1 and not edge_weights_2:
                break
        
            elif not edge_weights_1:
                for e in edge_weights_2:
                    edges2_id.append(str(e[1]))
                    edges2.append((e[2],e[3]))
                    sum_weights_2 += e[0]
                
                edge_weights_2 = []
                continue

            elif not edge_weights_2:
                for e in edge_weights_1:
                    edges1_id.append(str(e[1]))
                    edges1.append((e[2],e[3]))
                    sum_weights_1+= e[0]
                
                edge_weights_1 = []
                continue

            if not sum_weights_1 and not sum_weights_2:
                e = edge_weights_1.pop(0)
                edges1_id.append(str(e[1]))
                edges1.append((e[2],e[3]))
                sum_weights_1 += e[0]

                e = edge_weights_2.pop(0)
                edges2_id.append(str(e[1]))
                edges2.append((e[2],e[3]))
                sum_weights_2 += e[0]
                #print('Init', edges1, edges2)

            elif sum_weights_1 < sum_weights_2:
                e = edge_weights_1.pop(0)
                edges1_id.append(str(e[1]))
                edges1.append((e[2],e[3]))
                sum_weights_1 += e[0]
                #print('1<2', edges1, edges2)

            elif sum_weights_1 > sum_weights_2:
                e = edge_weights_2.pop(0)
                edges2_id.append(str(e[1]))
                edges2.append((e[2],e[3]))
                sum_weights_2 += e[0]
                #print('1>2', edges1, edges2)

        for e1 in edges1:
            map_edges[e1] = str(edges2)

        map_edges_id["+".join(edges1_id)] = "+".join(edges2_id)

        if edge_weights_1 and edge_weights_2:        
            map_edges_rec, map_edges_id_rec = matched_edges_on_weight(edge_weights_1=edge_weights_1, edge_weights_2=edge_weights_2, weight_tol=weight_tol)
            map_edges = {**map_edges, **map_edges_rec}
            map_edges_id = {**map_edges_id, **map_edges_id_rec}

        return map_edges, map_edges_id

    def split_keys(dict):
        new_dict = {}
        for keys in dict.keys():
            for key in keys.split('+'):
                new_dict[key] = dict[keys]
                
        return new_dict

    ####################################################################################################################################################################

    def is_within_tolerance(node1, node2,tolerance):
        try: 
            distN1N2 = abs(math.dist(node1, node2))
        except:
            return False
        
        return (distN1N2 <tolerance)

    def match_nodes(matsim, sumo, tolerance=15):
        print('... matching nodes with tol', tolerance)
        matsim_pos = nx.get_node_attributes(matsim,'pos')
        sumo_pos =  nx.get_node_attributes(sumo,'pos')

        node_map = {}

        for n1, pos1 in matsim_pos.items():
            for n2, pos2 in sumo_pos.items():
                if is_within_tolerance(pos1, pos2, tolerance):
                    if n1 in node_map:
                        node_map[n1] = node_map[n1] + "+" + n2
                    else:
                        node_map[n1] = n2

        return node_map, tolerance

    ####################################################################################################################################################################

    def plot_Graph(G, label, fig, node_color='red', node_size=1, node_map=None,\
                    width=1, edge_color='red', edge_map=None, style='solid',with_labels = False,):
            #get positions
            pos=nx.get_node_attributes(G,'pos')

            #Create color maps:
            if node_map is not None:
                    node_color_map = ['green' if node in node_map else node_color for node in G.nodes]
                    node_color=node_color_map
                    point = pos[list(node_map.keys())[0]]
                    plt.scatter(x= point[1], y= point[1], s=node_size, label="matched roads", color='green')
            if edge_map is not None:
                    edge_color_map = ['green' if edge in edge_map else edge_color for edge in G.edges]
                    edge_color = edge_color_map
            
            #plot graphs
            fig = nx.draw(G,pos, node_color=node_color, node_size=node_size, width=width, label=label,\
                    edge_color=edge_color, style=style, with_labels=with_labels)
            
            return fig


 """
