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

# @file    sumo.py
# @author  Joerg Schweizer
# @date   2012

from agilepy.lib_base.processes import Process, CmlMixin, ff, call, P, filepathlist_to_filepathstring, Options
from coremodules.scenario.scenario import load_scenario
from results import Simresults
from coremodules.network.network import SumoIdsConf
import agilepy.lib_base.xmlman as xm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.classman as cm
import numpy as np
from coremodules.modules_common import *
import os
import sys
import string
import time
import shutil  # for deleting directory trees
from collections import OrderedDict
from xml.sax import saxutils, parse, handler
if __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    SUMOPYDIR = os.path.join(APPDIR, '..', '..')
    sys.path.append(SUMOPYDIR)
try:
    if 'SUMO_HOME' in os.environ:
        tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
        sys.path.append(tools)
    else:
        print("please declare environment variable 'SUMO_HOME'")

    import traci
    import traci.constants as tc


except:
    print 'WARNING: No module traci in syspath. Please provide SUMO_HOME.'

    traci = None

#from agilepy.lib_base.misc import get_inversemap
#from agilepy.lib_base.geometry import find_area


def write_netconfig(filename_netconfig, filename_net,
                    filename_routes='',
                    filename_poly=None,
                    dirname_output='',
                    starttime=None, stoptime=None,
                    time_step=1.0,
                    time_to_teleport=-1,
                    pedestrian_model='None',
                    width_sublanes=-1.0,
                    filename_ptstops=None,
                    filepath_output_vehroute=None,
                    filepath_output_tripinfo=None,
                    is_tripdata_unfinished=False,
                    filepath_output_edgedata=None,
                    filepath_output_lanedata=None,
                    filepath_output_edgeemissions=None,
                    filepath_output_laneemissions=None,
                    filepath_output_edgenoise=None,
                    filepath_output_lanenoise=None,
                    filepath_output_electricenergy=None,
                    filepath_output_fcd=None,
                    filepath_output_summary=None,
                    freq=60,
                    is_exclude_emptyedges=False,
                    is_exclude_emptylanes=False,
                    is_ignore_route_errors=True,
                    filepath_gui=None,
                    seed=1025,
                    is_openscenegraph=False,
                    width_pedestrian_striping=0.49,
                    slowdownfactor_pedestrian_striping=0.2,
                    jamtime_pedestrian_striping=20,
                    jamtime_pedestrian_crossing_striping=10,
                    is_collission_check_junctions=True,
                    is_ignore_accidents=False,
                    collission_action='teleport',
                    is_ballistic_integrator=False,
                    is_rerouting=False,
                    probability_rerouting=0.0,
                    is_deterministic_rerouting=False,
                    period_rerouting=0,
                    preperiod_rerouting=60,
                    adaptationinterval_rerouting=1,
                    adaptationweight_rerouting=0.0,
                    adaptationsteps_rerouting=180,
                    taxiservice=None,
                    ):
    """
    filename_netconfig = output filename of network config file without path
    filename_net = input filename of network  file without path
    filename_rou = input filename of routes  file without path
    filename_poly = input filename of polygons file without path
    dirname_output = directory where config, network, route and poly file reside
    """
    # print 'write_netconfig >>%s<<'%filename_netconfig
    # print '  filename_poly=>>%s<<'%filename_poly
    if dirname_output:
        filepath_netconfig = os.path.join(dirname_output, filename_netconfig)
    else:
        filepath_netconfig = filename_netconfig

    if (filepath_output_edgedata is not None)\
        | (filepath_output_lanedata is not None)\
        | (filepath_output_edgeemissions is not None)\
        | (filepath_output_laneemissions is not None)\
        | (filepath_output_edgenoise is not None)\
        | (filepath_output_lanenoise is not None)\
            | (filepath_output_electricenergy is not None):
        # filename of additional files:
        filename_add = string.join(filename_netconfig.split('.')[:-2]+['outc.xml'], '.')
        filepath_add = os.path.join(dirname_output, filename_add)
        # print '  filepath_add',filepath_add
    else:
        filename_add = None

    simfile = open(filepath_netconfig, 'w')

    simfile.write(
        """<?xml version="1.0"?>
<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.sf.net/xsd/sumoConfiguration.xsd">
<input>\n""")

    simfile.write('  <net-file value="%s"/>\n' % filename_net)

    if filename_routes != "":
        simfile.write('  <route-files value="%s"/>\n' % filename_routes)

    # print '  filename_add',filename_add
    # print '  filepath_add',filepath_add
    simfile.write('  <additional-files value="')

    filenames_add = set([filename_poly, filename_add, filename_ptstops])
    filenames_add.discard(None)
    filenames_add = list(filenames_add)

    if len(filenames_add) > 0:
        for filename in filenames_add[:-1]:
            simfile.write('%s,' % filename)
        simfile.write('%s' % filenames_add[-1])

    simfile.write('" />\n')
    simfile.write('</input>\n')

    if (starttime is not None) & (stoptime is not None):
        simfile.write(
            """
<time>
    <begin value="%s"/>
    <end value="%s"/>
</time>
        """ % (starttime, stoptime))

    simfile.write('<time-to-teleport value="%s"/>\n' % time_to_teleport)
    simfile.write('<seed value="%s"/>\n' % seed)
    simfile.write('<step-length value="%s"/>\n' % time_step)

    simfile.write('<ignore-route-errors value="%s"/>\n' % is_ignore_route_errors)

    if is_ballistic_integrator:
        simfile.write('<step-method.ballistic value="True"/>\n')

    if width_sublanes > 0:
        simfile.write('<lateral-resolution value="%s"/>\n' % width_sublanes)
    # not (yet) recogniced...move to cml
    if pedestrian_model != 'None':
        simfile.write('<pedestrian.model value="%s"/>\n' % pedestrian_model)
        if pedestrian_model == 'striping':
            simfile.write('<pedestrian.striping.stripe-width value="%s"/>\n' % width_pedestrian_striping)
            simfile.write('<pedestrian.striping.dawdling value="%s"/>\n' % slowdownfactor_pedestrian_striping)
            simfile.write('<pedestrian.striping.jamtime value="%s"/>\n' % jamtime_pedestrian_striping)

            # from 1.3.0
            #simfile.write('<pedestrian.striping.jamtime.crossing value="%s"/>\n'%jamtime_pedestrian_crossing_striping)

    simfile.write('<collision.check-junctions value="%s"/>\n' % is_collission_check_junctions)
    simfile.write('<collision.action value="%s"/>\n' % collission_action)
    #simfile.write('<ignore-accidents value="%s"/>\n'%is_ignore_accidents)

    if taxiservice is not None:
        taxiservice.write_config(simfile, ident=0)

    simfile.write('<output>\n')
    # <output-file value="quickstart.net.xml"/>

    if filepath_output_vehroute is not None:
        simfile.write('<vehroute-output value="%s"/>\n' % filepath_output_vehroute)
    if filepath_output_tripinfo is not None:
        simfile.write('<tripinfo-output value="%s"/>\n' % filepath_output_tripinfo)

    if filepath_output_fcd is not None:
        simfile.write("""<fcd-output value="%s"/>
                          <device.fcd.probability  value="1"/>\n
                      """ % (filepath_output_fcd,))
        # <device.fcd.period value="1"/>

    if filepath_output_electricenergy is not None:
        simfile.write("""<battery-output value="%s"/>
        <battery-output.precision value="4"/>
        <device.battery.probability value="1"/>
        \n
        """ % (filepath_output_electricenergy,))

    if filepath_output_summary is not None:
        simfile.write("""<summary-output value="%s"/>""" % filepath_output_summary)

    simfile.write('</output>\n')

    if filepath_gui is not None:
        simfile.write('<gui-settings-file value="%s"/>\n' % filepath_gui)

    if is_openscenegraph:
        simfile.write('<osg-view value="true"/>\n')

    if is_tripdata_unfinished:
        simfile.write('<tripinfo-output.write-unfinished value="true"/>\n')

    if is_rerouting:
        simfile.write("""<routing>
        <device.rerouting.probability value="%.2f"/>
        <device.rerouting.deterministic value="%s"/>
        <device.rerouting.period value="%d"/>
        <device.rerouting.pre-period value="%d"/>
        <device.rerouting.adaptation-interval value = "%d"/>\n
            """ % (probability_rerouting,
                   is_deterministic_rerouting,
                   period_rerouting,
                   preperiod_rerouting,
                   float(adaptationinterval_rerouting)/time_step,
                   ))

        if adaptationweight_rerouting > 0:
            simfile.write("""<device.rerouting.adaptation-weight  value="%.4f"/>\n""" % adaptationweight_rerouting)
        else:
            simfile.write("""<device.rerouting.adaptation-steps value="%d"/>\n""" % adaptationsteps_rerouting)

        simfile.write('</routing>\n')

    # <report>
    #    <no-duration-log value="true"/>
    #    <no-step-log value="true"/>
    # </report>

    simfile.write('</configuration>\n')
    simfile.close()

    # add path to  additional files if necessary
    if filename_add is not None:
        addfile = open(filepath_add, 'w')
        addfile.write('<add>\n')
        if filepath_output_edgedata is not None:
            addfile.write('  <edgeData id="output_edgedata_%d" freq="%d" file="%s" excludeEmpty="%s"/>\n' %
                          (freq, freq, filepath_output_edgedata, str(is_exclude_emptyedges).lower()))

        if filepath_output_lanedata is not None:
            addfile.write('  <laneData id="output_lanedata_%d" freq="%d" file="%s" excludeEmpty="%s"/>\n' %
                          (freq, freq, filepath_output_lanedata, str(is_exclude_emptylanes).lower()))

        if filepath_output_edgeemissions is not None:
            addfile.write('  <edgeData id="output_edgeemissions_%d" type="emissions" freq="%d" file="%s" excludeEmpty="%s"/>\n' %
                          (freq, freq, filepath_output_edgeemissions, str(is_exclude_emptyedges).lower()))

        if filepath_output_laneemissions is not None:
            addfile.write('  <laneData id="output_laneemissions_%d" type="emissions" freq="%d" file="%s" excludeEmpty="%s"/>\n' %
                          (freq, freq, filepath_output_laneemissions, str(is_exclude_emptylanes).lower()))

        if filepath_output_edgenoise is not None:
            addfile.write('  <edgeData id="edgenoise_%d" type="harmonoise" freq="%d" file="%s" excludeEmpty="%s"/>\n' %
                          (freq, freq, filepath_output_edgenoise, str(is_exclude_emptyedges).lower()))

        if filepath_output_lanenoise is not None:
            addfile.write('  <laneData id="lanenoise_%d" type="harmonoise" freq="%d" file="%s" excludeEmpty="%s"/>\n' %
                          (freq, freq, filepath_output_lanenoise, str(is_exclude_emptylanes).lower()))

        # if filepath_output_electricenergy is not None:
        #    addfile.write('  <laneData id="lanenoise_%d" type="harmonoise" freq="%d" file="%s" excludeEmpty="%s"/>\n'%(freq,freq,filepath_output_lanenoise,str(is_exclude_emptylanes).lower()))

        addfile.write('</add>\n')
        addfile.close()


