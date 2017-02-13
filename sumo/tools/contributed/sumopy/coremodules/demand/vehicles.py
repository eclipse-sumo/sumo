import os
import sys
import string
from collections import OrderedDict
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.misc import get_inversemap

from coremodules.network.network import SumoIdsConf, MODES

LANECHANGEMODELS = ['LC2013', 'JE2013', 'DK2008', 'SL2015']

# http://sumo.dlr.de/wiki/Models/Emissions/HBEFA3-based
EMISSIONCLASSES = {
    'HBEFA3/Bus': 'average urban bus (all fuel types)',
    'HBEFA3/Coach': 'average long distance bus (all fuel types)',
    'HBEFA3/HDV': 'average heavy duty vehicle (all fuel types)',
    'HBEFA3/HDV_G': 'average gasoline driven heavy duty vehicle',
    'HBEFA3/HDV_D_EU0': 'diesel driven heavy duty vehicle Euro norm 0',
    'HBEFA3/HDV_D_EU1': 'diesel driven heavy duty vehicle Euro norm 1',
    'HBEFA3/HDV_D_EU2': 'diesel driven heavy duty vehicle Euro norm 2',
    'HBEFA3/HDV_D_EU3': 'diesel driven heavy duty vehicle Euro norm 3',
    'HBEFA3/HDV_D_EU4': 'diesel driven heavy duty vehicle Euro norm 4',
    'HBEFA3/HDV_D_EU5': 'diesel driven heavy duty vehicle Euro norm 5',
    'HBEFA3/HDV_D_EU6': 'diesel driven heavy duty vehicle Euro norm 6',
    'HBEFA3/HDV_D_East': 'Eastern norm heavy duty',
    'HBEFA3/zero': 'zero emission vehicle',
    'HBEFA3/LDV': 'average light duty vehicles (all fuel types)',
    'HBEFA3/LDV_G_EU0': 'gasoline driven light duty vehicle Euro norm 0',
    'HBEFA3/LDV_G_EU1': 'gasoline driven light duty vehicle Euro norm 1',
    'HBEFA3/LDV_G_EU2': 'gasoline driven light duty vehicle Euro norm 2',
    'HBEFA3/LDV_G_EU3': 'gasoline driven light duty vehicle Euro norm 3',
    'HBEFA3/LDV_G_EU4': 'gasoline driven light duty vehicle Euro norm 4',
    'HBEFA3/LDV_G_EU5': 'gasoline driven light duty vehicle Euro norm 5',
    'HBEFA3/LDV_G_EU6': 'gasoline driven light duty vehicle Euro norm 6',
    'HBEFA3/LDV_G_East': 'Eastern norm light duty gasoline',
    'HBEFA3/LDV_D_EU0': 'diesel driven light duty vehicle Euro norm 0',
    'HBEFA3/LDV_D_EU1': 'diesel driven light duty vehicle Euro norm 1',
    'HBEFA3/LDV_D_EU2': 'diesel driven light duty vehicle Euro norm 2',
    'HBEFA3/LDV_D_EU3': 'diesel driven light duty vehicle Euro norm 3',
    'HBEFA3/LDV_D_EU4': 'diesel driven light duty vehicle Euro norm 4',
    'HBEFA3/LDV_D_EU5': ' diesel driven light duty vehicle Euro norm 5',
    'HBEFA3/LDV_D_EU6': 'diesel driven light duty vehicle Euro norm 6',
    'HBEFA3/PC': 'average passenger car (all fuel types)',
    'HBEFA3/PC_Alternative': 'passenger car with alternative fuel technology',
    'HBEFA3/PC_G_EU0': 'gasoline driven passenger car Euro norm 0',
    'HBEFA3/PC_G_EU1': 'gasoline driven passenger car Euro norm 1',
    'HBEFA3/PC_G_EU2': 'gasoline driven passenger car Euro norm 2',
    'HBEFA3/PC_G_EU3': 'gasoline driven passenger car Euro norm 3',
    'HBEFA3/PC_G_EU4': 'gasoline driven passenger car Euro norm 4',
    'HBEFA3/PC_G_EU5': 'gasoline driven passenger car Euro norm 5',
    'HBEFA3/PC_G_EU6': 'gasoline driven passenger car Euro norm 6',
    'HBEFA3/PC_G_East': 'Eastern norm gasoline passenger car',
    'HBEFA3/PC_D_EU0': 'diesel driven passenger car Euro norm 0',
    'HBEFA3/PC_D_EU1': 'diesel driven passenger car Euro norm 1',
    'HBEFA3/PC_D_EU2': 'diesel driven passenger car Euro norm 2',
    'HBEFA3/PC_D_EU3': 'diesel driven passenger car Euro norm 3',
    'HBEFA3/PC_D_EU4': 'diesel driven passenger car Euro norm 4',
    'HBEFA3/PC_D_EU5': 'diesel driven passenger car Euro norm 5',
    'HBEFA3/PC_D_EU6': 'diesel driven passenger car Euro norm 6 ',
}

