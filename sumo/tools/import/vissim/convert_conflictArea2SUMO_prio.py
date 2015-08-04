#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
@file: convert_conflictArea2SUMO_prio.py
@date: 2015-06-15

@author: Gerald Richter <Gerald.Richter@ait.ac.at>
@author: Lukas Grohmann <Lukas.Grohmann@ait.ac.at>
@version $Id: $

Converts a VISSIM .inpx conflict-area into SUMO .net junction request info
after the raw topology conversion by netconvert

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2009-2015 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
# TODO: usage doc.ref

from __future__ import print_function

import argparse
from xml.dom import minidom

# import collections
import numpy as np


def dict_from_node_attributes(node):
    """takes a xml node and returns a dictionary with its attributes"""
    return dict((attn, node.getAttribute(attn)) for attn in
                node.attributes.keys())


def nparr_from_dict_list(dicl_tab, col_ns, col_ts):
    """converts a dictionary into an np array table structure"""
    return np.array([tuple(rd.get(cn, '-1') for cn in col_ns) for rd in
                     dicl_tab], dtype=np.dtype(list(zip(col_ns, col_ts))))


def convert_charBits2boolArr(char_bits):
    return np.fromiter((int(c) for c in char_bits), dtype=bool)


def convert_boolArr2charBits(bool_arr):
    return ''.join((str(int(bit)) for bit in bool_arr))


def get_relevant_conflict_areas(confl_area_tab):
    """exclude the conflict areas which are passive"""
    relevant_ca_tab = np.array(
        [ca for ca in confl_area_tab
            if ca['status'].upper() != 'PASSIVE'], dtype='O')
    for confl_ar in relevant_ca_tab:
        if confl_ar[1] not in conn_link_d or confl_ar[2] not in conn_link_d:
            print(confl_ar[1] + "   " + confl_ar[2])
    return relevant_ca_tab


def is_verbinder(xmldoc):
    """checks if a given link is a verbinder"""
    is_verbinder_d = {}
    link_l = xmldoc.getElementsByTagName("link")
    for link in link_l:
        if len(link.getElementsByTagName("fromLinkEndPt")) > 0:
            is_verbinder_d[link.getAttribute("no")] = True
        else:
            is_verbinder_d[link.getAttribute("no")] = False
    return is_verbinder_d


def get_vissim_data(inpxdoc):
    """parse the VISSIM data"""
    from_to_tab = []

    confl_area_tab = [dict_from_node_attributes(nd) for
                      nd in inpxdoc.getElementsByTagName('conflictArea')]
    confl_area_tab = nparr_from_dict_list(confl_area_tab,
                                          ('no', 'link1', 'link2', 'status'),
                                          ('O',) * 4)
    for lin in inpxdoc.getElementsByTagName('link'):
        link_d = dict_from_node_attributes(lin)
        if lin.hasChildNodes():
            lep_d = {}      # link end point dict
            for ept in ('fromLinkEndPt', 'toLinkEndPt'):
                lep_nd = lin.getElementsByTagName(ept)
                ch0 = ept[0]    # identifier 'f'rom / 't'o
                if len(lep_nd) > 0:
                    dic = dict_from_node_attributes(lep_nd.item(0))
                    dic['link'], dic['lane'] = dic['lane'].split(' ')
                    lep_d.update(
                        dict(
                            (ch0 + '_' + key, val) for
                                key, val in dic.items()))
            lep_d.update({'_link': link_d['no'], })
            from_to_tab.append(lep_d)

    # which columns to pick ?
    from_to_tab = nparr_from_dict_list(
        from_to_tab,
        '_link f_link f_lane t_link t_lane'.split(),
        'O O i O i'.split())
    return from_to_tab, confl_area_tab