class Sumo(CmlMixin, Process):
    def __init__(self, scenario,
                 results=None,
                 logger=None,
                 guimode='sumopy',  # sumo,
                 is_runnow=False,
                 is_run_background=False, is_nohup=False,
                 workdirpath=None,
                 is_export_net=True,
                 is_export_poly=True,
                 is_export_rou=True,
                 is_prompt_filepaths=False,
                 routefilepaths=None,
                 netfilepath=None,
                 ptstopsfilepath=None,
                 polyfilepath=None,
                 logfilepath='',
                 cmlfilepath=None,  # write command line into this file instead of executing sumo
                 **kwargs):

        #self.scenario = scenario
        self._cmlfilepath = cmlfilepath
        self._init_common('sumo', parent=scenario, name='SUMO',
                          logger=logger,
                          info='SUMO micro simulation of scenario.',
                          )

        # if results is None:
        #    self._results = Simresults(scenario=scenario)
        # else:
        self._results = results

        rootname = scenario.get_rootfilename()
        rootdirpath = scenario.get_workdirpath()
        self.configfilepath = os.path.join(rootdirpath, rootname+'.netc.xml')

        self.init_cml('xxx', is_run_background=is_run_background, is_nohup=is_nohup)  # pass main shell command
        attrsman = self.get_attrsman()

        self.guimode = attrsman.add(cm.AttrConf('guimode', guimode,
                                                groupnames=['options', 'misc'],
                                                choices=['sumopy', 'sumopy+map', 'native', 'nogui'],  # 'openscene'???
                                                name='GUI mode',
                                                perm='rw',
                                                info='Gui mode:  sumopy = sumopy style, sumopy+map = sumopy theme with backround map, native = Native SUMO gui, openscene = Open street graph, nogui = run without gui window'
                                                ))

        simtime_start_default = scenario.demand.get_time_depart_first()
        # estimate end of simtime
        simtime_end_default = scenario.demand.get_time_depart_last()

        self.simtime_start = attrsman.add(cm.AttrConf('simtime_start', kwargs.get('simtime_start', simtime_start_default),
                                                      groupnames=['options', 'timing'],
                                                      name='Start time',
                                                      perm='rw',
                                                      info='Start time of simulation in seconds after midnight.',
                                                      unit='s',
                                                      cml='--begin',  # no prefix
                                                      ))

        self.simtime_end = attrsman.add(cm.AttrConf('simtime_end', kwargs.get('simtime_end', simtime_end_default),
                                                    groupnames=['options', 'timing'],
                                                    name='End time',
                                                    perm='rw',
                                                    info='End time of simulation in seconds after midnight.',
                                                    unit='s',
                                                    cml='--end',  # no prefix
                                                    ))

        self.is_dynaroute = attrsman.add(cm.AttrConf('is_dynaroute', kwargs.get('is_dynaroute', False),
                                                     groupnames=['options', 'timing'],
                                                     name='Dynamic routing',
                                                     perm='rw',
                                                     info='Routing is always performed during the simulation on vehicle dearture time, based on current edge travel times. This option corrisponds to the so called one shot assignment.',
                                                     ))

        self.time_to_teleport = attrsman.add(cm.AttrConf('time_to_teleport', kwargs.get('time_to_teleport', -1),
                                                         groupnames=['options', 'timing'],
                                                         name='teleport',
                                                         perm='rw',
                                                         info='Time to teleport in seconds, which is the time after'
                                                         + 'dedlocks get resolved by teleporting\n'
                                                         + '-1 means no teleporting takes place',
                                                         metatype='time',
                                                         unit='s',
                                                         cml='--time-to-teleport',
                                                         ))

        self._init_attributes_basic(scenario, **kwargs)

        # print '\nSumo.__init__',kwargs

        #self.scenario = scenario
        #self.settings = scenario.settings

        self.time_sample = attrsman.add(cm.AttrConf('time_sample', kwargs.get('time_sample', 60),
                                                    groupnames=['options', 'timing'],
                                                    name='Output sample time',
                                                    perm='rw',
                                                    info='Common sampling time of output data.',
                                                    metatype='time',
                                                    unit='s',
                                                    ))

        self.time_warmup = attrsman.add(cm.AttrConf('time_warmup', kwargs.get('time_warmup', 0.0),
                                                    groupnames=['options', 'timing'],
                                                    name='Warmup time',
                                                    perm='rw',
                                                    info='Start recording results after this time. It currently work with edgeresults, connectionresults and virtualpersonresults',
                                                    metatype='time',
                                                    unit='s',
                                                    ))

        self.is_edgedata = attrsman.add(cm.AttrConf('is_edgedata', kwargs.get('is_edgedata', False),
                                                    groupnames=['options', 'output'],
                                                    name='Output edge data',
                                                    perm='rw',
                                                    info='If set, generate detailed data for all edges.'
                                                    ))

        self.is_routedata = attrsman.add(cm.AttrConf('is_routedata', kwargs.get('is_routedata', False),
                                                     groupnames=['options', 'output'],
                                                     name='Output route data',
                                                     perm='rw',
                                                     info='If set, generate detailed data for all routes.'
                                                     ))

        self.is_tripdata = attrsman.add(cm.AttrConf('is_tripdata', kwargs.get('is_tripdata', False),
                                                    groupnames=['options', 'output'],
                                                    name='Output trip data',
                                                    perm='rw',
                                                    info='If set, generate detailed data for all completed trips.'
                                                    ))

        self.is_tripdata_unfinished = attrsman.add(cm.AttrConf('is_tripdata_unfinished', kwargs.get('is_tripdata_unfinished', False),
                                                               groupnames=['options', 'output'],
                                                               name='Include unfinished trip data',
                                                               perm='rw',
                                                               info='If set, also unfinished trip data is collected.'
                                                               ))

        self.is_edgenoise = attrsman.add(cm.AttrConf('is_edgenoise', kwargs.get('is_edgenoise', False),
                                                     groupnames=['options', 'output'],
                                                     name='Output edge noise',
                                                     perm='rw',
                                                     info='If set, generate noise information for all edges.'
                                                     ))

        self.is_edgesemissions = attrsman.add(cm.AttrConf('is_edgesemissions', kwargs.get('is_edgesemissions', False),
                                                          groupnames=['options', 'output'],
                                                          name='Output edge emissions',
                                                          perm='rw',
                                                          info='If set, generate emission information for all edges.'
                                                          ))

        self.is_electricenergy = attrsman.add(cm.AttrConf('is_electricenergy', kwargs.get('is_electricenergy', False),
                                                          groupnames=['options', 'output'],
                                                          name='Output electric energy',
                                                          perm='rw',
                                                          info='If set, record energy data for vehicles with electric drive.'
                                                          ))

        self.is_fcd = attrsman.add(cm.AttrConf('is_fcd', kwargs.get('is_fcd', False),
                                               groupnames=['options', 'output'],
                                               name='Output vehicle trajectories',
                                               perm='rw',
                                               info='If set, record vehicle trajectories and speed for each timestep. This means SUMO FCD output is generated. Attention, this option will eat up a tremendous amount of disk space.'
                                               ))

        self.is_summary = attrsman.add(cm.AttrConf('is_summary', kwargs.get('is_summary', False),
                                                   groupnames=['options', 'output'],
                                                   name='Output summary',
                                                   perm='rw',
                                                   info='If set, generate summary data of simulation steps.'
                                                   ))

        outfile_prefix = kwargs.get('outfile_prefix', 'out')
        self.routesdatapath = attrsman.add(cm.AttrConf('routesdatapath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.roudata.xml'),
                                                       groupnames=['outputfiles', '_private'],
                                                       perm='r',
                                                       name='Route data file',
                                                       wildcards='Route data XML files (*.roudata.xml)|*.roudata.xml',
                                                       metatype='filepath',
                                                       info="""SUMO xml file with route output info.""",
                                                       #attrnames_data = ['depart','arrival'],
                                                       #element = 'vehicle',
                                                       #id_type = 'trip',
                                                       #reader = 'plain',
                                                       ))

        self.tripdatapath = attrsman.add(cm.AttrConf('tripdatapath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.tripdata.xml'),
                                                     groupnames=['outputfiles', '_private'],
                                                     perm='r',
                                                     name='Edge data file',
                                                     wildcards='Trip data XML files (*.tripdata.xml)|*.tripdata.xml',
                                                     metatype='filepath',
                                                     info="""SUMO xml file with trip output data.""",
                                                     attrnames_data=['depart', 'arrival', 'duration'],
                                                     #element = 'tripinfo',
                                                     #id_type = 'trip',
                                                     #reader = 'plain',
                                                     ))

        self.edgedatapath = attrsman.add(cm.AttrConf('edgedatapath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.edgedata.xml'),
                                                     groupnames=['outputfiles', '_private'],
                                                     perm='r',
                                                     name='Edge data file',
                                                     wildcards='Edge data XML files (*.edgedata.xml)|*.edgedata.xml',
                                                     metatype='filepath',
                                                     info="""SUMO xml file with edge output data.""",
                                                     ))

        self.edgenoisepath = attrsman.add(cm.AttrConf('edgenoisepath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.edgenoise.xml'),
                                                      groupnames=['outputfiles', '_private'],
                                                      perm='r',
                                                      name='Edge noise file',
                                                      wildcards='Edge noise XML files (*.edgenoise.xml)|*.edgenoise.xml',
                                                      metatype='filepath',
                                                      info="""SUMO xml file with edge noise data.""",
                                                      #attrnames_averaged = ['noise'],
                                                      #element = 'edge',
                                                      #id_type = 'edge',
                                                      #reader = 'interval',
                                                      ))

        self.edgeemissionspath = attrsman.add(cm.AttrConf('edgeemissionspath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.edgeemissions.xml'),
                                                          groupnames=['outputfiles', '_private'],
                                                          perm='r',
                                                          name='Edge noise file',
                                                          wildcards='Edge noise XML files (*.edgeemissions.xml)|*.edgeemissions.xml',
                                                          metatype='filepath',
                                                          info="""SUMO xml file with edge emission data.""",
                                                          attrnames_data=['fuel_abs', 'CO_abs',
                                                                          'CO2_abs', 'NOx_abs', 'PMx_abs'],
                                                          attrnames_averaged=['fuel_normed',
                                                                              'CO_normed', 'CO2_normed', ],
                                                          element='edge',
                                                          id_type='edge',
                                                          reader='interval',
                                                          ))

        self.electricenergypath = attrsman.add(cm.AttrConf('electricenergypath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.electricenergy.xml'),
                                                           groupnames=['outputfiles', '_private'],
                                                           perm='r',
                                                           name='Electrical energy file',
                                                           wildcards='Electrical energy  XML files (*.electricenergy.xml)|*.electricenergy.xml',
                                                           metatype='filepath',
                                                           info="""SUMO xml file with edge emission data.""",
                                                           attrnames_data=['Consum', 'energyCharged',
                                                                           'energyChargedInTransit', 'energyChargedStopped'],
                                                           attrnames_averaged=['speed', ],
                                                           element='vehicle',
                                                           id_type='vehicle',
                                                           reader='interval',
                                                           ))

        self.filepath_output_fcd = attrsman.add(cm.AttrConf('filepath_output_fcd', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.fcd.xml'),
                                                            groupnames=['outputfiles', '_private'],
                                                            perm='r',
                                                            name='Trajectory file',
                                                            wildcards='Trajectory  XML files (*.fcd.xml)|*.fcd.xml',
                                                            metatype='filepath',
                                                            info="""SUMO xml file with edge emission data.""",
                                                            attrnames_data=['x', 'y', 'angle', ],
                                                            attrnames_averaged=['speed', ],
                                                            element='vehicle',
                                                            id_type='vehicle',
                                                            reader='interval',
                                                            ))

        self.summarypath = attrsman.add(cm.AttrConf('summarypath', os.path.join(rootdirpath, rootname+'.'+outfile_prefix+'.electricenergysum.xml'),
                                                    groupnames=['_private'],
                                                    perm='r',
                                                    name='Simulation summary file',
                                                    wildcards='Simulation summary XML files (*.xml)|*.xml',
                                                    metatype='filepath',
                                                    info="""SUMO xml file with summary data on simulatin steps.""",
                                                    ))

        self.is_rerouting = attrsman.add(cm.AttrConf('is_rerouting', kwargs.get('is_rerouting', False),
                                                     groupnames=['options', 'rerouting'],
                                                     name='Rerouting enabled',
                                                     perm='rw',
                                                     info='Enable rerouting of vehicles during the simulation.'
                                                     ))

        self.probability_rerouting = attrsman.add(cm.AttrConf('probability_rerouting', kwargs.get('probability_rerouting', 0.25),
                                                              groupnames=['options', 'rerouting'],
                                                              name='Rerouting probability',
                                                              perm='rw',
                                                              info='The probability for a vehicle to have a routing device.'
                                                              ))

        self.is_deterministic_rerouting = attrsman.add(cm.AttrConf('is_deterministic_rerouting', kwargs.get('is_deterministic_rerouting', False),
                                                                   groupnames=['options', 'rerouting'],
                                                                   name='Deterministic rerouting',
                                                                   perm='rw',
                                                                   info='The devices are set deterministic using a fraction of 1000 (with the defined probability).'
                                                                   ))

        self.period_rerouting = attrsman.add(cm.AttrConf('period_rerouting', kwargs.get('period_rerouting', 180),
                                                         groupnames=['options', 'rerouting'],
                                                         name='Rerouting period',
                                                         perm='rw',
                                                         info='The period with which the vehicle shall be rerouted.',
                                                         unit='s',
                                                         ))

        self.preperiod_rerouting = attrsman.add(cm.AttrConf('preperiod_rerouting', kwargs.get('preperiod_rerouting', 180),
                                                            groupnames=['options', 'rerouting'],
                                                            name='Rerouting pre-period',
                                                            perm='rw',
                                                            info='The rerouting period before insertion/depart.',
                                                            unit='s',
                                                            ))
        self.adaptationinterval_rerouting = attrsman.add(cm.AttrConf('adaptationinterval_rerouting', kwargs.get('adaptationinterval_rerouting', 180),
                                                                     groupnames=['options', 'rerouting'],
                                                                     name='adaptation interval edge weight',
                                                                     perm='rw',
                                                                     info='The interval for updating the edge weights. Short update times means a large computational burden.',
                                                                     unit='s',
                                                                     ))
        self.adaptationweight_rerouting = attrsman.add(cm.AttrConf('adaptationweight_rerouting', kwargs.get('adaptationweight_rerouting', 0.0),
                                                                   groupnames=['options', 'rerouting'],
                                                                   name='Adaptation weight for rerouting',
                                                                   perm='rw',
                                                                   info='The weight of prior edge weights for exponential averaging from [0, 1]. Disabeled for 0.0.'
                                                                   ))

        self.adaptationsteps_rerouting = attrsman.add(cm.AttrConf('adaptationsteps_rerouting', kwargs.get('adaptationsteps_rerouting', 10),
                                                                  groupnames=['options', 'rerouting'],
                                                                  name='Adaptation steps for rerouting',
                                                                  perm='rw',
                                                                  info='The number of adaptation steps for edge weights averaging (enable for values > 0).'
                                                                  ))

        self._init_special(**kwargs)

        self.is_include_poly = attrsman.add(cm.AttrConf('is_include_poly', kwargs.get('is_include_poly', True),
                                                        name='Include buildings?',
                                                        perm='rw',
                                                        groupnames=['options', 'misc'],
                                                        info='Include building polynomials. Only for visualization purposes.',
                                                        ))

        self.is_start = attrsman.add(cm.AttrConf('is_start', kwargs.get('is_start', False),
                                                 groupnames=['options', 'misc'],
                                                 perm='rw',
                                                 name='Start immediately',
                                                 info='Immediate start of simulation, without waiting to press the start button in GUI mode.',
                                                 is_enabled=lambda self: self.guimode is not 'nogui',
                                                 ))

        self.is_quit_on_end = attrsman.add(cm.AttrConf('is_quit_on_end', kwargs.get('is_quit_on_end', False),
                                                       groupnames=['options', 'misc'],
                                                       perm='rw',
                                                       name='Quit on end',
                                                       info='Quits the GUI when the simulation stops.',
                                                       is_enabled=lambda self: self.guimode is not 'nogui',
                                                       ))

        self.logfilepath = attrsman.add(cm.AttrConf('logfilepath', logfilepath,
                                                    groupnames=['options', 'misc'],
                                                    perm='rw',
                                                    name='Log file',
                                                    wildcards='Log file (*.txt)|*.txt',
                                                    metatype='filepath',
                                                    info="Writes all messages to Log filepath, implies verbous. If blank, no logfile is created",
                                                    ))

        if ptstopsfilepath is None:
            ptstopsfilepath = scenario.net.ptstops.get_stopfilepath()

            self.is_export_ptstops = attrsman.add(cm.AttrConf('is_export_ptstops', True,
                                                              groupnames=['input', 'options'],
                                                              perm='rw',
                                                              name='Export PT stops?',
                                                              info='Export PT stops before simulation?',
                                                              ))

        if routefilepaths is None:
            routefilepaths = scenario.demand.get_routefilepath()

            self.is_export_rou = attrsman.add(cm.AttrConf('is_export_rou', is_export_rou,
                                                          groupnames=['input', 'options'],
                                                          perm='rw',
                                                          name='Export routes?',
                                                          info='Export current routes before simulation? Needs to be done only once after demand has changed.',
                                                          ))
        else:
            self.is_export_rou = False

        if netfilepath is None:
            netfilepath = scenario.net.get_filepath()

            self.is_export_net = attrsman.add(cm.AttrConf('is_export_net', is_export_net,
                                                          groupnames=['input', 'options'],
                                                          perm='rw',
                                                          name='Export net?',
                                                          info='Export current network before simulation? Needs to be done only once after network has changed.',
                                                          ))
        else:
            self.is_export_net = False

        if polyfilepath is None:
            polyfilepath = scenario.landuse.get_filepath()
            self.is_export_poly = attrsman.add(cm.AttrConf('is_export_poly', is_export_poly,
                                                           groupnames=['input', 'options'],
                                                           perm='rw',
                                                           name='Export buildings?',
                                                           info='Export current buildings before simulation? Needs to be done only once after buildings have changed.',
                                                           ))
        else:
            self.is_export_poly = False

        if is_prompt_filepaths:
            filepathgroupnames = ['input', 'options']
        else:
            filepathgroupnames = ['input', '_private']

        self.dirpath_results = attrsman.add(cm.AttrConf('dirpath_results', rootdirpath,
                                                        groupnames=filepathgroupnames,
                                                        perm='rw',
                                                        name='Result directory',
                                                        metatype='dirpath',
                                                        info="""Directory where general SUMO simulation result files are placed.""",
                                                        ))

        self.netfilepath = attrsman.add(cm.AttrConf('netfilepath', netfilepath,
                                                    groupnames=filepathgroupnames,
                                                    perm='rw',
                                                    name='Netfile',
                                                    wildcards='SUMO net XML files (*.net.xml)|*.net.xml',
                                                    metatype='filepath',
                                                    info="""SUMO network xml file.""",
                                                    ))

        self.ptstopsfilepath = attrsman.add(cm.AttrConf('ptstopsfilepath', ptstopsfilepath,
                                                        groupnames=filepathgroupnames,
                                                        perm='rw',
                                                        name='Stop file',
                                                        wildcards='SUMO XML files (*.add.xml)|*.add.xml',
                                                        metatype='filepath',
                                                        info="""SUMO additional xml file with info on public transport stops.""",
                                                        ))

        self.routefilepaths = attrsman.add(cm.AttrConf('routefilepaths', routefilepaths,
                                                       groupnames=filepathgroupnames,
                                                       perm='rw',
                                                       name='Route file(s)',
                                                       wildcards='Typemap XML files (*.rou.xml)|*.rou.xml',
                                                       metatype='filepaths',
                                                       info='SUMO route xml file.\n'
                                                       + 'If multiple, comma separated files are given'
                                                       + ' then make sure the start time of trips'
                                                       + ' are in increasing chronological order.',
                                                       ))

        self.polyfilepath = attrsman.add(cm.AttrConf('polyfilepath', polyfilepath,
                                                     groupnames=filepathgroupnames,
                                                     perm='rw',
                                                     name='Poly file',
                                                     wildcards='Poly XML files (*.poly.xml)|*.poly.xml',
                                                     metatype='filepath',
                                                     info='SUMO polynomial xml file',
                                                     ))

        # print '  is_export_poly,is_include_poly, filepath_poly',is_export_poly, self.is_include_poly, self.polyfilepath

        if is_runnow:
            self.run()

    def _init_attributes_basic(self, scenario, optionprefix='', **kwargs):
        attrsman = self.get_attrsman()

        if optionprefix == '':
            optionprefix_nice = ''
            prefixgroups = []
        else:
            optionprefix_nice = optionprefix.title()+': '
            prefixgroups = [optionprefix]

        self.time_step = attrsman.add(cm.AttrConf('time_step', kwargs.get('time_step', 0.2),
                                                  groupnames=['options', 'timing']+prefixgroups,
                                                  name=optionprefix_nice+'Time step',
                                                  perm='rw',
                                                  info='Basic simulation time step (1s by default).',
                                                  metatype='time',
                                                  unit='s',
                                                  cml=optionprefix+'--step-length',
                                                  ))

        self.is_ballistic_integrator = attrsman.add(cm.AttrConf('is_ballistic_integrator', kwargs.get('is_ballistic_integrator', False),
                                                                groupnames=['options', 'timing']+prefixgroups,
                                                                name=optionprefix_nice+'Ballistic integrator',
                                                                perm='rw',
                                                                info='Whether to use ballistic method for the  positional update of vehicles (default is a semi-implicit Euler method).',
                                                                cml=optionprefix+'--step-method.ballistic',
                                                                ))

        # print '  ',scenario.demand.vtypes.lanechangemodel.get_value()
        if scenario.demand.vtypes.lanechangemodel.get_value() in ['SL2015', ]:
            width_sublanes_default = 1.0
        else:
            width_sublanes_default = -1.0

        self.width_sublanes = attrsman.add(cm.AttrConf('width_sublanes', kwargs.get('width_sublanes', width_sublanes_default),
                                                       groupnames=['options', 'edges']+prefixgroups,
                                                       cml=optionprefix+'--lateral-resolution',
                                                       perm='rw',
                                                       name=optionprefix_nice+'Sublane width',
                                                       unit='m',
                                                       info='Width of sublanes. Should be less than lane width. If negative the sublanes are disabeled.',
                                                       is_enabled=lambda self: self.width_sublanes > 0,
                                                       ))

        #
        # --max-depart-delay <TIME>  <TIME> may be used to discard vehicles which could not be inserted within <TIME> seconds.

        # self.is_ignore_accidents = attrsman.add(cm.AttrConf(  'is_ignore_accidents', kwargs.get('is_ignore_accidents',False),
        #                                                        name = 'ignore accidents',
        #                                                        perm = 'rw',
        #                                                        groupnames = ['options','misc'],
        #                                                        info = 'Ignore accidents.',
        #                                                        ))

        self.is_collission_check_junctions = attrsman.add(cm.AttrConf('is_collission_check_junctions', kwargs.get('is_collission_check_junctions', True),
                                                                      name=optionprefix_nice+'Collission check junctions',
                                                                      perm='rw',
                                                                      groupnames=['options', 'misc']+prefixgroups,
                                                                      info='Perform collission check at junctions.',
                                                                      cml=optionprefix+'--collision.check-junctions',
                                                                      ))

        self.collission_action = attrsman.add(cm.AttrConf('collission_action', kwargs.get('collission_action', 'warn'),
                                                          name=optionprefix_nice+'Collission action',
                                                          choices=['none', 'warn', 'teleport', 'remove'],
                                                          perm='rw',
                                                          groupnames=['options', 'misc']+prefixgroups,
                                                          info='Specifioes what to do when a collission occurs.',
                                                          cml=optionprefix+'--collision.action',
                                                          ))

