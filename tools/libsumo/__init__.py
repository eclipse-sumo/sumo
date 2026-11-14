# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2018-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    __init__.py
# @author  Benjamin Striner
# @author  Michael Behrisch
# @date    2018-06-05

import importlib.machinery
import os
import sys
import warnings

SUMO_DATA_HOME = None
try:
    import sumo_data
    SUMO_DATA_HOME = sumo_data.__path__[0]
    if not os.environ.get("SUMO_HOME"):
        os.environ["SUMO_HOME"] = SUMO_DATA_HOME
    if not os.environ.get("PROJ_LIB") and not os.environ.get("PROJ_DATA"):
        os.environ["PROJ_LIB"] = os.environ["PROJ_DATA"] = os.path.join(SUMO_DATA_HOME, "data", "proj")
except ImportError:
    # fall back to the eclipse-sumo wheel
    try:
        import sumo
        if not hasattr(sumo, "SUMO_HOME"):  # maybe it is not the correct module
            raise ImportError()
        SUMO_DATA_HOME = sumo.SUMO_HOME
        if not os.environ.get("SUMO_HOME"):
            os.environ["SUMO_HOME"] = SUMO_DATA_HOME
    except ImportError:
        if not os.environ.get("SUMO_HOME"):
            warnings.warn("SUMO_HOME is not set and neither sumo-data nor the eclipse-sumo wheel are installed!")

if hasattr(sys, "_is_gil_enabled") and not sys._is_gil_enabled():
    warnings.warn("This package has not been validated in free-threaded Python (no-GIL mode).")
if hasattr(os, "add_dll_directory"):
    # since Python 3.8 the DLL search path has to be set explicitly see https://bugs.python.org/issue43173
    _local_bin = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "bin"))
    if SUMO_DATA_HOME and os.path.exists(os.path.join(SUMO_DATA_HOME, "bin", "z.dll")):
        os.add_dll_directory(os.path.abspath(os.path.join(SUMO_DATA_HOME, "bin")))
    elif "SUMO_HOME" in os.environ and os.path.exists(os.path.join(os.environ["SUMO_HOME"], "bin", "z.dll")):
        os.add_dll_directory(os.path.abspath(os.path.join(os.environ["SUMO_HOME"], "bin")))
    elif os.path.exists(os.path.join(_local_bin, "z.dll")):
        os.add_dll_directory(_local_bin)

try:
    # this tries to determine the version number of an installed wheel
    import importlib.metadata  # noqa
    __version__ = importlib.metadata.version(__name__)
except ImportError:
    # this is the fallback version, it gets replaced with the current version on "make install" or "make dist"
    __version__ = "0.0.0"

_delegated_to_installed = False
try:
    from .libsumo import vehicle, simulation, person, trafficlight, edge  # noqa
    from .libsumo import TraCIStage, TraCINextStopData, TraCIReservation, TraCILogic, TraCIPhase, TraCIException  # noqa
    from .libsumo import TraCICollision, TraCISignalConstraint  # noqa
    from ._libsumo import TraCILogic_phases_get, TraCILogic_phases_set, TraCILogic_swiginit, new_TraCILogic  # noqa
    from .libsumo import *  # noqa