def get_sumo_data(sumodoc):
    """parse the SUMO data"""
    junc_tab = []
    requ_tab = []
    conn_tab = []

    for jun in sumodoc.getElementsByTagName('junction'):
        junc_d = dict_from_node_attributes(jun)
        req_l = [dict_from_node_attributes(reqn) for
                 reqn in jun.getElementsByTagName('request')]
        junc_d['_m_foes'] = np.r_[
            [convert_charBits2boolArr(rq['foes']) for rq in req_l]]
        junc_d['_m_resp'] = np.r_[
            [convert_charBits2boolArr(rq['response']) for rq in req_l]]
        junc_d['_v_cont'] = convert_charBits2boolArr(
            [rq['cont'] for rq in req_l])
        junc_d['_index'] = [rq['index'] for rq in req_l]
        junc_tab.append(junc_d)

        [req.update({'_junction': junc_d['id']}) for req in req_l]
        requ_tab.extend(req_l)

    conn_tab = [dict_from_node_attributes(nd) for
                nd in sumodoc.getElementsByTagName('connection')]
    col_n = ('from', 'to', 'fromLane', 'toLane', 'via')
    col_t = ('O',) * 5
    conn_tab = nparr_from_dict_list(conn_tab, col_n, col_t)

    return junc_tab, requ_tab, conn_tab


def get_conn_verb_rel(conn_tab, from_to_tab):
    """returns 2 dictionaries, which contains the relation between connections
    and verbinder

    :param conn_tab:        matrix of BLA
    :param from_to__tab:    matrix of BLA

    :rtype link_conn_d, conn_link_d:    dicts of FOO
    """

    conn_link_d = {}  # key = verbinder.id, value = list<connection.id>
    link_conn_d = {}  # key = connection.id, value = verbinder.id
    for conn in conn_tab:
        #
        if ':' not in conn['from']:
            link = from_to_tab[
                (from_to_tab['f_link'] == conn['from'].split("[")[0]) & (
                    from_to_tab['t_link'] == conn['to'].split("[")[0])]
            if len(link) > 0:
                # dictionary to get the connection id for a given verbinder id
                link_conn_d[conn['via']] = link['_link'][0]
                if link["_link"][0] in conn_link_d:
                    conn_link_d[link["_link"][0]].append(conn['via'])
                else:
                    conn_link_d[link["_link"][0]] = [conn['via']]
            else:
                print ("from: " + conn['from'] + "to: " + conn['to'])
    return link_conn_d, conn_link_d

# find split connections


def set_split_connections(conn_tab, conn_link_d, link_conn_d):
    """finds the connections which are split by an internal junction
    """
    split_conn_tab = np.array(
        [co for co in conn_tab if ':' in co['from']
            and co['via'] != "-1"], dtype='O')
    for split_conn in split_conn_tab:
        for conn in conn_tab:
            if ((conn['via'][0:conn['via'].rfind('_')] == split_conn['from']) &
               (conn['to'] == split_conn['to']) &
               (conn['via'].split('_')[-1] == split_conn['fromLane'])):
                # print(conn['via'] + "    " + split_conn['via'])
                conn_link_d[link_conn_d[conn['via']]].append(split_conn['via'])
                link_conn_d[split_conn['via']] = link_conn_d[conn['via']]


def update_net_file(sumodoc, junc_tab, filename):
    """creates a new net file with the updated priority information"""
    junc_list = sumodoc.getElementsByTagName('junction')
    for junc in junc_list:
        junction = [
            item for item in junc_tab
                if junc.getAttribute('id') == item['id']][0]
        request_list = junc.getElementsByTagName('request')
        for requ in request_list:
            index = int(requ.getAttribute('index'))
            requ.setAttribute(
                'response',
                convert_boolArr2charBits(
                    junction['_m_resp'][index]))

    with open("%s.net.xml" % filename, "w") as ofh:
        sumodoc.writexml(ofh, addindent='    ', newl='\n')
        ofh.close()