#        self.factor_collission_mingap = attrsman.add(cm.AttrConf(  'factor_collission_mingap', kwargs.get('factor_collission_mingap',-1),
#                                                                name = 'factor_collission_mingap',
#                                                                perm = 'rw',
#                                                                groupnames = ['options','misc'],
#                                                                info = 'Sets the fraction of minGap that must be maintained to avoid collision detection. If a negative value is given, the carFollowModel parameter is used.',
#                                                                cml = optionprefix+'--collision.mingap-factor',
#                                                                ))

        # self.is_exclude_emptyedges = attrsman.add(cm.AttrConf(  'is_exclude_emptyedges', kwargs.get('is_exclude_emptyedges',True),
        #                                                        name = 'No empty edges',
        #                                                        perm = 'rw',
        #                                                        groupnames = ['options','misc'],
        #                                                        info = 'Excludes empty edges from being sampled.',
        #                                                        #cml = optionprefix+'--end',
        #                                                        ))

        # self.is_exclude_emptylanes = attrsman.add(cm.AttrConf(  'is_exclude_emptylanes',kwargs.get('is_exclude_emptylanes',True),
        #                                                        name = 'No empty lanes',
        #                                                        perm = 'rw',
        #                                                        groupnames = ['options','misc'],
        #                                                        info = 'Excludes empty edges from being sampled.',
        #                                                        cml = optionprefix+'--end',
        #                                                        ))

        self.seed = attrsman.add(cm.AttrConf('seed', kwargs.get('seed', 0),
                                             name=optionprefix_nice+'Seed',
                                             perm='rw',
                                             groupnames=['options', 'misc']+prefixgroups,
                                             info='Random seed.',
                                             cml=optionprefix+'--seed',
                                             ))

    def _init_special(self, **kwargs):
        """
        Special initializations. To be overridden.
        """
        pass

    def get_results(self):
        return self._results

    def is_ready(self):
        """
        Returns True if process is ready to run.

        """
        # here we can check existance of files
        return True

    def do(self):
        """
        Called by run after is_ready verification
        """

        print 'do... '

        scenario = self.parent

        # exports, if required
        delta = np.zeros(3, dtype=np.float32)
        if self.is_export_net:
            ptstopsfilepath = scenario.net.ptstops.export_sumoxml(self.ptstopsfilepath)
            netfilepath, delta = scenario.net.export_netxml(self.netfilepath, is_return_delta=True)
        else:
            ptstopsfilepath = self.ptstopsfilepath

        if self.is_export_rou & (self.routefilepaths.count(',') == 0):
            scenario.demand.export_routes_xml(self.routefilepaths,
                                              is_route=not self.is_dynaroute)

        if self.is_export_poly & (self.guimode not in ['sumopy+map']):
            polyfilepath = scenario.landuse.export_polyxml(self.polyfilepath, delta=delta)
        else:
            polyfilepath = self.polyfilepath

        # Output data
        if self.is_routedata:
            routesdatapath = self.routesdatapath
        else:
            routesdatapath = None

        if self.is_tripdata:
            tripdatapath = self.tripdatapath
        else:
            tripdatapath = None

        if self.is_edgedata:
            edgedatapath = self.edgedatapath
        else:
            edgedatapath = None

        if self.is_edgenoise:
            edgenoisepath = self.edgenoisepath
        else:
            edgenoisepath = None

        if self.is_edgesemissions:
            edgeemissionspath = self.edgeemissionspath
        else:
            edgeemissionspath = None

        if self.is_electricenergy:
            electricenergypath = self.electricenergypath
        else:
            electricenergypath = None

        if self.is_fcd:
            filepath_output_fcd = self.filepath_output_fcd
        else:
            filepath_output_fcd = None

        if self.is_summary:
            summarypath = self.summarypath
        else:
            summarypath = None

        # print '  is_include_poly, filepath_poly', self.is_include_poly, self.polyfilepath
        if self.is_include_poly & (self.guimode not in ['sumopy+map']):
            if polyfilepath is None:
                filename_poly = None
            elif os.path.isfile(self.polyfilepath):
                filename_poly = os.path.basename(self.polyfilepath)
            else:
                filename_poly = None
        else:
            filename_poly = None
        # print '  guimode',self.guimode,self.guimode == 'sumopy+map',is_maps
        # ['sumopy','sumopy+map','native','osg','nogui']
        if self.guimode in ['sumopy', 'sumopy+map']:
            filepath_gui = os.path.basename(self.parent.net.write_guiconfig(
                is_maps=self.guimode in ['sumopy+map'], delta=delta))
        else:
            filepath_gui = None

        if ptstopsfilepath is None:
            filename_ptstops = None
        elif os.path.isfile(ptstopsfilepath) & self.is_export_ptstops:
            filename_ptstops = os.path.basename(self.ptstopsfilepath)
        else:
            filename_ptstops = None
        # if self.files_input.filepath.get('ptstops')=='':
        #    filepath_ptstops = None
        #    filename_ptstops = None
        # else:
        #    filepath_ptstops = self.files_input.filepath.get('ptstops')
        #    filename_ptstops = os.path.basename(filepath_ptstops)

        # print '  >>%s<<'%self.configfilepath
        #filename_netconfig = os.path.basename(self.filepath_netconfig)

        # print '  filepath_poly =',filepath_poly
        # print '  filename_poly =',filename_poly
        # write configuration file

        write_netconfig(
            self.configfilepath,
            self.netfilepath,
            self.routefilepaths,
            starttime=self.simtime_start,
            stoptime=self.simtime_end,
            time_step=self.time_step,
            filename_ptstops=filename_ptstops,
            pedestrian_model=scenario.demand.vtypes.pedestrian_model.get_value(),
            width_sublanes=self.width_sublanes,
            filename_poly=filename_poly,
            dirname_output=self.dirpath_results,
            time_to_teleport=self.time_to_teleport,
            filepath_output_vehroute=routesdatapath,
            filepath_output_tripinfo=tripdatapath,
            is_tripdata_unfinished=self.is_tripdata_unfinished,
            filepath_output_edgedata=edgedatapath,
            #filepath_output_lanedata = self._get_filepath_output('lanedata'),
            filepath_output_edgeemissions=edgeemissionspath,
            filepath_output_electricenergy=electricenergypath,
            filepath_output_fcd=filepath_output_fcd,
            filepath_output_summary=summarypath,
            #filepath_output_laneemissions = self._get_filepath_output('laneemissions'),
            filepath_output_edgenoise=edgenoisepath,
            #filepath_output_lanenoise = self._get_filepath_output('lanenoise'),
            freq=self.time_sample,
            #is_exclude_emptyedges = self.is_exclude_emptyedges,
            #is_exclude_emptylanes =self.is_exclude_emptylanes,
            seed=self.seed,
            filepath_gui=filepath_gui,
            is_openscenegraph=self.guimode is 'openscene',
            width_pedestrian_striping=scenario.demand.vtypes.width_pedestrian_striping.get_value(),
            slowdownfactor_pedestrian_striping=scenario.demand.vtypes.slowdownfactor_pedestrian_striping.get_value(),
            jamtime_pedestrian_striping=scenario.demand.vtypes.jamtime_pedestrian_striping.get_value(),
            jamtime_pedestrian_crossing_striping=scenario.demand.vtypes.jamtime_pedestrian_crossing_striping.get_value(),
            is_collission_check_junctions=self.is_collission_check_junctions,
            #is_ignore_accidents = self.is_ignore_accidents,
            collission_action=self.collission_action,
            is_ballistic_integrator=self.is_ballistic_integrator,
            is_rerouting=self.is_rerouting,
            probability_rerouting=self.probability_rerouting,
            is_deterministic_rerouting=self.is_deterministic_rerouting,
            period_rerouting=self.period_rerouting,
            preperiod_rerouting=self.preperiod_rerouting,
            adaptationinterval_rerouting=self.adaptationinterval_rerouting,
            adaptationweight_rerouting=self.adaptationweight_rerouting,
            adaptationsteps_rerouting=self.adaptationsteps_rerouting,
            taxiservice=self.parent.simulation.taxiservice,
        )

        if self._cmlfilepath is None:
            print '  call run_cml'
            return self.run_cml(cml=self.get_cml())
        else:
            print '  do not simulate but write cml to', self._cmlfilepath
            f = open(self._cmlfilepath, "w+")
            f.write(self.get_cml()+'\n')

            f.close()
            return True
        # self.import_results()

    def get_scenario(self):
        return self.parent

    def import_results(self, results=None):
        """
        Imports simulation resuts into results object.
        """
        print 'import_results'

        # first a dict is created with available dataname as key
        # and filepath as value

        # with this dict, the import_xml method of results
        # is called, where the files are parsed for specific data

        if results is None:
            results = self._results
            if results is None:
                results = self.parent.simulation.results

        if results is not None:

            resultfilepaths = {}
            if self.is_edgedata:
                if os.path.isfile(self.edgedatapath):
                    resultfilepaths['edgedatapath'] = self.edgedatapath

            if self.is_edgenoise:
                if os.path.isfile(self.edgenoisepath):
                    resultfilepaths['edgenoisepath'] = self.edgenoisepath

            if self.is_edgesemissions:
                if os.path.isfile(self.edgeemissionspath):
                    resultfilepaths['edgeemissionspath'] = self.edgeemissionspath

            if self.is_routedata:
                if os.path.isfile(self.routesdatapath):
                    resultfilepaths['routesdatapath'] = self.routesdatapath

            if self.is_tripdata:
                if os.path.isfile(self.tripdatapath):
                    resultfilepaths['tripdatapath'] = self.tripdatapath

            if self.is_electricenergy:
                if os.path.isfile(self.electricenergypath):
                    resultfilepaths['electricenergypath'] = self.electricenergypath

            if self.is_fcd:
                if os.path.isfile(self.filepath_output_fcd):
                    resultfilepaths['trajectorypath'] = self.filepath_output_fcd

            if len(resultfilepaths) > 0:
                results.import_xml(self, **resultfilepaths)

    # no. do some preprocessing during import_results then take
    # specific actions on demand objects
    # def process_results(self):
    #    self._results.process(self)

    def get_cml(self, is_primed=True):
        """
        Overwritten, providing configuration file instead of cml options.
        """
        if is_primed:
            p = P
        else:
            p = ''
        print 'get_cml p=%s=' % p
        # print '  self.configfilepath=%s='%self.configfilepath
        # print '  self.logfilepath=%s='%self.logfilepath
        if self.guimode is 'nogui':
            command = 'sumo'
        else:
            command = 'sumo-gui'
            if self.is_quit_on_end:
                command += ' --quit-on-end'
            if self.is_start:
                command += ' --start'
        if self.logfilepath != '':
            command += ' --log '+p+self.logfilepath+p

        # went into config...
        # if self.pedestrian_model=='None':
        #    pedmodeloptions = ''
        # else:
        #    pedmodeloptions = '--pedestrian.model %s'%self.pedestrian_model
        # return command + ' ' +pedmodeloptions+' -c '+P+self.configfilepath+P
        return command + ' -c '+p+self.configfilepath+p