except ImportError:
    # When SUMO_HOME/tools is prepended to sys.path, an unbuilt tools/libsumo checkout
    # can shadow a wheel installed in site-packages (#18523). Fall back to remaining sys.path entries.
    _this_tools_dir = os.path.normcase(os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")))
    _search_paths = [
        p for p in sys.path
        if os.path.normcase(os.path.abspath(p or os.getcwd())) != _this_tools_dir
    ]
    _spec = importlib.machinery.PathFinder.find_spec(__name__, _search_paths)
    if _spec is None or _spec.loader is None or _spec.submodule_search_locations is None:
        raise
    _mod = sys.modules[__name__]
    sys.modules.pop(__name__ + ".libsumo", None)
    sys.modules.pop(__name__ + "._libsumo", None)
    _mod.__file__ = _spec.origin
    _mod.__loader__ = _spec.loader
    _mod.__package__ = __name__
    _mod.__path__ = list(_spec.submodule_search_locations)
    _mod.__spec__ = _spec
    _spec.loader.exec_module(_mod)
    _delegated_to_installed = True

if not _delegated_to_installed:
    from traci import connection, constants, exceptions, _vehicle, _person, _trafficlight, _simulation, _gui  # noqa
    from traci.step import StepManager, StepListener  # noqa

    if os.environ.get('LIBSUMO_AS_TRACI') != "quiet":
        try:
            import importlib.metadata  # noqa
            pyarrow_version = importlib.metadata.version("pyarrow")
            pyarrow_so_version = "%s%02i" % tuple(map(int, pyarrow_version.split(".")[:2]))
            sumo_arrow_so_version = simulation.getParameter("", "buildConfig.ARROW_SO_VERSION")
            if sumo_arrow_so_version and pyarrow_so_version != sumo_arrow_so_version:
                print("Warning! pyarrow is installed with version %s which might be incompatible with libsumo "
                      "which is compiled against libarrow%s." % (pyarrow_version, sumo_arrow_so_version))
                print(" Try to uninstall pyarrow or install a matching pyarrow version, if you encounter problems.")
        except (ImportError, ValueError, TypeError):
            # either pyarrow is not installed (fine) or importlib is not available (not great, but we cannot do much)
            # the ValueError / TypeError occur if we could not parse the pyarrow version
            pass

    DOMAINS = [
        busstop,  # noqa
        calibrator,  # noqa
        chargingstation,  # noqa
        edge,
        gui,  # noqa
        inductionloop,  # noqa
        junction,  # noqa
        lane,  # noqa
        lanearea,  # noqa
        meandata,  # noqa
        multientryexit,  # noqa
        overheadwire,  # noqa
        parkingarea,  # noqa
        person,
        poi,  # noqa
        polygon,  # noqa
        rerouter,  # noqa
        route,  # noqa
        routeprobe,  # noqa
        simulation,
        trafficlight,
        variablespeedsign,  # noqa
        vehicle,
        vehicletype,  # noqa
    ]

    _stepManager = StepManager()

    def wrapAsClassMethod(func, module):
        def wrapper(*args, **kwargs):
            return func(module, *args, **kwargs)
        wrapper.__name__ = func.__name__
        wrapper.__doc__ = func.__doc__
        return wrapper

    TraCIStage.__attr_repr__ = _simulation.Stage.__attr_repr__
    TraCIStage.__repr__ = _simulation.Stage.__repr__
    TraCIStage.toXML = _simulation.Stage.toXML
    TraCICollision.__attr_repr__ = _simulation.Collision.__attr_repr__
    TraCICollision.__repr__ = _simulation.Collision.__repr__

    TraCINextStopData.__attr_repr__ = _vehicle.StopData.__attr_repr__
    TraCINextStopData.__repr__ = _vehicle.StopData.__repr__

    TraCIReservation.__attr_repr__ = _person.Reservation.__attr_repr__
    TraCIReservation.__repr__ = _person.Reservation.__repr__

    def _to_traci_phases(phases):
        return [TraCIPhase(p.duration, p.state, p.minDur, p.maxDur,
                           () if p.next is None else p.next, p.name) for p in phases]

    def set_phases(self, phases):
        TraCILogic_phases_set(self, _to_traci_phases([] if phases is None else phases))

    def TraCILogic__init__(self, *args, **kwargs):
        programID = kwargs.get('programID', args[0] if len(args) > 0 else None)
        type_ = kwargs.get('type', args[1] if len(args) > 1 else None)
        currentPhaseIndex = kwargs.get('currentPhaseIndex', args[2] if len(args) > 2 else None)
        phases = kwargs.get('phases', args[3] if len(args) > 3 else None)
        subParameter = kwargs.get('subParameter', args[4] if len(args) > 4 else None)

        phases = _to_traci_phases([] if phases is None else phases)
        args = (programID, type_, currentPhaseIndex, phases)
        TraCILogic_swiginit(self, new_TraCILogic(*args))
        if subParameter is not None:
            self.subParameter = subParameter

    # Override methods and properties
    TraCILogic.__init__ = TraCILogic__init__
    TraCILogic.phases = property(TraCILogic_phases_get, set_phases)
    TraCILogic.getPhases = _trafficlight.Logic.getPhases
    TraCILogic.__repr__ = _trafficlight.Logic.__repr__
    TraCIPhase.__repr__ = _trafficlight.Phase.__repr__
    TraCISignalConstraint.__repr__ = _trafficlight.Constraint.__repr__

    exceptions.TraCIException = TraCIException
    simulation.Stage = TraCIStage
    vehicle.StopData = TraCINextStopData
    person.Reservation = TraCIReservation
    trafficlight.Phase = TraCIPhase
    trafficlight.Logic = TraCILogic
    vehicle.addFull = vehicle.add
    vehicle.addLegacy = wrapAsClassMethod(_vehicle.VehicleDomain.addLegacy, vehicle)
    vehicle.couldChangeLane = wrapAsClassMethod(_vehicle.VehicleDomain.couldChangeLane, vehicle)
    vehicle.wantsAndCouldChangeLane = wrapAsClassMethod(_vehicle.VehicleDomain.wantsAndCouldChangeLane, vehicle)
    vehicle.isStopped = wrapAsClassMethod(_vehicle.VehicleDomain.isStopped, vehicle)
    vehicle.isStoppedParking = wrapAsClassMethod(_vehicle.VehicleDomain.isStoppedParking, vehicle)
    vehicle.isStoppedTriggered = wrapAsClassMethod(_vehicle.VehicleDomain.isStoppedTriggered, vehicle)
    vehicle.isAtBusStop = wrapAsClassMethod(_vehicle.VehicleDomain.isAtBusStop, vehicle)
    vehicle.isAtContainerStop = wrapAsClassMethod(_vehicle.VehicleDomain.isAtContainerStop, vehicle)
    vehicle.setBusStop = wrapAsClassMethod(_vehicle.VehicleDomain.setBusStop, vehicle)
    vehicle.setContainerStop = wrapAsClassMethod(_vehicle.VehicleDomain.setContainerStop, vehicle)
    vehicle.setParkingAreaStop = wrapAsClassMethod(_vehicle.VehicleDomain.setParkingAreaStop, vehicle)
    vehicle.setChargingStationStop = wrapAsClassMethod(_vehicle.VehicleDomain.setChargingStationStop, vehicle)
    vehicle.getRightFollowers = wrapAsClassMethod(_vehicle.VehicleDomain.getRightFollowers, vehicle)
    vehicle.getRightLeaders = wrapAsClassMethod(_vehicle.VehicleDomain.getRightLeaders, vehicle)
    vehicle.getLeftFollowers = wrapAsClassMethod(_vehicle.VehicleDomain.getLeftFollowers, vehicle)
    vehicle.getLeftLeaders = wrapAsClassMethod(_vehicle.VehicleDomain.getLeftLeaders, vehicle)
    vehicle.getLaneChangeStatePretty = wrapAsClassMethod(_vehicle.VehicleDomain.getLaneChangeStatePretty, vehicle)
    vehicle._legacyGetLeader = True
    person.removeStages = wrapAsClassMethod(_person.PersonDomain.removeStages, person)
    _trafficlight.TraCIException = TraCIException
    trafficlight.setLinkState = wrapAsClassMethod(_trafficlight.TrafficLightDomain.setLinkState, trafficlight)
    trafficlight.getNemaPhaseCalls = wrapAsClassMethod(_trafficlight.TrafficLightDomain.getNemaPhaseCalls, trafficlight)
    trafficlight.setNemaSplits = wrapAsClassMethod(_trafficlight.TrafficLightDomain.setNemaSplits, trafficlight)
    trafficlight.setNemaMaxGreens = wrapAsClassMethod(_trafficlight.TrafficLightDomain.setNemaMaxGreens, trafficlight)
    trafficlight.setNemaCycleLength = wrapAsClassMethod(
        _trafficlight.TrafficLightDomain.setNemaCycleLength, trafficlight)
    trafficlight.setNemaOffset = wrapAsClassMethod(_trafficlight.TrafficLightDomain.setNemaOffset, trafficlight)

    def addStepListener(listener):
        return _stepManager.addStepListener(listener)

    def removeStepListener(listenerID):
        return _stepManager.removeStepListener(listenerID)

    def isLibsumo():
        return True

    def isLibtraci():
        return False

    def init(port):
        print("Warning! To make your code usable with traci and libsumo, please use traci.start instead of traci.init.")

    hasGUI = simulation.hasGUI
    load = simulation.load
    isLoaded = simulation.isLoaded
    getVersion = simulation.getVersion
    executeMove = simulation.executeMove

    _libsumo_step = simulation.step

    def simulationStep(time=0.):
        _libsumo_step(time)
        _stepManager.manageStepListeners(time)

    simulation.step = simulationStep

    def close(wait=True):
        for listenerID in list(_stepManager._stepListeners.keys()):
            _stepManager.removeStepListener(listenerID)
        simulation.close()
        _stepManager.close()

    def start(cmd, port=None, numRetries=constants.DEFAULT_NUM_RETRIES, label="default", verbose=False,
              traceFile=None, traceGetters=True, stdout=None, doSwitch=True):
        if port is not None:
            print("Warning! To make your code usable with traci and libsumo, do not set an explicit port.")
        version = simulation.start(cmd)
        if traceFile is not None:
            if _stepManager.startTracing(traceFile, traceGetters, DOMAINS):
                # simulationStep shows up as simulation.step
                global _libsumo_step
                _libsumo_step = _stepManager._addTracing(_libsumo_step, "simulation")
            _stepManager._traceFile.write("import traci\n")
            _stepManager.write("start", repr(cmd))
        return version

    def setLegacyGetLeader(enabled):
        vehicle._legacyGetLeader = enabled

    gui.DEFAULT_VIEW = _gui.GuiDomain.DEFAULT_VIEW  # noqa
