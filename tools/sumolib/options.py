# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2012-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    options.py
# @author  Jakob Erdmann
# @author  Michael Behrisch
# @date    2012-03-15


from __future__ import print_function
from __future__ import absolute_import
import os
import sys
import subprocess
from collections import namedtuple
import re
from xml.sax import parse, parseString, handler, saxutils
import optparse
import argparse
import io
from argparse import RawDescriptionHelpFormatter  # noqa
from copy import deepcopy
from functools import wraps
from .miscutils import openz, parseTime


class ConfigurationReader(handler.ContentHandler):

    """Reads a configuration template, storing the options in an OptionParser"""

    def __init__(self, optParse, groups, configoptions):
        self._opts = optParse
        self._groups = groups
        self._options = configoptions
        self._group = self._opts

    def startElement(self, name, attrs):
        if len(attrs) == 0:
            self._group = optparse.OptionGroup(self._opts, name)
        if self._group != self._opts and self._groups and self._group.title not in self._groups:
            return
        if 'type' in attrs and name != "help":
            if self._options and name not in self._options:
                return
            help = attrs.get("help", "")
            cat = attrs.get("category", "")
            option = optparse.Option("--" + name, help=help)
            if attrs["type"] == "BOOL":
                option = optparse.Option("--" + name, action="store_true", default=False, help=help, category=cat)
            elif attrs["type"] in ["FLOAT", "TIME"]:
                option.type = "float"
                if attrs["value"]:
                    option.default = float(attrs["value"])
            elif attrs["type"] == "INT":
                option.type = "int"
                if attrs["value"]:
                    option.default = int(attrs["value"])
            else:
                option.default = attrs["value"]
            self._group.add_option(option)

    def endElement(self, name):
        if self._group != self._opts and name == self._group.title:
            self._opts.add_option_group(self._group)
            self._group = self._opts


def pullOptions(executable, optParse, groups=None, configoptions=None):
    optoutput = subprocess.check_output([executable, "--save-template", "-"])
    parseString(optoutput, ConfigurationReader(optParse, groups, configoptions))


def get_long_option_names(application):
    # @todo using option "--save-template stdout" and parsing xml would be prettier
    output = subprocess.check_output([application, '--help'], universal_newlines=True)
    reprog = re.compile(r'(--\S*)\s')
    result = []
    for line in output.splitlines():
        m = reprog.search(line)
        if m:
            result.append(m.group(1))
    return result


def assign_prefixed_options(args, allowed_programs):
    prefixed_options = {}
    remaining = []
    consumed = False
    for arg_index, arg in enumerate(args):
        if consumed:
            consumed = False
            continue
        if arg[:2] == '--':
            separator_index = arg.find('-', 2)
            if separator_index != -1:
                program = arg[2:separator_index]
                if program in allowed_programs:
                    try:
                        if '--' in args[arg_index+1]:
                            raise ValueError()
                        option = [arg[separator_index+1:], args[arg_index+1]]
                    except (IndexError, ValueError):
                        raise ValueError("Please amend prefixed argument %s with a value." % arg)
                    prefixed_options.setdefault(program, []).append(option)
                    consumed = True
        if not consumed:
            remaining.append(arg)
    return prefixed_options, remaining


def get_prefixed_options(options):
    return options._prefixed_options


Option = namedtuple("Option", ["name", "value", "type", "help", "category"])


class OptionReader(handler.ContentHandler):

    """Reads an option file"""

    def __init__(self):
        self.opts = []

    def startElement(self, name, attrs):
        if 'value' in attrs:
            self.opts.append(Option(name, attrs['value'], attrs.get('type'), attrs.get('help'), attrs.get('category')))


def readOptions(filename):
    optionReader = OptionReader()
    parse(filename, optionReader)
    return optionReader.opts


def xmlescape(value):
    return saxutils.escape(str(value), {'"': '&quot;'})


