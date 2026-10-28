#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    servicer.py
# @author  Ralf Leibold
# @author  Michael Behrisch
# @date    2026-06-22

from simulation_manager import SimulationManager
import jupedsim_pb2_grpc
import jupedsim_pb2
import concurrent.futures
import os
import sys

import grpc

sys.path.insert(0, os.path.dirname(__file__))


class JuPedSimServiceServicer(jupedsim_pb2_grpc.JuPedSimServiceServicer):
    def __init__(self, debug=False, sqlite=False):
        self._manager = SimulationManager(debug=debug, sqlite=sqlite)

    @staticmethod
    def _handleResponse(response_class, context, response_content, error):
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return response_class()
        return response_class(**response_content)

    def CreateGeometry(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.CreateGeometryResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        boundary = [(p.x, p.y) for p in request.boundary.points]
        obstacles = [
            [(p.x, p.y) for p in obstacle.points]
            for obstacle in request.obstacles
        ]
        response, error = self._manager.create_geometry(boundary, obstacles)
        return self._handleResponse(jupedsim_pb2.CreateGeometryResponse, context, response, error)

    def SwitchGeometry(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.switch_geometry(
            request.simulation_id,
            request.geometry_id,
        )
        return self._handleResponse(jupedsim_pb2.EmptyResponse, context, response, error)

    def CreateSimulation(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.CreateSimulationResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.create_simulation(
            request.geometry_id,
            request.model_class,
            request.delta_t,
            request.model_parameters,
        )
        return self._handleResponse(jupedsim_pb2.CreateSimulationResponse, context, response, error)

    def GetModelClasses(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.GetModelClassesResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_model_classes()
        return self._handleResponse(jupedsim_pb2.GetModelClassesResponse, context, response, error)

    def CreateAgent(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.CreateAgentResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.create_agent(
            request.simulation_id,
            request.agent_model_class,
            request.agent_model_parameters,
        )
        return self._handleResponse(jupedsim_pb2.CreateAgentResponse, context, response, error)

    def RemoveAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.remove_agents(
            request.simulation_id,
            request.agent_ids,
        )
        return self._handleResponse(jupedsim_pb2.EmptyResponse, context, response, error)

    def SwitchAgentJourney(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.switch_agent_journey(
            request.simulation_id,
            request.agent_id,
            request.journey_id,
            request.stage_id,
        )
        return self._handleResponse(jupedsim_pb2.EmptyResponse, context, response, error)

    def GetAgentsInRegion(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.AgentIdsResponse:
        """gRPC RPC method."""
        region = [(p.x, p.y) for p in request.region.points]
        response, error = self._manager.get_agents_in_region(
            request.simulation_id,
            region,
        )
        return self._handleResponse(jupedsim_pb2.AgentIdsResponse, context, response, error)

    def GetDesiredSpeedOfAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.DesiredSpeedResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_desired_speed_of_agents(
            request.simulation_id,
            request.agent_ids,
        )
        return self._handleResponse(jupedsim_pb2.DesiredSpeedResponse, context, response, error)

    def SetDesiredSpeedOfAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.set_desired_speed_of_agents(
            request.simulation_id,
            request.desired_speeds,
        )
        return self._handleResponse(jupedsim_pb2.EmptyResponse, context, response, error)

    def AddWaypointStage(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.StageResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.add_waypoint_stage(
            request.simulation_id,
            request.point.x,
            request.point.y,
            request.distance,
        )
        return self._handleResponse(jupedsim_pb2.StageResponse, context, response, error)

    def AddExitStage(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.StageResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        polygon = [(p.x, p.y) for p in request.polygon]
        response, error = self._manager.add_exit_stage(
            request.simulation_id,
            polygon,
        )
        return self._handleResponse(jupedsim_pb2.StageResponse, context, response, error)

    def AddWaitingSetStage(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.StageResponse:
        """gRPC RPC method."""
        points = [(p.x, p.y) for p in request.points]
        response, error = self._manager.add_waiting_set_stage(
            request.simulation_id,
            points,
        )
        return self._handleResponse(jupedsim_pb2.StageResponse, context, response, error)

    def SetWaitingSetState(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.set_waiting_set_state(
            request.simulation_id,
            request.stage_id,
            request.state,
        )
        return self._handleResponse(jupedsim_pb2.EmptyResponse, context, response, error)

    def GetWaitingSetState(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.GetWaitingSetStateResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_waiting_set_state(
            request.simulation_id,
            request.stage_id,
        )
        return self._handleResponse(jupedsim_pb2.GetWaitingSetStateResponse, context, response, error)

    def AddJourney(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.AddJourneyResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.add_journey(
            request.simulation_id,
            list(request.stage_ids),
        )
        return self._handleResponse(jupedsim_pb2.AddJourneyResponse, context, response, error)

    def GetCorePropertiesOfAllAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.GetCorePropertiesOfAllAgentsResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_core_properties_of_all_agents(
            request.simulation_id,
        )
        return self._handleResponse(jupedsim_pb2.GetCorePropertiesOfAllAgentsResponse, context, response, error)

    def Iterate(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        count = request.count if request.HasField("count") else 1
        response, error = self._manager.iterate(request.simulation_id, count)
        return self._handleResponse(jupedsim_pb2.EmptyResponse, context, response, error)


class Server:
    """Context manager for a running gRPC server."""

    def __init__(self, socket, debug=False, sqlite=False):
        self._socket = socket
        self._debug = debug
        self._sqlite = sqlite
        self._server = None

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, *args):
        self.stop()

    def start(self):
        # Disable SO_REUSEPORT so that we cannot bind to a port already in use.
        self._server = grpc.server(
            concurrent.futures.ThreadPoolExecutor(max_workers=10),
            options=[("grpc.so_reuseport", 0)],
        )
        jupedsim_pb2_grpc.add_JuPedSimServiceServicer_to_server(
            JuPedSimServiceServicer(self._debug, self._sqlite), self._server
        )
        # bound_port is 0 if the port is already in use with SO_REUSEPORT disabled (see above).
        bound_port = self._server.add_insecure_port(self._socket)
        if bound_port == 0:
            raise OSError(f"Could not bind to {self._socket} (already in use?)")
        self._server.start()
        print(f"Server running on {self._socket}")

    def stop(self, grace=0):
        if self._server:
            self._server.stop(grace)

    def wait_for_termination(self, timeout=None):
        """Block until the server is stopped (or the timeout elapses)."""
        if self._server:
            self._server.wait_for_termination(timeout)


def serve(socket, debug=False, sqlite=False):
    with Server(socket, debug, sqlite) as server:
        try:
            server.wait_for_termination()
        except KeyboardInterrupt:
            print("\nShutting down.")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Run the JuPedSim gRPC server.")
    parser.add_argument(
        "--port", type=int, default=50051,
        help="TCP port to listen on (default: 50051)",
    )
    parser.add_argument(
        "--unix-socket", help="Unix socket to listen on",
    )
    parser.add_argument(
        "--debug",
        help="Dumps a python script per simulation that can be run without sumo.",
    )
    parser.add_argument(
        "--sqlite", action="store_true",
        help="Writes a trajectory db 'simulation{id}.sqlite' per simulation.",
    )
    args = parser.parse_args()
    serve("unix:" + args.unix_socket if args.unix_socket else f"[::]:{args.port}", args.debug, args.sqlite)