class Meso(Sumo):
    """
    Mesoscopic simulation process.
    The mesoscopic simulation model which uses the same input data as the main SUMO model. 
    It computes vehicle movements with queues and runs up to 100 times faster than the microscopic model of SUMO. Additionally, due to using a coarser model for intersections and lane-changing it is more tolerant of network modelling errors than SUMO.

    The simulation model is based on the work of Eissfeldt, Vehicle-based modelling of traffic.
    """

    def _init_special(self, **kwargs):
        """
        Special initializations. To be overridden.
        """
        attrsman = self.get_attrsman()

        self.add_option('edgelength_meso', kwargs.get('edgelength_meso', 100.0),
                        groupnames=['options', 'meso'],
                        cml='--meso-edgelength',
                        name='MESO: Maximum queue length',
                        unit='m',
                        info='Maximum queue length.',
                        )

        self.add_option('is_allow_overtaking', kwargs.get('is_allow_overtaking', True),
                        groupnames=['options', 'meso'],
                        cml='--meso-overtaking',
                        name='MESO: Allow overtaking',
                        info='If True, vehicles may overtake each other (swap their position). This is a randomized process depending on vehicle speeds and density.',
                        )

        self.add_option('enable_junction_control', kwargs.get('enable_junction_control', 'true'),
                        groupnames=['options', 'meso'],
                        choices=['true', 'false'],
                        cml='--meso-junction-control',
                        name='MESO: enable junction control',
                        info='If True, junctions are modeled as in the simplified microsim mode (without internal links).',
                        )

        self.add_option('time_panelty_minor', kwargs.get('time_panelty_minor', 0.0),
                        groupnames=['options', 'meso'],
                        cml='--meso-minor-penalty',
                        name='MESO: junction timepenalty',
                        unit='s',
                        info='fixed time penalty when passing an unprioritzed link.',
                        )

        self.add_option('penalty_tls', kwargs.get('penalty_tls', 0.0),
                        groupnames=['options', 'meso'],
                        cml='--meso-tls-penalty',
                        name='MESO: TLS penalty',
                        info='For values >0, a time penalty is applied according to the average delay time (based on red phase duration) and the minimum headway time is increased to model the maximum capacity (according the proportion of green time to cycle time). The panelty is used as a scaling factor that roughly corresponds to coordination (1.0 corresponds to uncoordinated traffic lights, whereas values approaching 0 model near-perfect coordination).',
                        )

        self.add_option('time_to_impatience', kwargs.get('time_to_impatience', 1000.0),
                        groupnames=['options', 'meso'],
                        cml='--time-to-impatience',
                        name='MESO: time to impacience',
                        unit='s',
                        info='Time to reach an impatience value of 1. Vehicles that reach an impatience value of 1 (maximum) can pass an intersection regardless of foe traffic with higher priority.',
                        )

    def get_cml(self, is_primed=True):
        """
        Overwritten, providing configuration file instead of cml options.
        """

        # currently meso options are provided through the command line options
        # while SUMO options are written to the configuration file
        options = self.get_options()
        optionstr = options.get_optionstring()

        return Sumo.get_cml(self, is_primed) + ' --mesosim true ' + optionstr


