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

# @file    bitjoin.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

# .##########################################################################################################################
# /////////////////////////////////////////// DAILY ABM FOR TRANSPORT DIGITAL TWINS ///////////////////////////////////////////#
# //       FRAMEWORK FOR DAILY ACTIVITY-BASED TRAVEL DEMAND GENERATION USING AGGREGATED MOBILITY & LAND-USE BIG DATA      //#
# //////////////////////////////////////////////////// DEVELOPMENT TEAM ////////////////////////////////////////////////////#
# /////////////////////////////////// 2022 VERSION: JOERG SCHWEIZER • CRISTIAN POLIZIANI ///////////////////////////////////#
# /////////////////////////////////////// 2025 VERSION: JOERG SCHWEIZER • AN NGUYEN ////////////////////////////////////////#
# //////////////////////////////////// UNIVERSITY OF BOLOGNA — DICAM (TRANSPORT GROUP) /////////////////////////////////////#
############################################################################################################################

import os
import sys
import time
import numpy as np
import csv
from numpy import random
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from agilepy.lib_base.geometry import is_polyline_in_polygon
from agilepy.lib_base.processes import Process
from collections import defaultdict
from collections import Counter

# from coremodules.modules_common import *
from coremodules.network.network import SumoIdsConf, MODES
from coremodules.network import routing
from coremodules.simulation import results as res
from . import demandbase
from . import virtualpop_results as res
from tqdm import tqdm

# ~ import  virtualpop_results_mpl as vpres
# ~ from coremodules.misc.matplottools import *
# ~ import  matplotlib.pyplot as plt
# ~ from matplotlib.path import Path
# ~ import matplotlib.patches as patchep

GENDERS = {'male': 0, 'female': 1, 'unknown': -1}

OCCUPATIONS = {'unknown': -1,
               'worker': 1,
               'student': 2,
               'employee': 3,
               'public employee': 4,
               'selfemployed': 5,
               'pensioneer': 6,
               'other': 7
               }

NAMES_M = ['GIUSEPPE', 'ANTONIO', 'GIOVANNI', 'FRANCESCO', 'LUIGI', 'MARIO',
           'ROBERTO', 'PAOLO', 'ANGELO', 'FRANCO', 'MARCO', 'VINCENZO', 'DOMENICO',
           'SALVATORE', 'PIETRO', 'CARLO', 'STEFANO', 'MICHELE', 'CLAUDIO', 'GIORGIO',
           'ANDREA', 'ALESSANDRO', 'MASSIMO', 'SERGIO', 'BRUNO', 'MAURO', 'GIANNINO',
           'ULDERIGO', 'LUCIANO', 'JOERG', 'MAURIZIO', 'ALBERTO', 'NICOLA', 'GIANCARLO', 'ENRICO', 'FABIO',
           'RENATO', 'GIANFRANCO', 'LUCA', 'LORENZO', 'ALDO', 'DANIELE', 'ANTONINO',
           'GIACOMO', 'RAFFAELE', 'FABRIZIO', 'CRISTIAN', 'VITTORIO', 'RENZO', 'GABRIELE', 'ADRIANO',
           'ENZO', 'GUIDO', 'GIULIANO', 'RICCARDO', 'GAETANO', 'GIANNI', 'DARIO', 'DAVIDE',
           'SILVANO', 'EMILIO', 'MARCELLO', 'FILIPPO', 'MATTEO', 'PIERO', 'FEDERICO', 'SANDRO',
           'ROCCO', 'CARMINE', 'GIULIO', 'UMBERTO', 'CESARE', 'ALFREDO', 'VITO', 'ORIANO', 'MANUEL',
           'LEONARDO', 'CARMELO', 'WALTER', 'FLAVIO', 'FAUSTO', 'PASQUALE', 'RIGOBERTO', 'ALFIO']

NAMES_F = ['MARIA', 'PAOLA', 'ANNA', 'DANIELA', 'LAURA', 'GIOVANNA', 'FRANCESCA',
           'PATRIZIA', 'ANNA MARIA', 'ANTONELLA', 'GIUSEPPINA', 'ANGELA', 'ELENA',
           'SILVIA', 'LUCIA', 'STEFANIA', 'BARBARA', 'CRISTINA', 'CARLA', 'MONICA',
           'ALESSANDRA', 'ROBERTA', 'MARIA GRAZIA', 'FRANCA', 'CLAUDIA', 'RITA',
           'MARIA TERESA', 'GABRIELLA', 'MARINA', 'TIZIANA', 'CATERINA', 'SILVANA',
           'CHIARA', 'SIMONA', 'ROSA', 'ROSANNA', 'NADIA', 'CINZIA', 'ELISABETTA', 'ELISA',
           'MARIA LUISA', 'LUCIANA', 'MICHELA', 'GIULIANA', 'SARA', 'TERESA', 'EMANUELA',
           'MARISA', 'LUISA', 'MANUELA', 'MARGHERITA', 'DONATELLA', 'GRAZIELLA', 'BENEDETTA',
           'LOREDANA', 'SABRINA', 'ANTONIETTA', 'LISA', 'SANDRA', 'FEDERICA', 'SONIA', 'VALENTINA',
           'RAFFAELLA', 'CARMELA', 'MARIA CRISTINA', 'ORNELLA', 'MARA', 'MARIA ROSA', 'JESSICA',
           'BRUNA', 'ANNAMARIA', 'MARIA ANTONIETTA', 'RENATA', 'ANNALISA', 'MARTA', 'GIULIA',
           'IVANA', 'ENRICA', 'MIRELLA', 'ELEONORA', 'PIERA', 'SIMONETTA', 'FILOMENA', 'GIUSEPPINA',
           'LILIANA', 'MARILENA', 'ILARIA', 'MARIA PIA', 'ISABELLA', 'MILENA', 'LORENA', 'MERI'
           ]


