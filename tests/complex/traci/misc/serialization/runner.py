#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

import io
import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.join(os.environ['SUMO_HOME'], 'tools'))
from traci import constants as tc  # noqa
from traci.connection import Connection  # noqa


def string_bytes(value):
    encoded = str(value).encode('utf8')
    return struct.pack('!i', len(encoded)) + encoded


class PackingTest(unittest.TestCase):
    def setUp(self):
        self.connection = Connection.__new__(Connection)

    def check_wire(self, format, value, expected):
        result = self.connection._pack(format, value)
        self.assertIsInstance(result, bytes)
        self.assertEqual(result, expected)

    def test_scalar_and_position_formats(self):
        cases = [
            ('i', -2147483648, struct.pack('!Bi', tc.TYPE_INTEGER, -2147483648)),
            ('I', 123, struct.pack('!i', 123)),
            ('d', -0., struct.pack('!Bd', tc.TYPE_DOUBLE, -0.)),
            ('D', 0.125, struct.pack('!d', 0.125)),
            ('b', -128, struct.pack('!Bb', tc.TYPE_BYTE, -128)),
            ('B', 255, struct.pack('!BB', tc.TYPE_UBYTE, 255)),
            ('u', 255, struct.pack('!B', 255)),
            ('t', 3, struct.pack('!Bi', tc.TYPE_COMPOUND, 3)),
            ('o', (12., -5.), struct.pack('!Bdd', tc.POSITION_2D, 12., -5.)),
            ('O', (12., -5., 3.), struct.pack('!Bddd', tc.POSITION_3D, 12., -5., 3.)),
            ('g', (12., -5.), struct.pack('!Bdd', tc.POSITION_LON_LAT, 12., -5.)),
            ('G', (12., -5., 3.), struct.pack('!Bddd', tc.POSITION_LON_LAT_ALT, 12., -5., 3.)),
            ('r', ('road_ä', 12.5, 2), struct.pack('!B', tc.POSITION_ROADMAP) +
             string_bytes('road_ä') + struct.pack('!dB', 12.5, 2)),
        ]
        for format, value, expected in cases:
            with self.subTest(format=format):
                self.check_wire(format, value, expected)
        self.assertEqual(self.connection._pack(''.join(c[0] for c in cases), *(c[1] for c in cases)),
                         b''.join(c[2] for c in cases))
        self.assertEqual(self.connection._pack(''), b'')

    def test_strings_and_large_unicode_lists(self):
        for value in ('', 'München 東京\0', 'ä東京' * 50000, 42, b'bytes'):
            with self.subTest(length=len(str(value))):
                self.check_wire('s', value, struct.pack('!B', tc.TYPE_STRING) + string_bytes(value))
        for values in ([], ['a', 'ä', '東京'], ['road_ä東京_%s' % i for i in range(10000)]):
            expected = struct.pack('!Bi', tc.TYPE_STRINGLIST, len(values))
            expected += b''.join(string_bytes(value) for value in values)
            self.check_wire('l', values, expected)

    def test_polygon_length_boundaries_and_large_shapes(self):
        for count in (0, 255, 256, 20000):
            points = [(float(index), -float(index)) for index in range(count)]
            if count <= 255:
                expected = struct.pack('!BB', tc.TYPE_POLYGON, count)
            else:
                expected = struct.pack('!BBi', tc.TYPE_POLYGON, 0, count)
            expected += b''.join(struct.pack('!dd', *point) for point in points)
            self.check_wire('p', points, expected)

    def test_color_and_float_lists(self):
        self.check_wire('c', (1, 2, 3), struct.pack('!BBBBB', tc.TYPE_COLOR, 1, 2, 3, 255))
        self.check_wire('c', (255, 128, 0, 0), struct.pack('!BBBBB', tc.TYPE_COLOR, 255, 128, 0, 0))
        for values in ([], [0., -0., float('inf'), float('-inf'), float('nan')],
                       [index / 10. for index in range(10000)]):
            expected = struct.pack('!Bi', tc.TYPE_DOUBLELIST, len(values))
            expected += struct.pack('!%sd' % len(values), *values)
            self.check_wire('f', values, expected)

    def test_mixed_variable_length_fields(self):
        values = (4, '東京', ['ä', 'road'], [(1., 2.), (3., 4.)], [0.5, 1.5])
        expected = struct.pack('!Bi', tc.TYPE_COMPOUND, 4)
        expected += struct.pack('!B', tc.TYPE_STRING) + string_bytes(values[1])
        expected += struct.pack('!Bi', tc.TYPE_STRINGLIST, 2) + string_bytes('ä') + string_bytes('road')
        expected += struct.pack('!BBdddd', tc.TYPE_POLYGON, 2, 1., 2., 3., 4.)
        expected += struct.pack('!Bidd', tc.TYPE_DOUBLELIST, 2, 0.5, 1.5)
        self.assertEqual(self.connection._pack('tslpf', *values), expected)


if __name__ == '__main__':
    output = io.StringIO()
    result = unittest.TextTestRunner(stream=output).run(unittest.defaultTestLoader.loadTestsFromTestCase(PackingTest))
    if not result.wasSuccessful():
        sys.stderr.write(output.getvalue())
        sys.exit(1)
    print('TraCI packing checks passed')