class SumoTraci(Sumo):
    """
    SUMO simulation process with interactive control via TRACI.
    """

    def _init_special(self, **kwargs):
        """
        Special initializations. To be overridden.
        """
        pass
        # self.is_simpla = self.get_attrsman().add(cm.AttrConf( 'is_simpla', kwargs.get('is_simpla',False),
        #                                        groupnames = ['options','misc'],
        #                                        name = 'Use simple platooning',
        #                                        perm = 'rw',
        #                                        info = 'Performs simple platooning operations, as configured in Simpla.',
        #                                        ))

    def do(self):
        """
        Called by run after is_ready verification
        """

        Sumo.do(self)

    def run_cml(self, cml):
        scenario = self.parent
        cmllist = cml.split(' ')
        print 'SumoTraci.run_cml', cmllist
        traci.start(cmllist)

        simplaconfig = self.parent.simulation.simplaconfig
        if simplaconfig.is_enabled:
            simplaconfig.prepare_sim()

        self.simtime = self.simtime_start
        self.duration = 1.0+self.simtime_end-self.simtime_start
        self.get_attrsman().status.set('running')
        print '  traci started', self.get_attrsman().status.get()
        simobjects = []

        for simobj in self.parent.simulation.get_simobjects():
            # attention, simpla is not a simulation object, only a demand object
            # print '  prepare_sim',simobj.ident
            simobjects += simobj.prepare_sim(self)

        #simobjects = self._prtservice.prepare_sim(self)
        self.simobjects = []
        for time_sample, simfunc in simobjects:
            self.simobjects.append([self.simtime_start, time_sample, simfunc])
        # print '  simobjects=',self.simobjects
        return True

    def get_cml(self):
        """
        Returns commandline with all options.
        The only difference from this method of the SUMO class is
        that filenames must not be quoted when passed to traci.
        """
        return Sumo.get_cml(self, is_primed=False)

    def aboard(self):
        return self.finish_sim()

    def step(self):
        # called interactively
        # when gui it is called through the timer function
        print 79*'='
        print "simstep", self.simtime, self.simtime_end, self.simtime >= self.simtime_end
        traci.simulationStep()
        for i in xrange(len(self.simobjects)):
            # for time_last, time_sample, simfunc in self.simobjects:
            time_last, time_sample, simfunc = self.simobjects[i]
            # print '  simfunc',simfunc,'time_last',time_last,'dt',self.simtime-time_last,'sim?',self.simtime-time_last > time_sample
            if self.simtime-time_last > time_sample:
                self.simobjects[i][0] = self.simtime
                simfunc(self)

        #self.get_logger().w(100.0*self.simtime/self.duration, key ='progress')

        if self.simtime >= self.simtime_end:  # | (not (traci.simulation.getMinExpectedNumber() > 0)):
            #   if self.simtime >= self.simtime_end:
            print '    end of simulation reached at', self.simtime
            return self.finish_sim()

        self.simtime += self.time_step

    def finish_sim(self):
        print 'finish_sim'
        # for demandobj in self.parent.demand.get_demandobjects():
        #    print '  finish_sim',demandobj.ident
        #    #demandobj.finish_sim(self) # why not for sim objects?
        traci.close()
        print '    traci closed.'
        self.get_attrsman().status.set('success')

        simplaconfig = self.parent.simulation.simplaconfig
        if simplaconfig.is_enabled:
            scenario = self.get_scenario()
            simplaconfig.finish_sim()

        return True

    # def process_step(self):
    #    #print traci.vehicle.getSubscriptionResults(vehID)
    #    print 'process_step time=',self.simtime


