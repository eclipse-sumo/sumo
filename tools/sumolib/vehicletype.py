# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    vehicletype.py
# @author  Mirko Barthauer (Technische Universitaet Braunschweig, Institut fuer Verkehr und Stadtbauwesen)
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @author  Maxwell Schrader
# @date    2021-07-19

# This file is a port of createVehTypeDistribution.py to sumolib

from __future__ import absolute_import
import os
import sys
import re
import xml.dom.minidom
import random
from sumolib.files.additional import write_additional_minidom
try:
    from typing import Any, List, Tuple, Union
except ImportError:
    # there are python2 versions on MacOS coming without typing
    pass

"""
Creates a vehicle type distribution with a number of representative car-following parameter sets.
"""


class _FixDistribution(object):

    def __init__(self, params, isNumeric=True):
        if isNumeric:
            self._params = tuple([float(p) for p in params])
        else:
            self._params = params
        self._limits = (0, None)
        self._isNumeric = isNumeric
        self._maxSampleAttempts = 10

    def setMaxSamplingAttempts(self, n):
        if n is not None:
            self._maxSampleAttempts = n

    def setLimits(self, limits):
        self._limits = limits

    def sampleValue(self):
        if self._isNumeric:
            value = None
            nrSampleAttempts = 0
            # Sample until value falls into limits
            while nrSampleAttempts < self._maxSampleAttempts \
                    and (value is None or (self._limits[1] is not None and value > self._limits[1]) or
                         (self._limits[0] is not None and value < self._limits[0])):
                value = self._sampleValue()
                nrSampleAttempts += 1
            # Eventually apply fallback cutting value to limits
            if self._limits[0] is not None and value < self._limits[0]:
                value = self._limits[0]
            elif self._limits[1] is not None and value > self._limits[1]:
                value = self._limits[1]
        else:
            value = self._sampleValue()
        return value

    def sampleValueString(self, decimalPlaces):
        if self._isNumeric:
            decimalPattern = "%." + str(decimalPlaces) + "f"
            return decimalPattern % self.sampleValue()
        return self.sampleValue()

    def _sampleValue(self):
        return self._params[0]


class _NormalDistribution(_FixDistribution):

    def __init__(self, mu, sd):
        _FixDistribution.__init__(self, (mu, sd))

    def _sampleValue(self):
        return random.normalvariate(self._params[0], self._params[1])


class _LogNormalDistribution(_FixDistribution):

    def __init__(self, mu, sd):
        _FixDistribution.__init__(self, (mu, sd))

    def _sampleValue(self):
        return random.lognormvariate(self._params[0], self._params[1])


class _NormalCappedDistribution(_FixDistribution):

    def __init__(self, mu, sd, min, max):
        _FixDistribution.__init__(self, (mu, sd, min, max))
        if mu < min or mu > max:
            raise ValueError("mean %s is outside cutoff bounds [%s, %s]" % (mu, min, max))

    def _sampleValue(self):
        while True:
            cand = random.normalvariate(self._params[0], self._params[1])
            if cand >= self._params[2] and cand <= self._params[3]:
                return cand


class _UniformDistribution(_FixDistribution):

    def __init__(self, a, b):
        _FixDistribution.__init__(self, (a, b))

    def _sampleValue(self):
        return random.uniform(self._params[0], self._params[1])


class _GammaDistribution(_FixDistribution):

    def __init__(self, alpha, beta):
        _FixDistribution.__init__(self, (alpha, 1.0 / beta))

    def _sampleValue(self):
        return random.gammavariate(self._params[0], self._params[1])


_DIST_DICT = {
    'normal': _NormalDistribution,
    'lognormal': _LogNormalDistribution,
    'normalCapped': _NormalCappedDistribution,
    'uniform': _UniformDistribution,
    'gamma': _GammaDistribution
}


