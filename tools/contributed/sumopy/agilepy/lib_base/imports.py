# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    imports.py
# @author  Joerg Schweizer
# @date    2012


import os
import sys
import types
import numpy as np
import agilepy.lib_base.classman as cm
import agilepy.lib_base.arrayman as am

from agilepy.lib_base.processes import Process


class CsvImporter(Process):
    def __init__(self,  obj, ident='csvimporter', name='CSV importer',
                 info='Import data from a CSV file into object',
                 logger=None, **kwargs):
        print 'CsvImporter.__init__'
        self._init_common(ident,
                          parent=obj,
                          name=name,
                          logger=logger,
                          info=info,
                          )

        attrsman = self.set_attrsman(cm.Attrsman(self))
        self.csvfilepath = attrsman.add(cm.AttrConf('csvfilepath', kwargs.get('csvfilepath', ''),
                                                    groupnames=['options'],
                                                    perm='rw',
                                                    name='CSV file',
                                                    wildcards='CSV file (*.csv)|*.csv|*.CSV',
                                                    metatype='filepath',
                                                    info="CSV plain text file path.",
                                                    ))

        self.sep = attrsman.add(cm.AttrConf('sep', kwargs.get('sep', ","),
                                            groupnames=['options', ],
                                            perm='rw',
                                            name='Separator',
                                            info="""Seperator used in SCV file. Exampe: ; , <space key>""",
                                            ))

        self.is_use_default_for_invalid = attrsman.add(cm.AttrConf('is_use_default_for_invalid', kwargs.get('is_use_default_for_invalid', True),
                                                                   groupnames=['options', ],
                                                                   perm='rw',
                                                                   name='Use default for invalid',
                                                                   info="""Use default for invalid.""",
                                                                   ))

        for attrconf in obj.get_colconfigs():
            colattrname = self._get_colattrname(attrconf.attrname)
            setattr(self, colattrname,
                    attrsman.add(cm.AttrConf(colattrname, attrconf.attrname,
                                             groupnames=['options', 'colnames'],
                                             perm='rw',
                                             attrname_orig=attrconf.attrname,
                                             name='Column name for %s' % attrconf.get_name(),
                                             info='Name of column in CSV file for column %s. Include also hyphens and other delimiters when present in CSV file.' % attrconf.get_name(),
                                             )))

    def _get_colattrname(self, attrname):
        return 'colname_'+attrname

    def do(self):
        print 'CsvImporter.do',
        obj = self.parent
        attrsman = self.get_attrsman()
        sep = self.sep
        logger = self._logger

        aa = '\"'
        emptycol = ("", "None", "NONE")
        INTTYPE = 'i'
        FLOATTYPE = 'f'
        STRINGTYPE = 's'
        BOOLTYPE = 'b'
        is_use_nan = not self.is_use_default_for_invalid

        #INTLISTTYPE = 'li'
        #FLOATLISTTYPE = 'lf'
        #STRINGLISTTYPE = 'ls'
        filepath = self.csvfilepath
        f = open(filepath, 'r')

        INTTYPES = (types.IntType, np.int,  np.int32,  np.int64)
        FLOATTYPES = (types.FloatType, types.LongType, types.ComplexType, np.float,  np.float32,  np.float64)
        BOOLTYPES = (types.BooleanType, np.bool_)

        #line = f.readline()
        # print '  line[:-1] = *%s*'%(line[:-1],)
        index_to_value = {}
        index_to_type = {}
        index_to_attrname = {}
        i = 0
        for csvattr_raw in f.readline().split(sep):

            csvattr = self._get_colattrname(csvattr_raw.strip().strip(aa))
            # print '  check csvattr *%s*, %d'%(csvattr,i),hasattr(self, csvattr)

            if hasattr(self, csvattr):
                colnameconf = attrsman.get_config(csvattr)
                index_to_attrname[i] = colnameconf.attrname_orig
                index_to_value[i] = []
                attrconfig = obj.get_config(colnameconf.attrname_orig)
                defaulttype = type(attrconfig.get_default())
                # print '   defaulttype',defaulttype
                if defaulttype in INTTYPES:
                    index_to_type[i] = INTTYPE
                elif defaulttype in FLOATTYPES:
                    index_to_type[i] = FLOATTYPE
                elif defaulttype in BOOLTYPES:
                    index_to_type[i] = BOOLTYPE
                else:
                    index_to_type[i] = STRINGTYPE
            i += 1

        if len(index_to_type) == 0:
            if logger:
                logger.w('file %s does not contain valid column names.' % os.path.basename(filepath))
            return False

        # print 'index_to_attrname',index_to_attrname
        # print 'index_to_value',index_to_value
        # print 'index_to_type',index_to_type
        #n_cols = len(attrsinds)
        if logger:
            logger.w('import CSV from %s' % os.path.basename(filepath))
        n_imported = 0
        ind_max = max(index_to_value.keys())
        for line in f.readlines():
            cols = line.split(sep)
            if len(cols) > ind_max:  # restrictive!
                for ind, vals, valtype in zip(index_to_value.keys(), index_to_value.values(), index_to_type.values()):
                    val = cols[ind].strip()
                    if val not in emptycol:
                        if valtype == INTTYPE:
                            vals.append(int(val))
                        elif valtype == FLOATTYPE:
                            vals.append(float(val))
                        elif valtype == BOOLTYPE:
                            if val in TRUEVALS:
                                vals.append(True)
                            else:
                                vals.append(False)
                        else:  # type == STRINGTYPE:
                            vals.append(val.strip(aa))

                    elif is_use_nan:
                        if valtype == INTTYPE:
                            vals.append(np.nan)
                        elif valtype == FLOATTYPE:
                            vals.append(np.nan)
                        elif valtype == BOOLTYPE:
                            vals.append(np.nan)
                        else:  # type == STRINGTYPE:
                            vals.append("")
                n_imported += 1

        ids = obj.add_rows(n_imported)
        for ind, values in index_to_value.iteritems():
            getattr(obj, index_to_attrname[ind])[ids] = values

        f.close()
        return True
