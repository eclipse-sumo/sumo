import concurrent.futures
import os
import sys

import grpc

sys.path.insert(0, os.path.dirname(__file__))

import jupedsim_pb2
import jupedsim_pb2_grpc
from simulation_manager import SimulationManager


class JuPedSimServiceServicer(jupedsim_pb2_grpc.JuPedSimServiceServicer):
    def __init__(self):
        self._manager = SimulationManager()

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
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.CreateGeometryResponse()
        return response

    def SwitchGeometry(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.switch_geometry(
            request.simulation_id,
            request.geometry_id,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.EmptyResponse()
        return response

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
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.CreateSimulationResponse()
        return response

    def CreateAgent(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.CreateAgentResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.create_agent(
            request.simulation_id,
            request.agent_model_class,
            request.agent_model_parameters,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.CreateAgentResponse()
        return response

    def RemoveAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.remove_agents(
            request.simulation_id,
            request.agent_ids,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.EmptyResponse()
        return response

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
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.EmptyResponse()
        return response

    def GetAgentsInRegion(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.AgentIdsResponse:
        """gRPC RPC method."""
        region = [(p.x, p.y) for p in request.region.points]
        response, error = self._manager.get_agents_in_region(
            request.simulation_id,
            region,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.AgentIdsResponse()
        return response

    def GetDesiredSpeedOfAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.DesiredSpeedResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_desired_speed_of_agents(
            request.simulation_id,
            request.agent_ids,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.DesiredSpeedResponse()
        return response

    def SetDesiredSpeedOfAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.set_desired_speed_of_agents(
            request.simulation_id,
            request.desired_speeds,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.EmptyResponse()
        return response

    def AddWaypointStage(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.AddWaypointStageResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.add_waypoint_stage(
            request.simulation_id,
            request.point.x,
            request.point.y,
            request.distance,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.AddWaypointStageResponse()
        return response

    def AddExitStage(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.AddExitStageResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        polygon = [(p.x, p.y) for p in request.polygon]
        response, error = self._manager.add_exit_stage(
            request.simulation_id,
            polygon,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.AddExitStageResponse()
        return response

    def AddWaitingSetStage(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.AddWaitingSetStageResponse:
        """gRPC RPC method."""
        points = [(p.x, p.y) for p in request.points]
        response, error = self._manager.add_waiting_set_stage(
            request.simulation_id,
            points,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.AddWaitingSetStageResponse()
        return response

    def SetWaitingSetState(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.EmptyResponse:
        """gRPC RPC method."""
        response, error = self._manager.set_waiting_set_state(
            request.simulation_id,
            request.stage_id,
            request.state,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.EmptyResponse()
        return response

    def GetWaitingSetState(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.GetWaitingSetStateResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_waiting_set_state(
            request.simulation_id,
            request.stage_id,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.GetWaitingSetStateResponse()
        return response

    def AddJourney(  # noqa: N802 (PascalCase per gRPC/RPC convention, not local variable/function)
        self, request, context
    ) -> jupedsim_pb2.AddJourneyResponse:
        """gRPC RPC method — PascalCase preserved from .proto definition."""
        response, error = self._manager.add_journey(
            request.simulation_id,
            list(request.stage_ids),
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.AddJourneyResponse()
        return response

    def GetCorePropertiesOfAllAgents(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.GetCorePropertiesOfAllAgentsResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_core_properties_of_all_agents(
            request.simulation_id,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.GetCorePropertiesOfAllAgentsResponse()
        return response

    def Iterate(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.IterateResponse:
        """gRPC RPC method."""
        count = request.count if request.HasField("count") else 1
        response, error = self._manager.iterate(request.simulation_id, count)
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.IterateResponse()
        return response

    def GetAgentCount(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.CountResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_agent_count(
            request.simulation_id,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.CountResponse()
        return response

    def GetIterationCount(  # noqa: N802
        self, request, context
    ) -> jupedsim_pb2.CountResponse:
        """gRPC RPC method."""
        response, error = self._manager.get_iteration_count(
            request.simulation_id,
        )
        if error:
            context.set_code(grpc.StatusCode.INVALID_ARGUMENT)
            context.set_details(error)
            return jupedsim_pb2.CountResponse()
        return response


class Server:
    """Context manager for a running gRPC server."""

    def __init__(self, port=50051):
        self._port = port
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
            JuPedSimServiceServicer(), self._server
        )
        # bound_port is 0 if the port is already in use with SO_REUSEPORT disabled (see above).
        bound_port = self._server.add_insecure_port(f"[::]:{self._port}")
        if bound_port == 0:
            raise OSError(f"Could not bind to port {self._port} (already in use?)")
        self._server.start()
        print(f"Server running on port {self._port}")

    def stop(self, grace=0):
        if self._server:
            self._server.stop(grace)

    def wait_for_termination(self, timeout=None):
        """Block until the server is stopped (or the timeout elapses)."""
        if self._server:
            self._server.wait_for_termination(timeout)

    @property
    def port(self):
        return self._port


def serve(port=50051):
    with Server(port) as server:
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
    args = parser.parse_args()
    serve(args.port)
