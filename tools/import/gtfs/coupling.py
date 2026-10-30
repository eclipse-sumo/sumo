# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2010-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    coupling.py
# @author  Jakob Erdmann
# @date    2026-10-29

"""
Helper functions for gtfs2pt.py for avoiding duplicate vehicles
"""

import sys
import pandas as pd

def get_shared_departure_trips(df: pd.DataFrame) -> dict:
    """
    For every trip_id, finds all other trips that depart from the same stop 
    at the exact same time.
    Returns:
        dict: { trip_id: [(other_trip_id, stop_id, other_stop_sequence), ...] }
    """
    # 1. Self-merge on stop and departure time to find overlaps
    # This pairs every trip with every other trip sharing the exact same stop event
    merged = df.merge(
        df, 
        on=["stop_id", "departure_time"], 
        suffixes=("", "_other")
    )

    # 2. Filter out self-matches (a trip sharing a stop with itself)
    overlapping_trips = merged[merged["trip_id"] != merged["trip_id_other"]]

    # 3. Group by the primary trip_id to build the dictionary
    grouped = overlapping_trips.groupby("trip_id")

    result = {}
    for trip_id, group in grouped:
        # Extract the requested tuple: (trip_id2, stop_id, stop_sequence_of_trip_id2)
        tuples = group[["trip_id_other", "stop_sequence", "stop_sequence_other"]].to_records(index=False)
        result[trip_id] = sorted([r.tolist() for r in tuples])

    return result


def get_trip_sequence_bounds(df: pd.DataFrame) -> pd.DataFrame:
    """
    Computes the minimum and maximum stop_sequence for every trip_id.
    Returns a DataFrame indexed by trip_id with columns 'min_sequence' and 'max_sequence'.
    """
    # Group by trip_id and aggregate using min and max functions
    bounds = df.groupby("trip_id")["stop_sequence"].agg(
        min_sequence="min",
        max_sequence="max"
    )
    bounds_dict = {row[0]: (row.min_sequence, row.max_sequence) 
                   for row in bounds.itertuples() }
    return bounds_dict


def delete_shared_stops(df, trip_and_index_list):
    # Set a MultiIndex to match pairs directly
    mask = df.set_index(["trip_id", "stop_sequence"]).index.isin(trip_and_index_list)
    # Invert the mask with ~ to keep unmatched
    return df[~mask].reset_index(drop=True)


def remove_coupled(df):
    seq_bounds = get_trip_sequence_bounds(df)
    shared_departures = get_shared_departure_trips(df)

    to_delete = []
    handledPairs = set()
    for trip_id, shared in shared_departures.items():
        min1, max1 = seq_bounds[trip_id]
        if len(shared) > 1:
            # group by distinct other trip_ids
            for trip_id2 in set([s[0] for s in shared]):
                if (trip_id2, trip_id) in handledPairs:
                    # common sequences are symmetrical so we can skip the second encounter
                    continue
                shared_seq = [(seq, seq2) for tid, seq, seq2 in shared if tid == trip_id2]
                if len(shared_seq) > 1:
                    # trips have at least one common movement between stops 
                    # print(trip_id, trip_id2, shared_seq)
                    min2, max2 = seq_bounds[trip_id2]
                    deleteID = None
                    tupleIndex = None
                    prefix1 = shared_seq[0][0] - min1
                    prefix2 = shared_seq[0][1] - min2
                    suffix1 = max1 - shared_seq[-1][0]
                    suffix2 = max2 - shared_seq[-1][1]

                    minLoss = min(prefix1, prefix2, suffix1, suffix2)
                    if prefix1 == minLoss:
                        # trip_id starts with a shared sequence
                        deleteID, tupleIndex = trip_id, 0
                    elif prefix2 == minLoss:
                        # trip_id2 starts with a shared sequence
                        deleteID, tupleIndex = trip_id2, 1
                    elif suffix1 == minLoss:
                        # trip_id ends with a shared sequence
                        deleteID, tupleIndex = trip_id, 0
                    elif suffix2 == minLoss:
                        # trip_id2 ends with a shared sequence
                        deleteID, tupleIndex = trip_id2, 1

                    if minLoss > 0:
                        # shared sequence is a merge/split. We elect to drop some data to avoid duplicate trains
                        print("Warning: loss of %s stops by trip %s due to merge/split with trip %s" % (
                            minLoss, deleteID, trip_id if trip_id != deleteID else trip_id2), file=sys.stderr)

                    handledPairs.add((trip_id, trip_id2))
                    to_delete += [(deleteID, pair[tupleIndex]) for pair in shared_seq]

    cleaned = delete_shared_stops(df, to_delete)
    return cleaned
