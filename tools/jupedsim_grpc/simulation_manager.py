import os
import sys
import threading

import jupedsim as jps
import shapely
from google.protobuf import json_format

sys.path.insert(0, os.path.dirname(__file__))

import jupedsim_pb2


class SimulationManager:
    def __init__(self):
        self._geometry_id_counter = 0
        self._simulation_id_counter = 0
        self._geometries = {}  # geometry_id -> shapely.Polygon
        self._simulations = {}  # sim_id -> jps.Simulation
        self._agents = {}  # agent_id -> simulation_id
        self._stages = {}  # stage_id (waypoint or exit) -> simulation_id
        self._journeys = {}  # journey_id -> simulation_id
        self._lock = threading.Lock()

    def generate_geometry_id(self):
        with self._lock:
            self._geometry_id_counter += 1
            return self._geometry_id_counter

    def generate_simulation_id(self):
        with self._lock:
            self._simulation_id_counter += 1
            return self._simulation_id_counter

    def create_geometry(self, boundary, obstacles):
        if len(boundary) < 3:
            return None, "boundary needs at least 3 points"
        for obstacle in obstacles:
            if len(obstacle) < 3:
                return None, "each obstacle needs at least 3 points"
        geometry_id = self.generate_geometry_id()
        self._geometries[geometry_id] = shapely.Polygon(
            boundary, holes=obstacles
        )
        return jupedsim_pb2.CreateGeometryResponse(
            geometry_id=geometry_id
        ), None

    def switch_geometry(self, simulation_id, geometry_id):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        if (geometry := self._geometries.get(geometry_id, None)) is None:
            return None, f"geometry ID {geometry_id} unknown"
        try:
            simulation.switch_geometry(geometry)
        except RuntimeError as e:
            return None, str(e)
        return jupedsim_pb2.EmptyResponse(), None

    def create_simulation(self, geometry_id, model_class, delta_t):
        if delta_t <= 0:
            return None, "delta_t must be positive"
        if (geometry := self._geometries.get(geometry_id, None)) is None:
            return None, f"geometry ID {geometry_id} unknown"
        if not model_class:
            return None, "model_class must not be empty"
        if (jps_model_cls := getattr(jps, model_class, None)) is None:
            return None, f"model {model_class} does not exist in JuPedSim"
        simulation_id = self.generate_simulation_id()
        self._simulations[simulation_id] = jps.Simulation(
            model=jps_model_cls(),
            geometry=geometry,
            dt=delta_t,
        )
        return jupedsim_pb2.CreateSimulationResponse(
            simulation_id=simulation_id
        ), None

    def create_agent(self, simulation_id, agent_model_class, parameters):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        if not agent_model_class:
            return None, "agent_model_class must not be empty"
        if not hasattr(jps, agent_model_class):
            return None, f"model {agent_model_class} does not exist in JuPedSim"
        param_dict = {}
        if parameters is not None:
            param_dict = json_format.MessageToDict(parameters)

        # Convert float values back to int where needed for native constructors
        for key in ("journey_id", "stage_id"):
            if key in param_dict and isinstance(param_dict[key], float):
                param_dict[key] = int(param_dict[key])

        # Create model parameters
        jps_model_param_cls = getattr(
            jps, f"{agent_model_class}AgentParameters"
        )
        agent_params = jps_model_param_cls(**param_dict)
        agent_id = simulation.add_agent(agent_params)
        self._agents[agent_id] = simulation_id
        return jupedsim_pb2.CreateAgentResponse(agent_id=agent_id), None

    def remove_agents(self, simulation_id, agent_ids):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        # mark_agent_for_removal defers actual removal to the next iterate.
        for agent_id in agent_ids:
            try:
                simulation.mark_agent_for_removal(agent_id)
            except RuntimeError as e:
                return None, str(e)
        return jupedsim_pb2.EmptyResponse(), None

    def switch_agent_journey(
        self, simulation_id, agent_id, journey_id, stage_id
    ):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        try:
            simulation.switch_agent_journey(agent_id, journey_id, stage_id)
        except RuntimeError as e:
            return None, str(e)
        return jupedsim_pb2.EmptyResponse(), None

    def get_agents_in_region(self, simulation_id, region):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        if len(region) < 3:
            return None, "region needs at least 3 points"
        ring = list(region)
        if ring[0] != ring[-1]:  # agents_in_polygon wants a closed ring
            ring.append(ring[0])
        agent_ids = simulation.agents_in_polygon(ring)
        return jupedsim_pb2.AgentIdsResponse(agent_ids=agent_ids), None

    def get_desired_speed_of_agents(self, simulation_id, agent_ids):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        speeds = {}
        try:
            for agent_id in agent_ids:
                speeds[agent_id] = simulation.agent(agent_id).model.desired_speed
        except (RuntimeError, AttributeError) as e:
            return None, str(e)
        return jupedsim_pb2.DesiredSpeedResponse(desired_speeds=speeds), None

    def set_desired_speed_of_agents(self, simulation_id, desired_speeds):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        try:
            for agent_id, speed in desired_speeds.items():
                simulation.agent(agent_id).model.desired_speed = speed
        except (RuntimeError, AttributeError) as e:
            return None, str(e)
        return jupedsim_pb2.EmptyResponse(), None

    def get_next_geometry_id(self):
        """Expose for testing."""
        return self._geometry_id_counter

    def get_next_simulation_id(self):
        """Expose for testing."""
        return self._simulation_id_counter

    def add_journey(self, simulation_id, stage_ids):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        journey = jps.JourneyDescription(stage_ids=stage_ids)
        for i in range(1, len(stage_ids)):
            journey.set_transition_for_stage(stage_ids[i-1], jps.Transition.create_fixed_transition(stage_ids[i]))
            print("setting fixed transition", stage_ids[i-1], stage_ids[i])
        journey_id = simulation.add_journey(journey)
        self._journeys[journey_id] = simulation_id
        return jupedsim_pb2.AddJourneyResponse(journey_id=journey_id), None

    def add_waypoint_stage(self, simulation_id, x, y, distance):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        waypoint_id = simulation.add_waypoint_stage((x, y), distance)
        print("adding new waypoint", waypoint_id, x, y)
        self._stages[waypoint_id] = simulation_id
        return jupedsim_pb2.AddWaypointStageResponse(
            waypoint_id=waypoint_id
        ), None

    def add_exit_stage(self, simulation_id, polygon):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        if len(polygon) < 3:
            return None, "exit polygon needs at least 3 points"
        stage_id = simulation.add_exit_stage(polygon)
        self._stages[stage_id] = simulation_id
        return jupedsim_pb2.AddExitStageResponse(stage_id=stage_id), None

    def add_waiting_set_stage(self, simulation_id, points):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        if len(points) < 1:
            return None, "waiting set needs at least 1 point"
        stage_id = simulation.add_waiting_set_stage(points)
        print("added waiting set", stage_id)
        self._stages[stage_id] = simulation_id
        return jupedsim_pb2.AddWaitingSetStageResponse(stage_id=stage_id), None

    def set_waiting_set_state(self, simulation_id, stage_id, state):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        jps_state = (
            jps.WaitingSetState.ACTIVE
            if state == jupedsim_pb2.ACTIVE
            else jps.WaitingSetState.INACTIVE
        )
        try:
            simulation.get_stage(stage_id).state = jps_state
        except (RuntimeError, AttributeError) as e:
            return None, str(e)
        return jupedsim_pb2.EmptyResponse(), None

    def get_waiting_set_state(self, simulation_id, stage_id):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        try:
            proxy = simulation.get_stage(stage_id)
            proto_state = (
                jupedsim_pb2.ACTIVE
                if proxy.state == jps.WaitingSetState.ACTIVE
                else jupedsim_pb2.INACTIVE
            )
            return jupedsim_pb2.GetWaitingSetStateResponse(
                state=proto_state,
                count_waiting=proxy.count_waiting(),
                count_targeting=proxy.count_targeting(),
            ), None
        except (RuntimeError, AttributeError) as e:
            return None, str(e)

    def get_core_properties_of_all_agents(self, simulation_id):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        properties = {}
        for agent in simulation.agents():
            properties[agent.id] = jupedsim_pb2.AgentCoreProperties(
                position=jupedsim_pb2.Point(
                    x=agent.position[0], y=agent.position[1]
                ),
                orientation=jupedsim_pb2.Point(
                    x=agent.orientation[0], y=agent.orientation[1]
                ),
            )
        return jupedsim_pb2.GetCorePropertiesOfAllAgentsResponse(
            properties=properties
        ), None

    def iterate(self, simulation_id, count=1):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        if count < 1:
            count = 1
        for _ in range(count):
            simulation.iterate()
        return jupedsim_pb2.IterateResponse(), None

    def get_agent_count(self, simulation_id):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        return jupedsim_pb2.CountResponse(
            count=simulation.agent_count()
        ), None

    def get_iteration_count(self, simulation_id):
        if (simulation := self._simulations.get(simulation_id, None)) is None:
            return None, f"simulation ID {simulation_id} unknown"
        return jupedsim_pb2.CountResponse(
            count=simulation.iteration_count()
        ), None
