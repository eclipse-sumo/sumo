"""
taxi and SAV service module


GENERAL CODE INFO: 

            The function "prepare_sim()" is the first function to be called in this script.
            In "prepare_sim()", the system checks if the conditions are met to use the SUMO TRACI features of the taxiservice,
            e.g. breakdown and slowdown simulation, info on the reservation/occupation status of the taxi/SAV fleets
            and extra info regarding the removal/disappearance of taxis/SAVs by SUMO during a simulation.
            If the conditions are met, the "process_step()" function is called after every time_update interval.
            The function "process step()" is the equivalent to a "main loop" of the taxi and SAV service.
            The code in the "process_step" function will be executed after each traci timestep, if:
            time_update > 0 AND the microscopic simulation is run by selecting the SUMO TRACi mode in hybridPY.
            There are functions in this script that are not called by "prepare_sim()" or "process_step()" function.
            These functions, e.g."write_config()", are called by other functions found in other files of hybridPY (see below).

                 
LOCATIONS OF SAV AND TAXI CODE WITHIN hybridPY: 
            
            The following hybridPY files were modified for the introduction of SAVs to hybridPY:
            (before May 2023, only normal, private taxis existed in hybridPY)
            (some keywords to help find relevant code sections easily: taxi, taxi:fleet, SAV, SAV_, etc.)
            - lib_base/classman.py
            - coremodules/demand/demand.py
            - coremodules/demand/demandbase.py
            - coremodules/demand/vehicles.py
            - coremodules/demand/virtualpop.py
            - coremodules/demand/wxgui.py
            - coremodules/simulation/taxi.py

            The following hybridPY files in [parentheses] contain code which either references or mentions "taxi"
            or is necessary for the SUMO mode "taxis" to function in general in hybridPY.
            The following files do not contain code specific to SAVs.
            (remember: SAVs and private taxis both belong to the SUMO mode "taxi")
            (exact location of taxi code easy to find by searching for the word taxi)
            - [coremodules/demand/origin_to_destination.py]
            - [coremodules/landuse/landuse.py]
            - [coremodules/misc/shapeformat.py]
            - [coremodules/network/netconvert.py]
            - [coremodules/network/network.py]
            - [coremodules/network/networkxtools.py]
            - [coremodules/simulation/antijammer.py]
            - [coremodules/simulation/results.py]
            - [coremodules/simulation/simulation.py]
            - [coremodules/simulation/sumo.py]
            - [coremodules/simulation/wxgui.py]

            A new SUMO Gui configuration was added for experimenting
            with taxi fleets for the introduction of SAVs,
            with all vehicle ids and fleets shown per default:
            - coremodules/network/netedit_config_SAV_high_vis.xml

            Additionally, there exists the following TRIP DATA ANALYSIS script:
            - examples/scripts/tripdata_analysis_without_hybridPY_py3.py
            
            This file can analyze the trips file that SUMO creates.
            It generates useful graphs, console output and a .csv file with data
            regarding modal split, door-door travel times, etc.
            The ANALYSIS SCRIPT WORKS WITHOUT hybridPY and runs with Python 3 (like SUMO).
            Python 2 or the hybridPY Python packages are not required.
            This is useful if you have trouble installing hybridPY on a machine but
            still want to run and analyze SUMO-simulation-files generated with hybridPY.
            In order for thae script to work,
            you MUST select the options "Output Trip Data"
            AND "Include Unfinished Trip Data" BEFORE running a simulation.
            Only then will the tripdata output file have the write format.

            The creation of this tripdata_analysis script was also necessary, since
            the preexisting hybridPY file coremodules/simulation/results.py analyses the results based
            on the SUMO "mode" of transport. Since both private taxis and SAVs have the same SUMO mode "taxi",
            the preexisting hybridPY results.py file cannot differentiate between the two.
            Reworking it to check for taxi fleets should be possible, yet developing a fresh hybridPY-free
            analysis script that runs on Python 3 (just like SUMO) proved beneficial to SUMO users
            who don't need hybridPY or have trouble installing Python 2 or the necessary packages for hybridPY.


SAV IMPLEMENTAION AND TAXI FLEETS:

            Shared Autnomous Vehicles (SAVs) and private taxis both belong
            to the same SUMO mode "taxi", but are assigned to 2 different taxi fleets:
            taxi:fleetPrivate and taxi:fleetSAV.
            These are the two official taxi fleets that the alogrithms in coremodules/simulation/taxi.py can handle.
            
            In general, however, any vehicle equipped with a so-called "taxi device" will function as taxi in SUMO.
            A taxi does not need to be assigned to a specific taxi fleet to transport passengers. BUT:
            Such fleetless taxis are classified as "rogue taxis" within this taxi and SAV service module.
            These rogue taxis will pick up passengers waiting for any of the taxi fleets,
            i.e., rogue taxis will transport passengers no matter if their travel plan is for private taxi or SAV.
            This can severely lessen the validity of a simulation, since the specific travel plan for that person becomes irrelevant.
            Therefore, rogue taxis should be avoided (any random vehicle with a taxi device).

            INSTEAD:
            All taxis (private taxis and SAV) should be generated in the hybridPY gui
            via <demand/trips and routes/generate taxi trips> or <demand/.../generate SAV trips>.
            
            The simulate_SAV_breakdowns() and simulate_SAV_slowdowns() code in this file
            only affects SAVs, i.e. it only affects "taxis" assigned to "taxi:fleetSAV".
            Sumo uses the same dispatch algorithm for all taxi fleets,
            i.e., both for the private taxi fleet and and the SAV fleet.
            In order to prevent vehicles in the private taxi fleet from engaging in ride-sharing,
            private taxis in hybridPY have a default capacity of 1.
            Thus, despite using the same ride-sharing algorithm as SAVs, private taxis don't engage in ride-sharing.
            If you write your own dispatch algorithm using Traci,
            it should be possible to implement different dispatch behavior for the various taxi fleets.

            Private taxis and SAVs are also assigned to different vtypes,
            but that is only relevant some of the other hybridPY files listed above,
            not for the code in this file.

            General SUMO taxi information: https://sumo.dlr.de/docs/Simulation/Taxi.html


(TODO A) CREATING A NEW hybridPY SCENARIO WITH PRIVATE TAXIS OR SAVS:

            TODO A: @ProfSchweizer: How do you include the text from both (TODO A) paragraphs into the hybridPY gui so normal users can read it?
                                    Adding text to the INFO section of <class TaxiService> has no effect (the text does not even show up
                                    when you hover over the <taxiservice> text in the menu <scenario.simulation>)

            TODO B: @ProfSchweizer: Please fill in steps 1-X for building a scenario from scratch

            1-X. Prepare the simulation (load/create map, create zones and acitivies ????)
            2. Generate the virtual population: Demand -> virt. pop. -> configure pop. -> generate
            3. Create plans (including SAV or private taxi plans): Demand -> virt. pop. -> plans -> generate
            4. Select plans: Demand -> virt. pop. -> plans -> select current plans
            5. Add SAVs: Demand -> trips and routes --> generate SAV trips
            6. Run the simulation via Simulation --> Microscopic Simulation ---> SUMO TRACi
               (or --> SUMO if you don't need certain features, e.g. breakdown simulation)
            
            Note regarding 4.:
            If you have both plans for private taxis and SAVs:
            Since SUMO sees both types of vehicles as a "taxi", the same estimated times
            will be calculated for both private taxi plans and SAV plans.
            Since the estimated times will be equal for both plans,
            hybridPY will select either all private taxi plans or all SAV plans
            (depending on which plans were generated first and have the lower id number).
            
            Therefore, if you want to properly evaluate a simulation with private taxis and SAVs,
            you must select plans using the shortest execution times.
            When you select the options for running a simulation, (in the SUMO or SUMO-Traci Dialog window)
            you MUST set the following options to true:
            - Ouput Trip Data
            - Include Unfinished Trip Data

            After every simulation, reselect the plans by shortest execution time
            Negative execution times appear if the plan was never finished.
            (the person never reached the destination)

            Repeat at least until all plans for all people have an executed time that is not zero.
            Ideally, keep iterating until the execution times don't change significantly anymore.
            Then, the optimum plans have been determined by executed times.
            You can introduce some randomness in the plan selector to motivate the virt. pop. to try
            plans with the non-shortest execution times, too. This may goodto escape
            the local travel-time minimum of a certain plan and find the global travel time minimum of all plans.
            (e.g., the executed times from the first run of the simulation could be skewed if most people
            choose to take their car (plan auto) and massive traffic jams ensue,
            Without randomness, barely anyone would ever retry the car plan due to the long or negative (unfinished) plan times.
            However, if nobody takes the car, the roads are free again, making the car a viable option again (travel-time-wise).
            With some randomness, some people will retry taking the car and possibly find a new shortest executed plan time
            that would never have been found without randomness)

            There is a demo-script to reiterate the simulation until a travel-time-optimum is reached.
            It is located in (TODO C)
            TODO C: @ProfSchweizer: in this github repo, there is such an iteration-script-file
                    (perhaps not configured fully correctly) called <runSimulation.py>.
                    Could it or a similar file be included in hybridPY, e.g. in the examples folder?


(TODO A)LOADING OLD hybridPY SCENARIOS (created with hybridPY 2.5 or earlier):

            If you are loading an hybridPY-Project created with an old hybridPY version without SAVs,
            perform the following steps in this order to enable SAVs:
            1. Select: Demand -> vehicles -> load defaults
            2. Regenerate the virtual population: Demand -> virt. pop. -> configure pop. -> generate
            3. Create plans (including SAV plans): Demand -> virt. pop. -> plans -> generate
            4. Select plans: Demand -> virt. pop. -> plans -> select current plans
            5. Add SAVs: Demand -> trips and routes --> generate SAV trips

            Steps 1 through 4 must be executed whenever you load an old hybridPY project,
            even if you do not want to add SAVs.
            Otherwise, there will be errors when you try to run it.                                  


TRACI DISPATCH ALGO:

            Among the taxi dispatch algorithm option, the dispatch algorithm "Traci" can be selected.
            Currently, however, there is no Traci disptach algorithm in hybridPY.
            Only the built-in SUMO algorithms are used.
            Therefore, if you select the TRACi algorithm, the taxis will remain idle throughout the simulation.

            If you want to develeop your own TRACi dispatch algorithm for hybridPY, here are some potentially important TRACi methods
            getRoadID(self, vehID), getRoute(self, vehID), changeTarget(self, vehID, edgeID), setStop(self, vehID, edgeID)
            See:
            https://sumo.dlr.de/daily/pydoc/traci.html
            https://sumo.dlr.de/daily/pydoc/traci._vehicle.html


"""

import os, sys
import numpy as np
import traci

from agilepy.lib_base.processes import Process

import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.xmlman as xm
from coremodules.demand.demandbase import DemandobjMixin
from coremodules.simulation.simulationbase import SimobjMixin