class HouseholdsFromFacilitiesGenerator(Process):
    def __init__(self, ident, virtualpop,  logger=None, **kwargs):
        print('HouseholdsFromFacilitiesGenerator.__init__')

        self._init_common(ident=ident,
                          parent=virtualpop,
                          name='Households from facilities generator',
                          logger=logger,
                          info='Create households from residential facilities by statistics.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        demand = self.parent.parent
        scenario = demand.get_scenario()
        zones = scenario.landuse.zones

        self.is_clear = attrsman.add(cm.AttrConf('is_clear', kwargs.get('is_clear', True),
                                                 groupnames=['options'],
                                                 name='Clear households',
                                                 info='Clear households in the database before creating new ones.',
                                                 ))

        self.is_update_facilities = attrsman.add(cm.AttrConf('is_update_facilities', kwargs.get('is_update_facilities', True),
                                                             groupnames=['options'],
                                                             name='Update facilities',
                                                             info='Update faciliites.',
                                                             ))

        # Select area to generate the VP either entire study area or only single zone

        self.min_area_fac = attrsman.add(cm.AttrConf('min_area_fac', kwargs.get('min_area_fac', 20.0),
                                                     dtype=np.float32,
                                                     groupnames=['options'],
                                                     name='Min Area Facilities',
                                                     info='Minimum area of facilities to be considered for the households creation.',
                                                     ))

        self.max_area_fac = attrsman.add(cm.AttrConf('max_area_fac', kwargs.get('max_area_fac', 100000.0),
                                                     groupnames=['options'],
                                                     name='Max Area Facilities',
                                                     info='Maximum area of facilities to be considered for the households creation.',
                                                     ))

        self.study_area = attrsman.add(cm.AttrConf('study_area', kwargs.get('study_area', 1),
                                                   # choices = {'Apply expicit bounding box': 1, 'Select zone': 2, 'Use all study area': 3},
                                                   choices={'Entire study area': 1, 'Single zone': 2},
                                                   groupnames=['options'],
                                                   name='Study area',
                                                   info='Select area to generate households. If the single zone is selected, the zone boundary needs to be created initially',
                                                   ))

        if len(zones) > 0:
            self.select_zone = attrsman.add(cm.AttrConf('select_zone', kwargs.get('select_zone', zones.ids_sumo[1]),
                                                        groupnames=['options'],
                                                        choices=zones.ids_sumo,
                                                        name='Select single zone',
                                                        info='Select a single zone for creating household if "Single zone" is selected in the step of Study area identification. Only residential buildings within the selected zone will be considered to generate households',
                                                        ))

        if hasattr(zones, 'population'):
            self.method_number_pop = attrsman.add(cm.AttrConf('method_number_pop', kwargs.get('method_number_pop', 3),
                                                              choices={
                                                                  'Use average living density data': 1, 'Use aggregated population data': 2, 'Use disaggregated population data': 3},
                                                              groupnames=['options'],
                                                              name='Input data type',
                                                              info='Choose whether to use the average living density data, aggregated or disaggregated population data to create households',
                                                              ))

        else:
            self.method_number_pop = attrsman.add(cm.AttrConf('method_number_pop', kwargs.get('method_number_pop', 2),
                                                              choices={'Use average living density data': 1,
                                                                       'Use aggregated population data': 2},
                                                              groupnames=['options'],
                                                              name='Input data type',
                                                              info='Choose wether to use the average living density data or aggregated population data to create households',
                                                              ))

        self.living_density = attrsman.add(cm.AttrConf('living_density', kwargs.get('living_density', 12.0),
                                                       groupnames=['options'],
                                                       unit='m^2/pers',
                                                       name='Average living density',
                                                       info='Specify only if the households are created based on the average number of people per facility including all floors in the bulding',
                                                       ))

        self.tot_pop = attrsman.add(cm.AttrConf('tot_pop', kwargs.get('tot_pop', 1000),
                                                groupnames=['options'],
                                                unit='persons',
                                                name='Aggregated Population',
                                                info='Specify only if the households are created from the aggregated city-level population data',
                                                ))

        self.pop_scale = attrsman.add(cm.AttrConf('pop_scale', kwargs.get('pop_scale', 1.0),
                                                  groupnames=['options'],
                                                  name='Population Scale',
                                                  info='Scales population to reflect growth to the present year compared to the year the population data was collected.',
                                                  ))

        self.share_minors = attrsman.add(cm.AttrConf('share_minors', kwargs.get('share_minors', 0.13),
                                                     groupnames=['options'],
                                                     name='Aggregated minor share',
                                                     info='Aggregated share of minors in the population (under 14 years old). In the context of traffic simulations minors are persons who need to be accompaigned by adulds when travelling',
                                                     ))

        self.share_majors = attrsman.add(cm.AttrConf('share_majors', kwargs.get('share_majors', 0.232),
                                                     groupnames=['options'],
                                                     name='Aggregated major share',
                                                     info='Aggregated share of majors in the population (over 65 years old). In the context of traffic simulations majors are generally retired and have different travel needs',
                                                     ))

        self.method_household_sizes = attrsman.add(cm.AttrConf('method_household_sizes', kwargs.get('method_household_sizes', 2),
                                                               choices={'Use average and stdev of household sizes': 1,
                                                                        'Use share of household sizes': 2},
                                                               groupnames=['options'],
                                                               name='Household sizing method',
                                                               info='Choose wether to use the saverage and stdev values or the share of household sizes to create households',
                                                               ))

        self.average_size = attrsman.add(cm.AttrConf('average_size', kwargs.get('average_size', 2.4),
                                                     groupnames=['options'],
                                                     name='Aggregated household size average',
                                                     info='Average size of household from city census statistics',
                                                     ))

        self.stdev_size = attrsman.add(cm.AttrConf('stdev_size', kwargs.get('stdev_size', 1.0),
                                                   groupnames=['options'],
                                                   name='Aggregated household size StDev',
                                                   info='Standard deviation of household size from city census statistics.',
                                                   ))

        self.share_sizes = attrsman.add(cm.AttrConf('share_sizes', kwargs.get('share_sizes', [0.328, 0.277, 0.19, 0.153, 0.039, 0.013]),
                                                    groupnames=['options'],
                                                    name='Aggregated household size shares',
                                                    info='Share of households with sizes [1, 2, 3, 4, 5, 6+] from city census statistics',
                                                    ))

        self.method_household_income = attrsman.add(cm.AttrConf('method_household_income', kwargs.get('method_household_income', 2),
                                                                choices={'Use average and stdev values of household incomes': 1,
                                                                         'Use average incomes for each size and stdev': 2},
                                                                groupnames=['options'],
                                                                name='Household income estimation method',
                                                                info='Choose wether to use the average and stdev values or the average incomes for each size and stdev',
                                                                ))

        self.av_income = attrsman.add(cm.AttrConf('av_income', kwargs.get('av_income', 31785.543),
                                                  groupnames=['options'],
                                                  name='Average income',
                                                  unit='$',
                                                  info='Average income of households.',
                                                  ))

        self.av_incomes = attrsman.add(cm.AttrConf('av_incomes', kwargs.get('av_incomes', [18733.*1.1811, 32599.*1.1811, 39716.*1.1811, 43248.*1.1811, 47081.*1.1811]),
                                                   groupnames=['options'],
                                                   name='Average income by size',
                                                   unit='$',
                                                   info='Average household incomes in dollars for sizes: [1, 2, 3, 4, 5+] from city census statistics',
                                                   ))

        self.std_income = attrsman.add(cm.AttrConf('std_income', kwargs.get('std_income', 1181.1),
                                                   groupnames=['options'],
                                                   name='StDev household income',
                                                   unit='$',
                                                   info='Standard deviation of household incomes.',
                                                   ))

        self.cars_per_person = attrsman.add(cm.AttrConf('cars_per_person', kwargs.get('cars_per_person', 0.76),
                                                        groupnames=['options'],
                                                        unit='cars/person',
                                                        name='Cars per person',
                                                        info='Share of car owners, without considering minors. 0.5 means that 50% of not-minors people own a car. Default value is related to the latest I.Stat information of 2020 for Italy',
                                                        ))

        self.motos_per_person = attrsman.add(cm.AttrConf('motos_per_person', kwargs.get('motos_per_person', 0.14),
                                                         groupnames=['options'],
                                                         unit='motos/person',
                                                         name='Motorcycle per person',
                                                         info='Share of moto owners, without considering minors.  0.5 means that 50% of not-minors people own a moto. Default value is related to the latest I.Stat information of 2020 for Italy',
                                                         ))

        self.bikes_per_person = attrsman.add(cm.AttrConf('bikes_per_person', kwargs.get('bikes_per_person', 0.9),
                                                         groupnames=['options'],
                                                         unit='bikes/person',
                                                         name='Bikes per person',
                                                         info='Average number of bikes per person.',
                                                         ))

    def do(self):
        print('HouseholdsFromFacilitiesGenerator.do')

# > STEP 0: LINKS
        demand = self.parent.parent
        households = demand.virtualpop.get_households()
        scenario = demand.get_scenario()
        landuse = scenario.landuse
        facilities = landuse.facilities
        zones = scenario.landuse.zones
        ids_zone = zones.get_ids()
        names_zone = zones.ids_sumo[ids_zone]
        facilities.identify_taz()
        facilities.identify_closest_edge()

        if self.is_update_facilities:
            facilities.update()
            pois = landuse.pois
            pois.guess_facilities()
            pois.update_landusetype_facilities()

        if self.is_clear:
            households.clear_households()

# > STEP 1: IDENTIFYING ELIGIBLE FACILIITES - RESIDENTIAL WITH MIN AND MAX AREA
        print("Initialization started...Identifying residential buildings with area between minimum and maximum thresholds")
        ids_fac = facilities.get_ids_building()  # Only the buildings are considered, without the facilities with "_area"
        print(f"Initialization started...Total number of buildings: {len(ids_fac)}")
        ids_fac_valid = ids_fac[(facilities.areas[ids_fac] > self.min_area_fac) &
                                (facilities.areas[ids_fac] < self.max_area_fac)]
        print(f"➔ Number of buildings with area between {
              self.min_area_fac} and {self.max_area_fac}: {len(ids_fac_valid)}")
        ids_fac_valid_global = ids_fac_valid[(facilities.ids_landusetype[ids_fac_valid]
                                              == landuse.landusetypes.get_id_from_formatted('residential'))]
        print(f"Initialization started...Number of valid residential buildings: {len(ids_fac_valid_global)}")

# > STEP 2: FILTERING FACILITIES WITHIN THE STUDY AREA
        # Use entire study area
        if self.study_area == 1:
            print('Initialization started...Selected residential buildings in entire study area')
            ids_fac_valid = ids_fac_valid_global
        # Use single zone
        else:
            print('Initialization started...Selected residential buildings inside zone %s' % (self.select_zone))
            ids_fac_valid = ids_fac_valid_global[(
                facilities.ids_zone[ids_fac_valid_global] == zones.ids_sumo.get_id_from_index(self.select_zone))]

# > STEP 3: GENERATING HOUSEHOLD ID, HOUSEHOLD BUILDINGS AND HOUSEHOLD SIZE
        # From living densities
        if self.method_number_pop == 1:
            print('METHOD: Creating households from living densities and assigning residences')
            for id_fac in tqdm(np.random.permutation(ids_fac_valid), desc="Assigning HH by landuse densities"):
                area = facilities.areas[id_fac]
                max_household_size = int(area/self.living_density+0.5)
                assigned_household_size = 0
                while assigned_household_size < max_household_size:
                    household_size = self.get_household_size(self.method_household_sizes,
                                                             average=self.average_size,
                                                             stdev=self.stdev_size,
                                                             shares=self.share_sizes[:]
                                                             )
                    min_required_area = household_size * self.living_density
                    remaining_area = (max_household_size - assigned_household_size) * self.living_density
                    if remaining_area < min_required_area:
                        print(
                            'WARNING: The remaining area of facility ID {id_fac} is insufficient to accommodate more households; moving to the next residential building')
                        break

                # ---Generating household
                    households.add_row(buildings=id_fac,
                                       sizes=household_size,
                                       )

                    assigned_household_size += household_size

        # From aggregated population
        elif self.method_number_pop == 2:
            print('METHOD: Creating households from the aggregated population and assign the residence')
            pop = 0
            current_capacities = np.zeros((np.max(ids_fac_valid)+1))
            current_capacities[ids_fac_valid] = facilities.capacities[ids_fac_valid]

            with tqdm(desc="Assigning HH by aggregated pop.") as pbar:

                while pop < int(self.tot_pop * self.pop_scale):
                    household_size = self.get_household_size(
                        self.method_household_sizes,
                        average=self.average_size,
                        stdev=self.stdev_size,
                        shares=self.share_sizes[:]
                    )

                    total_capacity = np.sum(current_capacities[ids_fac_valid])
                    probs = np.array(current_capacities[ids_fac_valid]/total_capacity, dtype=np.float32)
                    id_fac = int(np.random.choice(ids_fac_valid, 1, p=probs))

                # ---Generating household
                    households.add_row(buildings=id_fac,
                                       sizes=household_size,
                                       zone_name=self.select_zone
                                       )
                    current_capacities[id_fac] = max(current_capacities[id_fac] - household_size, 0.0)
                    pop += household_size
                    pbar.update(household_size)

        # From disaggregated population
        else:
            print('METHOD: Creating households from the disaggregated population and assign the residence')

            if self.study_area == 2:  # If single zone is sellected, Only generating HH to facilities within selected zones, If not, use the entire study area
                ids_zone = [zones.ids_sumo.get_id_from_index(self.select_zone)]
                names_zone = [self.select_zone]

            # get disaggregated population from zones
            for id_zone, name_zone in tqdm(zip(ids_zone, names_zone), total=len(ids_zone), desc="Assigning HH by disaggregated pop."):
                zone_population = int(zones.population[id_zone] * self.pop_scale)

                # Get valid facility in the zone
                ids_fac_valid = ids_fac_valid_global[(facilities.ids_zone[ids_fac_valid_global] == id_zone)]
                print("Zone ID:", id_zone, "Zone Popularion:", zone_population, "Home Facilities:", ids_fac_valid.size)

                if zone_population == 0 or ids_fac_valid.size == 0:
                    print('There is neither population nor home facilities inside the zone %s, moving to the next zones' % (id_zone))
                    continue

                pop = 0
                current_capacities = np.zeros((np.max(ids_fac_valid)+1))
                current_capacities[ids_fac_valid] = facilities.capacities[ids_fac_valid]

                # Add current capacities
                while pop < zone_population:
                    household_size = self.get_household_size(self.method_household_sizes,
                                                             average=self.average_size,
                                                             stdev=self.stdev_size,
                                                             shares=self.share_sizes[:]
                                                             )

                    total_capacity = np.sum(current_capacities[ids_fac_valid])
                    probs = np.array(current_capacities[ids_fac_valid]/total_capacity, dtype=np.float32)
                    id_fac = int(np.random.choice(ids_fac_valid, 1, p=probs))

                # ---Generating household
                    households.add_row(buildings=id_fac,
                                       sizes=household_size,
                                       zone_name=name_zone
                                       )
                    current_capacities[id_fac] = max(current_capacities[id_fac] - household_size, 0.0)
                    pop += household_size

        ids_household = households.get_ids()
        population = np.sum(households.sizes[ids_household])
        print(f'➔ Successfully generated {len(ids_household)} househoulds with {population} virtual population')

# > STEP 4: ADD INCOME TO HOUSEHOLDS

        # print('Add incomes to households...')
        household_sizes = households.sizes[ids_household]
        incomes = households.incomes

        for i, id_household in enumerate(tqdm(ids_household, desc="Assigning HH incomes")):
            if self.method_household_income == 1:
                av_income = self.av_income
            elif self.method_household_income == 2:
                if household_sizes[i] == 1:
                    av_income = self.av_incomes[0]
                elif household_sizes[i] == 2:
                    av_income = self.av_incomes[1]
                elif household_sizes[i] == 3:
                    av_income = self.av_incomes[2]
                elif household_sizes[i] == 4:
                    av_income = self.av_incomes[3]
                else:
                    av_income = self.av_incomes[4]

            # attention av_income is a string!
            #  these attributes should be configured as a Linst configurator!!
            incomes[id_household] = np.random.normal(float(av_income), self.std_income)

        print(f'➔ Successfully finished income assignment for: {len(ids_household)} househoulds')

# > STEP 5: ADD NUMBER OF MINORS AND MAJORS TO HOUSEHOLD
        minors = 0.
        population = int(np.sum(households.sizes[ids_household]))
        target_minors = int(self.share_minors * population) + 1
        household_incomes = households.incomes[ids_household]
        total_minor_prob = np.sum(household_sizes * household_incomes)

        # print('Add minors (under 14 years old) to the households...')
        with tqdm(desc="Assigning minors") as pbar:
            while minors < target_minors:
                id_household = np.random.choice(ids_household, 1,
                                                p=household_sizes * household_incomes / total_minor_prob
                                                )[0]
                id_household = int(id_household)
                if households.numbers_minor[id_household] < households.sizes[id_household]-1:
                    households.numbers_minor[id_household] += 1
                    minors += 1
                    pbar.update(1)

        print(f'➔ Total number of assigned minors: {minors}')

        majors = 0.
        target_majors = int(self.share_majors * population) + 1

        # print('add majors, who are over 65 years old, to the households...')
        with tqdm(desc="Assigning majors") as pbar:
            while majors < target_majors:
                id_household = np.random.choice(ids_household, 1,
                                                p=1.*household_sizes/population
                                                )[0]
                assigned_members = households.numbers_major[id_household] + households.numbers_minor[id_household]
                if assigned_members < households.sizes[id_household]:
                    households.numbers_major[id_household] += 1
                    majors += 1
                    pbar.update(1)
        print(f'➔ Total number of assigned majors: {majors}')

# > STEP 6: ADD NUMBER OF CARS, BIKES AND MOTOS TO HOUSEHOLDS

        minors = households.numbers_minor[ids_household]
        adults = household_sizes - minors
        total_minor = np.sum(minors)
        total_adult = population - total_minor
        cars_target = self.cars_per_person * total_adult + 1
        motos_target = self.motos_per_person * total_adult + 1
        bikes_target = self.bikes_per_person * population + 1

        car_moto_weights = adults * household_incomes
        car_moto_probs = car_moto_weights / np.sum(car_moto_weights)
        bike_probs = household_sizes / population

        # print('add car ownership to households...')
        cars = 0.
        with tqdm(desc='Assigning cars') as pbar:
            while cars < cars_target:
                ''' Number of cars assignned to HH depend on (i) number of adults, (ii) HH income and (iii) Max cars'''
                id_household = np.random.choice(ids_household, 1, p=1.*car_moto_probs)[0]

                # threshold: check at hh level to meet global ownership rate
                household_size = households.sizes[id_household]
                household_minor = households.numbers_minor[id_household]
                household_max_cars = int(self.cars_per_person * (household_size - household_minor) + 1)

                if households.numbers_car[id_household] < household_max_cars:
                    households.numbers_car[id_household] += 1
                    cars += 1
                    pbar.update(1)
        print(f'➔ Total number of assigned cars: {cars}')

        # print('Add moto ownership to households...')
        motos = 0.
        with tqdm(desc='Assigning motos') as pbar:
            while motos < motos_target:
                ''' Number of motor assignned to HH depend on (i) number of adults, (ii) HH income and (iii) Max motor'''
                id_household = np.random.choice(ids_household, 1, p=car_moto_probs)[0]

                # threshold: check at hh level to meet global ownership rate
                household_size = households.sizes[id_household]
                household_minor = households.numbers_minor[id_household]
                household_max_motos = int(self.motos_per_person * (household_size - household_minor) + 1)

                if households.numbers_moto[id_household] < household_max_motos:
                    households.numbers_moto[id_household] += 1
                    motos += 1
        print(f'➔ Total number of assigned motorcycles: {motos}')

        # print('Add bike ownership to households...')
        bikes = 0.
        with tqdm(desc='Assigning bikes') as pbar:
            while bikes < bikes_target:
                ''' Number of bike assignned to HH depend on HH size'''
                id_household = np.random.choice(ids_household, 1, p=bike_probs)[0]

                # threshold: check at hh level to meet global ownership rate
                household_size = households.sizes[id_household]
                household_max_bikes = int(self.bikes_per_person * household_size + 1)

                if households.numbers_bike[id_household] < household_max_bikes:
                    households.numbers_bike[id_household] += 1
                    bikes += 1
                    pbar.update(1)

        print(f'➔ Total number of assigned bikes: {bikes}')

        return True

    def get_household_size(self, method, average=0.0, stdev=0.0, shares=[]):

        if method == 1:
            size = int(np.random.normal(average, stdev, 1)+0.5)
            if size < 1:
                size = 1
        elif method == 2:
            # print(shares[:], shares)
            size = int(np.random.choice([1, 2, 3, 4, 5, 6], 1, p=shares[:]))

        return size


class VpFromHouseholdsGenerator(Process):
    def __init__(self, ident, virtualpop,  logger=None, **kwargs):
        print('VpFromHouseholdsGenerator.__init__')

        self._init_common(ident=ident,
                          parent=virtualpop,
                          name='Vp from households generator',
                          logger=logger,
                          info='Create a virtual population from households by statistics.',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        self.is_clear = attrsman.add(cm.AttrConf('is_clear', kwargs.get('is_clear', True),
                                                 groupnames=['options'],
                                                 name='Clear population',
                                                 info='Clear population in the database before creating new one.',
                                                 ))

        self.is_update_facilities = attrsman.add(cm.AttrConf('is_update_facilities', kwargs.get('is_update_facilities', False),
                                                             groupnames=['options'],
                                                             name='Update facilities',
                                                             info='Update faciliites.',
                                                             ))

        self.year = attrsman.add(cm.AttrConf('year', kwargs.get('year', 2025),
                                             groupnames=['options'],
                                             name='Current year',
                                             info='Current year to be used to guess the birth year of  the virtual persons.',
                                             ))

        self.share_male = attrsman.add(cm.AttrConf('share_male', kwargs.get('share_male', 0.4837),
                                                   groupnames=['options'],
                                                   name='Share Males',
                                                   info='Share of the populations that has a male gender. Default value is related to the latest I.Stat information of 2011 for Italy',
                                                   ))

        self.share_trips = attrsman.add(cm.AttrConf('share_trips', kwargs.get('share_trips', 0.9),
                                                    groupnames=['options'],
                                                    name='Share Trips',
                                                    info='Share of the populations that is an active traveler.',
                                                    ))

        self.modes_share = attrsman.add(cm.AttrConf('modes_share', kwargs.get('modes_share', [0.35, 0.10, 0.25, 0.08, 0.22]),
                                                    groupnames=['options'],
                                                    unit='%',
                                                    name='Mode share in study are',
                                                    info='The latest mode share in the study area corresponds to the following order: car, motorcycle, bus, bicycle, and walk',
                                                    ))

    def do(self):
        print('VpFromHouseholdsGenerator.do')

# > STEP 0: LINKS

        demand = self.parent.parent
        households = demand.virtualpop.get_households()
        virtualpop = demand.virtualpop

        if self.is_clear:
            virtualpop.clear_population()

        scenario = demand.get_scenario()
        net = scenario.net
        modes = net.modes
        landuse = scenario.landuse
        facilities = landuse.facilities

        if self.is_update_facilities:
            facilities.update()
            pois = landuse.pois
            pois.update_landusetype_facilities()

        ids_hsh = households.get_ids()

        # For each household
        ids_person_cars = []
        ids_person_motos = []
        ids_person_bikes = []

        for id_hsh in ids_hsh:
            size = households.sizes[id_hsh]
            ids_pers = []
            n_minors = households.numbers_minor[id_hsh]
            n_majors = households.numbers_major[id_hsh]
            n_adults = households.sizes[id_hsh]-n_minors-n_majors
            n_cars = households.numbers_car[id_hsh]
            n_bikes = households.numbers_bike[id_hsh]
            n_motos = households.numbers_moto[id_hsh]
            n_curr_minors = 0
            n_curr_majors = 0
            n_curr_adults = 0

            print('---------------------------------------------------------------------------------------------')
            print('Generating demographic information of household %i' % (id_hsh))

            # Only consider the active people from share_trip inputs
            for person in range(size):
                if np.random.rand(1) < self.share_trips:

                    # > STEP 1: GENERATING VIRTUAL POPULATION GENDER

                    id_gender = np.random.choice([GENDERS['male'], GENDERS['female']],
                                                 p=[self.share_male, 1-self.share_male])
                    gender_str = "Male" if id_gender == 0 else "Female" if id_gender == 1 else "Unknown"

# > STEP 2: GENERATING VIRTUAL POPULATION NAME

                    if id_gender == GENDERS['male']:
                        name = np.random.choice(NAMES_M)
                    elif id_gender == GENDERS['female']:
                        name = np.random.choice(NAMES_F)

# > STEP 3: GENERATING VIRTUAL POPULATION AGE

                    available_age_ranges = []
                    if n_minors > n_curr_minors:
                        available_age_ranges += ['minor'] * (n_minors - n_curr_minors)
                    if n_majors > n_curr_majors:
                        available_age_ranges += ['major'] * (n_majors - n_curr_majors)
                    if n_adults > n_curr_adults:
                        available_age_ranges += ['adult'] * (n_adults - n_curr_adults)
                    age_range = np.random.choice(available_age_ranges)

                    if age_range == 'minor':
                        age = np.random.choice(list(range(3, 14)))
                        n_curr_minors += 1
                    elif age_range == 'major':
                        age = np.random.choice(list(range(66, 100)))
                        n_curr_majors += 1
                    elif age_range == 'adult':
                        age = np.random.choice(list(range(14, 66)))
                        n_curr_adults += 1
                    else:
                        print('WARNING: no age has been assigned to a person of the household %i' % (id_hsh))

                    # Saving virtual population demographic information
                    print(f"Demographic information of Household {id_hsh}: Person {
                          person+1}, Name={name}, Gender={gender_str}, Age={age}")

                    id_pers = virtualpop.add_row()
                    virtualpop.ids_gender[id_pers] = id_gender
                    virtualpop.years_birth[id_pers] = self.year-age
                    virtualpop.identifications[id_pers] = name
                    virtualpop.ids_household[id_pers] = id_hsh

                    ids_pers.append(id_pers)

            households.virtual_persons[id_hsh] = ids_pers

# > STEP 3: ADDING VEHICLES TO THE VIRTUAL POPULATION

            ''' Assign cars to eligible people first, then update the set of assigned individuals.
                For motors and bikes, prioritize unassigned people; if vehicles still remain, assign as second vehicles to people who was already assigned.
            '''

            if ids_pers != []:

                ids_pers_car = []
                ids_pers_moto = []
                ids_pers_bike = []
                ids_pers_veh = set()  # A set to store already-assigned-vehicle people.

                for id_pers in ids_pers:
                    age = self.year - virtualpop.years_birth[id_pers]
                    if age >= 6:
                        ids_pers_bike.append(id_pers)
                        if age >= 14:
                            ids_pers_moto.append(id_pers)
                            if age >= 18:
                                ids_pers_car.append(id_pers)

                if n_cars > 0 and len(ids_pers_car) > 0:  # Assign cars to eligible people first
                    ids_person_car = np.random.choice(ids_pers_car, size=min(n_cars, len(ids_pers_car)), replace=False)
                    ids_person_cars.extend(ids_person_car)
                    ids_pers_veh.update(ids_person_car)

                # Prioritize unassigned people, if motors still remain, assign as second vehicles to people who was already assigned
                if n_motos > 0 and len(ids_pers_moto) > 0:
                    elig_ids_pers = [elig_id_pers for elig_id_pers in ids_pers_moto if elig_id_pers not in ids_pers_veh]

                    if len(elig_ids_pers) >= n_motos:
                        ids_person_moto = np.random.choice(elig_ids_pers, size=min(
                            n_motos, len(elig_ids_pers)), replace=False)

                    elif len(elig_ids_pers) == 0:
                        ids_person_moto = np.random.choice(ids_pers_moto, size=min(
                            n_motos, len(ids_pers_moto)), replace=False)

                    else:
                        ids_person_moto = elig_ids_pers
                        remain_motos = n_motos - len(elig_ids_pers)
                        if remain_motos > 0 and len(ids_pers_veh) > 0:
                            remain_ids_person_moto = np.random.choice(
                                list(ids_pers_veh), size=min(remain_motos, len(ids_pers_veh)), replace=False)
                            ids_person_moto.extend(remain_ids_person_moto)

                    ids_person_motos.extend(ids_person_moto)
                    ids_pers_veh.update(ids_person_moto)

                # Prioritize unassigned people, if bikes still remain, assign as second vehicles to people who was already assigned
                if n_bikes > 0 and len(ids_pers_bike) > 0:
                    elig_ids_pers = [elig_id_pers for elig_id_pers in ids_pers_bike if elig_id_pers not in ids_pers_veh]

                    if len(elig_ids_pers) >= n_bikes:
                        ids_person_bike = np.random.choice(elig_ids_pers, size=min(
                            n_bikes, len(elig_ids_pers)), replace=False)

                    elif len(elig_ids_pers) == 0:
                        ids_person_bike = np.random.choice(ids_pers_bike, size=min(
                            n_bikes, len(ids_pers_bike)), replace=False)

                    else:
                        ids_person_bike = elig_ids_pers
                        remain_bikes = n_bikes - len(elig_ids_pers)
                        if remain_bikes > 0 and len(ids_pers_veh) > 0:
                            remain_ids_person_bike = np.random.choice(
                                list(ids_pers_veh), size=min(remain_bikes, len(ids_pers_veh)), replace=False)
                            ids_person_bike.extend(remain_ids_person_bike)

                    ids_person_bikes.extend(ids_person_bike)
                    ids_pers_veh.update(ids_person_bike)

        virtualpop.get_iautos().assign_to_persons(ids_person_cars)
        virtualpop.get_imotos().assign_to_persons(ids_person_motos)
        virtualpop.get_ibikes().assign_to_persons(ids_person_bikes)

# > STEP 4: ASSIGNING PREFERRED MODES TO VIRTUAL POPULATION

        ids_mode = modes.get_ids()
        names_mode = modes.names.get_value()
        ids_person = virtualpop.get_ids()
        passenger = virtualpop.ids_iauto.get_value()
        bicycle = virtualpop.ids_ibike.get_value()
        motorcycle = virtualpop.ids_imoto.get_value()
        bus = self.get_ptstops_for_vp(ids_person)
        modes_share = self.modes_share

        num_pop = len(ids_person)
        preferred_modes = [None] * num_pop
        available_modes = []
        for person in range(num_pop):
            modes = []
            if passenger[person] > 0:
                modes.append("passenger")
            if motorcycle[person] > 0:
                modes.append("motorcycle")
            if person in bus and bus[person]:
                modes.append("bus")
            if bicycle[person] > 0:
                modes.append("bicycle")
            available_modes.append(modes)

        # Identify targeted vehicle numbers from mode share
        transport_modes = ["passenger", "motorcycle", "bus", "bicycle", "pedestrian"]
        target_vehicles = {mode: int(round(float(mode_share) * num_pop))
                           for mode, mode_share in zip(transport_modes, modes_share)}
        print('---------------------------------------------------------------------------------------------')
        print('➔ City mode share:', target_vehicles)

        # Pre-assign vehicle numbers to people, priority for people who have only one available mode
        assigned_vehicles = {mode: 0 for mode in transport_modes}
        for person, mode in enumerate(available_modes):
            if len(mode) == 1:
                if assigned_vehicles[mode[0]] >= target_vehicles[mode[0]]:
                    continue
                preferred_modes[person] = mode[0]
                assigned_vehicles[mode[0]] += 1

        # Assign vehicles to respect mode share and by priority as fastest mode then the last is pedestrian
        for person in np.random.permutation(num_pop):
            if preferred_modes[person] is not None:
                continue
            assigned = False
            for mode in transport_modes[:4]:
                if assigned_vehicles[mode] < target_vehicles[mode] and mode in available_modes[person]:
                    preferred_modes[person] = mode
                    assigned_vehicles[mode] += 1
                    assigned = True
                    break
            if not assigned:
                preferred_modes[person] = "pedestrian"
                assigned_vehicles["pedestrian"] += 1

        print('➔ Successfully assigned vehicles:', assigned_vehicles)

        # To convert to vehicle IDs and save
        ids_preferred_modes = []

        for preferred_mode in preferred_modes:
            for name_mode, id_mode in zip(names_mode, ids_mode):
                if preferred_mode == name_mode:
                    ids_preferred_modes.append(id_mode)

        for id_pers, id_preferred_modes in zip(ids_person, ids_preferred_modes):
            virtualpop.ids_mode_preferred[id_pers] = id_preferred_modes

        return True

    def get_ptstops_for_vp(self, ids_person, dist_walk=1000):
        """
        Returns a dictionary as key is person_id and True, False if closing bus stop or not
        Close stops are considered stops with a walking distance less than dist_walk value
        """

        scenario = self.parent.get_scenario()
        virtualpop = scenario.demand.virtualpop
        households = virtualpop.get_households()

        ids_hh_person = virtualpop.ids_household[ids_person]
        ids_hh_fac = households.buildings[ids_hh_person]

        facilities = scenario.landuse.facilities
        hh_coords = facilities.centroids[ids_hh_fac]

        ptstops = scenario.net.ptstops
        ids_stop = ptstops.get_ids()
        stop_coords = ptstops.centroids[ids_stop]

        ptstops_nearby = {}
        for id_person, hh_coord in zip(ids_person, hh_coords):
            dists = np.sqrt(np.sum((stop_coords - hh_coord)**2, axis=1))
            stops_nearby = (dists <= dist_walk)
            ptstops_nearby[id_person] = np.any(stops_nearby)

        return ptstops_nearby


class DailyActivityFromLandUseGenerator(Process):
    def __init__(self, ident, virtualpop,  logger=None, **kwargs):
        print('DailyActivityFromLandUseGenerator.__init__')

        self._init_common(ident=ident,
                          parent=virtualpop,
                          name='Daily activity generation from land-use facilities',
                          logger=logger,
                          info='Create daily activities for virtual population from land-use facilities',
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))

        demand = self.parent.parent
        scenario = demand.get_scenario()
        zones = scenario.landuse.zones

        self.is_update_facilities = attrsman.add(cm.AttrConf('is_update_facilities', kwargs.get('is_update_facilities', False),
                                                             groupnames=['options'],
                                                             name='Update facilities',
                                                             info='Update faciliites.',
                                                             ))

        self.is_clear_activities = attrsman.add(cm.AttrConf('is_clear_activities', kwargs.get('is_clear_activities', True),
                                                            groupnames=['options'],
                                                            name='Clear activities',
                                                            info='Clear activities',
                                                            ))

        self.is_use_pois = attrsman.add(cm.AttrConf('is_use_pois', kwargs.get('is_use_pois', False),
                                                    groupnames=['options'],
                                                    name='Use POIs',
                                                    info='Use POIs to supplement facilities and refine their attributes for destination choice.',
                                                    ))

        self.year = attrsman.add(cm.AttrConf('year', kwargs.get('year', 2025),
                                             groupnames=['options'],
                                             name='Current year',
                                             info='Current year to be used to guess the birth year of  the virtual persons.',
                                             ))

        self.study_area = attrsman.add(cm.AttrConf('study_area', kwargs.get('study_area', 1),
                                                   # choices = {'Apply expicit bounding box': 1, 'Select zone': 2, 'Use all study area': 3},
                                                   choices={'Entire study area': 1, 'Single zone': 2},
                                                   groupnames=['options'],
                                                   name='Study area',
                                                   info='Area to be considered for the activity destination distribution. If the single zone is selected, the zone boundary needs to be created initially',
                                                   ))

        if len(zones) > 0:
            self.select_zone = attrsman.add(cm.AttrConf('select_zone', kwargs.get('select_zone', zones.ids_sumo[1]),
                                                        groupnames=['options'],
                                                        choices=zones.ids_sumo,
                                                        name='Select single zone',
                                                        info='Select a single zone for creating the activity destination household, if "Single zone" is selected only buildings within the specified zone will be considered for generating activity destination',
                                                        ))

        self.min_area_fac = attrsman.add(cm.AttrConf('min_area_fac', kwargs.get('min_area_fac', 20.),
                                                     groupnames=['options'],
                                                     name='Min facility area',
                                                     unit='m^2',
                                                     info='Minimum area of facilities to be considered for the distribution model',
                                                     ))

        self.max_area_fac = attrsman.add(cm.AttrConf('max_area_fac', kwargs.get('max_area_fac', 100000.),
                                                     groupnames=['options'],
                                                     name='Max facility area',
                                                     unit='m^2',
                                                     info='Maximum area of facilities to be considered for the distribution model.',
                                                     ))

        self.av_floors = attrsman.add(cm.AttrConf('av_floors', kwargs.get('av_floors', [1., 1., 1., 1., 1., 1.]),
                                                  groupnames=['options'],
                                                  name='Av number floors',
                                                  info='Average floors for land uses: [industrial,commercial,education,leisure,residential, mixed]; used to evaluate maximum facility occupancy for the distribution model.',
                                                  ))

        self.time_interval = attrsman.add(cm.AttrConf('time_interval', kwargs.get('time_interval', 2),
                                                      choices={'Morning peak hour': 1, '24h': 2},
                                                      groupnames=['options'],
                                                      name='Time interval',
                                                      info='Choose wether to create a morning peak hour or 24h demand',
                                                      ))

        self.share_diaries_minor = attrsman.add(cm.AttrConf('share_diaries_minor', kwargs.get('share_diaries_minor', [0.02, 0.65, 0.15, 0.1, 0.08]),
                                                            groupnames=['options'],
                                                            name='Share diaries minors',
                                                            info='''Average shares of diaries for the following patterns: [hwh,heh,hlh,hsh,hoh], if 24h demand is selected.''',
                                                            ))

        self.share_diaries_major = attrsman.add(cm.AttrConf('share_diaries_major', kwargs.get('share_diaries_major', [0.15, 0.05, 0.4, 0.2, 0.2]),
                                                            groupnames=['options'],
                                                            name='Share diaries majors',
                                                            info='''Average shares of diaries for the following patterns: [hwh,heh,hlh,hsh,hoh], if 24h demand is selected.''',
                                                            ))

        self.share_diaries_adult = attrsman.add(cm.AttrConf('share_diaries_adult', kwargs.get('share_diaries_adult', [0.7, 0.15, 0.05, 0.05, 0.05]),
                                                            groupnames=['options'],
                                                            name='Share diaries adults',
                                                            info='''Average shares of diaries for the following patterns: [hwh,heh,hlh,hsh,hoh]., if 24h demand is selected''',
                                                            ))

        self.is_generate_secondary_activity = attrsman.add(cm.AttrConf('is_generate_secondary_activity', kwargs.get('is_generate_secondary_activity', True),
                                                                       groupnames=['options'],
                                                                       name='Include secondary activity',
                                                                       info='''If selected, the secondary activities will be generated after identifying non-home-based primary activities''',
                                                                       ))

        self.share_secondary_activity = attrsman.add(cm.AttrConf('share_secondary_activity', kwargs.get('share_secondary_activity', [0.4, 0.5, 0.5, 0.5, 0.]),
                                                                 groupnames=['options'],
                                                                 name='Share secondary activity',
                                                                 info='''Average shares of secondary activity for following primary activities: [hwh,heh,hlh,hsh,hoh]., if 24h demand is selected''',
                                                                 ))

        self.is_use_travel_time_budget = attrsman.add(cm.AttrConf('is_use_travel_time_budget', kwargs.get('is_use_travel_time_budget', True),
                                                                  groupnames=['options'],
                                                                  name='Use travel time budget',
                                                                  info='Use travel time budget in the destination choice model',
                                                                  ))

        self.travel_time_budget_method = attrsman.add(cm.AttrConf('travel_time_budget_method', kwargs.get('travel_time_budget_method', 2),
                                                                  choices={
                                                                      'Homogeneous normal distribution': 1, 'Heterogeneous normal distribution': 2, 'Log-linear regression model': 2},
                                                                  groupnames=['options'],
                                                                  name='Travel time budget method',
                                                                  info='Method used to assign daily travel time budgets (TTB) to individuals. Choose between a simple statistical sampling from a homogeneous/heterogeneous normal distribution (with and without considering age/gender in normal distribution) or a log-linear regression model based on personal attributes.',
                                                                  ))

        self.av_base_travel_time_budget = attrsman.add(cm.AttrConf('av_base_travel_time_budget', kwargs.get('av_base_travel_time_budget', 45*60),
                                                                   groupnames=['options'],
                                                                   name='Avg. base travel time budget',
                                                                   unit='second',
                                                                   info='Average daily travel time budget (in seconds) that individuals typically allocate for commuting purposes, without considering differences in age or gender.',
                                                                   ))

        self.std_base_travel_time_budget = attrsman.add(cm.AttrConf('std_base_travel_time_budget', kwargs.get('std_base_travel_time_budget', 10*60),
                                                        groupnames=['options'],
                                                        name='Std. base travel time budget',
                                                        unit='second',
                                                        info='Standard deviation (in seconds) of the daily travel time budgets, assumed to be identical across all individuals regardless of age or gender.',
                                                                    ))

        self.av_incremental_travel_time_budget = attrsman.add(cm.AttrConf('av_incremental_travel_time_budget', kwargs.get('av_incremental_travel_time_budget', 5*60),
                                                                          groupnames=['options'],
                                                                          name='Avg. incremental travel time budget',
                                                                          unit='second',
                                                                          info='Additional travel time budget (in seconds) applied to specific demographic groups, such as working-age individuals or males, to reflect differentiated travel behavior.',
                                                                          ))

        # self.std_incremental_travel_time_budget = attrsman.add(cm.AttrConf('std_incremental_travel_time_budget',kwargs.get('std_incremental_travel_time_budget',10*60),
        #                                                 groupnames = ['options'],
        #                                                 name = 'Std. incremental base travel time budget',
        #                                                 unit = 'second',
        #                                                 info = 'Standard deviation (in seconds) of the additional travel time budget applied to selected demographic groups, such as working-age individuals or males, to reflect differentiated travel behavior.',
        #                                                 ))

        self.log_linear_regression_parameters = attrsman.add(cm.AttrConf('log_linear_regression_parameters', kwargs.get('log_linear_regression_parameters', [8., 0.09, -0.11, 0.23, -0.02, 0.05, -0.36]),  # β₀, β_gender, β_elderly, β_worker, β_children, β_income, β_car
                                                                         groupnames=['options'],
                                                                         perm='wr',
                                                                         name='Log-linear Regression Parameters',
                                                                         info='Parameters used for log-linear regression model: [β₀ (intercept), β_gender, β_elderly, β_worker, β_children, β_income, β_car]. These coefficients are used to estimate daily travel time budget based on individual attributes.'
                                                                         ))

        self.congestion_factor = attrsman.add(cm.AttrConf('congestion_factor', kwargs.get('congestion_factor', 0.9),
                                                          groupnames=['options'],
                                                          name='Congestion factor',
                                                          info='Conservative factor (<1) aims to reduce travel distance in isochrone-based destination choice to reflect shorter reachable areas under real-world traffic compared to free-flow speeds used in the model.',
                                                          ))

        self.av_activity_duration = attrsman.add(cm.AttrConf('av_activity_duration', kwargs.get('av_activity_duration', [7., 8., 7., 4., 3., 5.]),
                                                             groupnames=['options'],
                                                             name='Avg. activity duration',
                                                             unit='hour',
                                                             info='''Average duration on the following activities: [Home, Work, Education, Leisure, Shopping, Others]; this value will be used for estimate activity duration and dymanic capacity of facilites for destination choice model.''',
                                                             ))

        self.std_activity_duration = attrsman.add(cm.AttrConf('std_activity_duration', kwargs.get('std_activity_duration', [2., 2., 2., 1., 1., 1.5]),
                                                              groupnames=['options'],
                                                              name='Std dev of activity duration',
                                                              unit='h',
                                                              info='Standard deviation of duration on the following activities: [Home, Work, Education, Leisure, Shopping, Others]; this value will be used for estimate activity duration and dymanic capacity of facilites for destination choice model.',
                                                              ))

        self.share_work_landtypes = attrsman.add(cm.AttrConf('share_work_landtypes', kwargs.get('share_work_landtypes', [0.5, 0.25, 0.05, 0.05, 0.05, 0.1]),
                                                             groupnames=['options'],
                                                             name='Work landuse share',
                                                             info='''Average share of work activitys performed on the following landusetypes: [industrial,commercial,education,leisure,residential,mixed].''',
                                                             ))

        self.shares_24h = attrsman.add(cm.AttrConf('shares_24h', kwargs.get('shares_24h', [0.1, 0.05, 0, 0.2, 0.05, 0.2]),
                                                   groupnames=['options'],
                                                   name='Shares nocturnal',
                                                   info='''Share of 24h activities by land use type: [industrial,commercial,education,leisure,residential, mixed]; will be used to estimate daily facility capacity - that determine its attraction''',
                                                   ))

        self.av_opening_hours = attrsman.add(cm.AttrConf('av_opening_hours', kwargs.get('av_opening_hours', [7., 9., 8., 10., 7.5, 8]),
                                                         groupnames=['options'],
                                                         name='Avg. opening hours',
                                                         unit='hour',
                                                         info='''Average opening hours by land use type: [industrial,commercial,education,leisure,residential, mixed], will be used to estimate daily facility capacity - that determine its attraction''',
                                                         ))

        self.av_closing_hours = attrsman.add(cm.AttrConf('av_closing_hours', kwargs.get('av_closing_hours', [17., 20., 17., 20., 23., 20.]),
                                                         groupnames=['options'],
                                                         name='Avg. closing hours',
                                                         unit='hour',
                                                         info='''Average closing hours by land use type: [industrial,commercial,education,leisure,residential, mixed], will be used to estimate daily facility capacity - that determine its attraction''',
                                                         ))

        self.std_opening_hours = attrsman.add(cm.AttrConf('std_opening_hours', kwargs.get('std_opening_hours', [3., 3., 3., 3., 3., 3.]),
                                                          groupnames=['options'],
                                                          name='Std dev. opening hours',
                                                          unit='hour',
                                                          info='''Standard deviation of opening hours by land use type: [industrial,commercial,education,leisure,residential, mixed], will be used to determine the earliest and latest begin time of activities''',
                                                          ))

        self.distribution_method = attrsman.add(cm.AttrConf('distribution_method', kwargs.get('distribution_method', 3),
                                                            groupnames=['options'],
                                                            choices={'Preferred travel distance': 1, 'Available capacity of destinations': 2, 'Available capacity of destinations and travel time budget constrained': 3,
                                                                     'Distant-based Gravitational model': 4, 'Time-based Gravitational model': 5, 'Distant-based Logit model': 6, 'Time-based Logit model': 7},
                                                            name='Distribution method',
                                                            info='''This is to decide what is the most appropriate demand distributin method to identify the location of intermediate activities.''',
                                                            ))

        self.av_travel_distance = attrsman.add(cm.AttrConf('av_travel_distance', kwargs.get('av_travel_distance', 500.),
                                                           groupnames=['options'],
                                                           name='Avg. preferred travel distance',
                                                           unit='m',
                                                           info='Destinations close to the preferred travel distance to be selected',
                                                           ))

        self.std_travel_distance = attrsman.add(cm.AttrConf('std_travel_distance', kwargs.get('std_travel_distance', 100.),
                                                            groupnames=['options'],
                                                            name='Std dev preferred travel distances',
                                                            unit='m',
                                                            info='Standard deviation of preferred travel distances.',
                                                            ))

        self.gravitational_capacity_parameter = attrsman.add(cm.AttrConf('gravitational_capacity_parameter', kwargs.get('gravitational_capacity_parameter', 1.5),
                                                                         groupnames=['options'],
                                                                         name='Capacity parameter in Gravitational model',
                                                                         info='A higher parameter indicates greater attraction to larger-capacity destinations',
                                                                         ))

        self.gravitational_dist_parameter = attrsman.add(cm.AttrConf('gravitational_dist_parameter', kwargs.get('gravitational_dist_parameter', -1.5),
                                                                     groupnames=['options'],
                                                                     name='Distance or time parameter in Gravitational model',
                                                                     info='A lower (more negative) parameter indicates that far destinations are less attractive',
                                                                     ))

        self.logit_area_parameter = attrsman.add(cm.AttrConf('logit_area_parameter', kwargs.get('logit_area_parameter', 0.005),
                                                             groupnames=['options'],
                                                             name='Capacity parameter in Logit model',
                                                             info='A higher parameter indicates greater attraction to larger-capacity destinations',
                                                             ))

        self.logit_dist_parameter = attrsman.add(cm.AttrConf('logit_dist_parameter', kwargs.get('logit_dist_parameter', 1.5),
                                                             groupnames=['options'],
                                                             name='Distance or time parameter in Logit model',
                                                             info='A lower (more negative) parameter indicates that closer destinations are more attractive',
                                                             ))

    def do(self):
        print('---------------------------------------------------------------------------------------------')
        print('DailyActivityFromLandUseGenerator.do')

# > STEP 0: LINKS

        scenario = self.parent.get_scenario()
        demand = scenario.demand
        virtualpop = demand.virtualpop
        net = scenario.net
        edges = net.edges
        modes = net.modes
        landuse = scenario.landuse
        facilities = landuse.facilities

        if self.is_update_facilities:
            facilities.update()

        if self.is_clear_activities:
            virtualpop.clear_activities()

        if self.is_use_pois:
            pois = landuse.pois
            ids_poi = pois.get_ids()
            pois.guess_facilities()
            pois.update_landusetype_facilities()

        if self.is_generate_secondary_activity:
            share_secondary_activity = self.share_secondary_activity
        else:
            share_secondary_activity = [0.0] * 5

        households = virtualpop.get_households()
        ids_hsh = households.get_ids()
        ids_virtual = virtualpop.get_ids()
        ids_virtual_hsh = virtualpop.ids_household
        years_birth = virtualpop.years_birth
        genders = virtualpop.ids_gender

# > STEP 1: INITIALIZING ALL POSSIBLE DESTINATIONS AND THEIR CAPACITIES BY LAND USE TYPE

        # Only the buildings (with "_area") are considered
        ids_fac = facilities.get_ids_building()

        # Define land use types using for activity location choices
        landusetypes_key = ['industrial', 'commercial', 'education', 'leisure', 'residential', 'mixed']

        print('Initialization started...Selected landusetypes for activity locations: %s' % (landusetypes_key))

        diary_types_key = ['hwh', 'heh', 'hlh', 'hsh', 'hoh']
        acitivity_types_key = ['home', 'work', 'education', 'leisure', 'shopping', 'others']

        # Initialize dictionary to store possible destinations classified by diffirent landuse types
        ids_possible_destinations = {}
        dyn_capacities = np.zeros((np.max(ids_fac)+1))

        for landusetype_key in landusetypes_key:
            index_landusetype = landusetypes_key.index(landusetype_key)
            landusetype_id = landuse.landusetypes.typekeys.get_id_from_index(landusetype_key)

            # Filter to get facilities of target landusetype
            ids_possible_fac = ids_fac[(facilities.ids_landusetype[ids_fac] == landusetype_id)]

            # Enrich/add facilities associated to the POI of the target landuse type
            if self.is_use_pois:
                ids_possible_pois = ids_poi[(pois.ids_landusetype[ids_poi] == landusetype_id)]
                poi_buildings = pois.nearest_buildings[ids_possible_pois]
                ids_possible_fac = np.unique(np.concatenate((ids_possible_fac, poi_buildings)))

            # Filter too small and too big facilities
            fac_areas = facilities.areas[ids_possible_fac]
            ids_possible_fac = ids_possible_fac[(fac_areas > self.min_area_fac) & (fac_areas < self.max_area_fac)]

            # Entire study area is considered for activity location
            if self.study_area == 1:
                ids_destinations = ids_possible_fac

            # Only single zone is considered for activity location
            else:
                ids_destinations = ids_possible_fac[(
                    facilities.ids_zone[ids_possible_fac] == landuse.zones.ids_sumo.get_id_from_index(self.select_zone))]

            # The now is to update to the dictionary
            ids_possible_destinations[landusetype_key] = ids_destinations
            print('Initialization started...Identifing facilities for landusetype: %s, ID landusetype: %s, Found: %s facilities' % (
                landusetype_key, landusetype_id, len(ids_destinations)))

            # Update capacities and occupancies of facilities based on base capacity and frequences of visit

            for id_destination, max_hour_capacity in self.get_maximum_capacities(ids_destinations, index_landusetype).items():
                dyn_capacities[id_destination] = max_hour_capacity

        # Decrease capacity of residential facilities where the households homes have been assigned but not <0
        permanence_home = self.av_activity_duration[acitivity_types_key.index('home')]
        for id_hsh in ids_hsh:
            id_fac_home = households.buildings[id_hsh]
            household_size = households.sizes[id_hsh]

            dyn_capacities[id_fac_home] = max(0., dyn_capacities[id_fac_home] - household_size * permanence_home)

        # Update the facility ids for activity destination only valid facilites are
        ids_fac = np.unique(np.concatenate(list(ids_possible_destinations.values())))


# > STEP 2: LINKING THE EDGES AND ASSOCIATED FACILITIES AND CALCULATING EDGE TIMES AND LEVEL OF ACCESS FOR FURTHER DIJKSTRA FORWARD CALCULATION

        # if self.is_use_travel_time_budget:

        print('---------------------------------------------------------------------------------------------')
        print('Initializing the edges and associated facilities ...')

        # Step 2.1 - Finding nearby edges from all building facilities

        n_best = 5
        ids_edge_target = edges.get_ids()
        fstar = edges.get_fstar()

        start_time = time.perf_counter()
        ids_fac_building = []  # ids_fac_building = [id_fac1, id_fac2,...]
        ids_edges_closest = []  # ids_edges_closest = [[id_edge1, id_edge2, id_edge3, id_edge4, id_edge5], [.........]]

        for id_fac in tqdm(ids_fac, desc="Finding nearby edges for facilities"):
            ids_edge_nearby, _ = edges.get_closest_edge(facilities.centroids[id_fac], n_best=n_best)
            ids_edges_closest.append(ids_edge_nearby)
            ids_fac_building.append(id_fac)
            # tqdm.write(f"Finding Closest Edges from Facility ID {id_fac}, Found Edges {ids_edge_nearby}")

        end_time = time.perf_counter()
        print(f"➔ Finding nearby edges finished in {end_time - start_time:.2f} seconds")

        # Step 2.2 - To create a dictionary with the keys are edges and values are nearby facilities
        start_time = time.perf_counter()
        # edge_to_fac = {id_edge1: [id_fac1, id_fac2,...], id_edge2: [id_fac2, id_fac3, id_fac4...],...}
        edge_to_fac = defaultdict(list)

        for id_fac_ref, id_edges_closest in tqdm(zip(ids_fac_building, ids_edges_closest),
                                                 total=len(ids_fac_building),
                                                 desc="Linking nearby edges to facilities"):
            for id_edge_closest in id_edges_closest:
                edge_to_fac[id_edge_closest].append(id_fac_ref)

        end_time = time.perf_counter()
        print(f"➔ Linking nearby edges finished in {end_time - start_time:.2f} seconds")

        # Step 2.3 - Initialize edge time and level of access fur further Dijkstra forward calculation

        virtual_pref_modes = set(virtualpop.ids_mode_preferred.get_value())

        # Estimate edgetimes and accesslevels for each mode in the VP preffered modes, used to generate isochrone
        edgetimes_by_mode = {}
        accesslevels_by_mode = {}

        start_time = time.perf_counter()

        for mode in tqdm(virtual_pref_modes, total=len(virtual_pref_modes), desc="Computing edge times & access levels"):
            edgetimes_by_mode[mode] = edges.get_times(
                id_mode=mode,
                speed_max=modes.speeds_max[mode],
                is_check_lanes=True
            )

            accesslevels_by_mode[mode] = edges.get_accesslevels(mode)

        end_time = time.perf_counter()

        print(f"➔ Finished computing travel time & access level in {end_time - start_time:.2f} seconds")

# > STEP 4: GENERATING DAILY TRAVEL DIARIES

        # Step 4.1: Creating fallback options for reachable egdes and facilities

        home_actype = demand.activitytypes.names.get_id_from_index('home')

        # Backup edge IDs and travel time for the primary activities
        fallback_edges_primary = None
        fallback_edges_time_primary = None
        # Backup edge IDs and travel time for the secondary activities
        fallback_edges_secondary = None
        fallback_edges_time_secondary = None

        # Backup fac IDs and travel time for the primary activities
        fallback_facs_primary = None
        fallback_facs_time_primary = None
        # Backup fac IDs and travel time for the secondary activities
        fallback_facs_secondary = None
        fallback_facs_time_secondary = None

        # Writing results
        output_activity_data = []

        # Now go to each person in the population to generate activity destination and time
        for id_virtual in ids_virtual:
            id_virtual_hsh = ids_virtual_hsh[id_virtual]
            id_fac_home = households.buildings[id_virtual_hsh]
            virtual_pref_mode = virtualpop.ids_mode_preferred[id_virtual]
            virtual_pref_mode_name = modes.names[virtual_pref_mode]
            age = self.year - years_birth[id_virtual]
            gender = genders[id_virtual]

            # Average daily travel time budget (in seconds) that individuals typically allocate for commuting purposes, without considering differences in age or gender.
            if self.travel_time_budget_method == 1:
                av_travel_time_budget = self.av_base_travel_time_budget
                std_base_travel_time_budget = self.std_base_travel_time_budget

                daily_travel_time_budget = max(
                    60*15,
                    round(np.random.normal(av_travel_time_budget, std_base_travel_time_budget) / 60) * 60
                )  # to make sure over 15 mins/day

            # Additional travel time budget (in seconds) applied to specific demographic groups, such as working-age individuals or males, to reflect differentiated travel behavior.
            elif self.travel_time_budget_method == 2:
                av_travel_time_budget = self.av_base_travel_time_budget
                std_base_travel_time_budget = self.std_base_travel_time_budget
                if 18 < age <= 65:
                    av_travel_time_budget += self.av_incremental_travel_time_budget
                    if gender == 0:
                        av_travel_time_budget += self.av_incremental_travel_time_budget

                daily_travel_time_budget = max(
                    60 * 15,
                    round(np.random.normal(av_travel_time_budget, std_base_travel_time_budget) / 60) * 60
                )

            else:
                daily_travel_time_budget = 0

            print('----------------------------------------------------------------------------------------------------------------------------------')
            print('Creating Daily Activities for Virtual Population...')
            print(f"Processing Virtual Population ID: {id_virtual}/{len(ids_virtual)}")
            print(f"➔ Home ID: {id_virtual_hsh}, Home Location: {
                  id_fac_home}, Preferred Transport Mode: {virtual_pref_mode_name}")

            # Step 4.2: Choosing peak hour or daily to create activities

            # Morning peak hour with only home-work activity

            if self.time_interval == 1:
                ids_act = virtualpop.get_activities().add_activities(ids_activitytype=[home_actype, work_actype],
                                                                     ids_facility=[id_fac_home, id_fac],
                                                                     hours_begin_earliest=[-1., 8.],
                                                                     hours_begin_latest=[-1., 9.],
                                                                     durations_min=[10, 6],
                                                                     durations_max=[14, 10])

            # Daily activies with travel diaries

            elif self.time_interval == 2:
                if age < 14:
                    diary = np.random.choice(['hwh', 'heh', 'hlh', 'hsh', 'hoh'], p=self.share_diaries_minor[:])
                elif age > 65:
                    diary = np.random.choice(['hwh', 'heh', 'hlh', 'hsh', 'hoh'], p=self.share_diaries_major[:])
                else:
                    diary = np.random.choice(['hwh', 'heh', 'hlh', 'hsh', 'hoh'], p=self.share_diaries_adult[:])
                # ids_activitytype =[]
                # for step in range(len(diary)):
                #     ids_activitytype.append(demand.activitytypes.symbols.get_id_from_index(diary[step]))

            # Step 4.3: Choosing diary types for daily activies (if 24-h model is sellected)

            diary_types = {
                'hwh': {
                    'primary': {
                        'actype': 'work',  # If no secondary activity, will be : Home-Work-Home
                        'landtype_choices': ['industrial', 'commercial', 'education', 'leisure', 'residential', 'mixed'],
                        'landtype_probs': self.share_work_landtypes
                    },
                    'secondary': {
                        'prob_do_secondary': share_secondary_activity[diary_types_key.index('hwh')],
                        'actypes': {       # Then will be: Home-Work-Shopping-Home/ Home-Work-Leisure-Home
                            'shopping': {
                                'landtype_choices': ['commercial', 'mixed']
                            },
                            'leisure': {
                                'landtype_choices': ['leisure', 'mixed']
                            }
                        }
                    }
                },

                'heh': {  # If no secondary activity, will be : Home-Education-Home
                    'primary': {
                        'actype': 'education',
                        'landtype_choices': ['education']
                    },
                    'secondary': {
                        'prob_do_secondary': share_secondary_activity[diary_types_key.index('heh')],
                        'actypes': {     # Then will be: Home-Education-Shopping-Home/ Home-Education-Leisure-Home
                            'shopping': {
                                'landtype_choices': ['commercial', 'mixed']
                            },
                            'leisure': {
                                'landtype_choices': ['leisure', 'mixed']
                            }
                        }
                    }
                },


                'hlh': {
                    'primary': {  # If no secondary activity, will be : Home-Leisure-Home
                        'actype': 'leisure',
                        'landtype_choices': ['leisure', 'mixed'],
                    },
                    'secondary': {
                        'prob_do_secondary': share_secondary_activity[diary_types_key.index('hlh')],
                        'actypes': {     # Then will be: Home-Leisure-Shopping-Home
                            'shopping': {
                                'landtype_choices': ['commercial', 'mixed']
                            }
                        }
                    }
                },


                'hsh': {
                    'primary': {  # If no secondary activity, will be : Home-Shopping-Home
                        'actype': 'shopping',
                        'landtype_choices': ['commercial', 'mixed'],
                    },
                    'secondary': {
                        'prob_do_secondary': share_secondary_activity[diary_types_key.index('hsh')],
                        'actypes': {    # Then will be: Home-Shopping-Leisure-Home
                                        'leisure': {
                                            'landtype_choices': ['leisure', 'mixed']
                                        }
                        }
                    }
                },

                'hoh': {
                    'primary': {
                        'actype': 'others',
                        'landtype_choices': ['industrial', 'commercial', 'education', 'leisure', 'residential', 'mixed'],
                    },
                    'secondary': {}  # No secondary for this diary
                }
            }

            # Now go to each diary of a population to build the activity chain including primary and secondary activities

            if diary in diary_types:
                primary_diary = diary_types[diary]

                # Step 4.4. Build daily activity chain without home activity (Primary + Secondary by probability), for example: Work-Shopping, Work-Shopping
                activity_chain = [primary_diary['primary']['actype']]  # Always include primary fist

                # Include secondary if any, based on prob_do_secondary
                secondary_attributes = primary_diary.get('secondary', {})
                prob_do_secondary = secondary_attributes.get('prob_do_secondary', 0.0)

                secondary_included = False

                if prob_do_secondary > 0 and np.random.rand() < prob_do_secondary and 'actypes' in secondary_attributes:
                    secondary_actypes = list(secondary_attributes['actypes'].keys())
                    secondary_act_type = np.random.choice(secondary_actypes)
                    activity_chain.append(secondary_act_type)

                    secondary_included = True

                    # Output example for primary and secondary, without home: activity_chain = ["'work'", "shopping"] or ["'education'", "shopping"],...
                else:
                    # No secondary activity added, only primary activities are generated
                    pass

                # Now to assign primary and secondary activity TTB, if no secondary activity, primary TTB = daily TTB
                if secondary_included:
                    # For one-way, if the secondary included, assumed the out-going time occupies 40-50% daily travel time
                    primary_travel_time_budget = int(
                        np.ceil(daily_travel_time_budget * np.random.uniform(0.4, 0.5) / 60)) * 60
                    # For one-way, assumed the out-going time occupies 20-30% daily travel time
                    secondary_travel_time_budget = int(
                        np.ceil(daily_travel_time_budget * np.random.uniform(0.2, 0.3) / 60)) * 60

                else:
                    # For one-way, if the secondary not included, assumed the out-going time occupies 50% daily travel time
                    primary_travel_time_budget = int(np.ceil(daily_travel_time_budget * 0.5 / 60)) * 60
                    secondary_travel_time_budget = None

# > STEP 5: GENERATING DAILY ACTIVITY LOCATION - LOCATION CHOICES

                # Step 5.1: Loop through activity chain, check primary or secondary activity if any and get landuse types, destination and travel time

                planned_activities = []

                # Loop through activity chain
                for actype in activity_chain:

                    # Check if primary activity in the activity chain
                    if actype == primary_diary['primary']['actype']:

                        # Define the landuse type and landuse ID for primary activity
                        act_attributes = primary_diary['primary']
                        landtype_choices = act_attributes.get('landtype_choices')
                        landtype_probs = act_attributes.get('landtype_probs')

                        if landtype_probs and len(landtype_probs) == len(landtype_choices):
                            # Choose landtype by probs - only for work activity
                            selected_landtype = np.random.choice(
                                landtype_choices, p=np.array(landtype_probs)/sum(landtype_probs))
                        else:
                            selected_landtype = random.choice(landtype_choices)

                        id_selected_landtype = landuse.landusetypes.typekeys.get_id_from_index(selected_landtype)

                        # Step 5.3: Generating isochrone to get reacheachable edges, reacheachable facilities and travel time for primary activity

                        reachable_ids_edges, reachable_edges_time, reachable_ids_facs, reachable_facs_time = self.isochrone_calculation(
                            id_fac_origin=id_fac_home,
                            n_best=n_best,
                            accesslevels=accesslevels_by_mode[virtual_pref_mode],
                            travel_time_budget=primary_travel_time_budget * self.congestion_factor,
                            min_travel_time=4*60 * self.congestion_factor,  # to filter the edges too close to home
                            ids_edge_target=ids_edge_target,
                            edgetimes_by_mode=edgetimes_by_mode[virtual_pref_mode],
                            fstar=fstar,
                            edge_to_fac=edge_to_fac,
                            fallback_edges=fallback_edges_primary,
                            fallback_edges_times=fallback_edges_time_primary,
                            id_fac_home=id_fac_home
                        )

                        print(f"➔ Reached {len(reachable_ids_edges)} Edges & {len(reachable_ids_facs)
                                                                              } Facilities from Home in {primary_travel_time_budget} second")

                        # Backup the results for future fallback if any
                        fallback_edges_primary = reachable_ids_edges
                        fallback_edges_time_primary = reachable_edges_time

                        # Step 5.4: Selecting final destination for primary activity and get travel time

                        id_fac_primary, travel_time_primary, selected_reachable_ids_facs, reachable_facs_time = self.get_activity_location(
                            id_fac_origin=id_fac_home,
                            id_selected_landtype=id_selected_landtype,
                            reachable_ids_facs=reachable_ids_facs,
                            reachable_facs_time=reachable_facs_time,
                            travel_time_budget=primary_travel_time_budget * self.congestion_factor,
                            fallback_facs=fallback_facs_primary,
                            fallback_facs_times=fallback_facs_time_primary,
                            ids_possible_destinations=ids_possible_destinations,
                            dyn_capacities=dyn_capacities)

                        print(f"Primary Activity Assigned: Facility ID = {
                              id_fac_primary}, Travel Time = {travel_time_primary:.2f} seconds")

                        # Backup the results for future fallback if any
                        fallback_facs_primary = selected_reachable_ids_facs
                        fallback_facs_time_primary = reachable_facs_time

                        # Step 5.5: Update the dynamic capacity and save the generated values of the primary activity to planned_activities

                        actype_primary = actype
                        primary_hour_capacity = self.av_activity_duration[acitivity_types_key.index(actype_primary)]

                        dyn_capacities[id_fac_primary] = max(0, dyn_capacities[id_fac_primary] - primary_hour_capacity)
                        dyn_capacities[id_fac_home] += self.av_activity_duration[acitivity_types_key.index('home')]

                        planned_activities.append({
                            'actype': actype_primary,
                            'facility_id': id_fac_primary,
                            'travel_time': travel_time_primary,
                            'landtype': selected_landtype
                        })
                        print("planned_activities", planned_activities)

                    # Check if secondary activity in the activity chain
                    else:
                        # Define the landuse type and landuse ID for secondary activity, choose either 'commercial', 'leisure', mixed
                        act_attributes = secondary_attributes['actypes'][actype]
                        landtype_choices = act_attributes.get('landtype_choices')

                        selected_landtype = random.choice(landtype_choices)
                        id_selected_landtype = landuse.landusetypes.typekeys.get_id_from_index(selected_landtype)

                        # Step 5.6: Generating isochrone to get reacheachable edges, reacheachable facilities and travel time for secondary activity
                        reachable_ids_edges, reachable_edges_time, reachable_ids_facs, reachable_facs_time = self.isochrone_calculation(
                            id_fac_origin=id_fac_primary,
                            n_best=n_best,
                            accesslevels=accesslevels_by_mode[virtual_pref_mode],
                            travel_time_budget=secondary_travel_time_budget * self.congestion_factor,
                            min_travel_time=2*60 * self.congestion_factor,  # to filter the edges too close to primary location
                            ids_edge_target=ids_edge_target,
                            edgetimes_by_mode=edgetimes_by_mode[virtual_pref_mode],
                            fstar=fstar,
                            edge_to_fac=edge_to_fac,
                            fallback_edges=fallback_edges_secondary,
                            fallback_edges_times=fallback_edges_time_secondary,
                            id_fac_home=id_fac_home)

                        print(f"➔ Reached {len(reachable_ids_edges)} Edges & {
                              len(reachable_ids_facs)} Facilities from primary activity location in {secondary_travel_time_budget} second")

                        # Backup the results for future fallback if any
                        fallback_edges_secondary = reachable_ids_edges
                        fallback_edges_time_secondary = reachable_edges_time

                       # Step 5.7: Selecting final destination for secondary activity and get travel time
                        id_fac_secondary, travel_time_secondary, selected_reachable_ids_facs, reachable_facs_time = self.get_activity_location(
                            id_fac_origin=id_fac_primary,
                            id_selected_landtype=id_selected_landtype,
                            reachable_ids_facs=reachable_ids_facs,
                            reachable_facs_time=reachable_facs_time,
                            travel_time_budget=secondary_travel_time_budget * self.congestion_factor,
                            fallback_facs=fallback_facs_secondary,
                            fallback_facs_times=fallback_facs_time_secondary,
                            ids_possible_destinations=ids_possible_destinations,
                            dyn_capacities=dyn_capacities)

                        print(f"Secondary Activity Assigned: Facility ID = {
                              id_fac_secondary}, Travel Time = {travel_time_secondary:.2f} seconds")

                        # Backup the results for future fallback if any

                        fallback_facs_secondary = selected_reachable_ids_facs
                        fallback_facs_time_secondary = reachable_facs_time

                        # Step 5.8: Update the dynamic capacity and save the generated values of the secondary activity to planned_activities

                        actype_secondary = actype
                        secondary_hour_capacity = self.av_activity_duration[acitivity_types_key.index(actype_secondary)]

                        dyn_capacities[id_fac_secondary] = max(
                            0, dyn_capacities[id_fac_secondary] - secondary_hour_capacity)
                        dyn_capacities[id_fac_primary] += primary_hour_capacity

                        planned_activities.append({
                            'actype': actype_secondary,
                            'facility_id': id_fac_secondary,
                            'travel_time': travel_time_secondary,
                            'landtype': selected_landtype
                        })

                        print("planned_activities", planned_activities)

# > STEP 5: SCHEDULING DAILY ACTIVITY - TIME CHOICES

                    # Step 5.8: Generating time scheduling to conduct activities including duration of activity and beginning time

                    activity_durations_min = []
                    activity_durations_max = []
                    activity_begin_earliest = []
                    activity_begin_latest = []

                    # Calculating outgoing travel time
                    outgoing_travel_time = sum([activity['travel_time']
                                               for activity in planned_activities]) / 3600  # convert to hours

                    # Step 1: Calculate durations for each activity first (primary first, then secondary)
                    for activity in planned_activities:

                        # To get activity duration from activity types
                        activity_index = acitivity_types_key.index(activity['actype'])
                        average_duration = float(self.av_activity_duration[activity_index])
                        std_duration = float(self.std_activity_duration[activity_index])

                        duration_min = max(0.5, average_duration - abs(np.random.normal(0, std_duration)))
                        duration_max = average_duration + abs(np.random.normal(0, std_duration))

                        activity_durations_min.append(duration_min)
                        activity_durations_max.append(duration_max)

                    # Step 2: Getting openning hour of primary activity to calculate min/max duration at home in the morning (before first activity)
                    primary_activity = planned_activities[0]
                    index_primary = landusetypes_key.index(primary_activity['landtype'])
                    average_open = float(self.av_opening_hours[index_primary])
                    std_open = float(self.std_opening_hours[index_primary])
                    primary_begin_earliest = max(0.0, average_open - abs(np.random.normal(0, std_open)))
                    primary_begin_latest = max(0.0, average_open + abs(np.random.normal(0, std_open)))

                    # Time at home in the morning
                    travel_time_primary = primary_activity['travel_time']
                    home_morning_duration_min = max(0, primary_begin_earliest - travel_time_primary/3600)
                    home_morning_duration_max = max(0, primary_begin_latest - travel_time_primary/3600)

                    # Step 3: Assign begin times for sequential activities (primary -> secondary)
                    activity_begin_earliest.append(primary_begin_earliest)
                    activity_begin_latest.append(primary_begin_latest)

                    # Only if the secondary activity exists, then look at the secondry activity
                    if len(planned_activities) > 1:
                        for secondary_index in range(1, len(planned_activities)):

                            # Primary activity ends before secondary begins
                            primary_end_earliest = (
                                activity_begin_earliest[secondary_index - 1]
                                + activity_durations_min[secondary_index - 1]
                                + (planned_activities[secondary_index - 1]['travel_time'] / 3600)
                            )
                            primary_end_latest = (
                                activity_begin_latest[secondary_index - 1]
                                + activity_durations_max[secondary_index - 1]
                                + (planned_activities[secondary_index - 1]['travel_time'] / 3600)
                            )

                            secondary_activity = planned_activities[secondary_index]
                            secondary_landusetype = landusetypes_key.index(secondary_activity['landtype'])
                            secondary_open = float(self.av_opening_hours[secondary_landusetype])
                            secondary_std_open = float(self.std_opening_hours[secondary_landusetype])

                            # Get the max of primary duration and openning time of facility to avoid leaving after arriving
                            secondary_begin_earliest = max(
                                primary_end_earliest,
                                secondary_open - abs(np.random.normal(0, secondary_std_open))
                            )
                            secondary_begin_latest = max(
                                primary_end_latest,
                                secondary_open + abs(np.random.normal(0, secondary_std_open))
                            )

                            activity_begin_earliest.append(secondary_begin_earliest)
                            activity_begin_latest.append(secondary_begin_latest)

                    # Step 4: Calculate time at home in the evening (after the last activity)

                    # End time of final secondary activity before returning home
                    secondary_end_earliest = activity_begin_earliest[-1] + activity_durations_min[-1]
                    secondary_end_latest = activity_begin_latest[-1] + activity_durations_max[-1]

                    # Remaining travel time budget for returning home (in hours)
                    incoming_travel_time = daily_travel_time_budget / 3600 - outgoing_travel_time

                    # Estimate earliest and latest possible time to return home
                    home_evening_duration_min = max(0, 24 - secondary_end_latest - incoming_travel_time)
                    home_evening_duration_max = max(0, 24 - secondary_end_earliest - incoming_travel_time)

                    home_evening_begin_earliest = secondary_end_latest + incoming_travel_time
                    home_evening_begin_latest = secondary_end_earliest + incoming_travel_time

                    # Step 5: Commit schedule to virtual population
                    home_id = demand.activitytypes.names.get_id_from_index('home')
                    # Create list of activity type ID doing in a day
                    ids_activitytype = [
                        home_id] + [demand.activitytypes.names.get_id_from_index(activity['actype']) for activity in planned_activities] + [home_id]
                    # Create list of facilities where the activities are taken
                    ids_facility = [id_fac_home] + [activity['facility_id']
                                                    for activity in planned_activities] + [id_fac_home]
                    # Create list min duration for the list of facilities
                    durations_min_list = [home_morning_duration_min] + \
                        activity_durations_min + [home_evening_duration_min]
                    # Create list max duration for the list of facilities
                    durations_max_list = [home_morning_duration_max] + \
                        activity_durations_max + [home_evening_duration_max]
                    # Create list earliest time to begin the activities
                    # Let the people leave earliest to have early morning traffic
                    activity_begin_earliest_list = [0.0] + activity_begin_earliest + [home_evening_begin_earliest]
                    # Create list latest time to begin the activities
                    activity_begin_latest_list = [0.0] + activity_begin_latest + [home_evening_begin_latest]
                    # Okay then save the resutls to scenario.demand.virtualpop.activities
                    ids_act = virtualpop.get_activities().add_activities(
                        ids_activitytype=ids_activitytype,
                        ids_facility=ids_facility,
                        hours_begin_earliest=activity_begin_earliest_list,
                        hours_begin_latest=activity_begin_latest_list,
                        durations_min=durations_min_list,
                        durations_max=durations_max_list
                    )
                    # These results go to scenario.demand.virtualpop
                    virtualpop.traveltimebudgets[id_virtual] = daily_travel_time_budget
                    virtualpop.diaries[id_virtual] = ids_activitytype
                    virtualpop.activitypatterns[id_virtual] = ids_act

            # Writing results
            row = [
                id_virtual,
                '-'.join(activity_chain),
                virtual_pref_mode_name,
                daily_travel_time_budget,
                primary_travel_time_budget,
                round(travel_time_primary, 2) if 'travel_time_primary' in locals() else '',
                secondary_travel_time_budget,
                round(travel_time_secondary, 2) if len(
                    planned_activities) > 1 and 'travel_time_secondary' in locals() else ''
            ]

            output_activity_data.append(row)

        output_filename = 'daily_activity_profiles.csv'
        with open(output_filename, mode='w', newline='', encoding='utf-8') as file:
            writer = csv.writer(file)

            writer.writerow([
                'id_virtual',
                'activity_chain',
                'virtual_pref_mode',
                'daily_travel_time_budget',
                'primary_travel_time_budget',
                'travel_time_primary',
                'secondary_travel_time_budget',
                'travel_time_secondary'
            ])

            writer.writerows(output_activity_data)

            # else: # Select all possible destination without time constrain
            #     ids_possible_destinations = {
            #                                 landtype: [id_fac for id_fac in ids_fac if id_fac != id_fac_home] # Just avoid to choose home as destination
            #                                 for landtype, ids_fac in ids_possible_destinations.items()
            #                                 }
            #     id_fac = self.location_choice(
            #                                 ids_possible_destinations[landuse.landusetypes.typekeys[id_selected_landtype]],
            #                                 dyn_capacities,
            #                                 id_fac_home)
            #     travel_time = 0

        # Printing the activity results
        print('----------------------------------------------------------------------------------------------------------------------------------')
        print('Successfully Generated Virtual Population Daily Activities:')

        virtual_diaries = virtualpop.diaries.get_value()
        act_ids_to_names = {
            act_id: demand.activitytypes.names[act_id]
            for act_id in demand.activitytypes.get_ids()
        }

        # Convert diary to string for example: home-work-home
        daily_diaries = [
            '-'.join(act_ids_to_names.get(act_id) for act_id in diary)
            for diary in virtual_diaries
        ]

        daily_diary_types = Counter(daily_diaries)
        total_diaries = len(daily_diaries)

        for daily_diaries_type, number_diary in daily_diary_types.items():
            percentage = (number_diary / total_diaries) * 100
            print(f"Daily Diary Type: {
                  daily_diaries_type} - Virtual Population: {number_diary}, Percentage: ({percentage:.1f}%)")

        print(f"Total: {total_diaries} Diaries")
        print('----------------------------------------------------------------------------------------------------------------------------------')

        return True

## FUNCTIONS TO CALCULATE ISOCHRONE, LOCATION CHOICE, DYNAMIC CAPACITY CALCULATION, ETC.############################################################

    def isochrone_calculation(self,
                              id_fac_origin,
                              n_best,
                              accesslevels,
                              travel_time_budget,
                              min_travel_time,
                              ids_edge_target,
                              edgetimes_by_mode,
                              fstar,
                              edge_to_fac,
                              fallback_edges,
                              fallback_edges_times,
                              id_fac_home):
        """
        Calculates isochrone: reachable edges and facilities from a facility within a travel time budget.
        """
        scenario = self.parent.get_scenario()
        net = scenario.net
        edges = net.edges
        facilities = scenario.landuse.facilities

        # Step 1: Get nearby edges from origin facility

        ids_edge_nearby, _ = edges.get_closest_edge(
            p=facilities.centroids[id_fac_origin],
            n_best=n_best,
            accesslevels=accesslevels
        )

        # Step 2: Find reachable edges using Dijkstra from those nearby edges

        for id_edge in ids_edge_nearby:
            reachable_edges_time, reachable_ids_edges = routing.edgedijkstra_forward(
                id_edge_start=id_edge,
                cost_limit=travel_time_budget,
                ids_edge_target=ids_edge_target,
                weights=edgetimes_by_mode,
                fstar=fstar
            )

            # Filter edges to avoid selecting the destination too close, less then minimum travel time
            reachable_edges_time = {edges: times for edges,
                                    times in reachable_edges_time.items() if times >= min_travel_time}
            reachable_ids_edges = {edges: pre_edges for edges,
                                   pre_edges in reachable_ids_edges.items() if edges in reachable_edges_time}

            if reachable_ids_edges:
                reachable_ids_edges = set(reachable_ids_edges.keys()) | set(reachable_ids_edges.values())
                break

        if not reachable_ids_edges:
            if fallback_edges is not None and fallback_edges_times is not None:
                reachable_ids_edges = fallback_edges
                reachable_edges_time = fallback_edges_times
            else:
                raise Exception(f"No reachable edges found from facility {id_fac_origin} and no fallback available.")

        # Step 3: Get reachable facilities (excluding home & origin)
        reachable_ids_facs = {
            fac_id
            for edge_id in reachable_ids_edges
            for fac_id in edge_to_fac.get(edge_id, [])
            if fac_id != id_fac_home and fac_id != id_fac_origin
        }  # reachable_ids_facs = {id_fac1, id_fac2, id_fac3, .......}, avoid to choose home as destination

        # Step 4: Get travel time to each reachable facility
        reachable_facs_time = {}    # reachable_facs_time = {id_fac1: time1, id_fac2: time2, id_fac3; time3:, .......]
        for id_edge, ids_fac in edge_to_fac.items():
            if id_edge in reachable_ids_edges:
                travel_time = reachable_edges_time.get(id_edge, travel_time_budget)
                for fac_id in ids_fac:
                    if fac_id == id_fac_home:  # Do not take id_home travel time
                        continue
                    if fac_id not in reachable_facs_time or travel_time < reachable_facs_time[fac_id]:
                        # Get the min travel time if a facility closing to more than one edge
                        reachable_facs_time[fac_id] = travel_time

        return reachable_ids_edges, reachable_edges_time, reachable_ids_facs, reachable_facs_time

    def get_activity_location(self,
                              id_fac_origin,
                              id_selected_landtype,
                              reachable_ids_facs,
                              reachable_facs_time,
                              travel_time_budget,
                              fallback_facs,
                              fallback_facs_times,
                              ids_possible_destinations,
                              dyn_capacities):
        """
        Select final facility destination for an activity, using reachable facilities, land-use type and dynamic activies.
        """
        # Step 1: Classify reachable facilities by land-use type

        classified_reachable_ids_facs = self.isochrone_facilities_classifier(
            reachable_ids_facs,
            ids_possible_destinations
        )
        # Step 2: Try to use the desired land-use type

        if id_selected_landtype in classified_reachable_ids_facs and classified_reachable_ids_facs[id_selected_landtype]:
            selected_reachable_ids_facs = classified_reachable_ids_facs[id_selected_landtype]

        # Step 3: Otherwise, try to use any other available land-use type
        else:
            for landtype, ids_facs in classified_reachable_ids_facs.items():
                if ids_facs:
                    id_selected_landtype = landtype
                    selected_reachable_ids_facs = ids_facs
                    break

            # Step 4: Use fallback if nothing is reachable
            else:
                if fallback_facs:
                    selected_reachable_ids_facs = fallback_facs
                    reachable_facs_time = fallback_facs_times
                else:
                    raise Exception(
                        "No reachable facilities found for any land type. Try to avoid having many isolated land uses.")

        # Step 5: Choose one facility from the selected candidates
        id_fac_destination = self.location_choice(
            selected_reachable_ids_facs,
            dyn_capacities,
            travel_time_budget,
            reachable_facs_time,
            id_fac_origin
        )
        travel_time = reachable_facs_time[id_fac_destination]

        return id_fac_destination, travel_time, selected_reachable_ids_facs, reachable_facs_time

    def identify_possible_destinations(self, is_include_amenities, landtype):

        landuse = self.parent.get_scenario().landuse.facilities.parent
        facilities = landuse.facilities
        pois = landuse.pois

        ids_poi = pois.get_ids()
        pois.guess_facilities()

        # Initial facilities: buildings with the target landuse type
        ids_fac = facilities.get_ids_building()  # Only the buildings (with "_area") are considered
        ids_possible_fac = ids_fac[(facilities.ids_landusetype[ids_fac] == landtype)]

        # Use pois to enrich facilities landuse, add POIs with selected landtype then added more facilities to the list
        if self.is_use_pois:
            ids_poi = pois.get_ids()
            ids_possible_pois = ids_poi[(pois.ids_landusetype[ids_poi] == landtype)]
            ids_possible_fac = np.concatenate((ids_possible_fac, pois.nearest_buildings[ids_possible_pois]))
            ids_possible_fac = np.unique(ids_possible_fac)

    # -------------------------Use amenities

        if is_include_amenities:
            am_landtype = landuse.landusetypes.typekeys.get_id_from_index(
                landuse.landusetypes.typekeys[landtype]+'_area')
            ids_possible_am = ids_fac[(facilities.ids_landusetype[ids_fac] == am_landtype)]
            # Eliminate internal facilities
            if not self.is_facilities_inside_amenities:
                for id_possible_am in ids_possible_am:
                    ids_delete_fac = []
                    for id_possible_fac in ids_possible_fac:
                        if is_polyline_in_polygon(facilities.shapes[id_possible_fac], facilities.shapes[id_possible_am]):
                            ids_delete_fac.append(id_possible_fac)

                    for id_delete_fac in ids_delete_fac:
                        ids_possible_fac = np.delete(ids_possible_fac, np.where(ids_possible_fac == id_delete_fac))

            ids_possible_fac = np.concatenate((ids_possible_fac, ids_possible_am))

    # -------------------------Filter too small and too big facilities
        ids_possible_fac = ids_possible_fac[(facilities.areas[ids_possible_fac] > self.min_area_fac) & (
            facilities.areas[ids_possible_fac] < self.max_area_fac)]
        # print('landtype',landtype, 'ids_possible_fac',ids_possible_fac, 'landtypesnew', facilities.ids_landusetype[ids_possible_fac])

    # -------------------------Spatial filter

        # Use entire study area
        if self.study_area == 1:
            # print('select valid facilities in entire study area')
            ids_possible_fac = ids_possible_fac
            # Use Zone
        elif self.study_area == 2:
            # print('select valid facilities inside zone %s'%(self.select_zone))
            ids_possible_fac = ids_possible_fac[(facilities.ids_zone[ids_possible_fac]
                                                 == landuse.zones.ids_sumo.get_id_from_index(self.select_zone))]

        return ids_possible_fac

    def isochrone_facilities_classifier(self, isochrone_fac_ids, ids_possible_destinations, landuse_types=None):
        """ 
        Classify facilities inside isochrone area by landuse type from global classification.
        once we had (i) classification of facilities in entire city, and (ii) list of facilities within isochrone area
        now is to classify facilites within isochrone by finding intersection between 2 sets            
        Returns dict: {landuse_type: [facility_ids inside isochrone]}.
        """
        # Initialize an empty dictionary to store classified facilities
        classified_facilities = defaultdict(list)

        # Use all categories if no specific landuse types provided
        if landuse_types is None:
            landuse_types = ids_possible_destinations.keys()

        isochrone_fac_ids = set(isochrone_fac_ids)

        # Iterate over each landuse type to classify facilities
        for landuse_type in landuse_types:
            # Get the facilities belonging to the current landuse type, defaulting to empty list if none
            fac_ids_set = set(ids_possible_destinations.get(landuse_type, []))

            # Find the intersection between 2 sets - isochrone and global facilities
            matched_fac_ids = isochrone_fac_ids.intersection(fac_ids_set)
            if matched_fac_ids:
                classified_facilities[landuse_type] = list(matched_fac_ids)

        return dict(classified_facilities)

    def location_choice(self, ids_possible_fac, dyn_capacities, travel_time_budget=None, travel_time_fac_reachable=None, id_fac_origin=None):
        """Choose the facility for intermediate activities based on distribution method."""

        landuse = self.parent.get_scenario().landuse.facilities.parent
        facilities = landuse.facilities

        def straight_line_distances():
            coord_origin = facilities.centroids[id_fac_origin][:-1]
            coords_fac = facilities.centroids[ids_possible_fac][:, :-1]
            diff_coords = coords_fac - coord_origin
            return np.linalg.norm(diff_coords, axis=1)

    # -------------------------Preferred  travel distance
        if self.distribution_method == 1:
            # Select the most probable facility based on as-the-crow-flies preferred travel distance, destinations close to the preferred travel distance to be selected

            dists = straight_line_distances()
            preferred_distance = np.random.normal(self.av_travel_distance, self.std_travel_distance)
            diff_from_pref = np.abs(dists - preferred_distance)
            id_fac = ids_possible_fac[np.argmin(diff_from_pref)]

    # -------------------------Preferred dynamic capacity of destinations
        if self.distribution_method == 2:
            # Select the most probable facility based on capacity of the destination, destinations with higher avaiable capacity (dynamic capacity) to be selected

            attractions = np.array(dyn_capacities[ids_possible_fac], dtype=float)
            total_attr = np.sum(attractions)

            if total_attr > 0:
                probabilities = attractions / total_attr
                id_fac = np.random.choice(ids_possible_fac, p=probabilities)
            else:
                id_fac = np.random.choice(ids_possible_fac)

    # -------------------------Preferred dynamic capacity of destinations and travel time budget constrained
        if self.distribution_method == 3:
            # Select the most probable facility based on dynamic capacity of the destination, and TTB, destinations with higher avaiable dynamic capacity and close to daily TTB to be selected

            capacities = np.array(dyn_capacities[ids_possible_fac], dtype=float)
            travel_time = np.array([travel_time_fac_reachable[id_fac] for id_fac in ids_possible_fac], dtype=float)
            travel_time_budget_gap = np.maximum(travel_time_budget - travel_time, 0.0)
            attractions = capacities * np.exp(-travel_time_budget_gap)
            total_attr = np.sum(attractions)

            if total_attr > 0:
                probabilities = attractions / total_attr
                id_fac = np.random.choice(ids_possible_fac, p=probabilities)
            else:
                id_fac = np.random.choice(ids_possible_fac)

    # -------------------------Distant-based and Time-based Gravitational
        elif self.distribution_method in [4, 5]:
            # Select the most probable facility based on a Gravitational model of the dynamic capacities and as-the-crow-flies distance/travel time to get there; higher capacity with shorter distance results in higher selection probability
            if self.distribution_method == 4:
                # Use as-the-crow-flies distance
                dists = straight_line_distances()
            else:
                # Use travel time
                dists = np.array([travel_time_fac_reachable[id_fac] for id_fac in ids_possible_fac])

            # Avoid zero or negative distances for power calculation
            dists = np.maximum(dists, 1e-6)

            attractions = (dyn_capacities[ids_possible_fac] ** self.gravitational_capacity_parameter) * \
                (dists ** self.gravitational_dist_parameter)
            probabilities = attractions / np.sum(attractions)
            id_fac = np.random.choice(ids_possible_fac, p=probabilities)

    # -------------------------Distant-based and Time-based Logit
        elif self.distribution_method in [6, 7]:
            # Select the most probable facility based on a logit model of the dynamic capacities and as-the-crow-flies distance and travel time to get there; higher capacity with shorter distance results in higher selection probability

            if self.distribution_method == 6:
                # Use as-the-crow-flies distance
                dists = straight_line_distances()
            else:
                # Use travel time
                dists = np.array([travel_time_fac_reachable[id_fac] for id_fac in ids_possible_fac])

            utilities = dyn_capacities[ids_possible_fac] * self.logit_area_parameter - dists * self.logit_dist_parameter
            utilities -= np.max(utilities)  # to avoid overflow
            exp_utilities = np.exp(utilities)
            probabilities = exp_utilities / np.sum(exp_utilities)
            id_fac = np.random.choice(ids_possible_fac, p=probabilities)

        return id_fac

    def get_maximum_capacities(self, ids_fac_landtype, index_landusetype):
        ''' This is to get maximum capacity.service-hour of the buildings (number of people*hour can handle) in a day based on openning and closing times'''

        landuse = self.parent.get_scenario().landuse.facilities.parent
        facilities = landuse.facilities

        # Get average values by land use type
        av_opening_hour = float(self.av_opening_hours[index_landusetype])
        av_closing_hour = float(self.av_closing_hours[index_landusetype])
        av_floors = self.av_floors[index_landusetype]

        # Identify 24-h facilities
        share_24h = self.shares_24h[index_landusetype]
        num_24h = int(share_24h * len(ids_fac_landtype))
        facilities_24h = set(np.random.choice(ids_fac_landtype, num_24h, replace=False))

        # Calculating maximum capacity.hour perday
        max_hour_capacities = {}
        for id_fac in ids_fac_landtype:
            is_24h_fac = id_fac in facilities_24h
            opening_hours = 24 if is_24h_fac else (av_closing_hour - av_opening_hour)
            base_capacity = facilities.capacities[id_fac] * av_floors
            max_hour_capacities[id_fac] = base_capacity * opening_hours

        return max_hour_capacities  # people * hour
