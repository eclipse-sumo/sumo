#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Mark Colley
# @date    2026-11-16

import contextlib
import importlib.util
import io
import os
import sys
import tempfile
import warnings
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path
from unittest.mock import patch

root = Path(os.environ['SUMO_HOME'])
sys.path[:0] = [str(root / 'tools'), str(root / 'tools/import/gtfs')]
import gtfs2fcd  # noqa
import gtfsutils  # noqa

# Pandas 2.x uses NumPy's deprecated generic timedelta unit internally.
warnings.filterwarnings('ignore', message="The 'generic' unit for NumPy timedelta is deprecated",
                        category=DeprecationWarning,
                        module=r'pandas\.core\.(tools\.timedeltas|internals\.concat)$')

if len(sys.argv) > 1:
    spec = importlib.util.spec_from_file_location('gtfsutils_before', sys.argv[1])
    gtfsutils = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(gtfsutils)
    gtfs2fcd.gtfsutils = gtfsutils

files = {
    'routes.txt': 'route_id,route_short_name,route_type\nr,R,3\n',
    'trips.txt': 'route_id,service_id,trip_id,direction_id\nr,s,t,0\n',
    'stops.txt': 'stop_id,stop_name,stop_lat,stop_lon\na,A,52,10\nb,B,52.001,10\n',
    'stop_times.txt': ('trip_id,arrival_time,departure_time,stop_id,stop_sequence\n'
                       't,08:00:00,08:00:00,a,1\nt,08:01:00,08:01:00,b,2\n'),
    'calendar.txt': ('service_id,monday,tuesday,wednesday,thursday,friday,saturday,sunday,start_date,end_date\n'
                     's,1,1,1,1,1,1,1,20260101,20261231\n'),
    'calendar_dates.txt': 'service_id,date,exception_type\n',
}
fare_table = 'stop_id,fare_zone,fare_token,start_char\na,1,T1,A\nb,2,H,B\n'


def write_feed(destination, fare=False, missing=False):
    with zipfile.ZipFile(destination, 'w') as archive:
        for name, data in files.items():
            if not missing or name != 'routes.txt':
                archive.writestr(name, data)
        if fare:
            archive.writestr('fare_stops.txt', fare_table)


def options_for(feed, work):
    options = gtfs2fcd.check_options(gtfs2fcd.add_options().parse_args([
        '--gtfs', str(feed), '--date', '20261106', '--modes', 'bus',
        '--fcd', str(work), '--vtype-output', str(work / 'vtypes.xml')]))
    options.gpsdat = None
    return options


with tempfile.TemporaryDirectory() as temporary:
    work = Path(temporary)
    snapshots = []
    for fare in (True, False):
        case = work / ('fare' if fare else 'plain')
        case.mkdir()
        feed = case / 'feed.zip'
        write_feed(feed, fare)
        options = options_for(feed, case)
        with contextlib.redirect_stdout(io.StringIO()):
            assert gtfs2fcd.main(options)
        records = ET.parse(case / 'bus.fcd.xml').getroot().findall('timestep/vehicle')
        assert len(records) == 2
        expected = [('1', 'T1', 'A'), ('2', 'H', 'B')] if fare else [('', '', '')] * 2
        assert [(r.get('fareZone'), r.get('fareSymbol'), r.get('startFare')) for r in records] == expected
        snapshots.append([{k: v for k, v in record.attrib.items()
                           if k not in ('fareZone', 'fareSymbol', 'startFare')} for record in records])
        vehicles = ET.parse(case / 'bus.rou.xml').getroot().findall('vehicle')
        assert len(vehicles) == 1 and vehicles[0].get('id') == 't' and vehicles[0].get('depart') == '28800'
    assert snapshots[0] == snapshots[1]
    print('GTFS fare tables survive complete FCD conversion with unchanged trip and stop data')
    print('Feeds without fare tables preserve empty fare attributes')

    opened = []
    original_openz = gtfsutils.sumolib.openz

    def tracked_openz(*args, **kwargs):
        handle = original_openz(*args, **kwargs)
        opened.append(handle)
        return handle

    for missing in (False, True):
        feed = work / ('missing.zip' if missing else 'complete.zip')
        write_feed(feed, fare=True, missing=missing)
        with patch.object(gtfsutils.sumolib, 'openz', tracked_openz):
            try:
                with contextlib.redirect_stdout(io.StringIO()):
                    gtfsutils.get_merged_data(options_for(feed, work))
                assert not missing
            except KeyError:
                assert missing
        assert opened[-1].closed
    assert len(opened) == 2
    print('Input archive handles close after successful and failed imports')