GUISHAPES = ["pedestrian",
             "bicycle",
             "motorcycle",
             "passenger",
             "passenger/sedan",
             "passenger/hatchback",
             "passenger/wagon",
             "passenger/van",
             "delivery",
             "transport",
             "transport/semitrailer",
             "transport/trailer",
             "bus",
             "bus/city",
             "bus/flexible",
             "bus/overland",
             "rail",
             "rail/light"
             "rail/railcar",
             "rail/city",
             "rail/slow",
             "rail/fast",
             "rail/cargo",
             "evehicle",
             ]

ALIGNMMENTS_LAT = ['center', 'left', 'right', 'compact', 'nice', 'arbitrary']


class VehicleTypes(am.ArrayObjman):

    def __init__(self, parent, net, is_add_default=True, **kwargs):

        self._init_objman(ident='vtypes',
                          parent=parent,
                          name='Vehicle Types',
                          info='Table of all available vehicle types, each with specific physical characteristics. Each vehicle can be used multiple times in the simulation',
                          xmltag=('vTypes', 'vType', 'ids_sumo'),
                          version=0.1,
                          **kwargs)
        self._init_attributes()
        if is_add_default:
            self.add_vtypes_default()

    def _init_attributes(self):
        net = self.parent.get_net()

        # lanechange model is now centralized: all vehicle types have the same
        # lanechange model.
        if self.get_version() < 0.1:
            self.delete('lanechangemodels')
            self.delete('alignments_lat')
            self.delete('speeds_max_lat')
            self.delete('gaps_min_lat')

        self.add(cm.AttrConf('lanechangemodel', LANECHANGEMODELS[0],
                             groupnames=['parameters'],
                             choices=LANECHANGEMODELS,
                             name='Lanechange model',
                             info="Lanechange model. The choice of the lanechange model will also determine the choice of lanechange parameters. With model SL2015, sublanes will be simulated.",
                             #xmltag = 'laneChangeModel',
                             ))

        self.add_col(SumoIdsConf('vtype', name='Type name', perm='rw'))

        self.add_col(am.ArrayConf('lengths', 5.0,
                                  groupnames=['parameters'],
                                  name='Length',
                                  unit='m',
                                  info="The vehicle's netto-length",
                                  xmltag='length',
                                  ))

        self.add_col(am.ArrayConf('widths', 2.0,
                                  groupnames=['parameters'],
                                  name='Width',
                                  unit='m',
                                  info="The vehicle's  width.",
                                  xmltag='width',
                                  ))

        self.add_col(am.ArrayConf('heights', 1.5,
                                  groupnames=['parameters'],
                                  name='Height',
                                  unit='m',
                                  info="The vehicle's  height.",
                                  xmltag='height',
                                  ))

        self.add_col(am.ArrayConf('numbers_persons_initial', 1,
                                  groupnames=['parameters'],
                                  name='Passengers',
                                  info="Initial number of persons in the vehicle.",
                                  xmltag='personNumber',
                                  ))

        self.add_col(am.ArrayConf('capacities_persons', 1,
                                  groupnames=['parameters'],
                                  name='Capacity',
                                  info="Maximum number of persons that fit in a vehicle.",
                                  xmltag='personCapacity',
                                  ))

        self.add_col(am.ArrayConf('numbers_container', 0,
                                  groupnames=['parameters'],
                                  name='Containers',
                                  info="Initial number of containers on the vehicle.",
                                  xmltag='containerNumber',
                                  ))

        self.add_col(am.ArrayConf('speeds_max', 70.0,
                                  groupnames=['parameters'],
                                  name='Max. speed',
                                  unit='m/s',
                                  info="The vehicle's maximum velocity",
                                  xmltag='maxSpeed',
                                  ))

        self.add_col(am.ArrayConf('accels', 0.8,
                                  groupnames=['parameters'],
                                  name='Max. accel.',
                                  unit='m/s^2',
                                  info='The acceleration ability of vehicles of this type',
                                  xmltag='accel',
                                  ))

        self.add_col(am.ArrayConf('decels', 4.5,
                                  groupnames=['parameters'],
                                  name='Max. decel.',
                                  unit='m/s^2',
                                  info='The acceleration ability of vehicles of this type',
                                  xmltag='decel',
                                  ))

        self.add_col(am.ArrayConf('taus', 1.0,
                                  groupnames=['parameters'],
                                  name='Reaction',
                                  unit='s',
                                  info="The driver's reaction time in s (actually the minimum time gap)",
                                  xmltag='tau',
                                  ))

        self.add_col(am.ArrayConf('sigmas', 0.5,
                                  groupnames=['parameters'],
                                  name='Driver',
                                  info='The driver imperfection (between 0 and 1). Used only in follower models  SUMOKrauss, SKOrig',
                                  xmltag='sigma',
                                  ))

        self.add_col(am.ArrayConf('dists_min', 2.5,
                                  groupnames=['parameters'],
                                  name='Min. gap',
                                  unit='m',
                                  info="Minimum empty space after leader.",
                                  xmltag='minGap',
                                  ))

        self.add_col(am.ArrayConf('times_boarding', 0.5,
                                  groupnames=['parameters'],
                                  name='boarding time',
                                  unit='s',
                                  info="The time required by a person to board the vehicle.",
                                  xmltag='boardingDuration',
                                  ))

        self.add_col(am.ArrayConf('times_loading',  	90.0,
                                  groupnames=['parameters'],
                                  name='loading time',
                                  unit='s',
                                  info="The time required by a person to board the vehicle.",
                                  xmltag='loadingDuration',
                                  ))

        self.add_col(am.IdsArrayConf('ids_mode', net.modes,
                                     groupnames=['state'],
                                     choices=MODES,
                                     name='Mode',
                                     info='ID of transport mode.',
                                     xmltag='vClass',
                                     ))

        emissionclasses_xml = {}
        for key in EMISSIONCLASSES.keys():
            # yes, map onto itself, otherwise choice values are taken
            emissionclasses_xml[key] = key

        self.add_col(am.ArrayConf('emissionclasses', 'HBEFA3/HDV_D_EU4',
                                  dtype='object',
                                  groupnames=['parameters'],
                                  choices=get_inversemap(EMISSIONCLASSES),
                                  name='Emission',
                                  info="HBEFA3 emission class, see sourceforge.net/apps/mediawiki/sumo/index.php?title=Simulation/Models/Emissions/HBEFA-based",
                                  xmltag='emissionClass',
                                  xmlmap=emissionclasses_xml,
                                  ))

        self.add_col(am.ArrayConf('impatiences', -1000.0,
                                  groupnames=['parameters'],
                                  name='Impatience',
                                  info="Impatience offset between -1000.0 and 1.0 (-100 or less equals off). Impatience grows at 1/teleport. If 1.0 is reached driver will disrigard priorities.",
                                  xmltag='impatience',
                                  ))

        self.add_col(am.ArrayConf('shapes_gui', "passenger",
                                  dtype='object',
                                  groupnames=['parameters'],
                                  name='GUI shape',
                                  choices=GUISHAPES,
                                  info="How this vehicle is rendered.",
                                  xmltag='guiShape',
                                  ))

        self.add_col(am.ArrayConf('colors', np.array((1.0, 1.0, 1.0, 1.0), np.float32),
                                  metatype='color',
                                  groupnames=['parameters'],
                                  name='Color',
                                  info="This vehicle type's color as RGBA tuple with values from 0 to 1.0",
                                  xmltag='color',
                                  ))

        self.add_col(am.ArrayConf('lanechange_strategies', 1.0,
                                  groupnames=['parameters'],
                                  name='Lane strategy',
                                  info="Lanechange model strategy factor. The eagerness for performing strategic lane changing. Higher values result in earlier lane-changing. default: 1.0, range [0-inf]",
                                  xmltag='lcStrategic',
                                  ))
        self.add_col(am.ArrayConf('lanechange_coops', 1.0,
                                  groupnames=['parameters'],
                                  name='Lane coop',
                                  info="Lanechange model cooperative factor.The willingness for performing cooperative lane changing. Lower values result in reduced cooperation. default: 1.0, range [0-1]",
                                  xmltag='lcCooperative',
                                  ))

        self.add_col(am.ArrayConf('lanechange_gains', 1.0,
                                  groupnames=['parameters'],
                                  name='Lane gain',
                                  info="Lanechange model gain factor.The eagerness for performing lane changing to gain speed. Higher values result in more lane-changing. default: 1.0, range [0-inf]",
                                  xmltag='lcSpeedGain',
                                  ))

        self.add_col(am.ArrayConf('lanechange_rightkeepings', 1.0,
                                  groupnames=['parameters'],
                                  name='Lane right',
                                  info="Lanechange model keep right factor.The eagerness for following the obligation to keep right. Higher values result in earlier lane-changing. default: 1.0, range [0-inf]",
                                  xmltag='lcKeepRight',
                                  ))

        self.add_col(am.ArrayConf('sublane_alignments_lat', ALIGNMMENTS_LAT[0],
                                  dtype='object',
                                  groupnames=['parameters'],
                                  choices=ALIGNMMENTS_LAT,
                                  name='sublane alignment',
                                  info='Lateral alignment within a lane. For sublane model only.',
                                  xmltag='latAlignment',
                                  ))

        self.add_col(am.ArrayConf('sublane_speeds_max_lat', 1.0,
                                  groupnames=['parameters'],
                                  name='Sublane max. speed',
                                  unit='m/s',
                                  info="The vehicle's maximum velocity in lateral direction. For sublane model only.",
                                  xmltag='maxSpeedLat',
                                  ))

        self.add_col(am.ArrayConf('sublane_gaps_min_lat', 0.12,
                                  groupnames=['parameters'],
                                  name='Sublane min. gap',
                                  unit='m',
                                  info="The vehicle's minimum distance to other vehicles in lateral direction. For sublane model only.",
                                  xmltag='minGapLat',
                                  ))

        self.add_col(am.ArrayConf('sublane_alignments_eager', 1.0,
                                  groupnames=['parameters'],
                                  name='Sublane eager',
                                  info="The eagerness using the configured lateral alignment within the lane. Higher values result in increased willingness to sacrifice speed for alignment. default: 1.0, range [0-inf]. For sublane model only.",
                                  xmltag='lcSublane',
                                  ))

        self.add_col(am.ArrayConf('sublane_pushyfactors', 0.0,
                                  groupnames=['parameters'],
                                  name='Sublane pushy',
                                  info="Willingness to encroach laterally on other drivers. default: 0.0, range 0 or 1. For sublane model only.",
                                  xmltag='lcPushy',
                                  ))

    def clear_vtypes(self):
        self.clear()

    def add_vtype(self, vtype, **kwargs):
        # print 'add_vtype',vtype
        if kwargs.has_key('mode'):
            id_mode = MODES.get(kwargs['mode'], 1)
            del kwargs['mode']
        else:
            id_mode = 1

        #_id = self.add_row( ids_sumo = vtype, ids_mode = id_mode,**kwargs)

        _id = self.add_row(ids_sumo=vtype,
                           accels=kwargs.get("accel", 2.9),
                           decels=kwargs.get("decel", 7.5),
                           taus=kwargs.get("tau", None),
                           sigmas=kwargs.get("sigma", 0.5),
                           lengths=kwargs.get("length", 4.3),
                           widths=kwargs.get("width", 1.8),
                           heights=kwargs.get("height", 1.50),
                           dists_min=kwargs.get("dist_min", 1.0),
                           speeds_max=kwargs.get("speed_max", 180.0 / 3.6),
                           ids_mode=id_mode,  # specifies mode for demand
                           colors=kwargs.get("color", np.array(
                               (185, 85, 255, 255), np.float32) / 255.0),
                           impatiences=kwargs.get("impatience", -1000),
                           shapes_gui=kwargs.get("shape_gui", 'passenger'),
                           numbers_persons=kwargs.get("number_persons", None),
                           capacities_persons=kwargs.get(
                               "capacity_persons", None),
                           emissionclasses=kwargs.get("emissionclass", None),
                           lanechanges=kwargs.get("lanechange", None),
                           lanechange_strategies=kwargs.get(
                               "lanechange_strategy", None),
                           lanechange_coops=kwargs.get(
                               "lanechange_coop", None),
                           lanechange_gains=kwargs.get(
                               "lanechange_gain", None),
                           lanechange_rightkeepings=kwargs.get(
                               "lanechange_rightkeeping", None),
                           sublane_alignments_lat=kwargs.get(
                               "sublane_alignment_lat", None),
                           sublane_speeds_max_lat=kwargs.get(
                               "sublane_speed_max_lat", None),
                           sublane_gaps_min_lat=kwargs.get(
                               "sublane_gap_min_lat", None),
                           sublane_alignments_eager=kwargs.get(
                               "sublane_alignment_eager", None),
                           sublane_pushyfactors=kwargs.get(
                               "sublane_pushyfactor", None),
                           )

        return _id

    def add_vtypes_default(self):
        # self.del_rows(self.get_ids())
        self.add_vtype('pedestrian',
                       accel=1.5,
                       decel=2.0,
                       sigma=0.5,
                       length=0.6,
                       width=0.8,
                       height=1.719,
                       number_persons=1,
                       capacity_persons=1,
                       dist_min=0.5,
                       speed_max=5.4 / 3.6,
                       mode='pedestrian',  # specifies mode for demand
                       color=np.array((210, 128, 0, 255), np.float32) / 255.0,
                       shape_gui='pedestrian',
                       impatience=1.0,
                       emissionclass='HBEFA3/zero',
                       times_boarding=0.0,
                       times_loading=0.0,
                       sublane_alignment_lat='nice',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.5,
                       sublane_alignment_eager=0.5,
                       sublane_pushyfactor=0.5,
                       )

        self.add_vtype('passenger1',
                       accel=2.9,
                       decel=7.5,
                       sigma=0.5,
                       length=4.3,
                       height=1.50,
                       width=1.8,
                       number_persons=1,
                       capacity_persons=4,
                       dist_min=1.0,
                       speed_max=180.0 / 3.6,
                       mode='passenger',  # specifies mode for demand
                       color=np.array((185, 85, 255, 255), np.float32) / 255.0,
                       shape_gui='passenger',
                       impatience=1.0,
                       emissionclass='HBEFA3/PC',
                       times_boarding=0.5,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       )

        self.add_vtype('bicycle',
                       accel=1.2,
                       decel=3.0,
                       sigma=0.7,
                       length=1.6,
                       width=0.9,
                       height=1.7,
                       number_persons=1,
                       capacity_persons=1,
                       dist_min=0.5,
                       speed_max=18.0 / 3.6,
                       mode='bicycle',  # specifies mode for demand
                       impatience=1.0,
                       emissionclass='HBEFA3/zero',
                       color=np.array((94, 203, 57, 255), np.float32) / 255.0,
                       shape_gui='bicycle',
                       lanechange_strategy=1.0,
                       lanechange_coop=1.0,
                       lanechange_gain=1.0,
                       lanechange_rightkeeping=1.0,
                       times_boarding=20.0,
                       times_loading=0.0,
                       sublane_alignment_lat='right',
                       sublane_speed_max_lat=0.8,
                       sublane_gap_min_lat=0.12,
                       sublane_alignment_eager=1.0,
                       sublane_pushyfactor=0.0,
                       )

        self.add_vtype('vespa',
                       accel=4.5,
                       decel=7.5,
                       sigma=0.7,
                       length=1.5,
                       height=1.7,
                       width=0.95,
                       number_persons=1,
                       capacity_persons=1,
                       dist_min=0.5,
                       speed_max=60.0 / 3.6,
                       mode='moped',  # specifies mode for demand
                       impatience=1.0,
                       emissionclass='HBEFA3/LDV_G_EU3',
                       color=np.array((205, 92, 0, 255), np.float32) / 255.0,
                       shape_gui='motorcycle',
                       times_boarding=20.0,
                       times_loading=90.0,
                       sublane_alignment_lat='left',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       sublane_alignment_eager=0.9,
                       sublane_pushyfactor=0.8,
                       )

        self.add_vtype('motorcycle',
                       accel=4.5,
                       decel=7.5,
                       sigma=0.7,
                       length=1.5,
                       height=1.7,
                       width=0.95,
                       number_persons=1,
                       capacity_persons=1,
                       dist_min=0.5,
                       speed_max=180.0 / 3.6,
                       mode='motorcycle',  # specifies mode for demand
                       impatience=1.0,
                       emissionclass='HBEFA3/LDV_G_EU3',
                       color=np.array((205, 92, 0, 255), np.float32) / 255.0,
                       shape_gui='motorcycle',
                       times_boarding=20.0,
                       times_loading=90.0,
                       sublane_alignment_lat='left',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       sublane_alignment_eager=0.9,
                       sublane_pushyfactor=0.8,
                       )

        self.add_vtype('taxi1',
                       accel=1.9,
                       decel=3.5,
                       sigma=0.5,
                       length=5.0,
                       height=1.80,
                       width=1.8,
                       number_persons=1,
                       capacity_persons=4,
                       dist_min=1.0,
                       speed_max=180.0 / 3.6,
                       mode='taxi',  # specifies mode for demand
                       color=np.array((185, 185, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='passenger/sedan',
                       impatience=1.0,
                       emissionclass='HBEFA3/PC',
                       times_boarding=30.0,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.8,
                       sublane_gap_min_lat=0.12,
                       )

        self.add_vtype('bus',
                       accel=1.2,
                       decel=4.0,
                       sigma=0.9,
                       length=12.0,
                       height=3.4,
                       width=2.5,
                       number_persons=20,
                       capacity_persons=85,
                       dist_min=0.5,
                       speed_max=80.0 / 3.6,
                       mode='bus',  # specifies mode for demand
                       impatience=1.0,
                       emissionclass='HBEFA3/Bus',
                       color=np.array((255, 192, 0, 255), np.float32) / 255.0,
                       shape_gui='bus',
                       times_boarding=2.0,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.24,
                       sublane_alignment_eager=10.0,
                       )

        self.add_vtype('bus_flexible',
                       accel=1.2,
                       decel=4.0,
                       sigma=0.9,
                       length=17.9,
                       width=2.5,
                       height=3.0,
                       number_persons=40,
                       capacity_persons=140,
                       dist_min=0.5,
                       speed_max=80.0 / 3.6,
                       mode='bus',  # specifies mode for demand
                       impatience=1.0,
                       emissionclass='HBEFA3/Bus',
                       color=np.array((255, 192, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='bus/flexible',
                       times_boarding=2.0,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.24,
                       sublane_alignment_eager=100.0,
                       )

        self.add_vtype('tram1',
                       accel=1.0,
                       decel=3.0,
                       sigma=0.9,
                       length=22.0,
                       width=2.4,
                       height=3.2,
                       number_persons=50,
                       capacity_persons=120,
                       dist_min=0.5,
                       speed_max=80.0 / 3.6,
                       mode='tram',  # specifies mode for demand
                       impatience=1.0,
                       emissionclass='HBEFA3/zero',
                       color=np.array((255, 192, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='rail/railcar',
                       times_boarding=1.5,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.24,
                       sublane_alignment_eager=1000000.0,
                       )

        self.add_vtype('rail_urban1',
                       accel=1.0,
                       decel=3.0,
                       sigma=0.9,
                       length=36.0,
                       width=3.0,
                       height=3.6,
                       number_persons=200,
                       capacity_persons=300,
                       dist_min=0.5,
                       speed_max=100.0 / 3.6,
                       mode='rail_urban',  # specifies mode for demand
                       emissionclass='HBEFA3/zero',
                       color=np.array((255, 192, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='rail/railcar',
                       times_boarding=1.5,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.24,
                       sublane_alignment_eager=1000000.0,
                       )

        self.add_vtype('van1',
                       accel=1.9,
                       decel=3.5,
                       sigma=0.5,
                       length=5.0,
                       height=2.50,
                       width=1.9,
                       number_persons=1,
                       capacity_persons=2,
                       dist_min=1.0,
                       speed_max=100.0 / 3.6,
                       mode='delivery',  # specifies mode for demand
                       color=np.array((185, 185, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='passenger/van',
                       impatience=1.0,
                       emissionclass='HBEFA3/LDV_D_EU3',
                       times_boarding=5.0,
                       times_loading=90.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       )

        self.add_vtype('truck',
                       accel=1.5,
                       decel=2.5,
                       sigma=0.5,
                       length=8.0,
                       height=3.50,
                       width=2.0,
                       number_persons=1,
                       capacity_persons=2,
                       dist_min=1.0,
                       speed_max=90.0 / 3.6,
                       mode='truck',  # specifies mode for demand
                       color=np.array((185, 185, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='truck',
                       impatience=1.0,
                       emissionclass='HBEFA3/HDV_D_EU2',
                       times_boarding=5.0,
                       times_loading=180.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       )

        self.add_vtype('truck_semitrailer',
                       accel=1.0,
                       decel=2.0,
                       sigma=0.5,
                       length=10.0,
                       height=4.50,
                       width=2.0,
                       number_persons=1,
                       capacity_persons=2,
                       dist_min=1.0,
                       speed_max=90.0 / 3.6,
                       mode='truck',  # specifies mode for demand
                       color=np.array((185, 185, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='truck/semitrailer',
                       impatience=1.0,
                       emissionclass='HBEFA3/HDV_D_EU2',
                       times_boarding=5.0,
                       times_loading=300.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       )

        self.add_vtype('truck_trailer',
                       accel=1.0,
                       decel=2.0,
                       sigma=0.5,
                       length=12.0,
                       height=3.50,
                       width=2.0,
                       number_persons=1,
                       capacity_persons=2,
                       dist_min=1.0,
                       speed_max=90.0 / 3.6,
                       mode='truck',  # specifies mode for demand
                       color=np.array((185, 185, 255, 255),
                                      np.float32) / 255.0,
                       shape_gui='truck/trailer',
                       impatience=1.0,
                       emissionclass='HBEFA3/HDV_D_EU2',
                       times_boarding=5.0,
                       times_loading=600.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=1.0,
                       sublane_gap_min_lat=0.12,
                       )

        self.add_vtype('evehicle1',
                       accel=2.5,
                       decel=2.5,
                       sigma=1.0,
                       length=3.5,
                       width=1.6,
                       height=1.7,
                       number_persons=1,
                       capacity_persons=4,
                       dist_min=0.5,
                       speed_max=10.0,
                       emissionclass='HBEFA3/zero',
                       mode='evehicle',  # specifies mode for demand
                       color=np.array((255, 240, 0, 255), np.float32) / 255.0,
                       shape_gui='evehicle',
                       times_boarding=1.5,
                       times_loading=20.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.24,
                       sublane_alignment_eager=1000000.0,
                       )

        self.add_vtype('PRT',
                       accel=2.5,
                       decel=2.5,
                       sigma=1.0,
                       length=3.5,
                       width=1.6,
                       height=1.7,
                       number_persons=1,
                       capacity_persons=4,
                       dist_min=0.5,
                       speed_max=10.0,
                       emissionclass='HBEFA3/zero',
                       mode='custom1',  # specifies mode for demand
                       color=np.array((255, 240, 0, 255), np.float32) / 255.0,
                       shape_gui='evehicle',
                       times_boarding=1.5,
                       times_loading=20.0,
                       sublane_alignment_lat='center',
                       sublane_speed_max_lat=0.5,
                       sublane_gap_min_lat=0.24,
                       sublane_alignment_eager=1000000.0,
                       )

    def select_by_mode(self, id_mode=None, mode=None, is_sumoid=False):
        if id_mode == None:
            id_mode = MODES[mode]
        ids = self.select_ids(self.ids_mode.get_value() == id_mode)
        #print 'select_by_mode',id_mode,self.ids_sumo[ids]#
        if is_sumoid:
            return self.ids_sumo[ids]
        else:
            return ids

    # def get_vtypes_by_mode(self,mode):
    #    return self.select_ids(self.modes.value == mode)

    def get_modes(self):
        """
        Returns a list of mode ids for which there are 
        currently vehicles in the database.
        """
        # print 'getClasses',self._types
        return list(set(self.ids_mode.value))

    def get_modechoices(self):
        """
        Returns a dictionary of modes for which there are 
        currently vehicles in the database.
        Key is mode name and value is mode ID
        """
        mode_vtypes = self.get_modes()
        mode_choice = OrderedDict()
        for mode, id_mode in MODES.iteritems():
            if id_mode in mode_vtypes:
                mode_choice[mode] = id_mode
        return mode_choice

    def _write_xml_body(self, fd,  indent, objconfigs, idcolconfig_include_tab, colconfigs,
                        objcolconfigs,
                        xmltag_item, attrconfig_id, xmltag_id, ids, ids_xml):
        print '_write_xml_body ident,ids', self.ident, ids
        print '  xmltag_item,xmltag_id,attrconfig_id',    xmltag_item, xmltag_id, attrconfig_id

        # ids_xml not used here!!
        if ids == None:
            ids = self.get_ids()
        for attrconfig in objconfigs:
            attrconfig.get_value().write_xml(fd, indent + 2)

        # check if columns contain objects
        #objcolconfigs = []
        scalarcolconfigs = colconfigs
        # for attrconfig in colconfigs:
        #    if attrconfig.metatype == 'obj':
        #        objcolconfigs.append(attrconfig)
        #    else:
        #        scalarcolconfigs.append(attrconfig)

        for _id in ids:
            fd.write(xm.start(xmltag_item, indent + 2))

            # print '   make tag and id',_id
            if xmltag_id == '':
                # no id tag will be written
                pass
            elif (attrconfig_id is None) & (xmltag_id is not None):
                # use specified id tag and and specified id values
                fd.write(xm.num(xmltag_id, id_xml))

            elif (attrconfig_id is not None):
                # use id tag and values of attrconfig_id
                attrconfig_id.write_xml(fd, _id)

            # print ' write
            # columns',len(scalarcolconfigs)>0,len(idcolconfig_include_tab)>0,len(objcolconfigs)>0
            for attrconfig in scalarcolconfigs:
                # print '    scalarcolconfig',attrconfig.attrname
                attrconfig.write_xml(fd, _id)

            # insert lanechange model here:
            fd.write(
                xm.num('laneChangeModel', self.lanechangemodel.get_value()))

            if (len(idcolconfig_include_tab) > 0) | (len(objcolconfigs) > 0):
                fd.write(xm.stop())

                for attrconfig in idcolconfig_include_tab:
                    # print '    include_tab',attrconfig.attrname
                    attrconfig.write_xml(fd, _id, indent + 4)

                for attrconfig in objcolconfigs:
                    # print '    objcolconfig',attrconfig.attrname
                    attrconfig[_id].write_xml(fd, indent + 4)
                fd.write(xm.end(xmltag_item, indent + 4))
            else:
                fd.write(xm.stopit())

        # print '  _write_xml_body: done'

    # def write_xml(self, fd, indent, xmltag_id = 'id', ids = None,
    #                is_print_begin_end = True, attrconfigs_excluded = []):
    #
    #    attrconfigs_excluded.append('lanechangemodel')


def parseColor(s):
        # print 'parseColor',s
    arr = s.split(',')
    ret = []
    for a in arr:
            # adapt for old color information:
        if a.find('.') >= 0:
                # color in range 0.0...1.0
            ret.append(float(a) * 255)
        else:
            # color in range 0...255
            ret.append(int(a))

    return tuple(ret)