class TaxiService(SimobjMixin,DemandobjMixin,cm.BaseObjman):
        def __init__(   self, ident, simulation,
                        name = 'Taxi and SAV service', 
                        info ='Taxi and SAV (Shared Autonomous Vehicle) service configuration.\n',
                        version = 0.1,
                        **kwargs):

            self._init_objman(  ident = ident, parent = simulation, 
                                name = name, info = info, 
                                version = 0.1,
                                **kwargs)
                                
            attrsman = self.set_attrsman(cm.Attrsman(self))
            self._init_attributes()
            self._init_constants()
            
            # make TaxiService a demand object as link
            self.get_scenario().demand.add_demandobject(obj = self)
            
        
        def get_scenario(self):
            return self.parent.parent
        
        def _init_constants(self):
            self._typemap = {}
            self.get_attrsman().do_not_save_attrs(['_typemap', ])
            
            # make TaxiService also a simulation object as link
            # self.get_scenario().simulation.add_simobject(obj = self)
            
        def _init_attributes(self):
            attrsman = self.get_attrsman()
            scenario = self.get_scenario()

            self.simtime = 0
            self.starttime = 0
            self.simtime_firstenter_processstep = None

            self.simtime_firstenter_breakdown = None
            self.simtime_firstenter_breakdown_planned = None
            self.number_of_SAVs_at_simtime_firstenter_breakdown_planned_selected = False
            self.number_of_SAVs_at_simtime_firstenter_breakdown_planned = None
            self.simtime_lastenter_breakdown = None

            self.simtime_firstenter_slowdown = None
            self.simtime_firstenter_slowdown_planned = None
            self.number_of_SAVs_at_simtime_firstenter_slowdown_planned_selected = False
            self.number_of_SAVs_at_simtime_firstenter_slowdown_planned = None
            self.simtime_lastenter_slowdown = None
            self.simtime_lastexit_slowdown = None

            self.index_SAV_last_stopped = 0
            self.id_of_next_SAV_to_break_down = None

            self.traci_SAV_breakdown_simulation = False
            self.traci_SAV_slowdown_simulation = False

            self.SAV_breakdown_simulator_called = False
        
            self.SAV_breakdown_startendtimes_validated = False
            self.SAV_breakdown_starttime_is_valid = False
            self.SAV_breakdown_endtime_is_valid = False
            self.SAV_breakdown_endtime_reached = False

            self.SAV_breakdown_endtime_executed = None
            self.SAV_breakdown_starttime_case = None
            self.SAV_breakdown_endtime_case = None

            self.SAV_slowdown_startendtimes_validated = False
            self.SAV_slowdown_starttime_is_valid = False
            self.SAV_slowdown_endtime_is_valid = False
            self.SAV_slowdown_endtime_reached = False
            
            self.SAV_slowdown_endtime_executed = None
            self.SAV_slowdown_starttime_case = None
            self.SAV_slowdown_endtime_case = None

            self.initial_breakdown_occured = False
            self.initial_SAV_to_break_down_selected = False
            self.breakdown_simulator_start_time_reached = False
            self.initial_SAV_breakdown_loop_passed = False

            self.initial_slowdown_flag = False
            self.fleet_slowed_down_flag = False

            self.taxis_all = []
            self.taxis_SAV_all = []
            self.taxis_SAV_all_old = []
            self.taxis_SAV_all_new = []
            self.taxis_SAV_removed = []
            self.taxis_SAV_removed_idle = []
            self.taxis_SAV_sharing = []
            self.taxis_private_all = []
            self.taxis_private_all_old = []
            self.taxis_private_all_new = []
            self.taxis_private_removed = []
            self.taxis_private_removed_idle = []
            self.taxis_private_sharing = []
            self.taxis_rogue_all = []
            self.taxis_rogue_all_old = []
            self.taxis_rogue_all_new = []
            self.taxis_rogue_removed = []
            self.taxis_rogue_removed_idle = []
            self.taxis_rogue_sharing = []

            self.taxis_idle = []
            self.taxis_SAV_idle = []
            self.taxis_private_idle = []

            self.taxis_picking_up = []
            self.taxis_SAV_picking_up = []
            self.taxis_private_picking_up = []

            self.taxis_occupied = []
            self.taxis_SAV_occupied = []
            self.taxis_private_occupied = []

            self.taxis_pick_and_occ = []
            self.taxis_SAV_pick_and_occ = []
            self.taxis_private_pick_and_occ = []

            self.taxis_SAV_not_broken = []
            self.taxis_SAV_no_longer_broken = []
            self.SAVs_removed_from_sim = []

            # dictionary of SAVs with breakdown-timestamps:
            self.taxis_SAV_broken = {}

            # dictionaries of all last-known times/locations of taxis
            # useful for debugging the network, e.g.:
            # if taxi gets removed from the sim by SUMO between timesteps,
            # and SUMO outputs:
            # <Warning: Vehicle vehicle_id ends idling in a cul-de-sac>
            # then you can use the last known location to figure out
            # where and when (and hopefully why) the taxi/SAV had isses
            self.taxis_all_locations = {}
            self.taxis_all_final_timestep = {}
            self.taxis_all_final_edge_id = {}     
            self.taxis_all_final_lane_id = {}
            self.taxis_all_final_lane_index = {}
            self.taxis_all_final_lane_position = {}

            self.dispatchalgorithm = attrsman.add(cm.AttrConf(  'dispatchalgorithm', 'routeExtension',
                                    groupnames = ['options'], 
                                    name = 'Dispatch Algorithm',
                                    choices = { 'greedy:  Assign taxis/SAVs to customers in the order of reservations (no ride-sharing)':'greedy',
                                                'greedyClosest:  Assign each taxi/SAV to the closest customer (no ride-sharing)':'greedyClosest',
                                                'greedyShared:  Assign taxis/SAVs to customers in the order of reservations and pick up others on the way':'greedyShared',
                                                'routeExtension:  Assign taxis/SAVs to customers in the order of reservations and make detours to pick up others':'routeExtension',
                                                'traci:  Algorithms managed by Traci':'traci'},
                                    info =  "Algorithm used to dispatch taxis and SAVs (shared autonomous vehicles)\n\n\
                                             In order for SAVs to engage in ridesharing\n\
                                             (transporting multiple customers to various destinations in the vehicle at once),\n\
                                             you must choose either <greedyShared>, <routeExtension> or your own traci-algorithm as a dispatch algorithm.\n\
                                             <routeExtension> appears to be the most effective pre-existing ride-sharing dispatch algorithm in SUMO.\n\n\
                                             Sumo uses the same dispatch algorithm for all taxi fleets,\n\
                                             i.e., both for the private taxi fleet and and the SAV fleet.\n\n\
                                             In order to prevent vehicles in the private taxi fleet from engaging in ride-sharing,\n\
                                             private taxis have a default capacity of 1.\n\
                                             Thus, despite using the same ride-sharing algorithm as SAVs, private taxis don't engage in ride-sharing.\n\n\
                                             If you write your own dispatch algorithm using Traci,\n\
                                             it should be possible to implement different dispatch behavior for the various taxi fleets.\n\n\
                                             General information: https://sumo.dlr.de/docs/Simulation/Taxi.html",
                                    tag = 'device.taxi.dispatch-algorithm',
                                    ))

            self.period_dispatch = attrsman.add(cm.AttrConf(  'period_dispatch', 1.0,
                                    groupnames = ['options'], 
                                    name = 'Dispatch period',
                                    info = "The period between successive calls to the dispatcher.",
                                    unit = 's',
                                    tag = 'device.taxi.dispatch-period',
                                    ))

            self.duration_pickup = attrsman.add(cm.AttrConf(  'duration_pickup', 15.0,
                                    groupnames = ['options'], 
                                    name = 'Pickup duration',
                                    info = "The duration needed to pick up a taxi or SAV customer.",
                                    unit = 's',
                                    tag = 'device.taxi.pickUpDuration',
                                    ))
                                    
            self.duration_dropoff = attrsman.add(cm.AttrConf(  'duration_dropoff', 15.0,
                                    groupnames = ['options'], 
                                    name = 'Dropoff duration',
                                    info = "The duration needed to drop off a taxi or SAV customer.",
                                    unit = 's',
                                    tag = 'device.taxi.dropOffDuration',
                                    ))
            
            self.loss_abs = attrsman.add(cm.AttrConf(  'loss_abs', -1.0,
                                    groupnames = ['options'], 
                                    name = 'Absolute loss',
                                    info = "Absolute acceptable loss for detours. Applies only to Dispatch Algorithm greedyShared.",
                                    unit = 's',
                                    ))
                                    
            self.loss_rel = attrsman.add(cm.AttrConf(  'loss_rel', -1.0,
                                    groupnames = ['options'], 
                                    name = 'Relative loss',
                                    info = "Relative  acceptable loss for detours. Applies only to Dispatch Algorithm greedyShared.",
                                    ))
            
                                    
            self.idlealgorithm = attrsman.add(cm.AttrConf(  'idlealgorithm', 'randomCircling',
                                    groupnames = ['options'], 
                                    name = 'Idle algorithm',
                                    choices = ['stop','randomCircling'],
                                    info = "Used idle algorithm that determines the behavior when taxi/SAV has no passengers.\n\n\
                                            If the idle algorithm is <stop> and <parking> is enabled,\n\
                                            taxis/SAVs never impact traffic while idling.\n\n\
                                            RECOMMENDED combinations:\n\
                                            a) set idle algorithm to <RANDOMCIRCLING> and DISABLE parking\n\
                                            b) set idle algorithm to <STOP> and ENABLE parking\n\
                                            Reason:\n\
                                            If idle algorithm is <stop> and <parking> is disabled,\n\
                                            the following unusual traffic jams occur:\n\
                                            Idle taxis/SAVs remain stopped on the road and block traffic while idle.\n\n\
                                            WARNING:\n\n\
                                            If the idle algorithm is <randomCircling>, taxis/SAVs may disappear from the simulation over time.\n\
                                            When this happens, the SUMO console states the vehicle ended idling in a cul-de-sac.\n\
                                            This seems to happen when an idling taxi/SAV tries to enter/exit a unreachable edge/lane.\n\
                                            Cleaning up the network reduces this problem or eliminates it (if network is perfectly connected)\n\
                                            Useful network improvements: connect edges/lanes, provide u-turns at the borders of the network\n\n\
                                            If the demand for taxis/SAVs demand is so high that the vehicles never enter an idling state,\n\
                                            the taxis/SAVs do not disappear from the simulation,\n\
                                            even if the idling algorithm is <randomCircling>v.\n\n\
                                            (tested in SUMO 1.15 and 1.16)",
                                    tag = 'device.taxi.idle-algorithm',
                                    ))

            self.parking = attrsman.add(cm.AttrConf(  'parking', False,
                                    groupnames = ['options'], 
                                    name = 'Parking',
                                    info = "If ENABLED, parking lots for taxis/SAVs are generated wherever a pick-up and drop-off is requested.\n\
                                            Therefore, during the pick-up/drop-off periods, taxis/SAVs never block other traffic.\n\n\
                                            RECOMMENDED combinations:\n\
                                            a) set idle algorithm to <RANDOMCIRCLING> and DISABLE parking\n\
                                            b) set idle algorithm to <STOP> and ENABLE parking\n\
                                            Reason:\n\
                                            If idle algorithm is <stop> and <parking> is disabled,\n\
                                            the following unusual traffic jams occur:\n\
                                            Idle taxis/SAVs remain stopped on the road and block traffic while idle.\n\n\
                                            WARNING:\n\n\
                                            a) If there are NO bicycles in the simulation,\n\
                                               <parking> can be enabled or disabled without causing SUMO to crash\n\n\
                                            b) If you have BICYCLES AND do NOT use the SUBLANE model:\n\
                                               <parking> can be enabled or disabled without causing SUMO to crash.\n\n\
                                            c) If you have BICYLCES AND use the SUBLANE model AND <parking> is ENABLED:\n\
                                               SUMO will NOT crash.\n\n\
                                            d) If you have BICYLCES AND use the SUBLANE model AND <parking> is DISABLED:\n\
                                               SUMO WILL probably CRASH, IF the sublane width is smaller than the width of a lane.\n\
                                               TO PREVENT Sumo from CRASHING BY setting the sublane width larger than any lane, e.g. 10 meters\n\
                                               (in the simulation launch window).\n\
                                               Disadvantage: only 1 bicycle fits laterally per lane (like without the sublane model)\n\
                                               Adventage: you still have the continuous lateral motion during lane changes from the sublane model\n\
                                               (without the sublane model, lane changes are instant)\n\n\
                                            Note: The sudden software crashes were observed in SUMO 1.15 and 1.16.\n\
                                            They occur when a taxi/SAV is stopped in a lane and a bicycle starts passing that taxi/SAV.\n\
                                            (parking_anywhere disabled, small sublanes (e.g. 1 meter) enabled)\n\
                                            SUMO suddenly crashes with no error message or warning and the SUMO window closes immediately.\n\n\
                                            Note: Sublane model only active if lane-changing model is SL2015.\n\
                                            (selectable in hybridPY under: demand --> vehicles)",
                                    tag = 'device.taxi.parking',
                                    ))
                                    
            self.time_update = attrsman.add(cm.AttrConf(  'time_update', -1.0,
                                        groupnames = ['options'], 
                                        name = 'Update time',
                                        info = "ONLY for Sumo TRACi simulation\n\n\
                                                Taxi service update time.\n\n\
                                                Update time MUST be greater than 0.0 to enable the features:\n\
                                                a) Taxi/SAV reservation/occupation info\n\
                                                b) Taxi/SAV removal extended info\n\
                                                b) SAV random breakdowns\n\
                                                c) SAV fleet slowdowns\n\n\
                                                Additional activation requirement for b) and c):\n\
                                                the interval and period times must also be greater than 0.0",
                                        unit = 's',
                                        ))

            self.traci_taxi_fleet_reservation_info  = attrsman.add(cm.AttrConf(  'traci_taxi_fleet_reservation_info', False,
                                    groupnames = ['options'], 
                                    name = 'Taxi/SAV reservation/occupation info',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Extended info on console regarding taxi fleet reservation and occupation status.\n\n\
                                            To enable this feature, the following parameter must be > 0.0:\n\
                                            Update time",
                                    ))

            self.display_number_of_taxis_SAVs = attrsman.add(cm.AttrConf(  'display_number_of_taxis_SAVs', True,
                                    groupnames = ['options'], 
                                    name = 'Display number of taxis/SAVs',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Display (in the traci console) the number\n\
                                            of private taxis and SAVs that are currently in the simulation,\n\
                                            as well as the number of private taxis and SAVs\n\
                                            removed from the sim by SUMO, e.g. due to network errors.\n\n\
                                            To enable this feature, the following parameter must be > 0.0:\n\
                                            Update time",
                                    ))


            self.traci_taxi_removal_info  = attrsman.add(cm.AttrConf(  'traci_taxi_removal_info', False,
                                    groupnames = ['options'], 
                                    name = 'Taxi/SAV removal/disappearance extra info',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Extended info on console regarding taxi removals by SUMO, e.g. due to network errors.\n\
                                            Provides id, fleet, last known location (edge and lane)\n\
                                            and timestep of removed taxis/SAVs.\n\n\
                                            Great METHOD FOR DEBUGGING a NETWORK:\n\
                                            Let sveral hundred taxis or SAVs idle around a network\n\
                                            in a scenario with no demand (no plans for taxis/SAVs).\n\
                                            The idle algorithm MUST be <randomCircling> for this method!\n\n\
                                            When taxis/SAVs idle around the network using the <randomCircling> algorithm,\n\
                                            they may get stuck on certain edges if the network has errors, e.g. disconnected lanes.\n\
                                            After a while, SUMO removes these vehicles and outputs this message:\n\
                                            Vehicle vehicle_id ends idling in a cul-de-sac\n\
                                            (vehicle_id is a placeholder for an actual id, e.g. '103' )\n\
                                            Afterwards, SUMO automatically removes the vehicle.\n\
                                            It disappears for the rest of the simulation.\n\
                                            Other situations that lead to the automatic removal of vehicles by SUMO may exist,\n\
                                            yet the above situation (idling with randomCircling in an imperfectly connected network)\n\
                                            has been observed the most.\n\n\
                                            hybridPY has no control over SUMO to prevent this automatic vehicle removal.\n\
                                            hybridPY can only provide data on removed vehicles.\n\
                                            So far, the best remedy has been to solve the root problem:\n\
                                            fix the imperfect network.\n\n\
                                            To enable this feature, the following parameter must be > 0.0:\n\
                                            Update time",
                                    ))
                                    
            self.speed_in_kmh = attrsman.add(cm.AttrConf(  'speed_in_kmh', False,
                                    groupnames = ['options'], 
                                    name = 'Display break/slowdown max speeds in km/h',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Activate to display breakdown / slowdown max speeds in km/h.\n\
                                            If deactivated, speeds will be displayed in m/s.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV break/slowdown period, SAV break/slowdown interval",
                                    ))

            self.SAV_breakdown_starttime = attrsman.add(cm.AttrConf(  'SAV_breakdown_starttime', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV random BREAKDOWN start time',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Start time of the first random breakdown of an SAV.\n\n\
                                            If this value is left at at the default value of -1.0, the first breakdown will \n\
                                            take place 1 breakdown interval after first call of breakdown simulator.\n\
                                            (giving the SAVs time to leave the spawn location)\n\n\
                                            If this value is set to any other value smaller than the start time of the simlation,\n\
                                            the first breakdown will take place immediately at first call of breakdown simulator.\n\n\
                                            Exception: if this start time value is greater than the breakdown end time,\n\
                                            the start time will be treated as the default value of -1.0.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    unit = 's',
                                    ))

            self.SAV_breakdown_endtime = attrsman.add(cm.AttrConf(  'SAV_breakdown_endtime', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV random BREAKDOWN end time',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            End time of the last random breakdown of an SAV.\n\
                                            If this value is smaller than the start time of the simulation, it will be ignored.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    unit = 's',
                                    ))

            self.SAV_breakdown_interval = attrsman.add(cm.AttrConf(  'SAV_breakdown_interval', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV random BREAKDOWN interval',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Interval in simulation-seconds (not simulation-steps)\n\
                                            until the next random SAV starts simulating a break down (slow-down to near-standstill).\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    unit = 's',
                                    ))
            
            self.SAV_breakdown_period = attrsman.add(cm.AttrConf(  'SAV_breakdown_period', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV random BREAKDOWN period',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Breakdown-period of an SAV until it resumes its non-broken speed.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    unit = 's',
                                    ))

            self.SAV_breakdown_speed = attrsman.add(cm.AttrConf(  'SAV_breakdown_speed', 0.14, # speed in m/s
                                    groupnames = ['options'], 
                                    name = 'SAV random BREAKDOWN speed',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Speed at which a broken-down SAV should move.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval\n\n\
                                            If teleporting active:\n\
                                            Set breakdown speed to >= 0.14 m/s (0.5 km/h) to avoid teleporting while broken-down.\n\
                                            This way, SAVs can simulate a breakdown with a near-standstill\n\
                                            and cause the intended traffic jams without being teleported,\n\
                                            e.g., to simulate a major error which requires an intervention by a teleoperator.\n\
                                            All other traffic jams (e.g. due to SUMO-glitches/network-errors) are resolved with teleporting.\n\n\
                                            A speed of 0.14 m/s = 0.5 km/h was set as the robust default break-down speed\n\
                                            in order to allow for short additional speed reductions (e.g. pedestrian crossings / stop lights)\n\
                                            without immediately falling below an average speed of 0.11 m/s within the current time-to-teleport period.\n\
                                            Vehicles with average speeds under 0.11 m/s are teleported (tested with SUMO 1.16)",
                                    unit = 'm/s',
                                    ))
            
            self.SAV_breakdown_random_seed = attrsman.add(cm.AttrConf(  'SAV_breakdown_random_seed', 1,
                                    groupnames = ['options'], 
                                    name = 'SAV random BREAKDOWN RANDOM SEED',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Random seed for determining which SAV should break down next.\n\
                                            Conditions:\n\
                                            SAV_breakdown_random_seed must be an integer number AND\n\
                                            0 <= SAV_breakdown_random_seed <= 4294967295\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    ))

            self.SAV_breakdown_extended_info = attrsman.add(cm.AttrConf(  'SAV_breakdown_extended_info', True,
                                    groupnames = ['options'], 
                                    name = 'SAV breakdown extra info',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Activate to constantly display more detailed warnings\n\
                                            as well as a summary of key parameters and events,\n\
                                            e.g. timestamps of first/last planned/executed breakdowns.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    ))

            self.SAV_breakdown_overview_table = attrsman.add(cm.AttrConf(  'SAV_breakdown_overview_table', True,
                                    groupnames = ['options'], 
                                    name = 'SAV breakdown overview table',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Activate to display (in the traci console)\n\
                                            a table of all currently broken SAVs,\n\
                                            as well as the next SAV to break down.\n\
                                            The table contains the following information for each SAV:\n\
                                               breakdown-start time | breakdown-end time | SAV id\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV break/slowdown period, SAV break/slowdown interval",
                                    ))

            self.SAV_slowdown_starttime = attrsman.add(cm.AttrConf(  'SAV_slowdown_starttime', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV random SLOWDOWN start time',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Start time of the slowdown of the SAV fleet.\n\n\
                                            If this value is left at the default value of -1.0, the first slowdown will \n\
                                            take place  1 slowdown interval after first call of breakdown simulator.\n\
                                            (giving the SAVs time to leave the spawn location)\n\n\
                                            If this value is set to any other value smaller than the start time of the simlation,\n\
                                            the first slowdown will take place immediately at first call of breakdown simulator.\n\n\
                                            Exception: if this start time value is greater than the slowdown end time,\n\
                                            the start time will be treated as the default value of -1.0.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV slowdown period, SAV slowdown interval",
                                    unit = 's',
                                    ))

            self.SAV_slowdown_endtime = attrsman.add(cm.AttrConf(  'SAV_slowdown_endtime', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV random slowDOWN end time',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            End time of the last slowdown of the SAV fleet.\n\
                                            If this value is smaller than the start time of the simulation, it will be ignored.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV slowdown period, SAV slowdown interval",
                                    unit = 's',
                                    ))

            self.SAV_slowdown_interval = attrsman.add(cm.AttrConf(  'SAV_slowdown_interval', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV fleet SLOWDOWN interval',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Interval in simulation-seconds (not simulation-steps) until the entire SAV fleet slows down again.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV fleet slowdown period, SAV fleet slowdown interval",
                                    unit = 's',
                                    ))

            self.SAV_slowdown_period = attrsman.add(cm.AttrConf(  'SAV_slowdown_period', -1.0,
                                    groupnames = ['options'], 
                                    name = 'SAV fleet SLOWDOWN period',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Slowdown-period until SAV fleet resumes normal speed.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV fleet slowdown period, SAV fleet slowdown interval",
                                    unit = 's',
                                    ))
            
            self.SAV_slowdown_speed = attrsman.add(cm.AttrConf(  'SAV_slowdown_speed', 2.0, # speed in m/s
                                    groupnames = ['options'], 
                                    name = 'SAV fleet SLOWDOWN speed',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Speed you wish to slow down the SAV-fleet to.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV fleet slowdown period, SAV fleet slowdown interval\n\n\
                                            A speed of 2.0 m/s = 7.2 km/h was set as the default slow-down speed,\n\
                                            e.g., to simulate bad weather disrupting sensors.\n\n\
                                            If teleporting active:\n\
                                            Set slowdown speed to >= 0.14 m/s (0.5 km/h) to avoid teleporting while fleet is slowed down.\n\
                                            This way, short additional speed reductions (e.g. pedestrian crossings / stop lights) are possible\n\
                                            without immediately falling below an average speed of 0.11 m/s within the current time-to-teleport period.\n\
                                            Vehicles with average speeds under 0.11 m/s vehicles are teleported (tested with SUMO 1.16)",
                                    unit = 'm/s',
                                    ))

            self.SAV_slowdown_extended_info = attrsman.add(cm.AttrConf(  'SAV_slowdown_extended_info', True,
                                    groupnames = ['options'], 
                                    name = 'SAV slowdown extra info',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Activate to constantly display more detailed warnings\n\
                                            as well as a summary of key parameters and events,\n\
                                            e.g. timestamps of first/last planned/executed slowdowns,\n\
                                            end of current slowdown, start of next slowdown, etc.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV breakdown period, SAV breakdown interval",
                                    ))

            self.SAV_slowdown_state_sign = attrsman.add(cm.AttrConf(  'SAV_slowdown_state_sign', True,
                                    groupnames = ['options'], 
                                    name = 'SAV slowdown state sign',
                                    info = "ONLY for Sumo TRACi simulation\n\n\
                                            Activate to display (in the traci console)\n\
                                            a large sign that shows the current fleet-wide SAV slowdown state:\n\
                                            If slowdown is currently active: SLOW SPEED\n\
                                            If slowodown not active:          NORMAL SPEED\n\
                                            This large sign is useful for visually verifying the slowdown state\n\
                                            whithout needing to pause the simulation to read normal-sized console text.\n\n\
                                            To enable this feature, the following parameters must be > 0.0:\n\
                                            Update time, SAV break/slowdown period, SAV break/slowdown interval",
                                    ))
            
        
        def config_simresults(self, results):
            pass
        

        def finish_sim(self):
            pass

       
        def prepare_sim(self, process):
            print('\nTaxiService.prepare_sim  self.ident      = ' + str(self.ident))
            print('TaxiService.prepare_sim  dispatchalgorithm = ' +  str(self.dispatchalgorithm))
            if self.time_update > 0:
                if self.SAV_breakdown_interval >= self.time_update and self.SAV_breakdown_period >= self.time_update \
                and self.SAV_breakdown_speed >= 0 and (self.SAV_breakdown_interval % self.time_update) == 0 \
                and (self.SAV_breakdown_period % self.time_update) == 0 \
                and self.SAV_breakdown_random_seed >= 0 and self.SAV_breakdown_random_seed <= 4294967295 \
                and (   ( self.SAV_breakdown_starttime == -1.0 and self.SAV_breakdown_endtime > 0) or (self.SAV_breakdown_starttime >= 0 \
                          and (self.SAV_breakdown_endtime > self.SAV_breakdown_starttime or self.SAV_breakdown_endtime == -1.0)          ) \
                     or ( self.SAV_breakdown_starttime == -1.0 and self.SAV_breakdown_endtime == -1.0                                    ) ):
                    np.random.seed(self.SAV_breakdown_random_seed)
                    self.traci_SAV_breakdown_simulation = True
                    print('\nSAV-BREAKDOWN-Simulator: Breakdowns of random SAVs are being simulated.\n' \
                        + '                         Interval between breakdowns in seconds:                {:12.3f}\n'.format(self.SAV_breakdown_interval) \
                        + '                         Duration of each breakdown in seconds:                 {:12.3f}\n'.format(self.SAV_breakdown_period) \
                        + '                         Max speed during breakdown in m/s:                          {:7.3f}\n'.format(self.SAV_breakdown_speed) \
                        + '                         Max speed during breakdown in km/h:                         {:7.3f}'.format(self.SAV_breakdown_speed*3.6))
                    print('SAV-BREAKDOWN-Simulator: If teleporting active:\n' \
                        + '                         breakdown speed must be set to at least 0.11 m/s\n' \
                        + '                         to avoid teleportation\n' \
                        + '                         (recommended robust minimum: 0.14 m/s)\n')
                else:
                    self.print_SAV_breakdown_parameter_errors()

                if self.SAV_slowdown_interval >= self.time_update and self.SAV_slowdown_period >= self.time_update \
                and self.SAV_slowdown_speed >= 0 and (self.SAV_slowdown_interval % self.time_update) == 0 \
                and (self.SAV_slowdown_period % self.time_update) == 0 \
                and (   ( self.SAV_slowdown_starttime == -1.0 and self.SAV_slowdown_endtime > 0) or (self.SAV_slowdown_starttime >= 0 \
                          and (self.SAV_slowdown_endtime > self.SAV_slowdown_starttime or self.SAV_slowdown_endtime == -1.0)          ) \
                     or ( self.SAV_slowdown_starttime == -1.0 and self.SAV_slowdown_endtime == -1.0                                   ) ):
                    self.traci_SAV_slowdown_simulation = True
                    print('SAV-SLOWDOWN-Simulator: Slowdowns of the entire SAV fleet are being simulated.\n' \
                        + '                        Interval between slowdowns in seconds:                  {:12.3f}\n'.format(self.SAV_slowdown_interval) \
                        + '                        Duration of each slowdown in seconds:                   {:12.3f}\n'.format(self.SAV_slowdown_period) \
                        + '                        Max speed during slowdown in m/s:                            {:7.3f}\n'.format(self.SAV_slowdown_speed) \
                        + '                        Max speed during slowdown in km/h:                          {:7.3f}'.format(self.SAV_slowdown_speed*3.6))
                    print('SAV-SLOWDOWN-Simulator: If teleporting active:\n' \
                        + '                        slowdown speed must be set to at least 0.11 m/s\n' \
                        + '                        to avoid teleportation\n' \
                        + '                        (recommended robust minimum: 0.14 m/s)\n')
                else:
                    self.print_SAV_slowdown_parameter_errors()

                return [    (self.time_update, self.process_step),]
            else:
                print('Update time of taxi-service is not greater than 0.0 seconds:')
                print('Neither SAV-breakdowns nor -slowdowns will be simulated,')
                print('nor can the following information be given:')
                print('- information regarding taxi/SAV reservation/occupation')
                print('- extended information regarding taxi/SAV removal/disappearance from sim by SUMO\n')
                return []


        def print_normal_speed_sign(self):
            print('\n'\
                + '        __    _      _______      _______     __       __        __        __        \n'\
                + '       |MM\  |M|    /MMMMMMM\    |MMMMMMM\   |MM\     /MM|      /MM\      |M|        \n'\
                + '       |M\M\ |M|   /M/     \M\   |M|    |M|  |M\M\   /M/M|     /M/\M\     |M|        \n'\
                + '       |M|\M\|M|  |M|       |M|  |MMMMMMM/   |M|\M\ /M/|M|    /M/__\M\    |M|        \n'\
                + '       |M| \M\M|   \M\_____/M/   |M|  \M\    |M| \M M/ |M|   /MMMMMMMM\   |M|_______ \n'\
                + '       |M|  \MM|    \MMMMMMM/    |M|    \M\  |M|  \M/  |M|  /M/      \M\  |MMMMMMMM/ \n'\
                + '                                                                                     \n'\
                + '                   _______   _______    ________   ________   _______                \n'\
                + '                  /MMMMMM/  |MMMMMMM\  |MMMMMMM/  |MMMMMMM/  |MMMMMMM\               \n'\
                + '                 /M/___     |M|___|M|  |M|______  |M|______  |M|    \M\              \n'\
                + '                 \M\MM\M\   |MMMMMMM/  |MMMMMMM/  |MMMMMMM/  |M|     |M|             \n'\
                + '                 ______/M/  |M|        |M|______  |M|______  |M|____/M/              \n'\
                + '                /MMMMMM//   |M/        |MMMMMMM/  |MMMMMMM/  |MMMMMMM/               \n\n')

        def print_slow_speed_sign(self):
            print('\n'\
                + '                    _______   __            _______     __              __        \n'\
                + '                   /MMMMMM/  |M|           /MMMMMMM\    \M\            /M/        \n'\
                + '                  /M/____    |M|          /M/     \M\    \M\    /\    /M/         \n'\
                + '                  \M\MM\M\   |M|         |M|       |M|    \M\ /M/\M\ /M/          \n'\
                + '                  ______/M/  |M|_______   \M\_____/M/      \M M/  \M M/           \n'\
                + '                 /MMMMMM//   |MMMMMMMM/    \MMMMMMM/        \M/    \M/            \n'\
                + '                                                                                  \n'\
                + '                _______   _______    ________   ________   _______                \n'\
                + '               /MMMMMM/  |MMMMMMM\  |MMMMMMM/  |MMMMMMM/  |MMMMMMM\               \n'\
                + '              /M/___     |M|___|M|  |M|______  |M|______  |M|    \M\              \n'\
                + '              \M\MM\M\   |MMMMMMM/  |MMMMMMM/  |MMMMMMM/  |M|     |M|             \n'\
                + '              ______/M/  |M|        |M|______  |M|______  |M|____/M/              \n'\
                + '             /MMMMMM//   |M/        |MMMMMMM/  |MMMMMMM/  |MMMMMMM/               \n\n')


        def print_SAV_breakdown_table(self):          
            print('\n             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx')
            print('                                     SAVs CURRENTLY BROKEN')
            print('             ------------------------------------------------------------------')
            print('                 BREAKDOWN START    ||    BREAKDOWN END    ||      SAV ID      ')
            print('             ------------------------------------------------------------------')
            oldest_breakdown_time = None 
            if len(self.taxis_SAV_broken) > 0:    
                # sorting algorithm: print broken-down SAVs from oldest (top) to newest (bottom) breakdown
                taxis_SAV_broken_copy = self.taxis_SAV_broken.copy()
                for SAV in self.taxis_SAV_broken:
                    # No breakdown can be younger than simtime.
                    # Thus, simtime is the correct upper bound/starting value.
                    oldest_breakdown_time = self.simtime
                    SAV_with_oldest_breakdown_time = None
                    for SAV_oldest in taxis_SAV_broken_copy:
                        if taxis_SAV_broken_copy[SAV_oldest] <= oldest_breakdown_time:
                            oldest_breakdown_time = taxis_SAV_broken_copy[SAV_oldest]
                            SAV_with_oldest_breakdown_time = SAV_oldest
                    if self.SAV_breakdown_endtime_is_valid == True and \
                    oldest_breakdown_time + self.SAV_breakdown_period > self.SAV_breakdown_endtime:
                    #self.simtime_lastenter_breakdown + self.SAV_breakdown_interval + self.SAV_breakdown_period > self.SAV_breakdown_endtime:
                        print('                 {:12.3f}       ||   {:12.3f}'.format(oldest_breakdown_time, \
                            (self.SAV_breakdown_endtime)) \
                            + '      ||      ' + str(SAV_with_oldest_breakdown_time))
                    else:
                        print('                 {:12.3f}       ||   {:12.3f}'.format(oldest_breakdown_time, \
                            (oldest_breakdown_time + self.SAV_breakdown_period)) \
                            + '      ||      ' + str(SAV_with_oldest_breakdown_time))
                    print('             ------------------------------------------------------------------')
                    del taxis_SAV_broken_copy[SAV_with_oldest_breakdown_time]
            else:
                print('                                            NONE                                   ')
                print('             ------------------------------------------------------------------')
            print('                                SAV selected to BREAK DOWN NEXT')
            print('             ------------------------------------------------------------------')
            if self.id_of_next_SAV_to_break_down != None:
                if self.SAV_breakdown_endtime_is_valid == True and \
                self.simtime_lastenter_breakdown + self.SAV_breakdown_interval + self.SAV_breakdown_period > self.SAV_breakdown_endtime:
                    print('                 {:12.3f}       ||   {:12.3f}'.format((self.simtime_lastenter_breakdown\
                        + self.SAV_breakdown_interval), \
                        (self.SAV_breakdown_endtime)) \
                        + '      ||      ' + str(self.id_of_next_SAV_to_break_down))            
                else:
                    print('                 {:12.3f}       ||   {:12.3f}'.format((self.simtime_lastenter_breakdown\
                        + self.SAV_breakdown_interval), \
                        (self.simtime_lastenter_breakdown + self.SAV_breakdown_interval + self.SAV_breakdown_period)) \
                        + '      ||      ' + str(self.id_of_next_SAV_to_break_down))
            else:
                print('                                       NONE SELECTED')
            print('             xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n\n')
            

        def print_SAV_breakdown_parameter_errors(self):
            if self.traci_SAV_breakdown_simulation:
                pass
            else:
                if  self.SAV_breakdown_interval == -1.0 and self.SAV_breakdown_period == -1.0:
                    print('SAV-BREAKDOWN-Simulator: No breakdowns are being simulated. Reason:')
                    print('                         Deactivated because default value (-1.0) assigned to\n'\
                        + '                         breakdown interval and breakdown period\n') 
                else:
                    print('SAV-BREAKDOWN-Simulator: No breakdowns are being simulated. Reason(s):\n')
                    if self.SAV_breakdown_random_seed < 0:
                        print('                         The given random seed is < 0')
                        print('                         It does not fulfill the necessary condition:')
                        print('                         0 <= SAV_breakdown_random_seed <= 4294967295\n')
                    if self.SAV_breakdown_random_seed > 4294967295:
                        print('                         The given random seed is > 4294967295')
                        print('                         It does not fulfill the necessary condition:')
                        print('                         0 <= SAV_breakdown_random_seed <= 4294967295\n')
                    if self.SAV_breakdown_speed < 0:
                        print('                         SAV_breakdown_speed < 0 m/s\n')
                    if self.SAV_breakdown_interval < self.time_update and self.SAV_breakdown_period >= self.time_update:
                        print('                         breakdown_interval < update-time of taxi-service\n')
                    if self.SAV_breakdown_interval >= self.time_update and self.SAV_breakdown_period < self.time_update:
                        print('                         breakdown_period < update-time of taxi-service\n')
                    if self.SAV_breakdown_interval < self.time_update and self.SAV_breakdown_period < self.time_update:
                        print('                         breakdown_interval and breakdown_period < update-time of taxi-service\n')
                    if self.SAV_breakdown_interval % self.time_update != 0 and (self.SAV_breakdown_period % self.time_update) == 0:
                        print('                         breakdown_interval modulo update_time != 0\n')
                    if self.SAV_breakdown_interval % self.time_update == 0 and (self.SAV_breakdown_period % self.time_update) != 0:
                        print('                         breakdown_period modulo update_time != 0\n')
                    if self.SAV_breakdown_interval % self.time_update != 0 and (self.SAV_breakdown_period % self.time_update) != 0:
                        print('                         breakdown_interval and breakdown_period modulo update_time != 0\n')
                    if self.SAV_breakdown_starttime != -1.0 and self.SAV_breakdown_starttime < 0:
                        print('                         breakdown_starttime < 0 BUT breakdown_starttime != -1.0\n' \
                            + '                         (breakown_starttime default value of -1.0 means:\n'\
                            + '                          start first breakdown 1 breakdown-interval after the start of the sim)\n')
                    if self.SAV_breakdown_endtime != -1.0 and self.SAV_breakdown_endtime <= 0:
                        print('                         breakdown_endtime <= 0 BUT breakdown_endtime != -1.0\n' \
                            + '                         (breakdown_endtime default value of -1.0 means:\n'\
                            + '                          keep running breakdowns until the sim ends   )\n')
                    if self.SAV_breakdown_starttime >= self.SAV_breakdown_endtime:
                        print('                         breakdown_starttime >= breakdown_endtime\n')
                    print('')


        def print_SAV_slowdown_parameter_errors(self):
            if self.traci_SAV_slowdown_simulation:
                    pass
            else:
                if self.SAV_slowdown_interval == -1.0 and self.SAV_slowdown_period == -1.0:
                    print('SAV-SLOWDOWN-Simulator: No slowdowns are being simulated. Reason:')
                    print('                        Deactivated beacuse default value (-1.0) assigned to\n'\
                        + '                        slowdown interval and slowdown period\n')
                else:
                    print('SAV-SLOWDOWN-Simulator: No slowdowns are being simulated. Reason(s):\n')
                    if self.SAV_slowdown_speed < 0:
                        print('                        SAV_slowdown_speed < 0 m/s\n')
                    if self.SAV_slowdown_interval < self.time_update and self.SAV_slowdown_period >= self.time_update:
                        print('                        slowdown_interval < update-time of taxi-service\n')
                    if self.SAV_slowdown_interval >= self.time_update and self.SAV_slowdown_period < self.time_update:
                        print('                        slowdown_period < update-time of taxi-service\n')
                    if self.SAV_slowdown_interval < self.time_update and self.SAV_slowdown_period < self.time_update:
                        print('                        slowdown_interval and slowdown_period < update-time of taxi-service\n')
                    if (self.SAV_slowdown_interval % self.time_update) != 0 and (self.SAV_slowdown_period % self.time_update) == 0:
                        print('                        slowdown_interval modulo update_time != 0\n')
                    if (self.SAV_slowdown_interval % self.time_update) == 0 and (self.SAV_slowdown_period % self.time_update) != 0:
                        print('                        slowdown_period modulo update_time != 0\n')
                    if (self.SAV_slowdown_interval % self.time_update) != 0 and (self.SAV_slowdown_period % self.time_update) != 0:
                        print('                        slowdown_interval and slowdown_period modulo update_time != 0\n')
                    if self.SAV_slowdown_starttime != -1.0 and self.SAV_slowdown_starttime < 0:
                        print('                         slowdown_starttime < 0 BUT slowdown_starttime != -1.0\n' \
                            + '                         (slowdown_starttime default value of -1.0 means:\n'\
                            + '                          start first slowdown 1 slowdown-interval after the start of the sim)\n')
                    if self.SAV_slowdown_endtime != -1.0 and self.SAV_slowdown_endtime <= 0:
                        print('                         slowdown_endtime <= 0 BUT slowdown_endtime != -1.0\n' \
                            + '                         (slowdown_endtime default value of -1.0 means:\n'\
                            + '                          keep running slowdowns until the sim ends   )\n')
                    if self.SAV_slowdown_starttime >= self.SAV_slowdown_endtime:
                        print('                         slowdown_starttime >= slowdown_endtime\n')
                    print('')

        # Prints summary of key executed breakdown/slowdown timestamps and params
        def print_breakdown_or_slowdown_summary(self, breakdown_or_slowdown):

            if breakdown_or_slowdown == 'breakdown':
                print('SAV-BREAKDOWN-Simulator: Breakdown INTERVAL:                                    {:12.3f}\n'.format(self.SAV_breakdown_interval) \
                    + '                         Breakdown PERIOD:                                      {:12.3f}'.format(self.SAV_breakdown_period))
                if self.speed_in_kmh:
                    print('                         Max SPEED during breakdown in km/h:                         {:7.3f}'.format(self.SAV_breakdown_speed*3.6))
                else:
                    print('                         Max SPEED during breakdown in m/s:                          {:7.3f}'.format(self.SAV_breakdown_speed))

                if self.simtime_firstenter_breakdown != None:
                    print('\nSAV-BREAKDOWN-Simulator: Timestamps of EXECUTED start/end times\n'\
                        + '                         Start of first breakdown:                              {:12.3f}'.format(self.simtime_firstenter_breakdown))
                    if self.SAV_breakdown_endtime_executed != None:
                        print('                         Start of last breakdown:                               {:12.3f}\n'.format(self.simtime_lastenter_breakdown) \
                            + '                         End of breakdown simulations:                          {:12.3f}'.format(self.SAV_breakdown_endtime_executed))
                if self.simtime_firstenter_breakdown_planned != None and self.simtime_firstenter_breakdown != None \
                and abs(1-(self.simtime_firstenter_breakdown_planned/self.simtime_firstenter_breakdown)) > 0.00001:
                #and self.simtime_firstenter_breakdown_planned != self.simtime_firstenter_breakdown:
                    print('SAV-BREAKDOWN-Simulator: WARNING! Regarding start time of first breakdown:\n'\
                        + '                         {:.3f}'.format(self.simtime_firstenter_breakdown_planned) \
                        + ' == planned start time  !=  executed start time == {:.3f}'.format(self.simtime_firstenter_breakdown))
                    if self.SAV_breakdown_starttime_case == 0:
                        print('                         Reason: planned start time for first breakdown was\n' \
                            + '                                 lower than start time of SUMO simulation.')
                    elif self.number_of_SAVs_at_simtime_firstenter_breakdown_planned == 0:
                        print('                         Reason: no SAV existed in sim until exec. start time')
                    else:
                        print('                         Exact reason unknown.\n'\
                            + '                         Available SAVs existed in sim at planned time.\n'\
                            + '                         Probable cause:\n'\
                            + '                         update time of SUMO simu > update time of taxiservice')
                #if self.SAV_breakdown_endtime_executed != None and self.SAV_breakdown_endtime_executed != self.SAV_breakdown_endtime:
                if self.SAV_breakdown_endtime_executed != None and abs(1-(self.SAV_breakdown_endtime_executed/self.SAV_breakdown_endtime)) > 0.00001:
                    print('SAV-BREAKDOWN-Simulator: WARNING! Regarding end time of breakdown simulator:\n'\
                        + '                         {:.3f}'.format(self.SAV_breakdown_endtime) \
                        + ' == planned start time  !=  executed end time == {:.3f}'.format(self.SAV_breakdown_endtime_executed))
                    print('                         Probable cause:\n'\
                        + '                         update time of SUMO sim > update time of taxiservice')
                print('')
            
            elif breakdown_or_slowdown == 'slowdown':
                print('SAV-SLOWDOWN-Simulator: Slowdown INTERVAL:                                      {:12.3f}\n'.format(self.SAV_slowdown_interval) \
                    + '                        Slowdown PERIOD:                                        {:12.3f}'.format(self.SAV_slowdown_period))
                if self.speed_in_kmh:
                    if self.traci_SAV_breakdown_simulation:
                        print('                        Max SPEED of non-broken SAVs during slowdown in km/h:        {:7.3f}'.format(self.SAV_slowdown_speed*3.6))
                    else:
                        print('                        Max SPEED of all SAVs during slowdown in km/h:               {:7.3f}'.format(self.SAV_slowdown_speed*3.6))
                else:
                    if self.traci_SAV_breakdown_simulation:
                        print('                        Max SPEED of non-broken SAVs during slowdown in m/s:         {:7.3f}'.format(self.SAV_slowdown_speed))
                    else:
                        print('                        Max SPEED of all SAVs during slowdown in m/s:                {:7.3f}'.format(self.SAV_slowdown_speed))
                if self.simtime_firstenter_slowdown != None:
                    print('\nSAV-SLOWDOWN-Simulator: Timestamps of EXECUTED start/end times\n'\
                        + '                        Start of first slowdown:                                {:12.3f}'.format(self.simtime_firstenter_slowdown))
                    if self.SAV_slowdown_endtime_executed != None:
                        print('                        Start of last slowdown:                                 {:12.3f}\n'.format(self.simtime_lastenter_slowdown) \
                            + '                        End of slowdown simulations:                            {:12.3f}'.format(self.SAV_slowdown_endtime_executed))
                if self.simtime_firstenter_slowdown_planned != None and self.simtime_firstenter_slowdown != None \
                and abs(1-(self.simtime_firstenter_slowdown_planned/self.simtime_firstenter_slowdown)) > 0.001:
                #and self.simtime_firstenter_slowdown_planned != self.simtime_firstenter_slowdown:
                    print('SAV-SLOWDOWN-Simulator: WARNING! Regarding start time of first slowdown:\n'\
                        + '                        {:.3f}'.format(self.simtime_firstenter_slowdown_planned) \
                        + ' == planned start time  !=  executed start time == {:.3f}'.format(self.simtime_firstenter_slowdown))
                    if self.SAV_slowdown_starttime_case == 0:
                        print('                        Reason: planned start time for first slowdown was\n' \
                            + '                                lower than start time of the SUMO sim.')
                    elif self.number_of_SAVs_at_simtime_firstenter_slowdown_planned == 0:
                        print('                        Reason: no SAV existed in sim until exec. start time')
                    else:
                        print('                        Exact reason unknown.\n'\
                            + '                        Available SAVs existed in the sim at planned time.\n'\
                            + '                        Probable cause:\n'\
                            + '                        update time of SUMO sim > update time of taxiservice')
                #if self.SAV_slowdown_endtime_executed != None and self.SAV_slowdown_endtime_executed != self.SAV_slowdown_endtime:
                if self.SAV_slowdown_endtime_executed != None and abs(1-(self.SAV_slowdown_endtime_executed/self.SAV_slowdown_endtime)) > 0.001:
                    print('SAV-SLOWDOWN-Simulator: WARNING! Regarding end time of slowdown simulator:\n'\
                        + '                         {:.3f}'.format(self.SAV_slowdown_endtime) \
                        + ' == planned start time   !=  executed end time == {:.3f}'.format(self.SAV_slowdown_endtime_executed))
                    print('                         Probable cause:\n'\
                        + '                         update time of SUMO sim > update time of taxiservice')
                print('')

            else:
                print('\nTaxiService.print_summary: Error! Unexpected value passed as breakdown_or_slowdown parameter!\n')\


        def print_update_fleets_reservation_status(self):
            print('\n\nRESERVATION / OCCUPATION INFO for all SAVs and TAXIS:')
            print('*****************************************************\n')
            if len(self.taxis_SAV_all) > 0:
                print('IDs of all SAVs currently in simulation:\n' + str(self.taxis_SAV_all) + '\n')
                if len(self.taxis_SAV_idle) > 0:
                    print('IDs of SAVs which are idle:\n ' + str(self.taxis_SAV_idle) + '\n')
                else:
                    print('IDs of SAVs which are idle: NONE\n')
                if len(self.taxis_SAV_picking_up) > 0:
                    print('IDs of SAVs picking up customers:\n' + str(self.taxis_SAV_picking_up) + '\n')
                else:
                    print('IDs of SAVs picking up customers: NONE\n')
                if len(self.taxis_SAV_occupied) > 0:
                    print('IDs of SAVs occupied:\n' + str(self.taxis_SAV_occupied) + '\n')
                    if len(self.taxis_SAV_pick_and_occ) > 0:
                        print('IDs of SAVs picking up & occupied:\n' + str(self.taxis_SAV_pick_and_occ) + '\n')
                    else:
                        print('IDs of SAVs pickung up & occupied: NONE\n')
                    print('IDs of SAV PASSENGERS:')
                    self.taxis_SAV_sharing = []
                    for taxi_id in self.taxis_SAV_all:
                        no_of_passengers = traci.vehicle.getPersonNumber(taxi_id)
                        if no_of_passengers == 1:
                            print('ID of the passenger in SAV ' + str(taxi_id) +': ' + str(traci.vehicle.getPersonIDList(taxi_id)))   
                        elif no_of_passengers > 1:
                            self.taxis_SAV_sharing.append(taxi_id)
                    for taxi_id in self.taxis_SAV_sharing:
                        no_of_passengers = traci.vehicle.getPersonNumber(taxi_id)
                        print('IDs of the ' + str(no_of_passengers) + ' passengers in SAV ' + str(taxi_id) + ': '\
                            + str(traci.vehicle.getPersonIDList(taxi_id)))
                    print('')
                else:
                    print('IDs of SAVs occupied: NONE\n')
                    print('IDs of SAVs pickung up & occupied: NONE\n')
                print('')
            else:
                print('NO SAVs currently in simulation.\n')
            if len(self.taxis_private_all) > 0:
                print('IDs of all PRIVATE TAXIS currently in simulation:\n' + str(self.taxis_private_all) + '\n')
                if len(self.taxis_private_idle) > 0:
                    print('IDs of PRIVATE TAXIS which are idle:\n' + str(self.taxis_private_idle) + '\n')
                else:
                    print('IDs of PRIVATE TAXIS which are idle: NONE\n')
                if len(self.taxis_private_picking_up) > 0:
                    print('IDs of PRIVATE TAXIS picking up customers:\n' + str(self.taxis_private_picking_up) + '\n')
                else:
                    print('IDs of PRIVATE TAXIS picking up customers: NONE\n')
                if len(self.taxis_private_occupied) > 0:
                    print('IDs of PRIVATE TAXIS occupied:\n' + str(self.taxis_private_occupied) + '\n')
                    if len(self.taxis_private_pick_and_occ) > 0:
                        print('IDs of PRIVATE TAXIS picking up & occupied:\n' + str(self.taxis_private_pick_and_occ) + '\n')
                    else:
                        print('IDs of PRIVATE TAXIS pickung up & occupied: NONE\n')
                    print('IDs of PRIVATE TAXI PASSENGERS:')
                    self.taxis_private_sharing = []
                    for taxi_id in self.taxis_private_all:
                        no_of_passengers = traci.vehicle.getPersonNumber(taxi_id)
                        if no_of_passengers == 1:
                            print('ID of the passenger in private Taxi ' + str(taxi_id) +': ' + str(traci.vehicle.getPersonIDList(taxi_id))) 
                        elif no_of_passengers > 1:
                            self.taxis_private_sharing.append(taxi_id)
                    for taxi_id in self.taxis_private_sharing:
                        no_of_passengers = traci.vehicle.getPersonNumber(taxi_id)
                        print('IDs of the ' + str(no_of_passengers) + ' passengers in private Taxi ' + str(taxi_id) + ': '\
                            + str(traci.vehicle.getPersonIDList(taxi_id)))
                    if len(self.taxis_private_sharing) > 0:
                        print('\nWARNING!')
                        print('At least one PRIVATE TAXI has a passenger CAPACITY > 1')
                        print('Private Taxis SHOULD ONLY HAVE a passenger CAPACITY == 1')
                        print('SUMO (at least until 1.16) can only use 1 taxi algorithm')
                        print('for all taxi fleets in the sim.ulation')
                        print('If a ridesharing taxi algorithm is active, the only way private taxis')
                        print('can remain private (no ridesharing) is if they have a capacity of 1.')
                        print('Otherwise, they will also engage in ride-sharing!')
                        print('Exception to the above warning:')
                        print('You are using your own traci dispatch algorithm that is programmed')
                        print('to never allows ride sharing for private taxis.\n')
                else:
                    print('IDs of PRIVATE TAXIS occupied: NONE')
                    print('IDs of PRIVATE TAXIS pickung up & occupied: NONE\n')
            else:
                print('NO PRIVATE TAXIS currently in simulation.\n')
            # In order to not unnecessarily confuse normal users,
            # information about rogue taxis is only presented if they exist
            # (if they exist, they may be a problem)
            if len(self.taxis_rogue_all) > 0:
                print('\nWARNING!')
                print('There are ROGUE TAXIS in the simulation!')
                print('These taxis belong to no known taxi fleet:')
                print('neither to taxi:fleetSAV nor to taxi:fleetPrivate.')
                print('Rogue taxis can pick up both SAV and private taxi passengers.\n')
                print('\nIDs of all ROGUE TAXIS currently in simulation:\n' + str(self.taxis_rogue_all) + '\n')
                if len(self.taxis_rogue_idle) > 0:
                    print('IDs of ROGUE TAXIS which are idle:\n' + str(self.taxis_rogue_idle) + '\n')
                else:
                    print('IDs of ROGUE TAXIS which are idle: NONE\n')
                if len(self.taxis_rogue_picking_up) > 0:
                    print('IDs of ROGUE TAXIS picking up customers:\n' + str(self.taxis_rogue_picking_up) + '\n')
                else:
                    print('IDs of ROGUE TAXIS picking up customers: NONE\n')
                if len(self.taxis_rogue_occupied) > 0:
                    print('IDs of ROGUE TAXIS occupied:\n' + str(self.taxis_rogue_occupied) + '\n')
                    if len(self.taxis_rogue_pick_and_occ) > 0:
                        print('IDs of ROGUE TAXIS picking up & occupied:\n' + str(self.taxis_rogue_pick_and_occ) + '\n')
                    else:
                        print('IDs of ROGUE TAXIS pickung up & occupied: NONE\n')
                    print('IDs of ROGUE TAXI PASSENGERS:')
                    self.taxis_rogue_sharing = []
                    for taxi_id in self.taxis_rogue_all:
                        no_of_passengers = traci.vehicle.getPersonNumber(taxi_id)
                        if no_of_passengers == 1:
                            print('ID of the passenger in rogue Taxi ' + str(taxi_id) +': ' + str(traci.vehicle.getPersonIDList(taxi_id))) 
                        elif no_of_passengers > 1:
                            self.taxis_rogue_sharing.append(taxi_id)
                    for taxi_id in self.taxis_rogue_sharing:
                        no_of_passengers = traci.vehicle.getPersonNumber(taxi_id)
                        print('IDs of the ' + str(no_of_passengers) + ' passengers in rogue Taxi ' + str(taxi_id) + ': '\
                            + str(traci.vehicle.getPersonIDList(taxi_id)))
                else:
                    print('IDs of ROGUE TAXIS occupied: NONE')
                    print('IDs of ROGUE TAXIS pickung up & occupied: NONE\n')
            print('')

        # process_step is equivalent to the main loop of the taxi/SAV service.
        # It only gets called if you launch the microscopic simulation in SUMO TRACi mode.
        # It gets called whenever time_update has passed.
        def process_step(self, process):

            self.simtime = process.simtime
            self.starttime = process.simtime_start
            if self.simtime_firstenter_processstep == None:
                self.simtime_firstenter_processstep = self.simtime

            edges = self.get_scenario().net.edges
            
            # list with id_sumo of all taxis
            ids_taxi_sumo_empty = traci.vehicle.getTaxiFleet(0)
            ids_taxi_sumo_pickup = traci.vehicle.getTaxiFleet(1)
            ids_taxi_sumo_occupied = traci.vehicle.getTaxiFleet(2)

            print(49*'= ' + '=')
            #print 'TaxiService.process_step           simtime == ',self.simtime
            print('\nTaxiService.process_step dispatchalgorithm = ' +  str(self.dispatchalgorithm))
            print('\n')
            self.update_status_taxis_and_SAVs()
            self.simulate_SAV_breakdowns()

            # For proper functionality:
            # The function simulate_SAV_slowdowns MUST be called AFTER simulate_SAV_breakdowns. Teason:
            # self.simulate_SAV_slowdowns needs the the most up-to-date data of broken-down SAVs,
            # so broken SAVs can stay at their break-down speed
            # no matter if an SAV-fleet-wide slowdown is currently active or not.
            self.simulate_SAV_slowdowns()
            if self.traci_taxi_fleet_reservation_info:
                self.print_update_fleets_reservation_status()
            print(49*'= ' + '=')          
        

        def select_next_SAV_to_break_down(self, print_successful_selection = True):
            
            if len(self.taxis_SAV_not_broken) > 0:
                self.id_of_next_SAV_to_break_down = self.select_random_non_broken_SAV()
                if print_successful_selection == True:
                    print('SAV-BREAKDOWN-Simulator: The newly selected, non-broken SAV')
                    print('                         to break down next is SAV with id ' \
                        + str(self.id_of_next_SAV_to_break_down) + '\n')

            # If all SAVs are broken, find a the broken SAV with the oldest breakdown time.
            # Reason: if the following condition is fulfilled:
            # [(breakdown_period/breakdown_interval)/number_of_SAVs] >= 1
            # Then: mathematically, all SAVs should always be broken down.
            # If only broken SAVs exist in the sim, then:
            # The SAV that will to finish it's current breakdown the soonest
            # (= the SAV that has the oldest breakdown time)
            # should immediately restart breakdown state.
            # If e.g. the SAV with the most recent breakdown were selected
            # to be the next to break down (e.g. selected randomly),
            # then the SAV with the oldest breakdown could be non-broken for a while,
            # which goes against the breakdown period and interval.
            # In this case, keeping the timing intervals/periods correct was seen as more important
            # than complete randomness.
            # Therefore:
            # If all SAVs are (always) broken, select the broken SAV with the oldest breakdown time
            # as the next SAV to break down.
            elif len(self.taxis_SAV_broken) > 0:
                # find the broken SAV with the oldest break-down
                # No breakdown can be younger than simtime.
                oldest_breakdown_time = self.simtime
                SAV_with_oldest_breakdown_time = None
                for SAV_oldest in self.taxis_SAV_broken:
                    if self.taxis_SAV_broken[SAV_oldest] <= oldest_breakdown_time:
                            oldest_breakdown_time = self.taxis_SAV_broken[SAV_oldest]
                            SAV_with_oldest_breakdown_time = SAV_oldest
                self.id_of_next_SAV_to_break_down = SAV_with_oldest_breakdown_time
                print('SAV-BREAKDOWN-Simulator: Since there is no non-broken SAV available now,')
                print('                         the SAV closest to finishing its current breakdown')
                print('                         will immeadiately restart a new breakdown.')
                print('                         The SAV selected to break down next is')
                print('                         the currently broken SAV with id ' \
                    + str(self.id_of_next_SAV_to_break_down) +'\n')
            else:
                print('SAV-BREAKDOWN-Simulator: WARNING! self.id_of_next_SAV_to_break_down is None,')
                print('                         AND there are no SAVs in the simulation.')
                print('                         Hence, no SAV can be selected for break down.\n')


        # Select the next random, not-currently-broken-down SAV to break down:
        def select_random_non_broken_SAV(self):
            
            if len(self.taxis_SAV_all) < 1:
                print('SAV-BREAKDOWN-Simulator: Right now, no SAV can be selected to break down\n' \
                    +     '                         because there are no SAVs in the simulation.')
                return None
            elif len(self.taxis_SAV_all) > 0 and len(self.taxis_SAV_broken) < 1 and len(self.taxis_SAV_not_broken) < 1:
                print('SAV-BREAKDOWN-Simulator: ERROR!')
                print('                         There is at least 1 SAV in the simulation,')
                print('                         AND no SAVs are broken.')
                print('                         BUT: there are also no non-broken SAVs!')
                return None
            elif len(self.taxis_SAV_not_broken) < 1:
                print('SAV-BREAKDOWN-Simulator: Right now, no SAV can be selected to break down\n' \
                    +     '                         because all SAVs are already broken down.')
                return None
            elif len(self.taxis_SAV_not_broken) == 1:
                return self.taxis_SAV_not_broken[0]
            elif len(self.taxis_SAV_not_broken) > 0:
                return self.taxis_SAV_not_broken[np.random.randint(0, len(self.taxis_SAV_not_broken)-1)]
            else:
                print('\nSAV-BREAKDOWN-Simulator: ERROR! Entered ELSE CASE of self.select_random_non_broken_SAV()!\n')
                return None


        def simulate_SAV_breakdowns(self):

            if self.SAV_breakdown_simulator_called == False:
                self.SAV_breakdown_simulator_called = True
            
            if self.traci_SAV_breakdown_simulation:

                # Check if the given start time and end time for the breakdown simulator are valid
                # (this if statement is only entered once)
                if self.SAV_breakdown_startendtimes_validated == False:
                    
                    self.SAV_breakdown_starttime_case, self.SAV_breakdown_endtime_case, \
                    self.SAV_breakdown_starttime_is_valid, self.SAV_breakdown_endtime_is_valid \
                    = self.validate_startendtimes(self.SAV_breakdown_starttime, self.SAV_breakdown_endtime)
                    
                    # see validate_startendtimes_case_interpreter() for case meaning
                    if self.SAV_breakdown_starttime_case == 0:
                        self.simtime_firstenter_breakdown_planned = self.simtime_firstenter_processstep
                    elif self.SAV_breakdown_starttime_case == 1:
                        self.simtime_firstenter_breakdown_planned = self.SAV_breakdown_starttime
                    else:
                        self.simtime_firstenter_breakdown_planned = self.simtime_firstenter_processstep + self.SAV_breakdown_interval
                    
                    self.SAV_breakdown_startendtimes_validated = True

                if self.simtime >= self.simtime_firstenter_breakdown_planned \
                and self.number_of_SAVs_at_simtime_firstenter_breakdown_planned_selected == False:
                    self.number_of_SAVs_at_simtime_firstenter_breakdown_planned = len(self.taxis_SAV_all)
                    self.number_of_SAVs_at_simtime_firstenter_breakdown_planned_selected = True

                if self.SAV_breakdown_extended_info:    
                    # print information regarding the start time and end time case of the breakdown simulator
                    self.validate_startendtimes_case_interpreter('breakdown', self.SAV_breakdown_starttime_case, self.SAV_breakdown_endtime_case)
                    # print summary of key executed breakdown timestamps and params
                    self.print_breakdown_or_slowdown_summary('breakdown')

                # if the breakdown end time has been reached:
                if self.SAV_breakdown_endtime_is_valid == True and self.simtime >= self.SAV_breakdown_endtime:
                    if self.SAV_breakdown_endtime_reached == False:
                        if len(self.taxis_SAV_broken) > 0:
                            for SAV in self.taxis_SAV_broken:
                                # If slow-down period has not yet passed, but if breakdown simulation active:
                                # --> Slow the recently un-broken ones down and add them to the slowed-down list.
                                if self.fleet_slowed_down_flag:
                                    traci.vehicle.setSpeed(SAV,self.SAV_breakdown_speed)
                                else:
                                    traci.vehicle.setSpeed(SAV,-1)
                        self.SAV_breakdown_endtime_executed = self.simtime
                        self.id_of_next_SAV_to_break_down = None
                        self.taxis_SAV_no_longer_broken = []
                        self.SAVs_removed_from_sim = []
                        self.taxis_SAV_broken = {}
                        self.taxis_SAV_not_broken = []
                        for SAV in self.taxis_SAV_all:
                            self.taxis_SAV_not_broken.append(SAV)
                        self.SAV_breakdown_endtime_reached = True
                        print('SAV-BREAKDOWN-Simulator: The breakdown simulator end time\n' \
                            + '                         of {:.3f} has been reached.\n'.format(self.SAV_breakdown_endtime) \
                            + '                         No more SAV breakdowns are being simulated.\n')
                    elif self.SAV_breakdown_extended_info and len(self.taxis_SAV_all) < 1:
                        print('SAV-BREAKDOWN-Simulator: No random SAV breakdowns can be simulated\n' \
                                +     '                         because there are no SAVs in the simulation.\n' \
                                +     '                         Furthermore:')
                    print('SAV-BREAKDOWN-Simulator: The breakdown simulator end time\n' \
                        + '                         of {:.3f} has been reached.\n'.format(self.SAV_breakdown_endtime) \
                        + '                         No more SAV breakdowns are being simulated.\n')

                # if the breakdown end time has not yet been reached:
                else:
                    # If the initial_SAV_to_break_down has NOT yet been selected:
                    if self.initial_SAV_to_break_down_selected is False or self.breakdown_simulator_start_time_reached is False:

                        if self.SAV_breakdown_extended_info is False:
                            print('SAV-BREAKDOWN-Simulator: Waiting to start first SAV breakdown.\n')
                        
                        # If there is currently at least 1 SAV in the simulation: 
                        if len(self.taxis_SAV_all) > 0:

                            # Update the list of NOT-broken-down SAVs (in case any were deleted/added by SUMO)
                            self.update_taxis_SAV_not_broken()

                            # If a valid breakdown simulator start time is given:
                            # --> start the first of the sim immediately at the breakdown sim start time
                            # This is achieved by tricking the algorithm into thinking that the previous breakdown
                            # occured 1 interval before the breakdown simulator start time
                            if self.SAV_breakdown_starttime_is_valid == True:

                                # if a valid starttime is given, the algorithm should immediately start the first breakdown at that time.
                                # This is achieved by tricking the algorithm into thinking that the previous breakdown
                                # occured 1 interval before the breakdown simulator start time
                                if self.simtime_lastenter_breakdown == None:
                                    self.simtime_lastenter_breakdown = self.SAV_breakdown_starttime - self.SAV_breakdown_interval
                                
                                if self.id_of_next_SAV_to_break_down == None:
                                    self.select_next_SAV_to_break_down()
                                    self.initial_SAV_to_break_down_selected = True
                                else:
                                    # Check if the SAV that has been selected for the next random breakdown has been removed from the simulation.
                                    # If so, find a new random SAV to simulate a breakdown with:
                                    self.update_id_of_next_SAV_to_break_down()
                                
                                # If the simulator start time has been reached:
                                if self.SAV_breakdown_starttime <= self.simtime:
                                    self.breakdown_simulator_start_time_reached = True
                        
                            # If no valid start time for the breakdown simulator is given,
                            # use the following default behavior:
                            # In order to let SAVs leave spawn locations properly,
                            # Wait 1 breakdown interval before starting breakdowns
                            # This is achieved by tricking the algorithm into thinking a breakdown occured at the start
                            else:

                                if self.simtime_lastenter_breakdown == None:
                                    self.simtime_lastenter_breakdown = self.simtime
                                self.select_next_SAV_to_break_down()
                                self.initial_SAV_to_break_down_selected = True
                                self.breakdown_simulator_start_time_reached = True
                            
                        else:
                            print('SAV-BREAKDOWN-Simulator: Right now, no SAV can be selected to break down\n' \
                                +     '                         because there are no SAVs in the simulation.\n')

                    # If the initial_SAV_to_break_down has already been selected:
                    if self.initial_SAV_to_break_down_selected is True and self.breakdown_simulator_start_time_reached is True:
                            
                        # Only perform breakdown code if there are any SAVs in the simulation:
                        if len(self.taxis_SAV_all) > 0:

                            # Update the dictionary of BROKEN-down SAVs in case any were deleted.
                            # (e.g. if it was removed by Sumo automatically after it ended idling in a cul-de-sac)
                            self.update_taxis_SAV_broken()
                            # Update the list of NOT-broken-down SAVs (in case any were deleted/added by SUMO)
                            self.update_taxis_SAV_not_broken()
                            # Go through the list and the broken-down SAVs that have fulfilled/exceeded their breakdown time,
                            # and return their speed to normal
                            self.update_taxis_SAV_no_longer_broken()
                            # Check if the SAV that has been selected for the next random breakdown has been removed from the simulation.
                            # If so, find a new random SAV to simulate a breakdown with:
                            self.update_id_of_next_SAV_to_break_down()

                            #if self.SAV_breakdown_starttime_is_valid == True and  iF SMALLER THAN SIMTIME?????

                            # If the initial breakdown has NOT yet occured:
                            if self.initial_breakdown_occured is False:

                                if self.simtime-self.simtime_lastenter_breakdown >= self.SAV_breakdown_interval:
                                    self.simtime_firstenter_breakdown = self.simtime
                                    self.simtime_lastenter_breakdown = self.simtime
                                    self.taxis_SAV_not_broken.remove(self.id_of_next_SAV_to_break_down)
                                    # Add the SAV the dictionary of broken SAVs and save the breakdown timestamp:
                                    self.taxis_SAV_broken[self.id_of_next_SAV_to_break_down] = self.simtime
                                    # slow down of the newest SAV in the self.taxis_SAV_broken list to 0.14 m/s (0.5 km/h)
                                    # speeds below 0.11 m/s will make SUMO teleport vehicles if teleporting active
                                    # using slow down instead of actual stop command to prevent SAV from being teleported
                                    traci.vehicle.setSpeed(self.id_of_next_SAV_to_break_down, self.SAV_breakdown_speed)
                                    print('SAV-BREAKDOWN-Simulator: START of the first BREAKDOWN NOW\n' \
                                        + '                         (breakdown of SAV with id ' \
                                        + str(self.id_of_next_SAV_to_break_down) + ')')
                                    # Select the next random, not-currently-broken-down SAV to break down:
                                    self.select_next_SAV_to_break_down()
                                    self.initial_breakdown_occured = True
                                elif self.SAV_breakdown_extended_info is False:
                                    print('SAV-BREAKDOWN-Simulator: Waiting to start first SAV breakdown.\n')           

                                self.initial_SAV_breakdown_loop_passed = True

                            # If the initial breakdown has already occured:
                            else:                                

                                if self.SAV_breakdown_extended_info is False:
                                    print('SAV-BREAKDOWN-Simulator: Simulating breakdowns of random SAVs.\n')

                                if self.simtime-self.simtime_lastenter_breakdown >= self.SAV_breakdown_interval:
                                    self.simtime_lastenter_breakdown = self.simtime
                                    # if the next SAV to break down is not currently broken, remove it from the non-broken list
                                    if self.taxis_SAV_not_broken.count(self.id_of_next_SAV_to_break_down) > 0:
                                        self.taxis_SAV_not_broken.remove(self.id_of_next_SAV_to_break_down)
                                    # if the next SAV to break down is already broken, do nothing
                                    # (a couple lines later, just issue new breakdown command and timestamp)
                                    elif list(self.taxis_SAV_broken.keys()).count(self.id_of_next_SAV_to_break_down) > 0:
                                        pass
                                    # if self.id_of_next_SAV_to_break_down is unkown
                                    # (e.g. if == None after vehicles removed from sim)
                                    else:
                                        self.select_next_SAV_to_break_down()
                                    print('SAV-BREAKDOWN-Simulator: A NEW BREAKDOWN will START NOW\n'\
                                        + '                         (breakdown of SAV with id ' \
                                        + str(self.id_of_next_SAV_to_break_down) + ')')
                                    # Add the SAV the dictionary of broken SAVs and save the breakdown timestamp:
                                    self.taxis_SAV_broken[self.id_of_next_SAV_to_break_down] = self.simtime
                                    # slow down of the newest SAV in the self.taxis_SAV_broken list to 0.14 m/s (0.5 km/h)
                                    # speeds below 0.11 m/s will make SUMO teleport vehicles if teleporting active
                                    # using slow down instead of actual stop command to prevent SAV from being teleported
                                    traci.vehicle.setSpeed(self.id_of_next_SAV_to_break_down, self.SAV_breakdown_speed)
                                    # Update the list of NOT-broken-down SAVs (in case any were deleted/added by SUMO)
                                    self.update_taxis_SAV_not_broken()
                                    # Select the next random, not-currently-broken-down SAV to break down:    
                                    #self.id_of_next_SAV_to_break_down = self.select_random_non_broken_SAV()
                                    self.select_next_SAV_to_break_down()

                        # # If there are currently no SAVs in the simulation:
                        else:
                            self.id_of_next_SAV_to_break_down = None
                            self.taxis_SAV_not_broken = []
                            self.taxis_SAV_no_longer_broken = []
                            self.SAVs_removed_from_sim = []
                            self.taxis_SAV_broken = {}
                            print('SAV-BREAKDOWN-Simulator: Right now, no SAV can be selected to break down\n' \
                                + '                         because there are no SAVs in the simulation.\n')
                
                if self.SAV_breakdown_overview_table:
                    self.print_SAV_breakdown_table()
                else:
                    print('')        

            else:
                self.print_SAV_breakdown_parameter_errors()
                print('')


        # For proper functionality:
        # The function simulate_SAV_slowdowns MUST be called AFTER simulate_SAV_breakdowns. Reason:
        # self.simulate_SAV_slowdowns needs the the most up-to-date data of broken-down SAVs,
        # so broken SAVs can stay at their break-down speed
        # no matter if an SAV-fleet-wide slowdown is currently active or not.
        def simulate_SAV_slowdowns(self):

            # if self.SAV_breakdown_simulator_called was not set to True by self.simulate_SAV_breakdowns()
            if self.SAV_breakdown_simulator_called == False:
                print('TaxiService: MAJOR ERROR!\n'\
                +   '             In coremodules/simulation/taxi.py, the process_step function did not call\n'\
                +   '             self.simulate_SAV_breakdowns() before self.simulate_SAV_slowdowns().\n')
                +   '             For proper functionality, in each process step,\n'\
                +   '             self.simulate_SAV_breakdowns() MUST be called\n'\
                +   '             BEFORE self.simulate_SAV_slowdowns()'
            
            # if self.SAV_breakdown_simulator_called was set to True by self.simulate_SAV_breakdowns()
            else:
                self.SAV_breakdown_simulator_called = False

                if self.traci_SAV_slowdown_simulation:

                    # Check if the given start time and end time for the slowdown simulator are valid
                    # (this if statement is only entered once)
                    if self.SAV_slowdown_startendtimes_validated == False:
                        
                        self.SAV_slowdown_starttime_case, self.SAV_slowdown_endtime_case, \
                        self.SAV_slowdown_starttime_is_valid, self.SAV_slowdown_endtime_is_valid \
                        = self.validate_startendtimes(self.SAV_slowdown_starttime, self.SAV_slowdown_endtime)
                         
                        # see validate_startendtimes_case_interpreter() for case meaning
                        if self.SAV_slowdown_starttime_case == 0:
                            self.simtime_firstenter_slowdown_planned = self.simtime_firstenter_processstep
                        elif self.SAV_slowdown_starttime_case == 1:
                            self.simtime_firstenter_slowdown_planned = self.SAV_slowdown_starttime
                        else:
                            self.simtime_firstenter_slowdown_planned = self.simtime_firstenter_processstep + self.SAV_slowdown_interval
                        
                        self.SAV_slowdown_startendtimes_validated = True

                    if self.simtime >= self.simtime_firstenter_slowdown_planned \
                    and self.number_of_SAVs_at_simtime_firstenter_slowdown_planned_selected == False:
                        self.number_of_SAVs_at_simtime_firstenter_slowdown_planned = len(self.taxis_SAV_all)
                        self.number_of_SAVs_at_simtime_firstenter_slowdown_planned_selected = True

                    if self.SAV_slowdown_extended_info:
                        # print information regarding the start time and end time case of the slowdown simulator
                        self.validate_startendtimes_case_interpreter('slowdown', self.SAV_slowdown_starttime_case, self.SAV_slowdown_endtime_case)
                        # print summary of key executed slowdown timestamps and params
                        self.print_breakdown_or_slowdown_summary('slowdown')

                    # Check if the given start time and end time for the slowdown simulator is valid
                    # (this if statement is only entered once)
                    if self.SAV_slowdown_startendtimes_validated == False:
                        self.validate_slowdown_startendtimes()

                    # if the slowdown end time has been reached:
                    if self.SAV_slowdown_endtime_is_valid == True and self.simtime >= self.SAV_slowdown_endtime:
                        if self.SAV_slowdown_endtime_reached == False:
                            if len(self.taxis_SAV_all) > 0:
                                if self.traci_SAV_breakdown_simulation == True and self.SAV_breakdown_endtime_reached == False:
                                    for SAV in self.taxis_SAV_not_broken:
                                        traci.vehicle.setSpeed(SAV,-1)
                                else:
                                    for SAV in self.taxis_SAV_all:
                                        traci.vehicle.setSpeed(SAV,-1)
                            self.SAV_slowdown_endtime_executed = self.simtime
                            self.fleet_slowed_down_flag = False
                            self.SAV_slowdown_endtime_reached = True
                            print('SAV-SLOWDOWN-Simulator: The slowdown-simulator end time\n' \
                                + '                        of {:.3f} has been reached.\n'.format(self.SAV_slowdown_endtime) \
                                + '                        No more fleet-wide SAV slowdowns are being simulated.')                        
                        elif self.SAV_slowdown_extended_info and len(self.taxis_SAV_all) < 1:
                            print('SAV-SLOWDOWN-Simulator: No fleet-wide SAV slowdown can be simulated.\n' \
                                +     '                        because there are no SAVs in the simulation.\n' \
                                +     '                        Furthermore:')
                        print('SAV-SLOWDOWN-Simulator: The slowdown-simulator end time\n' \
                                + '                        of {:.3f} has been reached.\n'.format(self.SAV_slowdown_endtime) \
                                + '                        No more fleet-wide SAV slowdowns are being simulated.')
                        if self.fleet_slowed_down_flag is True:
                            self.print_slow_speed_sign()
                        else:
                            self.print_normal_speed_sign()

                    # if the slowdown end time has not yet been reached:
                    else:
                        # In order to let SAVs leave spawn locations properly:
                        # Wait 1 slowdown interval before starting slowdowns (by tricking algorithm into thinking a breakdown occured at the start):
                        if self.simtime_lastenter_slowdown == None:
                            # if a valid starttime is given, the algorithm should immediately start the first slowdown at that time.
                            # This is achieved by tricking the algorithm into thinking that the previous slowdown
                            # occured 1 interval before the slowdown simulator start time
                            if self.SAV_slowdown_starttime_is_valid == True:
                                self.simtime_lastenter_slowdown = self.SAV_slowdown_starttime - self.SAV_slowdown_interval
                            else:
                                self.simtime_lastenter_slowdown = self.simtime

                        # If there is currently at least 1 SAV in the simulation:
                        if len(self.taxis_SAV_all) > 0:

                            # If the inital slowdown has NOT yet occured:
                            if self.initial_slowdown_flag is False:

                                if self.SAV_breakdown_extended_info is False:
                                    print('SAV-SLOWDOWN-Simulator: Waiting to start first slowdown of SAV fleet.')

                                # if the slowdown interval has passed
                                if self.simtime-self.simtime_lastenter_slowdown >= self.SAV_slowdown_interval:
                                    self.simtime_firstenter_slowdown = self.simtime
                                    self.simtime_lastenter_slowdown = self.simtime
                                    # If the SAV-breakdown-simulation is active:
                                    # Broken-down SAVs are not affected by fleetwide slowdowns.
                                    # If such an SAV stops being broken down, it will also be slowed down like the rest of the fleet.
                                    # New random breakdowns will continue to be simulated, whether a fleetwide slowdown is active or not.
                                    if self.traci_SAV_breakdown_simulation:
                                        for SAV in self.taxis_SAV_not_broken:
                                            traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                                    else:
                                        for SAV in self.taxis_SAV_all:
                                            traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                                    print('SAV-SLOWDOWN-Simulator: START of the first fleet-wide SLOWDOWN NOW')
                                    self.initial_slowdown_flag = True
                                    self.fleet_slowed_down_flag = True         

                            # If the initial slowdown has already occured:
                            else:
                                # Logic: If slowdown_period >= slowdown_interval, then SAV-fleet will be permanently slowed down.
                                # Hence: Only enter un-slowdown code, if slowdown_period < slowdown_interval.
                                if self.SAV_slowdown_period >= self.SAV_slowdown_interval:
                                    pass
                                elif self.simtime-self.simtime_lastenter_slowdown >= self.SAV_slowdown_interval:
                                    self.simtime_lastenter_slowdown = self.simtime
                                    if self.traci_SAV_breakdown_simulation:
                                        for SAV in self.taxis_SAV_not_broken:
                                            traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                                    else:
                                        for SAV in self.taxis_SAV_all:
                                            traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                                    print('SAV-SLOWDOWN-Simulator: START of the next fleet-wide SLOWDOWN NOW')
                                    if self.SAV_slowdown_endtime_is_valid == False \
                                    or self.simtime_lastenter_slowdown + self.SAV_slowdown_period < self.SAV_slowdown_endtime:
                                        print('SAV-SLOWDOWN-Simulator: This fleet-wide SAV-SLOWDOWN will END at simtime ' \
                                            + '{:12.3f}'.format(self.simtime_lastenter_slowdown + self.SAV_slowdown_period))
                                    else:
                                        print('SAV-SLOWDOWN-Simulator: This fleet-wide SAV-SLOWDOWN will END at simtime ' \
                                            + '{:12.3f}'.format(self.SAV_slowdown_endtime))
                                    self.fleet_slowed_down_flag = True
                                # After slowdown_period has passed, return speed of non-broken-down SAVs back to normal
                                elif self.fleet_slowed_down_flag is True and self.simtime-self.simtime_lastenter_slowdown >= self.SAV_slowdown_period:
                                    self.simtime_lastexit_slowdown = self.simtime
                                    if self.traci_SAV_breakdown_simulation:   # THIS PART SEEMS TO WORK
                                        for SAV in self.taxis_SAV_not_broken:
                                            traci.vehicle.setSpeed(SAV,-1)
                                    else:
                                        for SAV in self.taxis_SAV_all:
                                            traci.vehicle.setSpeed(SAV,-1)
                                    print('SAV-SLOWDOWN-Simulator: END of most recent Fleetwide SAV-SLOWDOWN just NOW')
                                    if self.traci_SAV_breakdown_simulation:
                                        print('                        All non-broken SAVs are driving at normal speeds again.')
                                    else:
                                        print('                        All SAVs are driving at normal speeds again.')
                                    if self.SAV_slowdown_endtime_is_valid == False \
                                    or self.simtime_lastenter_slowdown + self.SAV_slowdown_interval < self.SAV_slowdown_endtime:
                                        print('SAV-SLOWDOWN-Simulator: The NEXT fleet-wide SAV-slowdown will START AT simtime  ' \
                                            + '{:12.3f}'.format(self.simtime_lastenter_slowdown + self.SAV_slowdown_interval))
                                    self.fleet_slowed_down_flag = False
                                else:
                                    if self.fleet_slowed_down_flag:
                                        # send out the slowdown command to newly appeared SAVs
                                        if len(self.taxis_SAV_all_new) > 0:
                                            if self.traci_SAV_breakdown_simulation:
                                                for SAV in self.taxis_SAV_all_new:
                                                    if self.taxis_SAV_not_broken.count(SAV) > 0:
                                                        traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                                            else:
                                                for SAV in self.taxis_SAV_all_new:
                                                    traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                                        print('SAV-SLOWDOWN-Simulator: SAV fleet CURRENTLY in SLOWDOWN state.')
                                        if self.SAV_slowdown_extended_info:
                                            print('SAV-SLOWDOWN-Simulator: The current fleet-wide SAV-SLOWDOWN  STARTED at simtime ' \
                                                + '{:12.3f}'.format(self.simtime_lastenter_slowdown))
                                            if self.SAV_slowdown_endtime_is_valid == False \
                                            or self.simtime_lastenter_slowdown + self.SAV_slowdown_period < self.SAV_slowdown_endtime:
                                                print('SAV-SLOWDOWN-Simulator: The current fleet-wide SAV-SLOWDOWN will END at simtime ' \
                                                + '{:12.3f}'.format(self.simtime_lastenter_slowdown + self.SAV_slowdown_period))
                                            else:
                                                print('SAV-SLOWDOWN-Simulator: The current fleet-wide SAV-SLOWDOWN will END at simtime ' \
                                                + '{:12.3f}'.format(self.SAV_slowdown_endtime))
                                    else:
                                        print('SAV-SLOWDOWN-Simulator: SAV fleet NOT currently in SLOWDOWN state.')
                                        if self.SAV_slowdown_extended_info:
                                            print('SAV-SLOWDOWN-Simulator: The PREVIOUS fleet-wide slowdown    ENDED   at simtime  ' \
                                                + '{:12.3f}'.format(self.simtime_lastexit_slowdown))
                                            if self.SAV_slowdown_endtime_is_valid == False \
                                            or self.simtime_lastenter_slowdown + self.SAV_slowdown_interval < self.SAV_slowdown_endtime:
                                                print('SAV-SLOWDOWN-Simulator: The NEXT fleet-wide SAV-slowdown will START AT simtime  ' \
                                                    + '{:12.3f}'.format(self.simtime_lastenter_slowdown + self.SAV_slowdown_interval))                        

                            if (self.SAV_slowdown_extended_info == True) and (self.SAV_slowdown_period >= self.SAV_slowdown_interval):
                                print('SAV-SLOWDOWN-Simulator: WARNING:\n' \
                                    + '                        The SAV fleet will never exit the slowdown state\n'\
                                    + '                        during this simulation.\n')
                                if self.traci_SAV_breakdown_simulation:
                                    print('                        (except for random SAVs entering a breakdown state)\n')
                                print('                        Reason:\n' \
                                    + '                        slowdown_period = ' + str(self.SAV_slowdown_period)\
                                    + '  >=  slowdown_interval = ' + str(self.SAV_slowdown_interval))   
                            
                            if self.SAV_slowdown_state_sign:
                                if self.fleet_slowed_down_flag is True:
                                    self.print_slow_speed_sign()
                                else:
                                    self.print_normal_speed_sign()
                            else:
                                print('')

                        # If there are currently no SAVs in the simulation:
                        else:
                            print('SAV-SLOWDOWN-Simulator: Right now, no slowdown of SAV fleet can be simulated\n' \
                                + '                        because there are no SAVs in the simulation.')
                else:
                    self.print_SAV_slowdown_parameter_errors()
                    print('')


        def update_id_of_next_SAV_to_break_down(self):
            
            # If an SAV has already been selected to break down next
            if self.id_of_next_SAV_to_break_down != None:
                
                # Check if the SAV that has been selected for the next random breakdown has been removed from the simulation.
                # If so, find a new random SAV to simulate a breakdown with:
                if self.taxis_SAV_all.count(self.id_of_next_SAV_to_break_down) < 1:
                    print('SAV-BREAKDOWN-Simulator: WARNING! The previously selected SAV to break down\n' \
                        + '                         (SAV with id ' + str(self.id_of_next_SAV_to_break_down) + ')\n' \
                        + '                         has been removed from the simulation by SUMO.\n' \
                        + '                         (e.g. after getting stuck due to error in network)')
                    self.select_next_SAV_to_break_down()

                # else: if the previously selected next SAV to break down is an already broken SAV
                # (chosen when no non-broken ones were available), but new non-broken SAVs have appeared:
                # choose a non-broken SAV to break down next instead:
                elif list(self.taxis_SAV_broken.keys()).count(self.id_of_next_SAV_to_break_down) > 0 \
                and len(self.taxis_SAV_not_broken) > 0:
                    print('SAV-BREAKDOWN-Simulator: UPDATE: Due to a lack of non-broken SAVs at the time,\n'\
                        + '                         the previously selected next SAV to break down \n' \
                        + '                         (SAV with id ' + str(self.id_of_next_SAV_to_break_down) + ')\n' \
                        + '                         was an already-broken SAV.')
                    self.select_next_SAV_to_break_down(print_successful_selection = False)
                    print('                         Since new, non-broken SAVs have appeared in the sim,\n'\
                        + '                         a non-broken SAV ( SAV with id ' \
                        + str(self.id_of_next_SAV_to_break_down) + ')\n'\
                        + '                         has been selected to break down next instead.')


        def update_status_taxis_and_SAVs(self):
            
            self.taxis_all = traci.vehicle.getTaxiFleet(-1)
            self.taxis_SAV_all = []
            self.taxis_private_all = []
            for mytaxi in self.taxis_all:
                #update last known time and position
                # (if a vehicle has been removed from sim, no problem:
                #  they are not in self.taxis_all, so the traci command
                #  to check the edge/lane of a no-longer-existent vehicle
                #  will never be executed.
                #  But the last-known edge/lane of that vehicle is still stored)
                self.taxis_all_final_timestep[mytaxi] = self.simtime
                self.taxis_all_final_edge_id[mytaxi] = traci.vehicle.getRoadID(mytaxi)     
                self.taxis_all_final_lane_id[mytaxi] = traci.vehicle.getLaneID(mytaxi)
                self.taxis_all_final_lane_index[mytaxi] = traci.vehicle.getLaneIndex(mytaxi)
                # get position of the vehicle along the lane measured in m:
                self.taxis_all_final_lane_position[mytaxi] = traci.vehicle.getLanePosition(mytaxi)
                
                # update taxi fleets
                if traci.vehicle.getLine(mytaxi) == "taxi:fleetSAV":
                    self.taxis_SAV_all.append(mytaxi)
                elif traci.vehicle.getLine(mytaxi) == "taxi:fleetPrivate":
                    self.taxis_private_all.append(mytaxi)
                else:
                    self.taxis_rogue_all.append(SAV)

            # Add newly detected rogue taxis to the list:
            self.taxis_rogue_all_new = []
            for mytaxi in self.taxis_rogue_all:
                if self.taxis_rogue_all_old.count(mytaxi) < 1:
                    self.taxis_rogue_all_old.append(mytaxi)
                    self.taxis_rogue_all_new.append(maytaxi)
            # Delete removed rogue taxis from the list and print a warning:
            for mytaxi in self.taxis_rogue_all_old:
                if self.taxis_rogue_all.count(mytaxi) < 1:
                    self.taxis_rogue_removed.append(mytaxi)
                    if self.taxis_idle.count(mytaxi) > 0:
                        self.taxis_rogue_removed_idle.append(mytaxi)
                    self.taxis_rogue_all_old.remove(mytaxi)
                    print('\nWARNING!     A ROGUE TAXI has been REMOVED from the simulation by SUMO')
                    print('             (e.g. after getting stuck due to error in network)')
                    print('             REMOVED ROGUE TAXI id: ' + str(mytaxi) + '\n')

            # Add newly detected SAVs to the list:
            self.taxis_SAV_all_new = []
            for mytaxi in self.taxis_SAV_all:
                if self.taxis_SAV_all_old.count(mytaxi) < 1:
                    self.taxis_SAV_all_old.append(mytaxi)
                    self.taxis_SAV_all_new.append(mytaxi)
            # Delete removed SAVs from the list and print a warning:
            for mytaxi in self.taxis_SAV_all_old:
                if self.taxis_SAV_all.count(mytaxi) < 1:
                    self.taxis_SAV_removed.append(mytaxi)
                    if self.taxis_idle.count(mytaxi) > 0:
                        self.taxis_SAV_removed_idle.append(mytaxi)
                    self.taxis_SAV_all_old.remove(mytaxi)
                    print('\nWARNING!     An SAV has been REMOVED from the simulation by SUMO')
                    print('             (e.g. after getting stuck due to error in network)')
                    print('             REMOVED SAV id: ' + str(mytaxi) + '\n')

            # Add newly detected private taxis to the list:
            self.taxis_private_all_new = []
            for mytaxi in self.taxis_private_all:
                if self.taxis_private_all_old.count(mytaxi) < 1:
                    self.taxis_private_all_old.append(mytaxi)
                    self.taxis_private_all_new.append(mytaxi)
            # Delete removed private taxis from the list and print a warning:
            for mytaxi in self.taxis_private_all_old:
                if self.taxis_private_all.count(mytaxi) < 1:
                    self.taxis_private_removed.append(mytaxi)
                    if self.taxis_idle.count(mytaxi) > 0:
                        self.taxis_private_removed_idle.append(mytaxi)
                    self.taxis_private_all_old.remove(mytaxi)
                    print('\nWARNING!     A PRIVATE TAXI has been REMOVED from the simulation by SUMO')
                    print('             (e.g. after getting stuck due to error in network)')
                    print('             REMOVED PRIVATE TAXI id: ' + str(mytaxi) + '\n')

            # Only print the numbers/ids of rogue taxis if there are any (usually there shouldn't be any)
            # BUT: if there are rogue taxis, ALWAYS print their IDs, so you can find and analyze them
            if len(self.taxis_rogue_removed) > 0 or len(self.taxis_rogue_all) > 0:
                print('WARNING!   ROGUE TAXIS (neither SAV nor private taxi) HAVE APPEARED')
                print('           These taxis belong to no known taxi fleet:')
                print('           neither to taxi:fleetSAV nor to taxi:fleetPrivate.')
                print('           Rogue taxis can pick up both SAV and private taxi passengers.\n')
                if self.display_number_of_taxis_SAVs == True and len(self.taxis_rogue_all) > 0:
                    print('NUMBER of CURRENT ROGUE TAXIs:                 ' + str(len(self.taxis_rogue_all)))
                    print('IDs of    CURRENT ROGUE TAXIs:                 ' + str(self.taxis_rogue_all))
                    print('NUMBER of REMOVED ROGUE TAXIs:                 ' + str(len(self.taxis_rogue_removed)))
                if self.traci_taxi_removal_info and len(self.taxis_rogue_removed) > 0:
                    print('IDs    of REMOVED ROGUE TAXIs:                 ' + str(self.taxis_rogue_removed))
                    if len(self.taxis_rogue_removed_idle) > 0:
                        print('NUMBER of ROGUE TAXIs REMOVED while IDLING:    ' + str(len(self.taxis_rogue_removed_idle)))
                        print('IDs    of ROGUE TAXIs REMOVED while IDLING:    ' + str(self.taxis_rogue_removed_idle))
                        print('Idle algorithm:                                ' + str(self.idlealgorithm))
                        print('\nThese ROGUE TAXIs have been permanently removed from this simulation by SUMO,')
                        print('e.g., removed for getting stuck while idling because of an error in network.')
                        print('In the SUMO log, you may find:')
                        print('<Warning: Vehicle vehicle_id ends idling in a cul-de-sac>')
                        print('(vehicle_id is a placeholder for an actual id, such as: 103)')
                    else:
                        print('These ROGUE TAXIs have been permanently removed from this simulation by SUMO,')
                        print('e.g., removed for getting stuck because of an error in network.')
                        print('No ROGUE TAXI was removed while idling.')
                        print('\nLast-known info from timestep just before removal of these ROGUE TAXIS from sim:')
                    for mytaxi in self.taxis_rogue_removed:
                        print('\nROGUE TAXI ID:          ' + str(mytaxi))
                        print('Final timestep:         ' + str(self.taxis_all_final_timestep[mytaxi]))
                        print('Final edge id:          ' + str(self.taxis_all_final_edge_id[mytaxi]))     
                        print('Final lane id:          ' + str(self.taxis_all_final_lane_id[mytaxi]))
                        print('FInal lane index:       ' + str(self.taxis_all_final_lane_index[mytaxi]))
                        print('Final lane position     ' + str(self.taxis_all_final_lane_position[mytaxi]))
                        print('(Final lane position = final position of vehicle along the lane measured in m)')
                if (len(self.taxis_rogue_removed) > 0 or len(self.taxis_rogue_all) > 0) \
                or (self.display_number_of_taxis_SAVs == True and len(self.taxis_rogue_all) > 0):
                    print('')
                
            # print overview of SAV numbers (current and removed) and removed ids:
            if self.display_number_of_taxis_SAVs:
                print('NUMBER of CURRENT SAVs:                        ' + str(len(self.taxis_SAV_all)))
                print('NUMBER of REMOVED SAVs:                        ' + str(len(self.taxis_SAV_removed)))
            if self.traci_taxi_removal_info and len(self.taxis_SAV_removed) > 0:
                print('IDs    of REMOVED SAVs:                        ' + str(self.taxis_SAV_removed))
                if len(self.taxis_SAV_removed_idle) > 0:
                    print('NUMBER of SAVs REMOVED while IDLING:           ' + str(len(self.taxis_SAV_removed_idle)))
                    print('IDs    of SAVs REMOVED while IDLING:           ' + str(self.taxis_SAV_removed_idle))
                    print('Idle algorithm:                                ' + str(self.idlealgorithm))
                    print('\nThese SAVs have been permanently removed from this simulation by SUMO,')
                    print('e.g., removed for getting stuck while idling because of an error in network.')
                    print('In the SUMO log, you may find:')
                    print('<Warning: Vehicle vehicle_id ends idling in a cul-de-sac>')
                    print('(vehicle_id is a placeholder for an actual id, such as: 103)')
                else:
                    print('These SAVs have been permanently removed from this simulation by SUMO,')
                    print('e.g., removed for getting stuck because of an error in network.')
                    print('No SAV was removed while idling.')
                print('\nLast-known info from timestep just before removal of these SAVs from sim:')
                for mytaxi in self.taxis_SAV_removed:
                    print('\nSAV ID:                 ' + str(mytaxi))
                    print('Final timestep:         ' + str(self.taxis_all_final_timestep[mytaxi]))
                    print('Final edge id:          ' + str(self.taxis_all_final_edge_id[mytaxi]))     
                    print('Final lane id:          ' + str(self.taxis_all_final_lane_id[mytaxi]))
                    print('FInal lane index:       ' + str(self.taxis_all_final_lane_index[mytaxi]))
                    print('Final lane position     ' + str(self.taxis_all_final_lane_position[mytaxi]))
                    print('(Final lane position = final position of vehicle along the lane measured in m)')
            if self.display_number_of_taxis_SAVs or (self.traci_taxi_removal_info and len(self.taxis_SAV_removed) > 0):
                print('')

            # print overview of private taxi numbers (current and removed) and removed ids:
            if self.display_number_of_taxis_SAVs:
                print('NUMBER of CURRENT PRIVATE TAXIS:               ' + str(len(self.taxis_private_all)))
                print('NUMBER of REMOVED PRIVATE TAXIs:               ' + str(len(self.taxis_private_removed)))
            if self.traci_taxi_removal_info and len(self.taxis_private_removed) > 0:
                print('IDs    of REMOVED PRIVATE TAXIs:               ' + str(self.taxis_private_removed))
                if len(self.taxis_private_removed_idle) > 0:
                    print('NUMBER of PRIVATE TAXIs REMOVED while IDLING:  ' + str(len(self.taxis_private_removed_idle)))
                    print('IDs    of PRIVATE TAXIs REMOVED while IDLING:  ' + str(self.taxis_private_removed_idle))
                    print('Idle algorithm:                                ' + str(self.idlealgorithm))
                    print('\nThese PRIVATE TAXIs have been permanently removed from this simulation by SUMO,')
                    print('e.g., removed for getting stuck while idling because of an error in network.')
                    print('In the SUMO log, you may find:')
                    print('<Warning: Vehicle vehicle_id ends idling in a cul-de-sac>')
                    print('(vehicle_id is a placeholder for an actual id, such as: 103)')
                else:
                    print('These PRIVATE TAXIs have been permanently removed from this simulation by SUMO,')
                    print('e.g., removed for getting stuck because of an error in network.')
                    print('No PRIVATE TAXI was removed while idling.')
                print('\nLast-known info from timestep just before removal of these PRIVATE TAXIs from sim:')
                for mytaxi in self.taxis_private_removed:
                    print('\nPRIVATE TAXI ID:        ' + str(mytaxi))
                    print('Final timestep:         ' + str(self.taxis_all_final_timestep[mytaxi]))
                    print('Final edge id:          ' + str(self.taxis_all_final_edge_id[mytaxi]))     
                    print('Final lane id:          ' + str(self.taxis_all_final_lane_id[mytaxi]))
                    print('FInal lane index:       ' + str(self.taxis_all_final_lane_index[mytaxi]))
                    print('Final lane position     ' + str(self.taxis_all_final_lane_position[mytaxi]))
                    print('(Final lane position = final position of vehicle along the lane measured in m)')
            if self.display_number_of_taxis_SAVs or (self.traci_taxi_removal_info and len(self.taxis_private_removed) > 0):
                print('\n')

            # update variables:
            self.taxis_idle = traci.vehicle.getTaxiFleet(0)
            self.taxis_SAV_idle = []
            self.taxis_private_idle = []
            for mytaxi in self.taxis_idle:
                if traci.vehicle.getLine(mytaxi) == "taxi:fleetSAV":
                    self.taxis_SAV_idle.append(mytaxi)
                elif traci.vehicle.getLine(mytaxi) == "taxi:fleetPrivate":
                    self.taxis_private_idle.append(mytaxi)

            self.taxis_picking_up = traci.vehicle.getTaxiFleet(1)
            self.taxis_SAV_picking_up = []
            self.taxis_private_picking_up = []
            for mytaxi in self.taxis_picking_up:
                if traci.vehicle.getLine(mytaxi) == "taxi:fleetSAV":
                    self.taxis_SAV_picking_up.append(mytaxi)
                elif traci.vehicle.getLine(mytaxi) == "taxi:fleetPrivate":
                    self.taxis_private_picking_up.append(mytaxi)

            self.taxis_occupied = traci.vehicle.getTaxiFleet(2)
            self.taxis_SAV_occupied = []
            self.taxis_private_occupied = []
            for mytaxi in self.taxis_occupied:
                if traci.vehicle.getLine(mytaxi) == "taxi:fleetSAV":
                    self.taxis_SAV_occupied.append(mytaxi)
                elif traci.vehicle.getLine(mytaxi) == "taxi:fleetPrivate":
                    self.taxis_private_occupied.append(mytaxi)

            self.taxis_pick_and_occ = traci.vehicle.getTaxiFleet(3)
            self.taxis_SAV_pick_and_occ = []
            self.taxis_private_pick_and_occ = []
            for mytaxi in self.taxis_pick_and_occ:
                if traci.vehicle.getLine(mytaxi) == "taxi:fleetSAV":
                    self.taxis_SAV_pick_and_occ.append(mytaxi)
                elif traci.vehicle.getLine(mytaxi) == "taxi:fleetPrivate":
                    self.taxis_private_pick_and_occ.append(mytaxi)


        # Update the dictionary of BROKEN-down SAVs in case any were deleted.
        # (e.g. if it was removed by Sumo automatically after it ended idling in a cul-de-sac)
        def update_taxis_SAV_broken(self):
            
            self.SAVs_removed_from_sim = []
            for SAV in self.taxis_SAV_broken:
                if self.taxis_SAV_all.count(SAV) < 1:
                    self.SAVs_removed_from_sim.append(SAV)
            # Looping a second time to avoid deleting items in the list during the above iterating.
            # (changing list-size during iteration leads to runtime errors)
            for SAV in self.SAVs_removed_from_sim:
                if list(self.taxis_SAV_broken.keys()).count(SAV) > 0:
                    del self.taxis_SAV_broken[SAV]


        # Go through the list and the broken-down SAVs that have fulfilled/exceeded their breakdown time, and return speed to normal
        def update_taxis_SAV_no_longer_broken(self):
            
            self.taxis_SAV_no_longer_broken = []
            for SAV in self.taxis_SAV_broken:
                if (self.simtime - self.taxis_SAV_broken[SAV] - self.SAV_breakdown_period) > 0:
                    # If slow-down period has not yet passed, but if breakdown simulation active:
                    # --> Slow the recently un-broken ones down and add them to the slowed-down list.
                    if self.fleet_slowed_down_flag:
                        traci.vehicle.setSpeed(SAV,self.SAV_slowdown_speed)
                    else:
                        traci.vehicle.setSpeed(SAV,-1)
                    self.taxis_SAV_no_longer_broken.append(SAV)
                    self.taxis_SAV_not_broken.append(SAV)
            # Looping a second time to avoid deleting items in list during the above iteratiion.
            # (changing list-size during iteration leads to runtime errors)
            for SAV in self.taxis_SAV_no_longer_broken:
                del self.taxis_SAV_broken[SAV]


        # Update the list of NOT-broken-down SAVs (in case any were deleted/added by SUMO)
        def update_taxis_SAV_not_broken(self):
            
            self.taxis_SAV_not_broken = []
            for SAV in self.taxis_SAV_all:
                if list(self.taxis_SAV_broken.keys()).count(SAV) < 1:
                    self.taxis_SAV_not_broken.append(SAV)


        # interprets return value cases from validate_startendtimes()
        def validate_startendtimes_case_interpreter(self, breakdown_or_slowdown, starttime_case, endtime_case):
            
            # if the breakdown simulator is using this function
            if breakdown_or_slowdown == 'breakdown':
                
                # if starttime_valid == True
                if starttime_case == 0 or starttime_case == 1:
                    if starttime_case == 0:
                        print('SAV-BREAKDOWN-Simulator: WARNING! Given start time for breakdown\n'\
                            + '                         simulator ({:.3f} s) lower than\n'.format(self.SAV_breakdown_starttime) \
                            + '                         or equal to start time SUMO simulation. Therefore:\n'\
                            + '                         First breakdown planned to start \n' \
                            + '                         immediately at first call of breakdown simulator.')
                    elif starttime_case == 1:
                        pass
                
                # elif starttime_valid == False
                elif starttime_case == 2 or starttime_case == 3:

                    if starttime_case == 2:
                        print('SAV-BREAKDOWN-Simulator: Given start time for breakdown simulator (default):          -1.000')
                    elif starttime_case == 3:
                        print('SAV-BREAKDOWN-Simulator: WARNING! Given start time for\n'\
                            + '                         breakdown simulator ({:.3f} s)\n'.format(self.SAV_breakdown_starttime) \
                            + '                         larger than given end time ({:.3f} s)'.format(self.SAV_breakdown_endtime))
                    print('                         Default planned start time of first breakdown used:\n'\
                        + '                         1 breakdown interval ({:.3f} s)\n'.format(self.SAV_breakdown_interval) \
                        + '                         after first call of breakdown simulator.')

                else:
                    print('\nTaxiService.validate_startendtimes_case_interpreter: Error! Unexpected starttime_case\n'\
                        + '                                                       given as input!\n')
                
                # if endtime_valid == True
                if endtime_case == 1:
                    if starttime_case == 1:
                        print('SAV-BREAKDOWN-Simulator: Planned start time of first breakdown:                 {:12.3f}'.format(self.simtime_firstenter_breakdown_planned))
                    # print newline to visually separate normal data from Warnings
                    else:
                        print('\nSAV-BREAKDOWN-Simulator: Planned start time of first breakdown:                 {:12.3f}'.format(self.simtime_firstenter_breakdown_planned))
                    print('SAV-BREAKDOWN-Simulator: Planned end time of breakdown simulator:               {:12.3f}'.format(self.SAV_breakdown_endtime))

                # if endtime_valid == False
                elif endtime_case == 2 or endtime_case == 3:
                    if endtime_case == 2:
                        print('SAV-BREAKDOWN-Simulator: Given end time for breakdown simulator (default):            -1.000')
                    elif endtime_case == 3:
                        print('SAV-BREAKDOWN-Simulator: WARNING! Given end time for breakdown simulator\n'\
                            + '                         simulator ({:.3f} s) lower than\n'.format(self.SAV_breakdown_endtime) \
                            + '                         or equal to start time SUMO simulation. Therefore:\n'\
                            + '                         Given end time ignored.')
                    print('                         Using default behavior:\n' \
                        + '                         Breakdown simulations continue until end of SUMO sim.')
                    print('\nSAV-BREAKDOWN-Simulator: Planned start time of first breakdown:                 {:12.3f}'.format(self.simtime_firstenter_breakdown_planned))
                    print('SAV-BREAKDOWN-Simulator: Planned end time of slowdown simulator:                        None')

                else:
                    print('\nTaxiService.validate_startendtimes_case_interpreter: Error! Unexpected endtime_case\n'\
                        + '                                                       given as input!\n')
            
            # if the slowdown simulator is using this function
            elif breakdown_or_slowdown == 'slowdown':
                
                # if starttime_valid == True
                if starttime_case == 0 or starttime_case == 1:
                    if starttime_case == 0:
                        print('SAV-SLOWDOWN-Simulator: WARNING! Given start time for slowdown\n'\
                            + '                        simulator ({:.3f} s) lower than\n'.format(self.SAV_slowdown_starttime) \
                            + '                        or equal to start time SUMO simulation. Therefore:\n'\
                            + '                        First breakdown planned to start \n' \
                            + '                        immediately at first call of slowdown simulator.')
                    elif starttime_case == 1:
                        pass
                
                # if starttime_valid == False
                elif starttime_case == 2 or starttime_case == 3:
                    if starttime_case == 2:
                        print('SAV-SLOWDOWN-Simulator: Given start time for slowdown simulator (default):            -1.000')           
                    elif starttime_case == 3:
                        print('SAV-SLOWDOWN-Simulator: WARNING! Given start time for\n'\
                            + '                        slowdown simulator ({:.3f} s)\n'.format(self.SAV_slowdown_starttime) \
                            + '                        larger than given end time ({:.3f} s)'.format(self.SAV_slowdown_endtime))
                    print('                        Default planned start time of first slowdown used:\n'\
                        + '                        1 slowdown interval ({:.3f} s)\n'.format(self.SAV_slowdown_interval) \
                        + '                        after first call of slowdown simulator.')

                else:
                    print('\nTaxiService.validate_startendtimes_case_interpreter: Error! Unexpected starttime_case\n'\
                        + '                                                       given as input!\n')
                
                # if endtime_valid == True
                if endtime_case == 1:
                    if starttime_case == 1:
                        print('SAV-SLOWDOWN-Simulator: Planned start time of first slowdown:                   {:12.3f}'.format(self.simtime_firstenter_slowdown_planned))
                                        # print newline to visually separate normal data from Warnings
                    else:
                        print('\nSAV-SLOWDOWN-Simulator: Planned start time of first slowdown:                   {:12.3f}'.format(self.simtime_firstenter_slowdown_planned))
                    print('SAV-SLOWDOWN-Simulator: Planned end time of slowdown simulator:                 {:12.3f}'.format(self.SAV_slowdown_endtime))
                
                # if endtime_valid == False
                elif endtime_case == 2 or endtime_case == 3:
                    if endtime_case == 2:
                        print('SAV-SLOWDOWN-Simulator: Given end time for slowdown simulator (default):              -1.000')
                    elif endtime_case == 3:
                        print('SAV-SLOWDOWN-Simulator: WARNING! Given end time for slowdown simulator\n'\
                            + '                        simulator ({:.3f} s) lower than\n'.format(self.SAV_slowdown_endtime) \
                            + '                        or equal to start time SUMO simulation. Therefore:\n'\
                            + '                        Given end time ignored.')
                    print('                        Using default behavior:\n' \
                        + '                        Slowdown simulations continue until end of SUMO sim.')
                    print('\nSAV-SLOWDOWN-Simulator: Planned start time of first slowdown:                   {:12.3f}'.format(self.simtime_firstenter_slowdown_planned))
                    print('SAV-SLOWDOWN-Simulator: Planned end time of slowdown simulator:                         None')

                else:
                    print('\nTaxiService.validate_startendtimes_case_interpreter: Error! Unexpected endtime_case\n'\
                        + '                                                       given as input!\n')

            else:
                print('\nTaxiService.validate_startendtimes_case_interpreter: Error! Unexpected value passed as\n'\
                    + '                                                       breakdown_or_slowdown parameter!\n')

        # see validate_startendtimes_case_interpreter() for meaning of different cases
        def validate_startendtimes(self, starttime, endtime):
        # Check if the given start time and end time is valid
            
            starttime_case = None
            endtime_case = None
            starttime_valid = None
            endtime_valid = None

            # validate end time
            if self.simtime < endtime:
                endtime_case = 1
            elif endtime == -1.0:
                endtime_case = 2
            else:
                endtime_case = 3

            # validate start time
            if starttime == -1.0:
                starttime_case = 2
            else:
                if endtime_case == 1 and endtime <= starttime:
                    starttime_case = 3
                else:
                    if self.simtime >= starttime:
                        starttime_case = 0
                    else:
                        starttime_case = 1

            if starttime_case == 0 or starttime_case == 1:
                starttime_valid = True
            elif starttime_case == 2 or starttime_case == 3:
                starttime_valid = False
            else:
                print('\nTaxiService.validate_startendtimes: Error! Unexpected endtime_case assigned!\n'\
                    + '                                      starttime neither valid nor invalid!\n')

            if endtime_case == 1:
                endtime_valid = True
            elif endtime_case == 2 or endtime_case == 3:
                endtime_valid = False
            else:
                print('\nTaxiService.validate_startendtimes: Error! Unexpected endtime_case assigned!\n'\
                    + '                                      starttime neither valid nor invalid!\n')
            
            return starttime_case, endtime_case, starttime_valid, endtime_valid


        # Write taxi configuration to sumo configuration file feed.
        def write_config(self, fd, ident = 0):
            
            print('TaxiService.write_config')
            attrsman = self.get_attrsman() 
            fd.write(ident*' '+'<%s value="%s"/>\n'%(attrsman.dispatchalgorithm.tag,str(attrsman.dispatchalgorithm.get_value())))
            fd.write(ident*' '+'<%s value="%s"/>\n'%(attrsman.idlealgorithm.tag,str(attrsman.idlealgorithm.get_value())))