class ArgumentParser(argparse.ArgumentParser):
    """Drop-in replacement for argparse.ArgumentParser that adds support for
    sumo-style config files.
    Inspired by https://github.com/bw2/ConfigArgParse
    """

    @staticmethod
    def time(s):
        return parseTime(s)

    @staticmethod
    def file(s):
        return s

    @staticmethod
    def file_list(s):
        return s

    @staticmethod
    def net_file(s):
        return s

    @staticmethod
    def route_file(s):
        return s

    @staticmethod
    def route_file_list(s):
        return s

    @staticmethod
    def additional_file(s):
        return s

    @staticmethod
    def edgedata_file(s):
        return s

    @staticmethod
    def edge(s):
        return s

    @staticmethod
    def edge_list(s):
        return s

    @staticmethod
    def data_file(s):
        # arbitrary data file (i.e. for attributeStats.py and plotXMLAttributes.py)
        return s

    @staticmethod
    def sumoconfig_file(s):
        return s

    @staticmethod
    def sumoconfig_file_list(s):
        return s

    def __init__(self, *args, **kwargs):
        self._allowed_programs = kwargs.get("allowed_programs", [])
        if "allowed_programs" in kwargs:
            del kwargs["allowed_programs"]
        self._catch_all = None
        argparse.ArgumentParser.__init__(self, *args, **kwargs)
        # add common argument for loading configuration
        self.add_argument('-c', '--configuration-file', help='read configuration from FILE', metavar="FILE")
        # add common argument for save configuration
        self.add_argument('-C', '--save-configuration', help='save configuration to FILE and exit', metavar="FILE")
        # add common argument for save template
        self.add_argument('--save-template', help='save configuration template to FILE and exit', metavar="FILE")
        self._fix_path_args = set()

    def add_argument(self, *args, **kwargs):
        # due argparse only accept certain values (action, choices, type, help...),
        #  we need to extract extra parameters before call add_argument
        fix_path = kwargs.get("fix_path")
        if "fix_path" in kwargs:
            del kwargs["fix_path"]
        # get category
        category = kwargs.get("category")
        if "category" in kwargs:
            del kwargs["category"]
        catch_all = kwargs.get("catch_all", False)
        if "catch_all" in kwargs:
            del kwargs["catch_all"]
        # get action
        action = kwargs.get("action")
        # parse argument
        a = argparse.ArgumentParser.add_argument(self, *args, **kwargs)
        # check if fix path
        if fix_path is True:
            for s in a.option_strings:
                if s.startswith("--"):
                    self._fix_path_args.add(s[2:])
        # set category
        a.category = category
        # set if a is a boolean
        a.boolean = ((action == "store_true") or (action == "store_false"))
        # the value of a.required is lost during parsing
        a.isRequired = a.required
        a.isPositional = args[0][0] != "-"
        if catch_all:
            self._catch_all = a

    def add_option(self, *args, **kwargs):
        """alias for compatibility with OptionParser"""
        self.add_argument(*args, **kwargs)

    def get_option(self, dest):
        for action in self._actions:
            if action.dest == dest:
                return action
        return None

    def add_mutually_exclusive_group(self, required=False):
        group = argparse.ArgumentParser.add_mutually_exclusive_group(self, required=required)
        group.add_argument = handleCategoryWrapper(group.add_argument)
        return group

    def _write_config_file(self, namespace, toString=False):
        if namespace.save_configuration:
            with openz(namespace.save_configuration, "w") as out:
                self.write_config_to_file(out, namespace, False)
            sys.exit()
        if namespace.save_template:
            with openz(namespace.save_template, "w") as out:
                self.write_config_to_file(out, namespace, True)
            sys.exit()
        if toString:
            out = io.StringIO()
            try:
                self.write_config_to_file(out, namespace, False)
            except Exception:
                # python2.7
                out = io.BytesIO()
                self.write_config_to_file(out, namespace, False)
            return out.getvalue()

    def write_config_to_file(self, out, namespace, print_template):
        out.write(u'<configuration>\n')
        optionNames = vars(namespace).keys()
        if sys.version_info.major < 3 or sys.version_info.minor < 6:
            optionNames = sorted(optionNames)
        for k in optionNames:
            v = vars(namespace)[k]
            if k not in ("save_configuration", "save_template", "configuration_file", "_parser", "_prefixed_options"):
                key = k
                help = ''
                typeStr = ''
                category = ''
                required = ''
                positional = ''
                listSep = ''
                for a in self._actions:
                    if a.dest == k:
                        for s in a.option_strings:
                            if s.startswith("--"):
                                key = s[2:]
                                break
                        if print_template:
                            # default
                            if a.default is not None:
                                v = a.default
                            # help
                            if a.help is not None:
                                help = ' help="%s"' % xmlescape(a.help)

                            # note: missing time, filename, list of vehicles, edges and lanes
                            # category
                            category = ' category="%s"' % (a.category if a.category is not None else 'processing')
                            if a.boolean:
                                typeName = "bool"
                            elif a.type is None:
                                typeName = "string"
                            else:
                                typeName = a.type.__name__
                                if typeName == 'parseTime':
                                    typeName = 'time'
                                knownTypes = ['bool', 'float', 'int', 'time', 'file',
                                              'net_file', 'route_file', 'additional_file',
                                              'edgedata_file', 'data_file', 'file_list',
                                              'route_file_list', 'sumoconfig_file',
                                              'sumoconfig_file_list', 'edge', 'edge_list']
                                if typeName not in knownTypes:
                                    typeName = 'string'
                                elif typeName.endswith("_list"):
                                    typeName = typeName[:-5]
                                    listSep = ' listSeparator=","'
                            typeStr = ' type="%s"' % typeName
                            if a.isRequired:
                                required = ' required="true"'
                            if a.isPositional:
                                positional = ' positional="true"'
                            if a.nargs:
                                listSep = ' listSeparator=" "'

                        break
                if print_template or v != a.default:
                    if isinstance(v, list):
                        v = " ".join(map(str, v))
                    out.write(u'    <%s value="%s"%s%s%s%s%s%s/>\n' % (
                              key, xmlescape(v), typeStr, help, category,
                              required, positional, listSep))
        out.write(u'</configuration>\n')

    def parse_args(self, args=None, namespace=None):
        return self.parse_known_args(args, namespace, True)[0]

    def parse_known_args(self, args=None, namespace=None, check_unknown=False):
        if args is None:
            args = sys.argv[1:]
        elif isinstance(args, str):
            args = args.split()
        else:
            # gracefully handle non-string args passed from another script
            args = list(map(str, args))
        idx = -1
        if '-c' in args:
            idx = args.index('-c') + 1
        if '--configuration-file' in args:
            idx = args.index('--configuration-file') + 1
        if '--save-template' in args:
            for a in self._actions:
                a.required = False
            for g in self._mutually_exclusive_groups:
                g.required = False

        # add each config item to the commandline unless it's there already
        config_args = []
        pos_args = []
        if idx > 0:
            act_map = {}
            pos_map = {}
            multi_value = set()
            pos_idx = 0
            for a in self._actions:
                for s in a.option_strings:
                    if s.startswith("--"):
                        act_map[s[2:]] = a.option_strings
                        if a.nargs:
                            multi_value.add(s[2:])
                if len(a.option_strings) == 0:
                    pos_map[a.dest] = pos_idx
                    pos_args.append(None)
                    pos_idx += 1
            for cfg_file in args[idx].split(","):
                for option in readOptions(cfg_file):
                    is_set = False
                    for s in act_map.get(option.name, []):
                        if s in args:
                            is_set = True
                            break
                    value = option.value
                    if option.name in self._fix_path_args and not value.startswith("http"):
                        value = os.path.join(os.path.dirname(cfg_file), value)
                    if option.name in pos_map and option.name != 'remaining_args':
                        pos_args[pos_map[option.name]] = value
                    elif not is_set:
                        if value == "True":
                            config_args += ["--" + option.name]
                        elif value != "False":
                            if option.name == 'remaining_args':
                                # special case: magic option name to collect remaining arguments
                                config_args += value.split()
                            elif option.name in multi_value:
                                config_args += ["--" + option.name] + value.split()
                            else:
                                # permit negative values and empty strings in cfg files
                                config_args += ["--" + option.name + "=" + value]
        combined_args = args + config_args + [p for p in pos_args if p is not None]
        namespace, unknown_args = argparse.ArgumentParser.parse_known_args(
            self, args=combined_args, namespace=namespace)

        if self._allowed_programs and unknown_args and hasattr(namespace, "remaining_args"):
            # namespace.remaining_args are the legacy method to parse arguments
            # for subprograms (i.e. 'duarouter--weights.random-factor 2')
            # unknown_args are the new style # # ('--duarouter-weights.random-factor 2')
            # the default ArgumentParser interprets the first parameter for an
            # unknown argument as remaining_args and this also creates an
            # invalid error message when unknown options are parsed
            unknown_args.insert(1, namespace.remaining_args[0])
            namespace.remaining_args = []

        # print("parse_known_args:\n  args: %s\n  config_args: %s\n  pos_args: %s\n  "
        #       "combined_args: %s\n  remaining_args: %s\n  unknown_args: %s" %
        #       (args, config_args, pos_args, combined_args, namespace.remaining_args, unknown_args))

        namespace_as_dict = deepcopy(vars(namespace))
        namespace._prefixed_options, remaining_args = assign_prefixed_options(unknown_args, self._allowed_programs)

        for program in namespace._prefixed_options:
            prefixed_options = deepcopy(namespace._prefixed_options[program])
            for option in prefixed_options:
                option[0] = program + '-' + option[0]
            namespace_as_dict.update(dict(prefixed_options))

        if check_unknown and remaining_args:
            if self._catch_all:
                setattr(namespace, self._catch_all.dest,
                        getattr(namespace, self._catch_all.dest) + remaining_args)
            else:
                self.error('unrecognized arguments: %s' % ' '.join(remaining_args))

        extended_namespace = argparse.Namespace(**namespace_as_dict)
        self._write_config_file(extended_namespace)
        namespace.config_as_string = self._write_config_file(extended_namespace, toString=True)
        return namespace, remaining_args


def handleCategoryWrapper(func):
    @wraps(func)
    def inner(*args, **kwargs):
        category = kwargs.get("category")
        # remove category from arguments and set in result
        if "category" in kwargs:
            del kwargs["category"]
        result = func(*args, **kwargs)
        result.category = category
        # set if is a boolean
        action = kwargs.get("action")
        result.boolean = ((action == "store_true") or (action == "store_false"))
        result.isRequired = kwargs.get("required", False)
        result.isPositional = args[0][0] != "-"
        return result
    return inner


class SplitAction(argparse.Action):
    def __call__(self, parser, args, values, option_string=None):
        if len(values) == 1:
            values = [float(x) for x in values[0].split(',')]
        else:
            values = [float(x) for x in values]
        setattr(args, self.dest, values)