class VehAttribute:

    def __init__(self, name, is_param=False, distribution=None, distribution_params=None,
                 bounds=None, attribute_value=None):
        # type: (str, bool, str, Union[dict, Any], tuple, str) -> None
        """
        This emmulates one line of example config.txt in
            https://sumo.dlr.de/docs/Tools/Misc.html#createvehtypedistributionpy
        Either distribution or attribute_value should be populated
        Args:
            name (str): the name of the attribute. Examples: "tau", "sigma", "length"
            is_param (bool, optional): is the attribute a parameter that should be added as a child element.
            distribution (str, optional): the name of the distribution to use ()
            distribution_params (Union[dict, Any], optional): the parameters corresponding to the distribution
            bounds (tuple, optional): the bounds of the distribution.
            attribute_value (str, optional): if no distribution is given, the fixed value for the attribute
        """
        self.is_param = is_param
        self.name = name
        self.distribution = distribution
        self.distribution_params = distribution_params
        self.bounds = bounds
        self.attribute_value = attribute_value
        if self.attribute_value and self.distribution:
            raise ValueError("Only one of distribution or attribute value should be defined, not both")
        self.d_obj = self._dist_helper(distribution, distribution_params, bounds)

    def _dist_helper(self, distribution, dist_params, dist_bounds):
        # type: (...) -> Union[None, _FixDistribution]
        if distribution:
            try:
                d = _DIST_DICT[distribution](**dist_params)
                d.setLimits(dist_bounds) if dist_bounds else d.setLimits((0, None))
            except KeyError:
                raise ValueError("The distribution %s is not known. Please select one of: \n%s " %
                                 (distribution, "\n".join(_DIST_DICT.keys())))
        else:
            isNumeric = False if self.name == "emissionClass" else len(
                re.findall(r'^(-?[0-9]+(\.[0-9]+)?)$', self.attribute_value)) > 0
            d = _FixDistribution((self.attribute_value, ), isNumeric)
        return d

    def add_sampling_attempts(self, attempts):
        if self.d_obj:
            self.d_obj.setMaxSamplingAttempts(attempts)