def get_indices_from_ca(confl_area, conn_list_1, conn_list_2, int_lanes):
    """get the indices for each connection and set the priority"""
    priority_index_l = []
    subordinated_index_l = []
    if confl_area['status'] == 'ONEYIELDSTWO':
        for conn in conn_list_1:
            subordinated_index_l.append(int_lanes.index(conn))
        for conn in conn_list_2:
            priority_index_l.append(int_lanes.index(conn))
    elif confl_area['status'] == 'TWOYIELDSONE':
        for conn in conn_list_1:
            priority_index_l.append(int_lanes.index(conn))
        for conn in conn_list_2:
            subordinated_index_l.append(int_lanes.index(conn))
    else:
        print("Status nicht erkannt")
    return priority_index_l, subordinated_index_l


def change_response_bits(relevant_ca_tab, conn_link_d,
                         junc_tab, is_verbinder_d):
    """change the response bits according to the CA information from VISSIM"""
    for confl_area in relevant_ca_tab:
        if not is_verbinder_d[confl_area['link1']] \
                or not is_verbinder_d[confl_area['link2']]:
            continue
        conn_list_1 = conn_link_d[confl_area['link1']]
        conn_list_2 = conn_link_d[confl_area['link2']]
        # print(conn_list_1[0] + "   " + conn_list_2[0])
        junction = [
            junc for junc in junc_tab if len(
                set(conn_list_1).intersection(
                    junc['intLanes'].split(" "))) > 0 and len(
                        set(conn_list_2).intersection(
                            junc['intLanes'].split(" "))) > 0]
        if len(junction) > 0:
            junction = junction[0]
        else:
            continue
        int_lanes = junction['intLanes'].split(' ')
        # remove unused connections
        conn_list_1 = list(set(conn_list_1).intersection(int_lanes))
        conn_list_2 = list(set(conn_list_2).intersection(int_lanes))

        priority_index_l, subordinated_index_l = \
            get_indices_from_ca(confl_area,
                                conn_list_1,
                                conn_list_2,
                                int_lanes)
        # change the response bits according to the conflict areas
        for prio_ind in priority_index_l:
            for sub_ind in subordinated_index_l:
                # False = 0, True = 1
                junction['_m_resp'][prio_ind][-(sub_ind + 1)] = False
                junction['_m_resp'][sub_ind][-(prio_ind + 1)] = True


# MAIN
if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='conflict area priority conversion utility (VISSIM.inpx to SUMO)')
    parser.add_argument('--vissim-input', '-V', type=str,
                        help='VISSIM inpx file path')
    parser.add_argument('--SUMO-net', '-S', type=str,
                        help='SUMO net file path')
    parser.add_argument('--output-file', '-o', type=str,
                        help='output file name')
    args = parser.parse_args()
    print("\n", args, "\n")
    print('\n---\n\n* loading VISSIM net:\n\t', args.vissim_input)
    INPXDOC = minidom.parse(args.vissim_input)
    # INPXDOC = minidom.parse(inpx_file)
    print('\n---\n\n* loading SUMO net:\n\t', args.SUMO_net,)
    SUMODOC = minidom.parse(args.SUMO_net)
    # SUMODOC = minidom.parse(net_file)

    # parse the VISSIM data
    from_to_tab, confl_area_tab = get_vissim_data(INPXDOC)

    # check if a link is a verbinder
    is_verbinder_d = is_verbinder(INPXDOC)

    # parse the SUMO data
    junc_tab, requ_tab, conn_tab = get_sumo_data(SUMODOC)

    # get the relations between connections and verbinder
    link_conn_d, conn_link_d = get_conn_verb_rel(conn_tab, from_to_tab)

    # get only the conflict areas which are not passive
    relevant_ca_tab = get_relevant_conflict_areas(confl_area_tab)

    # update the verbinder-connection dictionaries with the split connection
    # data
    set_split_connections(conn_tab, conn_link_d, link_conn_d)

    # change the response bits of the junctions according to the information
    # of the conflict areas
    change_response_bits(relevant_ca_tab, conn_link_d,
                         junc_tab, is_verbinder_d)

    # edit the existing sumo .net file and create a new .net file
    update_net_file(SUMODOC, junc_tab, args.output_file)
