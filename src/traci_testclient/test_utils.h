/*******************************************************************************
 * Copyright (c) 2021-2022, Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
 * Copyright (c) 2022 Ansys, Inc.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *******************************************************************************/

//-----------------------------------------------------------------------------
/** @file  test_utils.h */
//-----------------------------------------------------------------------------

#pragma once

#include <MantleAPI/Common/i_identifiable.h>
#include <MantleAPI/Common/position.h>
#include <MantleAPI/EnvironmentalConditions/road_condition.h>
#include <MantleAPI/EnvironmentalConditions/weather.h>
#include <MantleAPI/Execution/i_environment.h>
#include <MantleAPI/Map/i_coord_converter.h>
#include <MantleAPI/Map/i_lane_location_query_service.h>
#include <MantleAPI/Map/lane_definition.h>
#include <MantleAPI/Traffic/entity_properties.h>
#include <MantleAPI/Traffic/i_controller_config.h>
#include <MantleAPI/Traffic/i_controller_repository.h>
#include <MantleAPI/Traffic/i_entity.h>
#include <MantleAPI/Traffic/i_entity_repository.h>
#include <MantleAPI/Traffic/i_traffic_swarm_service.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace mantle_api
{
class MockGeometryHelper : public mantle_api::IGeometryHelper
{
public:
  MOCK_METHOD(mantle_api::Vec3<units::length::meter_t>, TranslateGlobalPositionLocally, (const Vec3<units::length::meter_t>& global_position, const Orientation3<units::angle::radian_t>& local_orientation, const Vec3<units::length::meter_t>& local_translation), (const override));

  virtual std::vector<Vec3<units::length::meter_t>> TransformPolylinePointsFromWorldToLocal(
      const std::vector<Vec3<units::length::meter_t>>& polyline_points,
      const Vec3<units::length::meter_t>& position,
      const Orientation3<units::angle::radian_t>& orientation) const
  {
    // do not transform but return original points
    std::ignore = position;
    std::ignore = orientation;
    return polyline_points;
  }

  virtual Vec3<units::length::meter_t> TransformPositionFromWorldToLocal(
      const Vec3<units::length::meter_t>& point_position,
      const Vec3<units::length::meter_t>& position,
      const Orientation3<units::angle::radian_t>& orientation) const
  {
    // do not transform but return original points
    std::ignore = position;
    std::ignore = orientation;
    return point_position;
  }

  MOCK_METHOD(bool, AreOrientedSimilarly, (const Orientation3<units::angle::radian_t>& orientation1, const Orientation3<units::angle::radian_t>& orientation2), (const override));
};

class MockConverter : public mantle_api::ICoordConverter
{
public:
  MOCK_METHOD(mantle_api::Vec3<units::length::meter_t>, Convert, (mantle_api::Position position), (const override));
};

class MockVehicle : public mantle_api::IVehicle
{
public:
  MOCK_METHOD(mantle_api::UniqueId, GetUniqueId, (), (const, override));

  void SetName(const std::string& name) override { name_ = name; }
  const std::string& GetName() const override { return name_; }

  MOCK_METHOD(void, SetPosition, (const mantle_api::Vec3<units::length::meter_t>& inert_pos), (override));
  MOCK_METHOD(mantle_api::Vec3<units::length::meter_t>, GetPosition, (), (const, override));

  MOCK_METHOD(void, SetVelocity, (const mantle_api::Vec3<units::velocity::meters_per_second_t>& velocity), (override));
  MOCK_METHOD(mantle_api::Vec3<units::velocity::meters_per_second_t>, GetVelocity, (), (const, override));

  MOCK_METHOD(void, SetAcceleration, (const mantle_api::Vec3<units::acceleration::meters_per_second_squared_t>& acceleration), (override));
  MOCK_METHOD(mantle_api::Vec3<units::acceleration::meters_per_second_squared_t>, GetAcceleration, (), (const, override));

  MOCK_METHOD(void, SetOrientation, (const mantle_api::Orientation3<units::angle::radian_t>& orientation), (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angle::radian_t>, GetOrientation, (), (const, override));

  MOCK_METHOD(void, SetOrientationRate, (const mantle_api::Orientation3<units::angular_velocity::radians_per_second_t>& orientation_rate), (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angular_velocity::radians_per_second_t>, GetOrientationRate, (), (const, override));

  MOCK_METHOD(void,
              SetOrientationAcceleration,
              (const mantle_api::Orientation3<units::angular_acceleration::radians_per_second_squared_t>& orientation_acceleration),
              (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angular_acceleration::radians_per_second_squared_t>, GetOrientationAcceleration, (), (const, override));

  MOCK_METHOD(void, SetAssignedLaneIds, (const std::vector<std::uint64_t>& ids), (override));
  MOCK_METHOD(std::vector<std::uint64_t>, GetAssignedLaneIds, (), (const, override));

  MOCK_METHOD(void, SetVisibility, (const EntityVisibilityConfig& visibility), (override));
  MOCK_METHOD(EntityVisibilityConfig, GetVisibility, (), (const, override));

  void SetProperties(std::unique_ptr<mantle_api::EntityProperties> properties) override { properties_ = std::move(properties); }

  MOCK_METHOD(mantle_api::VehicleProperties*, GetPropertiesImpl, (), (const));
  mantle_api::VehicleProperties* GetProperties() const override
  {
    if (auto* properties = GetPropertiesImpl())
    {
      return properties;
    }
    return static_cast<mantle_api::VehicleProperties*>(properties_.get());
  }

  void SetIndicatorState(mantle_api::IndicatorState state) override { std::ignore = state; }
  mantle_api::IndicatorState GetIndicatorState() const override { return mantle_api::IndicatorState::kUnknown; }

private:
  std::string name_{};
  std::unique_ptr<mantle_api::EntityProperties> properties_{std::make_unique<mantle_api::VehicleProperties>()};
};

class MockQueryService : public mantle_api::ILaneLocationQueryService
{
public:
  MOCK_METHOD(Orientation3<units::angle::radian_t>, GetLaneOrientation, (const Vec3<units::length::meter_t>& position), (const override));

  MOCK_METHOD(Vec3<units::length::meter_t>, GetUpwardsShiftedLanePosition, (const Vec3<units::length::meter_t>& position, double upwards_shift, bool allowed_to_leave_lane), (const override));
  bool IsPositionOnLane(const Vec3<units::length::meter_t>& position) const override
  {
    std::ignore = position;
    return false;
  }

  MOCK_METHOD(std::vector<UniqueId>, GetLaneIdsAtPosition, (const Vec3<units::length::meter_t>& position), (const override));

  MOCK_METHOD(std::optional<Pose>, FindLanePoseAtDistanceFrom, (const Pose&, units::length::meter_t, Direction), (const, override));

  std::optional<Pose> FindRelativeLanePoseAtDistanceFrom(const Pose& reference_pose_on_lane, int relative_target_lane, units::length::meter_t distance, units::length::meter_t lateral_offset) const override
  {
    std::ignore = reference_pose_on_lane;
    std::ignore = relative_target_lane;
    std::ignore = distance;
    std::ignore = lateral_offset;
    Pose pose{};
    return pose;
  }

  std::optional<LaneId> GetRelativeLaneId(const mantle_api::Pose& reference_pose_on_lane, int relative_lane_target) const override
  {
    std::ignore = reference_pose_on_lane;
    std::ignore = relative_lane_target;
    return 0;
  }

  MOCK_METHOD(std::optional<units::length::meter_t>,
              GetLongitudinalLaneDistanceBetweenPositions,
              (const mantle_api::Vec3<units::length::meter_t>&, const mantle_api::Vec3<units::length::meter_t>&),
              (const, override));

  MOCK_METHOD(std::optional<Vec3<units::length::meter_t>>,
              GetPosition,
              (const Pose&, LateralDisplacementDirection, units::length::meter_t),
              (const, override));

  MOCK_METHOD(std::optional<Pose>,
              GetProjectedPoseAtLane,
              (const mantle_api::Vec3<units::length::meter_t>&, mantle_api::LaneId),
              (const, override));

  MOCK_METHOD(std::optional<Vec3<units::length::meter_t>>,
              GetProjectedCenterLinePoint,
              (const Vec3<units::length::meter_t>&),
              (const, override));

private:
  MockVehicle test_vehicle_{};
};

class MockPedestrian : public mantle_api::IPedestrian
{
public:
  MOCK_METHOD(mantle_api::UniqueId, GetUniqueId, (), (const, override));

  void SetName(const std::string& name) override { name_ = name; }
  const std::string& GetName() const override { return name_; }

  MOCK_METHOD(void, SetPosition, (const mantle_api::Vec3<units::length::meter_t>& inert_pos), (override));
  MOCK_METHOD(mantle_api::Vec3<units::length::meter_t>, GetPosition, (), (const, override));

  MOCK_METHOD(void, SetVelocity, (const mantle_api::Vec3<units::velocity::meters_per_second_t>& velocity), (override));
  MOCK_METHOD(mantle_api::Vec3<units::velocity::meters_per_second_t>, GetVelocity, (), (const, override));

  MOCK_METHOD(void, SetAcceleration, (const mantle_api::Vec3<units::acceleration::meters_per_second_squared_t>& acceleration), (override));
  MOCK_METHOD(mantle_api::Vec3<units::acceleration::meters_per_second_squared_t>, GetAcceleration, (), (const, override));

  MOCK_METHOD(void, SetOrientation, (const mantle_api::Orientation3<units::angle::radian_t>& orientation), (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angle::radian_t>, GetOrientation, (), (const, override));

  MOCK_METHOD(void, SetOrientationRate, (const mantle_api::Orientation3<units::angular_velocity::radians_per_second_t>& orientation_rate), (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angular_velocity::radians_per_second_t>, GetOrientationRate, (), (const, override));

  MOCK_METHOD(void,
              SetOrientationAcceleration,
              (const mantle_api::Orientation3<units::angular_acceleration::radians_per_second_squared_t>& orientation_acceleration),
              (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angular_acceleration::radians_per_second_squared_t>, GetOrientationAcceleration, (), (const, override));

  MOCK_METHOD(void, SetAssignedLaneIds, (const std::vector<std::uint64_t>& ids), (override));
  MOCK_METHOD(std::vector<std::uint64_t>, GetAssignedLaneIds, (), (const, override));

  MOCK_METHOD(void, SetVisibility, (const EntityVisibilityConfig& visibility), (override));
  MOCK_METHOD(EntityVisibilityConfig, GetVisibility, (), (const, override));

  void SetProperties(std::unique_ptr<mantle_api::EntityProperties> properties) override { properties_ = std::move(properties); }
  mantle_api::PedestrianProperties* GetProperties() const override
  {
    return static_cast<mantle_api::PedestrianProperties*>(properties_.get());
  }

private:
  std::string name_{};
  std::unique_ptr<mantle_api::EntityProperties> properties_{nullptr};
};

class MockStaticObject : public mantle_api::IStaticObject
{
public:
  MOCK_METHOD(mantle_api::UniqueId, GetUniqueId, (), (const, override));

  void SetName(const std::string& name) override { name_ = name; }
  const std::string& GetName() const override { return name_; }

  MOCK_METHOD(void, SetPosition, (const mantle_api::Vec3<units::length::meter_t>& inert_pos), (override));
  MOCK_METHOD(mantle_api::Vec3<units::length::meter_t>, GetPosition, (), (const, override));

  MOCK_METHOD(void, SetVelocity, (const mantle_api::Vec3<units::velocity::meters_per_second_t>& velocity), (override));
  MOCK_METHOD(mantle_api::Vec3<units::velocity::meters_per_second_t>, GetVelocity, (), (const, override));

  MOCK_METHOD(void, SetAcceleration, (const mantle_api::Vec3<units::acceleration::meters_per_second_squared_t>& acceleration), (override));
  MOCK_METHOD(mantle_api::Vec3<units::acceleration::meters_per_second_squared_t>, GetAcceleration, (), (const, override));

  MOCK_METHOD(void, SetOrientation, (const mantle_api::Orientation3<units::angle::radian_t>& orientation), (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angle::radian_t>, GetOrientation, (), (const, override));

  MOCK_METHOD(void, SetOrientationRate, (const mantle_api::Orientation3<units::angular_velocity::radians_per_second_t>& orientation_rate), (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angular_velocity::radians_per_second_t>, GetOrientationRate, (), (const, override));

  MOCK_METHOD(void,
              SetOrientationAcceleration,
              (const mantle_api::Orientation3<units::angular_acceleration::radians_per_second_squared_t>& orientation_acceleration),
              (override));
  MOCK_METHOD(mantle_api::Orientation3<units::angular_acceleration::radians_per_second_squared_t>, GetOrientationAcceleration, (), (const, override));

  MOCK_METHOD(void, SetAssignedLaneIds, (const std::vector<std::uint64_t>& ids), (override));
  MOCK_METHOD(std::vector<std::uint64_t>, GetAssignedLaneIds, (), (const, override));

  MOCK_METHOD(void, SetVisibility, (const EntityVisibilityConfig& visibility), (override));
  MOCK_METHOD(EntityVisibilityConfig, GetVisibility, (), (const, override));

  void SetProperties(std::unique_ptr<mantle_api::EntityProperties> properties) override { properties_ = std::move(properties); }
  MOCK_METHOD(mantle_api::StaticObjectProperties*, GetPropertiesImpl, (), (const));

  mantle_api::StaticObjectProperties* GetProperties() const override
  {
    if (auto* properties = GetPropertiesImpl())
    {
      return properties;
    }
    return static_cast<mantle_api::StaticObjectProperties*>(properties_.get());
  }

private:
  std::string name_{};
  std::unique_ptr<mantle_api::EntityProperties> properties_{nullptr};
};

class MockEntityRepository : public mantle_api::IEntityRepository
{
public:
  template <typename T>
  T& RegisterEntity()
  {
    auto entity = std::make_unique<T>();
    auto& entity_ref = entities_.emplace_back(std::move(entity));
    return *(dynamic_cast<T*>(entity_ref.get()));
  }

  MockEntityRepository()
      : test_vehicle_{RegisterEntity<MockVehicle>()},
        test_pedestrian_{RegisterEntity<MockPedestrian>()},
        test_static_object_{RegisterEntity<MockStaticObject>()}
  {
  }

  MOCK_METHOD(mantle_api::IVehicle&,
              Create,
              (const std::string& name, const mantle_api::VehicleProperties& properties),
              (override));

  mantle_api::IVehicle& Create(mantle_api::UniqueId id,
                               const std::string& name,
                               const mantle_api::VehicleProperties& properties) override
  {
    std::ignore = id;
    std::ignore = name;
    std::ignore = properties;
    return test_vehicle_;
  }

  MOCK_METHOD(mantle_api::IPedestrian&,
              Create,
              (const std::string& name, const mantle_api::PedestrianProperties& properties),
              (override));

  mantle_api::IPedestrian& Create(mantle_api::UniqueId id,
                                  const std::string& name,
                                  const mantle_api::PedestrianProperties& properties) override
  {
    std::ignore = id;
    std::ignore = name;
    std::ignore = properties;
    return test_pedestrian_;
  }

  mantle_api::IStaticObject& Create(mantle_api::UniqueId id,
                                    const std::string& name,
                                    const mantle_api::StaticObjectProperties& properties) override
  {
    std::ignore = id;
    std::ignore = name;
    std::ignore = properties;
    return test_static_object_;
  }

  MOCK_METHOD(mantle_api::IStaticObject&,
              Create,
              (const std::string& name, const mantle_api::StaticObjectProperties& properties),
              ());

  MOCK_METHOD(mantle_api::IEntity*,
              GetImpl,
              (const std::string& name),
              ());

  MOCK_METHOD(mantle_api::IEntity*,
              GetImpl,
              (const std::string& name),
              (const));

  std::optional<std::reference_wrapper<IEntity>> Get(const std::string& name) override
  {
    if (auto* entity = GetImpl(name))
    {
      return *entity;
    }
    return test_vehicle_;
  }

  std::optional<std::reference_wrapper<const IEntity>> Get(const std::string& name) const override
  {
    if (auto* entity = GetImpl(name))
    {
      return *entity;
    }
    return test_vehicle_;
  }

  std::optional<std::reference_wrapper<IEntity>> Get(mantle_api::UniqueId id) override
  {
    std::ignore = id;
    return test_vehicle_;
  }

  std::optional<std::reference_wrapper<const IEntity>> Get(mantle_api::UniqueId id) const override
  {
    std::ignore = id;
    return test_vehicle_;
  }

  mantle_api::IVehicle& GetHost() override { return test_vehicle_; }

  MOCK_METHOD(std::vector<std::unique_ptr<mantle_api::IEntity>>&,
              GetEntities,
              (),
              (const, override));

  bool Contains(UniqueId id) const override
  {
    std::ignore = id;
    return false;
  }

  MOCK_METHOD(void, Delete, (UniqueId), (override));
  MOCK_METHOD(void, Delete, (const std::string&), (override));

  void RegisterEntityCreatedCallback(const std::function<void(IEntity&)>& callback) override { std::ignore = callback; }
  void RegisterEntityDeletedCallback(const std::function<void(const std::string&)>& callback) override { std::ignore = callback; }
  void RegisterEntityDeletedCallback(const std::function<void(UniqueId)>& callback) override { std::ignore = callback; }

private:
  std::vector<std::unique_ptr<mantle_api::IEntity>> entities_{};
  MockVehicle& test_vehicle_;
  MockPedestrian& test_pedestrian_;
  MockStaticObject& test_static_object_;
};

class MockController : public mantle_api::IController
{
public:
  MOCK_METHOD(UniqueId,
              GetUniqueId,
              (),
              (const, override));

  MOCK_METHOD(void,
              SetName,
              (const std::string& name),
              (override));

  MOCK_METHOD(const std::string&,
              GetName,
              (),
              (const, override));

  MOCK_METHOD(void,
              ChangeState,
              (mantle_api::IController::LateralState, mantle_api::IController::LongitudinalState),
              (override));
};

class MockControllerRepository : public mantle_api::IControllerRepository
{
public:
  MOCK_METHOD(mantle_api::IController&,
              Create,
              (std::unique_ptr<IControllerConfig> config),
              (override));

  MOCK_METHOD(mantle_api::IController&,
              Create,
              (UniqueId id, std::unique_ptr<IControllerConfig> config),
              (override));

  MOCK_METHOD(std::optional<std::reference_wrapper<IController>>,
              Get,
              (UniqueId id),
              (override));

  MOCK_METHOD(bool,
              Contains,
              (UniqueId id),
              (const, override));

  MOCK_METHOD(void,
              Delete,
              (UniqueId id),
              (override));
};

class MockTrafficSwarmService : public mantle_api::ITrafficSwarmService
{
  public:
    MOCK_METHOD(std::vector<ITrafficSwarmService::SpawningPosition>,
                GetAvailableSpawningPoses,
                (),
                (const, override));

    MOCK_METHOD(mantle_api::VehicleProperties,
                GetVehicleProperties,
                (mantle_api::VehicleClass),
                (const, override));

    MOCK_METHOD(void,
                UpdateControllerConfig,
                (std::unique_ptr<mantle_api::ExternalControllerConfig>&, units::velocity::meters_per_second_t),
                (override));

    MOCK_METHOD(void,
                SetSwarmEntitiesCount,
                (size_t),
                (override));
};

class MockEnvironment : public mantle_api::IEnvironment
{
public:
  MOCK_METHOD(void,
              CreateMap,
              (const std::string& file_path, const mantle_api::MapDetails& map_details),
              (override)

  );

  MOCK_METHOD(void, AddEntityToController, (mantle_api::IEntity & entity, std::uint64_t controller_id), (override)

  );

  MOCK_METHOD(void, RemoveEntityFromController, (std::uint64_t entity_id, std::uint64_t controller_id), (override));

  MOCK_METHOD(void,
              UpdateControlStrategies,
              (std::uint64_t entity_id,
               std::vector<std::shared_ptr<mantle_api::ControlStrategy>> control_strategies),
              (override));

  MOCK_METHOD(bool,
              HasControlStrategyGoalBeenReached,
              (std::uint64_t entity_id, mantle_api::ControlStrategyType type),
              (const, override));

  MOCK_METHOD(void,
              SetWeather,
              (mantle_api::Weather weather),
              (override));

  const mantle_api::ILaneLocationQueryService& GetQueryService() const override { return query_service_; }

  const mantle_api::ICoordConverter* GetConverter() const override { return &converter_; }

  const mantle_api::IGeometryHelper* GetGeometryHelper() const override { return &geometry_helper_; }

  mantle_api::IEntityRepository& GetEntityRepository() override { return entity_repository_; }

  const mantle_api::IEntityRepository& GetEntityRepository() const override { return entity_repository_; }

  MockControllerRepository& GetControllerRepository() override { return controller_repository_; }

  const MockControllerRepository& GetControllerRepository() const override { return controller_repository_; }

  void SetRoadCondition(std::vector<mantle_api::FrictionPatch> friction_patches) override
  {
    std::ignore = friction_patches;
  }

  MOCK_METHOD(void,
              SetTrafficSignalState,
              (const std::string& traffic_signal_name, const std::string& traffic_signal_state),
              (override));

  MOCK_METHOD(void,
              ExecuteCustomCommand,
              (const std::vector<std::string>& actors, const std::string& type, const std::string& command),
              (override));

  MOCK_METHOD(void,
              SetUserDefinedValue,
              (const std::string& name, const std::string& value),
              (override));

  MOCK_METHOD(std::optional<std::string>,
              GetUserDefinedValue,
              (const std::string& name),
              (override));

  void SetDateTime(mantle_api::Time date_time) override { std::ignore = date_time; }

  mantle_api::Time GetDateTime() override { return mantle_api::Time(); }

  MOCK_METHOD(mantle_api::Time, GetSimulationTime, (), (override));

  MOCK_METHOD(void,
              SetDefaultRoutingBehavior,
              (mantle_api::DefaultRoutingBehavior default_routing_behavior),
              (override));

  MOCK_METHOD(void,
              AssignRoute,
              (mantle_api::UniqueId entity_id, mantle_api::RouteDefinition route_definition),
              (override));

  MOCK_METHOD(void,
              InitTrafficSwarmService,
              (const mantle_api::TrafficSwarmParameters& parameters),
              (override));

  MockTrafficSwarmService& GetTrafficSwarmService() override { return traffic_swarm_service_; }

private:
  MockQueryService query_service_{};
  MockEntityRepository entity_repository_{};
  MockControllerRepository controller_repository_{};
  MockConverter converter_{};
  MockGeometryHelper geometry_helper_{};
  MockTrafficSwarmService traffic_swarm_service_{};
};

}  // namespace mantle_api
