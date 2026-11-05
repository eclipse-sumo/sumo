// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2026 German Aerospace Center (DLR) and others.
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

#include <gtest/gtest.h>
#include <libsumo/Helper.h>

namespace {

std::vector<libsumo::Subscription> subscriptions() {
    std::vector<libsumo::Subscription> result;
    for (int index = 0; index < 5; ++index) {
        result.emplace_back(0, "v" + std::to_string(index), std::vector<int>{},
                            std::vector<std::shared_ptr<tcpip::Storage>>{}, 0, 10, 0, 100.);
        result.back().filterLanes = {index};
    }
    return result;
}

std::vector<std::string> ids(const std::vector<libsumo::Subscription>& values) {
    std::vector<std::string> result;
    for (const auto& value : values) {
        result.push_back(value.id);
    }
    return result;
}

}

TEST(SubscriptionCompaction, NoRemovalsKeepContextAndOrder) {
    for (int position : {0, 2, 4}) {
        auto values = subscriptions();
        auto* context = &values[position];
        auto* previous = context;
        libsumo::Helper::removeSubscriptionsIf(values, context, [](const libsumo::Subscription&) { return false; });
        EXPECT_EQ(context, previous);
        EXPECT_EQ(ids(values), (std::vector<std::string>{"v0", "v1", "v2", "v3", "v4"}));
    }
}

TEST(SubscriptionCompaction, KeptContextCanReceiveFiltersAfterMoving) {
    for (int position : {0, 2, 4}) {
        auto values = subscriptions();
        auto* context = &values[position];
        const auto contextID = context->id;
        libsumo::Helper::removeSubscriptionsIf(values, context, [&](const libsumo::Subscription& value) {
            return value.id != contextID && value.id != "v0" && value.id != "v4";
        });
        ASSERT_NE(context, nullptr);
        EXPECT_EQ(context->id, contextID);
        EXPECT_EQ(context->filterLanes, (std::vector<int>{position}));
        context->activeFilters = libsumo::SUBS_FILTER_NOOPPOSITE;
        EXPECT_TRUE(std::is_sorted(values.begin(), values.end(),
        [](const libsumo::Subscription& a, const libsumo::Subscription& b) { return a.id < b.id; }));
        for (const auto& value : values) {
            EXPECT_EQ(value.activeFilters, value.id == contextID ? libsumo::SUBS_FILTER_NOOPPOSITE : libsumo::SUBS_FILTER_NONE);
        }
    }
}

TEST(SubscriptionCompaction, RemovedContextsAreClearedInEveryPosition) {
    for (int position : {0, 2, 4}) {
        auto values = subscriptions();
        auto* context = &values[position];
        const auto contextID = context->id;
        libsumo::Helper::removeSubscriptionsIf(values, context, [&](const libsumo::Subscription& value) {
            return value.id == contextID;
        });
        EXPECT_EQ(context, nullptr);
        EXPECT_EQ(values.size(), 4u);
        for (const auto& value : values) {
            EXPECT_NE(value.id, contextID);
        }
    }
}

TEST(SubscriptionCompaction, AllRemovedAndNullContexts) {
    for (int position : {-1, 0, 2, 4}) {
        auto values = subscriptions();
        auto* context = position < 0 ? nullptr : &values[position];
        libsumo::Helper::removeSubscriptionsIf(values, context, [](const libsumo::Subscription&) { return true; });
        EXPECT_TRUE(values.empty());
        EXPECT_EQ(context, nullptr);
    }
    auto values = subscriptions();
    libsumo::Subscription* context = nullptr;
    libsumo::Helper::removeSubscriptionsIf(values, context, [](const libsumo::Subscription&) { return false; });
    EXPECT_EQ(context, nullptr);
    EXPECT_EQ(values.size(), 5u);
}

TEST(SubscriptionCompaction, UnmatchedStalePointerIsClearedWithoutDereferencing) {
    auto values = subscriptions();
    auto retired = std::make_unique<libsumo::Subscription>(values.front());
    auto* context = retired.get();
    retired.reset();
    libsumo::Helper::removeSubscriptionsIf(values, context, [](const libsumo::Subscription&) { return false; });
    EXPECT_EQ(context, nullptr);
    EXPECT_EQ(values.size(), 5u);
}

TEST(SubscriptionCompaction, ExpiryBoundaryIsStrict) {
    auto values = subscriptions();
    values[0].endTime = 9;
    values[1].beginTime = 20;
    values[1].endTime = 20;
    auto* context = &values[2];
    libsumo::Helper::removeSubscriptionsIf(values, context, [](const libsumo::Subscription& value) {
        return value.endTime < 10;
    });
    EXPECT_EQ(ids(values), (std::vector<std::string>{"v1", "v2", "v3", "v4"}));
    ASSERT_NE(context, nullptr);
    EXPECT_EQ(context->id, "v2");
}