class Duaiterate(CmlMixin, Sumo):  # attention, CmlMixin overrides Sumo
    def __init__(self, scenario,
                 routefilepath=None,
                 netfilepath=None,
                 ptstopsfilepath=None,
                 logger=None,
                 results=None,
                 **kwargs):

        self._init_common('duaiterate', name='Dynamic User Equilibrium traffic assignment',
                          parent=scenario,
                          logger=logger,
                          info='The Dynamic User equilibrium traffic Assignment is an iterative micro-simulation process.',
                          )

        # point to script
        self.init_cml(os.path.join(os.environ['SUMO_HOME'], 'tools', 'assign', 'duaIterate.py'),
                      workdirpath=scenario.get_workdirpath())

        self._results = results
        self.time_warmup = 0.0  # needed for compatibility with sumo process
        attrsman = self.get_attrsman()

        simtime_start_default = int(scenario.demand.get_time_depart_first())
        # estimate end of simtime
        simtime_end_default = int(scenario.demand.get_time_depart_last())

        self.add_option('simtime_start', kwargs.get('simtime_start', simtime_start_default),
                        groupnames=['options', 'timing'],
                        name='Start time',
                        perm='rw',
                        info='Start time of simulation in seconds after midnight.',
                        unit='s',
                        cml='--begin',  # no prefix
                        )

        self.add_option('simtime_end', kwargs.get('simtime_end', simtime_end_default),
                        groupnames=['options', 'timing'],
                        name='End time',
                        perm='rw',
                        info='End time of simulation in seconds after midnight.',
                        unit='s',
                        cml='--end',  # no prefix
                        )

        self.add_option('step_first', kwargs.get('step_first', 0),
                        groupnames=['options', 'timing'],
                        name='First step',
                        perm='rw',
                        info='First DUA step. Default is zero. By settting the first step, one can continue iterate from a previous process.',
                        cml='--first-step',
                        )

        self.add_option('step_last', kwargs.get('step_last', 50),
                        groupnames=['options', 'timing'],
                        name='Last step',
                        perm='rw',
                        info='Last DUA step.',
                        cml='--last-step',
                        )

        self.add_option('n_convergence_iterations', kwargs.get('n_convergence_iterations', 10),
                        groupnames=['options', 'processing'],
                        cml='--convergence-iterations',
                        name="convergence iterations",
                        info="Number of iterations to use for convergence calculation.",
                        )

        self.add_option('routesteps', kwargs.get('routesteps', 200),
                        groupnames=['options', 'processing'],
                        cml='--route-steps',
                        name="Route steps",
                        info="Last DUA iteration step.\n"
                        + "This is the maximim number of iterations performed.",
                        )

        self.add_option('time_aggregation', kwargs.get('time_aggregation', 900),
                        groupnames=['options', 'processing'],
                        name='Aggregation time',
                        cml='--aggregation',
                        info='Set main weights aggregation period.',
                        unit='s',
                        )

        self.add_option('is_mesosim', kwargs.get('is_mesosim', False),
                        groupnames=['options', 'processing'],
                        cml='--mesosim',
                        name='mesosim',
                        perm='rw',
                        info='Whether mesosim shall be used.',
                        )

        self.add_option('is_no_internal_link', kwargs.get('is_no_internal_link', False),
                        groupnames=['options', 'processing'],
                        cml='--nointernal-link',
                        name='Simulate no internal links',
                        perm='rw',
                        info='No internal links simulated.',
                        )

        # self.add_option(    'path', kwargs.get('path',''),
        #                    name = 'bin dir',
        #                    perm = 'rw',
        #                    info = 'Directory with SUMO binaries.',
        #                    metatype = 'dirpath',
        #                    )

        self.add_option('time_to_teleport', kwargs.get('time_to_teleport', -1),
                        groupnames=['options', 'timing'],
                        name='teleport',
                        perm='rw',
                        info='Time to teleport in seconds, which is the time after'
                        + 'dedlocks get resolved by teleporting\n'
                        + '-1 means no teleporting takes place',
                        metatype='time',
                        unit='s',
                        cml='--time-to-teleport',
                        is_enabled=lambda self: self.time_to_teleport >= 0,
                        )

        self.add_option('is_absrand', kwargs.get('is_absrand', False),
                        groupnames=['options', 'processing'],
                        cml='--absrand',
                        name='time rand',
                        perm='rw',
                        info='Use current time to generate random number.',
                        )

        # these are duarouter options, but not duaiterate options
        # self.add_option(    'is_remove_loops', kwargs.get('is_remove_loops',False),
        #                    groupnames = ['options','processing'],#
        #                    cml = '--remove-loops',
        #                    name = 'Remove loops',
        #                    perm = 'rw',
        #                    info = 'Remove loops within the route; Remove turnarounds at start and end of the route.',
        #                    )

        # self.add_option(    'is_repair_routes', kwargs.get('is_repair_routes',False),
        #                    groupnames = ['options','processing'],#
        #                    cml = '--repair',
        #                    name = 'Repair routes',
        #                    perm = 'rw',
        #                    info = 'Tries to correct a false route.',
        #                    )
        # self.add_option('is_ignore_errors',kwargs.get('is_ignore_errors',True),
        #                name = 'Ignore disconnected',
        #                info = 'Continue if a route could not be build.',
        #                cml = '--ignore-errors',
        #                groupnames = ['options','processing'],#
        #                perm='rw',
        #                )
        # self.add_option(    'use_no_internallink', kwargs.get('use_no_internallink',False),
        #                    groupnames = ['options','processing'],#
        #                    cml = '--internallink',
        #                    name = 'No internal links',
        #                    perm = 'rw',
        #                    info = 'Not to simulate internal link: true or false.',
        #                    )

        self.add_option('is_mesojunctioncontrol', kwargs.get('is_mesojunctioncontrol', False),
                        groupnames=['options', 'processing'],
                        cml='--mesojunctioncontrol',
                        name='meso junction control?',
                        perm='rw',
                        info='Enable mesoscopic traffic light and priority junciton handling.',
                        )

        self.add_option('is_mesomultiqueue', kwargs.get('is_mesomultiqueue', False),
                        groupnames=['options', 'processing'],
                        cml='--mesomultiqueue',
                        name='meso mesomulti queue?',
                        perm='rw',
                        info='Enable multiple queues at edge ends with mesoscopic simulation.',
                        )

        self.add_option('n_meso_recheck', kwargs.get('n_meso_recheck', 0),
                        groupnames=['options', 'processing'],
                        cml='--meso-recheck',
                        name='meso recheck count',
                        perm='rw',
                        info='Delay before checking whether a jam is gone. (higher values can lead to a big speed increase)',
                        )

        self.add_option('is_zip_old_iterations', kwargs.get('is_zip_old_iterations', False),
                        groupnames=['options', 'processing'],
                        cml='--7',
                        name='zip old iterations',
                        perm='rw',
                        info='zip old iterations using 7zip.',
                        )

        self.add_option('is_no_warnings', kwargs.get('is_no_warnings', False),
                        groupnames=['options', 'processing'],
                        cml='--noWarnings',
                        name='no warnings',
                        perm='rw',
                        info='Disables warnings.',
                        )

        self.add_option('is_weight_memory', kwargs.get('is_weight_memory', False),
                        groupnames=['options', 'processing'],
                        cml='--weight-memory',
                        name='Weight memory',
                        info='Smooth edge weights across iterations.',
                        )

        self.add_option('is_expand_weights', kwargs.get('is_expand_weights', False),
                        groupnames=['options', 'processing'],
                        cml='--weights.expand',
                        name='Expand edge weights',
                        info='Expand edge weights at time interval boundaries.',
                        )

        self.add_option('g_alpha', kwargs.get('g_alpha', 0.5),
                        groupnames=['options', 'processing'],
                        cml='--gA',
                        name="g_alpha",
                        perm='rw',
                        info="Sets Gawron's Alpha",
                        )

        self.add_option('g_beta', kwargs.get('g_beta', 0.9),
                        groupnames=['options', 'processing'],
                        cml='--gBeta',
                        name="g_beta",
                        perm='rw',
                        info="Sets Gawron's Beta",
                        )

        self.add_option('routingalgorithm', kwargs.get('routingalgorithm', 'dijkstra'),
                        groupnames=['options', 'processing'],
                        choices=['dijkstra', 'astar', 'CH', 'CHWrapper'],
                        cml='--routing-algorithm',
                        name='Routing algorithm',
                        info='Routing algorithm.',
                        )

        self.add_option('is_no_summary', kwargs.get('is_no_summary', False),
                        groupnames=['options', 'processing'],
                        cml='--disable-summary',
                        name="No summary",
                        perm='rw',
                        info="No summaries are written by the simulation after each step, which saves time and disk space.",
                        )

        self.add_option('is_no_tripinfo', kwargs.get('is_no_tripinfo', True),
                        groupnames=['options', 'processing'],
                        cml='--disable-tripinfos',
                        name="No Tripinfo",
                        perm='rw',
                        info="No tripinfos are written by the simulation after each step, which saves time and disk space.",
                        )

        self.add_option('inc_base', kwargs.get('inc_base', -1),
                        groupnames=['options', 'processing'],
                        cml='--inc-base',
                        name="inc base",
                        perm='rw',
                        info="Give the incrementation base. Negative values disable  incremental scaling.",
                        is_enabled=lambda self: self.inc_base >= 0,  # default is disabeled
                        )

        self.add_option('inc_value', kwargs.get('inc_value', 1),
                        groupnames=['options', 'processing'],
                        cml='--incrementation',
                        name="inc value",
                        perm='rw',
                        info="Give the incrementation",
                        )

        self.add_option('time_inc', kwargs.get('time_inc', 0),
                        groupnames=['options', 'processing'],
                        cml='--time-inc',
                        name="inc time",
                        perm='rw',
                        info="Give the time incrementation.",
                        unit='s',
                        )

        self.add_option('format_routefile', kwargs.get('format_routefile', 'None'),
                        groupnames=['options', 'processing'],
                        cml='--vehroute-file',
                        name="Route file format",
                        choices=('None', 'routesonly', 'detailed'),
                        perm='rw',
                        info="Choose the format of the route file.",
                        )

        self.add_option('is_output_lastroute', kwargs.get('is_output_lastroute', False),
                        groupnames=['options', 'processing'],
                        cml='--output-lastRoute',
                        name="Output last route",
                        perm='rw',
                        info="Output the last routes.",
                        )

        self.add_option('is_keep_all_routes', kwargs.get('is_keep_all_routes', False),
                        groupnames=['options', 'processing'],
                        cml='--keep-allroutes',
                        name="Keep all routes",
                        perm='rw',
                        info="Save routes even with near zero probability.",
                        )

        self.add_option('is_clean_alt', kwargs.get('is_clean_alt', True),
                        groupnames=['options', 'processing'],
                        cml='--clean-alt',
                        name="Clean alternative routes",
                        perm='rw',
                        info=" Whether old rou.alt.xml files shall be removed. This saves lots of disk space.",
                        )

        self.add_option('is_binary_routes', kwargs.get('is_binary_routes', False),
                        groupnames=['options', 'processing'],
                        cml='--binary',
                        name="Save routes in binary format",
                        perm='rw',
                        info="Use binary format for intermediate and resulting route files. This saves disk space",
                        )

        self.add_option('is_skip_first_routing', kwargs.get('is_skip_first_routing', False),
                        groupnames=['options', 'processing'],
                        cml='--skip-first-routing',
                        name="Skip first routing",
                        perm='r',
                        info="run simulation with demands before first routing. This means, use present routes for the first simulation step.",
                        )

        self.add_option('is_verbose_router', kwargs.get('is_verbose_router', False),
                        groupnames=['options', 'processing'],
                        cml='--router-verbose',
                        name="Verbose router",
                        perm='rw',
                        info="Verbose log of routing in dua.log file.",
                        )

        self.is_clean_files = attrsman.add(cm.AttrConf('is_clean_files', True,
                                                       groupnames=['input', 'options'],
                                                       perm='rw',
                                                       name='Clean up files',
                                                       info='Cleans up all temporary files and leaves only the route file of the last iteration (i.g. the final result).',
                                                       ))

        # self.add_option(   'districts', kwargs.get('use_districts',''),
        #                    groupnames = ['options','processing'],#
        #                    name = "Use districts",
        #                    perm = 'rw',
        #                    info = "Use districts as sources and targets.",
        #                    )

        # self.add_option(   'additional', kwargs.get('filepath_add',''),
        #                    name = "additional files",
        #                    perm = 'rw',
        #                    info = "Path of additional files.",
        #                    )

        # standart sumo options
        # add basic sumo options
        self._init_attributes_basic(scenario, optionprefix='sumo', **kwargs)

        # filepath stuff
        if ptstopsfilepath is None:
            self.ptstopsfilepath = scenario.net.ptstops.get_stopfilepath()

            self.is_export_ptstops = attrsman.add(cm.AttrConf('is_export_ptstops', kwargs.get('is_export_ptstops', True),
                                                              groupnames=['input', 'options'],
                                                              perm='rw',
                                                              name='Export PT stops?',
                                                              info='Export PT stops before simulation?',
                                                              ))
        else:
            self.ptstopsfilepath = ptstopsfilepath
            self.is_export_ptstops = False

        if routefilepath is None:
            self.routefilepath = scenario.demand.get_routefilepath()
            self.is_export_routes = attrsman.add(cm.AttrConf('is_export_routes', kwargs.get('is_export_routes', True),
                                                             groupnames=['input', 'options'],
                                                             perm='rw',
                                                             name='Export routes?',
                                                             info='Export routes before simulation? ',
                                                             ))

        else:
            self.is_export_routes = False
            self.routefilepath = routefilepath

        if netfilepath is None:
            self.netfilepath = scenario.net.get_filepath()

            self.is_export_net = attrsman.add(cm.AttrConf('is_export_net', kwargs.get('is_export_net', True),
                                                          groupnames=['input', 'options'],
                                                          perm='rw',
                                                          name='Export net?',
                                                          info='Export current network before simulation? Needs to be done only once after network has changed.',
                                                          ))
        else:
            self.is_export_net = False
            self.netfilepath = netfilepath

        # logfilepath not available in duaiterate but as SUMO arg
        # self.logfilepath = attrsman.add(cm.AttrConf('logfilepath',kwargs.get('logfilepath',''),
        #                                        groupnames = ['options','misc'],
        #                                        perm='rw',
        #                                        name = 'Log file',
        #                                        wildcards = 'Log file (*.txt)|*.txt',
        #                                        metatype = 'filepath',
        #                                        info = "Writes all messages to Log filepath, implies verbous. If blank, no logfile is created",
        #                                        ))

    def get_last_step(self):
        # very fragile method
        #filenames = os.listdir(self.workdir)
        # filenames.sort()
        #step = int(filenames[-1].split('_')[1])
        if 1:
            step_max = 0
            for filename in os.listdir(self.workdirpath):
                try:
                    step = int(filename)
                except:
                    step = -1
                if step > step_max:
                    step_max = step

        if 0:  # older sumo version
            step_max = 0
            for filename in os.listdir(self.workdirpath):
                if filename.startswith('dump'):
                    # print '  ',filename,
                    step = int(filename.split('_')[1])
                    if step > step_max:
                        step_max = step

        return min(step_max, self.step_last)

    def do(self):

        # /usr/local/share/sumo/tools/assign/duaIterate_analysis.py

        scenario = self.parent

        # exports, if required
        if self.is_export_net:
            ptstopsfilepath = scenario.net.ptstops.export_sumoxml(self.ptstopsfilepath)
            netfilepath = scenario.net.export_netxml(self.netfilepath, is_return_delta=False)
        else:
            ptstopsfilepath = self.ptstopsfilepath

        if self.is_export_routes:
            # scenario.demand.export_routes_xml( filepath=self.routefilepath,
            #                    demandobjects = [scenario.demand.trips], is_route = True,
            #                    vtypeattrs_excluded = ['times_boarding','times_loading'],# bug of duaiterate!!
            #                    )

            # eport routes of all demand objects

            scenario.demand.export_routes_xml(filepath=None, encoding='UTF-8',
                                              is_route=False,  # allow True if route export is implemened in virtual population self.is_skip_first_routing,# produce routes only if first dua routing is skipped
                                              vtypeattrs_excluded=['times_boarding',
                                                                   'times_loading'],  # bug of duaiterate!!
                                              is_plain=True,  # this will prevent exporting stops and plans
                                              is_exclude_pedestrians=True,  # no pedestriann trips, but plans are OK
                                              )

        # get command with duaiterate options
        cml = self.get_cml()

        # do filepath options here
        cml += ' --net-file %s' % (ff(self.netfilepath)) +\
            ' --routes %s' % (ff(self.routefilepath))

        additionalpaths = []
        if os.path.isfile(self.ptstopsfilepath):
            additionalpaths.append(self.ptstopsfilepath)

        if len(additionalpaths) > 0:
            cml += ' --additional %s' % (filepathlist_to_filepathstring(additionalpaths))

        # finally add all sumo command line options
        sumooptions = Options()
        for attrconfig in self.get_attrsman().get_configs(is_all=True):
            if ('sumo' in attrconfig.groupnames) & hasattr(attrconfig, 'cml'):
                # print '  sumooption',attrconfig.attrname,attrconfig.groupnames,'is path',attrconfig.get_metatype() in self.pathmetatypes,'has cmlmap',hasattr(attrconfig,'cmlvaluemap')
                is_enabled = True
                if hasattr(attrconfig, 'is_enabled'):
                    # print ' is_enabled=',attrconfig.is_enabled(self), attrconfig.get_value()
                    is_enabled = attrconfig.is_enabled(self)
                if is_enabled:  # disabeled options are simply not added
                    if hasattr(attrconfig, 'cmlvaluemap'):
                        cmlvaluemap = attrconfig.cmlvaluemap
                    else:
                        cmlvaluemap = None
                    is_filepath = attrconfig.get_metatype() in self.pathmetatypes
                    sumooptions.add_option(attrconfig.attrname, attrconfig.get_value(),
                                           attrconfig.cml, is_filepath=is_filepath, cmlvaluemap=cmlvaluemap)

        cml += ' '+sumooptions.get_optionstring()

        # if self.logfilepath != '':
        #    cml +=  ' --log %s'%(ff(self.logfilepath))

        # print '\n Starting command:',cml
        if self.run_cml(cml):  # call(cml):
            step_last = self.get_last_step()
            if 0:  # old SUMO version
                routefilepath_final = '.'.join(self.routefilepath.split(
                    '.')[:-2]) + "_%03d.rou.xml" % (self.get_last_step(),)
            else:
                # print '  new since 1.8.0 ?'
                # print '    workdirpath',self.workdirpath,'step_last',step_last
                resultdir = os.path.join(self.workdirpath, str(step_last))
                print '    resultdir', resultdir
                filename = self.parent.get_rootfilename() + "_%03d.rou.xml" % (self.get_last_step(),)
                print '    filename', filename
                #routefilepath_final = os.path.join(self.workdirpath, str(step_last),'.'.join(self.routefilepath.split('.')[:-2]) + "_%03d.rou.xml"%(step_last,))
                routefilepath_final = os.path.join(resultdir, filename)

            print '  load routes from file ', routefilepath_final
            if os.path.isfile(routefilepath_final):
                scenario.demand.import_routes_xml(routefilepath_final)

            if self.is_clean_files:
                if 0:  # old version
                    for filename in os.listdir(self.workdirpath):
                        if filename.startswith('dump'):
                            os.remove(os.path.join(self.workdirpath, filename))

                        elif filename.startswith('dua_dump_'):
                            os.remove(os.path.join(self.workdirpath, filename))

                        elif filename.startswith('iteration_'):
                            os.remove(os.path.join(self.workdirpath, filename))

                        elif filename.startswith('summary_'):
                            os.remove(os.path.join(self.workdirpath, filename))

                else:  # new since 1.8.0 ?'
                    for step in xrange(self.step_first, step_last):
                        filepath = os.path.join(self.workdirpath, str(step))
                        print '  delete dir', filepath
                        if os.path.isdir(filepath):
                            # print '    delete now'
                            try:
                                shutil.rmtree(filepath)
                            except OSError as e:
                                print("Error: %s - %s." % (e.filename, e.strerror))

            return True

        else:
            return False

    def import_results(self, results=None):
        """
        Imports simulation resuts into results object.
        """
        print 'import_results'

        # currently nothing is imported in results only routs are
        # reimported in trip database

        if results is None:
            results = self._results
            if results is None:
                results = self.parent.simulation.results