class CreateVehTypeDistribution:

    def __init__(self, seed=None, size=100, name='vehDist', resampling=100, decimal_places=3):
        # type: (int, int, str, int, int) -> None
        """
        Creates a VehicleType Distribution.
        See https://sumo.dlr.de/docs/Definition_of_Vehicles,_Vehicle_Types,_and_Routes.html#vehicle_type_distributions

        Args:
            seed (int, optional): random seed.
            size (int, optional): number of vTypes in the distribution.
            name (str, optional): alphanumerical ID used for the created vehicle type distribution.
            resampling (int, optional): number of attempts to resample a value until it lies in the specified bounds.
            decimal_places (int, optional): number of decimal places.
        """
        if seed:
            random.seed(seed)

        self.size = size
        self.name = name
        self.resampling = resampling
        self.decimal_places = decimal_places
        self.attributes = []  # type: List[VehAttribute]

    def add_attribute(self, attribute):
        # type: (Union[VehAttribute, dict]) -> None
        """
        Add an instance of the attribute class to the Parameters. Pass the sampling attempts "global" parameter
        Args:
            attribute (VehAttribute or dict): An instance of VehAttribute or
             a dictionary of parameters to be passed to the VehAttribute constructor
        """
        attribute = attribute if isinstance(attribute, VehAttribute) else VehAttribute(**attribute)
        attribute.add_sampling_attempts(self.resampling)
        self.attributes.append(attribute)

    def create_veh_dist(self, xml_dom):
        # type: (xml.dom.minidom.Document) -> xml.dom.minidom.Element
        # create the vehicleDist node
        vtype_dist_node = xml_dom.createElement("vTypeDistribution")
        vtype_dist_node.setAttribute("id", self.name)

        # create the vehicle types
        for i in range(self.size):
            veh_type_node = xml_dom.createElement("vType")
            veh_type_node.setAttribute("id", self.name + str(i))
            self._generate_vehType(xml_dom, veh_type_node)
            vtype_dist_node.appendChild(veh_type_node)

        return vtype_dist_node

    def to_xml(self, file_path):
        # type: (str) -> None
        xml_dom, existing_file = self._check_existing(file_path)
        vtype_dist_node = self.create_veh_dist(xml_dom)
        if existing_file:
            self._handle_existing(xml_dom, vtype_dist_node)
            with open(file_path, 'w') as f:
                dom_string = xml_dom.toprettyxml()
                # super annoying but this makes re-writing the xml a little bit prettier
                f.write(os.linesep.join([s for s in dom_string.splitlines() if s.strip()]))
        else:
            write_additional_minidom(xml_dom, vtype_dist_node, file_path=file_path)
        sys.stdout.write("Output written to %s" % file_path)

    def _handle_existing(self, xml_dom, veh_dist_node):
        # type: (xml.dom.minidom.Document, xml.dom.minidom.Element) -> None
        existingDistNodes = xml_dom.getElementsByTagName("vTypeDistribution")
        replaceNode = None
        for existingDistNode in existingDistNodes:
            if existingDistNode.hasAttribute("id") and existingDistNode.getAttribute("id") == self.name:
                replaceNode = existingDistNode
                break
        if replaceNode is not None:
            replaceNode.parentNode.replaceChild(veh_dist_node, replaceNode)
        else:
            xml_dom.documentElement.appendChild(veh_dist_node)

    def _generate_vehType(self, xml_dom, veh_type_node):
        # type: (xml.dom.minidom.Document, xml.dom.minidom.Element) -> xml.dom.minidom.Node
        for attr in self.attributes:
            if attr.is_param:
                param_node = xml_dom.createElement("param")
                param_node.setAttribute("key", attr.name)
                param_node.setAttribute(
                    "value", attr.d_obj.sampleValueString(self.decimal_places))
                veh_type_node.appendChild(param_node)
            else:
                veh_type_node.setAttribute(
                    attr.name, attr.d_obj.sampleValueString(self.decimal_places))

    @staticmethod
    def _check_existing(file_path):
        # type: (str) -> Tuple[xml.dom.minidom.Document, bool]
        if os.path.exists(file_path):
            try:
                return xml.dom.minidom.parse(file_path), True
            except Exception as e:
                raise ValueError("Cannot parse existing %s. Error: %s" % (file_path, e))
        else:
            return xml.dom.minidom.Document(), False

    def save_myself(self, file_path):
        # type: (str) -> None
        """
        This function saves the class to a json format. Used for logging simulation inputs

        Args:
            file_path (str): path to save json to
        """
        import json

        with open(file_path, "w") as f:
            f.write(
                json.dumps(
                    self,
                    default=lambda o: {
                        key: param for key, param in o.__dict__.items() if '_' not in key[0]},
                    sort_keys=True,
                    indent=4,
                )
            )


class CreateMultiVehTypeDistributions(CreateVehTypeDistribution):

    def __init__(self):
        # type: () -> None
        self.distributions = []  # type: List[CreateVehTypeDistribution]

    def register_veh_type_distribution(self, veh_type_dist, veh_attributes):
        # type: (Union[dict, CreateVehTypeDistribution], List[Union[dict, VehAttribute]]) -> None
        veh_type_dist = veh_type_dist if isinstance(
            veh_type_dist, CreateVehTypeDistribution) else CreateVehTypeDistribution(**veh_type_dist)

        for attr in veh_attributes:
            veh_type_dist.add_attribute(attr if isinstance(attr, VehAttribute) else VehAttribute(**attr))

        self.distributions.append(veh_type_dist)

    def write_xml(self, file_path):
        # type: (str) -> None
        """
        This function will overwrite existing files

        Args:
            file_path (str): Path to the file to write to
        """
        xml_dom, _ = self._check_existing(file_path)
        veh_dist_nodes = [dist.create_veh_dist(xml_dom=xml_dom) for dist in self.distributions]
        write_additional_minidom(xml_dom, veh_dist_nodes, file_path=file_path)
